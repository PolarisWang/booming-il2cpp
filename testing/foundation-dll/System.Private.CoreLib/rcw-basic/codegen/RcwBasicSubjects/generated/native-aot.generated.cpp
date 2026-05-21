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

namespace chaos::il2cpp::codegen::RcwBasicSubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


const void* chaos_vtable_RcwBasicSubjects_SimpleMath[];
inline TypeInfoV0 chaos_mt_RcwBasicSubjects__PrivateImplementationDetails_ = {{nullptr, nullptr, 183810839601874482ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_RcwBasicSubjects__PrivateImplementationDetails_ = static_cast<CHAOS_IL2CPP_INTPTR>(183810839601874482ULL);
inline TypeInfoV0 chaos_mt_RcwBasicSubjects_RcwBasicSubjects = {{nullptr, nullptr, 3771092768718678272ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_RcwBasicSubjects_RcwBasicSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(3771092768718678272ULL);
inline TypeInfoV0 chaos_mt_RcwBasicSubjects_RuntimeState = {{nullptr, nullptr, 2817087881528303146ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_RcwBasicSubjects_RuntimeState = static_cast<CHAOS_IL2CPP_INTPTR>(2817087881528303146ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Object = {{nullptr, nullptr, 15228727185366376748ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_INTPTR>(15228727185366376748ULL);
inline TypeInfoV0 chaos_mt_RcwBasicSubjects_SimpleMath = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_RcwBasicSubjects_SimpleMath, 5661985370520645527ULL, 3u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_RcwBasicSubjects_SimpleMath = static_cast<CHAOS_IL2CPP_INTPTR>(5661985370520645527ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Byte = {{nullptr, nullptr, 3217969471912086765ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Byte = static_cast<CHAOS_IL2CPP_INTPTR>(3217969471912086765ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_IntPtr = {{nullptr, nullptr, 15622295425673451578ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_IntPtr = static_cast<CHAOS_IL2CPP_INTPTR>(15622295425673451578ULL);
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
extern "C" void RcwBasicSubjects_SimpleMath__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 RcwBasicSubjects_SimpleMath_Add_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INT32 RcwBasicSubjects_SimpleMath_Multiply_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
const void* chaos_vtable_RcwBasicSubjects_SimpleMath[] =
{
	reinterpret_cast<void*>(RcwBasicSubjects_SimpleMath__ctor),
	reinterpret_cast<void*>(RcwBasicSubjects_SimpleMath_Add_System_Int32_System_Int32),
	reinterpret_cast<void*>(RcwBasicSubjects_SimpleMath_Multiply_System_Int32_System_Int32),
};
static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_RcwBasicSubjects_SimpleMath[] =
{
	{ 0x0000001Au, reinterpret_cast<void*>(&RcwBasicSubjects_SimpleMath__ctor) },
	{ 0x00000018u, reinterpret_cast<void*>(&RcwBasicSubjects_SimpleMath_Add_System_Int32_System_Int32) },
	{ 0x00000019u, reinterpret_cast<void*>(&RcwBasicSubjects_SimpleMath_Multiply_System_Int32_System_Int32) },
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

struct chaos_type_RcwBasicSubjects__PrivateImplementationDetails_
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR field_RcwBasicSubjects__PrivateImplementationDetails___DA5979D52E989A03770DCD85298CC184B885A2D2AF51AEB79151BB6630C1D46E = 0;
};

struct chaos_type_RcwBasicSubjects_RcwBasicSubjects
{
	ThinLockableHeader header{};
};

struct chaos_type_RcwBasicSubjects_RuntimeState
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Private_CoreLib_System_Object
{
	ThinLockableHeader header{};
};

struct chaos_type_RcwBasicSubjects_SimpleMath : public chaos_type_System_Private_CoreLib_System_Object
{
};

struct chaos_type_System_Private_CoreLib_System_Byte
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Private_CoreLib_System_IntPtr
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

CHAOS_IL2CPP_INT32 chaos_static_RcwBasicSubjects_RcwBasicSubjects___exitCode = 0;
CHAOS_IL2CPP_INTPTR chaos_static_RcwBasicSubjects_RuntimeState___state = 0;
CHAOS_IL2CPP_INTPTR chaos_static_System_Private_CoreLib_System_IntPtr__Zero = 0;

const CHAOS_IL2CPP_UINT8 chaos_static_field_data_RcwBasicSubjects__PrivateImplementationDetails___DA5979D52E989A03770DCD85298CC184B885A2D2AF51AEB79151BB6630C1D46E[] =
{
	0xC0, 0xCF, 0x4F, 0x7E, 0x8F, 0x3A, 0x0C, 0x4A, 0x8C, 0x9D, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0x6F,
};

bool chaos_try_get_static_field_data(
	CHAOS_IL2CPP_INTPTR chaos_field_handle,
	const CHAOS_IL2CPP_UINT8*& chaos_data,
	CHAOS_IL2CPP_SIZE& chaos_size)
{
	if (chaos_field_handle == static_cast<CHAOS_IL2CPP_INTPTR>(80024075u))
	{
		chaos_data = chaos_static_field_data_RcwBasicSubjects__PrivateImplementationDetails___DA5979D52E989A03770DCD85298CC184B885A2D2AF51AEB79151BB6630C1D46E;
		chaos_size = sizeof(chaos_static_field_data_RcwBasicSubjects__PrivateImplementationDetails___DA5979D52E989A03770DCD85298CC184B885A2D2AF51AEB79151BB6630C1D46E);
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

static void chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	ChaosObjectCtor(chaos_fn_arg_0);
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

extern "C" void RcwBasicSubjects_RcwBasicSubjects_Subject_0(void);
extern "C" void RcwBasicSubjects_RcwBasicSubjects_Subject_1(void);
extern "C" void RcwBasicSubjects_RcwBasicSubjects_Subject_2(void);
extern "C" void RcwBasicSubjects_RcwBasicSubjects_Subject_3(void);
extern "C" void RcwBasicSubjects_RcwBasicSubjects_Subject_4(void);
extern "C" void RcwBasicSubjects_RcwBasicSubjects_Subject_5(void);
extern "C" void RcwBasicSubjects_RcwBasicSubjects_Subject_6(void);
extern "C" void RcwBasicSubjects_AutoGeneratedProgram__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void RcwBasicSubjects_AutoGeneratedProgram_Main_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void RcwBasicSubjects_ConstantFortyTwo__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 RcwBasicSubjects_ConstantFortyTwo_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INTPTR RcwBasicSubjects_RcwBasicNativeEntry_CreateCcwForSimpleMath_System_IntPtr(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 RcwBasicSubjects_RcwBasicNativeEntry_MarshalCallComMethod_System_IntPtr_System_Int32_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2, CHAOS_IL2CPP_INT32 chaos_fn_arg_3);
extern "C" CHAOS_IL2CPP_INT32 RcwBasicSubjects_RcwBasicNativeEntry_MarshalCallDirectComMethod_System_IntPtr_System_Int32_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2, CHAOS_IL2CPP_INT32 chaos_fn_arg_3);
extern "C" CHAOS_IL2CPP_INTPTR RcwBasicSubjects_RcwBasicNativeEntry_MarshalCreateCcw_System_IntPtr_System_IntPtr(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR RcwBasicSubjects_RcwBasicNativeEntry_MarshalCreateRcw_System_IntPtr(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INTPTR RcwBasicSubjects_RcwBasicNativeEntry_MarshalGetRcwUnknown_System_IntPtr(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INTPTR RcwBasicSubjects_RcwBasicNativeEntry_MarshalRcwQueryInterface_System_IntPtr_System_IntPtr(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" void RcwBasicSubjects_RcwBasicNativeEntry_MarshalReleaseRcw_System_IntPtr(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 RcwBasicSubjects_RcwBasicNativeEntry_Run_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 RcwBasicSubjects_RcwBasicNativeEntry_TestRcwDirectVtable(void);
extern "C" CHAOS_IL2CPP_INT32 RcwBasicSubjects_RcwBasicNativeEntry_TestRcwMultipleWrappers(void);
extern "C" CHAOS_IL2CPP_INT32 RcwBasicSubjects_RcwBasicNativeEntry_TestRcwQiUnknownInterface(void);
extern "C" CHAOS_IL2CPP_INT32 RcwBasicSubjects_RcwBasicNativeEntry_TestRcwRoundTripIdentity(void);
extern "C" CHAOS_IL2CPP_INT32 RcwBasicSubjects_RcwBasicNativeEntry_TestRcwRoundTripQi(void);
extern "C" CHAOS_IL2CPP_INT32 RcwBasicSubjects_RcwBasicNativeEntry_TestRcwVtableMethodCall(void);
extern "C" CHAOS_IL2CPP_INTPTR RcwBasicSubjects_RuntimeState_Get(void);
extern "C" void RcwBasicSubjects_RuntimeState_Set_System_IntPtr(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void RcwBasicSubjects_SimpleMath__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 RcwBasicSubjects_SimpleMath_Add_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INT32 RcwBasicSubjects_SimpleMath_Multiply_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[32] = {
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
	2u,
	6u,
	10u,
	12u,
	13u,
	14u,
	16u,
	17u,
	18u,
	18u,
	18u,
	18u,
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
	::ChaosAbiMethodEntryV0 entries[31];
	CHAOS_IL2CPP_UINT8 params[23];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		31u,
		23u,
		786386331u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // RcwBasicSubjects_RcwBasicSubjects_Subject_0
		{ 0u, 0u },  // RcwBasicSubjects_RcwBasicSubjects_Subject_1
		{ 0u, 0u },  // RcwBasicSubjects_RcwBasicSubjects_Subject_2
		{ 0u, 0u },  // RcwBasicSubjects_RcwBasicSubjects_Subject_3
		{ 0u, 0u },  // RcwBasicSubjects_RcwBasicSubjects_Subject_4
		{ 0u, 0u },  // RcwBasicSubjects_RcwBasicSubjects_Subject_5
		{ 0u, 0u },  // RcwBasicSubjects_RcwBasicSubjects_Subject_6
		{ 0u, 0u },  // RcwBasicSubjects_AutoGeneratedProgram__ctor
		{ 0u, 1u },  // RcwBasicSubjects_AutoGeneratedProgram_Main_System_String
		{ 0u, 0u },  // RcwBasicSubjects_ConstantFortyTwo__ctor
		{ 1u, 0u },  // RcwBasicSubjects_ConstantFortyTwo_GetValue
		{ 2u, 1u },  // RcwBasicSubjects_RcwBasicNativeEntry_CreateCcwForSimpleMath_System_IntPtr
		{ 1u, 4u },  // RcwBasicSubjects_RcwBasicNativeEntry_MarshalCallComMethod_System_IntPtr_System_Int32_System_Int32_System_Int32
		{ 1u, 4u },  // RcwBasicSubjects_RcwBasicNativeEntry_MarshalCallDirectComMethod_System_IntPtr_System_Int32_System_Int32_System_Int32
		{ 2u, 2u },  // RcwBasicSubjects_RcwBasicNativeEntry_MarshalCreateCcw_System_IntPtr_System_IntPtr
		{ 2u, 1u },  // RcwBasicSubjects_RcwBasicNativeEntry_MarshalCreateRcw_System_IntPtr
		{ 2u, 1u },  // RcwBasicSubjects_RcwBasicNativeEntry_MarshalGetRcwUnknown_System_IntPtr
		{ 2u, 2u },  // RcwBasicSubjects_RcwBasicNativeEntry_MarshalRcwQueryInterface_System_IntPtr_System_IntPtr
		{ 0u, 1u },  // RcwBasicSubjects_RcwBasicNativeEntry_MarshalReleaseRcw_System_IntPtr
		{ 1u, 1u },  // RcwBasicSubjects_RcwBasicNativeEntry_Run_System_Int32
		{ 1u, 0u },  // RcwBasicSubjects_RcwBasicNativeEntry_TestRcwDirectVtable
		{ 1u, 0u },  // RcwBasicSubjects_RcwBasicNativeEntry_TestRcwMultipleWrappers
		{ 1u, 0u },  // RcwBasicSubjects_RcwBasicNativeEntry_TestRcwQiUnknownInterface
		{ 1u, 0u },  // RcwBasicSubjects_RcwBasicNativeEntry_TestRcwRoundTripIdentity
		{ 1u, 0u },  // RcwBasicSubjects_RcwBasicNativeEntry_TestRcwRoundTripQi
		{ 1u, 0u },  // RcwBasicSubjects_RcwBasicNativeEntry_TestRcwVtableMethodCall
		{ 2u, 0u },  // RcwBasicSubjects_RuntimeState_Get
		{ 0u, 1u },  // RcwBasicSubjects_RuntimeState_Set_System_IntPtr
		{ 0u, 0u },  // RcwBasicSubjects_SimpleMath__ctor
		{ 1u, 2u },  // RcwBasicSubjects_SimpleMath_Add_System_Int32_System_Int32
		{ 1u, 2u },  // RcwBasicSubjects_SimpleMath_Multiply_System_Int32_System_Int32
	},
	{
		2u,
		2u,
		2u,
		1u,
		1u,
		1u,
		2u,
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
		1u,
		2u,
		1u,
		1u,
		1u,
		1u,
	},
};
static const ::ChaosAbiManifestV0* const s_abi_manifest =
	reinterpret_cast<const ::ChaosAbiManifestV0*>(&s_abi_manifest_storage);
// ── Module registration ──

	static constexpr CHAOS_IL2CPP_UINT32 s_type_flags[11] = {
		0u,
		1548u,
		1556u,
		1556u,
		1536u,
		1536u,
		1548u,
		12u,
		0u,
		8u,
		1545u,
	};

	static constexpr const char* s_type_names[11] = {
		"<Module>",
		"RcwBasicSubjects",
		"ISimpleMath",
		"IConstantValue",
		"SimpleMath",
		"ConstantFortyTwo",
		"RcwBasicNativeEntry",
		"RuntimeState",
		"AutoGeneratedProgram",
		"<PrivateImplementationDetails>",
		"__StaticArrayInitTypeSize=16",
	};

	static constexpr const char* s_type_namespaces[11] = {
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

	static constexpr CHAOS_IL2CPP_UINT32 s_type_parent_tokens[11] = {
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

	static const TypeInfoHot* const s_type_info_ptrs[11] = {
		nullptr,
		&chaos_mt_RcwBasicSubjects_RcwBasicSubjects.hot,
		nullptr,
		nullptr,
		&chaos_mt_RcwBasicSubjects_SimpleMath.hot,
		nullptr,
		nullptr,
		&chaos_mt_RcwBasicSubjects_RuntimeState.hot,
		nullptr,
		&chaos_mt_RcwBasicSubjects__PrivateImplementationDetails_.hot,
		nullptr,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_children[1] = {
		33554443u,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_offset[12] = {
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
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_generic_param_constraint_offset[12] = {
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
		/* .name_utf8         = */ "RcwBasicSubjects",
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
		/* .type_count        = */ 11u,
	/* .custom_attribute_blob       = */ nullptr,  // Tier 1 — deferred
	/* .custom_attribute_offset     = */ nullptr,
	/* .custom_attribute_entity_count = */ 0u,
	/* .custom_attribute_materializer = */ nullptr,

		/* .abi_manifest      = */ s_abi_manifest,
	};
	static const CHAOS_IL2CPP_UINT32 s_native_aot_module_id =
		::chaos::il2cpp::runtime_core::RegisterModule("RcwBasicSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[31] = {
	{ ".ctor", 0x0000002Fu, 0u },  // AutoGeneratedProgram
	{ "Main", 0x0000002Eu, 1u },  // AutoGeneratedProgram
	{ ".ctor", 0x0000001Cu, 0u },  // ConstantFortyTwo
	{ "GetValue", 0x0000001Bu, 0u },  // ConstantFortyTwo
	{ "CreateCcwForSimpleMath", 0x00000024u, 1u },  // RcwBasicNativeEntry
	{ "MarshalCallComMethod", 0x0000002Au, 4u },  // RcwBasicNativeEntry
	{ "MarshalCallDirectComMethod", 0x0000002Bu, 4u },  // RcwBasicNativeEntry
	{ "MarshalCreateCcw", 0x00000025u, 2u },  // RcwBasicNativeEntry
	{ "MarshalCreateRcw", 0x00000026u, 1u },  // RcwBasicNativeEntry
	{ "MarshalGetRcwUnknown", 0x00000028u, 1u },  // RcwBasicNativeEntry
	{ "MarshalRcwQueryInterface", 0x00000029u, 2u },  // RcwBasicNativeEntry
	{ "MarshalReleaseRcw", 0x00000027u, 1u },  // RcwBasicNativeEntry
	{ "Run", 0x0000001Du, 1u },  // RcwBasicNativeEntry
	{ "TestRcwDirectVtable", 0x00000021u, 0u },  // RcwBasicNativeEntry
	{ "TestRcwMultipleWrappers", 0x00000022u, 0u },  // RcwBasicNativeEntry
	{ "TestRcwQiUnknownInterface", 0x00000023u, 0u },  // RcwBasicNativeEntry
	{ "TestRcwRoundTripIdentity", 0x0000001Eu, 0u },  // RcwBasicNativeEntry
	{ "TestRcwRoundTripQi", 0x0000001Fu, 0u },  // RcwBasicNativeEntry
	{ "TestRcwVtableMethodCall", 0x00000020u, 0u },  // RcwBasicNativeEntry
	{ "Subject_0", 0x0000000Eu, 0u },  // RcwBasicSubjects
	{ "Subject_1", 0x0000000Fu, 0u },  // RcwBasicSubjects
	{ "Subject_2", 0x00000010u, 0u },  // RcwBasicSubjects
	{ "Subject_3", 0x00000011u, 0u },  // RcwBasicSubjects
	{ "Subject_4", 0x00000012u, 0u },  // RcwBasicSubjects
	{ "Subject_5", 0x00000013u, 0u },  // RcwBasicSubjects
	{ "Subject_6", 0x00000014u, 0u },  // RcwBasicSubjects
	{ "Get", 0x0000002Cu, 0u },  // RuntimeState
	{ "Set", 0x0000002Du, 1u },  // RuntimeState
	{ ".ctor", 0x0000001Au, 0u },  // SimpleMath
	{ "Add", 0x00000018u, 2u },  // SimpleMath
	{ "Multiply", 0x00000019u, 2u },  // SimpleMath
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[6] = {
	{ "AutoGeneratedProgram", "", 0u, 2u },
	{ "ConstantFortyTwo", "", 2u, 2u },
	{ "RcwBasicNativeEntry", "", 4u, 15u },
	{ "RcwBasicSubjects", "", 19u, 7u },
	{ "RuntimeState", "", 26u, 2u },
	{ "SimpleMath", "", 28u, 3u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[31] = {
	{ 0x0000000Eu, 0u },
	{ 0x0000000Fu, 1u },
	{ 0x00000010u, 2u },
	{ 0x00000011u, 3u },
	{ 0x00000012u, 4u },
	{ 0x00000013u, 5u },
	{ 0x00000014u, 6u },
	{ 0x00000018u, 29u },
	{ 0x00000019u, 30u },
	{ 0x0000001Au, 28u },
	{ 0x0000001Bu, 10u },
	{ 0x0000001Cu, 9u },
	{ 0x0000001Du, 19u },
	{ 0x0000001Eu, 23u },
	{ 0x0000001Fu, 24u },
	{ 0x00000020u, 25u },
	{ 0x00000021u, 20u },
	{ 0x00000022u, 21u },
	{ 0x00000023u, 22u },
	{ 0x00000024u, 11u },
	{ 0x00000025u, 14u },
	{ 0x00000026u, 15u },
	{ 0x00000027u, 18u },
	{ 0x00000028u, 16u },
	{ 0x00000029u, 17u },
	{ 0x0000002Au, 12u },
	{ 0x0000002Bu, 13u },
	{ 0x0000002Cu, 26u },
	{ 0x0000002Du, 27u },
	{ 0x0000002Eu, 8u },
	{ 0x0000002Fu, 7u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[31] = {
	{ reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicSubjects_Subject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // RcwBasicSubjects::Subject_0
	{ reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicSubjects_Subject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // RcwBasicSubjects::Subject_1
	{ reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicSubjects_Subject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // RcwBasicSubjects::Subject_2
	{ reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicSubjects_Subject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // RcwBasicSubjects::Subject_3
	{ reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicSubjects_Subject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // RcwBasicSubjects::Subject_4
	{ reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicSubjects_Subject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // RcwBasicSubjects::Subject_5
	{ reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicSubjects_Subject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // RcwBasicSubjects::Subject_6
	{ reinterpret_cast<void*>(&RcwBasicSubjects_AutoGeneratedProgram__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AutoGeneratedProgram::.ctor
	{ reinterpret_cast<void*>(&RcwBasicSubjects_AutoGeneratedProgram_Main_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGeneratedProgram::Main
	{ reinterpret_cast<void*>(&RcwBasicSubjects_ConstantFortyTwo__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ConstantFortyTwo::.ctor
	{ reinterpret_cast<void*>(&RcwBasicSubjects_ConstantFortyTwo_GetValue), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ConstantFortyTwo::GetValue
	{ reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicNativeEntry_CreateCcwForSimpleMath_System_IntPtr), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // RcwBasicNativeEntry::CreateCcwForSimpleMath
	{ reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicNativeEntry_MarshalCallComMethod_System_IntPtr_System_Int32_System_Int32_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // RcwBasicNativeEntry::MarshalCallComMethod
	{ reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicNativeEntry_MarshalCallDirectComMethod_System_IntPtr_System_Int32_System_Int32_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // RcwBasicNativeEntry::MarshalCallDirectComMethod
	{ reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicNativeEntry_MarshalCreateCcw_System_IntPtr_System_IntPtr), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // RcwBasicNativeEntry::MarshalCreateCcw
	{ reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicNativeEntry_MarshalCreateRcw_System_IntPtr), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // RcwBasicNativeEntry::MarshalCreateRcw
	{ reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicNativeEntry_MarshalGetRcwUnknown_System_IntPtr), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // RcwBasicNativeEntry::MarshalGetRcwUnknown
	{ reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicNativeEntry_MarshalRcwQueryInterface_System_IntPtr_System_IntPtr), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // RcwBasicNativeEntry::MarshalRcwQueryInterface
	{ reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicNativeEntry_MarshalReleaseRcw_System_IntPtr), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // RcwBasicNativeEntry::MarshalReleaseRcw
	{ reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicNativeEntry_Run_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // RcwBasicNativeEntry::Run
	{ reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicNativeEntry_TestRcwDirectVtable), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // RcwBasicNativeEntry::TestRcwDirectVtable
	{ reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicNativeEntry_TestRcwMultipleWrappers), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // RcwBasicNativeEntry::TestRcwMultipleWrappers
	{ reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicNativeEntry_TestRcwQiUnknownInterface), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // RcwBasicNativeEntry::TestRcwQiUnknownInterface
	{ reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicNativeEntry_TestRcwRoundTripIdentity), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // RcwBasicNativeEntry::TestRcwRoundTripIdentity
	{ reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicNativeEntry_TestRcwRoundTripQi), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // RcwBasicNativeEntry::TestRcwRoundTripQi
	{ reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicNativeEntry_TestRcwVtableMethodCall), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // RcwBasicNativeEntry::TestRcwVtableMethodCall
	{ reinterpret_cast<void*>(&RcwBasicSubjects_RuntimeState_Get), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // RuntimeState::Get
	{ reinterpret_cast<void*>(&RcwBasicSubjects_RuntimeState_Set_System_IntPtr), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // RuntimeState::Set
	{ reinterpret_cast<void*>(&RcwBasicSubjects_SimpleMath__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // SimpleMath::.ctor
	{ reinterpret_cast<void*>(&RcwBasicSubjects_SimpleMath_Add_System_Int32_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // SimpleMath::Add
	{ reinterpret_cast<void*>(&RcwBasicSubjects_SimpleMath_Multiply_System_Int32_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // SimpleMath::Multiply
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"RcwBasicSubjects",
	s_hotpatch_types,
	6u,
	s_hotpatch_methods,
	31u,
	s_hotpatch_slots,
	31u,
	s_hotpatch_entries,
	31u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[14] = {
	"RcwBasicSubjects/RcwBasicSubjects::_exitCode",
	"System.Private.CoreLib/System.Object::.ctor:System.Void()",
	"System.Private.CoreLib/System.Runtime.InteropServices.GCHandle::Alloc:System.Runtime.InteropServices.GCHandle(System.Object)",
	"System.Private.CoreLib/System.Runtime.InteropServices.GCHandle::ToIntPtr:System.IntPtr(System.Runtime.InteropServices.GCHandle)",
	"System.Private.CoreLib/System.IntPtr::Zero",
	"System.Private.CoreLib/System.Runtime.InteropServices.GCHandle::FromIntPtr:System.Runtime.InteropServices.GCHandle(System.IntPtr)",
	"System.Private.CoreLib/System.Runtime.InteropServices.GCHandle::Free:System.Void()",
	"System.Private.CoreLib/System.Byte",
	"RcwBasicSubjects/<PrivateImplementationDetails>::DA5979D52E989A03770DCD85298CC184B885A2D2AF51AEB79151BB6630C1D46E",
	"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)",
	"System.Runtime.InteropServices/Marshal::AllocHGlobal:System.IntPtr(System.Int32)",
	"System.Runtime.InteropServices/Marshal::Copy:System.Void(System.Byte[],System.Int32,System.IntPtr,System.Int32)",
	"System.Runtime.InteropServices/Marshal::FreeHGlobal:System.Void(System.IntPtr)",
	"RcwBasicSubjects/RuntimeState::_state",
};

extern "C" void* kChaosExternalRuntimeFnTable[14] = {
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__),
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers__InitializeArray_System_Void_System_Array_System_RuntimeFieldHandle_),
	nullptr,
	nullptr,
	nullptr,
	nullptr,
};

extern "C" int32_t kChaosExternalRuntimeCount = 14;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[31])() = {
	reinterpret_cast<void(*)()>(&RcwBasicSubjects_RcwBasicSubjects_Subject_0),
	reinterpret_cast<void(*)()>(&RcwBasicSubjects_RcwBasicSubjects_Subject_1),
	reinterpret_cast<void(*)()>(&RcwBasicSubjects_RcwBasicSubjects_Subject_2),
	reinterpret_cast<void(*)()>(&RcwBasicSubjects_RcwBasicSubjects_Subject_3),
	reinterpret_cast<void(*)()>(&RcwBasicSubjects_RcwBasicSubjects_Subject_4),
	reinterpret_cast<void(*)()>(&RcwBasicSubjects_RcwBasicSubjects_Subject_5),
	reinterpret_cast<void(*)()>(&RcwBasicSubjects_RcwBasicSubjects_Subject_6),
	reinterpret_cast<void(*)()>(&RcwBasicSubjects_AutoGeneratedProgram__ctor),
	reinterpret_cast<void(*)()>(&RcwBasicSubjects_AutoGeneratedProgram_Main_System_String),
	reinterpret_cast<void(*)()>(&RcwBasicSubjects_ConstantFortyTwo__ctor),
	reinterpret_cast<void(*)()>(&RcwBasicSubjects_ConstantFortyTwo_GetValue),
	reinterpret_cast<void(*)()>(&RcwBasicSubjects_RcwBasicNativeEntry_CreateCcwForSimpleMath_System_IntPtr),
	reinterpret_cast<void(*)()>(&RcwBasicSubjects_RcwBasicNativeEntry_MarshalCallComMethod_System_IntPtr_System_Int32_System_Int32_System_Int32),
	reinterpret_cast<void(*)()>(&RcwBasicSubjects_RcwBasicNativeEntry_MarshalCallDirectComMethod_System_IntPtr_System_Int32_System_Int32_System_Int32),
	reinterpret_cast<void(*)()>(&RcwBasicSubjects_RcwBasicNativeEntry_MarshalCreateCcw_System_IntPtr_System_IntPtr),
	reinterpret_cast<void(*)()>(&RcwBasicSubjects_RcwBasicNativeEntry_MarshalCreateRcw_System_IntPtr),
	reinterpret_cast<void(*)()>(&RcwBasicSubjects_RcwBasicNativeEntry_MarshalGetRcwUnknown_System_IntPtr),
	reinterpret_cast<void(*)()>(&RcwBasicSubjects_RcwBasicNativeEntry_MarshalRcwQueryInterface_System_IntPtr_System_IntPtr),
	reinterpret_cast<void(*)()>(&RcwBasicSubjects_RcwBasicNativeEntry_MarshalReleaseRcw_System_IntPtr),
	reinterpret_cast<void(*)()>(&RcwBasicSubjects_RcwBasicNativeEntry_Run_System_Int32),
	reinterpret_cast<void(*)()>(&RcwBasicSubjects_RcwBasicNativeEntry_TestRcwDirectVtable),
	reinterpret_cast<void(*)()>(&RcwBasicSubjects_RcwBasicNativeEntry_TestRcwMultipleWrappers),
	reinterpret_cast<void(*)()>(&RcwBasicSubjects_RcwBasicNativeEntry_TestRcwQiUnknownInterface),
	reinterpret_cast<void(*)()>(&RcwBasicSubjects_RcwBasicNativeEntry_TestRcwRoundTripIdentity),
	reinterpret_cast<void(*)()>(&RcwBasicSubjects_RcwBasicNativeEntry_TestRcwRoundTripQi),
	reinterpret_cast<void(*)()>(&RcwBasicSubjects_RcwBasicNativeEntry_TestRcwVtableMethodCall),
	reinterpret_cast<void(*)()>(&RcwBasicSubjects_RuntimeState_Get),
	reinterpret_cast<void(*)()>(&RcwBasicSubjects_RuntimeState_Set_System_IntPtr),
	reinterpret_cast<void(*)()>(&RcwBasicSubjects_SimpleMath__ctor),
	reinterpret_cast<void(*)()>(&RcwBasicSubjects_SimpleMath_Add_System_Int32_System_Int32),
	reinterpret_cast<void(*)()>(&RcwBasicSubjects_SimpleMath_Multiply_System_Int32_System_Int32),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
extern "C" void (*kBenchmarkWrappers[31])() = {
	[]() {kAotMethods[0]();},
	[]() {kAotMethods[1]();},
	[]() {kAotMethods[2]();},
	[]() {kAotMethods[3]();},
	[]() {kAotMethods[4]();},
	[]() {kAotMethods[5]();},
	[]() {kAotMethods[6]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[7])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[8])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[9])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[10])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[11])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[12])(0,0,0,0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[13])(0,0,0,0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[14])(0,0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[15])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[16])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[17])(0,0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[18])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[19])(0);},
	[]() {kAotMethods[20]();},
	[]() {kAotMethods[21]();},
	[]() {kAotMethods[22]();},
	[]() {kAotMethods[23]();},
	[]() {kAotMethods[24]();},
	[]() {kAotMethods[25]();},
	[]() {kAotMethods[26]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[27])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[28])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[29])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0,0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[30])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0,0);},
};

// ── Subject entry index mapping ─────────────────────────────────
// Maps subject index (0-based sequential) to kAotMethod index.
// Used by runtime-entry.cpp to route --benchmark N to the correct
// AOT method slot, since kAotMethods[] includes lambdas/closures
// that shift subject methods to non-contiguous indices.
extern "C" const int kSubjectEntryCount = 7;
extern "C" const int kSubjectEntryIndices[7] = {
	0,
	1,
	2,
	3,
	4,
	5,
	6,
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

	/* ── Entry 0: RcwBasicSubjects_RcwBasicSubjects_Subject_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry0;
	/* ── Entry 1: RcwBasicSubjects_RcwBasicSubjects_Subject_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry1;
	/* ── Entry 2: RcwBasicSubjects_RcwBasicSubjects_Subject_2 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry2;
	/* ── Entry 3: RcwBasicSubjects_RcwBasicSubjects_Subject_3 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry3;
	/* ── Entry 4: RcwBasicSubjects_RcwBasicSubjects_Subject_4 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry4;
	/* ── Entry 5: RcwBasicSubjects_RcwBasicSubjects_Subject_5 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry5;
	/* ── Entry 6: RcwBasicSubjects_RcwBasicSubjects_Subject_6 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry6;
	/* ── Entry 7: RcwBasicSubjects_AutoGeneratedProgram__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry7;
	/* ── Entry 8: RcwBasicSubjects_AutoGeneratedProgram_Main_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry8;
	/* ── Entry 9: RcwBasicSubjects_ConstantFortyTwo__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry9;
	/* ── Entry 10: RcwBasicSubjects_ConstantFortyTwo_GetValue ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry10;
	/* ── Entry 11: RcwBasicSubjects_RcwBasicNativeEntry_CreateCcwForSimpleMath_System_IntPtr ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[8];
	} entry11;
	/* ── Entry 12: RcwBasicSubjects_RcwBasicNativeEntry_Run_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry12;
	/* ── Entry 13: RcwBasicSubjects_RcwBasicNativeEntry_TestRcwDirectVtable ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[11];
	} entry13;
	/* ── Entry 14: RcwBasicSubjects_RcwBasicNativeEntry_TestRcwMultipleWrappers ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry14;
	/* ── Entry 15: RcwBasicSubjects_RcwBasicNativeEntry_TestRcwQiUnknownInterface ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry15;
	/* ── Entry 16: RcwBasicSubjects_RcwBasicNativeEntry_TestRcwRoundTripIdentity ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[12];
	} entry16;
	/* ── Entry 17: RcwBasicSubjects_RcwBasicNativeEntry_TestRcwRoundTripQi ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[18];
	} entry17;
	/* ── Entry 18: RcwBasicSubjects_RcwBasicNativeEntry_TestRcwVtableMethodCall ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[13];
	} entry18;
	/* ── Entry 19: RcwBasicSubjects_RuntimeState_Get ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry19;
	/* ── Entry 20: RcwBasicSubjects_RuntimeState_Set_System_IntPtr ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry20;
	/* ── Entry 21: RcwBasicSubjects_SimpleMath__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry21;
	/* ── Entry 22: RcwBasicSubjects_SimpleMath_Add_System_Int32_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry22;
	/* ── Entry 23: RcwBasicSubjects_SimpleMath_Multiply_System_Int32_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry23;
} kChaosGcSlotMapsSection = {
	/* entry0 = RcwBasicSubjects_RcwBasicSubjects_Subject_0 */
	.entry0 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&RcwBasicSubjects_RcwBasicSubjects_Subject_0),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry1 = RcwBasicSubjects_RcwBasicSubjects_Subject_1 */
	.entry1 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&RcwBasicSubjects_RcwBasicSubjects_Subject_1),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry2 = RcwBasicSubjects_RcwBasicSubjects_Subject_2 */
	.entry2 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&RcwBasicSubjects_RcwBasicSubjects_Subject_2),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry3 = RcwBasicSubjects_RcwBasicSubjects_Subject_3 */
	.entry3 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&RcwBasicSubjects_RcwBasicSubjects_Subject_3),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry4 = RcwBasicSubjects_RcwBasicSubjects_Subject_4 */
	.entry4 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&RcwBasicSubjects_RcwBasicSubjects_Subject_4),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry5 = RcwBasicSubjects_RcwBasicSubjects_Subject_5 */
	.entry5 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&RcwBasicSubjects_RcwBasicSubjects_Subject_5),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry6 = RcwBasicSubjects_RcwBasicSubjects_Subject_6 */
	.entry6 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&RcwBasicSubjects_RcwBasicSubjects_Subject_6),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry7 = RcwBasicSubjects_AutoGeneratedProgram__ctor */
	.entry7 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&RcwBasicSubjects_AutoGeneratedProgram__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry8 = RcwBasicSubjects_AutoGeneratedProgram_Main_System_String */
	.entry8 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&RcwBasicSubjects_AutoGeneratedProgram_Main_System_String),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry9 = RcwBasicSubjects_ConstantFortyTwo__ctor */
	.entry9 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&RcwBasicSubjects_ConstantFortyTwo__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry10 = RcwBasicSubjects_ConstantFortyTwo_GetValue */
	.entry10 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&RcwBasicSubjects_ConstantFortyTwo_GetValue),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry11 = RcwBasicSubjects_RcwBasicNativeEntry_CreateCcwForSimpleMath_System_IntPtr */
	.entry11 = {
		/* entry_total_size = 52 */ 52u,
		/* code_address */ reinterpret_cast<const void*>(&RcwBasicSubjects_RcwBasicNativeEntry_CreateCcwForSimpleMath_System_IntPtr),
		/* frame_size = 72 */ 72u,
		/* num_gc_slots = 8 */ 8u,
		/* slots */ { 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u }
	},
	/* entry12 = RcwBasicSubjects_RcwBasicNativeEntry_Run_System_Int32 */
	.entry12 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&RcwBasicSubjects_RcwBasicNativeEntry_Run_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 8u, 16u, 24u, 32u }
	},
	/* entry13 = RcwBasicSubjects_RcwBasicNativeEntry_TestRcwDirectVtable */
	.entry13 = {
		/* entry_total_size = 64 */ 64u,
		/* code_address */ reinterpret_cast<const void*>(&RcwBasicSubjects_RcwBasicNativeEntry_TestRcwDirectVtable),
		/* frame_size = 88 */ 88u,
		/* num_gc_slots = 11 */ 11u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u, 72u, 80u }
	},
	/* entry14 = RcwBasicSubjects_RcwBasicNativeEntry_TestRcwMultipleWrappers */
	.entry14 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&RcwBasicSubjects_RcwBasicNativeEntry_TestRcwMultipleWrappers),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry15 = RcwBasicSubjects_RcwBasicNativeEntry_TestRcwQiUnknownInterface */
	.entry15 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&RcwBasicSubjects_RcwBasicNativeEntry_TestRcwQiUnknownInterface),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry16 = RcwBasicSubjects_RcwBasicNativeEntry_TestRcwRoundTripIdentity */
	.entry16 = {
		/* entry_total_size = 68 */ 68u,
		/* code_address */ reinterpret_cast<const void*>(&RcwBasicSubjects_RcwBasicNativeEntry_TestRcwRoundTripIdentity),
		/* frame_size = 96 */ 96u,
		/* num_gc_slots = 12 */ 12u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u, 72u, 80u, 88u }
	},
	/* entry17 = RcwBasicSubjects_RcwBasicNativeEntry_TestRcwRoundTripQi */
	.entry17 = {
		/* entry_total_size = 92 */ 92u,
		/* code_address */ reinterpret_cast<const void*>(&RcwBasicSubjects_RcwBasicNativeEntry_TestRcwRoundTripQi),
		/* frame_size = 144 */ 144u,
		/* num_gc_slots = 18 */ 18u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u, 72u, 80u, 88u, 96u, 104u, 112u, 120u, 128u, 136u }
	},
	/* entry18 = RcwBasicSubjects_RcwBasicNativeEntry_TestRcwVtableMethodCall */
	.entry18 = {
		/* entry_total_size = 72 */ 72u,
		/* code_address */ reinterpret_cast<const void*>(&RcwBasicSubjects_RcwBasicNativeEntry_TestRcwVtableMethodCall),
		/* frame_size = 104 */ 104u,
		/* num_gc_slots = 13 */ 13u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u, 72u, 80u, 88u, 96u }
	},
	/* entry19 = RcwBasicSubjects_RuntimeState_Get */
	.entry19 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&RcwBasicSubjects_RuntimeState_Get),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry20 = RcwBasicSubjects_RuntimeState_Set_System_IntPtr */
	.entry20 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&RcwBasicSubjects_RuntimeState_Set_System_IntPtr),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 8u }
	},
	/* entry21 = RcwBasicSubjects_SimpleMath__ctor */
	.entry21 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&RcwBasicSubjects_SimpleMath__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry22 = RcwBasicSubjects_SimpleMath_Add_System_Int32_System_Int32 */
	.entry22 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&RcwBasicSubjects_SimpleMath_Add_System_Int32_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 24u, 32u }
	},
	/* entry23 = RcwBasicSubjects_SimpleMath_Multiply_System_Int32_System_Int32 */
	.entry23 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&RcwBasicSubjects_SimpleMath_Multiply_System_Int32_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 24u, 32u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 912u;

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[31] = {
	reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicSubjects_Subject_0),
	reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicSubjects_Subject_1),
	reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicSubjects_Subject_2),
	reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicSubjects_Subject_3),
	reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicSubjects_Subject_4),
	reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicSubjects_Subject_5),
	reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicSubjects_Subject_6),
	reinterpret_cast<void*>(&RcwBasicSubjects_AutoGeneratedProgram__ctor),
	reinterpret_cast<void*>(&RcwBasicSubjects_AutoGeneratedProgram_Main_System_String),
	reinterpret_cast<void*>(&RcwBasicSubjects_ConstantFortyTwo__ctor),
	reinterpret_cast<void*>(&RcwBasicSubjects_ConstantFortyTwo_GetValue),
	reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicNativeEntry_CreateCcwForSimpleMath_System_IntPtr),
	reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicNativeEntry_MarshalCallComMethod_System_IntPtr_System_Int32_System_Int32_System_Int32),
	reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicNativeEntry_MarshalCallDirectComMethod_System_IntPtr_System_Int32_System_Int32_System_Int32),
	reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicNativeEntry_MarshalCreateCcw_System_IntPtr_System_IntPtr),
	reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicNativeEntry_MarshalCreateRcw_System_IntPtr),
	reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicNativeEntry_MarshalGetRcwUnknown_System_IntPtr),
	reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicNativeEntry_MarshalRcwQueryInterface_System_IntPtr_System_IntPtr),
	reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicNativeEntry_MarshalReleaseRcw_System_IntPtr),
	reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicNativeEntry_Run_System_Int32),
	reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicNativeEntry_TestRcwDirectVtable),
	reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicNativeEntry_TestRcwMultipleWrappers),
	reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicNativeEntry_TestRcwQiUnknownInterface),
	reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicNativeEntry_TestRcwRoundTripIdentity),
	reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicNativeEntry_TestRcwRoundTripQi),
	reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicNativeEntry_TestRcwVtableMethodCall),
	reinterpret_cast<void*>(&RcwBasicSubjects_RuntimeState_Get),
	reinterpret_cast<void*>(&RcwBasicSubjects_RuntimeState_Set_System_IntPtr),
	reinterpret_cast<void*>(&RcwBasicSubjects_SimpleMath__ctor),
	reinterpret_cast<void*>(&RcwBasicSubjects_SimpleMath_Add_System_Int32_System_Int32),
	reinterpret_cast<void*>(&RcwBasicSubjects_SimpleMath_Multiply_System_Int32_System_Int32),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
// ── VTable descriptors (for BootstrapRuntime TypeVTable registration) ──
static const VTableDescriptorV0 kChaosVTableDescriptors[] = {
	{
		CHAOS_IL2CPP_UINT64_C(0x4E9369B22D576F97),
		0x00000004u,
		0u,
		3u,
		reinterpret_cast<const void*>(kSlots_RcwBasicSubjects_SimpleMath),
		reinterpret_cast<const void**>(chaos_vtable_RcwBasicSubjects_SimpleMath),
		3u,
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
	.method_pointer_count      = 31u,
	.reverse_pinvoke_wrappers  = nullptr,
	.reverse_pinvoke_wrapper_count = 0u,
	.invoker_pointers          = nullptr,
	.invoker_pointer_count     = 0u,
	.unresolved_virtual_calls = nullptr,
	.unresolved_virtual_call_count = 0u,
	.type_capabilities       = nullptr,
	.type_capability_count   = 0u,
	.vtable_descriptors = kChaosVTableDescriptors,
	.vtable_descriptor_count = 1u,
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
	.image_name_utf8    = "RcwBasicSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_RcwBasicSubjects_RcwBasicSubjects[1] = {
	{ 0u, "RcwBasicSubjects/RcwBasicSubjects::_exitCode", "_exitCode", "System.Int32", 0LL },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_RcwBasicSubjects_RcwBasicSubjects[7] = {
	{ 0u, "RcwBasicSubjects/RcwBasicSubjects::Subject_0:System.Void()", "Subject_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "RcwBasicSubjects/RcwBasicSubjects::Subject_1:System.Void()", "Subject_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "RcwBasicSubjects/RcwBasicSubjects::Subject_2:System.Void()", "Subject_2", "System.Void", 0, nullptr, 0u },
	{ 0u, "RcwBasicSubjects/RcwBasicSubjects::Subject_3:System.Void()", "Subject_3", "System.Void", 0, nullptr, 0u },
	{ 0u, "RcwBasicSubjects/RcwBasicSubjects::Subject_4:System.Void()", "Subject_4", "System.Void", 0, nullptr, 0u },
	{ 0u, "RcwBasicSubjects/RcwBasicSubjects::Subject_5:System.Void()", "Subject_5", "System.Void", 0, nullptr, 0u },
	{ 0u, "RcwBasicSubjects/RcwBasicSubjects::Subject_6:System.Void()", "Subject_6", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_RcwBasicSubjects_AutoGeneratedProgram[2] = {
	{ 0u, "RcwBasicSubjects/AutoGeneratedProgram::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "RcwBasicSubjects/AutoGeneratedProgram::Main:System.Void(System.String[])", "Main", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_RcwBasicSubjects_ConstantFortyTwo[2] = {
	{ 0u, "RcwBasicSubjects/ConstantFortyTwo::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "RcwBasicSubjects/ConstantFortyTwo::GetValue:System.Int32()", "GetValue", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_RcwBasicSubjects_RcwBasicNativeEntry[15] = {
	{ 0u, "RcwBasicSubjects/RcwBasicNativeEntry::CreateCcwForSimpleMath:System.IntPtr(System.IntPtr)", "CreateCcwForSimpleMath", "System.Void", 0, nullptr, 0u },
	{ 0u, "RcwBasicSubjects/RcwBasicNativeEntry::MarshalCallComMethod:System.Int32(System.IntPtr,System.Int32,System.Int32,System.Int32)", "MarshalCallComMethod", "System.Void", 0, nullptr, 0u },
	{ 0u, "RcwBasicSubjects/RcwBasicNativeEntry::MarshalCallDirectComMethod:System.Int32(System.IntPtr,System.Int32,System.Int32,System.Int32)", "MarshalCallDirectComMethod", "System.Void", 0, nullptr, 0u },
	{ 0u, "RcwBasicSubjects/RcwBasicNativeEntry::MarshalCreateCcw:System.IntPtr(System.IntPtr,System.IntPtr)", "MarshalCreateCcw", "System.Void", 0, nullptr, 0u },
	{ 0u, "RcwBasicSubjects/RcwBasicNativeEntry::MarshalCreateRcw:System.IntPtr(System.IntPtr)", "MarshalCreateRcw", "System.Void", 0, nullptr, 0u },
	{ 0u, "RcwBasicSubjects/RcwBasicNativeEntry::MarshalGetRcwUnknown:System.IntPtr(System.IntPtr)", "MarshalGetRcwUnknown", "System.Void", 0, nullptr, 0u },
	{ 0u, "RcwBasicSubjects/RcwBasicNativeEntry::MarshalRcwQueryInterface:System.IntPtr(System.IntPtr,System.IntPtr)", "MarshalRcwQueryInterface", "System.Void", 0, nullptr, 0u },
	{ 0u, "RcwBasicSubjects/RcwBasicNativeEntry::MarshalReleaseRcw:System.Void(System.IntPtr)", "MarshalReleaseRcw", "System.Void", 0, nullptr, 0u },
	{ 0u, "RcwBasicSubjects/RcwBasicNativeEntry::Run:System.Int32(System.Int32)", "Run", "System.Void", 0, nullptr, 0u },
	{ 0u, "RcwBasicSubjects/RcwBasicNativeEntry::TestRcwDirectVtable:System.Int32()", "TestRcwDirectVtable", "System.Void", 0, nullptr, 0u },
	{ 0u, "RcwBasicSubjects/RcwBasicNativeEntry::TestRcwMultipleWrappers:System.Int32()", "TestRcwMultipleWrappers", "System.Void", 0, nullptr, 0u },
	{ 0u, "RcwBasicSubjects/RcwBasicNativeEntry::TestRcwQiUnknownInterface:System.Int32()", "TestRcwQiUnknownInterface", "System.Void", 0, nullptr, 0u },
	{ 0u, "RcwBasicSubjects/RcwBasicNativeEntry::TestRcwRoundTripIdentity:System.Int32()", "TestRcwRoundTripIdentity", "System.Void", 0, nullptr, 0u },
	{ 0u, "RcwBasicSubjects/RcwBasicNativeEntry::TestRcwRoundTripQi:System.Int32()", "TestRcwRoundTripQi", "System.Void", 0, nullptr, 0u },
	{ 0u, "RcwBasicSubjects/RcwBasicNativeEntry::TestRcwVtableMethodCall:System.Int32()", "TestRcwVtableMethodCall", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryFieldDescriptor kReflFields_RcwBasicSubjects_RuntimeState[1] = {
	{ 0u, "RcwBasicSubjects/RuntimeState::_state", "_state", "System.Int32", 0LL },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_RcwBasicSubjects_RuntimeState[2] = {
	{ 0u, "RcwBasicSubjects/RuntimeState::Get:System.IntPtr()", "Get", "System.Void", 0, nullptr, 0u },
	{ 0u, "RcwBasicSubjects/RuntimeState::Set:System.Void(System.IntPtr)", "Set", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_RcwBasicSubjects_SimpleMath[3] = {
	{ 0u, "RcwBasicSubjects/SimpleMath::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "RcwBasicSubjects/SimpleMath::Add:System.Int32(System.Int32,System.Int32)", "Add", "System.Void", 0, nullptr, 0u },
	{ 0u, "RcwBasicSubjects/SimpleMath::Multiply:System.Int32(System.Int32,System.Int32)", "Multiply", "System.Void", 0, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[6] = {
	{ 0u, "RcwBasicSubjects/RcwBasicSubjects", "RcwBasicSubjects/RcwBasicSubjects", "", "RcwBasicSubjects", "RcwBasicSubjects", nullptr, kReflFields_RcwBasicSubjects_RcwBasicSubjects, 1u, nullptr, 0u,
	kReflMethods_RcwBasicSubjects_RcwBasicSubjects, 7u },
	{ 0u, "RcwBasicSubjects/AutoGeneratedProgram", "RcwBasicSubjects/AutoGeneratedProgram", "", "AutoGeneratedProgram", "AutoGeneratedProgram", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_RcwBasicSubjects_AutoGeneratedProgram, 2u },
	{ 0u, "RcwBasicSubjects/ConstantFortyTwo", "RcwBasicSubjects/ConstantFortyTwo", "", "ConstantFortyTwo", "ConstantFortyTwo", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_RcwBasicSubjects_ConstantFortyTwo, 2u },
	{ 0u, "RcwBasicSubjects/RcwBasicNativeEntry", "RcwBasicSubjects/RcwBasicNativeEntry", "", "RcwBasicNativeEntry", "RcwBasicNativeEntry", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_RcwBasicSubjects_RcwBasicNativeEntry, 15u },
	{ 0u, "RcwBasicSubjects/RuntimeState", "RcwBasicSubjects/RuntimeState", "", "RuntimeState", "RuntimeState", nullptr, kReflFields_RcwBasicSubjects_RuntimeState, 1u, nullptr, 0u,
	kReflMethods_RcwBasicSubjects_RuntimeState, 2u },
	{ 0u, "RcwBasicSubjects/SimpleMath", "RcwBasicSubjects/SimpleMath", "", "SimpleMath", "SimpleMath", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_RcwBasicSubjects_SimpleMath, 3u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[6] = {
	&kReflTypes[0],
	&kReflTypes[1],
	&kReflTypes[2],
	&kReflTypes[3],
	&kReflTypes[4],
	&kReflTypes[5],
};

static const ReflectionQueryImageDescriptor kReflImage = { "RcwBasicSubjects", kReflTypePtrs, 6u };

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
// Managed method: RcwBasicSubjects/RcwBasicSubjects::Subject_0()
extern "C" void RcwBasicSubjects_RcwBasicSubjects_Subject_0(void)
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
	CHAOS_IL2CPP_INTPTR _s10{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_0 = _s0;
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
					_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_Run_System_Int32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
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
					_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_Run_System_Int32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
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
				chaos_static_RcwBasicSubjects_RcwBasicSubjects___exitCode = chaos_value;
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
				chaos_static_RcwBasicSubjects_RcwBasicSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: RcwBasicSubjects/RcwBasicSubjects::Subject_1()
extern "C" void RcwBasicSubjects_RcwBasicSubjects_Subject_1(void)
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
				auto& _d23 = s_hotpatch_entries[23];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d23)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d23))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d23.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_TestRcwRoundTripIdentity();
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
			{
				auto& _d23 = s_hotpatch_entries[23];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d23)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d23))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d23.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_TestRcwRoundTripIdentity();
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
				chaos_static_RcwBasicSubjects_RcwBasicSubjects___exitCode = chaos_value;
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
				chaos_static_RcwBasicSubjects_RcwBasicSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: RcwBasicSubjects/RcwBasicSubjects::Subject_2()
extern "C" void RcwBasicSubjects_RcwBasicSubjects_Subject_2(void)
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
				auto& _d24 = s_hotpatch_entries[24];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d24)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d24))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d24.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_TestRcwRoundTripQi();
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
			{
				auto& _d24 = s_hotpatch_entries[24];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d24)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d24))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d24.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_TestRcwRoundTripQi();
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
				chaos_static_RcwBasicSubjects_RcwBasicSubjects___exitCode = chaos_value;
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
				chaos_static_RcwBasicSubjects_RcwBasicSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: RcwBasicSubjects/RcwBasicSubjects::Subject_3()
extern "C" void RcwBasicSubjects_RcwBasicSubjects_Subject_3(void)
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
				auto& _d21 = s_hotpatch_entries[21];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d21)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d21))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d21.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_TestRcwMultipleWrappers();
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
			{
				auto& _d21 = s_hotpatch_entries[21];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d21)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d21))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d21.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_TestRcwMultipleWrappers();
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
				chaos_static_RcwBasicSubjects_RcwBasicSubjects___exitCode = chaos_value;
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
				chaos_static_RcwBasicSubjects_RcwBasicSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: RcwBasicSubjects/RcwBasicSubjects::Subject_4()
extern "C" void RcwBasicSubjects_RcwBasicSubjects_Subject_4(void)
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
				auto& _d22 = s_hotpatch_entries[22];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d22)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d22))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d22.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_TestRcwQiUnknownInterface();
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
			{
				auto& _d22 = s_hotpatch_entries[22];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d22)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d22))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d22.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_TestRcwQiUnknownInterface();
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
				chaos_static_RcwBasicSubjects_RcwBasicSubjects___exitCode = chaos_value;
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
				chaos_static_RcwBasicSubjects_RcwBasicSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: RcwBasicSubjects/RcwBasicSubjects::Subject_5()
extern "C" void RcwBasicSubjects_RcwBasicSubjects_Subject_5(void)
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
				auto& _d25 = s_hotpatch_entries[25];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d25)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d25))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d25.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_TestRcwVtableMethodCall();
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
			{
				auto& _d25 = s_hotpatch_entries[25];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d25)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d25))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d25.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_TestRcwVtableMethodCall();
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
				chaos_static_RcwBasicSubjects_RcwBasicSubjects___exitCode = chaos_value;
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
				chaos_static_RcwBasicSubjects_RcwBasicSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: RcwBasicSubjects/RcwBasicSubjects::Subject_6()
extern "C" void RcwBasicSubjects_RcwBasicSubjects_Subject_6(void)
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
				auto& _d20 = s_hotpatch_entries[20];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d20)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d20))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d20.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_TestRcwDirectVtable();
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
			{
				auto& _d20 = s_hotpatch_entries[20];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d20)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d20))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d20.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_TestRcwDirectVtable();
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
				chaos_static_RcwBasicSubjects_RcwBasicSubjects___exitCode = chaos_value;
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
				chaos_static_RcwBasicSubjects_RcwBasicSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: RcwBasicSubjects/AutoGeneratedProgram::.ctor()
extern "C" void RcwBasicSubjects_AutoGeneratedProgram__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: RcwBasicSubjects/AutoGeneratedProgram::Main(System.String[])
extern "C" void RcwBasicSubjects_AutoGeneratedProgram_Main_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	return;
}

// Managed method: RcwBasicSubjects/ConstantFortyTwo::.ctor()
extern "C" void RcwBasicSubjects_ConstantFortyTwo__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: RcwBasicSubjects/ConstantFortyTwo::GetValue()
extern "C" CHAOS_IL2CPP_INT32 RcwBasicSubjects_ConstantFortyTwo_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: RcwBasicSubjects/RcwBasicNativeEntry::CreateCcwForSimpleMath(System.IntPtr)
extern "C" CHAOS_IL2CPP_INTPTR RcwBasicSubjects_RcwBasicNativeEntry_CreateCcwForSimpleMath_System_IntPtr(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_RcwBasicSubjects_SimpleMath, {});
		chaos_object->header.type_info = &chaos_mt_RcwBasicSubjects_SimpleMath.hot;
		RcwBasicSubjects_SimpleMath__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[2])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[3])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	_s1 = chaos_args[0];
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		auto& _d14 = s_hotpatch_entries[14];
		CHAOS_IL2CPP_INTPTR _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d14)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d14))
		{
			alignas(16) uint8_t _d_ab[16];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
			_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_1));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d14.method_key, _d_ab, &_d_hpresult);
		}
		else
		{
			_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_MarshalCreateCcw_System_IntPtr_System_IntPtr(chaos_arg_0, chaos_arg_1);
		}
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	chaos_locals[2] = _s0;
	_s0 = chaos_locals[2];
	_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 == 0)
		{
			_s0 = chaos_locals[2];
			chaos_locals[5] = _s0;
		}
		else
		{
			_s6 = chaos_locals[1];
			{
				const auto chaos_arg_0 = _s6;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[5])(chaos_arg_0);
				_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[4] = _s6;
			_s6 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[4]);
			{
				reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[6])();
			}
			_s7 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			chaos_locals[5] = _s7;
		}
		_s6 = chaos_locals[5];
		return _s6;
	}
}

// AOT-unreachable stub: RcwBasicSubjects/RcwBasicNativeEntry::MarshalCallComMethod:System.Int32(System.IntPtr,System.Int32,System.Int32,System.Int32)
extern "C" CHAOS_IL2CPP_INT32 RcwBasicSubjects_RcwBasicNativeEntry_MarshalCallComMethod_System_IntPtr_System_Int32_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2, CHAOS_IL2CPP_INT32 chaos_fn_arg_3)
{
	return {};
}

// AOT-unreachable stub: RcwBasicSubjects/RcwBasicNativeEntry::MarshalCallDirectComMethod:System.Int32(System.IntPtr,System.Int32,System.Int32,System.Int32)
extern "C" CHAOS_IL2CPP_INT32 RcwBasicSubjects_RcwBasicNativeEntry_MarshalCallDirectComMethod_System_IntPtr_System_Int32_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2, CHAOS_IL2CPP_INT32 chaos_fn_arg_3)
{
	return {};
}

// AOT-unreachable stub: RcwBasicSubjects/RcwBasicNativeEntry::MarshalCreateCcw:System.IntPtr(System.IntPtr,System.IntPtr)
extern "C" CHAOS_IL2CPP_INTPTR RcwBasicSubjects_RcwBasicNativeEntry_MarshalCreateCcw_System_IntPtr_System_IntPtr(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	return {};
}

// AOT-unreachable stub: RcwBasicSubjects/RcwBasicNativeEntry::MarshalCreateRcw:System.IntPtr(System.IntPtr)
extern "C" CHAOS_IL2CPP_INTPTR RcwBasicSubjects_RcwBasicNativeEntry_MarshalCreateRcw_System_IntPtr(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return {};
}

// AOT-unreachable stub: RcwBasicSubjects/RcwBasicNativeEntry::MarshalGetRcwUnknown:System.IntPtr(System.IntPtr)
extern "C" CHAOS_IL2CPP_INTPTR RcwBasicSubjects_RcwBasicNativeEntry_MarshalGetRcwUnknown_System_IntPtr(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return {};
}

// AOT-unreachable stub: RcwBasicSubjects/RcwBasicNativeEntry::MarshalRcwQueryInterface:System.IntPtr(System.IntPtr,System.IntPtr)
extern "C" CHAOS_IL2CPP_INTPTR RcwBasicSubjects_RcwBasicNativeEntry_MarshalRcwQueryInterface_System_IntPtr_System_IntPtr(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	return {};
}

// AOT-unreachable stub: RcwBasicSubjects/RcwBasicNativeEntry::MarshalReleaseRcw:System.Void(System.IntPtr)
extern "C" void RcwBasicSubjects_RcwBasicNativeEntry_MarshalReleaseRcw_System_IntPtr(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
}

// Managed method: RcwBasicSubjects/RcwBasicNativeEntry::Run(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 RcwBasicSubjects_RcwBasicNativeEntry_Run_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
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
					auto& _d23 = s_hotpatch_entries[23];
					CHAOS_IL2CPP_INT32 _d_hpresult{};
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d23)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d23))
					{
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d23.method_key, nullptr, &_d_hpresult);
					}
					else
					{
						_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_TestRcwRoundTripIdentity();
					}
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
				}
				chaos_locals[2] = _s0;
			case 1:
				{
					auto& _d24 = s_hotpatch_entries[24];
					CHAOS_IL2CPP_INT32 _d_hpresult{};
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d24)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d24))
					{
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d24.method_key, nullptr, &_d_hpresult);
					}
					else
					{
						_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_TestRcwRoundTripQi();
					}
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
				}
				chaos_locals[2] = _s0;
			case 2:
				{
					auto& _d25 = s_hotpatch_entries[25];
					CHAOS_IL2CPP_INT32 _d_hpresult{};
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d25)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d25))
					{
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d25.method_key, nullptr, &_d_hpresult);
					}
					else
					{
						_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_TestRcwVtableMethodCall();
					}
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
				}
				chaos_locals[2] = _s0;
			case 3:
			{
				{
					auto& _d20 = s_hotpatch_entries[20];
					CHAOS_IL2CPP_INT32 _d_hpresult{};
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d20)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d20))
					{
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d20.method_key, nullptr, &_d_hpresult);
					}
					else
					{
						_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_TestRcwDirectVtable();
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

// Managed method: RcwBasicSubjects/RcwBasicNativeEntry::TestRcwDirectVtable()
extern "C" CHAOS_IL2CPP_INT32 RcwBasicSubjects_RcwBasicNativeEntry_TestRcwDirectVtable(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 7) chaos_locals{};
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


	{
		auto& _d26 = s_hotpatch_entries[26];
		CHAOS_IL2CPP_INTPTR _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d26)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d26))
		{
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d26.method_key, nullptr, &_d_hpresult);
		}
		else
		{
			_d_hpresult = RcwBasicSubjects_RuntimeState_Get();
		}
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 == 0)
		{
			_s2 = chaos_locals[0];
			{
				const auto chaos_arg_0 = _s2;
				auto& _d11 = s_hotpatch_entries[11];
				CHAOS_IL2CPP_INTPTR _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d11)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d11))
				{
					alignas(16) uint8_t _d_ab[8];
					ArgBuffer _d_bw(_d_ab);
					_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d11.method_key, _d_ab, &_d_hpresult);
				}
				else
				{
					_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_CreateCcwForSimpleMath_System_IntPtr(chaos_arg_0);
				}
				_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
			_s3 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			{
				if (_s2 == 0)
				{
					_s3 = chaos_locals[1];
					_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
					_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(10);
					_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
					{
						const auto chaos_arg_3 = _s6;
						const auto chaos_arg_2 = _s5;
						const auto chaos_arg_1 = _s4;
						const auto chaos_arg_0 = _s3;
						auto& _d13 = s_hotpatch_entries[13];
						CHAOS_IL2CPP_INT32 _d_hpresult{};
						if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d13)
							&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d13))
						{
							alignas(16) uint8_t _d_ab[20];
							ArgBuffer _d_bw(_d_ab);
							_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
							_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
							_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2));
							_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_3));
							::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
								_d13.method_key, _d_ab, &_d_hpresult);
						}
						else
						{
							_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_MarshalCallDirectComMethod_System_IntPtr_System_Int32_System_Int32_System_Int32(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_3));
						}
						_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
					}
					_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(15);
					_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s3) == static_cast<CHAOS_IL2CPP_INTPTR>(_s4) ? 1 : 0);
					_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
					_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s3) == static_cast<CHAOS_IL2CPP_INTPTR>(_s4) ? 1 : 0);
					{
						if (_s3 == 0)
						{
							_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
							chaos_locals[4] = _s3;
						}
						else
						{
							_s11 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
							chaos_locals[4] = _s11;
						}
					}
				}
				else
				{
					_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
					chaos_locals[4] = _s5;
				}
			}
		}
		else
		{
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			chaos_locals[4] = _s2;
		}
		_s2 = chaos_locals[4];
		return static_cast<CHAOS_IL2CPP_INT32>(_s2);
	}
}

// Managed method: RcwBasicSubjects/RcwBasicNativeEntry::TestRcwMultipleWrappers()
extern "C" CHAOS_IL2CPP_INT32 RcwBasicSubjects_RcwBasicNativeEntry_TestRcwMultipleWrappers(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: RcwBasicSubjects/RcwBasicNativeEntry::TestRcwQiUnknownInterface()
extern "C" CHAOS_IL2CPP_INT32 RcwBasicSubjects_RcwBasicNativeEntry_TestRcwQiUnknownInterface(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: RcwBasicSubjects/RcwBasicNativeEntry::TestRcwRoundTripIdentity()
extern "C" CHAOS_IL2CPP_INT32 RcwBasicSubjects_RcwBasicNativeEntry_TestRcwRoundTripIdentity(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 10) chaos_locals{};
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


	{
		auto& _d26 = s_hotpatch_entries[26];
		CHAOS_IL2CPP_INTPTR _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d26)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d26))
		{
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d26.method_key, nullptr, &_d_hpresult);
		}
		else
		{
			_d_hpresult = RcwBasicSubjects_RuntimeState_Get();
		}
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 == 0)
		{
			_s2 = chaos_locals[0];
			{
				const auto chaos_arg_0 = _s2;
				auto& _d11 = s_hotpatch_entries[11];
				CHAOS_IL2CPP_INTPTR _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d11)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d11))
				{
					alignas(16) uint8_t _d_ab[8];
					ArgBuffer _d_bw(_d_ab);
					_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d11.method_key, _d_ab, &_d_hpresult);
				}
				else
				{
					_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_CreateCcwForSimpleMath_System_IntPtr(chaos_arg_0);
				}
				_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
			_s3 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			{
				if (_s2 == 0)
				{
					_s3 = chaos_locals[1];
					{
						const auto chaos_arg_0 = _s3;
						auto& _d15 = s_hotpatch_entries[15];
						CHAOS_IL2CPP_INTPTR _d_hpresult{};
						if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d15)
							&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d15))
						{
							alignas(16) uint8_t _d_ab[8];
							ArgBuffer _d_bw(_d_ab);
							_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
							::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
								_d15.method_key, _d_ab, &_d_hpresult);
						}
						else
						{
							_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_MarshalCreateRcw_System_IntPtr(chaos_arg_0);
						}
						_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
					}
					chaos_locals[2] = _s3;
					_s3 = chaos_locals[2];
					_s4 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
					_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s3) == static_cast<CHAOS_IL2CPP_INTPTR>(_s4) ? 1 : 0);
					{
						if (_s3 == 0)
						{
							_s3 = chaos_locals[2];
							{
								const auto chaos_arg_0 = _s3;
								auto& _d16 = s_hotpatch_entries[16];
								CHAOS_IL2CPP_INTPTR _d_hpresult{};
								if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d16)
									&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d16))
								{
									alignas(16) uint8_t _d_ab[8];
									ArgBuffer _d_bw(_d_ab);
									_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
									::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
										_d16.method_key, _d_ab, &_d_hpresult);
								}
								else
								{
									_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_MarshalGetRcwUnknown_System_IntPtr(chaos_arg_0);
								}
								_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
							}
							_s4 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
							_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s3) == static_cast<CHAOS_IL2CPP_INTPTR>(_s4) ? 1 : 0);
							{
								if (_s3 == 0)
								{
									_s3 = chaos_locals[3];
									_s4 = chaos_locals[1];
									_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s3) == static_cast<CHAOS_IL2CPP_INTPTR>(_s4) ? 1 : 0);
									_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
									_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s3) == static_cast<CHAOS_IL2CPP_INTPTR>(_s4) ? 1 : 0);
									{
										if (_s3 == 0)
										{
											_s3 = chaos_locals[2];
											{
												const auto chaos_arg_0 = _s3;
												auto& _d18 = s_hotpatch_entries[18];
												if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d18)
													&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d18))
												{
													alignas(16) uint8_t _d_ab[8];
													ArgBuffer _d_bw(_d_ab);
													_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
													::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
														_d18.method_key, _d_ab, nullptr);
												}
												else
												{
													RcwBasicSubjects_RcwBasicNativeEntry_MarshalReleaseRcw_System_IntPtr(chaos_arg_0);
												}
											}
											_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
											chaos_locals[5] = _s3;
										}
										else
										{
											_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
											chaos_locals[5] = _s7;
										}
									}
								}
								else
								{
									_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
									chaos_locals[5] = _s6;
								}
							}
						}
						else
						{
							_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
							chaos_locals[5] = _s6;
						}
					}
				}
				else
				{
					_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
					chaos_locals[5] = _s5;
				}
			}
		}
		else
		{
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			chaos_locals[5] = _s2;
		}
		_s2 = chaos_locals[5];
		return static_cast<CHAOS_IL2CPP_INT32>(_s2);
	}
}

// Managed method: RcwBasicSubjects/RcwBasicNativeEntry::TestRcwRoundTripQi()
extern "C" CHAOS_IL2CPP_INT32 RcwBasicSubjects_RcwBasicNativeEntry_TestRcwRoundTripQi(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 13) chaos_locals{};
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


	{
		auto& _d26 = s_hotpatch_entries[26];
		CHAOS_IL2CPP_INTPTR _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d26)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d26))
		{
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d26.method_key, nullptr, &_d_hpresult);
		}
		else
		{
			_d_hpresult = RcwBasicSubjects_RuntimeState_Get();
		}
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 == 0)
		{
			_s2 = chaos_locals[0];
			{
				const auto chaos_arg_0 = _s2;
				auto& _d11 = s_hotpatch_entries[11];
				CHAOS_IL2CPP_INTPTR _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d11)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d11))
				{
					alignas(16) uint8_t _d_ab[8];
					ArgBuffer _d_bw(_d_ab);
					_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d11.method_key, _d_ab, &_d_hpresult);
				}
				else
				{
					_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_CreateCcwForSimpleMath_System_IntPtr(chaos_arg_0);
				}
				_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
			_s3 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			{
				if (_s2 == 0)
				{
					_s3 = chaos_locals[1];
					{
						const auto chaos_arg_0 = _s3;
						auto& _d15 = s_hotpatch_entries[15];
						CHAOS_IL2CPP_INTPTR _d_hpresult{};
						if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d15)
							&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d15))
						{
							alignas(16) uint8_t _d_ab[8];
							ArgBuffer _d_bw(_d_ab);
							_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
							::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
								_d15.method_key, _d_ab, &_d_hpresult);
						}
						else
						{
							_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_MarshalCreateRcw_System_IntPtr(chaos_arg_0);
						}
						_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
					}
					chaos_locals[2] = _s3;
					_s3 = chaos_locals[2];
					_s4 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
					_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s3) == static_cast<CHAOS_IL2CPP_INTPTR>(_s4) ? 1 : 0);
					{
						if (_s3 == 0)
						{
							_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(16);
							{
								const auto chaos_length = static_cast<CHAOS_IL2CPP_INT32>(_s3);
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
								_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
							}
							_s4 = _s3;
							_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(80024075u);
							{
								const auto chaos_arg_1 = _s5;
								const auto chaos_raw_arg_0 = _s4;
								const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
								chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
							}
							chaos_locals[3] = _s3;
							_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(16);
							{
								const auto chaos_arg_0 = _s3;
								const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[10])(chaos_arg_0);
								_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
							}
							chaos_locals[4] = _s3;
							_s3 = chaos_locals[3];
							_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
							_s5 = chaos_locals[4];
							_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(16);
							{
								const auto chaos_arg_3 = _s6;
								const auto chaos_arg_2 = _s5;
								const auto chaos_arg_1 = _s4;
								const auto chaos_arg_0 = _s3;
								reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[11])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3);
							}
							_s3 = chaos_locals[2];
							_s4 = chaos_locals[4];
							{
								const auto chaos_arg_1 = _s4;
								const auto chaos_arg_0 = _s3;
								auto& _d17 = s_hotpatch_entries[17];
								CHAOS_IL2CPP_INTPTR _d_hpresult{};
								if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d17)
									&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d17))
								{
									alignas(16) uint8_t _d_ab[16];
									ArgBuffer _d_bw(_d_ab);
									_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
									_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_1));
									::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
										_d17.method_key, _d_ab, &_d_hpresult);
								}
								else
								{
									_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_MarshalRcwQueryInterface_System_IntPtr_System_IntPtr(chaos_arg_0, chaos_arg_1);
								}
								_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
							}
							chaos_locals[5] = _s3;
							_s3 = chaos_locals[4];
							{
								const auto chaos_arg_0 = _s3;
								reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[12])(chaos_arg_0);
							}
							_s3 = chaos_locals[5];
							_s4 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
							_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s3) == static_cast<CHAOS_IL2CPP_INTPTR>(_s4) ? 1 : 0);
							{
								if (_s3 == 0)
								{
									_s16 = chaos_locals[2];
									{
										const auto chaos_arg_0 = _s16;
										auto& _d16 = s_hotpatch_entries[16];
										CHAOS_IL2CPP_INTPTR _d_hpresult{};
										if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d16)
											&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d16))
										{
											alignas(16) uint8_t _d_ab[8];
											ArgBuffer _d_bw(_d_ab);
											_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
											::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
												_d16.method_key, _d_ab, &_d_hpresult);
										}
										else
										{
											_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_MarshalGetRcwUnknown_System_IntPtr(chaos_arg_0);
										}
										_s16 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
									}
									chaos_locals[6] = _s16;
									_s16 = chaos_locals[5];
									_s17 = chaos_locals[6];
									_s16 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s16) == static_cast<CHAOS_IL2CPP_INTPTR>(_s17) ? 1 : 0);
									{
										if (_s16 == 0)
										{
											_s16 = chaos_locals[2];
											{
												const auto chaos_arg_0 = _s16;
												auto& _d18 = s_hotpatch_entries[18];
												if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d18)
													&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d18))
												{
													alignas(16) uint8_t _d_ab[8];
													ArgBuffer _d_bw(_d_ab);
													_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
													::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
														_d18.method_key, _d_ab, nullptr);
												}
												else
												{
													RcwBasicSubjects_RcwBasicNativeEntry_MarshalReleaseRcw_System_IntPtr(chaos_arg_0);
												}
											}
											_s16 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
											chaos_locals[8] = _s16;
										}
										else
										{
											_s19 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
											chaos_locals[8] = _s19;
										}
									}
								}
								else
								{
									_s19 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
									chaos_locals[8] = _s19;
								}
							}
						}
						else
						{
							_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
							chaos_locals[8] = _s6;
						}
					}
				}
				else
				{
					_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
					chaos_locals[8] = _s5;
				}
			}
		}
		else
		{
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			chaos_locals[8] = _s2;
		}
		_s2 = chaos_locals[8];
		return static_cast<CHAOS_IL2CPP_INT32>(_s2);
	}
}

// Managed method: RcwBasicSubjects/RcwBasicNativeEntry::TestRcwVtableMethodCall()
extern "C" CHAOS_IL2CPP_INT32 RcwBasicSubjects_RcwBasicNativeEntry_TestRcwVtableMethodCall(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 9) chaos_locals{};
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


	{
		auto& _d26 = s_hotpatch_entries[26];
		CHAOS_IL2CPP_INTPTR _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d26)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d26))
		{
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d26.method_key, nullptr, &_d_hpresult);
		}
		else
		{
			_d_hpresult = RcwBasicSubjects_RuntimeState_Get();
		}
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 == 0)
		{
			_s2 = chaos_locals[0];
			{
				const auto chaos_arg_0 = _s2;
				auto& _d11 = s_hotpatch_entries[11];
				CHAOS_IL2CPP_INTPTR _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d11)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d11))
				{
					alignas(16) uint8_t _d_ab[8];
					ArgBuffer _d_bw(_d_ab);
					_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d11.method_key, _d_ab, &_d_hpresult);
				}
				else
				{
					_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_CreateCcwForSimpleMath_System_IntPtr(chaos_arg_0);
				}
				_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
			_s3 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			{
				if (_s2 == 0)
				{
					_s3 = chaos_locals[1];
					{
						const auto chaos_arg_0 = _s3;
						auto& _d15 = s_hotpatch_entries[15];
						CHAOS_IL2CPP_INTPTR _d_hpresult{};
						if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d15)
							&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d15))
						{
							alignas(16) uint8_t _d_ab[8];
							ArgBuffer _d_bw(_d_ab);
							_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
							::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
								_d15.method_key, _d_ab, &_d_hpresult);
						}
						else
						{
							_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_MarshalCreateRcw_System_IntPtr(chaos_arg_0);
						}
						_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
					}
					chaos_locals[2] = _s3;
					_s3 = chaos_locals[2];
					_s4 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
					_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s3) == static_cast<CHAOS_IL2CPP_INTPTR>(_s4) ? 1 : 0);
					{
						if (_s3 == 0)
						{
							_s3 = chaos_locals[2];
							_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
							_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
							_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
							{
								const auto chaos_arg_3 = _s6;
								const auto chaos_arg_2 = _s5;
								const auto chaos_arg_1 = _s4;
								const auto chaos_arg_0 = _s3;
								auto& _d12 = s_hotpatch_entries[12];
								CHAOS_IL2CPP_INT32 _d_hpresult{};
								if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d12)
									&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d12))
								{
									alignas(16) uint8_t _d_ab[20];
									ArgBuffer _d_bw(_d_ab);
									_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
									_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
									_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2));
									_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_3));
									::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
										_d12.method_key, _d_ab, &_d_hpresult);
								}
								else
								{
									_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_MarshalCallComMethod_System_IntPtr_System_Int32_System_Int32_System_Int32(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_3));
								}
								_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
							}
							_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(7);
							_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s3) == static_cast<CHAOS_IL2CPP_INTPTR>(_s4) ? 1 : 0);
							_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
							_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s3) == static_cast<CHAOS_IL2CPP_INTPTR>(_s4) ? 1 : 0);
							{
								if (_s3 == 0)
								{
									_s3 = chaos_locals[2];
									{
										const auto chaos_arg_0 = _s3;
										auto& _d18 = s_hotpatch_entries[18];
										if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d18)
											&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d18))
										{
											alignas(16) uint8_t _d_ab[8];
											ArgBuffer _d_bw(_d_ab);
											_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
											::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
												_d18.method_key, _d_ab, nullptr);
										}
										else
										{
											RcwBasicSubjects_RcwBasicNativeEntry_MarshalReleaseRcw_System_IntPtr(chaos_arg_0);
										}
									}
									_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
									chaos_locals[5] = _s3;
								}
								else
								{
									_s11 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
									chaos_locals[5] = _s11;
								}
							}
						}
						else
						{
							_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
							chaos_locals[5] = _s6;
						}
					}
				}
				else
				{
					_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
					chaos_locals[5] = _s5;
				}
			}
		}
		else
		{
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			chaos_locals[5] = _s2;
		}
		_s2 = chaos_locals[5];
		return static_cast<CHAOS_IL2CPP_INT32>(_s2);
	}
}

// Managed method: RcwBasicSubjects/RuntimeState::Get()
extern "C" CHAOS_IL2CPP_INTPTR RcwBasicSubjects_RuntimeState_Get(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};


	_s0 = chaos_static_RcwBasicSubjects_RuntimeState___state;
	return _s0;
}

// Managed method: RcwBasicSubjects/RuntimeState::Set(System.IntPtr)
extern "C" void RcwBasicSubjects_RuntimeState_Set_System_IntPtr(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		auto chaos_value = _s0;
		chaos_static_RcwBasicSubjects_RuntimeState___state = chaos_value;
	}
	return;
}

// Managed method: RcwBasicSubjects/SimpleMath::.ctor()
extern "C" void RcwBasicSubjects_SimpleMath__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: RcwBasicSubjects/SimpleMath::Add(System.Int32,System.Int32)
extern "C" CHAOS_IL2CPP_INT32 RcwBasicSubjects_SimpleMath_Add_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2)
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

// Managed method: RcwBasicSubjects/SimpleMath::Multiply(System.Int32,System.Int32)
extern "C" CHAOS_IL2CPP_INT32 RcwBasicSubjects_SimpleMath_Multiply_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2)
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



}  // namespace chaos::il2cpp::codegen::RcwBasicSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 31;

extern "C" void ChaosJitRegisterAll() {}