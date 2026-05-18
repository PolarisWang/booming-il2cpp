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

namespace chaos::il2cpp::codegen::SnapshotProverSubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_SnapshotProverSubjects_IMyInterface = static_cast<CHAOS_IL2CPP_INTPTR>(1177377872687691913ULL);
const void* chaos_vtable_SnapshotProverSubjects_ImplHelper[];
const void* chaos_vtable_SnapshotProverSubjects_InstanceHelper[];
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Object = {{nullptr, nullptr, 15228727185366376748ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_INTPTR>(15228727185366376748ULL);static constexpr InterfaceMapEntry chaos_iface_map_SnapshotProverSubjects_ImplHelper[] = {
	{ chaos_type_id_SnapshotProverSubjects_IMyInterface, 1, 1 }
};
inline TypeInfoV0 chaos_mt_SnapshotProverSubjects_ImplHelper = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_SnapshotProverSubjects_ImplHelper, 15029137366836379383ULL, 2u, 32, 1, 1}, {chaos_iface_map_SnapshotProverSubjects_ImplHelper, nullptr, 1, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_SnapshotProverSubjects_ImplHelper = static_cast<CHAOS_IL2CPP_INTPTR>(15029137366836379383ULL);inline TypeInfoV0 chaos_mt_SnapshotProverSubjects_InstanceHelper = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_SnapshotProverSubjects_InstanceHelper, 16744285185866844682ULL, 2u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_SnapshotProverSubjects_InstanceHelper = static_cast<CHAOS_IL2CPP_INTPTR>(16744285185866844682ULL);inline TypeInfoV0 chaos_mt_SnapshotProverSubjects_LdftnHelper___O = {{nullptr, nullptr, 17279244380214704931ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_SnapshotProverSubjects_LdftnHelper___O = static_cast<CHAOS_IL2CPP_INTPTR>(17279244380214704931ULL);inline TypeInfoV0 chaos_mt_SnapshotProverSubjects_SnapshotProverSubjects = {{nullptr, nullptr, 17700440604878766952ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_SnapshotProverSubjects_SnapshotProverSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(17700440604878766952ULL);inline TypeInfoV0 chaos_mt_System_Collections_System_Collections_Generic_List_System_Int32_ = {{nullptr, nullptr, 8124737553590710378ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Collections_System_Collections_Generic_List_System_Int32_ = static_cast<CHAOS_IL2CPP_INTPTR>(8124737553590710378ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Delegate = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, nullptr, 7451128447593600616ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Delegate = static_cast<CHAOS_IL2CPP_INTPTR>(7451128447593600616ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_MulticastDelegate = {{&chaos_mt_System_Private_CoreLib_System_Delegate.hot, nullptr, 6681393039041505440ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_MulticastDelegate = static_cast<CHAOS_IL2CPP_INTPTR>(6681393039041505440ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Func_System_Int32_ = {{&chaos_mt_System_Private_CoreLib_System_MulticastDelegate.hot, nullptr, 8866194404114377402ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Func_System_Int32_ = static_cast<CHAOS_IL2CPP_INTPTR>(8866194404114377402ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_Assembly = {{nullptr, nullptr, 5474029880995115448ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_Assembly = static_cast<CHAOS_IL2CPP_INTPTR>(5474029880995115448ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName = {{nullptr, nullptr, 17082367815459723707ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_AssemblyName = static_cast<CHAOS_IL2CPP_INTPTR>(17082367815459723707ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo = {{nullptr, nullptr, 4137207361503509124ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_ConstructorInfo = static_cast<CHAOS_IL2CPP_INTPTR>(4137207361503509124ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo = {{nullptr, nullptr, 17040031516751226236ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_FieldInfo = static_cast<CHAOS_IL2CPP_INTPTR>(17040031516751226236ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo = {{nullptr, nullptr, 10748947813473285525ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_MethodInfo = static_cast<CHAOS_IL2CPP_INTPTR>(10748947813473285525ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_String = {{nullptr, nullptr, 1782325859292956794ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_String = static_cast<CHAOS_IL2CPP_INTPTR>(1782325859292956794ULL);inline TypeInfoV0 chaos_mt_SnapshotProverSubjects_IMyInterface = {{nullptr, nullptr, 1177377872687691913ULL, 0u, 32, 3, 0}, {nullptr, nullptr, 0, 0, 0, 0}};
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Int32 = {{nullptr, nullptr, 11009693519287992193ULL, 0u, 32, 2, 0}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_boxed_type_id_System_Private_CoreLib_System_Int32 = static_cast<CHAOS_IL2CPP_INTPTR>(11009693519287992193ULL);

// ── Virtual method table arrays ──
extern "C" void SnapshotProverSubjects_ImplHelper__ctor_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 SnapshotProverSubjects_ImplHelper_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
const void* chaos_vtable_SnapshotProverSubjects_ImplHelper[] =
{
	reinterpret_cast<void*>(SnapshotProverSubjects_ImplHelper__ctor_System_Int32),
	reinterpret_cast<void*>(SnapshotProverSubjects_ImplHelper_GetValue),
};
extern "C" void SnapshotProverSubjects_InstanceHelper__ctor_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 SnapshotProverSubjects_InstanceHelper_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
const void* chaos_vtable_SnapshotProverSubjects_InstanceHelper[] =
{
	reinterpret_cast<void*>(SnapshotProverSubjects_InstanceHelper__ctor_System_Int32),
	reinterpret_cast<void*>(SnapshotProverSubjects_InstanceHelper_GetValue),
};
static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_SnapshotProverSubjects_ImplHelper[] =
{
	{ 0x00000110u, reinterpret_cast<void*>(&SnapshotProverSubjects_ImplHelper__ctor_System_Int32) },
	{ 0x00000111u, reinterpret_cast<void*>(&SnapshotProverSubjects_ImplHelper_GetValue) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_SnapshotProverSubjects_InstanceHelper[] =
{
	{ 0x00000088u, reinterpret_cast<void*>(&SnapshotProverSubjects_InstanceHelper__ctor_System_Int32) },
	{ 0x00000089u, reinterpret_cast<void*>(&SnapshotProverSubjects_InstanceHelper_GetValue) },
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

struct chaos_type_System_Private_CoreLib_System_Object
{
	ThinLockableHeader header{};
};

struct chaos_type_SnapshotProverSubjects_ImplHelper : public chaos_type_System_Private_CoreLib_System_Object
{
	CHAOS_IL2CPP_INTPTR field_SnapshotProverSubjects_ImplHelper___val = 0;
};

struct chaos_type_SnapshotProverSubjects_InstanceHelper : public chaos_type_System_Private_CoreLib_System_Object
{
	CHAOS_IL2CPP_INTPTR field_SnapshotProverSubjects_InstanceHelper___val = 0;
};

struct chaos_type_SnapshotProverSubjects_LdftnHelper___O
{
	ThinLockableHeader header{};
};

struct chaos_type_SnapshotProverSubjects_SnapshotProverSubjects
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Collections_System_Collections_Generic_List_System_Int32_
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Private_CoreLib_System_Delegate : public chaos_type_System_Private_CoreLib_System_Object
{
	CHAOS_IL2CPP_INTPTR chaos_delegate_target = 0;
	CHAOS_IL2CPP_INTPTR chaos_delegate_method_ptr = 0;
	CHAOS_IL2CPP_INTPTR chaos_delegate_invocation_list = 0;
	CHAOS_IL2CPP_INTPTR chaos_delegate_invocation_count = 0;
	CHAOS_IL2CPP_UINT32 chaos_delegate_method_token = 0;
	CHAOS_IL2CPP_UINT32 _pad = 0;
};

struct chaos_type_System_Private_CoreLib_System_MulticastDelegate : public chaos_type_System_Private_CoreLib_System_Delegate
{
};

struct chaos_type_System_Private_CoreLib_System_Func_System_Int32_ : public chaos_type_System_Private_CoreLib_System_MulticastDelegate
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

struct chaos_boxed_type_System_Private_CoreLib_System_Int32
{
	PureTypeHeader header{};
	CHAOS_IL2CPP_INTPTR value = 0;
};

CHAOS_IL2CPP_INTPTR chaos_static_SnapshotProverSubjects_LdftnHelper___O___0___GetValue = 0;
CHAOS_IL2CPP_INT32 chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = 0;

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

extern "C" CHAOS_IL2CPP_INT32 SnapshotProverSubjects_ArithmeticOps_RunAdd(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_0(void);
extern "C" void SnapshotProverSubjects_Helper_Nop(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotProverSubjects_VoidCaller_DoNothing(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_1(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotProverSubjects_Helper_Square_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotProverSubjects_MathHelper_RunSquare(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_2(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotProverSubjects_FlowControl_IsPositive(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_3(void);
extern "C" void SnapshotProverSubjects_InstanceHelper__ctor_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 SnapshotProverSubjects_InstanceHelper_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotProverSubjects_CallVirtHelper_CreateAndUse(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_4(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotProverSubjects_BoxingHelper_BoxAndUnbox(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_5(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotProverSubjects_ConversionHelper_ConvertToInt(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_6(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotProverSubjects_LoopHelper_SumToFive(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_7(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotProverSubjects_Helper_GetValue(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotProverSubjects_LdftnHelper_GetFnPtr(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_8(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotProverSubjects_GenericsVirtEhDemo_DemoCombine(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_9(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Run_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" void SnapshotProverSubjects_ImplHelper__ctor_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 SnapshotProverSubjects_ImplHelper_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void SnapshotProverSubjects_MarkedClass__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[30] = {
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
	2u,
	2u,
	2u,
	2u,
	2u,
	2u,
	2u,
	2u,
	2u,
	2u,
	2u,
	2u,
	2u,
	2u,
	2u,
	3u,
	4u,
	4u,
	4u,
};

static constexpr struct {
	::ChaosAbiManifestV0 header;
	::ChaosAbiMethodEntryV0 entries[29];
	CHAOS_IL2CPP_UINT8 params[4];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		29u,
		4u,
		2408904273u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 1u, 0u },  // SnapshotProverSubjects_ArithmeticOps_RunAdd
		{ 0u, 0u },  // SnapshotProverSubjects_SnapshotProverSubjects_Subject_0
		{ 0u, 0u },  // SnapshotProverSubjects_Helper_Nop
		{ 1u, 0u },  // SnapshotProverSubjects_VoidCaller_DoNothing
		{ 0u, 0u },  // SnapshotProverSubjects_SnapshotProverSubjects_Subject_1
		{ 1u, 1u },  // SnapshotProverSubjects_Helper_Square_System_Int32
		{ 1u, 0u },  // SnapshotProverSubjects_MathHelper_RunSquare
		{ 0u, 0u },  // SnapshotProverSubjects_SnapshotProverSubjects_Subject_2
		{ 1u, 0u },  // SnapshotProverSubjects_FlowControl_IsPositive
		{ 0u, 0u },  // SnapshotProverSubjects_SnapshotProverSubjects_Subject_3
		{ 0u, 1u },  // SnapshotProverSubjects_InstanceHelper__ctor_System_Int32
		{ 1u, 0u },  // SnapshotProverSubjects_InstanceHelper_GetValue
		{ 1u, 0u },  // SnapshotProverSubjects_CallVirtHelper_CreateAndUse
		{ 0u, 0u },  // SnapshotProverSubjects_SnapshotProverSubjects_Subject_4
		{ 1u, 0u },  // SnapshotProverSubjects_BoxingHelper_BoxAndUnbox
		{ 0u, 0u },  // SnapshotProverSubjects_SnapshotProverSubjects_Subject_5
		{ 1u, 0u },  // SnapshotProverSubjects_ConversionHelper_ConvertToInt
		{ 0u, 0u },  // SnapshotProverSubjects_SnapshotProverSubjects_Subject_6
		{ 1u, 0u },  // SnapshotProverSubjects_LoopHelper_SumToFive
		{ 0u, 0u },  // SnapshotProverSubjects_SnapshotProverSubjects_Subject_7
		{ 1u, 0u },  // SnapshotProverSubjects_Helper_GetValue
		{ 1u, 0u },  // SnapshotProverSubjects_LdftnHelper_GetFnPtr
		{ 0u, 0u },  // SnapshotProverSubjects_SnapshotProverSubjects_Subject_8
		{ 1u, 0u },  // SnapshotProverSubjects_GenericsVirtEhDemo_DemoCombine
		{ 0u, 0u },  // SnapshotProverSubjects_SnapshotProverSubjects_Subject_9
		{ 0u, 1u },  // SnapshotProverSubjects_SnapshotProverSubjects_Run_System_Int32
		{ 0u, 1u },  // SnapshotProverSubjects_ImplHelper__ctor_System_Int32
		{ 1u, 0u },  // SnapshotProverSubjects_ImplHelper_GetValue
		{ 0u, 0u },  // SnapshotProverSubjects_MarkedClass__ctor
	},
	{
		1u,
		1u,
		1u,
		1u,
	},
};
static const ::ChaosAbiManifestV0* const s_abi_manifest =
	reinterpret_cast<const ::ChaosAbiManifestV0*>(&s_abi_manifest_storage);
// ── Module registration ──

	static constexpr CHAOS_IL2CPP_UINT32 s_type_flags[100] = {
		0u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1536u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		12u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1728u,
		1548u,
		1536u,
		1536u,
		1548u,
		1556u,
		1536u,
		1548u,
		1545u,
		1548u,
		1548u,
		1548u,
		1548u,
		1536u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1536u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1536u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1536u,
		1548u,
		1556u,
		1536u,
		1548u,
		1548u,
	};

	static constexpr const char* s_type_names[100] = {
		"<Module>",
		"SnapshotProverSubjects",
		"ArithmeticOps",
		"StringOps",
		"VoidCaller",
		"MathHelper",
		"FlowControl",
		"InstanceHelper",
		"CallVirtHelper",
		"BoxingHelper",
		"FieldHelper",
		"LoopHelper",
		"ConversionHelper",
		"Helper",
		"SwitchHelper",
		"LdftnHelper",
		"TargetHelper",
		"ArrayHelper",
		"EhHelper",
		"GenericHelper",
		"Wrapper`1",
		"DelegateHelper",
		"BaseClass",
		"DerivedClass",
		"VirtualHelper",
		"IMarker",
		"MarkedClass",
		"TypeCheckHelper",
		"SmallStruct",
		"SizeOfHelper",
		"IndirectHelper",
		"ArithmeticCompareHelper",
		"BranchDupHelper",
		"HasInstanceFields",
		"InstanceFieldHelper",
		"StaticFieldWriteHelper",
		"VirtualDispatchHelper",
		"BitwiseHelper",
		"ShiftHelper",
		"ArrayLengthHelper",
		"ValueTypeHelper",
		"LocalAllocHelper",
		"MulDivHelper",
		"BranchCompareHelper",
		"FloatOpsHelper",
		"ConvWideHelper",
		"NegShiftHelper",
		"LdcI8Helper",
		"ConvSmallIntHelper",
		"LdindI1Helper",
		"BranchCompareBHelper",
		"HasFields",
		"AddressHelper",
		"LdtokenHelper",
		"LdelemaHelper",
		"LdargaHelper",
		"CpobjHelper",
		"StindNarrowHelper",
		"CallvirtHelper",
		"UnboxHelper",
		"CpblkHelper",
		"LdindUnsignedHelper",
		"LdindI8Helper",
		"LdindWideHelper",
		"StindWideHelper",
		"ArrayRefHelper",
		"ThrowHelper",
		"OverflowHelper",
		"OverflowConvHelper",
		"CalliHelper",
		"MyClass",
		"LdVirtftnHelper",
		"LdelemAllHelper",
		"StelemAllHelper",
		"UnsignedOpsHelper",
		"RareOpsHelper",
		"StringFormatHelper",
		"BranchUnsignedHelper",
		"BranchUnsignedBHelper",
		"RethrowHelper",
		"OverflowUnHelper",
		"MoreRareOpsHelper",
		"OverflowConvExtHelper",
		"SimpleGapsHelper",
		"LdindStindGapsHelper",
		"CollectionsHelper",
		"ExternalCaller",
		"MultiDispatcher",
		"InternalHelper",
		"StringConcatHelper",
		"GenericsVirtEhDemo",
		"BoxInterfaceArrayDemo",
		"HotUpdateWithTypesDemo",
		"HotUpdateWithGenericsDemo",
		"SealedHelper",
		"SealedClassVirtualHelper",
		"IMyInterface",
		"ImplHelper",
		"InterfaceDevirtHelper",
		"<>O",
	};

	static constexpr const char* s_type_namespaces[100] = {
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_type_parent_tokens[100] = {
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
		33554455u,
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

	static const TypeInfoHot* const s_type_info_ptrs[100] = {
		nullptr,
		&chaos_mt_SnapshotProverSubjects_SnapshotProverSubjects.hot,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		&chaos_mt_SnapshotProverSubjects_InstanceHelper.hot,
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
		&chaos_mt_SnapshotProverSubjects_IMyInterface.hot,
		&chaos_mt_SnapshotProverSubjects_ImplHelper.hot,
		nullptr,
		&chaos_mt_SnapshotProverSubjects_LdftnHelper___O.hot,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_children[1] = {
		33554532u,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_offset[101] = {
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
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_generic_param_constraint_offset[101] = {
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



	static const ::chaos::il2cpp::runtime_core::ModuleDescriptor s_native_aot_module = {
		/* .name_utf8         = */ "SnapshotProverSubjects",
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
		/* .type_count        = */ 100u,
	/* .custom_attribute_blob       = */ nullptr,  // Tier 1 — deferred
	/* .custom_attribute_offset     = */ nullptr,
	/* .custom_attribute_entity_count = */ 0u,
	/* .custom_attribute_materializer = */ nullptr,

		/* .abi_manifest      = */ s_abi_manifest,
	};
	static const CHAOS_IL2CPP_UINT32 s_native_aot_module_id =
		::chaos::il2cpp::runtime_core::RegisterModule("SnapshotProverSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[29] = {
	{ "RunAdd", 0x00000083u, 0u },  // ArithmeticOps
	{ "BoxAndUnbox", 0x0000008Bu, 0u },  // BoxingHelper
	{ "CreateAndUse", 0x0000008Au, 0u },  // CallVirtHelper
	{ "ConvertToInt", 0x0000008Eu, 0u },  // ConversionHelper
	{ "IsPositive", 0x00000087u, 0u },  // FlowControl
	{ "DemoCombine", 0x00000108u, 0u },  // GenericsVirtEhDemo
	{ "Nop", 0x0000008Fu, 0u },  // Helper
	{ "Square", 0x00000090u, 1u },  // Helper
	{ "GetValue", 0x00000092u, 0u },  // Helper
	{ ".ctor", 0x00000110u, 1u },  // ImplHelper
	{ "GetValue", 0x00000111u, 0u },  // ImplHelper
	{ ".ctor", 0x00000088u, 1u },  // InstanceHelper
	{ "GetValue", 0x00000089u, 0u },  // InstanceHelper
	{ "GetFnPtr", 0x00000094u, 0u },  // LdftnHelper
	{ "SumToFive", 0x0000008Du, 0u },  // LoopHelper
	{ ".ctor", 0x000000A1u, 0u },  // MarkedClass
	{ "RunSquare", 0x00000086u, 0u },  // MathHelper
	{ "Subject_0", 0x00000079u, 0u },  // SnapshotProverSubjects
	{ "Subject_1", 0x0000007Au, 0u },  // SnapshotProverSubjects
	{ "Subject_2", 0x0000007Bu, 0u },  // SnapshotProverSubjects
	{ "Subject_3", 0x0000007Cu, 0u },  // SnapshotProverSubjects
	{ "Subject_4", 0x0000007Du, 0u },  // SnapshotProverSubjects
	{ "Subject_5", 0x0000007Eu, 0u },  // SnapshotProverSubjects
	{ "Subject_6", 0x0000007Fu, 0u },  // SnapshotProverSubjects
	{ "Subject_7", 0x00000080u, 0u },  // SnapshotProverSubjects
	{ "Subject_8", 0x00000081u, 0u },  // SnapshotProverSubjects
	{ "Subject_9", 0x00000082u, 0u },  // SnapshotProverSubjects
	{ "Run", 0x00000078u, 1u },  // SnapshotProverSubjects
	{ "DoNothing", 0x00000085u, 0u },  // VoidCaller
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[15] = {
	{ "ArithmeticOps", "", 0u, 1u },
	{ "BoxingHelper", "", 1u, 1u },
	{ "CallVirtHelper", "", 2u, 1u },
	{ "ConversionHelper", "", 3u, 1u },
	{ "FlowControl", "", 4u, 1u },
	{ "GenericsVirtEhDemo", "", 5u, 1u },
	{ "Helper", "", 6u, 3u },
	{ "ImplHelper", "", 9u, 2u },
	{ "InstanceHelper", "", 11u, 2u },
	{ "LdftnHelper", "", 13u, 1u },
	{ "LoopHelper", "", 14u, 1u },
	{ "MarkedClass", "", 15u, 1u },
	{ "MathHelper", "", 16u, 1u },
	{ "SnapshotProverSubjects", "", 17u, 11u },
	{ "VoidCaller", "", 28u, 1u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[29] = {
	{ 0x00000078u, 25u },
	{ 0x00000079u, 1u },
	{ 0x0000007Au, 4u },
	{ 0x0000007Bu, 7u },
	{ 0x0000007Cu, 9u },
	{ 0x0000007Du, 13u },
	{ 0x0000007Eu, 15u },
	{ 0x0000007Fu, 17u },
	{ 0x00000080u, 19u },
	{ 0x00000081u, 22u },
	{ 0x00000082u, 24u },
	{ 0x00000083u, 0u },
	{ 0x00000085u, 3u },
	{ 0x00000086u, 6u },
	{ 0x00000087u, 8u },
	{ 0x00000088u, 10u },
	{ 0x00000089u, 11u },
	{ 0x0000008Au, 12u },
	{ 0x0000008Bu, 14u },
	{ 0x0000008Du, 18u },
	{ 0x0000008Eu, 16u },
	{ 0x0000008Fu, 2u },
	{ 0x00000090u, 5u },
	{ 0x00000092u, 20u },
	{ 0x00000094u, 21u },
	{ 0x000000A1u, 28u },
	{ 0x00000108u, 23u },
	{ 0x00000110u, 26u },
	{ 0x00000111u, 27u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[29] = {
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_ArithmeticOps_RunAdd), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ArithmeticOps::RunAdd
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // SnapshotProverSubjects::Subject_0
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_Helper_Nop), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // Helper::Nop
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_VoidCaller_DoNothing), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // VoidCaller::DoNothing
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // SnapshotProverSubjects::Subject_1
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_Helper_Square_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // Helper::Square
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_MathHelper_RunSquare), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // MathHelper::RunSquare
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // SnapshotProverSubjects::Subject_2
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_FlowControl_IsPositive), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // FlowControl::IsPositive
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // SnapshotProverSubjects::Subject_3
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_InstanceHelper__ctor_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // InstanceHelper::.ctor
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_InstanceHelper_GetValue), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // InstanceHelper::GetValue
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_CallVirtHelper_CreateAndUse), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // CallVirtHelper::CreateAndUse
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // SnapshotProverSubjects::Subject_4
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_BoxingHelper_BoxAndUnbox), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // BoxingHelper::BoxAndUnbox
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // SnapshotProverSubjects::Subject_5
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_ConversionHelper_ConvertToInt), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ConversionHelper::ConvertToInt
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // SnapshotProverSubjects::Subject_6
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_LoopHelper_SumToFive), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // LoopHelper::SumToFive
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // SnapshotProverSubjects::Subject_7
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_Helper_GetValue), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // Helper::GetValue
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_LdftnHelper_GetFnPtr), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // LdftnHelper::GetFnPtr
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // SnapshotProverSubjects::Subject_8
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_GenericsVirtEhDemo_DemoCombine), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // GenericsVirtEhDemo::DemoCombine
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_9), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // SnapshotProverSubjects::Subject_9
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Run_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // SnapshotProverSubjects::Run
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_ImplHelper__ctor_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ImplHelper::.ctor
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_ImplHelper_GetValue), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ImplHelper::GetValue
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_MarkedClass__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // MarkedClass::.ctor
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"SnapshotProverSubjects",
	s_hotpatch_types,
	15u,
	s_hotpatch_methods,
	29u,
	s_hotpatch_slots,
	29u,
	s_hotpatch_entries,
	29u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[9] = {
	"SnapshotProverSubjects/SnapshotProverSubjects::_exitCode",
	"System.Private.CoreLib/System.Object::.ctor:System.Void()",
	"SnapshotProverSubjects/InstanceHelper::_val",
	"System.Private.CoreLib/System.Int32",
	"SnapshotProverSubjects/LdftnHelper+<>O::<0>__GetValue",
	"System.Private.CoreLib/System.Func<System.Int32>::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Collections/System.Collections.Generic.List<System.Int32>::.ctor:System.Void()",
	"System.Collections/System.Collections.Generic.List<System.Int32>::get_Count:System.Int32()",
	"SnapshotProverSubjects/ImplHelper::_val",
};

extern "C" void* kChaosExternalRuntimeFnTable[9] = {
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__),
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
static void (*kAotMethods[29])() = {
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_ArithmeticOps_RunAdd),
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_0),
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_Helper_Nop),
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_VoidCaller_DoNothing),
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_1),
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_Helper_Square_System_Int32),
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_MathHelper_RunSquare),
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_2),
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_FlowControl_IsPositive),
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_3),
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_InstanceHelper__ctor_System_Int32),
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_InstanceHelper_GetValue),
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_CallVirtHelper_CreateAndUse),
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_4),
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_BoxingHelper_BoxAndUnbox),
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_5),
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_ConversionHelper_ConvertToInt),
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_6),
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_LoopHelper_SumToFive),
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_7),
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_Helper_GetValue),
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_LdftnHelper_GetFnPtr),
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_8),
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_GenericsVirtEhDemo_DemoCombine),
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_9),
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_SnapshotProverSubjects_Run_System_Int32),
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_ImplHelper__ctor_System_Int32),
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_ImplHelper_GetValue),
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_MarkedClass__ctor),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
static void (*kBenchmarkWrappers[29])() = {
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
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[10])(0);},
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
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[25])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[26])(0);},
	[]() {kAotMethods[27]();},
	[]() {kAotMethods[28]();},
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
		reinterpret_cast<void(*)()>(entry.direct_ptr)();
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
		reinterpret_cast<void(*)()>(entry.direct_ptr)();
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
static void* const kMethodPointers[29] = {
	reinterpret_cast<void*>(&SnapshotProverSubjects_ArithmeticOps_RunAdd),
	reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_0),
	reinterpret_cast<void*>(&SnapshotProverSubjects_Helper_Nop),
	reinterpret_cast<void*>(&SnapshotProverSubjects_VoidCaller_DoNothing),
	reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_1),
	reinterpret_cast<void*>(&SnapshotProverSubjects_Helper_Square_System_Int32),
	reinterpret_cast<void*>(&SnapshotProverSubjects_MathHelper_RunSquare),
	reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_2),
	reinterpret_cast<void*>(&SnapshotProverSubjects_FlowControl_IsPositive),
	reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_3),
	reinterpret_cast<void*>(&SnapshotProverSubjects_InstanceHelper__ctor_System_Int32),
	reinterpret_cast<void*>(&SnapshotProverSubjects_InstanceHelper_GetValue),
	reinterpret_cast<void*>(&SnapshotProverSubjects_CallVirtHelper_CreateAndUse),
	reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_4),
	reinterpret_cast<void*>(&SnapshotProverSubjects_BoxingHelper_BoxAndUnbox),
	reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_5),
	reinterpret_cast<void*>(&SnapshotProverSubjects_ConversionHelper_ConvertToInt),
	reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_6),
	reinterpret_cast<void*>(&SnapshotProverSubjects_LoopHelper_SumToFive),
	reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_7),
	reinterpret_cast<void*>(&SnapshotProverSubjects_Helper_GetValue),
	reinterpret_cast<void*>(&SnapshotProverSubjects_LdftnHelper_GetFnPtr),
	reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_8),
	reinterpret_cast<void*>(&SnapshotProverSubjects_GenericsVirtEhDemo_DemoCombine),
	reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_9),
	reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Run_System_Int32),
	reinterpret_cast<void*>(&SnapshotProverSubjects_ImplHelper__ctor_System_Int32),
	reinterpret_cast<void*>(&SnapshotProverSubjects_ImplHelper_GetValue),
	reinterpret_cast<void*>(&SnapshotProverSubjects_MarkedClass__ctor),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
// ── VTable descriptors (for BootstrapRuntime TypeVTable registration) ──
static const VTableDescriptorV0 kChaosVTableDescriptors[] = {
	{
		CHAOS_IL2CPP_UINT64_C(0xD09238CEAED4FEF7),
		0x0000005Fu,
		0u,
		2u,
		reinterpret_cast<const void*>(kSlots_SnapshotProverSubjects_ImplHelper),
		reinterpret_cast<const void**>(chaos_vtable_SnapshotProverSubjects_ImplHelper),
		2u,
		1,
		{0, 0, 0},
		chaos_iface_map_SnapshotProverSubjects_ImplHelper,
		1u,
	},
	{
		CHAOS_IL2CPP_UINT64_C(0xE85FA6A9596DCA0A),
		0x00000007u,
		0u,
		2u,
		reinterpret_cast<const void*>(kSlots_SnapshotProverSubjects_InstanceHelper),
		reinterpret_cast<const void**>(chaos_vtable_SnapshotProverSubjects_InstanceHelper),
		2u,
		1,
		{0, 0, 0},
		nullptr,
		0u,
	},
};
extern "C" const CodeRegistrationV0 chaos_codegen_code_registration
	= {
	.struct_size               = sizeof(CodeRegistrationV0),
	.method_pointers           = kMethodPointers,
	.method_pointer_count      = 29u,
	.reverse_pinvoke_wrappers  = nullptr,
	.reverse_pinvoke_wrapper_count = 0u,
	.invoker_pointers          = nullptr,
	.invoker_pointer_count     = 0u,
	.unresolved_virtual_calls = nullptr,
	.unresolved_virtual_call_count = 0u,
	.type_capabilities       = nullptr,
	.type_capability_count   = 0u,
	.vtable_descriptors = kChaosVTableDescriptors,
	.vtable_descriptor_count = 2u,
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
	.image_name_utf8    = "SnapshotProverSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotProverSubjects_ArithmeticOps[1] = {
	{ 0u, "SnapshotProverSubjects/ArithmeticOps::RunAdd:System.Int32()", "RunAdd", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotProverSubjects_SnapshotProverSubjects[11] = {
	{ 0u, "SnapshotProverSubjects/SnapshotProverSubjects::Subject_0:System.Void()", "Subject_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotProverSubjects/SnapshotProverSubjects::Subject_1:System.Void()", "Subject_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotProverSubjects/SnapshotProverSubjects::Subject_2:System.Void()", "Subject_2", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotProverSubjects/SnapshotProverSubjects::Subject_3:System.Void()", "Subject_3", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotProverSubjects/SnapshotProverSubjects::Subject_4:System.Void()", "Subject_4", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotProverSubjects/SnapshotProverSubjects::Subject_5:System.Void()", "Subject_5", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotProverSubjects/SnapshotProverSubjects::Subject_6:System.Void()", "Subject_6", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotProverSubjects/SnapshotProverSubjects::Subject_7:System.Void()", "Subject_7", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotProverSubjects/SnapshotProverSubjects::Subject_8:System.Void()", "Subject_8", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotProverSubjects/SnapshotProverSubjects::Subject_9:System.Void()", "Subject_9", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotProverSubjects/SnapshotProverSubjects::Run:System.Void(System.Int32)", "Run", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotProverSubjects_Helper[3] = {
	{ 0u, "SnapshotProverSubjects/Helper::Nop:System.Void()", "Nop", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotProverSubjects/Helper::Square:System.Int32(System.Int32)", "Square", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotProverSubjects/Helper::GetValue:System.Int32()", "GetValue", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotProverSubjects_VoidCaller[1] = {
	{ 0u, "SnapshotProverSubjects/VoidCaller::DoNothing:System.Int32()", "DoNothing", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotProverSubjects_MathHelper[1] = {
	{ 0u, "SnapshotProverSubjects/MathHelper::RunSquare:System.Int32()", "RunSquare", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotProverSubjects_FlowControl[1] = {
	{ 0u, "SnapshotProverSubjects/FlowControl::IsPositive:System.Int32()", "IsPositive", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotProverSubjects_InstanceHelper[2] = {
	{ 0u, "SnapshotProverSubjects/InstanceHelper::.ctor:System.Void(System.Int32)", ".ctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotProverSubjects/InstanceHelper::GetValue:System.Int32()", "GetValue", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotProverSubjects_CallVirtHelper[1] = {
	{ 0u, "SnapshotProverSubjects/CallVirtHelper::CreateAndUse:System.Int32()", "CreateAndUse", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotProverSubjects_BoxingHelper[1] = {
	{ 0u, "SnapshotProverSubjects/BoxingHelper::BoxAndUnbox:System.Int32()", "BoxAndUnbox", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotProverSubjects_ConversionHelper[1] = {
	{ 0u, "SnapshotProverSubjects/ConversionHelper::ConvertToInt:System.Int32()", "ConvertToInt", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotProverSubjects_LoopHelper[1] = {
	{ 0u, "SnapshotProverSubjects/LoopHelper::SumToFive:System.Int32()", "SumToFive", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotProverSubjects_LdftnHelper[1] = {
	{ 0u, "SnapshotProverSubjects/LdftnHelper::GetFnPtr:System.Int32()", "GetFnPtr", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotProverSubjects_GenericsVirtEhDemo[1] = {
	{ 0u, "SnapshotProverSubjects/GenericsVirtEhDemo::DemoCombine:System.Int32()", "DemoCombine", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotProverSubjects_ImplHelper[2] = {
	{ 0u, "SnapshotProverSubjects/ImplHelper::.ctor:System.Void(System.Int32)", ".ctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotProverSubjects/ImplHelper::GetValue:System.Int32()", "GetValue", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotProverSubjects_MarkedClass[1] = {
	{ 0u, "SnapshotProverSubjects/MarkedClass::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryTypeDescriptor kReflTypes[15] = {
	{ 0u, "SnapshotProverSubjects/ArithmeticOps", "SnapshotProverSubjects/ArithmeticOps", "", "ArithmeticOps", "ArithmeticOps", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotProverSubjects_ArithmeticOps, 1u },
	{ 0u, "SnapshotProverSubjects/SnapshotProverSubjects", "SnapshotProverSubjects/SnapshotProverSubjects", "", "SnapshotProverSubjects", "SnapshotProverSubjects", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotProverSubjects_SnapshotProverSubjects, 11u },
	{ 0u, "SnapshotProverSubjects/Helper", "SnapshotProverSubjects/Helper", "", "Helper", "Helper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotProverSubjects_Helper, 3u },
	{ 0u, "SnapshotProverSubjects/VoidCaller", "SnapshotProverSubjects/VoidCaller", "", "VoidCaller", "VoidCaller", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotProverSubjects_VoidCaller, 1u },
	{ 0u, "SnapshotProverSubjects/MathHelper", "SnapshotProverSubjects/MathHelper", "", "MathHelper", "MathHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotProverSubjects_MathHelper, 1u },
	{ 0u, "SnapshotProverSubjects/FlowControl", "SnapshotProverSubjects/FlowControl", "", "FlowControl", "FlowControl", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotProverSubjects_FlowControl, 1u },
	{ 0u, "SnapshotProverSubjects/InstanceHelper", "SnapshotProverSubjects/InstanceHelper", "", "InstanceHelper", "InstanceHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotProverSubjects_InstanceHelper, 2u },
	{ 0u, "SnapshotProverSubjects/CallVirtHelper", "SnapshotProverSubjects/CallVirtHelper", "", "CallVirtHelper", "CallVirtHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotProverSubjects_CallVirtHelper, 1u },
	{ 0u, "SnapshotProverSubjects/BoxingHelper", "SnapshotProverSubjects/BoxingHelper", "", "BoxingHelper", "BoxingHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotProverSubjects_BoxingHelper, 1u },
	{ 0u, "SnapshotProverSubjects/ConversionHelper", "SnapshotProverSubjects/ConversionHelper", "", "ConversionHelper", "ConversionHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotProverSubjects_ConversionHelper, 1u },
	{ 0u, "SnapshotProverSubjects/LoopHelper", "SnapshotProverSubjects/LoopHelper", "", "LoopHelper", "LoopHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotProverSubjects_LoopHelper, 1u },
	{ 0u, "SnapshotProverSubjects/LdftnHelper", "SnapshotProverSubjects/LdftnHelper", "", "LdftnHelper", "LdftnHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotProverSubjects_LdftnHelper, 1u },
	{ 0u, "SnapshotProverSubjects/GenericsVirtEhDemo", "SnapshotProverSubjects/GenericsVirtEhDemo", "", "GenericsVirtEhDemo", "GenericsVirtEhDemo", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotProverSubjects_GenericsVirtEhDemo, 1u },
	{ 0u, "SnapshotProverSubjects/ImplHelper", "SnapshotProverSubjects/ImplHelper", "", "ImplHelper", "ImplHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotProverSubjects_ImplHelper, 2u },
	{ 0u, "SnapshotProverSubjects/MarkedClass", "SnapshotProverSubjects/MarkedClass", "", "MarkedClass", "MarkedClass", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotProverSubjects_MarkedClass, 1u },
};

static constexpr const ReflectionQueryTypeDescriptor* kReflTypePtrs[15] = {
	&kReflTypes[0],
	&kReflTypes[1],
	&kReflTypes[2],
	&kReflTypes[3],
	&kReflTypes[4],
	&kReflTypes[5],
	&kReflTypes[6],
	&kReflTypes[7],
	&kReflTypes[8],
	&kReflTypes[9],
	&kReflTypes[10],
	&kReflTypes[11],
	&kReflTypes[12],
	&kReflTypes[13],
	&kReflTypes[14],
};

static constexpr ReflectionQueryImageDescriptor kReflImage = { "SnapshotProverSubjects", kReflTypePtrs, 15u };

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
// Managed method: SnapshotProverSubjects/ArithmeticOps::RunAdd()
extern "C" CHAOS_IL2CPP_INT32 SnapshotProverSubjects_ArithmeticOps_RunAdd(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(7);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotProverSubjects/SnapshotProverSubjects::Subject_0()
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_0(void)
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
			_d_hpresult = SnapshotProverSubjects_ArithmeticOps_RunAdd();
		}
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
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
			_d_hpresult = SnapshotProverSubjects_ArithmeticOps_RunAdd();
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: SnapshotProverSubjects/Helper::Nop()
extern "C" void SnapshotProverSubjects_Helper_Nop(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: SnapshotProverSubjects/VoidCaller::DoNothing()
extern "C" CHAOS_IL2CPP_INT32 SnapshotProverSubjects_VoidCaller_DoNothing(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	{
		auto& _d6 = s_hotpatch_entries[6];
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d6)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d6))
		{
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d6.method_key, nullptr, nullptr);
		}
		else
		{
			SnapshotProverSubjects_Helper_Nop();
		}
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotProverSubjects/SnapshotProverSubjects::Subject_1()
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_1(void)
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
			_d_hpresult = SnapshotProverSubjects_VoidCaller_DoNothing();
		}
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
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
			_d_hpresult = SnapshotProverSubjects_VoidCaller_DoNothing();
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: SnapshotProverSubjects/Helper::Square(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 SnapshotProverSubjects_Helper_Square_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	_s1 = chaos_args[0];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapMul(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotProverSubjects/MathHelper::RunSquare()
extern "C" CHAOS_IL2CPP_INT32 SnapshotProverSubjects_MathHelper_RunSquare(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		const auto chaos_arg_0 = _s0;
		auto& _d7 = s_hotpatch_entries[7];
		CHAOS_IL2CPP_INT32 _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d7)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d7))
		{
			alignas(16) uint8_t _d_ab[4];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d7.method_key, _d_ab, &_d_hpresult);
		}
		else
		{
			_d_hpresult = SnapshotProverSubjects_Helper_Square_System_Int32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
		}
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotProverSubjects/SnapshotProverSubjects::Subject_2()
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_2(void)
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
			_d_hpresult = SnapshotProverSubjects_MathHelper_RunSquare();
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
			_d_hpresult = SnapshotProverSubjects_MathHelper_RunSquare();
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: SnapshotProverSubjects/FlowControl::IsPositive()
extern "C" CHAOS_IL2CPP_INT32 SnapshotProverSubjects_FlowControl_IsPositive(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		if (_s0 != 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		chaos_locals[1] = _s0;
		_s0 = chaos_locals[1];
		return static_cast<CHAOS_IL2CPP_INT32>(_s0);
	}
}

// Managed method: SnapshotProverSubjects/SnapshotProverSubjects::Subject_3()
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_3(void)
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
			_d_hpresult = SnapshotProverSubjects_FlowControl_IsPositive();
		}
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
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
			_d_hpresult = SnapshotProverSubjects_FlowControl_IsPositive();
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: SnapshotProverSubjects/InstanceHelper::.ctor(System.Int32)
extern "C" void SnapshotProverSubjects_InstanceHelper__ctor_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(chaos_arg_0);
	}
	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	{
		auto chaos_value = _s1;
		auto* chaos_object = reinterpret_cast<chaos_type_SnapshotProverSubjects_InstanceHelper*>(_s0);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_object->field_SnapshotProverSubjects_InstanceHelper___val));
		chaos_object->field_SnapshotProverSubjects_InstanceHelper___val = chaos_value;
		chaos_gc_dirty_card(chaos_object);
	}
	return;
}

// Managed method: SnapshotProverSubjects/InstanceHelper::GetValue()
extern "C" CHAOS_IL2CPP_INT32 SnapshotProverSubjects_InstanceHelper_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_SnapshotProverSubjects_InstanceHelper*>(_s0);
		_s0 = chaos_object->field_SnapshotProverSubjects_InstanceHelper___val;
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotProverSubjects/CallVirtHelper::CreateAndUse()
extern "C" CHAOS_IL2CPP_INT32 SnapshotProverSubjects_CallVirtHelper_CreateAndUse(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		const auto chaos_arg_1 = _s0;
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_SnapshotProverSubjects_InstanceHelper, {});
		chaos_object->header.type_info = &chaos_mt_SnapshotProverSubjects_InstanceHelper.hot;
		SnapshotProverSubjects_InstanceHelper__ctor_System_Int32(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s0);
		auto& _d12 = s_hotpatch_entries[12];
		CHAOS_IL2CPP_INT32 _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d12)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d12))
		{
			alignas(16) uint8_t _d_ab[8];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d12.method_key, _d_ab, &_d_hpresult);
		}
		else
		{
			_d_hpresult = SnapshotProverSubjects_InstanceHelper_GetValue(chaos_arg_0);
		}
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotProverSubjects/SnapshotProverSubjects::Subject_4()
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_4(void)
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
			_d_hpresult = SnapshotProverSubjects_CallVirtHelper_CreateAndUse();
		}
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
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
			_d_hpresult = SnapshotProverSubjects_CallVirtHelper_CreateAndUse();
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: SnapshotProverSubjects/BoxingHelper::BoxAndUnbox()
extern "C" CHAOS_IL2CPP_INT32 SnapshotProverSubjects_BoxingHelper_BoxAndUnbox(void)
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
	chaos_boxed_type_System_Private_CoreLib_System_Int32 chaos_box_storage_0{};
	{
		const auto chaos_value = _s0;
		chaos_box_storage_0.header.type_info = &chaos_mt_System_Private_CoreLib_System_Int32.hot;
		chaos_box_storage_0.value = chaos_value;
	}
	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_box_storage_0);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		auto* chaos_boxed = reinterpret_cast<chaos_boxed_type_System_Private_CoreLib_System_Int32*>(_s0);
		if (chaos_boxed == nullptr)
		{
			CHAOS_IL2CPP_FAIL();
		}
		_s0 = chaos_boxed->value;
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotProverSubjects/SnapshotProverSubjects::Subject_5()
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_5(void)
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
			_d_hpresult = SnapshotProverSubjects_BoxingHelper_BoxAndUnbox();
		}
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
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
			_d_hpresult = SnapshotProverSubjects_BoxingHelper_BoxAndUnbox();
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: SnapshotProverSubjects/ConversionHelper::ConvertToInt()
extern "C" CHAOS_IL2CPP_INT32 SnapshotProverSubjects_ConversionHelper_ConvertToInt(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(100);
	_s0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	chaos_locals[2] = _s0;
	_s0 = chaos_locals[2];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotProverSubjects/SnapshotProverSubjects::Subject_6()
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_6(void)
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
			_d_hpresult = SnapshotProverSubjects_ConversionHelper_ConvertToInt();
		}
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
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
			_d_hpresult = SnapshotProverSubjects_ConversionHelper_ConvertToInt();
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: SnapshotProverSubjects/LoopHelper::SumToFive()
extern "C" CHAOS_IL2CPP_INT32 SnapshotProverSubjects_LoopHelper_SumToFive(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 4) chaos_locals{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_eval_stack{};
	CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


chaos_label_1:
	chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
chaos_label_2:
	chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
chaos_label_3:
	chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
chaos_label_4:
	chaos_locals[1] = chaos_eval_stack[--chaos_stack_top];
chaos_label_5:
	goto chaos_label_15;
chaos_label_7:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
chaos_label_8:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[1];
chaos_label_9:
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(chaos_left, chaos_right));
	}
chaos_label_10:
	chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
chaos_label_11:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[1];
chaos_label_12:
	chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
chaos_label_13:
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(chaos_left, chaos_right));
	}
chaos_label_14:
	chaos_locals[1] = chaos_eval_stack[--chaos_stack_top];
chaos_label_15:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[1];
chaos_label_16:
	chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(5);
chaos_label_17:
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left < chaos_right ? 1 : 0);
	}
chaos_label_19:
	chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
chaos_label_20:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[2];
chaos_label_21:
	if (chaos_eval_stack[--chaos_stack_top] != 0)
		goto chaos_label_7;
chaos_label_23:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
chaos_label_24:
	chaos_locals[3] = chaos_eval_stack[--chaos_stack_top];
chaos_label_25:
	goto chaos_label_27;
chaos_label_27:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[3];
chaos_label_28:
	return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
}

// Managed method: SnapshotProverSubjects/SnapshotProverSubjects::Subject_7()
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_7(void)
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
			_d_hpresult = SnapshotProverSubjects_LoopHelper_SumToFive();
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
			_d_hpresult = SnapshotProverSubjects_LoopHelper_SumToFive();
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: SnapshotProverSubjects/Helper::GetValue()
extern "C" CHAOS_IL2CPP_INT32 SnapshotProverSubjects_Helper_GetValue(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotProverSubjects/LdftnHelper::GetFnPtr()
extern "C" CHAOS_IL2CPP_INT32 SnapshotProverSubjects_LdftnHelper_GetFnPtr(void)
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


	_s0 = chaos_static_SnapshotProverSubjects_LdftnHelper___O___0___GetValue;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			_s0 = 0;
			{
				// Hotpatch-aware ldftn wrapper (slot 8)
				static auto* chaos_ftn_thunk = +[](void) -> CHAOS_IL2CPP_INT32 {
					auto& _d_entry = s_hotpatch_entries[8];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						CHAOS_IL2CPP_INT32 _d_ret{};
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, nullptr, &_d_ret);
						return _d_ret;
					}
					return reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(_d_entry.direct_ptr)();
				};
			_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s1;
				const auto chaos_target = _s0;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s1 = _s0;
			{
				auto chaos_value = _s1;
				chaos_static_SnapshotProverSubjects_LdftnHelper___O___0___GetValue = chaos_value;
			}
		}
		chaos_locals[0] = _s0;
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		chaos_locals[1] = _s0;
		_s0 = chaos_locals[1];
		return static_cast<CHAOS_IL2CPP_INT32>(_s0);
	}
}

// Managed method: SnapshotProverSubjects/SnapshotProverSubjects::Subject_8()
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_8(void)
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
			_d_hpresult = SnapshotProverSubjects_LdftnHelper_GetFnPtr();
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
			_d_hpresult = SnapshotProverSubjects_LdftnHelper_GetFnPtr();
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: SnapshotProverSubjects/GenericsVirtEhDemo::DemoCombine()
extern "C" CHAOS_IL2CPP_INT32 SnapshotProverSubjects_GenericsVirtEhDemo_DemoCombine(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};


#if !defined(CHAOS_IL2CPP_EH_SETJMP) && !defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	try
	{
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Collections_System_Collections_Generic_List_System_Int32_, {});
				chaos_object->header.type_info = &chaos_mt_System_Collections_System_Collections_Generic_List_System_Int32_.hot;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[7])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[1] = _s1;
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
		_s1 = chaos_exception.object_value;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(-1);
			chaos_locals[1] = _s1;
	}
#elif defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	__try
	{
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Collections_System_Collections_Generic_List_System_Int32_, {});
				chaos_object->header.type_info = &chaos_mt_System_Collections_System_Collections_Generic_List_System_Int32_.hot;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			chaos_locals[0] = _s1;
			_s1 = chaos_locals[0];
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[7])();
				_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[1] = _s2;
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
		_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(-1);
			chaos_locals[1] = _s2;
	}
#else
	{
		auto* _chaos_jmp =
			chaos::il2cpp::runtime_core::push_exception_jmp_buf();
		if (setjmp(*_chaos_jmp) == 0)
		{
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Collections_System_Collections_Generic_List_System_Int32_, {});
				chaos_object->header.type_info = &chaos_mt_System_Collections_System_Collections_Generic_List_System_Int32_.hot;
				_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			chaos_locals[0] = _s2;
			_s2 = chaos_locals[0];
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[7])();
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[1] = _s3;
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
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(-1);
			chaos_locals[1] = _s3;
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	_s3 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s3);
}

// Managed method: SnapshotProverSubjects/SnapshotProverSubjects::Subject_9()
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_9(void)
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
			_d_hpresult = SnapshotProverSubjects_GenericsVirtEhDemo_DemoCombine();
		}
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
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
			_d_hpresult = SnapshotProverSubjects_GenericsVirtEhDemo_DemoCombine();
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: SnapshotProverSubjects/SnapshotProverSubjects::Run(System.Int32)
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Run_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		const auto chaos_switch_value = static_cast<CHAOS_IL2CPP_INT32>(_s0);
		switch (chaos_switch_value)
		{
			case 0:
				{
					auto& _d17 = s_hotpatch_entries[17];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d17)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d17))
					{
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d17.method_key, nullptr, nullptr);
					}
					else
					{
						SnapshotProverSubjects_SnapshotProverSubjects_Subject_0();
					}
				}
			case 1:
				{
					auto& _d18 = s_hotpatch_entries[18];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d18)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d18))
					{
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d18.method_key, nullptr, nullptr);
					}
					else
					{
						SnapshotProverSubjects_SnapshotProverSubjects_Subject_1();
					}
				}
			case 2:
				{
					auto& _d19 = s_hotpatch_entries[19];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d19)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d19))
					{
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d19.method_key, nullptr, nullptr);
					}
					else
					{
						SnapshotProverSubjects_SnapshotProverSubjects_Subject_2();
					}
				}
			case 3:
				{
					auto& _d20 = s_hotpatch_entries[20];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d20)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d20))
					{
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d20.method_key, nullptr, nullptr);
					}
					else
					{
						SnapshotProverSubjects_SnapshotProverSubjects_Subject_3();
					}
				}
			case 4:
				{
					auto& _d21 = s_hotpatch_entries[21];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d21)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d21))
					{
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d21.method_key, nullptr, nullptr);
					}
					else
					{
						SnapshotProverSubjects_SnapshotProverSubjects_Subject_4();
					}
				}
			case 5:
				{
					auto& _d22 = s_hotpatch_entries[22];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d22)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d22))
					{
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d22.method_key, nullptr, nullptr);
					}
					else
					{
						SnapshotProverSubjects_SnapshotProverSubjects_Subject_5();
					}
				}
			case 6:
				{
					auto& _d23 = s_hotpatch_entries[23];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d23)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d23))
					{
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d23.method_key, nullptr, nullptr);
					}
					else
					{
						SnapshotProverSubjects_SnapshotProverSubjects_Subject_6();
					}
				}
			case 7:
				{
					auto& _d24 = s_hotpatch_entries[24];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d24)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d24))
					{
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d24.method_key, nullptr, nullptr);
					}
					else
					{
						SnapshotProverSubjects_SnapshotProverSubjects_Subject_7();
					}
				}
			case 8:
				{
					auto& _d25 = s_hotpatch_entries[25];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d25)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d25))
					{
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d25.method_key, nullptr, nullptr);
					}
					else
					{
						SnapshotProverSubjects_SnapshotProverSubjects_Subject_8();
					}
				}
			case 9:
			{
				{
					auto& _d26 = s_hotpatch_entries[26];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d26)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d26))
					{
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d26.method_key, nullptr, nullptr);
					}
					else
					{
						SnapshotProverSubjects_SnapshotProverSubjects_Subject_9();
					}
				}
				break;
			}
			default:
				return;
		}
	}
	return;
}

// Managed method: SnapshotProverSubjects/ImplHelper::.ctor(System.Int32)
extern "C" void SnapshotProverSubjects_ImplHelper__ctor_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(chaos_arg_0);
	}
	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	{
		auto chaos_value = _s1;
		auto* chaos_object = reinterpret_cast<chaos_type_SnapshotProverSubjects_ImplHelper*>(_s0);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_object->field_SnapshotProverSubjects_ImplHelper___val));
		chaos_object->field_SnapshotProverSubjects_ImplHelper___val = chaos_value;
		chaos_gc_dirty_card(chaos_object);
	}
	return;
}

// Managed method: SnapshotProverSubjects/ImplHelper::GetValue()
extern "C" CHAOS_IL2CPP_INT32 SnapshotProverSubjects_ImplHelper_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_SnapshotProverSubjects_ImplHelper*>(_s0);
		_s0 = chaos_object->field_SnapshotProverSubjects_ImplHelper___val;
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotProverSubjects/MarkedClass::.ctor()
extern "C" void SnapshotProverSubjects_MarkedClass__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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



}  // namespace chaos::il2cpp::codegen::SnapshotProverSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 29;