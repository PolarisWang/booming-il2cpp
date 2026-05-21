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
#include "runtime_stubs/stub_common.h"
#include "string_table.h"
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

namespace chaos::il2cpp::codegen::RuntimeSelfTest {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


const void* chaos_vtable_RuntimeSelfTest_ArrayOperations[];
const void* chaos_vtable_RuntimeSelfTest_FinalizerProof[];
const void* chaos_vtable_RuntimeSelfTest_GcAllocationProof[];
const void* chaos_vtable_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0[];
const void* chaos_vtable_RuntimeSelfTest_StringOperations[];
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Object = {{nullptr, nullptr, 15228727185366376748ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_INTPTR>(15228727185366376748ULL);
inline TypeInfoV0 chaos_mt_RuntimeSelfTest_ArrayOperations = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_RuntimeSelfTest_ArrayOperations, 13310151847772998629ULL, 3u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_RuntimeSelfTest_ArrayOperations = static_cast<CHAOS_IL2CPP_INTPTR>(13310151847772998629ULL);
inline TypeInfoV0 chaos_mt_RuntimeSelfTest_FinalizerProof = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_RuntimeSelfTest_FinalizerProof, 1867398769891201420ULL, 6u, 32, 1, 5}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_RuntimeSelfTest_FinalizerProof = static_cast<CHAOS_IL2CPP_INTPTR>(1867398769891201420ULL);
inline TypeInfoV0 chaos_mt_RuntimeSelfTest_GcAllocationProof = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_RuntimeSelfTest_GcAllocationProof, 5921064987981938432ULL, 10u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_RuntimeSelfTest_GcAllocationProof = static_cast<CHAOS_IL2CPP_INTPTR>(5921064987981938432ULL);
inline TypeInfoV0 chaos_mt_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0 = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0, 10027820858295396049ULL, 11u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0 = static_cast<CHAOS_IL2CPP_INTPTR>(10027820858295396049ULL);
inline TypeInfoV0 chaos_mt_RuntimeSelfTest_StringOperations = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_RuntimeSelfTest_StringOperations, 17779707276959787309ULL, 13u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_RuntimeSelfTest_StringOperations = static_cast<CHAOS_IL2CPP_INTPTR>(17779707276959787309ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Delegate = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, nullptr, 7451128447593600616ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Delegate = static_cast<CHAOS_IL2CPP_INTPTR>(7451128447593600616ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_MulticastDelegate = {{&chaos_mt_System_Private_CoreLib_System_Delegate.hot, nullptr, 6681393039041505440ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_MulticastDelegate = static_cast<CHAOS_IL2CPP_INTPTR>(6681393039041505440ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Int32_ = {{&chaos_mt_System_Private_CoreLib_System_MulticastDelegate.hot, nullptr, 143744549824129343ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Func_System_Int32_System_Int32_ = static_cast<CHAOS_IL2CPP_INTPTR>(143744549824129343ULL);
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
inline TypeInfoV0 chaos_mt_System_Threading_Thread_System_Threading_Thread = {{nullptr, nullptr, 11502368789179341480ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Threading_Thread_System_Threading_Thread = static_cast<CHAOS_IL2CPP_INTPTR>(11502368789179341480ULL);
inline TypeInfoV0 chaos_mt_System_Threading_Thread_System_Threading_ThreadStart = {{nullptr, nullptr, 17186843898991579444ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Threading_Thread_System_Threading_ThreadStart = static_cast<CHAOS_IL2CPP_INTPTR>(17186843898991579444ULL);

// ── Virtual method table arrays ──
extern "C" void RuntimeSelfTest_ArrayOperations__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 RuntimeSelfTest_ArrayOperations_CreateAndSum_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR RuntimeSelfTest_ArrayOperations_CreateArray_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
const void* chaos_vtable_RuntimeSelfTest_ArrayOperations[] =
{
	reinterpret_cast<void*>(RuntimeSelfTest_ArrayOperations__ctor),
	reinterpret_cast<void*>(RuntimeSelfTest_ArrayOperations_CreateAndSum_System_Int32),
	reinterpret_cast<void*>(RuntimeSelfTest_ArrayOperations_CreateArray_System_Int32),
};
extern "C" void RuntimeSelfTest_FinalizerProof__ctor_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void RuntimeSelfTest_FinalizerProof_Finalize(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 RuntimeSelfTest_FinalizerProof_get_Id(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
const void* chaos_vtable_RuntimeSelfTest_FinalizerProof[] =
{
	nullptr,
	nullptr,
	nullptr,
	reinterpret_cast<void*>(RuntimeSelfTest_FinalizerProof__ctor_System_Int32),
	reinterpret_cast<void*>(RuntimeSelfTest_FinalizerProof_Finalize),
	reinterpret_cast<void*>(RuntimeSelfTest_FinalizerProof_get_Id),
};
extern "C" void RuntimeSelfTest_GcAllocationProof__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 RuntimeSelfTest_GcAllocationProof_AllocateAndReturn_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 RuntimeSelfTest_GcAllocationProof_AllocateAtomic_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 RuntimeSelfTest_GcAllocationProof_get_Value(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void RuntimeSelfTest_GcAllocationProof_set_Value_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
const void* chaos_vtable_RuntimeSelfTest_GcAllocationProof[] =
{
	reinterpret_cast<void*>(RuntimeSelfTest_GcAllocationProof__ctor),
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	reinterpret_cast<void*>(RuntimeSelfTest_GcAllocationProof_AllocateAndReturn_System_Int32),
	reinterpret_cast<void*>(RuntimeSelfTest_GcAllocationProof_AllocateAtomic_System_Int32),
	reinterpret_cast<void*>(RuntimeSelfTest_GcAllocationProof_get_Value),
	reinterpret_cast<void*>(RuntimeSelfTest_GcAllocationProof_set_Value_System_Int32),
};
extern "C" void RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0_ThreadedGcAllocation_b__0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
const void* chaos_vtable_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0[] =
{
	reinterpret_cast<void*>(RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__ctor),
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	reinterpret_cast<void*>(RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0_ThreadedGcAllocation_b__0),
};
extern "C" void RuntimeSelfTest_StringOperations__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INTPTR RuntimeSelfTest_StringOperations_ConcatStrings_System_String_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INTPTR RuntimeSelfTest_StringOperations_FormatString_System_String_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
const void* chaos_vtable_RuntimeSelfTest_StringOperations[] =
{
	reinterpret_cast<void*>(RuntimeSelfTest_StringOperations__ctor),
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
	reinterpret_cast<void*>(RuntimeSelfTest_StringOperations_ConcatStrings_System_String_System_String),
	reinterpret_cast<void*>(RuntimeSelfTest_StringOperations_FormatString_System_String_System_Int32),
};
static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_RuntimeSelfTest_ArrayOperations[] =
{
	{ 0x0000001Bu, reinterpret_cast<void*>(&RuntimeSelfTest_ArrayOperations__ctor) },
	{ 0x0000001Au, reinterpret_cast<void*>(&RuntimeSelfTest_ArrayOperations_CreateAndSum_System_Int32) },
	{ 0x00000019u, reinterpret_cast<void*>(&RuntimeSelfTest_ArrayOperations_CreateArray_System_Int32) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_RuntimeSelfTest_FinalizerProof[] =
{
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0x00000027u, reinterpret_cast<void*>(&RuntimeSelfTest_FinalizerProof__ctor_System_Int32) },
	{ 0x00000028u, reinterpret_cast<void*>(&RuntimeSelfTest_FinalizerProof_Finalize) },
	{ 0x00000026u, reinterpret_cast<void*>(&RuntimeSelfTest_FinalizerProof_get_Id) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_RuntimeSelfTest_GcAllocationProof[] =
{
	{ 0x00000020u, reinterpret_cast<void*>(&RuntimeSelfTest_GcAllocationProof__ctor) },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0x0000001Eu, reinterpret_cast<void*>(&RuntimeSelfTest_GcAllocationProof_AllocateAndReturn_System_Int32) },
	{ 0x0000001Fu, reinterpret_cast<void*>(&RuntimeSelfTest_GcAllocationProof_AllocateAtomic_System_Int32) },
	{ 0x0000001Cu, reinterpret_cast<void*>(&RuntimeSelfTest_GcAllocationProof_get_Value) },
	{ 0x0000001Du, reinterpret_cast<void*>(&RuntimeSelfTest_GcAllocationProof_set_Value_System_Int32) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0[] =
{
	{ 0x0000002Cu, reinterpret_cast<void*>(&RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__ctor) },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0x0000002Du, reinterpret_cast<void*>(&RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0_ThreadedGcAllocation_b__0) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_RuntimeSelfTest_StringOperations[] =
{
	{ 0x0000002Bu, reinterpret_cast<void*>(&RuntimeSelfTest_StringOperations__ctor) },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0x0000002Au, reinterpret_cast<void*>(&RuntimeSelfTest_StringOperations_ConcatStrings_System_String_System_String) },
	{ 0x00000029u, reinterpret_cast<void*>(&RuntimeSelfTest_StringOperations_FormatString_System_String_System_Int32) },
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

struct chaos_type_RuntimeSelfTest_ArrayOperations : public chaos_type_System_Private_CoreLib_System_Object
{
};

struct chaos_type_RuntimeSelfTest_FinalizerProof : public chaos_type_System_Private_CoreLib_System_Object
{
	CHAOS_IL2CPP_INTPTR field_RuntimeSelfTest_FinalizerProof___Id_k__BackingField = 0;
};

struct chaos_type_RuntimeSelfTest_GcAllocationProof : public chaos_type_System_Private_CoreLib_System_Object
{
	CHAOS_IL2CPP_INTPTR field_RuntimeSelfTest_GcAllocationProof___Value_k__BackingField = 0;
};

struct chaos_type_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0 : public chaos_type_System_Private_CoreLib_System_Object
{
	CHAOS_IL2CPP_INTPTR field_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__iterations = 0;
	CHAOS_IL2CPP_INTPTR field_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__sharedCounter = 0;
	CHAOS_IL2CPP_INTPTR field_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__sync = 0;
	CHAOS_IL2CPP_INTPTR field_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__workerDone = 0;
};

struct chaos_type_RuntimeSelfTest_StringOperations : public chaos_type_System_Private_CoreLib_System_Object
{
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

struct chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Int32_ : public chaos_type_System_Private_CoreLib_System_MulticastDelegate
{
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

struct chaos_type_System_Threading_Thread_System_Threading_Thread
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Threading_Thread_System_Threading_ThreadStart
{
	ThinLockableHeader header{};
};

const char* chaos_reflection_get_string_utf8(CHAOS_IL2CPP_INTPTR chaos_string_value) noexcept
{
	if (chaos_string_value == 0)
	{
		return nullptr;
	}

	if (chaos_is_string_id(chaos_string_value))
	{
		const auto chaos_view = chaos::il2cpp::string_table::Resolve(
			chaos_extract_string_id(chaos_string_value));
		return chaos_view.utf8_data;
	}

	auto* chaos_string = reinterpret_cast<chaos_type_System_Private_CoreLib_System_String*>(chaos_string_value);
	return chaos_string->utf8_data;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_create_string_utf8_copy(const char* chaos_utf8_data, CHAOS_IL2CPP_SIZE chaos_length)
{
	if (chaos_utf8_data == nullptr)
	{
		return 0;
	}

	const auto chaos_id = chaos::il2cpp::string_table::Intern(
		chaos_utf8_data, static_cast<CHAOS_IL2CPP_UINT32>(chaos_length));
	return chaos_make_string_id_value(chaos_id);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionConcatStringPairValues(CHAOS_IL2CPP_INTPTR chaos_left_string_value, CHAOS_IL2CPP_INTPTR chaos_right_string_value)
{
	const auto chaos_left_length =
		chaos_left_string_value == 0
			? static_cast<CHAOS_IL2CPP_SIZE>(0)
			: chaos_is_string_id(chaos_left_string_value)
				? static_cast<CHAOS_IL2CPP_SIZE>(chaos::il2cpp::string_table::Resolve(
					chaos_extract_string_id(chaos_left_string_value)).byte_count)
				: static_cast<CHAOS_IL2CPP_SIZE>(reinterpret_cast<chaos_type_System_Private_CoreLib_System_String*>(chaos_left_string_value)->length);
	const auto chaos_right_length =
		chaos_right_string_value == 0
			? static_cast<CHAOS_IL2CPP_SIZE>(0)
			: chaos_is_string_id(chaos_right_string_value)
				? static_cast<CHAOS_IL2CPP_SIZE>(chaos::il2cpp::string_table::Resolve(
					chaos_extract_string_id(chaos_right_string_value)).byte_count)
				: static_cast<CHAOS_IL2CPP_SIZE>(reinterpret_cast<chaos_type_System_Private_CoreLib_System_String*>(chaos_right_string_value)->length);
	const char* chaos_left_utf8 = chaos_reflection_get_string_utf8(chaos_left_string_value);
	const char* chaos_right_utf8 = chaos_reflection_get_string_utf8(chaos_right_string_value);
	if ((chaos_left_string_value != 0 && chaos_left_utf8 == nullptr && chaos_left_length != 0) ||
		(chaos_right_string_value != 0 && chaos_right_utf8 == nullptr && chaos_right_length != 0))
	{
		CHAOS_IL2CPP_FAIL();
	}

	const auto chaos_combined_length = chaos_left_length + chaos_right_length;
	auto* chaos_combined_utf8 = new char[chaos_combined_length + 1];
	if (chaos_left_length != 0)
	{
		CHAOS_IL2CPP_MEMCPY(chaos_combined_utf8, chaos_left_utf8, chaos_left_length);
	}
	if (chaos_right_length != 0)
	{
		CHAOS_IL2CPP_MEMCPY(chaos_combined_utf8 + chaos_left_length, chaos_right_utf8, chaos_right_length);
	}
	chaos_combined_utf8[chaos_combined_length] = '\0';
	const auto chaos_result = chaos_reflection_create_string_utf8_copy(chaos_combined_utf8, chaos_combined_length);
	delete[] chaos_combined_utf8;
	return chaos_result;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_create_string_literal(const char* chaos_utf8_data)
{
	if (chaos_utf8_data == nullptr)
	{
		return 0;
	}

	return chaos_reflection_create_string_utf8_copy(chaos_utf8_data, CHAOS_IL2CPP_STRLEN(chaos_utf8_data));
}


CHAOS_IL2CPP_INTPTR chaos_string_materialize(CHAOS_IL2CPP_INTPTR chaos_value) noexcept
{
	if (chaos_value == 0)
	{
		return 0;
	}

	if (!chaos_is_string_id(chaos_value))
	{
		return chaos_value;
	}

	const auto chaos_id = chaos_extract_string_id(chaos_value);
	const auto chaos_view = chaos::il2cpp::string_table::Resolve(chaos_id);

	auto* chaos_string = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_String, {});
	chaos_string->header.type_info = &chaos_mt_System_Private_CoreLib_System_String.hot;
	chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_view.byte_count);
	auto* owned_utf8 = static_cast<char*>(chaos::il2cpp::runtime_core::GcAllocateAtomic(chaos_view.byte_count + 1));
	CHAOS_IL2CPP_MEMCPY(owned_utf8, chaos_view.utf8_data, chaos_view.byte_count);
	owned_utf8[chaos_view.byte_count] = '\0';
	chaos_string->utf8_data = owned_utf8;
	chaos_string->string_id = chaos_id;
	return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
}


	// AOT-baked string table: sorted by StringId for binary search at runtime.
	constexpr chaos::il2cpp::string_table::StringEntry chaos_aot_string_entries[] = {
		{ 532744817614957869U, "RTS_INT_THREADED=", 17u },
		{ 2607821981565500683U, "hello", 5u },
		{ 2646729530017842629U, "RTS_EXC_HANDLED=-1", 18u },
		{ 3655909434593937953U, "RTS_GC_ALLOC=", 13u },
		{ 3687459647576096097U, "RTS_ARITH_ADD=", 14u },
		{ 4366065465556350155U, "RTS_INT_GC_ACROSS=", 18u },
		{ 5460223678947815943U, "RTS_GC_ALLOC_ATOMIC=", 20u },
		{ 5598294990661701711U, "RTS_ARITH_MUL=", 14u },
		{ 5717881983045765875U, "world", 5u },
		{ 6094863545746045109U, "RTS_INT_FINALIZER=", 18u },
		{ 6702458644621869811U, "RTS_INT_DELEGATE=", 17u },
		{ 6714048350784443879U, "RTS_ARR_SUM=", 12u },
		{ 6761683918713062125U, "RTS_STR_FORMAT=", 15u },
		{ 6764446324755329143U, "RTS_DONE=ok", 11u },
		{ 6810488739432363875U, "RTS_ARITH_SUB=", 14u },
		{ 8397726224804125835U, "RTS_ARR_LEN=", 12u },
		{ 8495190469562319739U, "RTS_EXC_UNEXPECTED=reached", 26u },
		{ 8687783789385879153U, "RTS_STR_CONCAT=", 15u },
	};

	constexpr CHAOS_IL2CPP_UINT32 chaos_aot_string_entry_count = sizeof(chaos_aot_string_entries) / sizeof(chaos_aot_string_entries[0]);

	// Register the AOT-baked string table with the runtime before any code uses it.
	static const CHAOS_IL2CPP_UINT32 s_aot_string_table_registered = []()
	{
		::chaos::il2cpp::string_table::InitializeFromAot(
			chaos_aot_string_entries,
			chaos_aot_string_entry_count);
		return 0u;
	}();

static void chaos_external_runtime_System_Console_System_Console__WriteLine_System_Void_System_String_(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
	if (chaos_arg_0 != 0) {
		const char* utf8 = nullptr;
		CHAOS_IL2CPP_UINT32 byte_count = 0;
		if (chaos_is_string_id(chaos_arg_0)) {
			auto sv = chaos::il2cpp::string_table::Resolve(chaos_extract_string_id(chaos_arg_0));
			if (sv.utf8_data != nullptr) {
				utf8 = sv.utf8_data;
				byte_count = sv.byte_count;
			}
		} else {
			auto* hdr = reinterpret_cast<const ::StubStringHeader*>(chaos_arg_0);
			utf8 = ::stub_string_data(reinterpret_cast<const void*>(chaos_arg_0));
			byte_count = static_cast<CHAOS_IL2CPP_UINT32>(hdr->byte_count);
		}
		if (utf8 != nullptr && byte_count > 0) {
			std::fwrite(utf8, 1, byte_count, stdout);
		}
		std::fputc('\n', stdout);
		std::fflush(stdout);
	}
}

static void chaos_external_runtime_System_Private_CoreLib_Monitor__Enter_System_Void_System_Object_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	chaos_monitor_enter(chaos_fn_arg_0, chaos_fn_arg_1);
}

static void chaos_external_runtime_System_Private_CoreLib_Monitor__Exit_System_Void_System_Object_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	chaos_monitor_exit(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Int32__ToString_System_String__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
	auto* chaos_value_slot = chaos_resolve_native_int_slot(chaos_arg_0);
	const CHAOS_IL2CPP_STRING chaos_formatted = ChaosIl2cpp::Common::format_int32(static_cast<CHAOS_IL2CPP_INT32>(*chaos_value_slot));
	const auto chaos_id = chaos::il2cpp::string_table::Intern(
		chaos_formatted.c_str(), static_cast<CHAOS_IL2CPP_UINT32>(chaos_formatted.size()));
	return chaos_make_string_id_value(chaos_id);
}

static void chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	ChaosObjectCtor(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_String__Concat_System_String_System_String_System_String_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	return ChaosReflectionConcatStringPairValues(chaos_fn_arg_0, chaos_fn_arg_1);
}

static void chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__Join_System_Void__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	chaos_thread_join(chaos_fn_arg_0);
}

static void chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__Sleep_System_Void_System_Int32_(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
{
	chaos_thread_sleep(chaos_fn_arg_0);
}

static void chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__Start_System_Void__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	chaos_thread_start(chaos_fn_arg_0);
}

static void chaos_external_runtime_System_Threading_Thread_System_Threading_Thread___ctor_System_Void_System_Threading_ThreadStart_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	chaos_thread_ctor(chaos_fn_arg_0, chaos_fn_arg_1);
}

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };

extern "C" CHAOS_IL2CPP_INT32 RuntimeSelfTest_ArithmeticProof_Add_System_Int32_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 RuntimeSelfTest_ArithmeticProof_Multiply_System_Int32_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 RuntimeSelfTest_ArithmeticProof_Subtract_System_Int32_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void RuntimeSelfTest_ArrayOperations__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 RuntimeSelfTest_ArrayOperations_CreateAndSum_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR RuntimeSelfTest_ArrayOperations_CreateArray_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void RuntimeSelfTest_AutoGeneratedProgram__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void RuntimeSelfTest_AutoGeneratedProgram_Main_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void RuntimeSelfTest_FinalizerProof__ctor_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void RuntimeSelfTest_FinalizerProof_Finalize(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 RuntimeSelfTest_FinalizerProof_get_Id(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void RuntimeSelfTest_GcAllocationProof__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 RuntimeSelfTest_GcAllocationProof_AllocateAndReturn_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 RuntimeSelfTest_GcAllocationProof_AllocateAtomic_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 RuntimeSelfTest_GcAllocationProof_get_Value(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void RuntimeSelfTest_GcAllocationProof_set_Value_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 RuntimeSelfTest_IntegrationProof_DoubleValue_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 RuntimeSelfTest_IntegrationProof_FinalizerAndNewAllocation_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 RuntimeSelfTest_IntegrationProof_GcAcrossMethodCalls_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 RuntimeSelfTest_IntegrationProof_GcAndDelegateInvoke_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 RuntimeSelfTest_IntegrationProof_ThreadedGcAllocation_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" void RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0_ThreadedGcAllocation_b__0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void RuntimeSelfTest_Program__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void RuntimeSelfTest_Program_Main_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void RuntimeSelfTest_StringOperations__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INTPTR RuntimeSelfTest_StringOperations_ConcatStrings_System_String_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INTPTR RuntimeSelfTest_StringOperations_FormatString_System_String_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[29] = {
	0u,
	2u,
	4u,
	6u,
	6u,
	7u,
	8u,
	8u,
	9u,
	10u,
	10u,
	10u,
	10u,
	11u,
	12u,
	12u,
	13u,
	14u,
	15u,
	16u,
	17u,
	18u,
	18u,
	18u,
	18u,
	19u,
	19u,
	21u,
	23u,
};

static constexpr struct {
	::ChaosAbiManifestV0 header;
	::ChaosAbiMethodEntryV0 entries[28];
	CHAOS_IL2CPP_UINT8 params[23];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		28u,
		23u,
		1176626461u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 1u, 2u },  // RuntimeSelfTest_ArithmeticProof_Add_System_Int32_System_Int32
		{ 1u, 2u },  // RuntimeSelfTest_ArithmeticProof_Multiply_System_Int32_System_Int32
		{ 1u, 2u },  // RuntimeSelfTest_ArithmeticProof_Subtract_System_Int32_System_Int32
		{ 0u, 0u },  // RuntimeSelfTest_ArrayOperations__ctor
		{ 1u, 1u },  // RuntimeSelfTest_ArrayOperations_CreateAndSum_System_Int32
		{ 2u, 1u },  // RuntimeSelfTest_ArrayOperations_CreateArray_System_Int32
		{ 0u, 0u },  // RuntimeSelfTest_AutoGeneratedProgram__ctor
		{ 0u, 1u },  // RuntimeSelfTest_AutoGeneratedProgram_Main_System_String
		{ 0u, 1u },  // RuntimeSelfTest_FinalizerProof__ctor_System_Int32
		{ 0u, 0u },  // RuntimeSelfTest_FinalizerProof_Finalize
		{ 1u, 0u },  // RuntimeSelfTest_FinalizerProof_get_Id
		{ 0u, 0u },  // RuntimeSelfTest_GcAllocationProof__ctor
		{ 1u, 1u },  // RuntimeSelfTest_GcAllocationProof_AllocateAndReturn_System_Int32
		{ 1u, 1u },  // RuntimeSelfTest_GcAllocationProof_AllocateAtomic_System_Int32
		{ 1u, 0u },  // RuntimeSelfTest_GcAllocationProof_get_Value
		{ 0u, 1u },  // RuntimeSelfTest_GcAllocationProof_set_Value_System_Int32
		{ 1u, 1u },  // RuntimeSelfTest_IntegrationProof_DoubleValue_System_Int32
		{ 1u, 1u },  // RuntimeSelfTest_IntegrationProof_FinalizerAndNewAllocation_System_Int32
		{ 1u, 1u },  // RuntimeSelfTest_IntegrationProof_GcAcrossMethodCalls_System_Int32
		{ 1u, 1u },  // RuntimeSelfTest_IntegrationProof_GcAndDelegateInvoke_System_Int32
		{ 1u, 1u },  // RuntimeSelfTest_IntegrationProof_ThreadedGcAllocation_System_Int32
		{ 0u, 0u },  // RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__ctor
		{ 0u, 0u },  // RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0_ThreadedGcAllocation_b__0
		{ 0u, 0u },  // RuntimeSelfTest_Program__ctor
		{ 0u, 1u },  // RuntimeSelfTest_Program_Main_System_String
		{ 0u, 0u },  // RuntimeSelfTest_StringOperations__ctor
		{ 2u, 2u },  // RuntimeSelfTest_StringOperations_ConcatStrings_System_String_System_String
		{ 2u, 2u },  // RuntimeSelfTest_StringOperations_FormatString_System_String_System_Int32
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
		2u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		2u,
		2u,
		2u,
		2u,
		1u,
	},
};
static const ::ChaosAbiManifestV0* const s_abi_manifest =
	reinterpret_cast<const ::ChaosAbiManifestV0*>(&s_abi_manifest_storage);
// ── Module registration ──

	static constexpr CHAOS_IL2CPP_UINT32 s_type_flags[10] = {
		0u,
		0u,
		0u,
		1548u,
		1536u,
		1536u,
		1548u,
		0u,
		1536u,
		1544u,
	};

	static constexpr const char* s_type_names[10] = {
		"<Module>",
		"Program",
		"AutoGeneratedProgram",
		"ArithmeticProof",
		"ArrayOperations",
		"GcAllocationProof",
		"IntegrationProof",
		"FinalizerProof",
		"StringOperations",
		"<>c__DisplayClass4_0",
	};

	static constexpr const char* s_type_namespaces[10] = {
		"",
		"",
		"",
		"RuntimeSelfTest",
		"RuntimeSelfTest",
		"RuntimeSelfTest",
		"RuntimeSelfTest",
		"RuntimeSelfTest",
		"RuntimeSelfTest",
		"",
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_type_parent_tokens[10] = {
		0u,
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

	static const TypeInfoHot* const s_type_info_ptrs[10] = {
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

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_children[1] = {
		33554442u,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_offset[11] = {
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
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_generic_param_constraint_offset[11] = {
		0u,
		0u,
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
		/* .name_utf8         = */ "RuntimeSelfTest",
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
		/* .type_count        = */ 10u,
	/* .custom_attribute_blob       = */ nullptr,  // Tier 1 — deferred
	/* .custom_attribute_offset     = */ nullptr,
	/* .custom_attribute_entity_count = */ 0u,
	/* .custom_attribute_materializer = */ nullptr,

		/* .abi_manifest      = */ s_abi_manifest,
	};
	static const CHAOS_IL2CPP_UINT32 s_native_aot_module_id =
		::chaos::il2cpp::runtime_core::RegisterModule("RuntimeSelfTest", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[28] = {
	{ "Add", 0x00000016u, 2u },  // ArithmeticProof
	{ "Multiply", 0x00000018u, 2u },  // ArithmeticProof
	{ "Subtract", 0x00000017u, 2u },  // ArithmeticProof
	{ ".ctor", 0x0000001Bu, 0u },  // ArrayOperations
	{ "CreateAndSum", 0x0000001Au, 1u },  // ArrayOperations
	{ "CreateArray", 0x00000019u, 1u },  // ArrayOperations
	{ ".ctor", 0x00000015u, 0u },  // AutoGeneratedProgram
	{ "Main", 0x00000014u, 1u },  // AutoGeneratedProgram
	{ ".ctor", 0x00000027u, 1u },  // FinalizerProof
	{ "Finalize", 0x00000028u, 0u },  // FinalizerProof
	{ "get_Id", 0x00000026u, 0u },  // FinalizerProof
	{ ".ctor", 0x00000020u, 0u },  // GcAllocationProof
	{ "AllocateAndReturn", 0x0000001Eu, 1u },  // GcAllocationProof
	{ "AllocateAtomic", 0x0000001Fu, 1u },  // GcAllocationProof
	{ "get_Value", 0x0000001Cu, 0u },  // GcAllocationProof
	{ "set_Value", 0x0000001Du, 1u },  // GcAllocationProof
	{ "DoubleValue", 0x00000022u, 1u },  // IntegrationProof
	{ "FinalizerAndNewAllocation", 0x00000023u, 1u },  // IntegrationProof
	{ "GcAcrossMethodCalls", 0x00000024u, 1u },  // IntegrationProof
	{ "GcAndDelegateInvoke", 0x00000021u, 1u },  // IntegrationProof
	{ "ThreadedGcAllocation", 0x00000025u, 1u },  // IntegrationProof
	{ ".ctor", 0x0000002Cu, 0u },  // IntegrationProof+<>c__DisplayClass4_0
	{ "<ThreadedGcAllocation>b__0", 0x0000002Du, 0u },  // IntegrationProof+<>c__DisplayClass4_0
	{ ".ctor", 0x00000013u, 0u },  // Program
	{ "<Main>$", 0x00000012u, 1u },  // Program
	{ ".ctor", 0x0000002Bu, 0u },  // StringOperations
	{ "ConcatStrings", 0x0000002Au, 2u },  // StringOperations
	{ "FormatString", 0x00000029u, 2u },  // StringOperations
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[9] = {
	{ "ArithmeticProof", "", 0u, 3u },
	{ "ArrayOperations", "", 3u, 3u },
	{ "AutoGeneratedProgram", "", 6u, 2u },
	{ "FinalizerProof", "", 8u, 3u },
	{ "GcAllocationProof", "", 11u, 5u },
	{ "IntegrationProof", "", 16u, 5u },
	{ "IntegrationProof+<>c__DisplayClass4_0", "", 21u, 2u },
	{ "Program", "", 23u, 2u },
	{ "StringOperations", "", 25u, 3u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[28] = {
	{ 0x00000012u, 24u },
	{ 0x00000013u, 23u },
	{ 0x00000014u, 7u },
	{ 0x00000015u, 6u },
	{ 0x00000016u, 0u },
	{ 0x00000017u, 2u },
	{ 0x00000018u, 1u },
	{ 0x00000019u, 5u },
	{ 0x0000001Au, 4u },
	{ 0x0000001Bu, 3u },
	{ 0x0000001Cu, 14u },
	{ 0x0000001Du, 15u },
	{ 0x0000001Eu, 12u },
	{ 0x0000001Fu, 13u },
	{ 0x00000020u, 11u },
	{ 0x00000021u, 19u },
	{ 0x00000022u, 16u },
	{ 0x00000023u, 17u },
	{ 0x00000024u, 18u },
	{ 0x00000025u, 20u },
	{ 0x00000026u, 10u },
	{ 0x00000027u, 8u },
	{ 0x00000028u, 9u },
	{ 0x00000029u, 27u },
	{ 0x0000002Au, 26u },
	{ 0x0000002Bu, 25u },
	{ 0x0000002Cu, 21u },
	{ 0x0000002Du, 22u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[28] = {
	{ reinterpret_cast<void*>(&RuntimeSelfTest_ArithmeticProof_Add_System_Int32_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ArithmeticProof::Add
	{ reinterpret_cast<void*>(&RuntimeSelfTest_ArithmeticProof_Multiply_System_Int32_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ArithmeticProof::Multiply
	{ reinterpret_cast<void*>(&RuntimeSelfTest_ArithmeticProof_Subtract_System_Int32_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ArithmeticProof::Subtract
	{ reinterpret_cast<void*>(&RuntimeSelfTest_ArrayOperations__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ArrayOperations::.ctor
	{ reinterpret_cast<void*>(&RuntimeSelfTest_ArrayOperations_CreateAndSum_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ArrayOperations::CreateAndSum
	{ reinterpret_cast<void*>(&RuntimeSelfTest_ArrayOperations_CreateArray_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ArrayOperations::CreateArray
	{ reinterpret_cast<void*>(&RuntimeSelfTest_AutoGeneratedProgram__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // AutoGeneratedProgram::.ctor
	{ reinterpret_cast<void*>(&RuntimeSelfTest_AutoGeneratedProgram_Main_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // AutoGeneratedProgram::Main
	{ reinterpret_cast<void*>(&RuntimeSelfTest_FinalizerProof__ctor_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // FinalizerProof::.ctor
	{ reinterpret_cast<void*>(&RuntimeSelfTest_FinalizerProof_Finalize), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // FinalizerProof::Finalize
	{ reinterpret_cast<void*>(&RuntimeSelfTest_FinalizerProof_get_Id), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // FinalizerProof::get_Id
	{ reinterpret_cast<void*>(&RuntimeSelfTest_GcAllocationProof__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // GcAllocationProof::.ctor
	{ reinterpret_cast<void*>(&RuntimeSelfTest_GcAllocationProof_AllocateAndReturn_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // GcAllocationProof::AllocateAndReturn
	{ reinterpret_cast<void*>(&RuntimeSelfTest_GcAllocationProof_AllocateAtomic_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // GcAllocationProof::AllocateAtomic
	{ reinterpret_cast<void*>(&RuntimeSelfTest_GcAllocationProof_get_Value), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // GcAllocationProof::get_Value
	{ reinterpret_cast<void*>(&RuntimeSelfTest_GcAllocationProof_set_Value_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // GcAllocationProof::set_Value
	{ reinterpret_cast<void*>(&RuntimeSelfTest_IntegrationProof_DoubleValue_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // IntegrationProof::DoubleValue
	{ reinterpret_cast<void*>(&RuntimeSelfTest_IntegrationProof_FinalizerAndNewAllocation_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // IntegrationProof::FinalizerAndNewAllocation
	{ reinterpret_cast<void*>(&RuntimeSelfTest_IntegrationProof_GcAcrossMethodCalls_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // IntegrationProof::GcAcrossMethodCalls
	{ reinterpret_cast<void*>(&RuntimeSelfTest_IntegrationProof_GcAndDelegateInvoke_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // IntegrationProof::GcAndDelegateInvoke
	{ reinterpret_cast<void*>(&RuntimeSelfTest_IntegrationProof_ThreadedGcAllocation_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // IntegrationProof::ThreadedGcAllocation
	{ reinterpret_cast<void*>(&RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // IntegrationProof+<>c__DisplayClass4_0::.ctor
	{ reinterpret_cast<void*>(&RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0_ThreadedGcAllocation_b__0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // IntegrationProof+<>c__DisplayClass4_0::<ThreadedGcAllocation>b__0
	{ reinterpret_cast<void*>(&RuntimeSelfTest_Program__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // Program::.ctor
	{ reinterpret_cast<void*>(&RuntimeSelfTest_Program_Main_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // Program::<Main>$
	{ reinterpret_cast<void*>(&RuntimeSelfTest_StringOperations__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // StringOperations::.ctor
	{ reinterpret_cast<void*>(&RuntimeSelfTest_StringOperations_ConcatStrings_System_String_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // StringOperations::ConcatStrings
	{ reinterpret_cast<void*>(&RuntimeSelfTest_StringOperations_FormatString_System_String_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // StringOperations::FormatString
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"RuntimeSelfTest",
	s_hotpatch_types,
	9u,
	s_hotpatch_methods,
	28u,
	s_hotpatch_slots,
	28u,
	s_hotpatch_entries,
	28u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[23] = {
	"System.Private.CoreLib/System.Object::.ctor:System.Void()",
	"System.Private.CoreLib/System.Int32",
	"RuntimeSelfTest/FinalizerProof::<Id>k__BackingField",
	"System.Private.CoreLib/System.Object::Finalize:System.Void()",
	"RuntimeSelfTest/GcAllocationProof::<Value>k__BackingField",
	"System.Private.CoreLib/System.GC::Collect:System.Void()",
	"System.Private.CoreLib/System.GC::WaitForPendingFinalizers:System.Void()",
	"System.Private.CoreLib/System.Func<System.Int32,System.Int32>::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Private.CoreLib/System.Func<System.Int32,System.Int32>::Invoke:System.Int32(System.Int32)",
	"RuntimeSelfTest/IntegrationProof+<>c__DisplayClass4_0::iterations",
	"RuntimeSelfTest/IntegrationProof+<>c__DisplayClass4_0::sync",
	"RuntimeSelfTest/IntegrationProof+<>c__DisplayClass4_0::sharedCounter",
	"RuntimeSelfTest/IntegrationProof+<>c__DisplayClass4_0::workerDone",
	"System.Threading.Thread/System.Threading.ThreadStart::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Threading.Thread/System.Threading.Thread::.ctor:System.Void(System.Threading.ThreadStart)",
	"System.Threading.Thread/System.Threading.Thread::Start:System.Void()",
	"System.Threading.Thread/System.Threading.Thread::Sleep:System.Void(System.Int32)",
	"System.Threading.Thread/System.Threading.Thread::Join:System.Void()",
	"System.Threading/Monitor::Enter:System.Void(System.Object,System.Boolean&)",
	"System.Threading/Monitor::Exit:System.Void(System.Object)",
	"System.Private.CoreLib/System.Int32::ToString:System.String()",
	"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)",
	"System.Console/System.Console::WriteLine:System.Void(System.String)",
};

extern "C" void* kChaosExternalRuntimeFnTable[23] = {
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__),
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
	reinterpret_cast<void*>(&chaos_external_runtime_System_Threading_Thread_System_Threading_Thread___ctor_System_Void_System_Threading_ThreadStart_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__Start_System_Void__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__Sleep_System_Void_System_Int32_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__Join_System_Void__),
	nullptr,
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Int32__ToString_System_String__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_String__Concat_System_String_System_String_System_String_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Console_System_Console__WriteLine_System_Void_System_String_),
};

extern "C" int32_t kChaosExternalRuntimeCount = 23;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[28])() = {
	reinterpret_cast<void(*)()>(&RuntimeSelfTest_ArithmeticProof_Add_System_Int32_System_Int32),
	reinterpret_cast<void(*)()>(&RuntimeSelfTest_ArithmeticProof_Multiply_System_Int32_System_Int32),
	reinterpret_cast<void(*)()>(&RuntimeSelfTest_ArithmeticProof_Subtract_System_Int32_System_Int32),
	reinterpret_cast<void(*)()>(&RuntimeSelfTest_ArrayOperations__ctor),
	reinterpret_cast<void(*)()>(&RuntimeSelfTest_ArrayOperations_CreateAndSum_System_Int32),
	reinterpret_cast<void(*)()>(&RuntimeSelfTest_ArrayOperations_CreateArray_System_Int32),
	reinterpret_cast<void(*)()>(&RuntimeSelfTest_AutoGeneratedProgram__ctor),
	reinterpret_cast<void(*)()>(&RuntimeSelfTest_AutoGeneratedProgram_Main_System_String),
	reinterpret_cast<void(*)()>(&RuntimeSelfTest_FinalizerProof__ctor_System_Int32),
	reinterpret_cast<void(*)()>(&RuntimeSelfTest_FinalizerProof_Finalize),
	reinterpret_cast<void(*)()>(&RuntimeSelfTest_FinalizerProof_get_Id),
	reinterpret_cast<void(*)()>(&RuntimeSelfTest_GcAllocationProof__ctor),
	reinterpret_cast<void(*)()>(&RuntimeSelfTest_GcAllocationProof_AllocateAndReturn_System_Int32),
	reinterpret_cast<void(*)()>(&RuntimeSelfTest_GcAllocationProof_AllocateAtomic_System_Int32),
	reinterpret_cast<void(*)()>(&RuntimeSelfTest_GcAllocationProof_get_Value),
	reinterpret_cast<void(*)()>(&RuntimeSelfTest_GcAllocationProof_set_Value_System_Int32),
	reinterpret_cast<void(*)()>(&RuntimeSelfTest_IntegrationProof_DoubleValue_System_Int32),
	reinterpret_cast<void(*)()>(&RuntimeSelfTest_IntegrationProof_FinalizerAndNewAllocation_System_Int32),
	reinterpret_cast<void(*)()>(&RuntimeSelfTest_IntegrationProof_GcAcrossMethodCalls_System_Int32),
	reinterpret_cast<void(*)()>(&RuntimeSelfTest_IntegrationProof_GcAndDelegateInvoke_System_Int32),
	reinterpret_cast<void(*)()>(&RuntimeSelfTest_IntegrationProof_ThreadedGcAllocation_System_Int32),
	reinterpret_cast<void(*)()>(&RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__ctor),
	reinterpret_cast<void(*)()>(&RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0_ThreadedGcAllocation_b__0),
	reinterpret_cast<void(*)()>(&RuntimeSelfTest_Program__ctor),
	reinterpret_cast<void(*)()>(&RuntimeSelfTest_Program_Main_System_String),
	reinterpret_cast<void(*)()>(&RuntimeSelfTest_StringOperations__ctor),
	reinterpret_cast<void(*)()>(&RuntimeSelfTest_StringOperations_ConcatStrings_System_String_System_String),
	reinterpret_cast<void(*)()>(&RuntimeSelfTest_StringOperations_FormatString_System_String_System_Int32),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
extern "C" void (*kBenchmarkWrappers[28])() = {
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[0])(0,0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[1])(0,0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[2])(0,0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[3])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[4])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[5])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[6])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[7])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[8])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[9])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[10])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[11])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[12])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[13])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[14])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[15])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[16])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[17])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[18])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[19])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[20])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[21])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[22])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[23])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[24])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[25])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[26])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),chaos_make_string_id_value(8397726224804125835ULL),chaos_make_string_id_value(8397726224804125835ULL));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[27])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),chaos_make_string_id_value(8397726224804125835ULL),0);},
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

	/* ── Entry 0: RuntimeSelfTest_ArithmeticProof_Add_System_Int32_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry0;
	/* ── Entry 1: RuntimeSelfTest_ArithmeticProof_Multiply_System_Int32_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry1;
	/* ── Entry 2: RuntimeSelfTest_ArithmeticProof_Subtract_System_Int32_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry2;
	/* ── Entry 3: RuntimeSelfTest_ArrayOperations__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry3;
	/* ── Entry 4: RuntimeSelfTest_ArrayOperations_CreateAndSum_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[9];
	} entry4;
	/* ── Entry 5: RuntimeSelfTest_ArrayOperations_CreateArray_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry5;
	/* ── Entry 6: RuntimeSelfTest_AutoGeneratedProgram__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry6;
	/* ── Entry 7: RuntimeSelfTest_AutoGeneratedProgram_Main_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry7;
	/* ── Entry 8: RuntimeSelfTest_FinalizerProof__ctor_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry8;
	/* ── Entry 9: RuntimeSelfTest_FinalizerProof_Finalize ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry9;
	/* ── Entry 10: RuntimeSelfTest_FinalizerProof_get_Id ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry10;
	/* ── Entry 11: RuntimeSelfTest_GcAllocationProof__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry11;
	/* ── Entry 12: RuntimeSelfTest_GcAllocationProof_AllocateAndReturn_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry12;
	/* ── Entry 13: RuntimeSelfTest_GcAllocationProof_AllocateAtomic_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry13;
	/* ── Entry 14: RuntimeSelfTest_GcAllocationProof_get_Value ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry14;
	/* ── Entry 15: RuntimeSelfTest_GcAllocationProof_set_Value_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry15;
	/* ── Entry 16: RuntimeSelfTest_IntegrationProof_DoubleValue_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry16;
	/* ── Entry 17: RuntimeSelfTest_IntegrationProof_FinalizerAndNewAllocation_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry17;
	/* ── Entry 18: RuntimeSelfTest_IntegrationProof_GcAcrossMethodCalls_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[11];
	} entry18;
	/* ── Entry 19: RuntimeSelfTest_IntegrationProof_GcAndDelegateInvoke_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry19;
	/* ── Entry 20: RuntimeSelfTest_IntegrationProof_ThreadedGcAllocation_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[10];
	} entry20;
	/* ── Entry 21: RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry21;
	/* ── Entry 22: RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0_ThreadedGcAllocation_b__0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[12];
	} entry22;
	/* ── Entry 23: RuntimeSelfTest_Program__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry23;
	/* ── Entry 24: RuntimeSelfTest_Program_Main_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[11];
	} entry24;
	/* ── Entry 25: RuntimeSelfTest_StringOperations__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry25;
	/* ── Entry 26: RuntimeSelfTest_StringOperations_ConcatStrings_System_String_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry26;
	/* ── Entry 27: RuntimeSelfTest_StringOperations_FormatString_System_String_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry27;
} kChaosGcSlotMapsSection = {
	/* entry0 = RuntimeSelfTest_ArithmeticProof_Add_System_Int32_System_Int32 */
	.entry0 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&RuntimeSelfTest_ArithmeticProof_Add_System_Int32_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 16u, 24u }
	},
	/* entry1 = RuntimeSelfTest_ArithmeticProof_Multiply_System_Int32_System_Int32 */
	.entry1 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&RuntimeSelfTest_ArithmeticProof_Multiply_System_Int32_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 16u, 24u }
	},
	/* entry2 = RuntimeSelfTest_ArithmeticProof_Subtract_System_Int32_System_Int32 */
	.entry2 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&RuntimeSelfTest_ArithmeticProof_Subtract_System_Int32_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 16u, 24u }
	},
	/* entry3 = RuntimeSelfTest_ArrayOperations__ctor */
	.entry3 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&RuntimeSelfTest_ArrayOperations__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry4 = RuntimeSelfTest_ArrayOperations_CreateAndSum_System_Int32 */
	.entry4 = {
		/* entry_total_size = 56 */ 56u,
		/* code_address */ reinterpret_cast<const void*>(&RuntimeSelfTest_ArrayOperations_CreateAndSum_System_Int32),
		/* frame_size = 80 */ 80u,
		/* num_gc_slots = 9 */ 9u,
		/* slots */ { 0u, 16u, 24u, 32u, 40u, 48u, 56u, 64u, 72u }
	},
	/* entry5 = RuntimeSelfTest_ArrayOperations_CreateArray_System_Int32 */
	.entry5 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&RuntimeSelfTest_ArrayOperations_CreateArray_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 16u, 24u }
	},
	/* entry6 = RuntimeSelfTest_AutoGeneratedProgram__ctor */
	.entry6 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&RuntimeSelfTest_AutoGeneratedProgram__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry7 = RuntimeSelfTest_AutoGeneratedProgram_Main_System_String */
	.entry7 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&RuntimeSelfTest_AutoGeneratedProgram_Main_System_String),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry8 = RuntimeSelfTest_FinalizerProof__ctor_System_Int32 */
	.entry8 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&RuntimeSelfTest_FinalizerProof__ctor_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 16u, 24u }
	},
	/* entry9 = RuntimeSelfTest_FinalizerProof_Finalize */
	.entry9 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&RuntimeSelfTest_FinalizerProof_Finalize),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry10 = RuntimeSelfTest_FinalizerProof_get_Id */
	.entry10 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&RuntimeSelfTest_FinalizerProof_get_Id),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry11 = RuntimeSelfTest_GcAllocationProof__ctor */
	.entry11 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&RuntimeSelfTest_GcAllocationProof__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry12 = RuntimeSelfTest_GcAllocationProof_AllocateAndReturn_System_Int32 */
	.entry12 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&RuntimeSelfTest_GcAllocationProof_AllocateAndReturn_System_Int32),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 16u, 24u, 32u, 40u }
	},
	/* entry13 = RuntimeSelfTest_GcAllocationProof_AllocateAtomic_System_Int32 */
	.entry13 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&RuntimeSelfTest_GcAllocationProof_AllocateAtomic_System_Int32),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry14 = RuntimeSelfTest_GcAllocationProof_get_Value */
	.entry14 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&RuntimeSelfTest_GcAllocationProof_get_Value),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry15 = RuntimeSelfTest_GcAllocationProof_set_Value_System_Int32 */
	.entry15 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&RuntimeSelfTest_GcAllocationProof_set_Value_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 16u, 24u }
	},
	/* entry16 = RuntimeSelfTest_IntegrationProof_DoubleValue_System_Int32 */
	.entry16 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&RuntimeSelfTest_IntegrationProof_DoubleValue_System_Int32),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 8u, 16u }
	},
	/* entry17 = RuntimeSelfTest_IntegrationProof_FinalizerAndNewAllocation_System_Int32 */
	.entry17 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&RuntimeSelfTest_IntegrationProof_FinalizerAndNewAllocation_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 8u, 16u, 24u, 32u }
	},
	/* entry18 = RuntimeSelfTest_IntegrationProof_GcAcrossMethodCalls_System_Int32 */
	.entry18 = {
		/* entry_total_size = 64 */ 64u,
		/* code_address */ reinterpret_cast<const void*>(&RuntimeSelfTest_IntegrationProof_GcAcrossMethodCalls_System_Int32),
		/* frame_size = 96 */ 96u,
		/* num_gc_slots = 11 */ 11u,
		/* slots */ { 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u, 72u, 80u, 88u }
	},
	/* entry19 = RuntimeSelfTest_IntegrationProof_GcAndDelegateInvoke_System_Int32 */
	.entry19 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&RuntimeSelfTest_IntegrationProof_GcAndDelegateInvoke_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 8u, 16u, 24u }
	},
	/* entry20 = RuntimeSelfTest_IntegrationProof_ThreadedGcAllocation_System_Int32 */
	.entry20 = {
		/* entry_total_size = 60 */ 60u,
		/* code_address */ reinterpret_cast<const void*>(&RuntimeSelfTest_IntegrationProof_ThreadedGcAllocation_System_Int32),
		/* frame_size = 88 */ 88u,
		/* num_gc_slots = 10 */ 10u,
		/* slots */ { 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u, 72u, 80u }
	},
	/* entry21 = RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__ctor */
	.entry21 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry22 = RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0_ThreadedGcAllocation_b__0 */
	.entry22 = {
		/* entry_total_size = 68 */ 68u,
		/* code_address */ reinterpret_cast<const void*>(&RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0_ThreadedGcAllocation_b__0),
		/* frame_size = 96 */ 96u,
		/* num_gc_slots = 12 */ 12u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u, 72u, 80u, 88u }
	},
	/* entry23 = RuntimeSelfTest_Program__ctor */
	.entry23 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&RuntimeSelfTest_Program__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry24 = RuntimeSelfTest_Program_Main_System_String */
	.entry24 = {
		/* entry_total_size = 64 */ 64u,
		/* code_address */ reinterpret_cast<const void*>(&RuntimeSelfTest_Program_Main_System_String),
		/* frame_size = 88 */ 88u,
		/* num_gc_slots = 11 */ 11u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u, 72u, 80u }
	},
	/* entry25 = RuntimeSelfTest_StringOperations__ctor */
	.entry25 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&RuntimeSelfTest_StringOperations__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry26 = RuntimeSelfTest_StringOperations_ConcatStrings_System_String_System_String */
	.entry26 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&RuntimeSelfTest_StringOperations_ConcatStrings_System_String_System_String),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry27 = RuntimeSelfTest_StringOperations_FormatString_System_String_System_Int32 */
	.entry27 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&RuntimeSelfTest_StringOperations_FormatString_System_String_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 24u, 32u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 1036u;

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[28] = {
	reinterpret_cast<void*>(&RuntimeSelfTest_ArithmeticProof_Add_System_Int32_System_Int32),
	reinterpret_cast<void*>(&RuntimeSelfTest_ArithmeticProof_Multiply_System_Int32_System_Int32),
	reinterpret_cast<void*>(&RuntimeSelfTest_ArithmeticProof_Subtract_System_Int32_System_Int32),
	reinterpret_cast<void*>(&RuntimeSelfTest_ArrayOperations__ctor),
	reinterpret_cast<void*>(&RuntimeSelfTest_ArrayOperations_CreateAndSum_System_Int32),
	reinterpret_cast<void*>(&RuntimeSelfTest_ArrayOperations_CreateArray_System_Int32),
	reinterpret_cast<void*>(&RuntimeSelfTest_AutoGeneratedProgram__ctor),
	reinterpret_cast<void*>(&RuntimeSelfTest_AutoGeneratedProgram_Main_System_String),
	reinterpret_cast<void*>(&RuntimeSelfTest_FinalizerProof__ctor_System_Int32),
	reinterpret_cast<void*>(&RuntimeSelfTest_FinalizerProof_Finalize),
	reinterpret_cast<void*>(&RuntimeSelfTest_FinalizerProof_get_Id),
	reinterpret_cast<void*>(&RuntimeSelfTest_GcAllocationProof__ctor),
	reinterpret_cast<void*>(&RuntimeSelfTest_GcAllocationProof_AllocateAndReturn_System_Int32),
	reinterpret_cast<void*>(&RuntimeSelfTest_GcAllocationProof_AllocateAtomic_System_Int32),
	reinterpret_cast<void*>(&RuntimeSelfTest_GcAllocationProof_get_Value),
	reinterpret_cast<void*>(&RuntimeSelfTest_GcAllocationProof_set_Value_System_Int32),
	reinterpret_cast<void*>(&RuntimeSelfTest_IntegrationProof_DoubleValue_System_Int32),
	reinterpret_cast<void*>(&RuntimeSelfTest_IntegrationProof_FinalizerAndNewAllocation_System_Int32),
	reinterpret_cast<void*>(&RuntimeSelfTest_IntegrationProof_GcAcrossMethodCalls_System_Int32),
	reinterpret_cast<void*>(&RuntimeSelfTest_IntegrationProof_GcAndDelegateInvoke_System_Int32),
	reinterpret_cast<void*>(&RuntimeSelfTest_IntegrationProof_ThreadedGcAllocation_System_Int32),
	reinterpret_cast<void*>(&RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__ctor),
	reinterpret_cast<void*>(&RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0_ThreadedGcAllocation_b__0),
	reinterpret_cast<void*>(&RuntimeSelfTest_Program__ctor),
	reinterpret_cast<void*>(&RuntimeSelfTest_Program_Main_System_String),
	reinterpret_cast<void*>(&RuntimeSelfTest_StringOperations__ctor),
	reinterpret_cast<void*>(&RuntimeSelfTest_StringOperations_ConcatStrings_System_String_System_String),
	reinterpret_cast<void*>(&RuntimeSelfTest_StringOperations_FormatString_System_String_System_Int32),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
// ── VTable descriptors (for BootstrapRuntime TypeVTable registration) ──
static const VTableDescriptorV0 kChaosVTableDescriptors[] = {
	{
		CHAOS_IL2CPP_UINT64_C(0xB8B72895D904CBE5),
		0x00000004u,
		0u,
		3u,
		reinterpret_cast<const void*>(kSlots_RuntimeSelfTest_ArrayOperations),
		reinterpret_cast<const void**>(chaos_vtable_RuntimeSelfTest_ArrayOperations),
		3u,
		1,
		{0, 0, 0},
		nullptr,
		0u,
	},
	{
		CHAOS_IL2CPP_UINT64_C(0x19EA55498D5DD18C),
		0x00000007u,
		0u,
		6u,
		reinterpret_cast<const void*>(kSlots_RuntimeSelfTest_FinalizerProof),
		reinterpret_cast<const void**>(chaos_vtable_RuntimeSelfTest_FinalizerProof),
		6u,
		1,
		{0, 0, 0},
		nullptr,
		0u,
	},
	{
		CHAOS_IL2CPP_UINT64_C(0x522BD93C44AA7F00),
		0x00000005u,
		0u,
		10u,
		reinterpret_cast<const void*>(kSlots_RuntimeSelfTest_GcAllocationProof),
		reinterpret_cast<const void**>(chaos_vtable_RuntimeSelfTest_GcAllocationProof),
		10u,
		1,
		{0, 0, 0},
		nullptr,
		0u,
	},
	{
		CHAOS_IL2CPP_UINT64_C(0x8B29F9F0E1F326D1),
		0x00000009u,
		0u,
		11u,
		reinterpret_cast<const void*>(kSlots_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0),
		reinterpret_cast<const void**>(chaos_vtable_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0),
		11u,
		1,
		{0, 0, 0},
		nullptr,
		0u,
	},
	{
		CHAOS_IL2CPP_UINT64_C(0xF6BE3591459DA92D),
		0x00000008u,
		0u,
		13u,
		reinterpret_cast<const void*>(kSlots_RuntimeSelfTest_StringOperations),
		reinterpret_cast<const void**>(chaos_vtable_RuntimeSelfTest_StringOperations),
		13u,
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
	.method_pointer_count      = 28u,
	.reverse_pinvoke_wrappers  = nullptr,
	.reverse_pinvoke_wrapper_count = 0u,
	.invoker_pointers          = nullptr,
	.invoker_pointer_count     = 0u,
	.unresolved_virtual_calls = nullptr,
	.unresolved_virtual_call_count = 0u,
	.type_capabilities       = nullptr,
	.type_capability_count   = 0u,
	.vtable_descriptors = kChaosVTableDescriptors,
	.vtable_descriptor_count = 5u,
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
	.image_name_utf8    = "RuntimeSelfTest",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryMethodDescriptor kReflMethods_RuntimeSelfTest_ArithmeticProof[3] = {
	{ 0u, "RuntimeSelfTest/ArithmeticProof::Add:System.Int32(System.Int32,System.Int32)", "Add", "System.Void", 0, nullptr, 0u },
	{ 0u, "RuntimeSelfTest/ArithmeticProof::Multiply:System.Int32(System.Int32,System.Int32)", "Multiply", "System.Void", 0, nullptr, 0u },
	{ 0u, "RuntimeSelfTest/ArithmeticProof::Subtract:System.Int32(System.Int32,System.Int32)", "Subtract", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_RuntimeSelfTest_ArrayOperations[3] = {
	{ 0u, "RuntimeSelfTest/ArrayOperations::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "RuntimeSelfTest/ArrayOperations::CreateAndSum:System.Int32(System.Int32)", "CreateAndSum", "System.Void", 0, nullptr, 0u },
	{ 0u, "RuntimeSelfTest/ArrayOperations::CreateArray:System.Int32[](System.Int32)", "CreateArray", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_RuntimeSelfTest_AutoGeneratedProgram[2] = {
	{ 0u, "RuntimeSelfTest/AutoGeneratedProgram::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "RuntimeSelfTest/AutoGeneratedProgram::Main:System.Void(System.String[])", "Main", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_RuntimeSelfTest_FinalizerProof[3] = {
	{ 0u, "RuntimeSelfTest/FinalizerProof::.ctor:System.Void(System.Int32)", ".ctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "RuntimeSelfTest/FinalizerProof::Finalize:System.Void()", "Finalize", "System.Void", 0, nullptr, 0u },
	{ 0u, "RuntimeSelfTest/FinalizerProof::get_Id:System.Int32()", "get_Id", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_RuntimeSelfTest_GcAllocationProof[5] = {
	{ 0u, "RuntimeSelfTest/GcAllocationProof::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "RuntimeSelfTest/GcAllocationProof::AllocateAndReturn:System.Int32(System.Int32)", "AllocateAndReturn", "System.Void", 0, nullptr, 0u },
	{ 0u, "RuntimeSelfTest/GcAllocationProof::AllocateAtomic:System.Int32(System.Int32)", "AllocateAtomic", "System.Void", 0, nullptr, 0u },
	{ 0u, "RuntimeSelfTest/GcAllocationProof::get_Value:System.Int32()", "get_Value", "System.Void", 0, nullptr, 0u },
	{ 0u, "RuntimeSelfTest/GcAllocationProof::set_Value:System.Void(System.Int32)", "set_Value", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_RuntimeSelfTest_IntegrationProof[5] = {
	{ 0u, "RuntimeSelfTest/IntegrationProof::DoubleValue:System.Int32(System.Int32)", "DoubleValue", "System.Void", 0, nullptr, 0u },
	{ 0u, "RuntimeSelfTest/IntegrationProof::FinalizerAndNewAllocation:System.Int32(System.Int32)", "FinalizerAndNewAllocation", "System.Void", 0, nullptr, 0u },
	{ 0u, "RuntimeSelfTest/IntegrationProof::GcAcrossMethodCalls:System.Int32(System.Int32)", "GcAcrossMethodCalls", "System.Void", 0, nullptr, 0u },
	{ 0u, "RuntimeSelfTest/IntegrationProof::GcAndDelegateInvoke:System.Int32(System.Int32)", "GcAndDelegateInvoke", "System.Void", 0, nullptr, 0u },
	{ 0u, "RuntimeSelfTest/IntegrationProof::ThreadedGcAllocation:System.Int32(System.Int32)", "ThreadedGcAllocation", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0[2] = {
	{ 0u, "RuntimeSelfTest/IntegrationProof+<>c__DisplayClass4_0::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "RuntimeSelfTest/IntegrationProof+<>c__DisplayClass4_0::<ThreadedGcAllocation>b__0:System.Void()", "<ThreadedGcAllocation>b__0", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_RuntimeSelfTest_Program[2] = {
	{ 0u, "RuntimeSelfTest/Program::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "RuntimeSelfTest/Program::<Main>$:System.Void(System.String[])", "<Main>$", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_RuntimeSelfTest_StringOperations[3] = {
	{ 0u, "RuntimeSelfTest/StringOperations::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "RuntimeSelfTest/StringOperations::ConcatStrings:System.String(System.String,System.String)", "ConcatStrings", "System.Void", 0, nullptr, 0u },
	{ 0u, "RuntimeSelfTest/StringOperations::FormatString:System.String(System.String,System.Int32)", "FormatString", "System.Void", 0, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[9] = {
	{ 0u, "RuntimeSelfTest/ArithmeticProof", "RuntimeSelfTest/ArithmeticProof", "", "ArithmeticProof", "ArithmeticProof", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_RuntimeSelfTest_ArithmeticProof, 3u },
	{ 0u, "RuntimeSelfTest/ArrayOperations", "RuntimeSelfTest/ArrayOperations", "", "ArrayOperations", "ArrayOperations", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_RuntimeSelfTest_ArrayOperations, 3u },
	{ 0u, "RuntimeSelfTest/AutoGeneratedProgram", "RuntimeSelfTest/AutoGeneratedProgram", "", "AutoGeneratedProgram", "AutoGeneratedProgram", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_RuntimeSelfTest_AutoGeneratedProgram, 2u },
	{ 0u, "RuntimeSelfTest/FinalizerProof", "RuntimeSelfTest/FinalizerProof", "", "FinalizerProof", "FinalizerProof", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_RuntimeSelfTest_FinalizerProof, 3u },
	{ 0u, "RuntimeSelfTest/GcAllocationProof", "RuntimeSelfTest/GcAllocationProof", "", "GcAllocationProof", "GcAllocationProof", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_RuntimeSelfTest_GcAllocationProof, 5u },
	{ 0u, "RuntimeSelfTest/IntegrationProof", "RuntimeSelfTest/IntegrationProof", "", "IntegrationProof", "IntegrationProof", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_RuntimeSelfTest_IntegrationProof, 5u },
	{ 0u, "RuntimeSelfTest/IntegrationProof+<>c__DisplayClass4_0", "RuntimeSelfTest/IntegrationProof+<>c__DisplayClass4_0", "", "IntegrationProof+<>c__DisplayClass4_0", "IntegrationProof+<>c__DisplayClass4_0", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0, 2u },
	{ 0u, "RuntimeSelfTest/Program", "RuntimeSelfTest/Program", "", "Program", "Program", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_RuntimeSelfTest_Program, 2u },
	{ 0u, "RuntimeSelfTest/StringOperations", "RuntimeSelfTest/StringOperations", "", "StringOperations", "StringOperations", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_RuntimeSelfTest_StringOperations, 3u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[9] = {
	&kReflTypes[0],
	&kReflTypes[1],
	&kReflTypes[2],
	&kReflTypes[3],
	&kReflTypes[4],
	&kReflTypes[5],
	&kReflTypes[6],
	&kReflTypes[7],
	&kReflTypes[8],
};

static const ReflectionQueryImageDescriptor kReflImage = { "RuntimeSelfTest", kReflTypePtrs, 9u };

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
// Managed method: RuntimeSelfTest/ArithmeticProof::Add(System.Int32,System.Int32)
extern "C" CHAOS_IL2CPP_INT32 RuntimeSelfTest_ArithmeticProof_Add_System_Int32_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapMul(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: RuntimeSelfTest/ArithmeticProof::Multiply(System.Int32,System.Int32)
extern "C" CHAOS_IL2CPP_INT32 RuntimeSelfTest_ArithmeticProof_Multiply_System_Int32_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapMul(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: RuntimeSelfTest/ArithmeticProof::Subtract(System.Int32,System.Int32)
extern "C" CHAOS_IL2CPP_INT32 RuntimeSelfTest_ArithmeticProof_Subtract_System_Int32_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapSub(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: RuntimeSelfTest/ArrayOperations::.ctor()
extern "C" void RuntimeSelfTest_ArrayOperations__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		ChaosObjectCtor(chaos_arg_0);
	}
	return;
}

// Managed method: RuntimeSelfTest/ArrayOperations::CreateAndSum(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 RuntimeSelfTest_ArrayOperations_CreateAndSum_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 4) chaos_locals{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 4) chaos_eval_stack{};
	CHAOS_IL2CPP_SIZE chaos_stack_top = 0;
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

chaos_label_0:
	chaos_eval_stack[chaos_stack_top++] = chaos_args[1];
chaos_label_1:
	{
		const auto chaos_length = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
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
		chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
chaos_label_6:
	chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
chaos_label_7:
	chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
chaos_label_8:
	chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
chaos_label_9:
	goto chaos_label_21;
chaos_label_11:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
chaos_label_12:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[2];
chaos_label_13:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[2];
chaos_label_14:
	chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
chaos_label_15:
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(chaos_left, chaos_right));
	}
chaos_label_16:
	{
		auto chaos_value_raw = chaos_eval_stack[--chaos_stack_top];
		const auto chaos_value = chaos_value_raw;
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_eval_stack[--chaos_stack_top]);
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
chaos_label_17:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[2];
chaos_label_18:
	chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
chaos_label_19:
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(chaos_left, chaos_right));
	}
chaos_label_20:
	chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
chaos_label_21:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[2];
chaos_label_22:
	chaos_eval_stack[chaos_stack_top++] = chaos_args[1];
chaos_label_23:
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		if (chaos_left < chaos_right)
			goto chaos_label_11;
	}
chaos_label_25:
	chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
chaos_label_26:
	chaos_locals[1] = chaos_eval_stack[--chaos_stack_top];
chaos_label_27:
	chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
chaos_label_28:
	chaos_locals[3] = chaos_eval_stack[--chaos_stack_top];
chaos_label_29:
	goto chaos_label_41;
chaos_label_31:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[1];
chaos_label_32:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
chaos_label_33:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[3];
chaos_label_34:
	{
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_eval_stack[--chaos_stack_top]);
		if (chaos_array == nullptr)
		{
			CHAOS_IL2CPP_FAIL();
		}
		if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_element = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];
		chaos_eval_stack[chaos_stack_top++] = chaos_element;
	}
chaos_label_35:
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(chaos_left, chaos_right));
	}
chaos_label_36:
	chaos_locals[1] = chaos_eval_stack[--chaos_stack_top];
chaos_label_37:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[3];
chaos_label_38:
	chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
chaos_label_39:
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(chaos_left, chaos_right));
	}
chaos_label_40:
	chaos_locals[3] = chaos_eval_stack[--chaos_stack_top];
chaos_label_41:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[3];
chaos_label_42:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
chaos_label_43:
	chaos_eval_stack[chaos_stack_top++] = [&](){ auto* _c_arr = reinterpret_cast<chaos_managed_array*>(chaos_eval_stack[--chaos_stack_top]); return _c_arr ? static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_c_arr->length)) : CHAOS_IL2CPP_INTPTR{}; }();
chaos_label_44:
	chaos_eval_stack[chaos_stack_top - 1] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[chaos_stack_top - 1]));
chaos_label_45:
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		if (chaos_left < chaos_right)
			goto chaos_label_31;
	}
chaos_label_47:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[1];
chaos_label_48:
	return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
}

// Managed method: RuntimeSelfTest/ArrayOperations::CreateArray(System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR RuntimeSelfTest_ArrayOperations_CreateArray_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
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
	return _s0;
}

// Managed method: RuntimeSelfTest/AutoGeneratedProgram::.ctor()
extern "C" void RuntimeSelfTest_AutoGeneratedProgram__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		ChaosObjectCtor(chaos_arg_0);
	}
	return;
}

// Managed method: RuntimeSelfTest/AutoGeneratedProgram::Main(System.String[])
extern "C" void RuntimeSelfTest_AutoGeneratedProgram_Main_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	return;
}

// Managed method: RuntimeSelfTest/FinalizerProof::.ctor(System.Int32)
extern "C" void RuntimeSelfTest_FinalizerProof__ctor_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
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
		ChaosObjectCtor(chaos_arg_0);
	}
	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	{
		auto chaos_value = _s1;
		if (chaos_is_string_id(chaos_value))
		{
			chaos_value = chaos_string_materialize(chaos_value);
		}
		auto* chaos_object = reinterpret_cast<chaos_type_RuntimeSelfTest_FinalizerProof*>(_s0);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_object->field_RuntimeSelfTest_FinalizerProof___Id_k__BackingField));
		chaos_object->field_RuntimeSelfTest_FinalizerProof___Id_k__BackingField = chaos_value;
		chaos_gc_dirty_card(chaos_object);
	}
	return;
}

// Managed method: RuntimeSelfTest/FinalizerProof::Finalize()
extern "C" void RuntimeSelfTest_FinalizerProof_Finalize(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

		auto _chaos_finally = [&]()
		{
			_s0 = chaos_args[0];
			{
				reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[3])();
			}
		};
	CHAOS_EH_TRY_FINALLY
	CHAOS_EH_FINALLY_END
	return;
}

// Managed method: RuntimeSelfTest/FinalizerProof::get_Id()
extern "C" CHAOS_IL2CPP_INT32 RuntimeSelfTest_FinalizerProof_get_Id(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_RuntimeSelfTest_FinalizerProof*>(_s0);
		_s0 = chaos_object->field_RuntimeSelfTest_FinalizerProof___Id_k__BackingField;
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: RuntimeSelfTest/GcAllocationProof::.ctor()
extern "C" void RuntimeSelfTest_GcAllocationProof__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		ChaosObjectCtor(chaos_arg_0);
	}
	return;
}

// Managed method: RuntimeSelfTest/GcAllocationProof::AllocateAndReturn(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 RuntimeSelfTest_GcAllocationProof_AllocateAndReturn_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_RuntimeSelfTest_GcAllocationProof, {});
		chaos_object->header.type_info = &chaos_mt_RuntimeSelfTest_GcAllocationProof.hot;
		RuntimeSelfTest_GcAllocationProof__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	_s1 = _s0;
	_s2 = chaos_args[1];
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s1);
		auto& _d15 = s_hotpatch_entries[15];
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d15)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d15))
		{
			alignas(16) uint8_t _d_ab[12];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
			_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d15.method_key, _d_ab, nullptr);
		}
		else
		{
			RuntimeSelfTest_GcAllocationProof_set_Value_System_Int32(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
		}
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s0);
		auto& _d14 = s_hotpatch_entries[14];
		CHAOS_IL2CPP_INT32 _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d14)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d14))
		{
			alignas(16) uint8_t _d_ab[8];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d14.method_key, _d_ab, &_d_hpresult);
		}
		else
		{
			_d_hpresult = RuntimeSelfTest_GcAllocationProof_get_Value(chaos_arg_0);
		}
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: RuntimeSelfTest/GcAllocationProof::AllocateAtomic(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 RuntimeSelfTest_GcAllocationProof_AllocateAtomic_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

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
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s2 = chaos_args[1];
	{
		auto chaos_value_raw = _s2;
		const auto chaos_value = chaos_value_raw;
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s1);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s0);
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
	_s0 = chaos_locals[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s1);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s0);
		const auto chaos_element = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];
		_s0 = chaos_element;
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: RuntimeSelfTest/GcAllocationProof::get_Value()
extern "C" CHAOS_IL2CPP_INT32 RuntimeSelfTest_GcAllocationProof_get_Value(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_RuntimeSelfTest_GcAllocationProof*>(_s0);
		_s0 = chaos_object->field_RuntimeSelfTest_GcAllocationProof___Value_k__BackingField;
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: RuntimeSelfTest/GcAllocationProof::set_Value(System.Int32)
extern "C" void RuntimeSelfTest_GcAllocationProof_set_Value_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	{
		auto chaos_value = _s1;
		if (chaos_is_string_id(chaos_value))
		{
			chaos_value = chaos_string_materialize(chaos_value);
		}
		auto* chaos_object = reinterpret_cast<chaos_type_RuntimeSelfTest_GcAllocationProof*>(_s0);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_object->field_RuntimeSelfTest_GcAllocationProof___Value_k__BackingField));
		chaos_object->field_RuntimeSelfTest_GcAllocationProof___Value_k__BackingField = chaos_value;
		chaos_gc_dirty_card(chaos_object);
	}
	return;
}

// Managed method: RuntimeSelfTest/IntegrationProof::DoubleValue(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 RuntimeSelfTest_IntegrationProof_DoubleValue_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapMul(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: RuntimeSelfTest/IntegrationProof::FinalizerAndNewAllocation(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 RuntimeSelfTest_IntegrationProof_FinalizerAndNewAllocation_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_eval_stack{};
	CHAOS_IL2CPP_SIZE chaos_stack_top = 0;
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

chaos_label_0:
	chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
chaos_label_1:
	chaos_locals[1] = chaos_eval_stack[--chaos_stack_top];
chaos_label_2:
	goto chaos_label_15;
chaos_label_4:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[1];
chaos_label_5:
	{
		const auto chaos_arg_1 = chaos_eval_stack[--chaos_stack_top];
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_RuntimeSelfTest_FinalizerProof, {});
		chaos_object->header.type_info = &chaos_mt_RuntimeSelfTest_FinalizerProof.hot;
		RuntimeSelfTest_FinalizerProof__ctor_System_Int32(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
		chaos::il2cpp::runtime_core::chaos_gc_register_finalizable(chaos_object);
		chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
chaos_label_10:
	chaos_stack_top--;
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
	chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(10);
chaos_label_18:
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		if (chaos_left < chaos_right)
			goto chaos_label_4;
	}
chaos_label_20:
	{
		reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[5])();
	}
chaos_label_25:
	{
		reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[6])();
	}
chaos_label_30:
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_RuntimeSelfTest_GcAllocationProof, {});
		chaos_object->header.type_info = &chaos_mt_RuntimeSelfTest_GcAllocationProof.hot;
		RuntimeSelfTest_GcAllocationProof__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
chaos_label_35:
	chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
chaos_label_36:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
chaos_label_37:
	chaos_eval_stack[chaos_stack_top++] = chaos_args[0];
chaos_label_38:
	{
		const auto chaos_arg_1 = chaos_eval_stack[--chaos_stack_top];
		const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_eval_stack[--chaos_stack_top]);
		auto& _d12 = s_hotpatch_entries[12];
		CHAOS_IL2CPP_INT32 _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d12)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d12))
		{
			alignas(16) uint8_t _d_ab[12];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
			_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d12.method_key, _d_ab, &_d_hpresult);
		}
		else
		{
			_d_hpresult = RuntimeSelfTest_GcAllocationProof_AllocateAndReturn_System_Int32(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
		}
		chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
chaos_label_43:
	return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
}

// Managed method: RuntimeSelfTest/IntegrationProof::GcAcrossMethodCalls(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 RuntimeSelfTest_IntegrationProof_GcAcrossMethodCalls_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 7) chaos_locals{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 4) chaos_eval_stack{};
	CHAOS_IL2CPP_SIZE chaos_stack_top = 0;
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

chaos_label_0:
	chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
chaos_label_1:
	chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
chaos_label_2:
	chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
chaos_label_3:
	chaos_locals[1] = chaos_eval_stack[--chaos_stack_top];
chaos_label_4:
	goto chaos_label_92;
chaos_label_6:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[1];
chaos_label_7:
	chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
chaos_label_8:
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(chaos_left, chaos_right));
	}
chaos_label_9:
	chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(16);
chaos_label_11:
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapMul(chaos_left, chaos_right));
	}
chaos_label_12:
	chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
chaos_label_13:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[2];
chaos_label_14:
	{
		const auto chaos_length = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
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
		chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
chaos_label_19:
	chaos_locals[3] = chaos_eval_stack[--chaos_stack_top];
chaos_label_20:
	chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
chaos_label_21:
	chaos_locals[5] = chaos_eval_stack[--chaos_stack_top];
chaos_label_23:
	goto chaos_label_39;
chaos_label_25:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[3];
chaos_label_26:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[5];
chaos_label_28:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[1];
chaos_label_29:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[5];
chaos_label_31:
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(chaos_left, chaos_right));
	}
chaos_label_32:
	{
		auto chaos_value_raw = chaos_eval_stack[--chaos_stack_top];
		const auto chaos_value = chaos_value_raw;
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_eval_stack[--chaos_stack_top]);
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
chaos_label_33:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[5];
chaos_label_35:
	chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
chaos_label_36:
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(chaos_left, chaos_right));
	}
chaos_label_37:
	chaos_locals[5] = chaos_eval_stack[--chaos_stack_top];
chaos_label_39:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[5];
chaos_label_41:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[2];
chaos_label_42:
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		if (chaos_left < chaos_right)
			goto chaos_label_25;
	}
chaos_label_44:
	chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
chaos_label_45:
	chaos_locals[4] = chaos_eval_stack[--chaos_stack_top];
chaos_label_47:
	chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
chaos_label_48:
	chaos_locals[6] = chaos_eval_stack[--chaos_stack_top];
chaos_label_50:
	goto chaos_label_67;
chaos_label_52:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[4];
chaos_label_54:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[3];
chaos_label_55:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[6];
chaos_label_57:
	{
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_eval_stack[--chaos_stack_top]);
		if (chaos_array == nullptr)
		{
			CHAOS_IL2CPP_FAIL();
		}
		if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_element = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];
		chaos_eval_stack[chaos_stack_top++] = chaos_element;
	}
chaos_label_58:
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(chaos_left, chaos_right));
	}
chaos_label_59:
	chaos_locals[4] = chaos_eval_stack[--chaos_stack_top];
chaos_label_61:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[6];
chaos_label_63:
	chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
chaos_label_64:
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(chaos_left, chaos_right));
	}
chaos_label_65:
	chaos_locals[6] = chaos_eval_stack[--chaos_stack_top];
chaos_label_67:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[6];
chaos_label_69:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[2];
chaos_label_70:
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		if (chaos_left < chaos_right)
			goto chaos_label_52;
	}
chaos_label_72:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[1];
chaos_label_73:
	chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(3);
chaos_label_74:
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosRem(chaos_left, chaos_right));
	}
chaos_label_75:
	chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(2);
chaos_label_76:
	{
		const auto chaos_right = chaos_eval_stack[--chaos_stack_top];
		const auto chaos_left = chaos_eval_stack[--chaos_stack_top];
		if (chaos_left != chaos_right)
			goto chaos_label_83;
	}
chaos_label_78:
	{
		reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[5])();
	}
chaos_label_83:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
chaos_label_84:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[4];
chaos_label_86:
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(chaos_left, chaos_right));
	}
chaos_label_87:
	chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
chaos_label_88:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[1];
chaos_label_89:
	chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
chaos_label_90:
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(chaos_left, chaos_right));
	}
chaos_label_91:
	chaos_locals[1] = chaos_eval_stack[--chaos_stack_top];
chaos_label_92:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[1];
chaos_label_93:
	chaos_eval_stack[chaos_stack_top++] = chaos_args[0];
chaos_label_94:
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		if (chaos_left < chaos_right)
			goto chaos_label_6;
	}
chaos_label_96:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
chaos_label_97:
	return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
}

// Managed method: RuntimeSelfTest/IntegrationProof::GcAndDelegateInvoke(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 RuntimeSelfTest_IntegrationProof_GcAndDelegateInvoke_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = 0;
	{
		// Hotpatch-aware ldftn wrapper (slot 16)
		static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INT32 chaos_fn_arg_0) -> CHAOS_IL2CPP_INT32 {
			auto& _d_entry = s_hotpatch_entries[16];
			if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
				&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
			{
				alignas(16) uint8_t _d_ab[4];
				ArgBuffer _d_bw(_d_ab);
				_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_fn_arg_0));
				CHAOS_IL2CPP_INT32 _d_ret{};
				::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
					_d_entry.method_key, _d_ab, &_d_ret);
				return _d_ret;
			}
			return reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INT32)>(_d_entry.direct_ptr)(chaos_fn_arg_0);
		};
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
	}
	{
		const auto chaos_method_ptr = _s1;
		const auto chaos_target = _s0;
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Int32_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Int32_.hot;
		chaos_object->chaos_delegate_target = chaos_target;
		chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[0] = _s0;
	{
		reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[5])();
	}
	{
		reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[6])();
	}
	_s0 = chaos_locals[0];
	_s1 = chaos_args[0];
	{
		auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_delegate_value = _s0;
		if (chaos_delegate_value == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		auto* chaos_delegate = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Int32_*>(chaos_delegate_value);
		if (chaos_delegate->chaos_delegate_invocation_count > 0)
		{
			const auto* chaos_invocation_list = reinterpret_cast<const CHAOS_IL2CPP_VECTOR(CHAOS_IL2CPP_INTPTR)*>(chaos_delegate->chaos_delegate_invocation_list);
			if (chaos_invocation_list == nullptr ||
				static_cast<CHAOS_IL2CPP_INTPTR>(chaos_invocation_list->size()) != chaos_delegate->chaos_delegate_invocation_count)
			{
				CHAOS_IL2CPP_FAIL();
			}

			CHAOS_IL2CPP_INT32 chaos_result{};
			for (CHAOS_IL2CPP_SIZE chaos_delegate_index = 0; chaos_delegate_index < chaos_invocation_list->size(); ++chaos_delegate_index)
			{
				const auto chaos_invocation_delegate_value = (*chaos_invocation_list)[chaos_delegate_index];
				if (chaos_invocation_delegate_value == 0)
				{
					CHAOS_IL2CPP_FAIL();
				}
				auto* chaos_invocation_delegate = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Int32_*>(chaos_invocation_delegate_value);
				if (chaos_invocation_delegate->chaos_delegate_method_ptr == 0)
				{
					CHAOS_IL2CPP_FAIL();
				}
				if (chaos_invocation_delegate->chaos_delegate_target == 0)
				{
					const auto chaos_open_function = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)>(chaos_invocation_delegate->chaos_delegate_method_ptr);
					chaos_result = chaos_open_function(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
				}
				else
				{
					const auto chaos_closed_function = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR chaos_delegate_target, CHAOS_IL2CPP_INT32 chaos_fn_arg_0)>(chaos_invocation_delegate->chaos_delegate_method_ptr);
					chaos_result = chaos_closed_function(chaos_invocation_delegate->chaos_delegate_target, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
				}
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		else
		{
			if (chaos_delegate->chaos_delegate_method_ptr == 0)
			{
				CHAOS_IL2CPP_FAIL();
			}
			CHAOS_IL2CPP_INT32 __chaos_hotpatch_result{};
			bool __chaos_hotpatch_taken = false;
			uint64_t __chaos_args_buf[1] = { (uint64_t)chaos_arg_0 };
			uint64_t __chaos_ret_buf[2] = {};
			if (chaos_delegate->chaos_delegate_method_token != 0)
			{
				__chaos_hotpatch_taken = ::chaos::il2cpp::runtime_core::DelegateHotpatchCheckpoint(
					chaos_delegate->chaos_delegate_method_token,
					__chaos_args_buf, __chaos_ret_buf, 1);
				if (__chaos_hotpatch_taken)
				{
					__chaos_hotpatch_result = *reinterpret_cast<const CHAOS_IL2CPP_INT32*>(__chaos_ret_buf);
				}
			}

			if (__chaos_hotpatch_taken)
			{
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(__chaos_hotpatch_result);
			}
			else
			{
				if (chaos_delegate->chaos_delegate_target == 0)
				{
					const auto chaos_open_function = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)>(chaos_delegate->chaos_delegate_method_ptr);
					const auto chaos_result = chaos_open_function(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
					_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
				}
				else
				{
					const auto chaos_closed_function = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR chaos_delegate_target, CHAOS_IL2CPP_INT32 chaos_fn_arg_0)>(chaos_delegate->chaos_delegate_method_ptr);
					const auto chaos_result = chaos_closed_function(chaos_delegate->chaos_delegate_target, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
					_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
				}
			}
		}
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s3);
}

// Managed method: RuntimeSelfTest/IntegrationProof::ThreadedGcAllocation(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 RuntimeSelfTest_IntegrationProof_ThreadedGcAllocation_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 6) chaos_locals{};
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
	CHAOS_IL2CPP_INTPTR _s23{};
	CHAOS_IL2CPP_INTPTR _s24{};
	CHAOS_IL2CPP_INTPTR _s25{};
	CHAOS_IL2CPP_INTPTR _s26{};
	CHAOS_IL2CPP_INTPTR _s27{};
	CHAOS_IL2CPP_INTPTR _s28{};
	CHAOS_IL2CPP_INTPTR _s29{};
	CHAOS_IL2CPP_INTPTR _s30{};
	CHAOS_IL2CPP_INTPTR _s31{};
	CHAOS_IL2CPP_INTPTR _s32{};
	CHAOS_IL2CPP_INTPTR _s33{};
	CHAOS_IL2CPP_INTPTR _s34{};
	CHAOS_IL2CPP_INTPTR _s35{};
	CHAOS_IL2CPP_INTPTR _s36{};
	CHAOS_IL2CPP_INTPTR _s37{};
	CHAOS_IL2CPP_INTPTR _s38{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0, {});
		chaos_object->header.type_info = &chaos_mt_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0.hot;
		RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s1 = chaos_args[0];
	{
		auto chaos_value = _s1;
		if (chaos_is_string_id(chaos_value))
		{
			chaos_value = chaos_string_materialize(chaos_value);
		}
		auto* chaos_object = reinterpret_cast<chaos_type_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0*>(_s0);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_object->field_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__iterations));
		chaos_object->field_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__iterations = chaos_value;
		chaos_gc_dirty_card(chaos_object);
	}
	_s0 = chaos_locals[0];
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Object, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Object.hot;
		chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	{
		auto chaos_value = _s1;
		if (chaos_is_string_id(chaos_value))
		{
			chaos_value = chaos_string_materialize(chaos_value);
		}
		auto* chaos_object = reinterpret_cast<chaos_type_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0*>(_s0);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_object->field_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__sync));
		chaos_object->field_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__sync = chaos_value;
		chaos_gc_dirty_card(chaos_object);
	}
	_s0 = chaos_locals[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		auto chaos_value = _s1;
		if (chaos_is_string_id(chaos_value))
		{
			chaos_value = chaos_string_materialize(chaos_value);
		}
		auto* chaos_object = reinterpret_cast<chaos_type_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0*>(_s0);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_object->field_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__sharedCounter));
		chaos_object->field_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__sharedCounter = chaos_value;
		chaos_gc_dirty_card(chaos_object);
	}
	_s0 = chaos_locals[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		auto chaos_value = _s1;
		if (chaos_is_string_id(chaos_value))
		{
			chaos_value = chaos_string_materialize(chaos_value);
		}
		auto* chaos_object = reinterpret_cast<chaos_type_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0*>(_s0);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_object->field_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__workerDone));
		chaos_object->field_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__workerDone = chaos_value;
		chaos_gc_dirty_card(chaos_object);
	}
	_s0 = chaos_locals[0];
	{
		// Hotpatch-aware ldftn wrapper (slot 22)
		static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
			auto& _d_entry = s_hotpatch_entries[22];
			if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
				&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
			{
				alignas(16) uint8_t _d_ab[8];
				ArgBuffer _d_bw(_d_ab);
				_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
				::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
					_d_entry.method_key, _d_ab, nullptr);
				return;
			}
			reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(_d_entry.direct_ptr)(chaos_fn_arg_0);
		};
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
	}
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Threading_Thread_System_Threading_ThreadStart, {});
		chaos_object->header.type_info = &chaos_mt_System_Threading_Thread_System_Threading_ThreadStart.hot;
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	{
		const auto chaos_arg_1 = _s0;
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Threading_Thread_System_Threading_Thread, {});
		chaos_object->header.type_info = &chaos_mt_System_Threading_Thread_System_Threading_Thread.hot;
		chaos_external_runtime_System_Threading_Thread_System_Threading_Thread___ctor_System_Void_System_Threading_ThreadStart_(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object), chaos_arg_1);
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	{
		const auto chaos_arg_0 = _s0;
		chaos_thread_start(chaos_arg_0);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[2] = _s0;
	// br (handled via structured EH branches)
	{
		reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[5])();
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		chaos_thread_sleep(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
	}
	_s0 = chaos_locals[2];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	chaos_locals[2] = _s0;
	_s0 = chaos_locals[2];
	_s1 = chaos_locals[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0*>(_s1);
		_s1 = chaos_object->field_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__iterations;
	}
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosDiv(static_cast<CHAOS_IL2CPP_INT32>(_s1), static_cast<CHAOS_IL2CPP_INT32>(_s2)));
	// blt (structured EH branch)
	_s2 = chaos_locals[1];
	{
		const auto chaos_arg_0 = _s2;
		chaos_thread_join(chaos_arg_0);
	}
	_s2 = chaos_locals[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0*>(_s2);
		_s2 = chaos_object->field_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__sync;
	}
	chaos_locals[3] = _s2;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[4] = _s2;
		auto _chaos_finally = [&]()
		{
			_s2 = chaos_locals[4];
			{
				if (_s2 != 0)
				{
					_s2 = chaos_locals[3];
					{
						const auto chaos_arg_0 = _s2;
						chaos_monitor_exit(chaos_arg_0);
					}
				}
			}
		};
	CHAOS_EH_TRY_FINALLY
			_s2 = chaos_locals[3];
			_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[4]);
			{
				const auto chaos_arg_1 = _s3;
				const auto chaos_arg_0 = _s2;
				chaos_monitor_enter(chaos_arg_0, chaos_arg_1);
			}
			_s2 = chaos_locals[0];
			{
				auto* chaos_object = reinterpret_cast<chaos_type_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0*>(_s2);
				_s2 = chaos_object->field_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__sharedCounter;
			}
			chaos_locals[5] = _s2;
	CHAOS_EH_FINALLY_END
	_s2 = chaos_locals[5];
	return static_cast<CHAOS_IL2CPP_INT32>(_s2);
}

// Managed method: RuntimeSelfTest/IntegrationProof+<>c__DisplayClass4_0::.ctor()
extern "C" void RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		ChaosObjectCtor(chaos_arg_0);
	}
	return;
}

// Managed method: RuntimeSelfTest/IntegrationProof+<>c__DisplayClass4_0::<ThreadedGcAllocation>b__0()
extern "C" void RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0_ThreadedGcAllocation_b__0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 6) chaos_locals{};
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
	CHAOS_IL2CPP_INTPTR _s23{};
	CHAOS_IL2CPP_INTPTR _s24{};
	CHAOS_IL2CPP_INTPTR _s25{};
	CHAOS_IL2CPP_INTPTR _s26{};
	CHAOS_IL2CPP_INTPTR _s27{};
	CHAOS_IL2CPP_INTPTR _s28{};
	CHAOS_IL2CPP_INTPTR _s29{};
	CHAOS_IL2CPP_INTPTR _s30{};
	CHAOS_IL2CPP_INTPTR _s31{};
	CHAOS_IL2CPP_INTPTR _s32{};
	CHAOS_IL2CPP_INTPTR _s33{};
	CHAOS_IL2CPP_INTPTR _s34{};
	CHAOS_IL2CPP_INTPTR _s35{};
	CHAOS_IL2CPP_INTPTR _s36{};
	CHAOS_IL2CPP_INTPTR _s37{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[0] = _s0;
	// br (handled via structured EH branches)
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_RuntimeSelfTest_GcAllocationProof, {});
		chaos_object->header.type_info = &chaos_mt_RuntimeSelfTest_GcAllocationProof.hot;
		RuntimeSelfTest_GcAllocationProof__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	_s1 = chaos_locals[0];
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s0);
		auto& _d15 = s_hotpatch_entries[15];
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d15)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d15))
		{
			alignas(16) uint8_t _d_ab[12];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
			_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d15.method_key, _d_ab, nullptr);
		}
		else
		{
			RuntimeSelfTest_GcAllocationProof_set_Value_System_Int32(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
		}
	}
	_s0 = chaos_args[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0*>(_s0);
		_s0 = chaos_object->field_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__sync;
	}
	chaos_locals[2] = _s0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[3] = _s0;
	_s0 = chaos_locals[2];
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[3]);
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		chaos_monitor_enter(chaos_arg_0, chaos_arg_1);
	}
	_s0 = chaos_args[0];
	_s1 = chaos_args[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0*>(_s1);
		_s1 = chaos_object->field_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__sharedCounter;
	}
	_s2 = chaos_locals[1];
	{
		const auto chaos_raw_arg_0 = _s2;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s2);
		auto& _d14 = s_hotpatch_entries[14];
		CHAOS_IL2CPP_INT32 _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d14)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d14))
		{
			alignas(16) uint8_t _d_ab[8];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d14.method_key, _d_ab, &_d_hpresult);
		}
		else
		{
			_d_hpresult = RuntimeSelfTest_GcAllocationProof_get_Value(chaos_arg_0);
		}
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s1), static_cast<CHAOS_IL2CPP_INT32>(_s2)));
	{
		auto chaos_value = _s1;
		if (chaos_is_string_id(chaos_value))
		{
			chaos_value = chaos_string_materialize(chaos_value);
		}
		auto* chaos_object = reinterpret_cast<chaos_type_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0*>(_s0);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_object->field_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__sharedCounter));
		chaos_object->field_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__sharedCounter = chaos_value;
		chaos_gc_dirty_card(chaos_object);
	}
	// leave (handled via structured EH branches)
	_s0 = chaos_locals[3];
	// brfalse (structured EH branch)
	_s1 = chaos_locals[2];
	{
		const auto chaos_arg_0 = _s1;
		chaos_monitor_exit(chaos_arg_0);
	}
	// endfinally (handled via structured EH)
	_s1 = chaos_locals[0];
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s1), static_cast<CHAOS_IL2CPP_INT32>(_s2)));
	chaos_locals[0] = _s1;
	_s1 = chaos_locals[0];
	_s2 = chaos_args[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0*>(_s2);
		_s2 = chaos_object->field_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__iterations;
	}
	// blt (structured EH branch)
	_s3 = chaos_args[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0*>(_s3);
		_s3 = chaos_object->field_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__sync;
	}
	chaos_locals[4] = _s3;
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[5] = _s3;
		auto _chaos_finally = [&]()
		{
			_s3 = chaos_locals[5];
			{
				if (_s3 != 0)
				{
					_s3 = chaos_locals[4];
					{
						const auto chaos_arg_0 = _s3;
						chaos_monitor_exit(chaos_arg_0);
					}
				}
			}
		};
	CHAOS_EH_TRY_FINALLY
				auto _chaos_finally = [&]()
				{
					_s3 = chaos_locals[3];
					{
						if (_s3 != 0)
						{
							_s3 = chaos_locals[2];
							{
								const auto chaos_arg_0 = _s3;
								chaos_monitor_exit(chaos_arg_0);
							}
						}
					}
				};
			CHAOS_EH_TRY_FINALLY
					_s3 = chaos_locals[4];
					_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[5]);
					{
						const auto chaos_arg_1 = _s4;
						const auto chaos_arg_0 = _s3;
						chaos_monitor_enter(chaos_arg_0, chaos_arg_1);
					}
					_s3 = chaos_args[0];
					_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
					{
						auto chaos_value = _s4;
						if (chaos_is_string_id(chaos_value))
						{
							chaos_value = chaos_string_materialize(chaos_value);
						}
						auto* chaos_object = reinterpret_cast<chaos_type_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0*>(_s3);
						BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_object->field_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__workerDone));
						chaos_object->field_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__workerDone = chaos_value;
						chaos_gc_dirty_card(chaos_object);
					}
			CHAOS_EH_FINALLY_END
	CHAOS_EH_FINALLY_END
	_s3 = chaos_locals[0];
	_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s3), static_cast<CHAOS_IL2CPP_INT32>(_s4)));
	chaos_locals[0] = _s3;
	_s3 = chaos_locals[0];
	_s4 = chaos_args[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0*>(_s4);
		_s4 = chaos_object->field_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__iterations;
	}
	// blt (structured EH branch)
	_s5 = chaos_args[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0*>(_s5);
		_s5 = chaos_object->field_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__sync;
	}
	chaos_locals[4] = _s5;
	_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[5] = _s5;
	_s5 = chaos_locals[4];
	_s6 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[5]);
	{
		const auto chaos_arg_1 = _s6;
		const auto chaos_arg_0 = _s5;
		chaos_monitor_enter(chaos_arg_0, chaos_arg_1);
	}
	_s5 = chaos_args[0];
	_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		auto chaos_value = _s6;
		if (chaos_is_string_id(chaos_value))
		{
			chaos_value = chaos_string_materialize(chaos_value);
		}
		auto* chaos_object = reinterpret_cast<chaos_type_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0*>(_s5);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_object->field_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__workerDone));
		chaos_object->field_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__workerDone = chaos_value;
		chaos_gc_dirty_card(chaos_object);
	}
	// leave (handled via structured EH branches)
	_s5 = chaos_locals[5];
	// brfalse (structured EH branch)
	_s6 = chaos_locals[4];
	{
		const auto chaos_arg_0 = _s6;
		chaos_monitor_exit(chaos_arg_0);
	}
	// endfinally (handled via structured EH)
	return;
}

// Managed method: RuntimeSelfTest/Program::.ctor()
extern "C" void RuntimeSelfTest_Program__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		ChaosObjectCtor(chaos_arg_0);
	}
	return;
}

// Managed method: RuntimeSelfTest/Program::<Main>$(System.String[])
extern "C" void RuntimeSelfTest_Program_Main_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 6) chaos_locals{};
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
	CHAOS_IL2CPP_INTPTR _s23{};
	CHAOS_IL2CPP_INTPTR _s24{};
	CHAOS_IL2CPP_INTPTR _s25{};
	CHAOS_IL2CPP_INTPTR _s26{};
	CHAOS_IL2CPP_INTPTR _s27{};
	CHAOS_IL2CPP_INTPTR _s28{};
	CHAOS_IL2CPP_INTPTR _s29{};
	CHAOS_IL2CPP_INTPTR _s30{};
	CHAOS_IL2CPP_INTPTR _s31{};
	CHAOS_IL2CPP_INTPTR _s32{};
	CHAOS_IL2CPP_INTPTR _s33{};
	CHAOS_IL2CPP_INTPTR _s34{};
	CHAOS_IL2CPP_INTPTR _s35{};
	CHAOS_IL2CPP_INTPTR _s36{};
	CHAOS_IL2CPP_INTPTR _s37{};
	CHAOS_IL2CPP_INTPTR _s38{};
	CHAOS_IL2CPP_INTPTR _s39{};
	CHAOS_IL2CPP_INTPTR _s40{};
	CHAOS_IL2CPP_INTPTR _s41{};
	CHAOS_IL2CPP_INTPTR _s42{};
	CHAOS_IL2CPP_INTPTR _s43{};
	CHAOS_IL2CPP_INTPTR _s44{};
	CHAOS_IL2CPP_INTPTR _s45{};
	CHAOS_IL2CPP_INTPTR _s46{};
	CHAOS_IL2CPP_INTPTR _s47{};
	CHAOS_IL2CPP_INTPTR _s48{};
	CHAOS_IL2CPP_INTPTR _s49{};
	CHAOS_IL2CPP_INTPTR _s50{};
	CHAOS_IL2CPP_INTPTR _s51{};
	CHAOS_IL2CPP_INTPTR _s52{};
	CHAOS_IL2CPP_INTPTR _s53{};
	CHAOS_IL2CPP_INTPTR _s54{};
	CHAOS_IL2CPP_INTPTR _s55{};
	CHAOS_IL2CPP_INTPTR _s56{};
	CHAOS_IL2CPP_INTPTR _s57{};
	CHAOS_IL2CPP_INTPTR _s58{};
	CHAOS_IL2CPP_INTPTR _s59{};
	CHAOS_IL2CPP_INTPTR _s60{};
	CHAOS_IL2CPP_INTPTR _s61{};
	CHAOS_IL2CPP_INTPTR _s62{};
	CHAOS_IL2CPP_INTPTR _s63{};
	CHAOS_IL2CPP_INTPTR _s64{};
	CHAOS_IL2CPP_INTPTR _s65{};
	CHAOS_IL2CPP_INTPTR _s66{};
	CHAOS_IL2CPP_INTPTR _s67{};
	CHAOS_IL2CPP_INTPTR _s68{};
	CHAOS_IL2CPP_INTPTR _s69{};
	CHAOS_IL2CPP_INTPTR _s70{};
	CHAOS_IL2CPP_INTPTR _s71{};
	CHAOS_IL2CPP_INTPTR _s72{};
	CHAOS_IL2CPP_INTPTR _s73{};
	CHAOS_IL2CPP_INTPTR _s74{};
	CHAOS_IL2CPP_INTPTR _s75{};
	CHAOS_IL2CPP_INTPTR _s76{};
	CHAOS_IL2CPP_INTPTR _s77{};
	CHAOS_IL2CPP_INTPTR _s78{};
	CHAOS_IL2CPP_INTPTR _s79{};
	CHAOS_IL2CPP_INTPTR _s80{};
	CHAOS_IL2CPP_INTPTR _s81{};
	CHAOS_IL2CPP_INTPTR _s82{};
	CHAOS_IL2CPP_INTPTR _s83{};
	CHAOS_IL2CPP_INTPTR _s84{};
	CHAOS_IL2CPP_INTPTR _s85{};
	CHAOS_IL2CPP_INTPTR _s86{};
	CHAOS_IL2CPP_INTPTR _s87{};
	CHAOS_IL2CPP_INTPTR _s88{};
	CHAOS_IL2CPP_INTPTR _s89{};
	CHAOS_IL2CPP_INTPTR _s90{};
	CHAOS_IL2CPP_INTPTR _s91{};
	CHAOS_IL2CPP_INTPTR _s92{};
	CHAOS_IL2CPP_INTPTR _s93{};
	CHAOS_IL2CPP_INTPTR _s94{};
	CHAOS_IL2CPP_INTPTR _s95{};
	CHAOS_IL2CPP_INTPTR _s96{};
	CHAOS_IL2CPP_INTPTR _s97{};
	CHAOS_IL2CPP_INTPTR _s98{};
	CHAOS_IL2CPP_INTPTR _s99{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	{{
		_s0 = CHAOS_IL2CPP_STRING_ID("RTS_ARITH_ADD=");
	}}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		auto& _d0 = s_hotpatch_entries[0];
		CHAOS_IL2CPP_INT32 _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d0)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d0))
		{
			alignas(16) uint8_t _d_ab[8];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
			_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d0.method_key, _d_ab, &_d_hpresult);
		}
		else
		{
			_d_hpresult = RuntimeSelfTest_ArithmeticProof_Add_System_Int32_System_Int32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	chaos_locals[3] = _s1;
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[3]);
	{
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Int32__ToString_System_String__(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = ChaosReflectionConcatStringPairValues(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Console_System_Console__WriteLine_System_Void_System_String_(chaos_arg_0);
	}
	{{
		_s0 = CHAOS_IL2CPP_STRING_ID("RTS_ARITH_SUB=");
	}}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(10);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		auto& _d2 = s_hotpatch_entries[2];
		CHAOS_IL2CPP_INT32 _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d2)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d2))
		{
			alignas(16) uint8_t _d_ab[8];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
			_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d2.method_key, _d_ab, &_d_hpresult);
		}
		else
		{
			_d_hpresult = RuntimeSelfTest_ArithmeticProof_Subtract_System_Int32_System_Int32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	chaos_locals[3] = _s1;
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[3]);
	{
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Int32__ToString_System_String__(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = ChaosReflectionConcatStringPairValues(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Console_System_Console__WriteLine_System_Void_System_String_(chaos_arg_0);
	}
	{{
		_s0 = CHAOS_IL2CPP_STRING_ID("RTS_ARITH_MUL=");
	}}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		auto& _d1 = s_hotpatch_entries[1];
		CHAOS_IL2CPP_INT32 _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d1)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d1))
		{
			alignas(16) uint8_t _d_ab[8];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
			_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d1.method_key, _d_ab, &_d_hpresult);
		}
		else
		{
			_d_hpresult = RuntimeSelfTest_ArithmeticProof_Multiply_System_Int32_System_Int32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	chaos_locals[3] = _s1;
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[3]);
	{
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Int32__ToString_System_String__(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = ChaosReflectionConcatStringPairValues(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Console_System_Console__WriteLine_System_Void_System_String_(chaos_arg_0);
	}
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_RuntimeSelfTest_GcAllocationProof, {});
		chaos_object->header.type_info = &chaos_mt_RuntimeSelfTest_GcAllocationProof.hot;
		RuntimeSelfTest_GcAllocationProof__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[0] = _s0;
	{{
		_s0 = CHAOS_IL2CPP_STRING_ID("RTS_GC_ALLOC=");
	}}
	_s1 = chaos_locals[0];
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s1);
		auto& _d12 = s_hotpatch_entries[12];
		CHAOS_IL2CPP_INT32 _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d12)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d12))
		{
			alignas(16) uint8_t _d_ab[12];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
			_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d12.method_key, _d_ab, &_d_hpresult);
		}
		else
		{
			_d_hpresult = RuntimeSelfTest_GcAllocationProof_AllocateAndReturn_System_Int32(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	chaos_locals[3] = _s1;
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[3]);
	{
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Int32__ToString_System_String__(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = ChaosReflectionConcatStringPairValues(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Console_System_Console__WriteLine_System_Void_System_String_(chaos_arg_0);
	}
	{{
		_s0 = CHAOS_IL2CPP_STRING_ID("RTS_GC_ALLOC_ATOMIC=");
	}}
	_s1 = chaos_locals[0];
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(100);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s1);
		auto& _d13 = s_hotpatch_entries[13];
		CHAOS_IL2CPP_INT32 _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d13)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d13))
		{
			alignas(16) uint8_t _d_ab[12];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
			_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d13.method_key, _d_ab, &_d_hpresult);
		}
		else
		{
			_d_hpresult = RuntimeSelfTest_GcAllocationProof_AllocateAtomic_System_Int32(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	chaos_locals[3] = _s1;
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[3]);
	{
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Int32__ToString_System_String__(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = ChaosReflectionConcatStringPairValues(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Console_System_Console__WriteLine_System_Void_System_String_(chaos_arg_0);
	}
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_RuntimeSelfTest_StringOperations, {});
		chaos_object->header.type_info = &chaos_mt_RuntimeSelfTest_StringOperations.hot;
		RuntimeSelfTest_StringOperations__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[1] = _s0;
	{{
		_s0 = CHAOS_IL2CPP_STRING_ID("RTS_STR_FORMAT=");
	}}
	_s1 = chaos_locals[1];
	{{
		_s2 = CHAOS_IL2CPP_STRING_ID("hello");
	}}
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
	{
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s1);
		auto& _d27 = s_hotpatch_entries[27];
		CHAOS_IL2CPP_INTPTR _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d27)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d27))
		{
			alignas(16) uint8_t _d_ab[20];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
			_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_1));
			_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d27.method_key, _d_ab, &_d_hpresult);
		}
		else
		{
			_d_hpresult = RuntimeSelfTest_StringOperations_FormatString_System_String_System_Int32(chaos_arg_0, chaos_arg_1, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2));
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = ChaosReflectionConcatStringPairValues(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Console_System_Console__WriteLine_System_Void_System_String_(chaos_arg_0);
	}
	{{
		_s0 = CHAOS_IL2CPP_STRING_ID("RTS_STR_CONCAT=");
	}}
	_s1 = chaos_locals[1];
	{{
		_s2 = CHAOS_IL2CPP_STRING_ID("hello");
	}}
	{{
		_s3 = CHAOS_IL2CPP_STRING_ID("world");
	}}
	{
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s1);
		auto& _d26 = s_hotpatch_entries[26];
		CHAOS_IL2CPP_INTPTR _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d26)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d26))
		{
			alignas(16) uint8_t _d_ab[24];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
			_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_1));
			_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_2));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d26.method_key, _d_ab, &_d_hpresult);
		}
		else
		{
			_d_hpresult = RuntimeSelfTest_StringOperations_ConcatStrings_System_String_System_String(chaos_arg_0, chaos_arg_1, chaos_arg_2);
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = ChaosReflectionConcatStringPairValues(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Console_System_Console__WriteLine_System_Void_System_String_(chaos_arg_0);
	}
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_RuntimeSelfTest_ArrayOperations, {});
		chaos_object->header.type_info = &chaos_mt_RuntimeSelfTest_ArrayOperations.hot;
		RuntimeSelfTest_ArrayOperations__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[2] = _s0;
	{{
		_s0 = CHAOS_IL2CPP_STRING_ID("RTS_ARR_SUM=");
	}}
	_s1 = chaos_locals[2];
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s1);
		auto& _d4 = s_hotpatch_entries[4];
		CHAOS_IL2CPP_INT32 _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d4)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d4))
		{
			alignas(16) uint8_t _d_ab[12];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
			_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d4.method_key, _d_ab, &_d_hpresult);
		}
		else
		{
			_d_hpresult = RuntimeSelfTest_ArrayOperations_CreateAndSum_System_Int32(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	chaos_locals[3] = _s1;
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[3]);
	{
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Int32__ToString_System_String__(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = ChaosReflectionConcatStringPairValues(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Console_System_Console__WriteLine_System_Void_System_String_(chaos_arg_0);
	}
	{{
		_s0 = CHAOS_IL2CPP_STRING_ID("RTS_ARR_LEN=");
	}}
	_s1 = chaos_locals[2];
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(10);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s1);
		auto& _d5 = s_hotpatch_entries[5];
		CHAOS_IL2CPP_INTPTR _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d5)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d5))
		{
			alignas(16) uint8_t _d_ab[12];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
			_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d5.method_key, _d_ab, &_d_hpresult);
		}
		else
		{
			_d_hpresult = RuntimeSelfTest_ArrayOperations_CreateArray_System_Int32(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	_s1 = [&](){ auto* _c_arr = reinterpret_cast<chaos_managed_array*>(_s1); return _c_arr ? static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_c_arr->length)) : CHAOS_IL2CPP_INTPTR{}; }();
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
	chaos_locals[3] = _s1;
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[3]);
	{
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Int32__ToString_System_String__(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = ChaosReflectionConcatStringPairValues(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Console_System_Console__WriteLine_System_Void_System_String_(chaos_arg_0);
	}
	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			chaos_locals[4] = _s0;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
			_s1 = chaos_locals[4];
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosDiv(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
			chaos_locals[5] = _s0;
			{{
				_s0 = CHAOS_IL2CPP_STRING_ID("RTS_EXC_UNEXPECTED=reached");
			}}
			{
				const auto chaos_arg_0 = _s0;
				chaos_external_runtime_System_Console_System_Console__WriteLine_System_Void_System_String_(chaos_arg_0);
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
			{{
				_s0 = CHAOS_IL2CPP_STRING_ID("RTS_EXC_HANDLED=-1");
			}}
			{
				const auto chaos_arg_0 = _s0;
				chaos_external_runtime_System_Console_System_Console__WriteLine_System_Void_System_String_(chaos_arg_0);
			}
	CHAOS_EH_END
	{{
		_s0 = CHAOS_IL2CPP_STRING_ID("RTS_INT_DELEGATE=");
	}}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(21);
	{
		const auto chaos_arg_0 = _s1;
		auto& _d19 = s_hotpatch_entries[19];
		CHAOS_IL2CPP_INT32 _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d19)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d19))
		{
			alignas(16) uint8_t _d_ab[4];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d19.method_key, _d_ab, &_d_hpresult);
		}
		else
		{
			_d_hpresult = RuntimeSelfTest_IntegrationProof_GcAndDelegateInvoke_System_Int32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	chaos_locals[3] = _s1;
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[3]);
	{
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Int32__ToString_System_String__(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = ChaosReflectionConcatStringPairValues(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Console_System_Console__WriteLine_System_Void_System_String_(chaos_arg_0);
	}
	{{
		_s0 = CHAOS_IL2CPP_STRING_ID("RTS_INT_FINALIZER=");
	}}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(99);
	{
		const auto chaos_arg_0 = _s1;
		auto& _d17 = s_hotpatch_entries[17];
		CHAOS_IL2CPP_INT32 _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d17)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d17))
		{
			alignas(16) uint8_t _d_ab[4];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d17.method_key, _d_ab, &_d_hpresult);
		}
		else
		{
			_d_hpresult = RuntimeSelfTest_IntegrationProof_FinalizerAndNewAllocation_System_Int32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	chaos_locals[3] = _s1;
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[3]);
	{
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Int32__ToString_System_String__(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = ChaosReflectionConcatStringPairValues(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Console_System_Console__WriteLine_System_Void_System_String_(chaos_arg_0);
	}
	{{
		_s0 = CHAOS_IL2CPP_STRING_ID("RTS_INT_GC_ACROSS=");
	}}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
	{
		const auto chaos_arg_0 = _s1;
		auto& _d18 = s_hotpatch_entries[18];
		CHAOS_IL2CPP_INT32 _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d18)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d18))
		{
			alignas(16) uint8_t _d_ab[4];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d18.method_key, _d_ab, &_d_hpresult);
		}
		else
		{
			_d_hpresult = RuntimeSelfTest_IntegrationProof_GcAcrossMethodCalls_System_Int32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	chaos_locals[3] = _s1;
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[3]);
	{
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Int32__ToString_System_String__(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = ChaosReflectionConcatStringPairValues(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Console_System_Console__WriteLine_System_Void_System_String_(chaos_arg_0);
	}
	{{
		_s0 = CHAOS_IL2CPP_STRING_ID("RTS_INT_THREADED=");
	}}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(10);
	{
		const auto chaos_arg_0 = _s1;
		auto& _d20 = s_hotpatch_entries[20];
		CHAOS_IL2CPP_INT32 _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d20)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d20))
		{
			alignas(16) uint8_t _d_ab[4];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d20.method_key, _d_ab, &_d_hpresult);
		}
		else
		{
			_d_hpresult = RuntimeSelfTest_IntegrationProof_ThreadedGcAllocation_System_Int32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	chaos_locals[3] = _s1;
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[3]);
	{
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Int32__ToString_System_String__(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = ChaosReflectionConcatStringPairValues(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Console_System_Console__WriteLine_System_Void_System_String_(chaos_arg_0);
	}
	{{
		_s0 = CHAOS_IL2CPP_STRING_ID("RTS_DONE=ok");
	}}
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Console_System_Console__WriteLine_System_Void_System_String_(chaos_arg_0);
	}
	return;
}

// Managed method: RuntimeSelfTest/StringOperations::.ctor()
extern "C" void RuntimeSelfTest_StringOperations__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		ChaosObjectCtor(chaos_arg_0);
	}
	return;
}

// Managed method: RuntimeSelfTest/StringOperations::ConcatStrings(System.String,System.String)
extern "C" CHAOS_IL2CPP_INTPTR RuntimeSelfTest_StringOperations_ConcatStrings_System_String_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2)
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
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = ChaosReflectionConcatStringPairValues(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return _s0;
}

// Managed method: RuntimeSelfTest/StringOperations::FormatString(System.String,System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR RuntimeSelfTest_StringOperations_FormatString_System_String_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);
	chaos_args[2] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_2);

	_s0 = chaos_args[1];
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_args[2]);
	{
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Int32__ToString_System_String__(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = ChaosReflectionConcatStringPairValues(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return _s0;
}



}  // namespace chaos::il2cpp::codegen::RuntimeSelfTest
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 28;

// ── JIT Method Entry Table ───────────────────────────────────────────
// Auto-generated by chaos-il2cpp codegen for --mode jit.
#include <cstdint>
#include "jit_registration.h"

static const char kMethodJson_0[] = "{\"methodId\":\"runtime-self-test.arithmetic-proof.add\",\"subjectId\":\"RuntimeSelfTest/ArithmeticProof::Add:System.Int32(System.Int32,System.Int32)\",\"signature\":\"System.Int32 ArithmeticProof::Add(System.Int32,System.Int32)\",\"identity\":{\"assemblyName\":\"RuntimeSelfTest\",\"declaringTypeSubjectId\":\"RuntimeSelfTest/ArithmeticProof\",\"definitionSubjectId\":\"RuntimeSelfTest/ArithmeticProof::Add:System.Int32(System.Int32,System.Int32)\",\"subjectId\":\"RuntimeSelfTest/ArithmeticProof::Add:System.Int32(System.Int32,System.Int32)\",\"methodId\":\"runtime-self-test.arithmetic-proof.add\",\"signature\":\"System.Int32 ArithmeticProof::Add(System.Int32,System.Int32)\"},\"nativeSymbol\":\"RuntimeSelfTest_ArithmeticProof_Add_System_Int32_System_Int32\",\"isStatic\":true,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":2,\"parameterAbis\":[{\"carrierKindCode\":1,\"typeShape\":2},{\"carrierKindCode\":1,\"typeShape\":2}],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"mul\",\"opCode\":27,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":3,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"RuntimeSelfTest\"}";
static const char kMethodJson_1[] = "{\"methodId\":\"runtime-self-test.arithmetic-proof.multiply\",\"subjectId\":\"RuntimeSelfTest/ArithmeticProof::Multiply:System.Int32(System.Int32,System.Int32)\",\"signature\":\"System.Int32 ArithmeticProof::Multiply(System.Int32,System.Int32)\",\"identity\":{\"assemblyName\":\"RuntimeSelfTest\",\"declaringTypeSubjectId\":\"RuntimeSelfTest/ArithmeticProof\",\"definitionSubjectId\":\"RuntimeSelfTest/ArithmeticProof::Multiply:System.Int32(System.Int32,System.Int32)\",\"subjectId\":\"RuntimeSelfTest/ArithmeticProof::Multiply:System.Int32(System.Int32,System.Int32)\",\"methodId\":\"runtime-self-test.arithmetic-proof.multiply\",\"signature\":\"System.Int32 ArithmeticProof::Multiply(System.Int32,System.Int32)\"},\"nativeSymbol\":\"RuntimeSelfTest_ArithmeticProof_Multiply_System_Int32_System_Int32\",\"isStatic\":true,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":2,\"parameterAbis\":[{\"carrierKindCode\":1,\"typeShape\":2},{\"carrierKindCode\":1,\"typeShape\":2}],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"mul\",\"opCode\":27,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":3,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"RuntimeSelfTest\"}";
static const char kMethodJson_2[] = "{\"methodId\":\"runtime-self-test.arithmetic-proof.subtract\",\"subjectId\":\"RuntimeSelfTest/ArithmeticProof::Subtract:System.Int32(System.Int32,System.Int32)\",\"signature\":\"System.Int32 ArithmeticProof::Subtract(System.Int32,System.Int32)\",\"identity\":{\"assemblyName\":\"RuntimeSelfTest\",\"declaringTypeSubjectId\":\"RuntimeSelfTest/ArithmeticProof\",\"definitionSubjectId\":\"RuntimeSelfTest/ArithmeticProof::Subtract:System.Int32(System.Int32,System.Int32)\",\"subjectId\":\"RuntimeSelfTest/ArithmeticProof::Subtract:System.Int32(System.Int32,System.Int32)\",\"methodId\":\"runtime-self-test.arithmetic-proof.subtract\",\"signature\":\"System.Int32 ArithmeticProof::Subtract(System.Int32,System.Int32)\"},\"nativeSymbol\":\"RuntimeSelfTest_ArithmeticProof_Subtract_System_Int32_System_Int32\",\"isStatic\":true,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":2,\"parameterAbis\":[{\"carrierKindCode\":1,\"typeShape\":2},{\"carrierKindCode\":1,\"typeShape\":2}],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"sub\",\"opCode\":26,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":3,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"RuntimeSelfTest\"}";
static const char kMethodJson_3[] = "{\"methodId\":\"runtime-self-test.array-operations.ctor\",\"subjectId\":\"RuntimeSelfTest/ArrayOperations::.ctor:System.Void()\",\"signature\":\"System.Void ArrayOperations::.ctor()\",\"identity\":{\"assemblyName\":\"RuntimeSelfTest\",\"declaringTypeSubjectId\":\"RuntimeSelfTest/ArrayOperations\",\"definitionSubjectId\":\"RuntimeSelfTest/ArrayOperations::.ctor:System.Void()\",\"subjectId\":\"RuntimeSelfTest/ArrayOperations::.ctor:System.Void()\",\"methodId\":\"runtime-self-test.array-operations.ctor\",\"signature\":\"System.Void ArrayOperations::.ctor()\"},\"nativeSymbol\":\"RuntimeSelfTest_ArrayOperations__ctor\",\"isStatic\":false,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"RuntimeSelfTest/ArrayOperations\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":6,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"RuntimeSelfTest\"}";
static const char kMethodJson_4[] = "{\"methodId\":\"runtime-self-test.array-operations.create-and-sum\",\"subjectId\":\"RuntimeSelfTest/ArrayOperations::CreateAndSum:System.Int32(System.Int32)\",\"signature\":\"System.Int32 ArrayOperations::CreateAndSum(System.Int32)\",\"identity\":{\"assemblyName\":\"RuntimeSelfTest\",\"declaringTypeSubjectId\":\"RuntimeSelfTest/ArrayOperations\",\"definitionSubjectId\":\"RuntimeSelfTest/ArrayOperations::CreateAndSum:System.Int32(System.Int32)\",\"subjectId\":\"RuntimeSelfTest/ArrayOperations::CreateAndSum:System.Int32(System.Int32)\",\"methodId\":\"runtime-self-test.array-operations.create-and-sum\",\"signature\":\"System.Int32 ArrayOperations::CreateAndSum(System.Int32)\"},\"nativeSymbol\":\"RuntimeSelfTest_ArrayOperations_CreateAndSum_System_Int32\",\"isStatic\":false,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":1,\"parameterAbis\":[{\"carrierKindCode\":1,\"typeShape\":2}],\"localCount\":4,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":0,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Int32\",\"ilOffset\":1,\"resultType\":\"System.Int32[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Int32\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":6,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":8,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":21,\"ilOffset\":9,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":11,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":12,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":13,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"add\",\"opCode\":25,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stelem\",\"opCode\":45,\"operand\":\"System.Int32\",\"ilOffset\":16,\"resultType\":\"System.Void\",\"runtimeServiceKind\":13,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":17,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"add\",\"opCode\":25,\"ilOffset\":19,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":20,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":21,\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"blt\",\"opCode\":21,\"operand\":11,\"ilOffset\":23,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":26,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":27,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":3,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":41,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":31,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":32,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":3,\"ilOffset\":33,\"isPreserveSig\":false},{\"op\":\"ldelem\",\"opCode\":44,\"operand\":\"System.Int32\",\"ilOffset\":34,\"resultType\":\"System.Int32\",\"runtimeServiceKind\":12,\"isPreserveSig\":false},{\"op\":\"add\",\"opCode\":25,\"ilOffset\":35,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":36,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":3,\"ilOffset\":37,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":38,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"add\",\"opCode\":25,\"ilOffset\":39,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":3,\"ilOffset\":40,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":3,\"ilOffset\":41,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":42,\"isPreserveSig\":false},{\"op\":\"ldlen\",\"opCode\":46,\"ilOffset\":43,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"conv.i4\",\"opCode\":39,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"blt\",\"opCode\":21,\"operand\":31,\"ilOffset\":45,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":47,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":48,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"RuntimeSelfTest\"}";
static const char kMethodJson_5[] = "{\"methodId\":\"runtime-self-test.array-operations.create-array\",\"subjectId\":\"RuntimeSelfTest/ArrayOperations::CreateArray:System.Int32[](System.Int32)\",\"signature\":\"System.Int32[] ArrayOperations::CreateArray(System.Int32)\",\"identity\":{\"assemblyName\":\"RuntimeSelfTest\",\"declaringTypeSubjectId\":\"RuntimeSelfTest/ArrayOperations\",\"definitionSubjectId\":\"RuntimeSelfTest/ArrayOperations::CreateArray:System.Int32[](System.Int32)\",\"subjectId\":\"RuntimeSelfTest/ArrayOperations::CreateArray:System.Int32[](System.Int32)\",\"methodId\":\"runtime-self-test.array-operations.create-array\",\"signature\":\"System.Int32[] ArrayOperations::CreateArray(System.Int32)\"},\"nativeSymbol\":\"RuntimeSelfTest_ArrayOperations_CreateArray_System_Int32\",\"isStatic\":false,\"returnType\":\"System.Int32[]\",\"returnAbi\":{\"carrierKindCode\":2,\"typeShape\":1},\"parameterCount\":1,\"parameterAbis\":[{\"carrierKindCode\":1,\"typeShape\":2}],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":0,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Int32\",\"ilOffset\":1,\"resultType\":\"System.Int32[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Int32\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":6,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"RuntimeSelfTest\"}";
static const char kMethodJson_6[] = "{\"methodId\":\"runtime-self-test.auto-generated-program.ctor\",\"subjectId\":\"RuntimeSelfTest/AutoGeneratedProgram::.ctor:System.Void()\",\"signature\":\"System.Void AutoGeneratedProgram::.ctor()\",\"identity\":{\"assemblyName\":\"RuntimeSelfTest\",\"declaringTypeSubjectId\":\"RuntimeSelfTest/AutoGeneratedProgram\",\"definitionSubjectId\":\"RuntimeSelfTest/AutoGeneratedProgram::.ctor:System.Void()\",\"subjectId\":\"RuntimeSelfTest/AutoGeneratedProgram::.ctor:System.Void()\",\"methodId\":\"runtime-self-test.auto-generated-program.ctor\",\"signature\":\"System.Void AutoGeneratedProgram::.ctor()\"},\"nativeSymbol\":\"RuntimeSelfTest_AutoGeneratedProgram__ctor\",\"isStatic\":false,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"RuntimeSelfTest/AutoGeneratedProgram\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":6,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"RuntimeSelfTest\"}";
static const char kMethodJson_7[] = "{\"methodId\":\"runtime-self-test.auto-generated-program.main\",\"subjectId\":\"RuntimeSelfTest/AutoGeneratedProgram::Main:System.Void(System.String[])\",\"signature\":\"System.Void AutoGeneratedProgram::Main(System.String[])\",\"identity\":{\"assemblyName\":\"RuntimeSelfTest\",\"declaringTypeSubjectId\":\"RuntimeSelfTest/AutoGeneratedProgram\",\"definitionSubjectId\":\"RuntimeSelfTest/AutoGeneratedProgram::Main:System.Void(System.String[])\",\"subjectId\":\"RuntimeSelfTest/AutoGeneratedProgram::Main:System.Void(System.String[])\",\"methodId\":\"runtime-self-test.auto-generated-program.main\",\"signature\":\"System.Void AutoGeneratedProgram::Main(System.String[])\"},\"nativeSymbol\":\"RuntimeSelfTest_AutoGeneratedProgram_Main_System_String\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":1,\"parameterAbis\":[{\"carrierKindCode\":2,\"typeShape\":1}],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":0,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"RuntimeSelfTest\"}";
static const char kMethodJson_8[] = "{\"methodId\":\"runtime-self-test.finalizer-proof.ctor\",\"subjectId\":\"RuntimeSelfTest/FinalizerProof::.ctor:System.Void(System.Int32)\",\"signature\":\"System.Void FinalizerProof::.ctor(System.Int32)\",\"identity\":{\"assemblyName\":\"RuntimeSelfTest\",\"declaringTypeSubjectId\":\"RuntimeSelfTest/FinalizerProof\",\"definitionSubjectId\":\"RuntimeSelfTest/FinalizerProof::.ctor:System.Void(System.Int32)\",\"subjectId\":\"RuntimeSelfTest/FinalizerProof::.ctor:System.Void(System.Int32)\",\"methodId\":\"runtime-self-test.finalizer-proof.ctor\",\"signature\":\"System.Void FinalizerProof::.ctor(System.Int32)\"},\"nativeSymbol\":\"RuntimeSelfTest_FinalizerProof__ctor_System_Int32\",\"isStatic\":false,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":1,\"parameterAbis\":[{\"carrierKindCode\":1,\"typeShape\":2}],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"RuntimeSelfTest/FinalizerProof\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":6,\"resultType\":\"RuntimeSelfTest/FinalizerProof\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stfld\",\"opCode\":11,\"operand\":\"RuntimeSelfTest/FinalizerProof::\\u003CId\\u003Ek__BackingField\",\"ilOffset\":8,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"RuntimeSelfTest\",\"subjectKind\":\"field\",\"subjectId\":\"RuntimeSelfTest/FinalizerProof::\\u003CId\\u003Ek__BackingField\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"RuntimeSelfTest\",\"subjectId\":\"RuntimeSelfTest/FinalizerProof::\\u003CId\\u003Ek__BackingField\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"RuntimeSelfTest/FinalizerProof\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":3,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":13,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"RuntimeSelfTest\"}";
static const char kMethodJson_9[] = "{\"methodId\":\"runtime-self-test.finalizer-proof.finalize\",\"subjectId\":\"RuntimeSelfTest/FinalizerProof::Finalize:System.Void()\",\"signature\":\"System.Void FinalizerProof::Finalize()\",\"identity\":{\"assemblyName\":\"RuntimeSelfTest\",\"declaringTypeSubjectId\":\"RuntimeSelfTest/FinalizerProof\",\"definitionSubjectId\":\"RuntimeSelfTest/FinalizerProof::Finalize:System.Void()\",\"subjectId\":\"RuntimeSelfTest/FinalizerProof::Finalize:System.Void()\",\"methodId\":\"runtime-self-test.finalizer-proof.finalize\",\"signature\":\"System.Void FinalizerProof::Finalize()\"},\"nativeSymbol\":\"RuntimeSelfTest_FinalizerProof_Finalize\",\"isStatic\":false,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":2,\"tryOffset\":0,\"tryLength\":2,\"handlerOffset\":2,\"handlerLength\":7}],\"instructions\":[{\"op\":\"leave\",\"opCode\":50,\"operand\":9,\"ilOffset\":0,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":2,\"resultType\":\"RuntimeSelfTest/FinalizerProof\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":3,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Object::Finalize:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::Finalize:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::Finalize:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"endfinally\",\"opCode\":51,\"ilOffset\":8,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":9,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"RuntimeSelfTest\"}";
static const char kMethodJson_10[] = "{\"methodId\":\"runtime-self-test.finalizer-proof.get-id\",\"subjectId\":\"RuntimeSelfTest/FinalizerProof::get_Id:System.Int32()\",\"signature\":\"System.Int32 FinalizerProof::get_Id()\",\"identity\":{\"assemblyName\":\"RuntimeSelfTest\",\"declaringTypeSubjectId\":\"RuntimeSelfTest/FinalizerProof\",\"definitionSubjectId\":\"RuntimeSelfTest/FinalizerProof::get_Id:System.Int32()\",\"subjectId\":\"RuntimeSelfTest/FinalizerProof::get_Id:System.Int32()\",\"methodId\":\"runtime-self-test.finalizer-proof.get-id\",\"signature\":\"System.Int32 FinalizerProof::get_Id()\"},\"nativeSymbol\":\"RuntimeSelfTest_FinalizerProof_get_Id\",\"isStatic\":false,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"RuntimeSelfTest/FinalizerProof\",\"isPreserveSig\":false},{\"op\":\"ldfld\",\"opCode\":10,\"operand\":\"RuntimeSelfTest/FinalizerProof::\\u003CId\\u003Ek__BackingField\",\"ilOffset\":1,\"resultType\":\"System.Int32\",\"reference\":{\"assemblyName\":\"RuntimeSelfTest\",\"subjectKind\":\"field\",\"subjectId\":\"RuntimeSelfTest/FinalizerProof::\\u003CId\\u003Ek__BackingField\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"RuntimeSelfTest\",\"subjectId\":\"RuntimeSelfTest/FinalizerProof::\\u003CId\\u003Ek__BackingField\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"RuntimeSelfTest/FinalizerProof\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":2,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":6,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"RuntimeSelfTest\"}";
static const char kMethodJson_11[] = "{\"methodId\":\"runtime-self-test.gc-allocation-proof.ctor\",\"subjectId\":\"RuntimeSelfTest/GcAllocationProof::.ctor:System.Void()\",\"signature\":\"System.Void GcAllocationProof::.ctor()\",\"identity\":{\"assemblyName\":\"RuntimeSelfTest\",\"declaringTypeSubjectId\":\"RuntimeSelfTest/GcAllocationProof\",\"definitionSubjectId\":\"RuntimeSelfTest/GcAllocationProof::.ctor:System.Void()\",\"subjectId\":\"RuntimeSelfTest/GcAllocationProof::.ctor:System.Void()\",\"methodId\":\"runtime-self-test.gc-allocation-proof.ctor\",\"signature\":\"System.Void GcAllocationProof::.ctor()\"},\"nativeSymbol\":\"RuntimeSelfTest_GcAllocationProof__ctor\",\"isStatic\":false,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"RuntimeSelfTest/GcAllocationProof\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":6,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"RuntimeSelfTest\"}";
static const char kMethodJson_12[] = "{\"methodId\":\"runtime-self-test.gc-allocation-proof.allocate-and-return\",\"subjectId\":\"RuntimeSelfTest/GcAllocationProof::AllocateAndReturn:System.Int32(System.Int32)\",\"signature\":\"System.Int32 GcAllocationProof::AllocateAndReturn(System.Int32)\",\"identity\":{\"assemblyName\":\"RuntimeSelfTest\",\"declaringTypeSubjectId\":\"RuntimeSelfTest/GcAllocationProof\",\"definitionSubjectId\":\"RuntimeSelfTest/GcAllocationProof::AllocateAndReturn:System.Int32(System.Int32)\",\"subjectId\":\"RuntimeSelfTest/GcAllocationProof::AllocateAndReturn:System.Int32(System.Int32)\",\"methodId\":\"runtime-self-test.gc-allocation-proof.allocate-and-return\",\"signature\":\"System.Int32 GcAllocationProof::AllocateAndReturn(System.Int32)\"},\"nativeSymbol\":\"RuntimeSelfTest_GcAllocationProof_AllocateAndReturn_System_Int32\",\"isStatic\":false,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":1,\"parameterAbis\":[{\"carrierKindCode\":1,\"typeShape\":2}],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":0,\"resultType\":\"RuntimeSelfTest/GcAllocationProof\",\"callee\":\"RuntimeSelfTest/GcAllocationProof::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"RuntimeSelfTest\",\"subjectKind\":\"method\",\"subjectId\":\"RuntimeSelfTest/GcAllocationProof::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"RuntimeSelfTest\",\"subjectId\":\"RuntimeSelfTest/GcAllocationProof\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"baseTypeSubjectId\":\"System.Private.CoreLib/System.Object\",\"implementedInterfaceSubjectIds\":[],\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":5,\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":6,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":7,\"resultType\":\"System.Void\",\"callee\":\"RuntimeSelfTest/GcAllocationProof::set_Value:System.Void(System.Int32)\",\"reference\":{\"assemblyName\":\"RuntimeSelfTest\",\"subjectKind\":\"method\",\"subjectId\":\"RuntimeSelfTest/GcAllocationProof::set_Value:System.Void(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"RuntimeSelfTest\",\"subjectId\":\"RuntimeSelfTest/GcAllocationProof::set_Value:System.Void(System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"RuntimeSelfTest_GcAllocationProof_set_Value_System_Int32\",\"targetParameterCount\":1,\"targetReturnType\":\"System.Void\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":12,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":13,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"callee\":\"RuntimeSelfTest/GcAllocationProof::get_Value:System.Int32()\",\"reference\":{\"assemblyName\":\"RuntimeSelfTest\",\"subjectKind\":\"method\",\"subjectId\":\"RuntimeSelfTest/GcAllocationProof::get_Value:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"RuntimeSelfTest\",\"subjectId\":\"RuntimeSelfTest/GcAllocationProof::get_Value:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"RuntimeSelfTest_GcAllocationProof_get_Value\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":19,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"RuntimeSelfTest\"}";
static const char kMethodJson_13[] = "{\"methodId\":\"runtime-self-test.gc-allocation-proof.allocate-atomic\",\"subjectId\":\"RuntimeSelfTest/GcAllocationProof::AllocateAtomic:System.Int32(System.Int32)\",\"signature\":\"System.Int32 GcAllocationProof::AllocateAtomic(System.Int32)\",\"identity\":{\"assemblyName\":\"RuntimeSelfTest\",\"declaringTypeSubjectId\":\"RuntimeSelfTest/GcAllocationProof\",\"definitionSubjectId\":\"RuntimeSelfTest/GcAllocationProof::AllocateAtomic:System.Int32(System.Int32)\",\"subjectId\":\"RuntimeSelfTest/GcAllocationProof::AllocateAtomic:System.Int32(System.Int32)\",\"methodId\":\"runtime-self-test.gc-allocation-proof.allocate-atomic\",\"signature\":\"System.Int32 GcAllocationProof::AllocateAtomic(System.Int32)\"},\"nativeSymbol\":\"RuntimeSelfTest_GcAllocationProof_AllocateAtomic_System_Int32\",\"isStatic\":false,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":1,\"parameterAbis\":[{\"carrierKindCode\":1,\"typeShape\":2}],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":0,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Int32\",\"ilOffset\":1,\"resultType\":\"System.Int32[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Int32\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":6,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stelem\",\"opCode\":45,\"operand\":\"System.Int32\",\"ilOffset\":10,\"resultType\":\"System.Void\",\"runtimeServiceKind\":13,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":11,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldelem\",\"opCode\":44,\"operand\":\"System.Int32\",\"ilOffset\":13,\"resultType\":\"System.Int32\",\"runtimeServiceKind\":12,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":14,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"RuntimeSelfTest\"}";
static const char kMethodJson_14[] = "{\"methodId\":\"runtime-self-test.gc-allocation-proof.get-value\",\"subjectId\":\"RuntimeSelfTest/GcAllocationProof::get_Value:System.Int32()\",\"signature\":\"System.Int32 GcAllocationProof::get_Value()\",\"identity\":{\"assemblyName\":\"RuntimeSelfTest\",\"declaringTypeSubjectId\":\"RuntimeSelfTest/GcAllocationProof\",\"definitionSubjectId\":\"RuntimeSelfTest/GcAllocationProof::get_Value:System.Int32()\",\"subjectId\":\"RuntimeSelfTest/GcAllocationProof::get_Value:System.Int32()\",\"methodId\":\"runtime-self-test.gc-allocation-proof.get-value\",\"signature\":\"System.Int32 GcAllocationProof::get_Value()\"},\"nativeSymbol\":\"RuntimeSelfTest_GcAllocationProof_get_Value\",\"isStatic\":false,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"RuntimeSelfTest/GcAllocationProof\",\"isPreserveSig\":false},{\"op\":\"ldfld\",\"opCode\":10,\"operand\":\"RuntimeSelfTest/GcAllocationProof::\\u003CValue\\u003Ek__BackingField\",\"ilOffset\":1,\"resultType\":\"System.Int32\",\"reference\":{\"assemblyName\":\"RuntimeSelfTest\",\"subjectKind\":\"field\",\"subjectId\":\"RuntimeSelfTest/GcAllocationProof::\\u003CValue\\u003Ek__BackingField\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"RuntimeSelfTest\",\"subjectId\":\"RuntimeSelfTest/GcAllocationProof::\\u003CValue\\u003Ek__BackingField\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"RuntimeSelfTest/GcAllocationProof\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":2,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":6,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"RuntimeSelfTest\"}";
static const char kMethodJson_15[] = "{\"methodId\":\"runtime-self-test.gc-allocation-proof.set-value\",\"subjectId\":\"RuntimeSelfTest/GcAllocationProof::set_Value:System.Void(System.Int32)\",\"signature\":\"System.Void GcAllocationProof::set_Value(System.Int32)\",\"identity\":{\"assemblyName\":\"RuntimeSelfTest\",\"declaringTypeSubjectId\":\"RuntimeSelfTest/GcAllocationProof\",\"definitionSubjectId\":\"RuntimeSelfTest/GcAllocationProof::set_Value:System.Void(System.Int32)\",\"subjectId\":\"RuntimeSelfTest/GcAllocationProof::set_Value:System.Void(System.Int32)\",\"methodId\":\"runtime-self-test.gc-allocation-proof.set-value\",\"signature\":\"System.Void GcAllocationProof::set_Value(System.Int32)\"},\"nativeSymbol\":\"RuntimeSelfTest_GcAllocationProof_set_Value_System_Int32\",\"isStatic\":false,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":1,\"parameterAbis\":[{\"carrierKindCode\":1,\"typeShape\":2}],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"RuntimeSelfTest/GcAllocationProof\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stfld\",\"opCode\":11,\"operand\":\"RuntimeSelfTest/GcAllocationProof::\\u003CValue\\u003Ek__BackingField\",\"ilOffset\":2,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"RuntimeSelfTest\",\"subjectKind\":\"field\",\"subjectId\":\"RuntimeSelfTest/GcAllocationProof::\\u003CValue\\u003Ek__BackingField\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"RuntimeSelfTest\",\"subjectId\":\"RuntimeSelfTest/GcAllocationProof::\\u003CValue\\u003Ek__BackingField\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"RuntimeSelfTest/GcAllocationProof\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":3,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":7,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"RuntimeSelfTest\"}";
static const char kMethodJson_16[] = "{\"methodId\":\"runtime-self-test.integration-proof.double-value\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof::DoubleValue:System.Int32(System.Int32)\",\"signature\":\"System.Int32 IntegrationProof::DoubleValue(System.Int32)\",\"identity\":{\"assemblyName\":\"RuntimeSelfTest\",\"declaringTypeSubjectId\":\"RuntimeSelfTest/IntegrationProof\",\"definitionSubjectId\":\"RuntimeSelfTest/IntegrationProof::DoubleValue:System.Int32(System.Int32)\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof::DoubleValue:System.Int32(System.Int32)\",\"methodId\":\"runtime-self-test.integration-proof.double-value\",\"signature\":\"System.Int32 IntegrationProof::DoubleValue(System.Int32)\"},\"nativeSymbol\":\"RuntimeSelfTest_IntegrationProof_DoubleValue_System_Int32\",\"isStatic\":true,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":1,\"parameterAbis\":[{\"carrierKindCode\":1,\"typeShape\":2}],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":2,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"mul\",\"opCode\":27,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":3,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"RuntimeSelfTest\"}";
static const char kMethodJson_17[] = "{\"methodId\":\"runtime-self-test.integration-proof.finalizer-and-new-allocation\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof::FinalizerAndNewAllocation:System.Int32(System.Int32)\",\"signature\":\"System.Int32 IntegrationProof::FinalizerAndNewAllocation(System.Int32)\",\"identity\":{\"assemblyName\":\"RuntimeSelfTest\",\"declaringTypeSubjectId\":\"RuntimeSelfTest/IntegrationProof\",\"definitionSubjectId\":\"RuntimeSelfTest/IntegrationProof::FinalizerAndNewAllocation:System.Int32(System.Int32)\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof::FinalizerAndNewAllocation:System.Int32(System.Int32)\",\"methodId\":\"runtime-self-test.integration-proof.finalizer-and-new-allocation\",\"signature\":\"System.Int32 IntegrationProof::FinalizerAndNewAllocation(System.Int32)\"},\"nativeSymbol\":\"RuntimeSelfTest_IntegrationProof_FinalizerAndNewAllocation_System_Int32\",\"isStatic\":true,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":1,\"parameterAbis\":[{\"carrierKindCode\":1,\"typeShape\":2}],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":0,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":1,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":15,\"ilOffset\":2,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":4,\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":5,\"resultType\":\"RuntimeSelfTest/FinalizerProof\",\"callee\":\"RuntimeSelfTest/FinalizerProof::.ctor:System.Void(System.Int32)\",\"reference\":{\"assemblyName\":\"RuntimeSelfTest\",\"subjectKind\":\"method\",\"subjectId\":\"RuntimeSelfTest/FinalizerProof::.ctor:System.Void(System.Int32)\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"RuntimeSelfTest\",\"subjectId\":\"RuntimeSelfTest/FinalizerProof\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"baseTypeSubjectId\":\"System.Private.CoreLib/System.Object\",\"implementedInterfaceSubjectIds\":[],\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":10,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":11,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"add\",\"opCode\":25,\"ilOffset\":13,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":14,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":15,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":10,\"ilOffset\":16,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"blt\",\"opCode\":21,\"operand\":4,\"ilOffset\":18,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":20,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.GC::Collect:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.GC::Collect:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.GC::Collect:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":25,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.GC::WaitForPendingFinalizers:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.GC::WaitForPendingFinalizers:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.GC::WaitForPendingFinalizers:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":30,\"resultType\":\"RuntimeSelfTest/GcAllocationProof\",\"callee\":\"RuntimeSelfTest/GcAllocationProof::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"RuntimeSelfTest\",\"subjectKind\":\"method\",\"subjectId\":\"RuntimeSelfTest/GcAllocationProof::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"RuntimeSelfTest\",\"subjectId\":\"RuntimeSelfTest/GcAllocationProof\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"baseTypeSubjectId\":\"System.Private.CoreLib/System.Object\",\"implementedInterfaceSubjectIds\":[],\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":35,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":36,\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":37,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":38,\"resultType\":\"System.Int32\",\"callee\":\"RuntimeSelfTest/GcAllocationProof::AllocateAndReturn:System.Int32(System.Int32)\",\"reference\":{\"assemblyName\":\"RuntimeSelfTest\",\"subjectKind\":\"method\",\"subjectId\":\"RuntimeSelfTest/GcAllocationProof::AllocateAndReturn:System.Int32(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"RuntimeSelfTest\",\"subjectId\":\"RuntimeSelfTest/GcAllocationProof::AllocateAndReturn:System.Int32(System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"RuntimeSelfTest_GcAllocationProof_AllocateAndReturn_System_Int32\",\"targetParameterCount\":1,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":43,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"RuntimeSelfTest\"}";
static const char kMethodJson_18[] = "{\"methodId\":\"runtime-self-test.integration-proof.gc-across-method-calls\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof::GcAcrossMethodCalls:System.Int32(System.Int32)\",\"signature\":\"System.Int32 IntegrationProof::GcAcrossMethodCalls(System.Int32)\",\"identity\":{\"assemblyName\":\"RuntimeSelfTest\",\"declaringTypeSubjectId\":\"RuntimeSelfTest/IntegrationProof\",\"definitionSubjectId\":\"RuntimeSelfTest/IntegrationProof::GcAcrossMethodCalls:System.Int32(System.Int32)\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof::GcAcrossMethodCalls:System.Int32(System.Int32)\",\"methodId\":\"runtime-self-test.integration-proof.gc-across-method-calls\",\"signature\":\"System.Int32 IntegrationProof::GcAcrossMethodCalls(System.Int32)\"},\"nativeSymbol\":\"RuntimeSelfTest_IntegrationProof_GcAcrossMethodCalls_System_Int32\",\"isStatic\":true,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":1,\"parameterAbis\":[{\"carrierKindCode\":1,\"typeShape\":2}],\"localCount\":7,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":0,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":1,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":3,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":92,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":6,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"add\",\"opCode\":25,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":16,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"mul\",\"opCode\":27,\"ilOffset\":11,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":12,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":13,\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Int32\",\"ilOffset\":14,\"resultType\":\"System.Int32[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Int32\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":3,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":20,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":5,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":39,\"ilOffset\":23,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":3,\"ilOffset\":25,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":5,\"ilOffset\":26,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":28,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":5,\"ilOffset\":29,\"isPreserveSig\":false},{\"op\":\"add\",\"opCode\":25,\"ilOffset\":31,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stelem\",\"opCode\":45,\"operand\":\"System.Int32\",\"ilOffset\":32,\"resultType\":\"System.Void\",\"runtimeServiceKind\":13,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":5,\"ilOffset\":33,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":35,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"add\",\"opCode\":25,\"ilOffset\":36,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":5,\"ilOffset\":37,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":5,\"ilOffset\":39,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":41,\"isPreserveSig\":false},{\"op\":\"blt\",\"opCode\":21,\"operand\":25,\"ilOffset\":42,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":4,\"ilOffset\":45,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":47,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":6,\"ilOffset\":48,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":67,\"ilOffset\":50,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":4,\"ilOffset\":52,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":3,\"ilOffset\":54,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":6,\"ilOffset\":55,\"isPreserveSig\":false},{\"op\":\"ldelem\",\"opCode\":44,\"operand\":\"System.Int32\",\"ilOffset\":57,\"resultType\":\"System.Int32\",\"runtimeServiceKind\":12,\"isPreserveSig\":false},{\"op\":\"add\",\"opCode\":25,\"ilOffset\":58,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":4,\"ilOffset\":59,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":6,\"ilOffset\":61,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":63,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"add\",\"opCode\":25,\"ilOffset\":64,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":6,\"ilOffset\":65,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":6,\"ilOffset\":67,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":69,\"isPreserveSig\":false},{\"op\":\"blt\",\"opCode\":21,\"operand\":52,\"ilOffset\":70,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":72,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":3,\"ilOffset\":73,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"rem\",\"opCode\":29,\"ilOffset\":74,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":2,\"ilOffset\":75,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"bne.un\",\"opCode\":79,\"operand\":83,\"ilOffset\":76,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":78,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.GC::Collect:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.GC::Collect:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.GC::Collect:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":83,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":4,\"ilOffset\":84,\"isPreserveSig\":false},{\"op\":\"add\",\"opCode\":25,\"ilOffset\":86,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":87,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":88,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":89,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"add\",\"opCode\":25,\"ilOffset\":90,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":91,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":92,\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":93,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"blt\",\"opCode\":21,\"operand\":6,\"ilOffset\":94,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":96,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":97,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"RuntimeSelfTest\"}";
static const char kMethodJson_19[] = "{\"methodId\":\"runtime-self-test.integration-proof.gc-and-delegate-invoke\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof::GcAndDelegateInvoke:System.Int32(System.Int32)\",\"signature\":\"System.Int32 IntegrationProof::GcAndDelegateInvoke(System.Int32)\",\"identity\":{\"assemblyName\":\"RuntimeSelfTest\",\"declaringTypeSubjectId\":\"RuntimeSelfTest/IntegrationProof\",\"definitionSubjectId\":\"RuntimeSelfTest/IntegrationProof::GcAndDelegateInvoke:System.Int32(System.Int32)\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof::GcAndDelegateInvoke:System.Int32(System.Int32)\",\"methodId\":\"runtime-self-test.integration-proof.gc-and-delegate-invoke\",\"signature\":\"System.Int32 IntegrationProof::GcAndDelegateInvoke(System.Int32)\"},\"nativeSymbol\":\"RuntimeSelfTest_IntegrationProof_GcAndDelegateInvoke_System_Int32\",\"isStatic\":true,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":1,\"parameterAbis\":[{\"carrierKindCode\":1,\"typeShape\":2}],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":0,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldftn\",\"opCode\":73,\"operand\":\"RuntimeSelfTest/IntegrationProof::DoubleValue:System.Int32(System.Int32)\",\"ilOffset\":1,\"resultType\":\"System.IntPtr\",\"callee\":\"RuntimeSelfTest/IntegrationProof::DoubleValue:System.Int32(System.Int32)\",\"reference\":{\"assemblyName\":\"RuntimeSelfTest\",\"subjectKind\":\"method\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof::DoubleValue:System.Int32(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"RuntimeSelfTest\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof::DoubleValue:System.Int32(System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":7,\"resultType\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Int32\\u003E\",\"callee\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Int32\\u003E::.ctor:System.Void(System.Object,System.IntPtr)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Int32\\u003E::.ctor:System.Void(System.Object,System.IntPtr)\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Int32\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":12,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":13,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.GC::Collect:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.GC::Collect:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.GC::Collect:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":18,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.GC::WaitForPendingFinalizers:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.GC::WaitForPendingFinalizers:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.GC::WaitForPendingFinalizers:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":23,\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Int32\\u003E::Invoke:System.Int32(System.Int32)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Int32\\u003E::Invoke:System.Int32(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Int32\\u003E::Invoke:System.Int32(System.Int32)\",\"openDefinitionSubjectId\":\"System.Private.CoreLib/System.Func\\u00602::Invoke:!1(!0)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Func\\u00602::Invoke:!1(!0);type=[System.Int32,System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Func\\u00602::Invoke:!1(!0);type=[System.Int32,System.Int32];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Func\\u00602::Invoke:!1(!0)\",\"typeArguments\":[\"System.Int32\",\"System.Int32\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Func\\u00602::Invoke:!1(!0);type=[System.Int32,System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Func\\u00602::Invoke:!1(!0);type=[System.Int32,System.Int32];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Int32\\u003E::Invoke:System.Int32(System.Int32)\",\"definitionSubjectId\":\"System.Private.CoreLib/System.Func\\u00602::Invoke:!1(!0)\",\"displaySubjectId\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Int32\\u003E::Invoke(System.Int32)\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Func\\u00602::Invoke:!1(!0)\",\"typeArguments\":[\"System.Int32\",\"System.Int32\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":30,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"RuntimeSelfTest\"}";
static const char kMethodJson_20[] = 
    "{\"methodId\":\"runtime-self-test.integration-proof.threaded-gc-allocation\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof::ThreadedGcAllocation:System.Int32(System.Int32)\",\"signature\":\"System.Int32 IntegrationProof::ThreadedGcAllocation(System.Int32)\",\"identity\":{\"assemblyName\":\"RuntimeSelfTest\",\"declaringTypeSubjectId\":\"RuntimeSelfTest/IntegrationProof\",\"definitionSubjectId\":\"RuntimeSelfTest/IntegrationProof::ThreadedGcAllocation:System.Int32(System.Int32)\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof::ThreadedGcAllocation:System.Int32(System.Int32)\",\"methodId\":\"runtime-self-test.integration-proof.threaded-gc-allocation\",\"signature\":\"System.Int32 IntegrationProof::ThreadedGcAllocation(System.Int32)\"},\"nativeSymbol\":\"RuntimeSelfTest_IntegrationProof_ThreadedGcAllocation_System_Int32\",\"isStatic\":true,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":1,\"parameterAbis\":[{\"carrierKindCode\":1,\"typeShape\":2}],\"localCount\":6,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":2,\"tryOffset\":108,\"tryLength\":18,\"handlerOffset\":126,\"handlerLength\":11}],\"instructions\":[{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":0,\"resultType\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0\",\"callee\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"RuntimeSelfTest\",\"subjectKind\":\"method\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"RuntimeSelfTest\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":true,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"baseTypeSubjectId\":\"System.Private.CoreLib/System.Object\",\"implementedInterfaceSubjectIds\":[],\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":6,\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stfld\",\"opCode\":11,\"operand\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::iterations\",\"ilOffset\":8,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"RuntimeSelfTest\",\"subjectKind\":\"field\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::iterations\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"RuntimeSelfTest\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::iterations\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":3,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":13,\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":14,\"resultType\":\"System.Private.CoreLib/System.Object\",\"callee\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"stfld\",\"opCode\":11,\"operand\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::sync\",\"ilOffset\":19,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"RuntimeSelfTest\",\"subjectKind\":\"field\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::sync\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"RuntimeSelfTest\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::sync\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Object\"},\"runtimeServiceKind\":3,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":24,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stfld\",\"opCode\":11,\"operand\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::sharedCounter\",\"ilOffset\":26,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"RuntimeSelfTest\",\"subjectKind\":\"field\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::sharedCounter\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"RuntimeSelfTest\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::sharedCounter\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":3,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":31,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stfld\",\"opCode\":11,\"operand\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::workerDone\",\"ilOffset\":33,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"RuntimeSelfTest\",\"subjectKind\":\"field\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::workerDone\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"RuntimeSelfTest\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::workerDone\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Boolean\"},\"runtimeServiceKind\":3,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":38,\"isPreserveSig\":false},{\"op\":\"ldftn\",\"opCode\":73,\"operand\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::\\u003CThreadedGcAllocation\\u003Eb__0:System.Void()\",\"ilOffset\":39,\"resultType\":\"System.IntPtr\",\"callee\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::\\u003CThreadedGcAllocation\\u003Eb__0:System.Void()\",\"reference\":{\"assemblyName\":\"RuntimeSelfTest\",\"subjectKind\":\"method\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::\\u003CThreadedGcAllocation\\u003Eb__0:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"RuntimeSelfTest\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::\\u003CThreadedGcAllocation\\u003Eb__0:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":45,\"resultType\":\"System.Threading.Thread/System.Threading.ThreadStart\",\"callee\":\"System.Threading.Thread/System.Threading.ThreadStart::.ctor:System.Void(System.Object,System.IntPtr)\",\"reference\":{\"assemblyName\":\"System.Threading.Thread\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading.Thread/System.Threading.ThreadStart::.ctor:System.Void(System.Object,System.IntPtr)\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Threading.Thread\",\"subjectId\":\"System.Threading.Thread/System.Threading.ThreadStart\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":50,\"resultType\":\"System.Threading.Thread/System.Threading.Thread\",\"callee\":\"System.Threading.Thread/System.Threading.Thread::.ctor:System.Void(System.Threading.ThreadStart)\",\"reference\":{\"assemblyName\":\"System.Threading.Thread\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading.Thread/System.Threading.Thread::.ctor:System.Void(System.Threading.ThreadStart)\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Threading.Thread\",\"subjectId\":\"System.Threading.Thread/System.Threading.Thread\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":55,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":56,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":57,\"resultType\":\"System.Void\",\"callee\":\"System.Threading.Thread/System.Threading.Thread::Start:System.Void()\",\"reference\":{\"assemblyName\":\"System.Threading.Thread\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading.Thread/System.Threading.Thread::Start:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading.Thread\",\"subjectId\":\"System.Threading.Thread/System.Threading.Thread::Start:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":62,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":63,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":81,\"ilOffset\":64,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":66,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.GC::Collect:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.GC::Collect:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.GC::Collect:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":71,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":72,\"resultType\":\"System.Void\",\"callee\":\"System.Threading.Thread/System.Threading.Thread::Sleep:System.Void(System.Int32)\",\"reference\":{\"assemblyName\":\"System.Threading.Thread\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading.Thread/System.Threading.Thread::Sleep:System.Void(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading.Thread\",\"subjectId\":\"System.Threading.Thread/System.Threading.Thread::Sleep:System.Void(System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":77,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":78,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"add\",\"opCode\":25,\"ilOffset\":79,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":80,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":81,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":82,\"isPreserveSig\":false},{\"op\":\"ldfld\",\"opCode\":10,\"operand\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::iterations\",\"ilOffset\":83,\"resultType\":\"System.Int32\",\"reference\":{\"assemblyName\":\"RuntimeSelfTest\",\"subjectKind\":\"field\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::iterations\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"RuntimeSelfTest\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::iterations\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":2,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":4,\"ilOffset\":88,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"div\",\"opCode\":28,\"ilOffset\":89,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"blt\",\"opCode\":21,\"operand\":66,\"ilOffset\":90,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":92,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":93,\"resultType\":\"System.Void\",\"callee\":\"System.Threading.Thread/System.Threading.Thread::Join:System.Void()\",\"reference\":{\"assemblyName\":\"System.Threading.Thread\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading.Thread/System.Threading.Thread::Join:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading.Thread\",\"subjectId\":\"System.Threading.Thread/System.Threading.Thread::Join:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":98,\"isPreserveSig\":false},{\"op\":\"ldfld\",\"opCode\":10,\"operand\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::sync\",\"ilOffset\":99,\"resultType\":\"System.Object\",\"reference\":{\"assemblyName\":\"RuntimeSelfTest\",\"subjectKind\":\"field\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::sync\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"RuntimeSelfTest\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::sync\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Object\"},\"runtimeServiceKind\":2,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":3,\"ilOffset\":104,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand"
    "\":0,\"ilOffset\":105,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":4,\"ilOffset\":106,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":3,\"ilOffset\":108,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":4,\"ilOffset\":109,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":111,\"resultType\":\"System.Void\",\"callee\":\"System.Threading/Monitor::Enter:System.Void(System.Object,System.Boolean\\u0026)\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/Monitor::Enter:System.Void(System.Object,System.Boolean\\u0026)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/Monitor::Enter:System.Void(System.Object,System.Boolean\\u0026)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":116,\"isPreserveSig\":false},{\"op\":\"ldfld\",\"opCode\":10,\"operand\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::sharedCounter\",\"ilOffset\":117,\"resultType\":\"System.Int32\",\"reference\":{\"assemblyName\":\"RuntimeSelfTest\",\"subjectKind\":\"field\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::sharedCounter\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"RuntimeSelfTest\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::sharedCounter\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":2,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":5,\"ilOffset\":122,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":137,\"ilOffset\":124,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":4,\"ilOffset\":126,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":136,\"ilOffset\":128,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":3,\"ilOffset\":130,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":131,\"resultType\":\"System.Void\",\"callee\":\"System.Threading/Monitor::Exit:System.Void(System.Object)\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/Monitor::Exit:System.Void(System.Object)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/Monitor::Exit:System.Void(System.Object)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"endfinally\",\"opCode\":51,\"ilOffset\":136,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":5,\"ilOffset\":137,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":139,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"RuntimeSelfTest\"}"
    ;
static const char kMethodJson_21[] = "{\"methodId\":\"runtime-self-test.integration-proof-c-display-class4-0.ctor\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::.ctor:System.Void()\",\"signature\":\"System.Void IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::.ctor()\",\"identity\":{\"assemblyName\":\"RuntimeSelfTest\",\"declaringTypeSubjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0\",\"definitionSubjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::.ctor:System.Void()\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::.ctor:System.Void()\",\"methodId\":\"runtime-self-test.integration-proof-c-display-class4-0.ctor\",\"signature\":\"System.Void IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::.ctor()\"},\"nativeSymbol\":\"RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__ctor\",\"isStatic\":false,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":6,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"RuntimeSelfTest\"}";
static const char kMethodJson_22[] = 
    "{\"methodId\":\"runtime-self-test.integration-proof-c-display-class4-0.threaded-gc-allocation-b-0\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::\\u003CThreadedGcAllocation\\u003Eb__0:System.Void()\",\"signature\":\"System.Void IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::\\u003CThreadedGcAllocation\\u003Eb__0()\",\"identity\":{\"assemblyName\":\"RuntimeSelfTest\",\"declaringTypeSubjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0\",\"definitionSubjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::\\u003CThreadedGcAllocation\\u003Eb__0:System.Void()\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::\\u003CThreadedGcAllocation\\u003Eb__0:System.Void()\",\"methodId\":\"runtime-self-test.integration-proof-c-display-class4-0.threaded-gc-allocation-b-0\",\"signature\":\"System.Void IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::\\u003CThreadedGcAllocation\\u003Eb__0()\"},\"nativeSymbol\":\"RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0_ThreadedGcAllocation_b__0\",\"isStatic\":false,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":6,\"exceptionRegionCount\":2,\"exceptionRegions\":[{\"handlingKindCode\":2,\"tryOffset\":26,\"tryLength\":29,\"handlerOffset\":55,\"handlerLength\":10},{\"handlingKindCode\":2,\"tryOffset\":89,\"tryLength\":18,\"handlerOffset\":107,\"handlerLength\":12}],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":0,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":1,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":69,\"ilOffset\":2,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":4,\"resultType\":\"RuntimeSelfTest/GcAllocationProof\",\"callee\":\"RuntimeSelfTest/GcAllocationProof::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"RuntimeSelfTest\",\"subjectKind\":\"method\",\"subjectId\":\"RuntimeSelfTest/GcAllocationProof::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"RuntimeSelfTest\",\"subjectId\":\"RuntimeSelfTest/GcAllocationProof\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"baseTypeSubjectId\":\"System.Private.CoreLib/System.Object\",\"implementedInterfaceSubjectIds\":[],\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":9,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":10,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":11,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":12,\"resultType\":\"System.Void\",\"callee\":\"RuntimeSelfTest/GcAllocationProof::set_Value:System.Void(System.Int32)\",\"reference\":{\"assemblyName\":\"RuntimeSelfTest\",\"subjectKind\":\"method\",\"subjectId\":\"RuntimeSelfTest/GcAllocationProof::set_Value:System.Void(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"RuntimeSelfTest\",\"subjectId\":\"RuntimeSelfTest/GcAllocationProof::set_Value:System.Void(System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"RuntimeSelfTest_GcAllocationProof_set_Value_System_Int32\",\"targetParameterCount\":1,\"targetReturnType\":\"System.Void\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":17,\"resultType\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0\",\"isPreserveSig\":false},{\"op\":\"ldfld\",\"opCode\":10,\"operand\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::sync\",\"ilOffset\":18,\"resultType\":\"System.Object\",\"reference\":{\"assemblyName\":\"RuntimeSelfTest\",\"subjectKind\":\"field\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::sync\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"RuntimeSelfTest\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::sync\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Object\"},\"runtimeServiceKind\":2,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":23,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":3,\"ilOffset\":25,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":26,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":3,\"ilOffset\":27,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":29,\"resultType\":\"System.Void\",\"callee\":\"System.Threading/Monitor::Enter:System.Void(System.Object,System.Boolean\\u0026)\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/Monitor::Enter:System.Void(System.Object,System.Boolean\\u0026)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/Monitor::Enter:System.Void(System.Object,System.Boolean\\u0026)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":34,\"resultType\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":35,\"resultType\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0\",\"isPreserveSig\":false},{\"op\":\"ldfld\",\"opCode\":10,\"operand\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::sharedCounter\",\"ilOffset\":36,\"resultType\":\"System.Int32\",\"reference\":{\"assemblyName\":\"RuntimeSelfTest\",\"subjectKind\":\"field\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::sharedCounter\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"RuntimeSelfTest\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::sharedCounter\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":2,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":41,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":42,\"resultType\":\"System.Int32\",\"callee\":\"RuntimeSelfTest/GcAllocationProof::get_Value:System.Int32()\",\"reference\":{\"assemblyName\":\"RuntimeSelfTest\",\"subjectKind\":\"method\",\"subjectId\":\"RuntimeSelfTest/GcAllocationProof::get_Value:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"RuntimeSelfTest\",\"subjectId\":\"RuntimeSelfTest/GcAllocationProof::get_Value:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"RuntimeSelfTest_GcAllocationProof_get_Value\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"add\",\"opCode\":25,\"ilOffset\":47,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stfld\",\"opCode\":11,\"operand\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::sharedCounter\",\"ilOffset\":48,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"RuntimeSelfTest\",\"subjectKind\":\"field\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::sharedCounter\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"RuntimeSelfTest\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::sharedCounter\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":3,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":65,\"ilOffset\":53,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":3,\"ilOffset\":55,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":64,\"ilOffset\":56,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":58,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":59,\"resultType\":\"System.Void\",\"callee\":\"System.Threading/Monitor::Exit:System.Void(System.Object)\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/Monitor::Exit:System.Void(System.Object)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/Monitor::Exit:System.Void(System.Object)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"endfinally\",\"opCode\":51,\"ilOffset\":64,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":65,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":66,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"add\",\"opCode\":25,\"ilOffset\":67,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":68,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":69,\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":70,\"resultType\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0\",\"isPreserveSig\":false},{\"op\":\"ldfld\",\"opCode\":10,\"operand\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::iterations\",\"ilOffset\":71,\"resultType\":\"System.Int32\",\"reference\":{\"assemblyName\":\"RuntimeSelfTest\",\"subjectKind\":\"field\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::iterations\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"RuntimeSelfTest\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::iterations\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":2,\"isPreserveSig\":false},{\"op\":\"blt\",\"opCode\":21,\"operand\":4,\"ilOffset\":76,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":78,\"resultType\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0\",\"isPreserveSig\":false},{\"op\":\"ldfld\",\"opCode\":10,\"operand\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::sync\",\"ilOffset\":79,\"resultType\":\"System.Object\",\"reference\":{\"assemblyName\":\"RuntimeSelfTest\",\"subjectKind\":\"field\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::sync\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"RuntimeSelfTest\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::sync\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Object\"},\"runtimeServiceKind\":2,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":4,\"ilOffset\":84,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":86,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":5,\"ilOffset\":87,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":4,\"ilOffset\":89,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":5,\"ilOffset\":91,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":93,\"resultType\":\"System.Void\",\"callee\":\"System.Threading/Monitor::Enter:System.Void(System.Object,System.Boolean\\u0026)\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/Monitor::Enter:System.Void(System.Object,System.Boolean\\u0026)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/Monitor::Enter:System.Void(System.Object,System.Boolean\\u0026)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":98,\"resultType\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":99,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stfld\",\"opCode\":11,\"operand\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::workerDone\",\"ilOffset\":100,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"RuntimeSelfTest\",\"subjectKind\":\"field\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::workerDone\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"RuntimeSelfTest\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0::workerDone\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"RuntimeSelfTest/IntegrationProof\\u002B\\u003C\\u003Ec__DisplayClass4_0\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Boolean\"},\"runtimeServiceKind\":3,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":119,\"ilOffset\":105,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":5,\"ilOffset\":107,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":118,\"ilOffset\":109,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":4,\"ilOffset\":111,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":113,\"resultType\":\"System.Void\",\"callee\":\"System.Threading/Monitor::Exit:System.Void(System.Object)\",\"referen"
    "ce\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/Monitor::Exit:System.Void(System.Object)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/Monitor::Exit:System.Void(System.Object)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"endfinally\",\"opCode\":51,\"ilOffset\":118,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":119,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"RuntimeSelfTest\"}"
    ;
static const char kMethodJson_23[] = "{\"methodId\":\"runtime-self-test.program.ctor\",\"subjectId\":\"RuntimeSelfTest/Program::.ctor:System.Void()\",\"signature\":\"System.Void Program::.ctor()\",\"identity\":{\"assemblyName\":\"RuntimeSelfTest\",\"declaringTypeSubjectId\":\"RuntimeSelfTest/Program\",\"definitionSubjectId\":\"RuntimeSelfTest/Program::.ctor:System.Void()\",\"subjectId\":\"RuntimeSelfTest/Program::.ctor:System.Void()\",\"methodId\":\"runtime-self-test.program.ctor\",\"signature\":\"System.Void Program::.ctor()\"},\"nativeSymbol\":\"RuntimeSelfTest_Program__ctor\",\"isStatic\":false,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"RuntimeSelfTest/Program\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":6,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"RuntimeSelfTest\"}";
static const char kMethodJson_24[] = 
    "{\"methodId\":\"runtime-self-test.program.main\",\"subjectId\":\"RuntimeSelfTest/Program::\\u003CMain\\u003E$:System.Void(System.String[])\",\"signature\":\"System.Void Program::\\u003CMain\\u003E$(System.String[])\",\"identity\":{\"assemblyName\":\"RuntimeSelfTest\",\"declaringTypeSubjectId\":\"RuntimeSelfTest/Program\",\"definitionSubjectId\":\"RuntimeSelfTest/Program::\\u003CMain\\u003E$:System.Void(System.String[])\",\"subjectId\":\"RuntimeSelfTest/Program::\\u003CMain\\u003E$:System.Void(System.String[])\",\"methodId\":\"runtime-self-test.program.main\",\"signature\":\"System.Void Program::\\u003CMain\\u003E$(System.String[])\"},\"nativeSymbol\":\"RuntimeSelfTest_Program_Main_System_String\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":1,\"parameterAbis\":[{\"carrierKindCode\":2,\"typeShape\":1}],\"localCount\":6,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":292,\"tryLength\":21,\"handlerOffset\":313,\"handlerLength\":13,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"RTS_ARITH_ADD=\",\"ilOffset\":0,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":2,\"ilOffset\":5,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":3,\"ilOffset\":6,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"callee\":\"RuntimeSelfTest/ArithmeticProof::Add:System.Int32(System.Int32,System.Int32)\",\"reference\":{\"assemblyName\":\"RuntimeSelfTest\",\"subjectKind\":\"method\",\"subjectId\":\"RuntimeSelfTest/ArithmeticProof::Add:System.Int32(System.Int32,System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"RuntimeSelfTest\",\"subjectId\":\"RuntimeSelfTest/ArithmeticProof::Add:System.Int32(System.Int32,System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"RuntimeSelfTest_ArithmeticProof_Add_System_Int32_System_Int32\",\"targetParameterCount\":2,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":3,\"ilOffset\":12,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":3,\"ilOffset\":13,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":15,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":20,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":25,\"resultType\":\"System.Void\",\"callee\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"reference\":{\"assemblyName\":\"System.Console\",\"subjectKind\":\"method\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Console\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"RTS_ARITH_SUB=\",\"ilOffset\":30,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":10,\"ilOffset\":35,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":4,\"ilOffset\":37,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":38,\"resultType\":\"System.Int32\",\"callee\":\"RuntimeSelfTest/ArithmeticProof::Subtract:System.Int32(System.Int32,System.Int32)\",\"reference\":{\"assemblyName\":\"RuntimeSelfTest\",\"subjectKind\":\"method\",\"subjectId\":\"RuntimeSelfTest/ArithmeticProof::Subtract:System.Int32(System.Int32,System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"RuntimeSelfTest\",\"subjectId\":\"RuntimeSelfTest/ArithmeticProof::Subtract:System.Int32(System.Int32,System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"RuntimeSelfTest_ArithmeticProof_Subtract_System_Int32_System_Int32\",\"targetParameterCount\":2,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":3,\"ilOffset\":43,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":3,\"ilOffset\":44,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":46,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":51,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":56,\"resultType\":\"System.Void\",\"callee\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"reference\":{\"assemblyName\":\"System.Console\",\"subjectKind\":\"method\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Console\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"RTS_ARITH_MUL=\",\"ilOffset\":61,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":3,\"ilOffset\":66,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":4,\"ilOffset\":67,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":68,\"resultType\":\"System.Int32\",\"callee\":\"RuntimeSelfTest/ArithmeticProof::Multiply:System.Int32(System.Int32,System.Int32)\",\"reference\":{\"assemblyName\":\"RuntimeSelfTest\",\"subjectKind\":\"method\",\"subjectId\":\"RuntimeSelfTest/ArithmeticProof::Multiply:System.Int32(System.Int32,System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"RuntimeSelfTest\",\"subjectId\":\"RuntimeSelfTest/ArithmeticProof::Multiply:System.Int32(System.Int32,System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"RuntimeSelfTest_ArithmeticProof_Multiply_System_Int32_System_Int32\",\"targetParameterCount\":2,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":3,\"ilOffset\":73,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":3,\"ilOffset\":74,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":76,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":81,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":86,\"resultType\":\"System.Void\",\"callee\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"reference\":{\"assemblyName\":\"System.Console\",\"subjectKind\":\"method\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Console\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":91,\"resultType\":\"RuntimeSelfTest/GcAllocationProof\",\"callee\":\"RuntimeSelfTest/GcAllocationProof::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"RuntimeSelfTest\",\"subjectKind\":\"method\",\"subjectId\":\"RuntimeSelfTest/GcAllocationProof::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"RuntimeSelfTest\",\"subjectId\":\"RuntimeSelfTest/GcAllocationProof\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"baseTypeSubjectId\":\"System.Private.CoreLib/System.Object\",\"implementedInterfaceSubjectIds\":[],\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":96,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"RTS_GC_ALLOC=\",\"ilOffset\":97,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":102,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":103,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":105,\"resultType\":\"System.Int32\",\"callee\":\"RuntimeSelfTest/GcAllocationProof::AllocateAndReturn:System.Int32(System.Int32)\",\"reference\":{\"assemblyName\":\"RuntimeSelfTest\",\"subjectKind\":\"method\",\"subjectId\":\"RuntimeSelfTest/GcAllocationProof::AllocateAndReturn:System.Int32(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"RuntimeSelfTest\",\"subjectId\":\"RuntimeSelfTest/GcAllocationProof::AllocateAndReturn:System.Int32(System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"RuntimeSelfTest_GcAllocationProof_AllocateAndReturn_System_Int32\",\"targetParameterCount\":1,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":3,\"ilOffset\":110,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":3,\"ilOffset\":111,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":113,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":118,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":123,\"resultType\":\"System.Void\",\"callee\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"reference\":{\"assemblyName\":\"System.Console\",\"subjectKind\":\"method\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Console\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"typeShape\":0,\"arrayElementTypeSh"
    "ape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"RTS_GC_ALLOC_ATOMIC=\",\"ilOffset\":128,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":133,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":100,\"ilOffset\":134,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":136,\"resultType\":\"System.Int32\",\"callee\":\"RuntimeSelfTest/GcAllocationProof::AllocateAtomic:System.Int32(System.Int32)\",\"reference\":{\"assemblyName\":\"RuntimeSelfTest\",\"subjectKind\":\"method\",\"subjectId\":\"RuntimeSelfTest/GcAllocationProof::AllocateAtomic:System.Int32(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"RuntimeSelfTest\",\"subjectId\":\"RuntimeSelfTest/GcAllocationProof::AllocateAtomic:System.Int32(System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"RuntimeSelfTest_GcAllocationProof_AllocateAtomic_System_Int32\",\"targetParameterCount\":1,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":3,\"ilOffset\":141,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":3,\"ilOffset\":142,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":144,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":149,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":154,\"resultType\":\"System.Void\",\"callee\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"reference\":{\"assemblyName\":\"System.Console\",\"subjectKind\":\"method\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Console\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":159,\"resultType\":\"RuntimeSelfTest/StringOperations\",\"callee\":\"RuntimeSelfTest/StringOperations::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"RuntimeSelfTest\",\"subjectKind\":\"method\",\"subjectId\":\"RuntimeSelfTest/StringOperations::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"RuntimeSelfTest\",\"subjectId\":\"RuntimeSelfTest/StringOperations\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"baseTypeSubjectId\":\"System.Private.CoreLib/System.Object\",\"implementedInterfaceSubjectIds\":[],\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":164,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"RTS_STR_FORMAT=\",\"ilOffset\":165,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":170,\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":171,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":3,\"ilOffset\":176,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":177,\"resultType\":\"System.String\",\"callee\":\"RuntimeSelfTest/StringOperations::FormatString:System.String(System.String,System.Int32)\",\"reference\":{\"assemblyName\":\"RuntimeSelfTest\",\"subjectKind\":\"method\",\"subjectId\":\"RuntimeSelfTest/StringOperations::FormatString:System.String(System.String,System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"RuntimeSelfTest\",\"subjectId\":\"RuntimeSelfTest/StringOperations::FormatString:System.String(System.String,System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"RuntimeSelfTest_StringOperations_FormatString_System_String_System_Int32\",\"targetParameterCount\":2,\"targetReturnType\":\"System.String\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":182,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":187,\"resultType\":\"System.Void\",\"callee\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"reference\":{\"assemblyName\":\"System.Console\",\"subjectKind\":\"method\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Console\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"RTS_STR_CONCAT=\",\"ilOffset\":192,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":197,\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":198,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"world\",\"ilOffset\":203,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":208,\"resultType\":\"System.String\",\"callee\":\"RuntimeSelfTest/StringOperations::ConcatStrings:System.String(System.String,System.String)\",\"reference\":{\"assemblyName\":\"RuntimeSelfTest\",\"subjectKind\":\"method\",\"subjectId\":\"RuntimeSelfTest/StringOperations::ConcatStrings:System.String(System.String,System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"RuntimeSelfTest\",\"subjectId\":\"RuntimeSelfTest/StringOperations::ConcatStrings:System.String(System.String,System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"RuntimeSelfTest_StringOperations_ConcatStrings_System_String_System_String\",\"targetParameterCount\":2,\"targetReturnType\":\"System.String\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":213,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":218,\"resultType\":\"System.Void\",\"callee\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"reference\":{\"assemblyName\":\"System.Console\",\"subjectKind\":\"method\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Console\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":223,\"resultType\":\"RuntimeSelfTest/ArrayOperations\",\"callee\":\"RuntimeSelfTest/ArrayOperations::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"RuntimeSelfTest\",\"subjectKind\":\"method\",\"subjectId\":\"RuntimeSelfTest/ArrayOperations::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"RuntimeSelfTest\",\"subjectId\":\"RuntimeSelfTest/ArrayOperations\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"baseTypeSubjectId\":\"System.Private.CoreLib/System.Object\",\"implementedInterfaceSubjectIds\":[],\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":228,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"RTS_ARR_SUM=\",\"ilOffset\":229,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":234,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":5,\"ilOffset\":235,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":236,\"resultType\":\"System.Int32\",\"callee\":\"RuntimeSelfTest/ArrayOperations::CreateAndSum:System.Int32(System.Int32)\",\"reference\":{\"assemblyName\":\"RuntimeSelfTest\",\"subjectKind\":\"method\",\"subjectId\":\"RuntimeSelfTest/ArrayOperations::CreateAndSum:System.Int32(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"RuntimeSelfTest\",\"subjectId\":\"RuntimeSelfTest/ArrayOperations::CreateAndSum:System.Int32(System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"RuntimeSelfTest_ArrayOperations_CreateAndSum_System_Int32\",\"targetParameterCount\":1,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":3,\"ilOffset\":241,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":3,\"ilOffset\":242,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":244,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":249,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":254,\"resultType\":\"System.Void\",\"callee\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"reference\":{\"assemblyName\":\"System.Console\",\"subjectKind\":\"method\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Console\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"RTS_ARR_LEN=\",\"ilOffset\":259,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":264,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":10,\"ilOffset\":265,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":267,\"resultType\":\"System.Int32[]\",\"callee\":\"RuntimeSelfTest/ArrayOperations::CreateArray:System.Int32[](System.Int32)\",\"reference\":{\"assemblyName\":\"RuntimeSelfTest\",\"subjectKind\":\"method\",\"subjectId\":\"RuntimeSelfTest/ArrayOperations::CreateArray:System.Int32[](System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"RuntimeSelfTest\",\"subjectId\":\"RuntimeSelfTest/ArrayOperations::CreateArray:System.Int32[](System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"RuntimeSelfTest_ArrayOperations_CreateArray_System_Int32\",\"targetParameterCount\":1,\"targetReturnType\":\"System.Int32[]\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"ldlen\",\"opCode\":46,\"ilOffset\":272,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"conv.i4\",\"opCode\":39,\"ilOffset\":273,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":3,\"ilOffset\":274,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand"
    "\":3,\"ilOffset\":275,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":277,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":282,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":287,\"resultType\":\"System.Void\",\"callee\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"reference\":{\"assemblyName\":\"System.Console\",\"subjectKind\":\"method\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Console\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":292,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":4,\"ilOffset\":293,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":5,\"ilOffset\":295,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":4,\"ilOffset\":296,\"isPreserveSig\":false},{\"op\":\"div\",\"opCode\":28,\"ilOffset\":298,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":5,\"ilOffset\":299,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"RTS_EXC_UNEXPECTED=reached\",\"ilOffset\":301,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":306,\"resultType\":\"System.Void\",\"callee\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"reference\":{\"assemblyName\":\"System.Console\",\"subjectKind\":\"method\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Console\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":326,\"ilOffset\":311,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":313,\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"RTS_EXC_HANDLED=-1\",\"ilOffset\":314,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":319,\"resultType\":\"System.Void\",\"callee\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"reference\":{\"assemblyName\":\"System.Console\",\"subjectKind\":\"method\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Console\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":326,\"ilOffset\":324,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"RTS_INT_DELEGATE=\",\"ilOffset\":326,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":21,\"ilOffset\":331,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":333,\"resultType\":\"System.Int32\",\"callee\":\"RuntimeSelfTest/IntegrationProof::GcAndDelegateInvoke:System.Int32(System.Int32)\",\"reference\":{\"assemblyName\":\"RuntimeSelfTest\",\"subjectKind\":\"method\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof::GcAndDelegateInvoke:System.Int32(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"RuntimeSelfTest\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof::GcAndDelegateInvoke:System.Int32(System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"RuntimeSelfTest_IntegrationProof_GcAndDelegateInvoke_System_Int32\",\"targetParameterCount\":1,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":3,\"ilOffset\":338,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":3,\"ilOffset\":339,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":341,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":346,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":351,\"resultType\":\"System.Void\",\"callee\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"reference\":{\"assemblyName\":\"System.Console\",\"subjectKind\":\"method\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Console\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"RTS_INT_FINALIZER=\",\"ilOffset\":356,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":99,\"ilOffset\":361,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":363,\"resultType\":\"System.Int32\",\"callee\":\"RuntimeSelfTest/IntegrationProof::FinalizerAndNewAllocation:System.Int32(System.Int32)\",\"reference\":{\"assemblyName\":\"RuntimeSelfTest\",\"subjectKind\":\"method\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof::FinalizerAndNewAllocation:System.Int32(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"RuntimeSelfTest\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof::FinalizerAndNewAllocation:System.Int32(System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"RuntimeSelfTest_IntegrationProof_FinalizerAndNewAllocation_System_Int32\",\"targetParameterCount\":1,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":3,\"ilOffset\":368,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":3,\"ilOffset\":369,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":371,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":376,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":381,\"resultType\":\"System.Void\",\"callee\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"reference\":{\"assemblyName\":\"System.Console\",\"subjectKind\":\"method\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Console\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"RTS_INT_GC_ACROSS=\",\"ilOffset\":386,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":4,\"ilOffset\":391,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":392,\"resultType\":\"System.Int32\",\"callee\":\"RuntimeSelfTest/IntegrationProof::GcAcrossMethodCalls:System.Int32(System.Int32)\",\"reference\":{\"assemblyName\":\"RuntimeSelfTest\",\"subjectKind\":\"method\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof::GcAcrossMethodCalls:System.Int32(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"RuntimeSelfTest\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof::GcAcrossMethodCalls:System.Int32(System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"RuntimeSelfTest_IntegrationProof_GcAcrossMethodCalls_System_Int32\",\"targetParameterCount\":1,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":3,\"ilOffset\":397,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":3,\"ilOffset\":398,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":400,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":405,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":410,\"resultType\":\"System.Void\",\"callee\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"reference\":{\"assemblyName\":\"System.Console\",\"subjectKind\":\"method\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Console\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"RTS_INT_THREADED=\",\"ilOffset\":415,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":10,\"ilOffset\":420,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":422,\"resultType\":\"System.Int32\",\"callee\":\"RuntimeSelfTest/IntegrationProof::ThreadedGcAllocation:System.Int32(System.Int32)\",\"reference\":{\"assemblyName\":\"RuntimeSelfTest\",\"subjectKind\":\"method\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof::ThreadedGcAllocation:System.Int32(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"RuntimeSelfTest\",\"subjectId\":\"RuntimeSelfTest/IntegrationProof::ThreadedGcAllocation:System.Int32(System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTy"
    "peKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"RuntimeSelfTest_IntegrationProof_ThreadedGcAllocation_System_Int32\",\"targetParameterCount\":1,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":3,\"ilOffset\":427,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":3,\"ilOffset\":428,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":430,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":435,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":440,\"resultType\":\"System.Void\",\"callee\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"reference\":{\"assemblyName\":\"System.Console\",\"subjectKind\":\"method\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Console\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"RTS_DONE=ok\",\"ilOffset\":445,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":450,\"resultType\":\"System.Void\",\"callee\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"reference\":{\"assemblyName\":\"System.Console\",\"subjectKind\":\"method\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Console\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":455,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"RuntimeSelfTest\"}"
    ;
static const char kMethodJson_25[] = "{\"methodId\":\"runtime-self-test.string-operations.ctor\",\"subjectId\":\"RuntimeSelfTest/StringOperations::.ctor:System.Void()\",\"signature\":\"System.Void StringOperations::.ctor()\",\"identity\":{\"assemblyName\":\"RuntimeSelfTest\",\"declaringTypeSubjectId\":\"RuntimeSelfTest/StringOperations\",\"definitionSubjectId\":\"RuntimeSelfTest/StringOperations::.ctor:System.Void()\",\"subjectId\":\"RuntimeSelfTest/StringOperations::.ctor:System.Void()\",\"methodId\":\"runtime-self-test.string-operations.ctor\",\"signature\":\"System.Void StringOperations::.ctor()\"},\"nativeSymbol\":\"RuntimeSelfTest_StringOperations__ctor\",\"isStatic\":false,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"RuntimeSelfTest/StringOperations\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":6,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"RuntimeSelfTest\"}";
static const char kMethodJson_26[] = "{\"methodId\":\"runtime-self-test.string-operations.concat-strings\",\"subjectId\":\"RuntimeSelfTest/StringOperations::ConcatStrings:System.String(System.String,System.String)\",\"signature\":\"System.String StringOperations::ConcatStrings(System.String,System.String)\",\"identity\":{\"assemblyName\":\"RuntimeSelfTest\",\"declaringTypeSubjectId\":\"RuntimeSelfTest/StringOperations\",\"definitionSubjectId\":\"RuntimeSelfTest/StringOperations::ConcatStrings:System.String(System.String,System.String)\",\"subjectId\":\"RuntimeSelfTest/StringOperations::ConcatStrings:System.String(System.String,System.String)\",\"methodId\":\"runtime-self-test.string-operations.concat-strings\",\"signature\":\"System.String StringOperations::ConcatStrings(System.String,System.String)\"},\"nativeSymbol\":\"RuntimeSelfTest_StringOperations_ConcatStrings_System_String_System_String\",\"isStatic\":false,\"returnType\":\"System.String\",\"returnAbi\":{\"carrierKindCode\":2,\"typeSubjectId\":\"System.Private.CoreLib/System.String\",\"typeShape\":1},\"parameterCount\":2,\"parameterAbis\":[{\"carrierKindCode\":2,\"typeSubjectId\":\"System.Private.CoreLib/System.String\",\"typeShape\":1},{\"carrierKindCode\":2,\"typeSubjectId\":\"System.Private.CoreLib/System.String\",\"typeShape\":1}],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":0,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":2,\"ilOffset\":1,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":2,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":7,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"RuntimeSelfTest\"}";
static const char kMethodJson_27[] = "{\"methodId\":\"runtime-self-test.string-operations.format-string\",\"subjectId\":\"RuntimeSelfTest/StringOperations::FormatString:System.String(System.String,System.Int32)\",\"signature\":\"System.String StringOperations::FormatString(System.String,System.Int32)\",\"identity\":{\"assemblyName\":\"RuntimeSelfTest\",\"declaringTypeSubjectId\":\"RuntimeSelfTest/StringOperations\",\"definitionSubjectId\":\"RuntimeSelfTest/StringOperations::FormatString:System.String(System.String,System.Int32)\",\"subjectId\":\"RuntimeSelfTest/StringOperations::FormatString:System.String(System.String,System.Int32)\",\"methodId\":\"runtime-self-test.string-operations.format-string\",\"signature\":\"System.String StringOperations::FormatString(System.String,System.Int32)\"},\"nativeSymbol\":\"RuntimeSelfTest_StringOperations_FormatString_System_String_System_Int32\",\"isStatic\":false,\"returnType\":\"System.String\",\"returnAbi\":{\"carrierKindCode\":2,\"typeSubjectId\":\"System.Private.CoreLib/System.String\",\"typeShape\":1},\"parameterCount\":2,\"parameterAbis\":[{\"carrierKindCode\":2,\"typeSubjectId\":\"System.Private.CoreLib/System.String\",\"typeShape\":1},{\"carrierKindCode\":1,\"typeShape\":2}],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":0,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldarga\",\"opCode\":75,\"operand\":2,\"ilOffset\":1,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":3,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":8,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":13,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"RuntimeSelfTest\"}";

extern "C" const JitMethodEntry kChaosJitMethodEntries[28] =
{
    { kMethodJson_0, 1603u, 0x00000016u, 0u },
    { kMethodJson_1, 1643u, 0x00000018u, 0u },
    { kMethodJson_2, 1643u, 0x00000017u, 0u },
    { kMethodJson_3, 1813u, 0x0000001Bu, 0u },
    { kMethodJson_4, 5446u, 0x0000001Au, 0u },
    { kMethodJson_5, 1912u, 0x00000019u, 0u },
    { kMethodJson_6, 1865u, 0x00000015u, 0u },
    { kMethodJson_7, 1264u, 0x00000014u, 0u },
    { kMethodJson_8, 2820u, 0x00000027u, 0u },
    { kMethodJson_9, 2119u, 0x00000028u, 0u },
    { kMethodJson_10, 1915u, 0x00000026u, 0u },
    { kMethodJson_11, 1835u, 0x00000020u, 0u },
    { kMethodJson_12, 3808u, 0x0000001Eu, 0u },
    { kMethodJson_13, 2780u, 0x0000001Fu, 0u },
    { kMethodJson_14, 1992u, 0x0000001Cu, 0u },
    { kMethodJson_15, 2194u, 0x0000001Du, 0u },
    { kMethodJson_16, 1565u, 0x00000022u, 0u },
    { kMethodJson_17, 6120u, 0x00000023u, 0u },
    { kMethodJson_18, 8349u, 0x00000024u, 0u },
    { kMethodJson_19, 6692u, 0x00000021u, 0u },
    { kMethodJson_20, 17902u, 0x00000025u, 0u },
    { kMethodJson_21, 2134u, 0x0000002Cu, 0u },
    { kMethodJson_22, 15262u, 0x0000002Du, 0u },
    { kMethodJson_23, 1731u, 0x00000013u, 0u },
    { kMethodJson_24, 46967u, 0x00000012u, 0u },
    { kMethodJson_25, 1823u, 0x0000002Bu, 0u },
    { kMethodJson_26, 2477u, 0x0000002Au, 0u },
    { kMethodJson_27, 3018u, 0x00000029u, 0u }
};

extern "C" const uint32_t kChaosJitMethodEntryCount = 28u;

extern "C" void ChaosJitRegisterAll() {
    RegisterJitMethods(kChaosJitMethodEntries, kChaosJitMethodEntryCount);
}