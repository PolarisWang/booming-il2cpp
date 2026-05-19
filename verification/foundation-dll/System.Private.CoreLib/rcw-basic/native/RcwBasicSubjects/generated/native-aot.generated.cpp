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

namespace chaos::il2cpp::codegen::RcwBasicSubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


const void* chaos_vtable_RcwBasicSubjects_SimpleMath[];
inline TypeInfoV0 chaos_mt_RcwBasicSubjects__PrivateImplementationDetails_ = {{nullptr, nullptr, 183810839601874482ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_RcwBasicSubjects__PrivateImplementationDetails_ = static_cast<CHAOS_IL2CPP_INTPTR>(183810839601874482ULL);inline TypeInfoV0 chaos_mt_RcwBasicSubjects_RcwBasicSubjects = {{nullptr, nullptr, 3771092768718678272ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_RcwBasicSubjects_RcwBasicSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(3771092768718678272ULL);inline TypeInfoV0 chaos_mt_RcwBasicSubjects_RuntimeState = {{nullptr, nullptr, 2817087881528303146ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_RcwBasicSubjects_RuntimeState = static_cast<CHAOS_IL2CPP_INTPTR>(2817087881528303146ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Object = {{nullptr, nullptr, 15228727185366376748ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_INTPTR>(15228727185366376748ULL);inline TypeInfoV0 chaos_mt_RcwBasicSubjects_SimpleMath = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_RcwBasicSubjects_SimpleMath, 5661985370520645527ULL, 3u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_RcwBasicSubjects_SimpleMath = static_cast<CHAOS_IL2CPP_INTPTR>(5661985370520645527ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Byte = {{nullptr, nullptr, 3217969471912086765ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Byte = static_cast<CHAOS_IL2CPP_INTPTR>(3217969471912086765ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_IntPtr = {{nullptr, nullptr, 15622295425673451578ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_IntPtr = static_cast<CHAOS_IL2CPP_INTPTR>(15622295425673451578ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_Assembly = {{nullptr, nullptr, 5474029880995115448ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_Assembly = static_cast<CHAOS_IL2CPP_INTPTR>(5474029880995115448ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName = {{nullptr, nullptr, 17082367815459723707ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_AssemblyName = static_cast<CHAOS_IL2CPP_INTPTR>(17082367815459723707ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo = {{nullptr, nullptr, 4137207361503509124ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_ConstructorInfo = static_cast<CHAOS_IL2CPP_INTPTR>(4137207361503509124ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo = {{nullptr, nullptr, 17040031516751226236ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_FieldInfo = static_cast<CHAOS_IL2CPP_INTPTR>(17040031516751226236ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo = {{nullptr, nullptr, 10748947813473285525ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_MethodInfo = static_cast<CHAOS_IL2CPP_INTPTR>(10748947813473285525ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_String = {{nullptr, nullptr, 1782325859292956794ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
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
	{ 0x00000019u, reinterpret_cast<void*>(&RcwBasicSubjects_SimpleMath__ctor) },
	{ 0x00000017u, reinterpret_cast<void*>(&RcwBasicSubjects_SimpleMath_Add_System_Int32_System_Int32) },
	{ 0x00000018u, reinterpret_cast<void*>(&RcwBasicSubjects_SimpleMath_Multiply_System_Int32_System_Int32) },
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
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[30] = {
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
	5u,
	9u,
	11u,
	12u,
	13u,
	15u,
	16u,
	17u,
	17u,
	17u,
	17u,
	17u,
	17u,
	17u,
	17u,
	18u,
	18u,
	20u,
	22u,
};

static constexpr struct {
	::ChaosAbiManifestV0 header;
	::ChaosAbiMethodEntryV0 entries[29];
	CHAOS_IL2CPP_UINT8 params[22];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		29u,
		22u,
		1042860098u,  // FNV-1a over entries+params
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

	static constexpr CHAOS_IL2CPP_UINT32 s_type_flags[10] = {
		0u,
		1548u,
		1556u,
		1556u,
		1536u,
		1536u,
		1548u,
		12u,
		8u,
		1545u,
	};

	static constexpr const char* s_type_names[10] = {
		"<Module>",
		"RcwBasicSubjects",
		"ISimpleMath",
		"IConstantValue",
		"SimpleMath",
		"ConstantFortyTwo",
		"RcwBasicNativeEntry",
		"RuntimeState",
		"<PrivateImplementationDetails>",
		"__StaticArrayInitTypeSize=16",
	};

	static constexpr const char* s_type_namespaces[10] = {
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
		&chaos_mt_RcwBasicSubjects_RcwBasicSubjects.hot,
		nullptr,
		nullptr,
		&chaos_mt_RcwBasicSubjects_SimpleMath.hot,
		nullptr,
		nullptr,
		&chaos_mt_RcwBasicSubjects_RuntimeState.hot,
		&chaos_mt_RcwBasicSubjects__PrivateImplementationDetails_.hot,
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
		0u,
		0u,
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
		/* .type_count        = */ 10u,
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
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[29] = {
	{ ".ctor", 0x0000001Bu, 0u },  // ConstantFortyTwo
	{ "GetValue", 0x0000001Au, 0u },  // ConstantFortyTwo
	{ "CreateCcwForSimpleMath", 0x00000023u, 1u },  // RcwBasicNativeEntry
	{ "MarshalCallComMethod", 0x00000029u, 4u },  // RcwBasicNativeEntry
	{ "MarshalCallDirectComMethod", 0x0000002Au, 4u },  // RcwBasicNativeEntry
	{ "MarshalCreateCcw", 0x00000024u, 2u },  // RcwBasicNativeEntry
	{ "MarshalCreateRcw", 0x00000025u, 1u },  // RcwBasicNativeEntry
	{ "MarshalGetRcwUnknown", 0x00000027u, 1u },  // RcwBasicNativeEntry
	{ "MarshalRcwQueryInterface", 0x00000028u, 2u },  // RcwBasicNativeEntry
	{ "MarshalReleaseRcw", 0x00000026u, 1u },  // RcwBasicNativeEntry
	{ "Run", 0x0000001Cu, 1u },  // RcwBasicNativeEntry
	{ "TestRcwDirectVtable", 0x00000022u, 0u },  // RcwBasicNativeEntry
	{ "TestRcwMultipleWrappers", 0x0000001Fu, 0u },  // RcwBasicNativeEntry
	{ "TestRcwQiUnknownInterface", 0x00000020u, 0u },  // RcwBasicNativeEntry
	{ "TestRcwRoundTripIdentity", 0x0000001Du, 0u },  // RcwBasicNativeEntry
	{ "TestRcwRoundTripQi", 0x0000001Eu, 0u },  // RcwBasicNativeEntry
	{ "TestRcwVtableMethodCall", 0x00000021u, 0u },  // RcwBasicNativeEntry
	{ "Subject_0", 0x0000000Du, 0u },  // RcwBasicSubjects
	{ "Subject_1", 0x0000000Eu, 0u },  // RcwBasicSubjects
	{ "Subject_2", 0x0000000Fu, 0u },  // RcwBasicSubjects
	{ "Subject_3", 0x00000010u, 0u },  // RcwBasicSubjects
	{ "Subject_4", 0x00000011u, 0u },  // RcwBasicSubjects
	{ "Subject_5", 0x00000012u, 0u },  // RcwBasicSubjects
	{ "Subject_6", 0x00000013u, 0u },  // RcwBasicSubjects
	{ "Get", 0x0000002Bu, 0u },  // RuntimeState
	{ "Set", 0x0000002Cu, 1u },  // RuntimeState
	{ ".ctor", 0x00000019u, 0u },  // SimpleMath
	{ "Add", 0x00000017u, 2u },  // SimpleMath
	{ "Multiply", 0x00000018u, 2u },  // SimpleMath
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[5] = {
	{ "ConstantFortyTwo", "", 0u, 2u },
	{ "RcwBasicNativeEntry", "", 2u, 15u },
	{ "RcwBasicSubjects", "", 17u, 7u },
	{ "RuntimeState", "", 24u, 2u },
	{ "SimpleMath", "", 26u, 3u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[29] = {
	{ 0x0000000Du, 0u },
	{ 0x0000000Eu, 1u },
	{ 0x0000000Fu, 2u },
	{ 0x00000010u, 3u },
	{ 0x00000011u, 4u },
	{ 0x00000012u, 5u },
	{ 0x00000013u, 6u },
	{ 0x00000017u, 27u },
	{ 0x00000018u, 28u },
	{ 0x00000019u, 26u },
	{ 0x0000001Au, 8u },
	{ 0x0000001Bu, 7u },
	{ 0x0000001Cu, 17u },
	{ 0x0000001Du, 21u },
	{ 0x0000001Eu, 22u },
	{ 0x0000001Fu, 19u },
	{ 0x00000020u, 20u },
	{ 0x00000021u, 23u },
	{ 0x00000022u, 18u },
	{ 0x00000023u, 9u },
	{ 0x00000024u, 12u },
	{ 0x00000025u, 13u },
	{ 0x00000026u, 16u },
	{ 0x00000027u, 14u },
	{ 0x00000028u, 15u },
	{ 0x00000029u, 10u },
	{ 0x0000002Au, 11u },
	{ 0x0000002Bu, 24u },
	{ 0x0000002Cu, 25u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[29] = {
	{ reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicSubjects_Subject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // RcwBasicSubjects::Subject_0
	{ reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicSubjects_Subject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // RcwBasicSubjects::Subject_1
	{ reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicSubjects_Subject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // RcwBasicSubjects::Subject_2
	{ reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicSubjects_Subject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // RcwBasicSubjects::Subject_3
	{ reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicSubjects_Subject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // RcwBasicSubjects::Subject_4
	{ reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicSubjects_Subject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // RcwBasicSubjects::Subject_5
	{ reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicSubjects_Subject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // RcwBasicSubjects::Subject_6
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
	{ reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicNativeEntry_TestRcwQiUnknownInterface), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // RcwBasicNativeEntry::TestRcwQiUnknownInterface
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
	5u,
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
static void (*kAotMethods[29])() = {
	reinterpret_cast<void(*)()>(&RcwBasicSubjects_RcwBasicSubjects_Subject_0),
	reinterpret_cast<void(*)()>(&RcwBasicSubjects_RcwBasicSubjects_Subject_1),
	reinterpret_cast<void(*)()>(&RcwBasicSubjects_RcwBasicSubjects_Subject_2),
	reinterpret_cast<void(*)()>(&RcwBasicSubjects_RcwBasicSubjects_Subject_3),
	reinterpret_cast<void(*)()>(&RcwBasicSubjects_RcwBasicSubjects_Subject_4),
	reinterpret_cast<void(*)()>(&RcwBasicSubjects_RcwBasicSubjects_Subject_5),
	reinterpret_cast<void(*)()>(&RcwBasicSubjects_RcwBasicSubjects_Subject_6),
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
static void (*kBenchmarkWrappers[29])() = {
	[]() {kAotMethods[0]();},
	[]() {kAotMethods[1]();},
	[]() {kAotMethods[2]();},
	[]() {kAotMethods[3]();},
	[]() {kAotMethods[4]();},
	[]() {kAotMethods[5]();},
	[]() {kAotMethods[6]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[7])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[8])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[9])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[10])(0,0,0,0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[11])(0,0,0,0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[12])(0,0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[13])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[14])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[15])(0,0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[16])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[17])(0);},
	[]() {kAotMethods[18]();},
	[]() {kAotMethods[19]();},
	[]() {kAotMethods[20]();},
	[]() {kAotMethods[21]();},
	[]() {kAotMethods[22]();},
	[]() {kAotMethods[23]();},
	[]() {kAotMethods[24]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[25])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[26])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[27])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0,0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[28])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0,0);},
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
static void* const kMethodPointers[29] = {
	reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicSubjects_Subject_0),
	reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicSubjects_Subject_1),
	reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicSubjects_Subject_2),
	reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicSubjects_Subject_3),
	reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicSubjects_Subject_4),
	reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicSubjects_Subject_5),
	reinterpret_cast<void*>(&RcwBasicSubjects_RcwBasicSubjects_Subject_6),
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
	.vtable_descriptor_count = 1u,
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

static const ReflectionQueryTypeDescriptor kReflTypes[5] = {
	{ 0u, "RcwBasicSubjects/RcwBasicSubjects", "RcwBasicSubjects/RcwBasicSubjects", "", "RcwBasicSubjects", "RcwBasicSubjects", nullptr, kReflFields_RcwBasicSubjects_RcwBasicSubjects, 1u, nullptr, 0u,
	kReflMethods_RcwBasicSubjects_RcwBasicSubjects, 7u },
	{ 0u, "RcwBasicSubjects/ConstantFortyTwo", "RcwBasicSubjects/ConstantFortyTwo", "", "ConstantFortyTwo", "ConstantFortyTwo", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_RcwBasicSubjects_ConstantFortyTwo, 2u },
	{ 0u, "RcwBasicSubjects/RcwBasicNativeEntry", "RcwBasicSubjects/RcwBasicNativeEntry", "", "RcwBasicNativeEntry", "RcwBasicNativeEntry", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_RcwBasicSubjects_RcwBasicNativeEntry, 15u },
	{ 0u, "RcwBasicSubjects/RuntimeState", "RcwBasicSubjects/RuntimeState", "", "RuntimeState", "RuntimeState", nullptr, kReflFields_RcwBasicSubjects_RuntimeState, 1u, nullptr, 0u,
	kReflMethods_RcwBasicSubjects_RuntimeState, 2u },
	{ 0u, "RcwBasicSubjects/SimpleMath", "RcwBasicSubjects/SimpleMath", "", "SimpleMath", "SimpleMath", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_RcwBasicSubjects_SimpleMath, 3u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[5] = {
	&kReflTypes[0],
	&kReflTypes[1],
	&kReflTypes[2],
	&kReflTypes[3],
	&kReflTypes[4],
};

static const ReflectionQueryImageDescriptor kReflImage = { "RcwBasicSubjects", kReflTypePtrs, 5u };

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
				chaos_static_RcwBasicSubjects_RcwBasicSubjects___exitCode = chaos_value;
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
				chaos_static_RcwBasicSubjects_RcwBasicSubjects___exitCode = chaos_value;
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
				chaos_static_RcwBasicSubjects_RcwBasicSubjects___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
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
				chaos_static_RcwBasicSubjects_RcwBasicSubjects___exitCode = chaos_value;
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
				chaos_static_RcwBasicSubjects_RcwBasicSubjects___exitCode = chaos_value;
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
				chaos_static_RcwBasicSubjects_RcwBasicSubjects___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
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
				chaos_static_RcwBasicSubjects_RcwBasicSubjects___exitCode = chaos_value;
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
				chaos_static_RcwBasicSubjects_RcwBasicSubjects___exitCode = chaos_value;
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
				chaos_static_RcwBasicSubjects_RcwBasicSubjects___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
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
				chaos_static_RcwBasicSubjects_RcwBasicSubjects___exitCode = chaos_value;
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
				chaos_static_RcwBasicSubjects_RcwBasicSubjects___exitCode = chaos_value;
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
				chaos_static_RcwBasicSubjects_RcwBasicSubjects___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
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
				chaos_static_RcwBasicSubjects_RcwBasicSubjects___exitCode = chaos_value;
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
				chaos_static_RcwBasicSubjects_RcwBasicSubjects___exitCode = chaos_value;
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
				chaos_static_RcwBasicSubjects_RcwBasicSubjects___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
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
				chaos_static_RcwBasicSubjects_RcwBasicSubjects___exitCode = chaos_value;
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
				chaos_static_RcwBasicSubjects_RcwBasicSubjects___exitCode = chaos_value;
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
				chaos_static_RcwBasicSubjects_RcwBasicSubjects___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
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
				chaos_static_RcwBasicSubjects_RcwBasicSubjects___exitCode = chaos_value;
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
				chaos_static_RcwBasicSubjects_RcwBasicSubjects___exitCode = chaos_value;
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
				chaos_static_RcwBasicSubjects_RcwBasicSubjects___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
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
		chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(chaos_arg_0);
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
		auto& _d5 = s_hotpatch_entries[5];
		CHAOS_IL2CPP_INTPTR _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d5)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d5))
		{
			alignas(16) uint8_t _d_ab[16];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
			_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_1));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d5.method_key, _d_ab, &_d_hpresult);
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
			_s0 = chaos_locals[1];
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[5])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[4] = _s0;
			_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[4]);
			{
				reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[6])();
			}
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			chaos_locals[5] = _s1;
		}
		_s1 = chaos_locals[5];
		return _s1;
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
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		const auto chaos_switch_value = static_cast<CHAOS_IL2CPP_INT32>(_s0);
		switch (chaos_switch_value)
		{
			case 0:
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
						_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_TestRcwRoundTripIdentity();
					}
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
				}
				chaos_locals[2] = _s0;
			case 1:
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
						_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_TestRcwRoundTripQi();
					}
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
				}
				chaos_locals[2] = _s0;
			case 2:
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
						_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_TestRcwMultipleWrappers();
					}
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
				}
				chaos_locals[2] = _s0;
			case 3:
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
						_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_TestRcwQiUnknownInterface();
					}
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
				}
				chaos_locals[2] = _s0;
			case 4:
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
						_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_TestRcwVtableMethodCall();
					}
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
				}
				chaos_locals[2] = _s0;
			case 5:
			{
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
		auto& _d24 = s_hotpatch_entries[24];
		CHAOS_IL2CPP_INTPTR _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d24)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d24))
		{
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d24.method_key, nullptr, &_d_hpresult);
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
			_s0 = chaos_locals[0];
			{
				const auto chaos_arg_0 = _s0;
				auto& _d2 = s_hotpatch_entries[2];
				CHAOS_IL2CPP_INTPTR _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d2)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d2))
				{
					alignas(16) uint8_t _d_ab[8];
					ArgBuffer _d_bw(_d_ab);
					_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d2.method_key, _d_ab, &_d_hpresult);
				}
				else
				{
					_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_CreateCcwForSimpleMath_System_IntPtr(chaos_arg_0);
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			{
				if (_s0 == 0)
				{
					_s0 = chaos_locals[1];
					_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
					_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(10);
					_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
					{
						const auto chaos_arg_3 = _s3;
						const auto chaos_arg_2 = _s2;
						const auto chaos_arg_1 = _s1;
						const auto chaos_arg_0 = _s0;
						auto& _d4 = s_hotpatch_entries[4];
						CHAOS_IL2CPP_INT32 _d_hpresult{};
						if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d4)
							&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d4))
						{
							alignas(16) uint8_t _d_ab[20];
							ArgBuffer _d_bw(_d_ab);
							_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
							_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
							_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2));
							_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_3));
							::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
								_d4.method_key, _d_ab, &_d_hpresult);
						}
						else
						{
							_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_MarshalCallDirectComMethod_System_IntPtr_System_Int32_System_Int32_System_Int32(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_3));
						}
						_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
					}
					_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(15);
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
					_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
					{
						if (_s0 == 0)
						{
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
							chaos_locals[4] = _s0;
						}
						else
						{
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
							chaos_locals[4] = _s0;
						}
					}
				}
				else
				{
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
					chaos_locals[4] = _s0;
				}
			}
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			chaos_locals[4] = _s0;
		}
		_s0 = chaos_locals[4];
		return static_cast<CHAOS_IL2CPP_INT32>(_s0);
	}
}

// Managed method: RcwBasicSubjects/RcwBasicNativeEntry::TestRcwMultipleWrappers()
extern "C" CHAOS_IL2CPP_INT32 RcwBasicSubjects_RcwBasicNativeEntry_TestRcwMultipleWrappers(void)
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
	CHAOS_IL2CPP_INTPTR _s49{};
	CHAOS_IL2CPP_INTPTR _s50{};
	CHAOS_IL2CPP_INTPTR _s51{};
	CHAOS_IL2CPP_INTPTR _s52{};
	CHAOS_IL2CPP_INTPTR _s53{};


	{
		auto& _d24 = s_hotpatch_entries[24];
		CHAOS_IL2CPP_INTPTR _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d24)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d24))
		{
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d24.method_key, nullptr, &_d_hpresult);
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
			_s0 = chaos_locals[0];
			{
				const auto chaos_arg_0 = _s0;
				auto& _d2 = s_hotpatch_entries[2];
				CHAOS_IL2CPP_INTPTR _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d2)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d2))
				{
					alignas(16) uint8_t _d_ab[8];
					ArgBuffer _d_bw(_d_ab);
					_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d2.method_key, _d_ab, &_d_hpresult);
				}
				else
				{
					_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_CreateCcwForSimpleMath_System_IntPtr(chaos_arg_0);
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			{
				if (_s0 == 0)
				{
					_s0 = chaos_locals[1];
					{
						const auto chaos_arg_0 = _s0;
						auto& _d6 = s_hotpatch_entries[6];
						CHAOS_IL2CPP_INTPTR _d_hpresult{};
						if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d6)
							&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d6))
						{
							alignas(16) uint8_t _d_ab[8];
							ArgBuffer _d_bw(_d_ab);
							_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
							::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
								_d6.method_key, _d_ab, &_d_hpresult);
						}
						else
						{
							_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_MarshalCreateRcw_System_IntPtr(chaos_arg_0);
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
							_s0 = chaos_locals[1];
							{
								const auto chaos_arg_0 = _s0;
								auto& _d6 = s_hotpatch_entries[6];
								CHAOS_IL2CPP_INTPTR _d_hpresult{};
								if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d6)
									&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d6))
								{
									alignas(16) uint8_t _d_ab[8];
									ArgBuffer _d_bw(_d_ab);
									_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
									::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
										_d6.method_key, _d_ab, &_d_hpresult);
								}
								else
								{
									_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_MarshalCreateRcw_System_IntPtr(chaos_arg_0);
								}
								_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
							}
							chaos_locals[3] = _s0;
							_s0 = chaos_locals[3];
							_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
							{
								if (_s0 == 0)
								{
									_s0 = chaos_locals[2];
									{
										const auto chaos_arg_0 = _s0;
										auto& _d7 = s_hotpatch_entries[7];
										CHAOS_IL2CPP_INTPTR _d_hpresult{};
										if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d7)
											&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d7))
										{
											alignas(16) uint8_t _d_ab[8];
											ArgBuffer _d_bw(_d_ab);
											_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
											::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
												_d7.method_key, _d_ab, &_d_hpresult);
										}
										else
										{
											_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_MarshalGetRcwUnknown_System_IntPtr(chaos_arg_0);
										}
										_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
									}
									chaos_locals[4] = _s0;
									_s0 = chaos_locals[3];
									{
										const auto chaos_arg_0 = _s0;
										auto& _d7 = s_hotpatch_entries[7];
										CHAOS_IL2CPP_INTPTR _d_hpresult{};
										if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d7)
											&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d7))
										{
											alignas(16) uint8_t _d_ab[8];
											ArgBuffer _d_bw(_d_ab);
											_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
											::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
												_d7.method_key, _d_ab, &_d_hpresult);
										}
										else
										{
											_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_MarshalGetRcwUnknown_System_IntPtr(chaos_arg_0);
										}
										_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
									}
									chaos_locals[5] = _s0;
									_s0 = chaos_locals[4];
									_s1 = chaos_locals[5];
									_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
									_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
									_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
									{
										if (_s0 == 0)
										{
											_s0 = chaos_locals[4];
											_s1 = chaos_locals[1];
											_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
											_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
											_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
											{
												if (_s0 == 0)
												{
													_s0 = chaos_locals[3];
													{
														const auto chaos_arg_0 = _s0;
														auto& _d9 = s_hotpatch_entries[9];
														if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d9)
															&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d9))
														{
															alignas(16) uint8_t _d_ab[8];
															ArgBuffer _d_bw(_d_ab);
															_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
															::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
																_d9.method_key, _d_ab, nullptr);
														}
														else
														{
															RcwBasicSubjects_RcwBasicNativeEntry_MarshalReleaseRcw_System_IntPtr(chaos_arg_0);
														}
													}
													_s0 = chaos_locals[2];
													{
														const auto chaos_arg_0 = _s0;
														auto& _d9 = s_hotpatch_entries[9];
														if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d9)
															&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d9))
														{
															alignas(16) uint8_t _d_ab[8];
															ArgBuffer _d_bw(_d_ab);
															_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
															::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
																_d9.method_key, _d_ab, nullptr);
														}
														else
														{
															RcwBasicSubjects_RcwBasicNativeEntry_MarshalReleaseRcw_System_IntPtr(chaos_arg_0);
														}
													}
													_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
													chaos_locals[7] = _s0;
												}
												else
												{
													_s0 = chaos_locals[3];
													{
														const auto chaos_arg_0 = _s0;
														auto& _d9 = s_hotpatch_entries[9];
														if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d9)
															&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d9))
														{
															alignas(16) uint8_t _d_ab[8];
															ArgBuffer _d_bw(_d_ab);
															_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
															::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
																_d9.method_key, _d_ab, nullptr);
														}
														else
														{
															RcwBasicSubjects_RcwBasicNativeEntry_MarshalReleaseRcw_System_IntPtr(chaos_arg_0);
														}
													}
													_s0 = chaos_locals[2];
													{
														const auto chaos_arg_0 = _s0;
														auto& _d9 = s_hotpatch_entries[9];
														if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d9)
															&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d9))
														{
															alignas(16) uint8_t _d_ab[8];
															ArgBuffer _d_bw(_d_ab);
															_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
															::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
																_d9.method_key, _d_ab, nullptr);
														}
														else
														{
															RcwBasicSubjects_RcwBasicNativeEntry_MarshalReleaseRcw_System_IntPtr(chaos_arg_0);
														}
													}
													_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(6);
													chaos_locals[7] = _s0;
												}
											}
										}
										else
										{
											_s0 = chaos_locals[3];
											{
												const auto chaos_arg_0 = _s0;
												auto& _d9 = s_hotpatch_entries[9];
												if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d9)
													&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d9))
												{
													alignas(16) uint8_t _d_ab[8];
													ArgBuffer _d_bw(_d_ab);
													_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
													::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
														_d9.method_key, _d_ab, nullptr);
												}
												else
												{
													RcwBasicSubjects_RcwBasicNativeEntry_MarshalReleaseRcw_System_IntPtr(chaos_arg_0);
												}
											}
											_s0 = chaos_locals[2];
											{
												const auto chaos_arg_0 = _s0;
												auto& _d9 = s_hotpatch_entries[9];
												if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d9)
													&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d9))
												{
													alignas(16) uint8_t _d_ab[8];
													ArgBuffer _d_bw(_d_ab);
													_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
													::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
														_d9.method_key, _d_ab, nullptr);
												}
												else
												{
													RcwBasicSubjects_RcwBasicNativeEntry_MarshalReleaseRcw_System_IntPtr(chaos_arg_0);
												}
											}
											_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
											chaos_locals[7] = _s0;
										}
									}
								}
								else
								{
									_s0 = chaos_locals[2];
									{
										const auto chaos_arg_0 = _s0;
										auto& _d9 = s_hotpatch_entries[9];
										if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d9)
											&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d9))
										{
											alignas(16) uint8_t _d_ab[8];
											ArgBuffer _d_bw(_d_ab);
											_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
											::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
												_d9.method_key, _d_ab, nullptr);
										}
										else
										{
											RcwBasicSubjects_RcwBasicNativeEntry_MarshalReleaseRcw_System_IntPtr(chaos_arg_0);
										}
									}
									_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
									chaos_locals[7] = _s0;
								}
							}
						}
						else
						{
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
							chaos_locals[7] = _s0;
						}
					}
				}
				else
				{
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
					chaos_locals[7] = _s0;
				}
			}
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			chaos_locals[7] = _s0;
		}
		_s0 = chaos_locals[7];
		return static_cast<CHAOS_IL2CPP_INT32>(_s0);
	}
}

// Managed method: RcwBasicSubjects/RcwBasicNativeEntry::TestRcwQiUnknownInterface()
extern "C" CHAOS_IL2CPP_INT32 RcwBasicSubjects_RcwBasicNativeEntry_TestRcwQiUnknownInterface(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 11) chaos_locals{};
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


	{
		auto& _d24 = s_hotpatch_entries[24];
		CHAOS_IL2CPP_INTPTR _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d24)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d24))
		{
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d24.method_key, nullptr, &_d_hpresult);
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
			_s0 = chaos_locals[0];
			{
				const auto chaos_arg_0 = _s0;
				auto& _d2 = s_hotpatch_entries[2];
				CHAOS_IL2CPP_INTPTR _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d2)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d2))
				{
					alignas(16) uint8_t _d_ab[8];
					ArgBuffer _d_bw(_d_ab);
					_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d2.method_key, _d_ab, &_d_hpresult);
				}
				else
				{
					_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_CreateCcwForSimpleMath_System_IntPtr(chaos_arg_0);
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			{
				if (_s0 == 0)
				{
					_s0 = chaos_locals[1];
					{
						const auto chaos_arg_0 = _s0;
						auto& _d6 = s_hotpatch_entries[6];
						CHAOS_IL2CPP_INTPTR _d_hpresult{};
						if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d6)
							&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d6))
						{
							alignas(16) uint8_t _d_ab[8];
							ArgBuffer _d_bw(_d_ab);
							_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
							::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
								_d6.method_key, _d_ab, &_d_hpresult);
						}
						else
						{
							_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_MarshalCreateRcw_System_IntPtr(chaos_arg_0);
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
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(16);
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
							_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(80024075u);
							{
								const auto chaos_arg_1 = _s2;
								const auto chaos_raw_arg_0 = _s1;
								const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s1);
								chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers__InitializeArray_System_Void_System_Array_System_RuntimeFieldHandle_(chaos_arg_0, chaos_arg_1);
							}
							chaos_locals[3] = _s0;
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(16);
							{
								const auto chaos_arg_0 = _s0;
								const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[10])(chaos_arg_0);
								_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
							}
							chaos_locals[4] = _s0;
							_s0 = chaos_locals[3];
							_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
							_s2 = chaos_locals[4];
							_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(16);
							{
								const auto chaos_arg_3 = _s3;
								const auto chaos_arg_2 = _s2;
								const auto chaos_arg_1 = _s1;
								const auto chaos_arg_0 = _s0;
								reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[11])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3);
							}
							_s0 = chaos_locals[2];
							_s1 = chaos_locals[4];
							{
								const auto chaos_arg_1 = _s1;
								const auto chaos_arg_0 = _s0;
								auto& _d8 = s_hotpatch_entries[8];
								CHAOS_IL2CPP_INTPTR _d_hpresult{};
								if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d8)
									&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d8))
								{
									alignas(16) uint8_t _d_ab[16];
									ArgBuffer _d_bw(_d_ab);
									_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
									_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_1));
									::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
										_d8.method_key, _d_ab, &_d_hpresult);
								}
								else
								{
									_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_MarshalRcwQueryInterface_System_IntPtr_System_IntPtr(chaos_arg_0, chaos_arg_1);
								}
								_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
							}
							chaos_locals[5] = _s0;
							_s0 = chaos_locals[4];
							{
								const auto chaos_arg_0 = _s0;
								reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[12])(chaos_arg_0);
							}
							_s0 = chaos_locals[5];
							_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
							_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
							{
								if (_s0 == 0)
								{
									_s0 = chaos_locals[2];
									{
										const auto chaos_arg_0 = _s0;
										auto& _d9 = s_hotpatch_entries[9];
										if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d9)
											&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d9))
										{
											alignas(16) uint8_t _d_ab[8];
											ArgBuffer _d_bw(_d_ab);
											_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
											::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
												_d9.method_key, _d_ab, nullptr);
										}
										else
										{
											RcwBasicSubjects_RcwBasicNativeEntry_MarshalReleaseRcw_System_IntPtr(chaos_arg_0);
										}
									}
									_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
									chaos_locals[7] = _s0;
								}
								else
								{
									_s0 = chaos_locals[2];
									{
										const auto chaos_arg_0 = _s0;
										auto& _d9 = s_hotpatch_entries[9];
										if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d9)
											&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d9))
										{
											alignas(16) uint8_t _d_ab[8];
											ArgBuffer _d_bw(_d_ab);
											_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
											::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
												_d9.method_key, _d_ab, nullptr);
										}
										else
										{
											RcwBasicSubjects_RcwBasicNativeEntry_MarshalReleaseRcw_System_IntPtr(chaos_arg_0);
										}
									}
									_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
									chaos_locals[7] = _s0;
								}
							}
						}
						else
						{
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
							chaos_locals[7] = _s0;
						}
					}
				}
				else
				{
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
					chaos_locals[7] = _s0;
				}
			}
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			chaos_locals[7] = _s0;
		}
		_s0 = chaos_locals[7];
		return static_cast<CHAOS_IL2CPP_INT32>(_s0);
	}
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
		auto& _d24 = s_hotpatch_entries[24];
		CHAOS_IL2CPP_INTPTR _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d24)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d24))
		{
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d24.method_key, nullptr, &_d_hpresult);
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
			_s0 = chaos_locals[0];
			{
				const auto chaos_arg_0 = _s0;
				auto& _d2 = s_hotpatch_entries[2];
				CHAOS_IL2CPP_INTPTR _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d2)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d2))
				{
					alignas(16) uint8_t _d_ab[8];
					ArgBuffer _d_bw(_d_ab);
					_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d2.method_key, _d_ab, &_d_hpresult);
				}
				else
				{
					_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_CreateCcwForSimpleMath_System_IntPtr(chaos_arg_0);
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			{
				if (_s0 == 0)
				{
					_s0 = chaos_locals[1];
					{
						const auto chaos_arg_0 = _s0;
						auto& _d6 = s_hotpatch_entries[6];
						CHAOS_IL2CPP_INTPTR _d_hpresult{};
						if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d6)
							&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d6))
						{
							alignas(16) uint8_t _d_ab[8];
							ArgBuffer _d_bw(_d_ab);
							_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
							::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
								_d6.method_key, _d_ab, &_d_hpresult);
						}
						else
						{
							_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_MarshalCreateRcw_System_IntPtr(chaos_arg_0);
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
							{
								const auto chaos_arg_0 = _s0;
								auto& _d7 = s_hotpatch_entries[7];
								CHAOS_IL2CPP_INTPTR _d_hpresult{};
								if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d7)
									&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d7))
								{
									alignas(16) uint8_t _d_ab[8];
									ArgBuffer _d_bw(_d_ab);
									_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
									::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
										_d7.method_key, _d_ab, &_d_hpresult);
								}
								else
								{
									_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_MarshalGetRcwUnknown_System_IntPtr(chaos_arg_0);
								}
								_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
							}
							_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
							{
								if (_s0 == 0)
								{
									_s0 = chaos_locals[3];
									_s1 = chaos_locals[1];
									_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
									_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
									_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
									{
										if (_s0 == 0)
										{
											_s0 = chaos_locals[2];
											{
												const auto chaos_arg_0 = _s0;
												auto& _d9 = s_hotpatch_entries[9];
												if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d9)
													&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d9))
												{
													alignas(16) uint8_t _d_ab[8];
													ArgBuffer _d_bw(_d_ab);
													_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
													::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
														_d9.method_key, _d_ab, nullptr);
												}
												else
												{
													RcwBasicSubjects_RcwBasicNativeEntry_MarshalReleaseRcw_System_IntPtr(chaos_arg_0);
												}
											}
											_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
											chaos_locals[5] = _s0;
										}
										else
										{
											_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
											chaos_locals[5] = _s0;
										}
									}
								}
								else
								{
									_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
									chaos_locals[5] = _s0;
								}
							}
						}
						else
						{
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
							chaos_locals[5] = _s0;
						}
					}
				}
				else
				{
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
					chaos_locals[5] = _s0;
				}
			}
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			chaos_locals[5] = _s0;
		}
		_s0 = chaos_locals[5];
		return static_cast<CHAOS_IL2CPP_INT32>(_s0);
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
		auto& _d24 = s_hotpatch_entries[24];
		CHAOS_IL2CPP_INTPTR _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d24)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d24))
		{
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d24.method_key, nullptr, &_d_hpresult);
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
			_s0 = chaos_locals[0];
			{
				const auto chaos_arg_0 = _s0;
				auto& _d2 = s_hotpatch_entries[2];
				CHAOS_IL2CPP_INTPTR _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d2)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d2))
				{
					alignas(16) uint8_t _d_ab[8];
					ArgBuffer _d_bw(_d_ab);
					_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d2.method_key, _d_ab, &_d_hpresult);
				}
				else
				{
					_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_CreateCcwForSimpleMath_System_IntPtr(chaos_arg_0);
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			{
				if (_s0 == 0)
				{
					_s0 = chaos_locals[1];
					{
						const auto chaos_arg_0 = _s0;
						auto& _d6 = s_hotpatch_entries[6];
						CHAOS_IL2CPP_INTPTR _d_hpresult{};
						if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d6)
							&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d6))
						{
							alignas(16) uint8_t _d_ab[8];
							ArgBuffer _d_bw(_d_ab);
							_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
							::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
								_d6.method_key, _d_ab, &_d_hpresult);
						}
						else
						{
							_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_MarshalCreateRcw_System_IntPtr(chaos_arg_0);
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
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(16);
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
							_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(80024075u);
							{
								const auto chaos_arg_1 = _s2;
								const auto chaos_raw_arg_0 = _s1;
								const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s1);
								chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers__InitializeArray_System_Void_System_Array_System_RuntimeFieldHandle_(chaos_arg_0, chaos_arg_1);
							}
							chaos_locals[3] = _s0;
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(16);
							{
								const auto chaos_arg_0 = _s0;
								const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[10])(chaos_arg_0);
								_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
							}
							chaos_locals[4] = _s0;
							_s0 = chaos_locals[3];
							_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
							_s2 = chaos_locals[4];
							_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(16);
							{
								const auto chaos_arg_3 = _s3;
								const auto chaos_arg_2 = _s2;
								const auto chaos_arg_1 = _s1;
								const auto chaos_arg_0 = _s0;
								reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[11])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3);
							}
							_s0 = chaos_locals[2];
							_s1 = chaos_locals[4];
							{
								const auto chaos_arg_1 = _s1;
								const auto chaos_arg_0 = _s0;
								auto& _d8 = s_hotpatch_entries[8];
								CHAOS_IL2CPP_INTPTR _d_hpresult{};
								if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d8)
									&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d8))
								{
									alignas(16) uint8_t _d_ab[16];
									ArgBuffer _d_bw(_d_ab);
									_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
									_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_1));
									::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
										_d8.method_key, _d_ab, &_d_hpresult);
								}
								else
								{
									_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_MarshalRcwQueryInterface_System_IntPtr_System_IntPtr(chaos_arg_0, chaos_arg_1);
								}
								_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
							}
							chaos_locals[5] = _s0;
							_s0 = chaos_locals[4];
							{
								const auto chaos_arg_0 = _s0;
								reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[12])(chaos_arg_0);
							}
							_s0 = chaos_locals[5];
							_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
							{
								if (_s0 == 0)
								{
									_s0 = chaos_locals[2];
									{
										const auto chaos_arg_0 = _s0;
										auto& _d7 = s_hotpatch_entries[7];
										CHAOS_IL2CPP_INTPTR _d_hpresult{};
										if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d7)
											&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d7))
										{
											alignas(16) uint8_t _d_ab[8];
											ArgBuffer _d_bw(_d_ab);
											_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
											::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
												_d7.method_key, _d_ab, &_d_hpresult);
										}
										else
										{
											_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_MarshalGetRcwUnknown_System_IntPtr(chaos_arg_0);
										}
										_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
									}
									chaos_locals[6] = _s0;
									_s0 = chaos_locals[5];
									_s1 = chaos_locals[6];
									_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
									{
										if (_s0 == 0)
										{
											_s0 = chaos_locals[2];
											{
												const auto chaos_arg_0 = _s0;
												auto& _d9 = s_hotpatch_entries[9];
												if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d9)
													&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d9))
												{
													alignas(16) uint8_t _d_ab[8];
													ArgBuffer _d_bw(_d_ab);
													_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
													::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
														_d9.method_key, _d_ab, nullptr);
												}
												else
												{
													RcwBasicSubjects_RcwBasicNativeEntry_MarshalReleaseRcw_System_IntPtr(chaos_arg_0);
												}
											}
											_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
											chaos_locals[8] = _s0;
										}
										else
										{
											_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
											chaos_locals[8] = _s0;
										}
									}
								}
								else
								{
									_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
									chaos_locals[8] = _s0;
								}
							}
						}
						else
						{
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
							chaos_locals[8] = _s0;
						}
					}
				}
				else
				{
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
					chaos_locals[8] = _s0;
				}
			}
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			chaos_locals[8] = _s0;
		}
		_s0 = chaos_locals[8];
		return static_cast<CHAOS_IL2CPP_INT32>(_s0);
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
		auto& _d24 = s_hotpatch_entries[24];
		CHAOS_IL2CPP_INTPTR _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d24)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d24))
		{
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d24.method_key, nullptr, &_d_hpresult);
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
			_s0 = chaos_locals[0];
			{
				const auto chaos_arg_0 = _s0;
				auto& _d2 = s_hotpatch_entries[2];
				CHAOS_IL2CPP_INTPTR _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d2)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d2))
				{
					alignas(16) uint8_t _d_ab[8];
					ArgBuffer _d_bw(_d_ab);
					_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d2.method_key, _d_ab, &_d_hpresult);
				}
				else
				{
					_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_CreateCcwForSimpleMath_System_IntPtr(chaos_arg_0);
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			{
				if (_s0 == 0)
				{
					_s0 = chaos_locals[1];
					{
						const auto chaos_arg_0 = _s0;
						auto& _d6 = s_hotpatch_entries[6];
						CHAOS_IL2CPP_INTPTR _d_hpresult{};
						if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d6)
							&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d6))
						{
							alignas(16) uint8_t _d_ab[8];
							ArgBuffer _d_bw(_d_ab);
							_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
							::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
								_d6.method_key, _d_ab, &_d_hpresult);
						}
						else
						{
							_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_MarshalCreateRcw_System_IntPtr(chaos_arg_0);
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
							_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
							_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
							_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
							{
								const auto chaos_arg_3 = _s3;
								const auto chaos_arg_2 = _s2;
								const auto chaos_arg_1 = _s1;
								const auto chaos_arg_0 = _s0;
								auto& _d3 = s_hotpatch_entries[3];
								CHAOS_IL2CPP_INT32 _d_hpresult{};
								if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d3)
									&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d3))
								{
									alignas(16) uint8_t _d_ab[20];
									ArgBuffer _d_bw(_d_ab);
									_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
									_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
									_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2));
									_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_3));
									::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
										_d3.method_key, _d_ab, &_d_hpresult);
								}
								else
								{
									_d_hpresult = RcwBasicSubjects_RcwBasicNativeEntry_MarshalCallComMethod_System_IntPtr_System_Int32_System_Int32_System_Int32(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_3));
								}
								_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
							}
							_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(7);
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
							_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
							{
								if (_s0 == 0)
								{
									_s0 = chaos_locals[2];
									{
										const auto chaos_arg_0 = _s0;
										auto& _d9 = s_hotpatch_entries[9];
										if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d9)
											&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d9))
										{
											alignas(16) uint8_t _d_ab[8];
											ArgBuffer _d_bw(_d_ab);
											_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
											::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
												_d9.method_key, _d_ab, nullptr);
										}
										else
										{
											RcwBasicSubjects_RcwBasicNativeEntry_MarshalReleaseRcw_System_IntPtr(chaos_arg_0);
										}
									}
									_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
									chaos_locals[5] = _s0;
								}
								else
								{
									_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
									chaos_locals[5] = _s0;
								}
							}
						}
						else
						{
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
							chaos_locals[5] = _s0;
						}
					}
				}
				else
				{
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
					chaos_locals[5] = _s0;
				}
			}
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			chaos_locals[5] = _s0;
		}
		_s0 = chaos_locals[5];
		return static_cast<CHAOS_IL2CPP_INT32>(_s0);
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
		chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(chaos_arg_0);
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
extern "C" const int kAotMethodCount = 29;

extern "C" void ChaosJitRegisterAll() {}