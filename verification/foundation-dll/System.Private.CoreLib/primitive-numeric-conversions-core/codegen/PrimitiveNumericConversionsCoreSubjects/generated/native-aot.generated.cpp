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

namespace chaos::il2cpp::codegen::PrimitiveNumericConversionsCoreSubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


inline TypeInfoV0 chaos_mt_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects = {{nullptr, nullptr, 3329548187654970984ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(3329548187654970984ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Object = {{nullptr, nullptr, 15228727185366376748ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_INTPTR>(15228727185366376748ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_Assembly = {{nullptr, nullptr, 5474029880995115448ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
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
	{ reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // PrimitiveNumericConversionsCoreSubjects::Subject_0
	{ reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // PrimitiveNumericConversionsCoreSubjects::Subject_1
	{ reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // PrimitiveNumericConversionsCoreSubjects::Subject_2
	{ reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // PrimitiveNumericConversionsCoreSubjects::Subject_3
	{ reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // PrimitiveNumericConversionsCoreSubjects::Subject_4
	{ reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // PrimitiveNumericConversionsCoreSubjects::Subject_5
	{ reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // PrimitiveNumericConversionsCoreSubjects::Subject_6
	{ reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // PrimitiveNumericConversionsCoreSubjects::Subject_7
	{ reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // PrimitiveNumericConversionsCoreSubjects::Subject_8
	{ reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_9), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // PrimitiveNumericConversionsCoreSubjects::Subject_9
	{ reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestByteToInt), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // PrimitiveNumericConversionsCoreNativeEntry::TestByteToInt
	{ reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestDoubleToFloat), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // PrimitiveNumericConversionsCoreNativeEntry::TestDoubleToFloat
	{ reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestDoubleToInt), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // PrimitiveNumericConversionsCoreNativeEntry::TestDoubleToInt
	{ reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestFloatToDouble), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // PrimitiveNumericConversionsCoreNativeEntry::TestFloatToDouble
	{ reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToByte), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // PrimitiveNumericConversionsCoreNativeEntry::TestIntToByte
	{ reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToDouble), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // PrimitiveNumericConversionsCoreNativeEntry::TestIntToDouble
	{ reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToLong), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // PrimitiveNumericConversionsCoreNativeEntry::TestIntToLong
	{ reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToShort), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // PrimitiveNumericConversionsCoreNativeEntry::TestIntToShort
	{ reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestLongToInt), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // PrimitiveNumericConversionsCoreNativeEntry::TestLongToInt
	{ reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestUintToLong), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // PrimitiveNumericConversionsCoreNativeEntry::TestUintToLong
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
static void (*kBenchmarkWrappers[20])() = {
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


#if !defined(CHAOS_IL2CPP_EH_SETJMP) && !defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	try
	{
	}
	catch (const chaos_managed_exception& chaos_exception)
	{
		if (chaos_exception.object_value < 0) { throw; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_exception.object_value);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{
				throw;
			}
		}
		_s0 = chaos_exception.object_value;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
			}
	}
#elif defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	__try
	{
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
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
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
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
			}
	}
#else
	{
		auto* _chaos_jmp =
			chaos::il2cpp::runtime_core::push_exception_jmp_buf();
		if (setjmp(*_chaos_jmp) == 0)
		{
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
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
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
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
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


#if !defined(CHAOS_IL2CPP_EH_SETJMP) && !defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	try
	{
	}
	catch (const chaos_managed_exception& chaos_exception)
	{
		if (chaos_exception.object_value < 0) { throw; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_exception.object_value);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{
				throw;
			}
		}
		_s0 = chaos_exception.object_value;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
			}
	}
#elif defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	__try
	{
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
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
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
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
			}
	}
#else
	{
		auto* _chaos_jmp =
			chaos::il2cpp::runtime_core::push_exception_jmp_buf();
		if (setjmp(*_chaos_jmp) == 0)
		{
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
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
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
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
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


#if !defined(CHAOS_IL2CPP_EH_SETJMP) && !defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	try
	{
	}
	catch (const chaos_managed_exception& chaos_exception)
	{
		if (chaos_exception.object_value < 0) { throw; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_exception.object_value);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{
				throw;
			}
		}
		_s0 = chaos_exception.object_value;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
			}
	}
#elif defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	__try
	{
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
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
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
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
			}
	}
#else
	{
		auto* _chaos_jmp =
			chaos::il2cpp::runtime_core::push_exception_jmp_buf();
		if (setjmp(*_chaos_jmp) == 0)
		{
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
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
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
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
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


#if !defined(CHAOS_IL2CPP_EH_SETJMP) && !defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	try
	{
	}
	catch (const chaos_managed_exception& chaos_exception)
	{
		if (chaos_exception.object_value < 0) { throw; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_exception.object_value);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{
				throw;
			}
		}
		_s0 = chaos_exception.object_value;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
			}
	}
#elif defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	__try
	{
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
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
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
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
			}
	}
#else
	{
		auto* _chaos_jmp =
			chaos::il2cpp::runtime_core::push_exception_jmp_buf();
		if (setjmp(*_chaos_jmp) == 0)
		{
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
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
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
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
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


#if !defined(CHAOS_IL2CPP_EH_SETJMP) && !defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	try
	{
	}
	catch (const chaos_managed_exception& chaos_exception)
	{
		if (chaos_exception.object_value < 0) { throw; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_exception.object_value);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{
				throw;
			}
		}
		_s0 = chaos_exception.object_value;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
			}
	}
#elif defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	__try
	{
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
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
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
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
			}
	}
#else
	{
		auto* _chaos_jmp =
			chaos::il2cpp::runtime_core::push_exception_jmp_buf();
		if (setjmp(*_chaos_jmp) == 0)
		{
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
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
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
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
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


#if !defined(CHAOS_IL2CPP_EH_SETJMP) && !defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	try
	{
	}
	catch (const chaos_managed_exception& chaos_exception)
	{
		if (chaos_exception.object_value < 0) { throw; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_exception.object_value);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{
				throw;
			}
		}
		_s0 = chaos_exception.object_value;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
			}
	}
#elif defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	__try
	{
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
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
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
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
			}
	}
#else
	{
		auto* _chaos_jmp =
			chaos::il2cpp::runtime_core::push_exception_jmp_buf();
		if (setjmp(*_chaos_jmp) == 0)
		{
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
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
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
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
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


#if !defined(CHAOS_IL2CPP_EH_SETJMP) && !defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	try
	{
	}
	catch (const chaos_managed_exception& chaos_exception)
	{
		if (chaos_exception.object_value < 0) { throw; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_exception.object_value);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{
				throw;
			}
		}
		_s0 = chaos_exception.object_value;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
			}
	}
#elif defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	__try
	{
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
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
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
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
			}
	}
#else
	{
		auto* _chaos_jmp =
			chaos::il2cpp::runtime_core::push_exception_jmp_buf();
		if (setjmp(*_chaos_jmp) == 0)
		{
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
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
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
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
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


#if !defined(CHAOS_IL2CPP_EH_SETJMP) && !defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	try
	{
	}
	catch (const chaos_managed_exception& chaos_exception)
	{
		if (chaos_exception.object_value < 0) { throw; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_exception.object_value);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{
				throw;
			}
		}
		_s0 = chaos_exception.object_value;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
			}
	}
#elif defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	__try
	{
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
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
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
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
			}
	}
#else
	{
		auto* _chaos_jmp =
			chaos::il2cpp::runtime_core::push_exception_jmp_buf();
		if (setjmp(*_chaos_jmp) == 0)
		{
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
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
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
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
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


#if !defined(CHAOS_IL2CPP_EH_SETJMP) && !defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	try
	{
	}
	catch (const chaos_managed_exception& chaos_exception)
	{
		if (chaos_exception.object_value < 0) { throw; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_exception.object_value);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{
				throw;
			}
		}
		_s0 = chaos_exception.object_value;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
			}
	}
#elif defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	__try
	{
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
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
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
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
			}
	}
#else
	{
		auto* _chaos_jmp =
			chaos::il2cpp::runtime_core::push_exception_jmp_buf();
		if (setjmp(*_chaos_jmp) == 0)
		{
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
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
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
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
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


#if !defined(CHAOS_IL2CPP_EH_SETJMP) && !defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	try
	{
	}
	catch (const chaos_managed_exception& chaos_exception)
	{
		if (chaos_exception.object_value < 0) { throw; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_exception.object_value);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{
				throw;
			}
		}
		_s0 = chaos_exception.object_value;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
			}
	}
#elif defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	__try
	{
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
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
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
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
			}
	}
#else
	{
		auto* _chaos_jmp =
			chaos::il2cpp::runtime_core::push_exception_jmp_buf();
		if (setjmp(*_chaos_jmp) == 0)
		{
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
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
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
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
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
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		chaos_locals[4] = _s0;
		_s0 = chaos_locals[4];
		return static_cast<CHAOS_IL2CPP_INT32>(_s0);
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
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
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
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		chaos_locals[3] = _s0;
		_s0 = chaos_locals[3];
		return static_cast<CHAOS_IL2CPP_INT32>(_s0);
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
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
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
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		chaos_locals[3] = _s0;
		_s0 = chaos_locals[3];
		return static_cast<CHAOS_IL2CPP_INT32>(_s0);
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
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		chaos_locals[2] = _s0;
		_s0 = chaos_locals[2];
		return static_cast<CHAOS_IL2CPP_INT32>(_s0);
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
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
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
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
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
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		chaos_locals[2] = _s0;
		_s0 = chaos_locals[2];
		return static_cast<CHAOS_IL2CPP_INT32>(_s0);
	}
}



}  // namespace chaos::il2cpp::codegen::PrimitiveNumericConversionsCoreSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 20;

// ── JIT Method Entry Table ───────────────────────────────────────────
// Auto-generated by chaos-il2cpp codegen for --mode jit.
#include <cstdint>
#include "jit_registration.h"

static const char kMethodJson_0[] = "{\"methodId\":\"primitive-numeric-conversions-core-subjects.primitive-numeric-conversions-core-subjects.subject-0\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_0:System.Void()\",\"signature\":\"System.Void PrimitiveNumericConversionsCoreSubjects::Subject_0()\",\"identity\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"declaringTypeSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects\",\"definitionSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_0:System.Void()\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_0:System.Void()\",\"methodId\":\"primitive-numeric-conversions-core-subjects.primitive-numeric-conversions-core-subjects.subject-0\",\"signature\":\"System.Void PrimitiveNumericConversionsCoreSubjects::Subject_0()\"},\"nativeSymbol\":\"PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":29,\"handlerOffset\":30,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"call\",\"opCode\":14,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"callee\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestByteToInt:System.Int32()\",\"reference\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestByteToInt:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestByteToInt:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestByteToInt\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"callee\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestByteToInt:System.Int32()\",\"reference\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestByteToInt:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestByteToInt:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestByteToInt\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":17,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":27,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\",\"ilOffset\":22,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\",\"ilOffset\":33,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":41,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"PrimitiveNumericConversionsCoreSubjects\"}";
static const char kMethodJson_1[] = "{\"methodId\":\"primitive-numeric-conversions-core-subjects.primitive-numeric-conversions-core-subjects.subject-1\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_1:System.Void()\",\"signature\":\"System.Void PrimitiveNumericConversionsCoreSubjects::Subject_1()\",\"identity\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"declaringTypeSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects\",\"definitionSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_1:System.Void()\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_1:System.Void()\",\"methodId\":\"primitive-numeric-conversions-core-subjects.primitive-numeric-conversions-core-subjects.subject-1\",\"signature\":\"System.Void PrimitiveNumericConversionsCoreSubjects::Subject_1()\"},\"nativeSymbol\":\"PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":29,\"handlerOffset\":30,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"call\",\"opCode\":14,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"callee\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestIntToLong:System.Int32()\",\"reference\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestIntToLong:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestIntToLong:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToLong\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"callee\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestIntToLong:System.Int32()\",\"reference\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestIntToLong:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestIntToLong:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToLong\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":17,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":27,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\",\"ilOffset\":22,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\",\"ilOffset\":33,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":41,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"PrimitiveNumericConversionsCoreSubjects\"}";
static const char kMethodJson_2[] = "{\"methodId\":\"primitive-numeric-conversions-core-subjects.primitive-numeric-conversions-core-subjects.subject-2\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_2:System.Void()\",\"signature\":\"System.Void PrimitiveNumericConversionsCoreSubjects::Subject_2()\",\"identity\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"declaringTypeSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects\",\"definitionSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_2:System.Void()\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_2:System.Void()\",\"methodId\":\"primitive-numeric-conversions-core-subjects.primitive-numeric-conversions-core-subjects.subject-2\",\"signature\":\"System.Void PrimitiveNumericConversionsCoreSubjects::Subject_2()\"},\"nativeSymbol\":\"PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_2\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":29,\"handlerOffset\":30,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"call\",\"opCode\":14,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"callee\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestIntToShort:System.Int32()\",\"reference\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestIntToShort:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestIntToShort:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToShort\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"callee\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestIntToShort:System.Int32()\",\"reference\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestIntToShort:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestIntToShort:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToShort\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":17,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":27,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\",\"ilOffset\":22,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\",\"ilOffset\":33,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":41,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"PrimitiveNumericConversionsCoreSubjects\"}";
static const char kMethodJson_3[] = "{\"methodId\":\"primitive-numeric-conversions-core-subjects.primitive-numeric-conversions-core-subjects.subject-3\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_3:System.Void()\",\"signature\":\"System.Void PrimitiveNumericConversionsCoreSubjects::Subject_3()\",\"identity\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"declaringTypeSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects\",\"definitionSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_3:System.Void()\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_3:System.Void()\",\"methodId\":\"primitive-numeric-conversions-core-subjects.primitive-numeric-conversions-core-subjects.subject-3\",\"signature\":\"System.Void PrimitiveNumericConversionsCoreSubjects::Subject_3()\"},\"nativeSymbol\":\"PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_3\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":29,\"handlerOffset\":30,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"call\",\"opCode\":14,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"callee\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestIntToByte:System.Int32()\",\"reference\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestIntToByte:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestIntToByte:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToByte\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"callee\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestIntToByte:System.Int32()\",\"reference\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestIntToByte:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestIntToByte:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToByte\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":17,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":27,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\",\"ilOffset\":22,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\",\"ilOffset\":33,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":41,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"PrimitiveNumericConversionsCoreSubjects\"}";
static const char kMethodJson_4[] = "{\"methodId\":\"primitive-numeric-conversions-core-subjects.primitive-numeric-conversions-core-subjects.subject-4\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_4:System.Void()\",\"signature\":\"System.Void PrimitiveNumericConversionsCoreSubjects::Subject_4()\",\"identity\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"declaringTypeSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects\",\"definitionSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_4:System.Void()\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_4:System.Void()\",\"methodId\":\"primitive-numeric-conversions-core-subjects.primitive-numeric-conversions-core-subjects.subject-4\",\"signature\":\"System.Void PrimitiveNumericConversionsCoreSubjects::Subject_4()\"},\"nativeSymbol\":\"PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_4\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":29,\"handlerOffset\":30,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"call\",\"opCode\":14,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"callee\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestFloatToDouble:System.Int32()\",\"reference\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestFloatToDouble:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestFloatToDouble:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestFloatToDouble\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"callee\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestFloatToDouble:System.Int32()\",\"reference\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestFloatToDouble:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestFloatToDouble:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestFloatToDouble\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":17,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":27,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\",\"ilOffset\":22,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\",\"ilOffset\":33,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":41,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"PrimitiveNumericConversionsCoreSubjects\"}";
static const char kMethodJson_5[] = "{\"methodId\":\"primitive-numeric-conversions-core-subjects.primitive-numeric-conversions-core-subjects.subject-5\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_5:System.Void()\",\"signature\":\"System.Void PrimitiveNumericConversionsCoreSubjects::Subject_5()\",\"identity\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"declaringTypeSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects\",\"definitionSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_5:System.Void()\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_5:System.Void()\",\"methodId\":\"primitive-numeric-conversions-core-subjects.primitive-numeric-conversions-core-subjects.subject-5\",\"signature\":\"System.Void PrimitiveNumericConversionsCoreSubjects::Subject_5()\"},\"nativeSymbol\":\"PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_5\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":29,\"handlerOffset\":30,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"call\",\"opCode\":14,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"callee\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestDoubleToFloat:System.Int32()\",\"reference\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestDoubleToFloat:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestDoubleToFloat:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestDoubleToFloat\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"callee\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestDoubleToFloat:System.Int32()\",\"reference\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestDoubleToFloat:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestDoubleToFloat:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestDoubleToFloat\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":17,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":27,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\",\"ilOffset\":22,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\",\"ilOffset\":33,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":41,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"PrimitiveNumericConversionsCoreSubjects\"}";
static const char kMethodJson_6[] = "{\"methodId\":\"primitive-numeric-conversions-core-subjects.primitive-numeric-conversions-core-subjects.subject-6\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_6:System.Void()\",\"signature\":\"System.Void PrimitiveNumericConversionsCoreSubjects::Subject_6()\",\"identity\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"declaringTypeSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects\",\"definitionSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_6:System.Void()\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_6:System.Void()\",\"methodId\":\"primitive-numeric-conversions-core-subjects.primitive-numeric-conversions-core-subjects.subject-6\",\"signature\":\"System.Void PrimitiveNumericConversionsCoreSubjects::Subject_6()\"},\"nativeSymbol\":\"PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_6\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":29,\"handlerOffset\":30,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"call\",\"opCode\":14,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"callee\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestIntToDouble:System.Int32()\",\"reference\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestIntToDouble:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestIntToDouble:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToDouble\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"callee\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestIntToDouble:System.Int32()\",\"reference\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestIntToDouble:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestIntToDouble:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToDouble\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":17,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":27,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\",\"ilOffset\":22,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\",\"ilOffset\":33,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":41,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"PrimitiveNumericConversionsCoreSubjects\"}";
static const char kMethodJson_7[] = "{\"methodId\":\"primitive-numeric-conversions-core-subjects.primitive-numeric-conversions-core-subjects.subject-7\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_7:System.Void()\",\"signature\":\"System.Void PrimitiveNumericConversionsCoreSubjects::Subject_7()\",\"identity\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"declaringTypeSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects\",\"definitionSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_7:System.Void()\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_7:System.Void()\",\"methodId\":\"primitive-numeric-conversions-core-subjects.primitive-numeric-conversions-core-subjects.subject-7\",\"signature\":\"System.Void PrimitiveNumericConversionsCoreSubjects::Subject_7()\"},\"nativeSymbol\":\"PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_7\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":29,\"handlerOffset\":30,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"call\",\"opCode\":14,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"callee\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestDoubleToInt:System.Int32()\",\"reference\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestDoubleToInt:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestDoubleToInt:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestDoubleToInt\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"callee\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestDoubleToInt:System.Int32()\",\"reference\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestDoubleToInt:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestDoubleToInt:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestDoubleToInt\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":17,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":27,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\",\"ilOffset\":22,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\",\"ilOffset\":33,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":41,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"PrimitiveNumericConversionsCoreSubjects\"}";
static const char kMethodJson_8[] = "{\"methodId\":\"primitive-numeric-conversions-core-subjects.primitive-numeric-conversions-core-subjects.subject-8\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_8:System.Void()\",\"signature\":\"System.Void PrimitiveNumericConversionsCoreSubjects::Subject_8()\",\"identity\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"declaringTypeSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects\",\"definitionSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_8:System.Void()\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_8:System.Void()\",\"methodId\":\"primitive-numeric-conversions-core-subjects.primitive-numeric-conversions-core-subjects.subject-8\",\"signature\":\"System.Void PrimitiveNumericConversionsCoreSubjects::Subject_8()\"},\"nativeSymbol\":\"PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_8\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":29,\"handlerOffset\":30,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"call\",\"opCode\":14,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"callee\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestUintToLong:System.Int32()\",\"reference\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestUintToLong:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestUintToLong:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestUintToLong\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"callee\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestUintToLong:System.Int32()\",\"reference\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestUintToLong:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestUintToLong:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestUintToLong\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":17,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":27,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\",\"ilOffset\":22,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\",\"ilOffset\":33,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":41,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"PrimitiveNumericConversionsCoreSubjects\"}";
static const char kMethodJson_9[] = "{\"methodId\":\"primitive-numeric-conversions-core-subjects.primitive-numeric-conversions-core-subjects.subject-9\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_9:System.Void()\",\"signature\":\"System.Void PrimitiveNumericConversionsCoreSubjects::Subject_9()\",\"identity\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"declaringTypeSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects\",\"definitionSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_9:System.Void()\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_9:System.Void()\",\"methodId\":\"primitive-numeric-conversions-core-subjects.primitive-numeric-conversions-core-subjects.subject-9\",\"signature\":\"System.Void PrimitiveNumericConversionsCoreSubjects::Subject_9()\"},\"nativeSymbol\":\"PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_9\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":29,\"handlerOffset\":30,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"call\",\"opCode\":14,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"callee\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestLongToInt:System.Int32()\",\"reference\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestLongToInt:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestLongToInt:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestLongToInt\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"callee\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestLongToInt:System.Int32()\",\"reference\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestLongToInt:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestLongToInt:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestLongToInt\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":17,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":27,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\",\"ilOffset\":22,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\",\"ilOffset\":33,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":41,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"PrimitiveNumericConversionsCoreSubjects\"}";
static const char kMethodJson_10[] = "{\"methodId\":\"primitive-numeric-conversions-core-subjects.primitive-numeric-conversions-core-native-entry.test-byte-to-int\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestByteToInt:System.Int32()\",\"signature\":\"System.Int32 PrimitiveNumericConversionsCoreNativeEntry::TestByteToInt()\",\"identity\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"declaringTypeSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry\",\"definitionSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestByteToInt:System.Int32()\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestByteToInt:System.Int32()\",\"methodId\":\"primitive-numeric-conversions-core-subjects.primitive-numeric-conversions-core-native-entry.test-byte-to-int\",\"signature\":\"System.Int32 PrimitiveNumericConversionsCoreNativeEntry::TestByteToInt()\"},\"nativeSymbol\":\"PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestByteToInt\",\"isStatic\":true,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":3,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":200,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":6,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":8,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":9,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":200,\"ilOffset\":10,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"bne.un\",\"opCode\":79,\"operand\":20,\"ilOffset\":15,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":21,\"ilOffset\":18,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":20,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":24,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":24,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":25,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"PrimitiveNumericConversionsCoreSubjects\"}";
static const char kMethodJson_11[] = "{\"methodId\":\"primitive-numeric-conversions-core-subjects.primitive-numeric-conversions-core-native-entry.test-double-to-float\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestDoubleToFloat:System.Int32()\",\"signature\":\"System.Int32 PrimitiveNumericConversionsCoreNativeEntry::TestDoubleToFloat()\",\"identity\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"declaringTypeSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry\",\"definitionSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestDoubleToFloat:System.Int32()\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestDoubleToFloat:System.Int32()\",\"methodId\":\"primitive-numeric-conversions-core-subjects.primitive-numeric-conversions-core-native-entry.test-double-to-float\",\"signature\":\"System.Int32 PrimitiveNumericConversionsCoreNativeEntry::TestDoubleToFloat()\"},\"nativeSymbol\":\"PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestDoubleToFloat\",\"isStatic\":true,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":5,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.r8\",\"opCode\":3,\"operand\":2.71828,\"ilOffset\":1,\"resultType\":\"System.Double\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":10,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":11,\"isPreserveSig\":false},{\"op\":\"conv.r4\",\"opCode\":41,\"ilOffset\":12,\"resultType\":\"System.Single\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":14,\"isPreserveSig\":false},{\"op\":\"conv.r8\",\"opCode\":42,\"ilOffset\":15,\"resultType\":\"System.Double\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":17,\"isPreserveSig\":false},{\"op\":\"conv.r4\",\"opCode\":41,\"ilOffset\":18,\"resultType\":\"System.Single\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":3,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":3,\"ilOffset\":20,\"isPreserveSig\":false},{\"op\":\"ldc.r4\",\"opCode\":2,\"operand\":2.71828,\"ilOffset\":21,\"resultType\":\"System.Single\",\"isPreserveSig\":false},{\"op\":\"bne.un\",\"opCode\":79,\"operand\":31,\"ilOffset\":26,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":28,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":32,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":31,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":4,\"ilOffset\":32,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":36,\"ilOffset\":34,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":4,\"ilOffset\":36,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":38,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"PrimitiveNumericConversionsCoreSubjects\"}";
static const char kMethodJson_12[] = "{\"methodId\":\"primitive-numeric-conversions-core-subjects.primitive-numeric-conversions-core-native-entry.test-double-to-int\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestDoubleToInt:System.Int32()\",\"signature\":\"System.Int32 PrimitiveNumericConversionsCoreNativeEntry::TestDoubleToInt()\",\"identity\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"declaringTypeSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry\",\"definitionSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestDoubleToInt:System.Int32()\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestDoubleToInt:System.Int32()\",\"methodId\":\"primitive-numeric-conversions-core-subjects.primitive-numeric-conversions-core-native-entry.test-double-to-int\",\"signature\":\"System.Int32 PrimitiveNumericConversionsCoreNativeEntry::TestDoubleToInt()\"},\"nativeSymbol\":\"PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestDoubleToInt\",\"isStatic\":true,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":3,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.r8\",\"opCode\":3,\"operand\":3.99,\"ilOffset\":1,\"resultType\":\"System.Double\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":10,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":11,\"isPreserveSig\":false},{\"op\":\"conv.i4\",\"opCode\":39,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":14,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":3,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"bne.un\",\"opCode\":79,\"operand\":21,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":22,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":25,\"ilOffset\":23,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":25,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":26,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"PrimitiveNumericConversionsCoreSubjects\"}";
static const char kMethodJson_13[] = "{\"methodId\":\"primitive-numeric-conversions-core-subjects.primitive-numeric-conversions-core-native-entry.test-float-to-double\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestFloatToDouble:System.Int32()\",\"signature\":\"System.Int32 PrimitiveNumericConversionsCoreNativeEntry::TestFloatToDouble()\",\"identity\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"declaringTypeSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry\",\"definitionSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestFloatToDouble:System.Int32()\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestFloatToDouble:System.Int32()\",\"methodId\":\"primitive-numeric-conversions-core-subjects.primitive-numeric-conversions-core-native-entry.test-float-to-double\",\"signature\":\"System.Int32 PrimitiveNumericConversionsCoreNativeEntry::TestFloatToDouble()\"},\"nativeSymbol\":\"PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestFloatToDouble\",\"isStatic\":true,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":4,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.r4\",\"opCode\":2,\"operand\":3.14,\"ilOffset\":1,\"resultType\":\"System.Single\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":6,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"conv.r8\",\"opCode\":42,\"ilOffset\":8,\"resultType\":\"System.Double\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":9,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":10,\"isPreserveSig\":false},{\"op\":\"conv.r4\",\"opCode\":41,\"ilOffset\":11,\"resultType\":\"System.Single\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":12,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":13,\"isPreserveSig\":false},{\"op\":\"ldc.r4\",\"opCode\":2,\"operand\":3.14,\"ilOffset\":14,\"resultType\":\"System.Single\",\"isPreserveSig\":false},{\"op\":\"bne.un\",\"opCode\":79,\"operand\":24,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":25,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":3,\"ilOffset\":25,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":28,\"ilOffset\":26,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":3,\"ilOffset\":28,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":29,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"PrimitiveNumericConversionsCoreSubjects\"}";
static const char kMethodJson_14[] = "{\"methodId\":\"primitive-numeric-conversions-core-subjects.primitive-numeric-conversions-core-native-entry.test-int-to-byte\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestIntToByte:System.Int32()\",\"signature\":\"System.Int32 PrimitiveNumericConversionsCoreNativeEntry::TestIntToByte()\",\"identity\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"declaringTypeSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry\",\"definitionSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestIntToByte:System.Int32()\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestIntToByte:System.Int32()\",\"methodId\":\"primitive-numeric-conversions-core-subjects.primitive-numeric-conversions-core-native-entry.test-int-to-byte\",\"signature\":\"System.Int32 PrimitiveNumericConversionsCoreNativeEntry::TestIntToByte()\"},\"nativeSymbol\":\"PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToByte\",\"isStatic\":true,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":3,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":298,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":6,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"conv.u1\",\"opCode\":39,\"ilOffset\":8,\"resultType\":\"System.Byte\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":9,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":10,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":11,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"bne.un\",\"opCode\":79,\"operand\":18,\"ilOffset\":13,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":19,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":22,\"ilOffset\":20,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":22,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":23,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"PrimitiveNumericConversionsCoreSubjects\"}";
static const char kMethodJson_15[] = "{\"methodId\":\"primitive-numeric-conversions-core-subjects.primitive-numeric-conversions-core-native-entry.test-int-to-double\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestIntToDouble:System.Int32()\",\"signature\":\"System.Int32 PrimitiveNumericConversionsCoreNativeEntry::TestIntToDouble()\",\"identity\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"declaringTypeSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry\",\"definitionSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestIntToDouble:System.Int32()\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestIntToDouble:System.Int32()\",\"methodId\":\"primitive-numeric-conversions-core-subjects.primitive-numeric-conversions-core-native-entry.test-int-to-double\",\"signature\":\"System.Int32 PrimitiveNumericConversionsCoreNativeEntry::TestIntToDouble()\"},\"nativeSymbol\":\"PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToDouble\",\"isStatic\":true,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":4,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":3,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":4,\"isPreserveSig\":false},{\"op\":\"conv.r8\",\"opCode\":42,\"ilOffset\":5,\"resultType\":\"System.Double\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":6,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"conv.i4\",\"opCode\":39,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":9,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":10,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":11,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"bne.un\",\"opCode\":79,\"operand\":18,\"ilOffset\":13,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":19,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":3,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":22,\"ilOffset\":20,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":3,\"ilOffset\":22,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":23,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"PrimitiveNumericConversionsCoreSubjects\"}";
static const char kMethodJson_16[] = "{\"methodId\":\"primitive-numeric-conversions-core-subjects.primitive-numeric-conversions-core-native-entry.test-int-to-long\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestIntToLong:System.Int32()\",\"signature\":\"System.Int32 PrimitiveNumericConversionsCoreNativeEntry::TestIntToLong()\",\"identity\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"declaringTypeSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry\",\"definitionSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestIntToLong:System.Int32()\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestIntToLong:System.Int32()\",\"methodId\":\"primitive-numeric-conversions-core-subjects.primitive-numeric-conversions-core-native-entry.test-int-to-long\",\"signature\":\"System.Int32 PrimitiveNumericConversionsCoreNativeEntry::TestIntToLong()\"},\"nativeSymbol\":\"PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToLong\",\"isStatic\":true,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":3,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":-1,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":3,\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":4,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":6,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":-1,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":8,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"bne.un\",\"opCode\":79,\"operand\":14,\"ilOffset\":9,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":11,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":15,\"ilOffset\":12,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":15,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":18,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":19,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"PrimitiveNumericConversionsCoreSubjects\"}";
static const char kMethodJson_17[] = "{\"methodId\":\"primitive-numeric-conversions-core-subjects.primitive-numeric-conversions-core-native-entry.test-int-to-short\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestIntToShort:System.Int32()\",\"signature\":\"System.Int32 PrimitiveNumericConversionsCoreNativeEntry::TestIntToShort()\",\"identity\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"declaringTypeSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry\",\"definitionSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestIntToShort:System.Int32()\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestIntToShort:System.Int32()\",\"methodId\":\"primitive-numeric-conversions-core-subjects.primitive-numeric-conversions-core-native-entry.test-int-to-short\",\"signature\":\"System.Int32 PrimitiveNumericConversionsCoreNativeEntry::TestIntToShort()\"},\"nativeSymbol\":\"PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToShort\",\"isStatic\":true,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":3,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":65535,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":6,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"conv.i2\",\"opCode\":39,\"ilOffset\":8,\"resultType\":\"System.Int16\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":9,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":10,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":-1,\"ilOffset\":11,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"bne.un\",\"opCode\":79,\"operand\":17,\"ilOffset\":12,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":18,\"ilOffset\":15,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":18,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":21,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":21,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":22,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"PrimitiveNumericConversionsCoreSubjects\"}";
static const char kMethodJson_18[] = "{\"methodId\":\"primitive-numeric-conversions-core-subjects.primitive-numeric-conversions-core-native-entry.test-long-to-int\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestLongToInt:System.Int32()\",\"signature\":\"System.Int32 PrimitiveNumericConversionsCoreNativeEntry::TestLongToInt()\",\"identity\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"declaringTypeSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry\",\"definitionSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestLongToInt:System.Int32()\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestLongToInt:System.Int32()\",\"methodId\":\"primitive-numeric-conversions-core-subjects.primitive-numeric-conversions-core-native-entry.test-long-to-int\",\"signature\":\"System.Int32 PrimitiveNumericConversionsCoreNativeEntry::TestLongToInt()\"},\"nativeSymbol\":\"PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestLongToInt\",\"isStatic\":true,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":3,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i8\",\"opCode\":1,\"operand\":4294967297,\"ilOffset\":1,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":10,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":11,\"isPreserveSig\":false},{\"op\":\"conv.i4\",\"opCode\":39,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":14,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"bne.un\",\"opCode\":79,\"operand\":21,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":22,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":25,\"ilOffset\":23,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":25,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":26,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"PrimitiveNumericConversionsCoreSubjects\"}";
static const char kMethodJson_19[] = "{\"methodId\":\"primitive-numeric-conversions-core-subjects.primitive-numeric-conversions-core-native-entry.test-uint-to-long\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestUintToLong:System.Int32()\",\"signature\":\"System.Int32 PrimitiveNumericConversionsCoreNativeEntry::TestUintToLong()\",\"identity\":{\"assemblyName\":\"PrimitiveNumericConversionsCoreSubjects\",\"declaringTypeSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry\",\"definitionSubjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestUintToLong:System.Int32()\",\"subjectId\":\"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestUintToLong:System.Int32()\",\"methodId\":\"primitive-numeric-conversions-core-subjects.primitive-numeric-conversions-core-native-entry.test-uint-to-long\",\"signature\":\"System.Int32 PrimitiveNumericConversionsCoreNativeEntry::TestUintToLong()\"},\"nativeSymbol\":\"PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestUintToLong\",\"isStatic\":true,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":3,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":-2147483648,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":6,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"conv.u8\",\"opCode\":40,\"ilOffset\":8,\"resultType\":\"System.UInt64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":9,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":10,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":-2147483648,\"ilOffset\":11,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.u8\",\"opCode\":40,\"ilOffset\":16,\"resultType\":\"System.UInt64\",\"isPreserveSig\":false},{\"op\":\"bne.un\",\"opCode\":79,\"operand\":22,\"ilOffset\":17,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":19,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":23,\"ilOffset\":20,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":23,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":26,\"ilOffset\":24,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":26,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":27,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"PrimitiveNumericConversionsCoreSubjects\"}";

extern "C" const JitMethodEntry kChaosJitMethodEntries[20] =
{
    { kMethodJson_0, 6439u, 0x00000004u, 0u },
    { kMethodJson_1, 6439u, 0x00000005u, 0u },
    { kMethodJson_2, 6447u, 0x00000006u, 0u },
    { kMethodJson_3, 6439u, 0x00000007u, 0u },
    { kMethodJson_4, 6471u, 0x00000008u, 0u },
    { kMethodJson_5, 6471u, 0x00000009u, 0u },
    { kMethodJson_6, 6455u, 0x0000000Au, 0u },
    { kMethodJson_7, 6455u, 0x0000000Bu, 0u },
    { kMethodJson_8, 6447u, 0x0000000Cu, 0u },
    { kMethodJson_9, 6439u, 0x0000000Du, 0u },
    { kMethodJson_10, 2889u, 0x0000000Eu, 0u },
    { kMethodJson_11, 3567u, 0x00000013u, 0u },
    { kMethodJson_12, 3002u, 0x00000015u, 0u },
    { kMethodJson_13, 3288u, 0x00000012u, 0u },
    { kMethodJson_14, 2980u, 0x00000011u, 0u },
    { kMethodJson_15, 3262u, 0x00000014u, 0u },
    { kMethodJson_16, 3069u, 0x0000000Fu, 0u },
    { kMethodJson_17, 2991u, 0x00000010u, 0u },
    { kMethodJson_18, 2991u, 0x00000017u, 0u },
    { kMethodJson_19, 3101u, 0x00000016u, 0u }
};

extern "C" const uint32_t kChaosJitMethodEntryCount = 20u;

extern "C" void ChaosJitRegisterAll() {
    RegisterJitMethods(kChaosJitMethodEntries, kChaosJitMethodEntryCount);
}