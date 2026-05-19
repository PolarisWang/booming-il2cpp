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

namespace chaos::il2cpp::codegen::CcwBasicSubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_CcwBasicSubjects_IConstantValue = static_cast<CHAOS_IL2CPP_INTPTR>(11127514427160927756ULL);
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_CcwBasicSubjects_ISimpleMath = static_cast<CHAOS_IL2CPP_INTPTR>(502509295808532539ULL);
const void* chaos_vtable_CcwBasicSubjects_ConstantFortyTwo[];
const void* chaos_vtable_CcwBasicSubjects_DualInterfaceImpl[];
const void* chaos_vtable_CcwBasicSubjects_SimpleMath[];
inline TypeInfoV0 chaos_mt_CcwBasicSubjects_CcwBasicSubjects = {{nullptr, nullptr, 5516802378886089046ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_CcwBasicSubjects_CcwBasicSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(5516802378886089046ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Object = {{nullptr, nullptr, 15228727185366376748ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_INTPTR>(15228727185366376748ULL);static constexpr InterfaceMapEntry chaos_iface_map_CcwBasicSubjects_ConstantFortyTwo[] = {
	{ chaos_type_id_CcwBasicSubjects_IConstantValue, 1, 1 }
};
inline TypeInfoV0 chaos_mt_CcwBasicSubjects_ConstantFortyTwo = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_CcwBasicSubjects_ConstantFortyTwo, 9147543264357235806ULL, 2u, 32, 1, 1}, {chaos_iface_map_CcwBasicSubjects_ConstantFortyTwo, nullptr, 1, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_CcwBasicSubjects_ConstantFortyTwo = static_cast<CHAOS_IL2CPP_INTPTR>(9147543264357235806ULL);static constexpr InterfaceMapEntry chaos_iface_map_CcwBasicSubjects_DualInterfaceImpl[] = {
	{ chaos_type_id_CcwBasicSubjects_ISimpleMath, 2, 2 },
	{ chaos_type_id_CcwBasicSubjects_IConstantValue, 1, 1 }
};
inline TypeInfoV0 chaos_mt_CcwBasicSubjects_DualInterfaceImpl = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_CcwBasicSubjects_DualInterfaceImpl, 14234888545717869799ULL, 4u, 32, 1, 1}, {chaos_iface_map_CcwBasicSubjects_DualInterfaceImpl, nullptr, 2, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_CcwBasicSubjects_DualInterfaceImpl = static_cast<CHAOS_IL2CPP_INTPTR>(14234888545717869799ULL);static constexpr InterfaceMapEntry chaos_iface_map_CcwBasicSubjects_SimpleMath[] = {
	{ chaos_type_id_CcwBasicSubjects_ISimpleMath, 2, 2 }
};
inline TypeInfoV0 chaos_mt_CcwBasicSubjects_SimpleMath = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_CcwBasicSubjects_SimpleMath, 4431683709018832856ULL, 4u, 32, 1, 1}, {chaos_iface_map_CcwBasicSubjects_SimpleMath, nullptr, 1, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_CcwBasicSubjects_SimpleMath = static_cast<CHAOS_IL2CPP_INTPTR>(4431683709018832856ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_Assembly = {{nullptr, nullptr, 5474029880995115448ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_Assembly = static_cast<CHAOS_IL2CPP_INTPTR>(5474029880995115448ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName = {{nullptr, nullptr, 17082367815459723707ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_AssemblyName = static_cast<CHAOS_IL2CPP_INTPTR>(17082367815459723707ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo = {{nullptr, nullptr, 4137207361503509124ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_ConstructorInfo = static_cast<CHAOS_IL2CPP_INTPTR>(4137207361503509124ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo = {{nullptr, nullptr, 17040031516751226236ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_FieldInfo = static_cast<CHAOS_IL2CPP_INTPTR>(17040031516751226236ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo = {{nullptr, nullptr, 10748947813473285525ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_MethodInfo = static_cast<CHAOS_IL2CPP_INTPTR>(10748947813473285525ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_String = {{nullptr, nullptr, 1782325859292956794ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_String = static_cast<CHAOS_IL2CPP_INTPTR>(1782325859292956794ULL);inline TypeInfoV0 chaos_mt_CcwBasicSubjects_IConstantValue = {{nullptr, nullptr, 11127514427160927756ULL, 0u, 32, 3, 0}, {nullptr, nullptr, 0, 0, 0, 0}};
inline TypeInfoV0 chaos_mt_CcwBasicSubjects_ISimpleMath = {{nullptr, nullptr, 502509295808532539ULL, 0u, 32, 3, 0}, {nullptr, nullptr, 0, 0, 0, 0}};

// ── Virtual method table arrays ──
extern "C" void CcwBasicSubjects_ConstantFortyTwo__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 CcwBasicSubjects_ConstantFortyTwo_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
const void* chaos_vtable_CcwBasicSubjects_ConstantFortyTwo[] =
{
	reinterpret_cast<void*>(CcwBasicSubjects_ConstantFortyTwo__ctor),
	reinterpret_cast<void*>(CcwBasicSubjects_ConstantFortyTwo_GetValue),
};
extern "C" void CcwBasicSubjects_DualInterfaceImpl__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 CcwBasicSubjects_DualInterfaceImpl_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 CcwBasicSubjects_DualInterfaceImpl_Add_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INT32 CcwBasicSubjects_DualInterfaceImpl_Multiply_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
const void* chaos_vtable_CcwBasicSubjects_DualInterfaceImpl[] =
{
	reinterpret_cast<void*>(CcwBasicSubjects_DualInterfaceImpl__ctor),
	reinterpret_cast<void*>(CcwBasicSubjects_DualInterfaceImpl_GetValue),
	reinterpret_cast<void*>(CcwBasicSubjects_DualInterfaceImpl_Add_System_Int32_System_Int32),
	reinterpret_cast<void*>(CcwBasicSubjects_DualInterfaceImpl_Multiply_System_Int32_System_Int32),
};
extern "C" void CcwBasicSubjects_SimpleMath__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 CcwBasicSubjects_SimpleMath_Add_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INT32 CcwBasicSubjects_SimpleMath_Multiply_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
const void* chaos_vtable_CcwBasicSubjects_SimpleMath[] =
{
	reinterpret_cast<void*>(CcwBasicSubjects_SimpleMath__ctor),
	nullptr,
	reinterpret_cast<void*>(CcwBasicSubjects_SimpleMath_Add_System_Int32_System_Int32),
	reinterpret_cast<void*>(CcwBasicSubjects_SimpleMath_Multiply_System_Int32_System_Int32),
};
static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_CcwBasicSubjects_ConstantFortyTwo[] =
{
	{ 0x00000015u, reinterpret_cast<void*>(&CcwBasicSubjects_ConstantFortyTwo__ctor) },
	{ 0x00000014u, reinterpret_cast<void*>(&CcwBasicSubjects_ConstantFortyTwo_GetValue) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_CcwBasicSubjects_DualInterfaceImpl[] =
{
	{ 0x00000019u, reinterpret_cast<void*>(&CcwBasicSubjects_DualInterfaceImpl__ctor) },
	{ 0x00000018u, reinterpret_cast<void*>(&CcwBasicSubjects_DualInterfaceImpl_GetValue) },
	{ 0x00000016u, reinterpret_cast<void*>(&CcwBasicSubjects_DualInterfaceImpl_Add_System_Int32_System_Int32) },
	{ 0x00000017u, reinterpret_cast<void*>(&CcwBasicSubjects_DualInterfaceImpl_Multiply_System_Int32_System_Int32) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_CcwBasicSubjects_SimpleMath[] =
{
	{ 0x00000013u, reinterpret_cast<void*>(&CcwBasicSubjects_SimpleMath__ctor) },
	{ 0u, nullptr },
	{ 0x00000011u, reinterpret_cast<void*>(&CcwBasicSubjects_SimpleMath_Add_System_Int32_System_Int32) },
	{ 0x00000012u, reinterpret_cast<void*>(&CcwBasicSubjects_SimpleMath_Multiply_System_Int32_System_Int32) },
};

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

struct chaos_type_CcwBasicSubjects_CcwBasicSubjects
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Private_CoreLib_System_Object
{
	ThinLockableHeader header{};
};

struct chaos_type_CcwBasicSubjects_ConstantFortyTwo : public chaos_type_System_Private_CoreLib_System_Object
{
};

struct chaos_type_CcwBasicSubjects_DualInterfaceImpl : public chaos_type_System_Private_CoreLib_System_Object
{
};

struct chaos_type_CcwBasicSubjects_SimpleMath : public chaos_type_System_Private_CoreLib_System_Object
{
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

CHAOS_IL2CPP_INT32 chaos_static_CcwBasicSubjects_CcwBasicSubjects___exitCode = 0;

static void chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	ChaosObjectCtor(chaos_fn_arg_0);
}

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };

extern "C" void CcwBasicSubjects_CcwBasicSubjects_Subject_0(void);
extern "C" void CcwBasicSubjects_CcwBasicSubjects_Subject_1(void);
extern "C" void CcwBasicSubjects_CcwBasicSubjects_Subject_2(void);
extern "C" void CcwBasicSubjects_CcwBasicSubjects_Subject_3(void);
extern "C" void CcwBasicSubjects_CcwBasicSubjects_Subject_4(void);
extern "C" CHAOS_IL2CPP_INT32 CcwBasicSubjects_CcwBasicNativeEntry_Run_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 CcwBasicSubjects_CcwBasicNativeEntry_TestConstantValue(void);
extern "C" CHAOS_IL2CPP_INT32 CcwBasicSubjects_CcwBasicNativeEntry_TestDualInterface(void);
extern "C" CHAOS_IL2CPP_INT32 CcwBasicSubjects_CcwBasicNativeEntry_TestSimpleMathAdd(void);
extern "C" CHAOS_IL2CPP_INT32 CcwBasicSubjects_CcwBasicNativeEntry_TestSimpleMathMul(void);
extern "C" void CcwBasicSubjects_ConstantFortyTwo__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 CcwBasicSubjects_ConstantFortyTwo_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CcwBasicSubjects_DualInterfaceImpl__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 CcwBasicSubjects_DualInterfaceImpl_Add_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INT32 CcwBasicSubjects_DualInterfaceImpl_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 CcwBasicSubjects_DualInterfaceImpl_Multiply_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" void CcwBasicSubjects_SimpleMath__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 CcwBasicSubjects_SimpleMath_Add_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INT32 CcwBasicSubjects_SimpleMath_Multiply_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);


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
	1u,
	1u,
	1u,
	1u,
	1u,
	1u,
	1u,
	1u,
	3u,
	3u,
	5u,
	5u,
	7u,
	9u,
};

static constexpr struct {
	::ChaosAbiManifestV0 header;
	::ChaosAbiMethodEntryV0 entries[19];
	CHAOS_IL2CPP_UINT8 params[9];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		19u,
		9u,
		39669302u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // CcwBasicSubjects_CcwBasicSubjects_Subject_0
		{ 0u, 0u },  // CcwBasicSubjects_CcwBasicSubjects_Subject_1
		{ 0u, 0u },  // CcwBasicSubjects_CcwBasicSubjects_Subject_2
		{ 0u, 0u },  // CcwBasicSubjects_CcwBasicSubjects_Subject_3
		{ 0u, 0u },  // CcwBasicSubjects_CcwBasicSubjects_Subject_4
		{ 1u, 1u },  // CcwBasicSubjects_CcwBasicNativeEntry_Run_System_Int32
		{ 1u, 0u },  // CcwBasicSubjects_CcwBasicNativeEntry_TestConstantValue
		{ 1u, 0u },  // CcwBasicSubjects_CcwBasicNativeEntry_TestDualInterface
		{ 1u, 0u },  // CcwBasicSubjects_CcwBasicNativeEntry_TestSimpleMathAdd
		{ 1u, 0u },  // CcwBasicSubjects_CcwBasicNativeEntry_TestSimpleMathMul
		{ 0u, 0u },  // CcwBasicSubjects_ConstantFortyTwo__ctor
		{ 1u, 0u },  // CcwBasicSubjects_ConstantFortyTwo_GetValue
		{ 0u, 0u },  // CcwBasicSubjects_DualInterfaceImpl__ctor
		{ 1u, 2u },  // CcwBasicSubjects_DualInterfaceImpl_Add_System_Int32_System_Int32
		{ 1u, 0u },  // CcwBasicSubjects_DualInterfaceImpl_GetValue
		{ 1u, 2u },  // CcwBasicSubjects_DualInterfaceImpl_Multiply_System_Int32_System_Int32
		{ 0u, 0u },  // CcwBasicSubjects_SimpleMath__ctor
		{ 1u, 2u },  // CcwBasicSubjects_SimpleMath_Add_System_Int32_System_Int32
		{ 1u, 2u },  // CcwBasicSubjects_SimpleMath_Multiply_System_Int32_System_Int32
	},
	{
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
	},
};
static const ::ChaosAbiManifestV0* const s_abi_manifest =
	reinterpret_cast<const ::ChaosAbiManifestV0*>(&s_abi_manifest_storage);
// ── Module registration ──

	static constexpr CHAOS_IL2CPP_UINT32 s_type_flags[8] = {
		0u,
		1548u,
		1556u,
		1556u,
		1536u,
		1536u,
		1536u,
		1548u,
	};

	static constexpr const char* s_type_names[8] = {
		"<Module>",
		"CcwBasicSubjects",
		"ISimpleMath",
		"IConstantValue",
		"SimpleMath",
		"ConstantFortyTwo",
		"DualInterfaceImpl",
		"CcwBasicNativeEntry",
	};

	static constexpr const char* s_type_namespaces[8] = {
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_type_parent_tokens[8] = {
		0u,
		0u,
		0u,
		0u,
		0u,
		0u,
		0u,
		0u,
	};

	static const TypeInfoHot* const s_type_info_ptrs[8] = {
		nullptr,
		&chaos_mt_CcwBasicSubjects_CcwBasicSubjects.hot,
		&chaos_mt_CcwBasicSubjects_ISimpleMath.hot,
		&chaos_mt_CcwBasicSubjects_IConstantValue.hot,
		&chaos_mt_CcwBasicSubjects_SimpleMath.hot,
		&chaos_mt_CcwBasicSubjects_ConstantFortyTwo.hot,
		&chaos_mt_CcwBasicSubjects_DualInterfaceImpl.hot,
		nullptr,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_offset[9] = {
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

	static constexpr CHAOS_IL2CPP_UINT32 s_generic_param_constraint_offset[9] = {
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



	static const ::chaos::il2cpp::runtime_core::ModuleDescriptor s_native_aot_module = {
		/* .name_utf8         = */ "CcwBasicSubjects",
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
		/* .type_count        = */ 8u,
	/* .custom_attribute_blob       = */ nullptr,  // Tier 1 — deferred
	/* .custom_attribute_offset     = */ nullptr,
	/* .custom_attribute_entity_count = */ 0u,
	/* .custom_attribute_materializer = */ nullptr,

		/* .abi_manifest      = */ s_abi_manifest,
	};
	static const CHAOS_IL2CPP_UINT32 s_native_aot_module_id =
		::chaos::il2cpp::runtime_core::RegisterModule("CcwBasicSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[19] = {
	{ "Run", 0x0000001Au, 1u },  // CcwBasicNativeEntry
	{ "TestConstantValue", 0x0000001Du, 0u },  // CcwBasicNativeEntry
	{ "TestDualInterface", 0x0000001Eu, 0u },  // CcwBasicNativeEntry
	{ "TestSimpleMathAdd", 0x0000001Bu, 0u },  // CcwBasicNativeEntry
	{ "TestSimpleMathMul", 0x0000001Cu, 0u },  // CcwBasicNativeEntry
	{ "Subject_0", 0x00000009u, 0u },  // CcwBasicSubjects
	{ "Subject_1", 0x0000000Au, 0u },  // CcwBasicSubjects
	{ "Subject_2", 0x0000000Bu, 0u },  // CcwBasicSubjects
	{ "Subject_3", 0x0000000Cu, 0u },  // CcwBasicSubjects
	{ "Subject_4", 0x0000000Du, 0u },  // CcwBasicSubjects
	{ ".ctor", 0x00000015u, 0u },  // ConstantFortyTwo
	{ "GetValue", 0x00000014u, 0u },  // ConstantFortyTwo
	{ ".ctor", 0x00000019u, 0u },  // DualInterfaceImpl
	{ "Add", 0x00000016u, 2u },  // DualInterfaceImpl
	{ "GetValue", 0x00000018u, 0u },  // DualInterfaceImpl
	{ "Multiply", 0x00000017u, 2u },  // DualInterfaceImpl
	{ ".ctor", 0x00000013u, 0u },  // SimpleMath
	{ "Add", 0x00000011u, 2u },  // SimpleMath
	{ "Multiply", 0x00000012u, 2u },  // SimpleMath
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[5] = {
	{ "CcwBasicNativeEntry", "", 0u, 5u },
	{ "CcwBasicSubjects", "", 5u, 5u },
	{ "ConstantFortyTwo", "", 10u, 2u },
	{ "DualInterfaceImpl", "", 12u, 4u },
	{ "SimpleMath", "", 16u, 3u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[19] = {
	{ 0x00000009u, 0u },
	{ 0x0000000Au, 1u },
	{ 0x0000000Bu, 2u },
	{ 0x0000000Cu, 3u },
	{ 0x0000000Du, 4u },
	{ 0x00000011u, 17u },
	{ 0x00000012u, 18u },
	{ 0x00000013u, 16u },
	{ 0x00000014u, 11u },
	{ 0x00000015u, 10u },
	{ 0x00000016u, 13u },
	{ 0x00000017u, 15u },
	{ 0x00000018u, 14u },
	{ 0x00000019u, 12u },
	{ 0x0000001Au, 5u },
	{ 0x0000001Bu, 8u },
	{ 0x0000001Cu, 9u },
	{ 0x0000001Du, 6u },
	{ 0x0000001Eu, 7u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[19] = {
	{ reinterpret_cast<void*>(&CcwBasicSubjects_CcwBasicSubjects_Subject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // CcwBasicSubjects::Subject_0
	{ reinterpret_cast<void*>(&CcwBasicSubjects_CcwBasicSubjects_Subject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // CcwBasicSubjects::Subject_1
	{ reinterpret_cast<void*>(&CcwBasicSubjects_CcwBasicSubjects_Subject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // CcwBasicSubjects::Subject_2
	{ reinterpret_cast<void*>(&CcwBasicSubjects_CcwBasicSubjects_Subject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // CcwBasicSubjects::Subject_3
	{ reinterpret_cast<void*>(&CcwBasicSubjects_CcwBasicSubjects_Subject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // CcwBasicSubjects::Subject_4
	{ reinterpret_cast<void*>(&CcwBasicSubjects_CcwBasicNativeEntry_Run_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // CcwBasicNativeEntry::Run
	{ reinterpret_cast<void*>(&CcwBasicSubjects_CcwBasicNativeEntry_TestConstantValue), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // CcwBasicNativeEntry::TestConstantValue
	{ reinterpret_cast<void*>(&CcwBasicSubjects_CcwBasicNativeEntry_TestDualInterface), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // CcwBasicNativeEntry::TestDualInterface
	{ reinterpret_cast<void*>(&CcwBasicSubjects_CcwBasicNativeEntry_TestSimpleMathAdd), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // CcwBasicNativeEntry::TestSimpleMathAdd
	{ reinterpret_cast<void*>(&CcwBasicSubjects_CcwBasicNativeEntry_TestSimpleMathMul), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // CcwBasicNativeEntry::TestSimpleMathMul
	{ reinterpret_cast<void*>(&CcwBasicSubjects_ConstantFortyTwo__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ConstantFortyTwo::.ctor
	{ reinterpret_cast<void*>(&CcwBasicSubjects_ConstantFortyTwo_GetValue), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ConstantFortyTwo::GetValue
	{ reinterpret_cast<void*>(&CcwBasicSubjects_DualInterfaceImpl__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // DualInterfaceImpl::.ctor
	{ reinterpret_cast<void*>(&CcwBasicSubjects_DualInterfaceImpl_Add_System_Int32_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // DualInterfaceImpl::Add
	{ reinterpret_cast<void*>(&CcwBasicSubjects_DualInterfaceImpl_GetValue), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // DualInterfaceImpl::GetValue
	{ reinterpret_cast<void*>(&CcwBasicSubjects_DualInterfaceImpl_Multiply_System_Int32_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // DualInterfaceImpl::Multiply
	{ reinterpret_cast<void*>(&CcwBasicSubjects_SimpleMath__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SimpleMath::.ctor
	{ reinterpret_cast<void*>(&CcwBasicSubjects_SimpleMath_Add_System_Int32_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SimpleMath::Add
	{ reinterpret_cast<void*>(&CcwBasicSubjects_SimpleMath_Multiply_System_Int32_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SimpleMath::Multiply
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"CcwBasicSubjects",
	s_hotpatch_types,
	5u,
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

extern "C" const char* kChaosExternalRuntimeSubjects[2] = {
	"CcwBasicSubjects/CcwBasicSubjects::_exitCode",
	"System.Private.CoreLib/System.Object::.ctor:System.Void()",
};

extern "C" void* kChaosExternalRuntimeFnTable[2] = {
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__),
};

extern "C" int32_t kChaosExternalRuntimeCount = 2;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[19])() = {
	reinterpret_cast<void(*)()>(&CcwBasicSubjects_CcwBasicSubjects_Subject_0),
	reinterpret_cast<void(*)()>(&CcwBasicSubjects_CcwBasicSubjects_Subject_1),
	reinterpret_cast<void(*)()>(&CcwBasicSubjects_CcwBasicSubjects_Subject_2),
	reinterpret_cast<void(*)()>(&CcwBasicSubjects_CcwBasicSubjects_Subject_3),
	reinterpret_cast<void(*)()>(&CcwBasicSubjects_CcwBasicSubjects_Subject_4),
	reinterpret_cast<void(*)()>(&CcwBasicSubjects_CcwBasicNativeEntry_Run_System_Int32),
	reinterpret_cast<void(*)()>(&CcwBasicSubjects_CcwBasicNativeEntry_TestConstantValue),
	reinterpret_cast<void(*)()>(&CcwBasicSubjects_CcwBasicNativeEntry_TestDualInterface),
	reinterpret_cast<void(*)()>(&CcwBasicSubjects_CcwBasicNativeEntry_TestSimpleMathAdd),
	reinterpret_cast<void(*)()>(&CcwBasicSubjects_CcwBasicNativeEntry_TestSimpleMathMul),
	reinterpret_cast<void(*)()>(&CcwBasicSubjects_ConstantFortyTwo__ctor),
	reinterpret_cast<void(*)()>(&CcwBasicSubjects_ConstantFortyTwo_GetValue),
	reinterpret_cast<void(*)()>(&CcwBasicSubjects_DualInterfaceImpl__ctor),
	reinterpret_cast<void(*)()>(&CcwBasicSubjects_DualInterfaceImpl_Add_System_Int32_System_Int32),
	reinterpret_cast<void(*)()>(&CcwBasicSubjects_DualInterfaceImpl_GetValue),
	reinterpret_cast<void(*)()>(&CcwBasicSubjects_DualInterfaceImpl_Multiply_System_Int32_System_Int32),
	reinterpret_cast<void(*)()>(&CcwBasicSubjects_SimpleMath__ctor),
	reinterpret_cast<void(*)()>(&CcwBasicSubjects_SimpleMath_Add_System_Int32_System_Int32),
	reinterpret_cast<void(*)()>(&CcwBasicSubjects_SimpleMath_Multiply_System_Int32_System_Int32),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
static void (*kBenchmarkWrappers[19])() = {
	[]() {kAotMethods[0]();},
	[]() {kAotMethods[1]();},
	[]() {kAotMethods[2]();},
	[]() {kAotMethods[3]();},
	[]() {kAotMethods[4]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[5])(0);},
	[]() {kAotMethods[6]();},
	[]() {kAotMethods[7]();},
	[]() {kAotMethods[8]();},
	[]() {kAotMethods[9]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[10])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[11])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[12])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[13])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0,0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[14])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[15])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0,0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[16])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[17])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0,0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[18])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0,0);},
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
static void* const kMethodPointers[19] = {
	reinterpret_cast<void*>(&CcwBasicSubjects_CcwBasicSubjects_Subject_0),
	reinterpret_cast<void*>(&CcwBasicSubjects_CcwBasicSubjects_Subject_1),
	reinterpret_cast<void*>(&CcwBasicSubjects_CcwBasicSubjects_Subject_2),
	reinterpret_cast<void*>(&CcwBasicSubjects_CcwBasicSubjects_Subject_3),
	reinterpret_cast<void*>(&CcwBasicSubjects_CcwBasicSubjects_Subject_4),
	reinterpret_cast<void*>(&CcwBasicSubjects_CcwBasicNativeEntry_Run_System_Int32),
	reinterpret_cast<void*>(&CcwBasicSubjects_CcwBasicNativeEntry_TestConstantValue),
	reinterpret_cast<void*>(&CcwBasicSubjects_CcwBasicNativeEntry_TestDualInterface),
	reinterpret_cast<void*>(&CcwBasicSubjects_CcwBasicNativeEntry_TestSimpleMathAdd),
	reinterpret_cast<void*>(&CcwBasicSubjects_CcwBasicNativeEntry_TestSimpleMathMul),
	reinterpret_cast<void*>(&CcwBasicSubjects_ConstantFortyTwo__ctor),
	reinterpret_cast<void*>(&CcwBasicSubjects_ConstantFortyTwo_GetValue),
	reinterpret_cast<void*>(&CcwBasicSubjects_DualInterfaceImpl__ctor),
	reinterpret_cast<void*>(&CcwBasicSubjects_DualInterfaceImpl_Add_System_Int32_System_Int32),
	reinterpret_cast<void*>(&CcwBasicSubjects_DualInterfaceImpl_GetValue),
	reinterpret_cast<void*>(&CcwBasicSubjects_DualInterfaceImpl_Multiply_System_Int32_System_Int32),
	reinterpret_cast<void*>(&CcwBasicSubjects_SimpleMath__ctor),
	reinterpret_cast<void*>(&CcwBasicSubjects_SimpleMath_Add_System_Int32_System_Int32),
	reinterpret_cast<void*>(&CcwBasicSubjects_SimpleMath_Multiply_System_Int32_System_Int32),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
// ── VTable descriptors (for BootstrapRuntime TypeVTable registration) ──
static const VTableDescriptorV0 kChaosVTableDescriptors[] = {
	{
		CHAOS_IL2CPP_UINT64_C(0x7EF29A221787BC5E),
		0x00000005u,
		0u,
		2u,
		reinterpret_cast<const void*>(kSlots_CcwBasicSubjects_ConstantFortyTwo),
		reinterpret_cast<const void**>(chaos_vtable_CcwBasicSubjects_ConstantFortyTwo),
		2u,
		1,
		{0, 0, 0},
		chaos_iface_map_CcwBasicSubjects_ConstantFortyTwo,
		1u,
	},
	{
		CHAOS_IL2CPP_UINT64_C(0xC58C7BB670CA5CE7),
		0x00000006u,
		0u,
		4u,
		reinterpret_cast<const void*>(kSlots_CcwBasicSubjects_DualInterfaceImpl),
		reinterpret_cast<const void**>(chaos_vtable_CcwBasicSubjects_DualInterfaceImpl),
		4u,
		1,
		{0, 0, 0},
		chaos_iface_map_CcwBasicSubjects_DualInterfaceImpl,
		2u,
	},
	{
		CHAOS_IL2CPP_UINT64_C(0x3D8080DA70F3FFD8),
		0x00000004u,
		0u,
		4u,
		reinterpret_cast<const void*>(kSlots_CcwBasicSubjects_SimpleMath),
		reinterpret_cast<const void**>(chaos_vtable_CcwBasicSubjects_SimpleMath),
		4u,
		1,
		{0, 0, 0},
		chaos_iface_map_CcwBasicSubjects_SimpleMath,
		1u,
	},
};
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
	.vtable_descriptors = kChaosVTableDescriptors,
	.vtable_descriptor_count = 3u,
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
	.image_name_utf8    = "CcwBasicSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_CcwBasicSubjects_CcwBasicSubjects[1] = {
	{ 0u, "CcwBasicSubjects/CcwBasicSubjects::_exitCode", "_exitCode", "System.Int32", 0LL },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_CcwBasicSubjects_CcwBasicSubjects[5] = {
	{ 0u, "CcwBasicSubjects/CcwBasicSubjects::Subject_0:System.Void()", "Subject_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "CcwBasicSubjects/CcwBasicSubjects::Subject_1:System.Void()", "Subject_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "CcwBasicSubjects/CcwBasicSubjects::Subject_2:System.Void()", "Subject_2", "System.Void", 0, nullptr, 0u },
	{ 0u, "CcwBasicSubjects/CcwBasicSubjects::Subject_3:System.Void()", "Subject_3", "System.Void", 0, nullptr, 0u },
	{ 0u, "CcwBasicSubjects/CcwBasicSubjects::Subject_4:System.Void()", "Subject_4", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_CcwBasicSubjects_CcwBasicNativeEntry[5] = {
	{ 0u, "CcwBasicSubjects/CcwBasicNativeEntry::Run:System.Int32(System.Int32)", "Run", "System.Void", 0, nullptr, 0u },
	{ 0u, "CcwBasicSubjects/CcwBasicNativeEntry::TestConstantValue:System.Int32()", "TestConstantValue", "System.Void", 0, nullptr, 0u },
	{ 0u, "CcwBasicSubjects/CcwBasicNativeEntry::TestDualInterface:System.Int32()", "TestDualInterface", "System.Void", 0, nullptr, 0u },
	{ 0u, "CcwBasicSubjects/CcwBasicNativeEntry::TestSimpleMathAdd:System.Int32()", "TestSimpleMathAdd", "System.Void", 0, nullptr, 0u },
	{ 0u, "CcwBasicSubjects/CcwBasicNativeEntry::TestSimpleMathMul:System.Int32()", "TestSimpleMathMul", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_CcwBasicSubjects_ConstantFortyTwo[2] = {
	{ 0u, "CcwBasicSubjects/ConstantFortyTwo::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "CcwBasicSubjects/ConstantFortyTwo::GetValue:System.Int32()", "GetValue", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_CcwBasicSubjects_DualInterfaceImpl[4] = {
	{ 0u, "CcwBasicSubjects/DualInterfaceImpl::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "CcwBasicSubjects/DualInterfaceImpl::Add:System.Int32(System.Int32,System.Int32)", "Add", "System.Void", 0, nullptr, 0u },
	{ 0u, "CcwBasicSubjects/DualInterfaceImpl::GetValue:System.Int32()", "GetValue", "System.Void", 0, nullptr, 0u },
	{ 0u, "CcwBasicSubjects/DualInterfaceImpl::Multiply:System.Int32(System.Int32,System.Int32)", "Multiply", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_CcwBasicSubjects_SimpleMath[3] = {
	{ 0u, "CcwBasicSubjects/SimpleMath::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "CcwBasicSubjects/SimpleMath::Add:System.Int32(System.Int32,System.Int32)", "Add", "System.Void", 0, nullptr, 0u },
	{ 0u, "CcwBasicSubjects/SimpleMath::Multiply:System.Int32(System.Int32,System.Int32)", "Multiply", "System.Void", 0, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[5] = {
	{ 0u, "CcwBasicSubjects/CcwBasicSubjects", "CcwBasicSubjects/CcwBasicSubjects", "", "CcwBasicSubjects", "CcwBasicSubjects", nullptr, kReflFields_CcwBasicSubjects_CcwBasicSubjects, 1u, nullptr, 0u,
	kReflMethods_CcwBasicSubjects_CcwBasicSubjects, 5u },
	{ 0u, "CcwBasicSubjects/CcwBasicNativeEntry", "CcwBasicSubjects/CcwBasicNativeEntry", "", "CcwBasicNativeEntry", "CcwBasicNativeEntry", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_CcwBasicSubjects_CcwBasicNativeEntry, 5u },
	{ 0u, "CcwBasicSubjects/ConstantFortyTwo", "CcwBasicSubjects/ConstantFortyTwo", "", "ConstantFortyTwo", "ConstantFortyTwo", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_CcwBasicSubjects_ConstantFortyTwo, 2u },
	{ 0u, "CcwBasicSubjects/DualInterfaceImpl", "CcwBasicSubjects/DualInterfaceImpl", "", "DualInterfaceImpl", "DualInterfaceImpl", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_CcwBasicSubjects_DualInterfaceImpl, 4u },
	{ 0u, "CcwBasicSubjects/SimpleMath", "CcwBasicSubjects/SimpleMath", "", "SimpleMath", "SimpleMath", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_CcwBasicSubjects_SimpleMath, 3u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[5] = {
	&kReflTypes[0],
	&kReflTypes[1],
	&kReflTypes[2],
	&kReflTypes[3],
	&kReflTypes[4],
};

static const ReflectionQueryImageDescriptor kReflImage = { "CcwBasicSubjects", kReflTypePtrs, 5u };

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
// Managed method: CcwBasicSubjects/CcwBasicSubjects::Subject_0()
extern "C" void CcwBasicSubjects_CcwBasicSubjects_Subject_0(void)
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
	CHAOS_IL2CPP_INTPTR _s9{};


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
				chaos_static_CcwBasicSubjects_CcwBasicSubjects___exitCode = chaos_value;
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
				chaos_static_CcwBasicSubjects_CcwBasicSubjects___exitCode = chaos_value;
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
				chaos_static_CcwBasicSubjects_CcwBasicSubjects___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	return;
}

// Managed method: CcwBasicSubjects/CcwBasicSubjects::Subject_1()
extern "C" void CcwBasicSubjects_CcwBasicSubjects_Subject_1(void)
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
				chaos_static_CcwBasicSubjects_CcwBasicSubjects___exitCode = chaos_value;
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
				chaos_static_CcwBasicSubjects_CcwBasicSubjects___exitCode = chaos_value;
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
				chaos_static_CcwBasicSubjects_CcwBasicSubjects___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	return;
}

// Managed method: CcwBasicSubjects/CcwBasicSubjects::Subject_2()
extern "C" void CcwBasicSubjects_CcwBasicSubjects_Subject_2(void)
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
				chaos_static_CcwBasicSubjects_CcwBasicSubjects___exitCode = chaos_value;
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
				chaos_static_CcwBasicSubjects_CcwBasicSubjects___exitCode = chaos_value;
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
				chaos_static_CcwBasicSubjects_CcwBasicSubjects___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	return;
}

// Managed method: CcwBasicSubjects/CcwBasicSubjects::Subject_3()
extern "C" void CcwBasicSubjects_CcwBasicSubjects_Subject_3(void)
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
				chaos_static_CcwBasicSubjects_CcwBasicSubjects___exitCode = chaos_value;
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
				chaos_static_CcwBasicSubjects_CcwBasicSubjects___exitCode = chaos_value;
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
				chaos_static_CcwBasicSubjects_CcwBasicSubjects___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	return;
}

// Managed method: CcwBasicSubjects/CcwBasicSubjects::Subject_4()
extern "C" void CcwBasicSubjects_CcwBasicSubjects_Subject_4(void)
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
				chaos_static_CcwBasicSubjects_CcwBasicSubjects___exitCode = chaos_value;
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
				chaos_static_CcwBasicSubjects_CcwBasicSubjects___exitCode = chaos_value;
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
				chaos_static_CcwBasicSubjects_CcwBasicSubjects___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	return;
}

// Managed method: CcwBasicSubjects/CcwBasicNativeEntry::Run(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 CcwBasicSubjects_CcwBasicNativeEntry_Run_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
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
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		const auto chaos_switch_value = static_cast<CHAOS_IL2CPP_INT32>(_s0);
		switch (chaos_switch_value)
		{
			case 0:
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
						_d_hpresult = CcwBasicSubjects_CcwBasicNativeEntry_TestSimpleMathAdd();
					}
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
				}
				chaos_locals[2] = _s0;
			case 1:
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
						_d_hpresult = CcwBasicSubjects_CcwBasicNativeEntry_TestSimpleMathMul();
					}
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
				}
				chaos_locals[2] = _s0;
			case 2:
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
						_d_hpresult = CcwBasicSubjects_CcwBasicNativeEntry_TestConstantValue();
					}
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
				}
				chaos_locals[2] = _s0;
			case 3:
			{
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
						_d_hpresult = CcwBasicSubjects_CcwBasicNativeEntry_TestDualInterface();
					}
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
				}
				chaos_locals[2] = _s0;
				break;
			}
			default:
			{
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1);
				chaos_locals[2] = _s0;
				break;
			}
		}
	}
	_s0 = chaos_locals[2];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: CcwBasicSubjects/CcwBasicNativeEntry::TestConstantValue()
extern "C" CHAOS_IL2CPP_INT32 CcwBasicSubjects_CcwBasicNativeEntry_TestConstantValue(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_CcwBasicSubjects_ConstantFortyTwo, {});
		chaos_object->header.type_info = &chaos_mt_CcwBasicSubjects_ConstantFortyTwo.hot;
		CcwBasicSubjects_ConstantFortyTwo__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		CHAOS_IL2CPP_INT32 chaos_callvirt_result{};
		chaos_callvirt_result = (*reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)>(chaos_vtable_resolve(chaos_object_get_type_info(reinterpret_cast<void*>(chaos_arg_0))->vtable_array, 1u)))(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_callvirt_result);
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: CcwBasicSubjects/CcwBasicNativeEntry::TestDualInterface()
extern "C" CHAOS_IL2CPP_INT32 CcwBasicSubjects_CcwBasicNativeEntry_TestDualInterface(void)
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


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_CcwBasicSubjects_DualInterfaceImpl, {});
		chaos_object->header.type_info = &chaos_mt_CcwBasicSubjects_DualInterfaceImpl.hot;
		CcwBasicSubjects_DualInterfaceImpl__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[0];
	chaos_locals[2] = _s0;
	_s0 = chaos_locals[1];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(10);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
	{
		const auto chaos_raw_arg_2 = _s2;
		const auto chaos_arg_2 = chaos_raw_arg_2;
		const auto chaos_raw_arg_1 = _s1;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		CHAOS_IL2CPP_INT32 chaos_callvirt_result{};
		chaos_callvirt_result = (*reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2)>(chaos_vtable_resolve(chaos_object_get_type_info(reinterpret_cast<void*>(chaos_arg_0))->vtable_array, 2u)))(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2));
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_callvirt_result);
	}
	_s1 = chaos_locals[2];
	{
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		CHAOS_IL2CPP_INT32 chaos_callvirt_result{};
		chaos_callvirt_result = (*reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)>(chaos_vtable_resolve(chaos_object_get_type_info(reinterpret_cast<void*>(chaos_arg_0))->vtable_array, 1u)))(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_callvirt_result);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	chaos_locals[3] = _s0;
	_s0 = chaos_locals[3];
	chaos_locals[4] = _s0;
	_s0 = chaos_locals[4];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: CcwBasicSubjects/CcwBasicNativeEntry::TestSimpleMathAdd()
extern "C" CHAOS_IL2CPP_INT32 CcwBasicSubjects_CcwBasicNativeEntry_TestSimpleMathAdd(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_CcwBasicSubjects_SimpleMath, {});
		chaos_object->header.type_info = &chaos_mt_CcwBasicSubjects_SimpleMath.hot;
		CcwBasicSubjects_SimpleMath__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(10);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
	{
		const auto chaos_raw_arg_2 = _s2;
		const auto chaos_arg_2 = chaos_raw_arg_2;
		const auto chaos_raw_arg_1 = _s1;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		CHAOS_IL2CPP_INT32 chaos_callvirt_result{};
		chaos_callvirt_result = (*reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2)>(chaos_vtable_resolve(chaos_object_get_type_info(reinterpret_cast<void*>(chaos_arg_0))->vtable_array, 2u)))(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2));
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_callvirt_result);
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: CcwBasicSubjects/CcwBasicNativeEntry::TestSimpleMathMul()
extern "C" CHAOS_IL2CPP_INT32 CcwBasicSubjects_CcwBasicNativeEntry_TestSimpleMathMul(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_CcwBasicSubjects_SimpleMath, {});
		chaos_object->header.type_info = &chaos_mt_CcwBasicSubjects_SimpleMath.hot;
		CcwBasicSubjects_SimpleMath__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
	{
		const auto chaos_raw_arg_2 = _s2;
		const auto chaos_arg_2 = chaos_raw_arg_2;
		const auto chaos_raw_arg_1 = _s1;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		CHAOS_IL2CPP_INT32 chaos_callvirt_result{};
		chaos_callvirt_result = (*reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2)>(chaos_vtable_resolve(chaos_object_get_type_info(reinterpret_cast<void*>(chaos_arg_0))->vtable_array, 3u)))(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2));
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_callvirt_result);
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: CcwBasicSubjects/ConstantFortyTwo::.ctor()
extern "C" void CcwBasicSubjects_ConstantFortyTwo__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(chaos_arg_0);
	}
	return;
}

// Managed method: CcwBasicSubjects/ConstantFortyTwo::GetValue()
extern "C" CHAOS_IL2CPP_INT32 CcwBasicSubjects_ConstantFortyTwo_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: CcwBasicSubjects/DualInterfaceImpl::.ctor()
extern "C" void CcwBasicSubjects_DualInterfaceImpl__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(chaos_arg_0);
	}
	return;
}

// Managed method: CcwBasicSubjects/DualInterfaceImpl::Add(System.Int32,System.Int32)
extern "C" CHAOS_IL2CPP_INT32 CcwBasicSubjects_DualInterfaceImpl_Add_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);
	chaos_args[2] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_2);

	_s0 = chaos_args[1];
	_s1 = chaos_args[2];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(10);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: CcwBasicSubjects/DualInterfaceImpl::GetValue()
extern "C" CHAOS_IL2CPP_INT32 CcwBasicSubjects_DualInterfaceImpl_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(100);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: CcwBasicSubjects/DualInterfaceImpl::Multiply(System.Int32,System.Int32)
extern "C" CHAOS_IL2CPP_INT32 CcwBasicSubjects_DualInterfaceImpl_Multiply_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);
	chaos_args[2] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_2);

	_s0 = chaos_args[1];
	_s1 = chaos_args[2];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapMul(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapMul(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: CcwBasicSubjects/SimpleMath::.ctor()
extern "C" void CcwBasicSubjects_SimpleMath__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(chaos_arg_0);
	}
	return;
}

// Managed method: CcwBasicSubjects/SimpleMath::Add(System.Int32,System.Int32)
extern "C" CHAOS_IL2CPP_INT32 CcwBasicSubjects_SimpleMath_Add_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);
	chaos_args[2] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_2);

	_s0 = chaos_args[1];
	_s1 = chaos_args[2];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: CcwBasicSubjects/SimpleMath::Multiply(System.Int32,System.Int32)
extern "C" CHAOS_IL2CPP_INT32 CcwBasicSubjects_SimpleMath_Multiply_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);
	chaos_args[2] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_2);

	_s0 = chaos_args[1];
	_s1 = chaos_args[2];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapMul(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}



}  // namespace chaos::il2cpp::codegen::CcwBasicSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 19;

// ── JIT Method Entry Table ───────────────────────────────────────────
// Auto-generated by chaos-il2cpp codegen for --mode jit.
#include <cstdint>
#include "jit_registration.h"

static const char kMethodJson_0[] = "{\"methodId\":\"ccw-basic-subjects.ccw-basic-subjects.subject-0\",\"subjectId\":\"CcwBasicSubjects/CcwBasicSubjects::Subject_0:System.Void()\",\"signature\":\"System.Void CcwBasicSubjects::Subject_0()\",\"identity\":{\"assemblyName\":\"CcwBasicSubjects\",\"declaringTypeSubjectId\":\"CcwBasicSubjects/CcwBasicSubjects\",\"definitionSubjectId\":\"CcwBasicSubjects/CcwBasicSubjects::Subject_0:System.Void()\",\"subjectId\":\"CcwBasicSubjects/CcwBasicSubjects::Subject_0:System.Void()\",\"methodId\":\"ccw-basic-subjects.ccw-basic-subjects.subject-0\",\"signature\":\"System.Void CcwBasicSubjects::Subject_0()\"},\"nativeSymbol\":\"CcwBasicSubjects_CcwBasicSubjects_Subject_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":33,\"handlerOffset\":34,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":4,\"resultType\":\"System.Int32\",\"callee\":\"CcwBasicSubjects/CcwBasicNativeEntry::Run:System.Int32(System.Int32)\",\"reference\":{\"assemblyName\":\"CcwBasicSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry::Run:System.Int32(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"CcwBasicSubjects\",\"subjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry::Run:System.Int32(System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"CcwBasicSubjects_CcwBasicNativeEntry_Run_System_Int32\",\"targetParameterCount\":1,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":11,\"resultType\":\"System.Int32\",\"callee\":\"CcwBasicSubjects/CcwBasicNativeEntry::Run:System.Int32(System.Int32)\",\"reference\":{\"assemblyName\":\"CcwBasicSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry::Run:System.Int32(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"CcwBasicSubjects\",\"subjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry::Run:System.Int32(System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"CcwBasicSubjects_CcwBasicNativeEntry_Run_System_Int32\",\"targetParameterCount\":1,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":16,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":19,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":22,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":31,\"ilOffset\":23,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"CcwBasicSubjects/CcwBasicSubjects::_exitCode\",\"ilOffset\":26,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"CcwBasicSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"CcwBasicSubjects/CcwBasicSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"CcwBasicSubjects\",\"subjectId\":\"CcwBasicSubjects/CcwBasicSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"CcwBasicSubjects/CcwBasicSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":45,\"ilOffset\":32,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":34,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":36,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"CcwBasicSubjects/CcwBasicSubjects::_exitCode\",\"ilOffset\":37,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"CcwBasicSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"CcwBasicSubjects/CcwBasicSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"CcwBasicSubjects\",\"subjectId\":\"CcwBasicSubjects/CcwBasicSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"CcwBasicSubjects/CcwBasicSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":45,\"ilOffset\":43,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":45,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CcwBasicSubjects\"}";
static const char kMethodJson_1[] = "{\"methodId\":\"ccw-basic-subjects.ccw-basic-subjects.subject-1\",\"subjectId\":\"CcwBasicSubjects/CcwBasicSubjects::Subject_1:System.Void()\",\"signature\":\"System.Void CcwBasicSubjects::Subject_1()\",\"identity\":{\"assemblyName\":\"CcwBasicSubjects\",\"declaringTypeSubjectId\":\"CcwBasicSubjects/CcwBasicSubjects\",\"definitionSubjectId\":\"CcwBasicSubjects/CcwBasicSubjects::Subject_1:System.Void()\",\"subjectId\":\"CcwBasicSubjects/CcwBasicSubjects::Subject_1:System.Void()\",\"methodId\":\"ccw-basic-subjects.ccw-basic-subjects.subject-1\",\"signature\":\"System.Void CcwBasicSubjects::Subject_1()\"},\"nativeSymbol\":\"CcwBasicSubjects_CcwBasicSubjects_Subject_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":29,\"handlerOffset\":30,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"call\",\"opCode\":14,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"callee\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestSimpleMathAdd:System.Int32()\",\"reference\":{\"assemblyName\":\"CcwBasicSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestSimpleMathAdd:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"CcwBasicSubjects\",\"subjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestSimpleMathAdd:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"CcwBasicSubjects_CcwBasicNativeEntry_TestSimpleMathAdd\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"callee\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestSimpleMathAdd:System.Int32()\",\"reference\":{\"assemblyName\":\"CcwBasicSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestSimpleMathAdd:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"CcwBasicSubjects\",\"subjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestSimpleMathAdd:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"CcwBasicSubjects_CcwBasicNativeEntry_TestSimpleMathAdd\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":17,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":27,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"CcwBasicSubjects/CcwBasicSubjects::_exitCode\",\"ilOffset\":22,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"CcwBasicSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"CcwBasicSubjects/CcwBasicSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"CcwBasicSubjects\",\"subjectId\":\"CcwBasicSubjects/CcwBasicSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"CcwBasicSubjects/CcwBasicSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"CcwBasicSubjects/CcwBasicSubjects::_exitCode\",\"ilOffset\":33,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"CcwBasicSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"CcwBasicSubjects/CcwBasicSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"CcwBasicSubjects\",\"subjectId\":\"CcwBasicSubjects/CcwBasicSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"CcwBasicSubjects/CcwBasicSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":41,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CcwBasicSubjects\"}";
static const char kMethodJson_2[] = "{\"methodId\":\"ccw-basic-subjects.ccw-basic-subjects.subject-2\",\"subjectId\":\"CcwBasicSubjects/CcwBasicSubjects::Subject_2:System.Void()\",\"signature\":\"System.Void CcwBasicSubjects::Subject_2()\",\"identity\":{\"assemblyName\":\"CcwBasicSubjects\",\"declaringTypeSubjectId\":\"CcwBasicSubjects/CcwBasicSubjects\",\"definitionSubjectId\":\"CcwBasicSubjects/CcwBasicSubjects::Subject_2:System.Void()\",\"subjectId\":\"CcwBasicSubjects/CcwBasicSubjects::Subject_2:System.Void()\",\"methodId\":\"ccw-basic-subjects.ccw-basic-subjects.subject-2\",\"signature\":\"System.Void CcwBasicSubjects::Subject_2()\"},\"nativeSymbol\":\"CcwBasicSubjects_CcwBasicSubjects_Subject_2\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":29,\"handlerOffset\":30,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"call\",\"opCode\":14,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"callee\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestSimpleMathMul:System.Int32()\",\"reference\":{\"assemblyName\":\"CcwBasicSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestSimpleMathMul:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"CcwBasicSubjects\",\"subjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestSimpleMathMul:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"CcwBasicSubjects_CcwBasicNativeEntry_TestSimpleMathMul\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"callee\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestSimpleMathMul:System.Int32()\",\"reference\":{\"assemblyName\":\"CcwBasicSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestSimpleMathMul:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"CcwBasicSubjects\",\"subjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestSimpleMathMul:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"CcwBasicSubjects_CcwBasicNativeEntry_TestSimpleMathMul\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":17,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":27,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"CcwBasicSubjects/CcwBasicSubjects::_exitCode\",\"ilOffset\":22,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"CcwBasicSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"CcwBasicSubjects/CcwBasicSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"CcwBasicSubjects\",\"subjectId\":\"CcwBasicSubjects/CcwBasicSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"CcwBasicSubjects/CcwBasicSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"CcwBasicSubjects/CcwBasicSubjects::_exitCode\",\"ilOffset\":33,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"CcwBasicSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"CcwBasicSubjects/CcwBasicSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"CcwBasicSubjects\",\"subjectId\":\"CcwBasicSubjects/CcwBasicSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"CcwBasicSubjects/CcwBasicSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":41,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CcwBasicSubjects\"}";
static const char kMethodJson_3[] = "{\"methodId\":\"ccw-basic-subjects.ccw-basic-subjects.subject-3\",\"subjectId\":\"CcwBasicSubjects/CcwBasicSubjects::Subject_3:System.Void()\",\"signature\":\"System.Void CcwBasicSubjects::Subject_3()\",\"identity\":{\"assemblyName\":\"CcwBasicSubjects\",\"declaringTypeSubjectId\":\"CcwBasicSubjects/CcwBasicSubjects\",\"definitionSubjectId\":\"CcwBasicSubjects/CcwBasicSubjects::Subject_3:System.Void()\",\"subjectId\":\"CcwBasicSubjects/CcwBasicSubjects::Subject_3:System.Void()\",\"methodId\":\"ccw-basic-subjects.ccw-basic-subjects.subject-3\",\"signature\":\"System.Void CcwBasicSubjects::Subject_3()\"},\"nativeSymbol\":\"CcwBasicSubjects_CcwBasicSubjects_Subject_3\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":29,\"handlerOffset\":30,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"call\",\"opCode\":14,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"callee\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestConstantValue:System.Int32()\",\"reference\":{\"assemblyName\":\"CcwBasicSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestConstantValue:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"CcwBasicSubjects\",\"subjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestConstantValue:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"CcwBasicSubjects_CcwBasicNativeEntry_TestConstantValue\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"callee\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestConstantValue:System.Int32()\",\"reference\":{\"assemblyName\":\"CcwBasicSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestConstantValue:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"CcwBasicSubjects\",\"subjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestConstantValue:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"CcwBasicSubjects_CcwBasicNativeEntry_TestConstantValue\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":17,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":27,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"CcwBasicSubjects/CcwBasicSubjects::_exitCode\",\"ilOffset\":22,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"CcwBasicSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"CcwBasicSubjects/CcwBasicSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"CcwBasicSubjects\",\"subjectId\":\"CcwBasicSubjects/CcwBasicSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"CcwBasicSubjects/CcwBasicSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"CcwBasicSubjects/CcwBasicSubjects::_exitCode\",\"ilOffset\":33,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"CcwBasicSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"CcwBasicSubjects/CcwBasicSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"CcwBasicSubjects\",\"subjectId\":\"CcwBasicSubjects/CcwBasicSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"CcwBasicSubjects/CcwBasicSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":41,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CcwBasicSubjects\"}";
static const char kMethodJson_4[] = "{\"methodId\":\"ccw-basic-subjects.ccw-basic-subjects.subject-4\",\"subjectId\":\"CcwBasicSubjects/CcwBasicSubjects::Subject_4:System.Void()\",\"signature\":\"System.Void CcwBasicSubjects::Subject_4()\",\"identity\":{\"assemblyName\":\"CcwBasicSubjects\",\"declaringTypeSubjectId\":\"CcwBasicSubjects/CcwBasicSubjects\",\"definitionSubjectId\":\"CcwBasicSubjects/CcwBasicSubjects::Subject_4:System.Void()\",\"subjectId\":\"CcwBasicSubjects/CcwBasicSubjects::Subject_4:System.Void()\",\"methodId\":\"ccw-basic-subjects.ccw-basic-subjects.subject-4\",\"signature\":\"System.Void CcwBasicSubjects::Subject_4()\"},\"nativeSymbol\":\"CcwBasicSubjects_CcwBasicSubjects_Subject_4\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":29,\"handlerOffset\":30,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"call\",\"opCode\":14,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"callee\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestDualInterface:System.Int32()\",\"reference\":{\"assemblyName\":\"CcwBasicSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestDualInterface:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"CcwBasicSubjects\",\"subjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestDualInterface:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"CcwBasicSubjects_CcwBasicNativeEntry_TestDualInterface\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"callee\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestDualInterface:System.Int32()\",\"reference\":{\"assemblyName\":\"CcwBasicSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestDualInterface:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"CcwBasicSubjects\",\"subjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestDualInterface:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"CcwBasicSubjects_CcwBasicNativeEntry_TestDualInterface\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":17,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":27,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"CcwBasicSubjects/CcwBasicSubjects::_exitCode\",\"ilOffset\":22,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"CcwBasicSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"CcwBasicSubjects/CcwBasicSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"CcwBasicSubjects\",\"subjectId\":\"CcwBasicSubjects/CcwBasicSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"CcwBasicSubjects/CcwBasicSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"CcwBasicSubjects/CcwBasicSubjects::_exitCode\",\"ilOffset\":33,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"CcwBasicSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"CcwBasicSubjects/CcwBasicSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"CcwBasicSubjects\",\"subjectId\":\"CcwBasicSubjects/CcwBasicSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"CcwBasicSubjects/CcwBasicSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":41,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CcwBasicSubjects\"}";
static const char kMethodJson_5[] = "{\"methodId\":\"ccw-basic-subjects.ccw-basic-native-entry.run\",\"subjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry::Run:System.Int32(System.Int32)\",\"signature\":\"System.Int32 CcwBasicNativeEntry::Run(System.Int32)\",\"identity\":{\"assemblyName\":\"CcwBasicSubjects\",\"declaringTypeSubjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry\",\"definitionSubjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry::Run:System.Int32(System.Int32)\",\"subjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry::Run:System.Int32(System.Int32)\",\"methodId\":\"ccw-basic-subjects.ccw-basic-native-entry.run\",\"signature\":\"System.Int32 CcwBasicNativeEntry::Run(System.Int32)\"},\"nativeSymbol\":\"CcwBasicSubjects_CcwBasicNativeEntry_Run_System_Int32\",\"isStatic\":true,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":1,\"parameterAbis\":[{\"carrierKindCode\":1,\"typeShape\":2}],\"localCount\":3,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":2,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":3,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":5,\"isPreserveSig\":false},{\"op\":\"switch\",\"opCode\":69,\"operand\":[29,37,45,53],\"ilOffset\":6,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":61,\"ilOffset\":27,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"callee\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestSimpleMathAdd:System.Int32()\",\"reference\":{\"assemblyName\":\"CcwBasicSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestSimpleMathAdd:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"CcwBasicSubjects\",\"subjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestSimpleMathAdd:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"CcwBasicSubjects_CcwBasicNativeEntry_TestSimpleMathAdd\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":34,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":65,\"ilOffset\":35,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":37,\"resultType\":\"System.Int32\",\"callee\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestSimpleMathMul:System.Int32()\",\"reference\":{\"assemblyName\":\"CcwBasicSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestSimpleMathMul:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"CcwBasicSubjects\",\"subjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestSimpleMathMul:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"CcwBasicSubjects_CcwBasicNativeEntry_TestSimpleMathMul\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":42,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":65,\"ilOffset\":43,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":45,\"resultType\":\"System.Int32\",\"callee\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestConstantValue:System.Int32()\",\"reference\":{\"assemblyName\":\"CcwBasicSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestConstantValue:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"CcwBasicSubjects\",\"subjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestConstantValue:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"CcwBasicSubjects_CcwBasicNativeEntry_TestConstantValue\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":50,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":65,\"ilOffset\":51,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":53,\"resultType\":\"System.Int32\",\"callee\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestDualInterface:System.Int32()\",\"reference\":{\"assemblyName\":\"CcwBasicSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestDualInterface:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"CcwBasicSubjects\",\"subjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestDualInterface:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"CcwBasicSubjects_CcwBasicNativeEntry_TestDualInterface\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":58,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":65,\"ilOffset\":59,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":-1,\"ilOffset\":61,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":62,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":65,\"ilOffset\":63,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":65,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":66,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CcwBasicSubjects\"}";
static const char kMethodJson_6[] = "{\"methodId\":\"ccw-basic-subjects.ccw-basic-native-entry.test-constant-value\",\"subjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestConstantValue:System.Int32()\",\"signature\":\"System.Int32 CcwBasicNativeEntry::TestConstantValue()\",\"identity\":{\"assemblyName\":\"CcwBasicSubjects\",\"declaringTypeSubjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry\",\"definitionSubjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestConstantValue:System.Int32()\",\"subjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestConstantValue:System.Int32()\",\"methodId\":\"ccw-basic-subjects.ccw-basic-native-entry.test-constant-value\",\"signature\":\"System.Int32 CcwBasicNativeEntry::TestConstantValue()\"},\"nativeSymbol\":\"CcwBasicSubjects_CcwBasicNativeEntry_TestConstantValue\",\"isStatic\":true,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":1,\"resultType\":\"CcwBasicSubjects/ConstantFortyTwo\",\"callee\":\"CcwBasicSubjects/ConstantFortyTwo::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"CcwBasicSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"CcwBasicSubjects/ConstantFortyTwo::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"CcwBasicSubjects\",\"subjectId\":\"CcwBasicSubjects/ConstantFortyTwo\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"baseTypeSubjectId\":\"System.Private.CoreLib/System.Object\",\"implementedInterfaceSubjectIds\":[\"CcwBasicSubjects/IConstantValue\"],\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":6,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"callee\":\"CcwBasicSubjects/IConstantValue::GetValue:System.Int32()\",\"reference\":{\"assemblyName\":\"CcwBasicSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"CcwBasicSubjects/IConstantValue::GetValue:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"CcwBasicSubjects\",\"subjectId\":\"CcwBasicSubjects/IConstantValue::GetValue:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"CcwBasicSubjects_IConstantValue_GetValue\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":2,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":16,\"ilOffset\":14,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":16,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":17,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CcwBasicSubjects\"}";
static const char kMethodJson_7[] = "{\"methodId\":\"ccw-basic-subjects.ccw-basic-native-entry.test-dual-interface\",\"subjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestDualInterface:System.Int32()\",\"signature\":\"System.Int32 CcwBasicNativeEntry::TestDualInterface()\",\"identity\":{\"assemblyName\":\"CcwBasicSubjects\",\"declaringTypeSubjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry\",\"definitionSubjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestDualInterface:System.Int32()\",\"subjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestDualInterface:System.Int32()\",\"methodId\":\"ccw-basic-subjects.ccw-basic-native-entry.test-dual-interface\",\"signature\":\"System.Int32 CcwBasicNativeEntry::TestDualInterface()\"},\"nativeSymbol\":\"CcwBasicSubjects_CcwBasicNativeEntry_TestDualInterface\",\"isStatic\":true,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":5,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":1,\"resultType\":\"CcwBasicSubjects/DualInterfaceImpl\",\"callee\":\"CcwBasicSubjects/DualInterfaceImpl::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"CcwBasicSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"CcwBasicSubjects/DualInterfaceImpl::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"CcwBasicSubjects\",\"subjectId\":\"CcwBasicSubjects/DualInterfaceImpl\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"baseTypeSubjectId\":\"System.Private.CoreLib/System.Object\",\"implementedInterfaceSubjectIds\":[\"CcwBasicSubjects/IConstantValue\",\"CcwBasicSubjects/ISimpleMath\"],\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":6,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":8,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":9,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":10,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":11,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":10,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":5,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"callee\":\"CcwBasicSubjects/ISimpleMath::Add:System.Int32(System.Int32,System.Int32)\",\"reference\":{\"assemblyName\":\"CcwBasicSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"CcwBasicSubjects/ISimpleMath::Add:System.Int32(System.Int32,System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"CcwBasicSubjects\",\"subjectId\":\"CcwBasicSubjects/ISimpleMath::Add:System.Int32(System.Int32,System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"CcwBasicSubjects_ISimpleMath_Add_System_Int32_System_Int32\",\"targetParameterCount\":2,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":2,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":20,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"callee\":\"CcwBasicSubjects/IConstantValue::GetValue:System.Int32()\",\"reference\":{\"assemblyName\":\"CcwBasicSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"CcwBasicSubjects/IConstantValue::GetValue:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"CcwBasicSubjects\",\"subjectId\":\"CcwBasicSubjects/IConstantValue::GetValue:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"CcwBasicSubjects_IConstantValue_GetValue\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":2,\"isPreserveSig\":false},{\"op\":\"add\",\"opCode\":25,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":3,\"ilOffset\":27,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":3,\"ilOffset\":28,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":4,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":33,\"ilOffset\":31,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":4,\"ilOffset\":33,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":35,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CcwBasicSubjects\"}";
static const char kMethodJson_8[] = "{\"methodId\":\"ccw-basic-subjects.ccw-basic-native-entry.test-simple-math-add\",\"subjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestSimpleMathAdd:System.Int32()\",\"signature\":\"System.Int32 CcwBasicNativeEntry::TestSimpleMathAdd()\",\"identity\":{\"assemblyName\":\"CcwBasicSubjects\",\"declaringTypeSubjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry\",\"definitionSubjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestSimpleMathAdd:System.Int32()\",\"subjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestSimpleMathAdd:System.Int32()\",\"methodId\":\"ccw-basic-subjects.ccw-basic-native-entry.test-simple-math-add\",\"signature\":\"System.Int32 CcwBasicNativeEntry::TestSimpleMathAdd()\"},\"nativeSymbol\":\"CcwBasicSubjects_CcwBasicNativeEntry_TestSimpleMathAdd\",\"isStatic\":true,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":1,\"resultType\":\"CcwBasicSubjects/SimpleMath\",\"callee\":\"CcwBasicSubjects/SimpleMath::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"CcwBasicSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"CcwBasicSubjects/SimpleMath::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"CcwBasicSubjects\",\"subjectId\":\"CcwBasicSubjects/SimpleMath\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"baseTypeSubjectId\":\"System.Private.CoreLib/System.Object\",\"implementedInterfaceSubjectIds\":[\"CcwBasicSubjects/ISimpleMath\"],\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":6,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":10,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":5,\"ilOffset\":10,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":11,\"resultType\":\"System.Int32\",\"callee\":\"CcwBasicSubjects/ISimpleMath::Add:System.Int32(System.Int32,System.Int32)\",\"reference\":{\"assemblyName\":\"CcwBasicSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"CcwBasicSubjects/ISimpleMath::Add:System.Int32(System.Int32,System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"CcwBasicSubjects\",\"subjectId\":\"CcwBasicSubjects/ISimpleMath::Add:System.Int32(System.Int32,System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"CcwBasicSubjects_ISimpleMath_Add_System_Int32_System_Int32\",\"targetParameterCount\":2,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":2,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":19,\"ilOffset\":17,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":19,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":20,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CcwBasicSubjects\"}";
static const char kMethodJson_9[] = "{\"methodId\":\"ccw-basic-subjects.ccw-basic-native-entry.test-simple-math-mul\",\"subjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestSimpleMathMul:System.Int32()\",\"signature\":\"System.Int32 CcwBasicNativeEntry::TestSimpleMathMul()\",\"identity\":{\"assemblyName\":\"CcwBasicSubjects\",\"declaringTypeSubjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry\",\"definitionSubjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestSimpleMathMul:System.Int32()\",\"subjectId\":\"CcwBasicSubjects/CcwBasicNativeEntry::TestSimpleMathMul:System.Int32()\",\"methodId\":\"ccw-basic-subjects.ccw-basic-native-entry.test-simple-math-mul\",\"signature\":\"System.Int32 CcwBasicNativeEntry::TestSimpleMathMul()\"},\"nativeSymbol\":\"CcwBasicSubjects_CcwBasicNativeEntry_TestSimpleMathMul\",\"isStatic\":true,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":1,\"resultType\":\"CcwBasicSubjects/SimpleMath\",\"callee\":\"CcwBasicSubjects/SimpleMath::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"CcwBasicSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"CcwBasicSubjects/SimpleMath::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"CcwBasicSubjects\",\"subjectId\":\"CcwBasicSubjects/SimpleMath\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"baseTypeSubjectId\":\"System.Private.CoreLib/System.Object\",\"implementedInterfaceSubjectIds\":[\"CcwBasicSubjects/ISimpleMath\"],\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":6,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":3,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":4,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":10,\"resultType\":\"System.Int32\",\"callee\":\"CcwBasicSubjects/ISimpleMath::Multiply:System.Int32(System.Int32,System.Int32)\",\"reference\":{\"assemblyName\":\"CcwBasicSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"CcwBasicSubjects/ISimpleMath::Multiply:System.Int32(System.Int32,System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"CcwBasicSubjects\",\"subjectId\":\"CcwBasicSubjects/ISimpleMath::Multiply:System.Int32(System.Int32,System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"CcwBasicSubjects_ISimpleMath_Multiply_System_Int32_System_Int32\",\"targetParameterCount\":2,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":2,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":15,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":18,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":19,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CcwBasicSubjects\"}";
static const char kMethodJson_10[] = "{\"methodId\":\"ccw-basic-subjects.constant-forty-two.ctor\",\"subjectId\":\"CcwBasicSubjects/ConstantFortyTwo::.ctor:System.Void()\",\"signature\":\"System.Void ConstantFortyTwo::.ctor()\",\"identity\":{\"assemblyName\":\"CcwBasicSubjects\",\"declaringTypeSubjectId\":\"CcwBasicSubjects/ConstantFortyTwo\",\"definitionSubjectId\":\"CcwBasicSubjects/ConstantFortyTwo::.ctor:System.Void()\",\"subjectId\":\"CcwBasicSubjects/ConstantFortyTwo::.ctor:System.Void()\",\"methodId\":\"ccw-basic-subjects.constant-forty-two.ctor\",\"signature\":\"System.Void ConstantFortyTwo::.ctor()\"},\"nativeSymbol\":\"CcwBasicSubjects_ConstantFortyTwo__ctor\",\"isStatic\":false,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"CcwBasicSubjects/ConstantFortyTwo\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":7,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CcwBasicSubjects\"}";
static const char kMethodJson_11[] = "{\"methodId\":\"ccw-basic-subjects.constant-forty-two.get-value\",\"subjectId\":\"CcwBasicSubjects/ConstantFortyTwo::GetValue:System.Int32()\",\"signature\":\"System.Int32 ConstantFortyTwo::GetValue()\",\"identity\":{\"assemblyName\":\"CcwBasicSubjects\",\"declaringTypeSubjectId\":\"CcwBasicSubjects/ConstantFortyTwo\",\"definitionSubjectId\":\"CcwBasicSubjects/ConstantFortyTwo::GetValue:System.Int32()\",\"subjectId\":\"CcwBasicSubjects/ConstantFortyTwo::GetValue:System.Int32()\",\"methodId\":\"ccw-basic-subjects.constant-forty-two.get-value\",\"signature\":\"System.Int32 ConstantFortyTwo::GetValue()\"},\"nativeSymbol\":\"CcwBasicSubjects_ConstantFortyTwo_GetValue\",\"isStatic\":false,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":0,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":2,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CcwBasicSubjects\"}";
static const char kMethodJson_12[] = "{\"methodId\":\"ccw-basic-subjects.dual-interface-impl.ctor\",\"subjectId\":\"CcwBasicSubjects/DualInterfaceImpl::.ctor:System.Void()\",\"signature\":\"System.Void DualInterfaceImpl::.ctor()\",\"identity\":{\"assemblyName\":\"CcwBasicSubjects\",\"declaringTypeSubjectId\":\"CcwBasicSubjects/DualInterfaceImpl\",\"definitionSubjectId\":\"CcwBasicSubjects/DualInterfaceImpl::.ctor:System.Void()\",\"subjectId\":\"CcwBasicSubjects/DualInterfaceImpl::.ctor:System.Void()\",\"methodId\":\"ccw-basic-subjects.dual-interface-impl.ctor\",\"signature\":\"System.Void DualInterfaceImpl::.ctor()\"},\"nativeSymbol\":\"CcwBasicSubjects_DualInterfaceImpl__ctor\",\"isStatic\":false,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"CcwBasicSubjects/DualInterfaceImpl\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":7,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CcwBasicSubjects\"}";
static const char kMethodJson_13[] = "{\"methodId\":\"ccw-basic-subjects.dual-interface-impl.add\",\"subjectId\":\"CcwBasicSubjects/DualInterfaceImpl::Add:System.Int32(System.Int32,System.Int32)\",\"signature\":\"System.Int32 DualInterfaceImpl::Add(System.Int32,System.Int32)\",\"identity\":{\"assemblyName\":\"CcwBasicSubjects\",\"declaringTypeSubjectId\":\"CcwBasicSubjects/DualInterfaceImpl\",\"definitionSubjectId\":\"CcwBasicSubjects/DualInterfaceImpl::Add:System.Int32(System.Int32,System.Int32)\",\"subjectId\":\"CcwBasicSubjects/DualInterfaceImpl::Add:System.Int32(System.Int32,System.Int32)\",\"methodId\":\"ccw-basic-subjects.dual-interface-impl.add\",\"signature\":\"System.Int32 DualInterfaceImpl::Add(System.Int32,System.Int32)\"},\"nativeSymbol\":\"CcwBasicSubjects_DualInterfaceImpl_Add_System_Int32_System_Int32\",\"isStatic\":false,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":2,\"parameterAbis\":[{\"carrierKindCode\":1,\"typeShape\":2},{\"carrierKindCode\":1,\"typeShape\":2}],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":0,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":2,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"add\",\"opCode\":25,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":10,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"add\",\"opCode\":25,\"ilOffset\":5,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":6,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CcwBasicSubjects\"}";
static const char kMethodJson_14[] = "{\"methodId\":\"ccw-basic-subjects.dual-interface-impl.get-value\",\"subjectId\":\"CcwBasicSubjects/DualInterfaceImpl::GetValue:System.Int32()\",\"signature\":\"System.Int32 DualInterfaceImpl::GetValue()\",\"identity\":{\"assemblyName\":\"CcwBasicSubjects\",\"declaringTypeSubjectId\":\"CcwBasicSubjects/DualInterfaceImpl\",\"definitionSubjectId\":\"CcwBasicSubjects/DualInterfaceImpl::GetValue:System.Int32()\",\"subjectId\":\"CcwBasicSubjects/DualInterfaceImpl::GetValue:System.Int32()\",\"methodId\":\"ccw-basic-subjects.dual-interface-impl.get-value\",\"signature\":\"System.Int32 DualInterfaceImpl::GetValue()\"},\"nativeSymbol\":\"CcwBasicSubjects_DualInterfaceImpl_GetValue\",\"isStatic\":false,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":100,\"ilOffset\":0,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":2,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CcwBasicSubjects\"}";
static const char kMethodJson_15[] = "{\"methodId\":\"ccw-basic-subjects.dual-interface-impl.multiply\",\"subjectId\":\"CcwBasicSubjects/DualInterfaceImpl::Multiply:System.Int32(System.Int32,System.Int32)\",\"signature\":\"System.Int32 DualInterfaceImpl::Multiply(System.Int32,System.Int32)\",\"identity\":{\"assemblyName\":\"CcwBasicSubjects\",\"declaringTypeSubjectId\":\"CcwBasicSubjects/DualInterfaceImpl\",\"definitionSubjectId\":\"CcwBasicSubjects/DualInterfaceImpl::Multiply:System.Int32(System.Int32,System.Int32)\",\"subjectId\":\"CcwBasicSubjects/DualInterfaceImpl::Multiply:System.Int32(System.Int32,System.Int32)\",\"methodId\":\"ccw-basic-subjects.dual-interface-impl.multiply\",\"signature\":\"System.Int32 DualInterfaceImpl::Multiply(System.Int32,System.Int32)\"},\"nativeSymbol\":\"CcwBasicSubjects_DualInterfaceImpl_Multiply_System_Int32_System_Int32\",\"isStatic\":false,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":2,\"parameterAbis\":[{\"carrierKindCode\":1,\"typeShape\":2},{\"carrierKindCode\":1,\"typeShape\":2}],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":0,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":2,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"mul\",\"opCode\":27,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":2,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"mul\",\"opCode\":27,\"ilOffset\":4,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":5,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CcwBasicSubjects\"}";
static const char kMethodJson_16[] = "{\"methodId\":\"ccw-basic-subjects.simple-math.ctor\",\"subjectId\":\"CcwBasicSubjects/SimpleMath::.ctor:System.Void()\",\"signature\":\"System.Void SimpleMath::.ctor()\",\"identity\":{\"assemblyName\":\"CcwBasicSubjects\",\"declaringTypeSubjectId\":\"CcwBasicSubjects/SimpleMath\",\"definitionSubjectId\":\"CcwBasicSubjects/SimpleMath::.ctor:System.Void()\",\"subjectId\":\"CcwBasicSubjects/SimpleMath::.ctor:System.Void()\",\"methodId\":\"ccw-basic-subjects.simple-math.ctor\",\"signature\":\"System.Void SimpleMath::.ctor()\"},\"nativeSymbol\":\"CcwBasicSubjects_SimpleMath__ctor\",\"isStatic\":false,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"CcwBasicSubjects/SimpleMath\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":7,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CcwBasicSubjects\"}";
static const char kMethodJson_17[] = "{\"methodId\":\"ccw-basic-subjects.simple-math.add\",\"subjectId\":\"CcwBasicSubjects/SimpleMath::Add:System.Int32(System.Int32,System.Int32)\",\"signature\":\"System.Int32 SimpleMath::Add(System.Int32,System.Int32)\",\"identity\":{\"assemblyName\":\"CcwBasicSubjects\",\"declaringTypeSubjectId\":\"CcwBasicSubjects/SimpleMath\",\"definitionSubjectId\":\"CcwBasicSubjects/SimpleMath::Add:System.Int32(System.Int32,System.Int32)\",\"subjectId\":\"CcwBasicSubjects/SimpleMath::Add:System.Int32(System.Int32,System.Int32)\",\"methodId\":\"ccw-basic-subjects.simple-math.add\",\"signature\":\"System.Int32 SimpleMath::Add(System.Int32,System.Int32)\"},\"nativeSymbol\":\"CcwBasicSubjects_SimpleMath_Add_System_Int32_System_Int32\",\"isStatic\":false,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":2,\"parameterAbis\":[{\"carrierKindCode\":1,\"typeShape\":2},{\"carrierKindCode\":1,\"typeShape\":2}],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":0,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":2,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"add\",\"opCode\":25,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":3,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CcwBasicSubjects\"}";
static const char kMethodJson_18[] = "{\"methodId\":\"ccw-basic-subjects.simple-math.multiply\",\"subjectId\":\"CcwBasicSubjects/SimpleMath::Multiply:System.Int32(System.Int32,System.Int32)\",\"signature\":\"System.Int32 SimpleMath::Multiply(System.Int32,System.Int32)\",\"identity\":{\"assemblyName\":\"CcwBasicSubjects\",\"declaringTypeSubjectId\":\"CcwBasicSubjects/SimpleMath\",\"definitionSubjectId\":\"CcwBasicSubjects/SimpleMath::Multiply:System.Int32(System.Int32,System.Int32)\",\"subjectId\":\"CcwBasicSubjects/SimpleMath::Multiply:System.Int32(System.Int32,System.Int32)\",\"methodId\":\"ccw-basic-subjects.simple-math.multiply\",\"signature\":\"System.Int32 SimpleMath::Multiply(System.Int32,System.Int32)\"},\"nativeSymbol\":\"CcwBasicSubjects_SimpleMath_Multiply_System_Int32_System_Int32\",\"isStatic\":false,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":2,\"parameterAbis\":[{\"carrierKindCode\":1,\"typeShape\":2},{\"carrierKindCode\":1,\"typeShape\":2}],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":0,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":2,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"mul\",\"opCode\":27,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":3,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CcwBasicSubjects\"}";

extern "C" const JitMethodEntry kChaosJitMethodEntries[19] =
{
    { kMethodJson_0, 5322u, 0x00000009u, 0u },
    { kMethodJson_1, 5129u, 0x0000000Au, 0u },
    { kMethodJson_2, 5129u, 0x0000000Bu, 0u },
    { kMethodJson_3, 5129u, 0x0000000Cu, 0u },
    { kMethodJson_4, 5129u, 0x0000000Du, 0u },
    { kMethodJson_5, 6105u, 0x0000001Au, 0u },
    { kMethodJson_6, 3110u, 0x0000001Du, 0u },
    { kMethodJson_7, 4809u, 0x0000001Eu, 0u },
    { kMethodJson_8, 3361u, 0x0000001Bu, 0u },
    { kMethodJson_9, 3379u, 0x0000001Cu, 0u },
    { kMethodJson_10, 1835u, 0x00000015u, 0u },
    { kMethodJson_11, 1257u, 0x00000014u, 0u },
    { kMethodJson_12, 1845u, 0x00000019u, 0u },
    { kMethodJson_13, 1824u, 0x00000016u, 0u },
    { kMethodJson_14, 1267u, 0x00000018u, 0u },
    { kMethodJson_15, 1863u, 0x00000017u, 0u },
    { kMethodJson_16, 1773u, 0x00000013u, 0u },
    { kMethodJson_17, 1568u, 0x00000011u, 0u },
    { kMethodJson_18, 1608u, 0x00000012u, 0u }
};

extern "C" const uint32_t kChaosJitMethodEntryCount = 19u;

extern "C" void ChaosJitRegisterAll() {
    RegisterJitMethods(kChaosJitMethodEntries, kChaosJitMethodEntryCount);
}