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

namespace chaos::il2cpp::codegen::InterfaceDispatchSubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_InterfaceDispatchSubjects_IBase = static_cast<CHAOS_IL2CPP_INTPTR>(15449752221026509750ULL);
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_InterfaceDispatchSubjects_ICalculator = static_cast<CHAOS_IL2CPP_INTPTR>(771964806139090035ULL);
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_InterfaceDispatchSubjects_IDerived = static_cast<CHAOS_IL2CPP_INTPTR>(3305311391869622176ULL);
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_InterfaceDispatchSubjects_ISimple = static_cast<CHAOS_IL2CPP_INTPTR>(14787273767725517097ULL);
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_InterfaceDispatchSubjects_IWithDefault = static_cast<CHAOS_IL2CPP_INTPTR>(8355604655153802020ULL);
const void* chaos_vtable_InterfaceDispatchSubjects_CalcAdd[];
const void* chaos_vtable_InterfaceDispatchSubjects_CalcMul[];
const void* chaos_vtable_InterfaceDispatchSubjects_ImplDiamond[];
const void* chaos_vtable_InterfaceDispatchSubjects_ImplDiamond2[];
const void* chaos_vtable_InterfaceDispatchSubjects_ImplSimple[];
const void* chaos_vtable_InterfaceDispatchSubjects_ImplWithDefault[];
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Object = {{nullptr, nullptr, 15228727185366376748ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_INTPTR>(15228727185366376748ULL);static constexpr InterfaceMapEntry chaos_iface_map_InterfaceDispatchSubjects_CalcAdd[] = {
	{ chaos_type_id_InterfaceDispatchSubjects_ICalculator, 1, 2 }
};
inline TypeInfoV0 chaos_mt_InterfaceDispatchSubjects_CalcAdd = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_InterfaceDispatchSubjects_CalcAdd, 2898228154134497916ULL, 3u, 32, 1, 1}, {chaos_iface_map_InterfaceDispatchSubjects_CalcAdd, nullptr, 1, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_InterfaceDispatchSubjects_CalcAdd = static_cast<CHAOS_IL2CPP_INTPTR>(2898228154134497916ULL);static constexpr InterfaceMapEntry chaos_iface_map_InterfaceDispatchSubjects_CalcMul[] = {
	{ chaos_type_id_InterfaceDispatchSubjects_ICalculator, 1, 2 }
};
inline TypeInfoV0 chaos_mt_InterfaceDispatchSubjects_CalcMul = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_InterfaceDispatchSubjects_CalcMul, 13787125010580401065ULL, 3u, 32, 1, 1}, {chaos_iface_map_InterfaceDispatchSubjects_CalcMul, nullptr, 1, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_InterfaceDispatchSubjects_CalcMul = static_cast<CHAOS_IL2CPP_INTPTR>(13787125010580401065ULL);static constexpr InterfaceMapEntry chaos_iface_map_InterfaceDispatchSubjects_ImplDiamond[] = {
	{ chaos_type_id_InterfaceDispatchSubjects_IDerived, 4, 1 },
	{ chaos_type_id_InterfaceDispatchSubjects_IBase, 3, 1 }
};
inline TypeInfoV0 chaos_mt_InterfaceDispatchSubjects_ImplDiamond = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_InterfaceDispatchSubjects_ImplDiamond, 5829620332136686238ULL, 5u, 32, 1, 1}, {chaos_iface_map_InterfaceDispatchSubjects_ImplDiamond, nullptr, 2, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_InterfaceDispatchSubjects_ImplDiamond = static_cast<CHAOS_IL2CPP_INTPTR>(5829620332136686238ULL);static constexpr InterfaceMapEntry chaos_iface_map_InterfaceDispatchSubjects_ImplDiamond2[] = {
	{ chaos_type_id_InterfaceDispatchSubjects_IDerived, 4, 1 },
	{ chaos_type_id_InterfaceDispatchSubjects_IBase, 3, 1 }
};
inline TypeInfoV0 chaos_mt_InterfaceDispatchSubjects_ImplDiamond2 = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_InterfaceDispatchSubjects_ImplDiamond2, 16094583433855183428ULL, 5u, 32, 1, 1}, {chaos_iface_map_InterfaceDispatchSubjects_ImplDiamond2, nullptr, 2, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_InterfaceDispatchSubjects_ImplDiamond2 = static_cast<CHAOS_IL2CPP_INTPTR>(16094583433855183428ULL);static constexpr InterfaceMapEntry chaos_iface_map_InterfaceDispatchSubjects_ImplSimple[] = {
	{ chaos_type_id_InterfaceDispatchSubjects_ISimple, 5, 1 }
};
inline TypeInfoV0 chaos_mt_InterfaceDispatchSubjects_ImplSimple = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_InterfaceDispatchSubjects_ImplSimple, 17332692813202302180ULL, 6u, 32, 1, 1}, {chaos_iface_map_InterfaceDispatchSubjects_ImplSimple, nullptr, 1, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_InterfaceDispatchSubjects_ImplSimple = static_cast<CHAOS_IL2CPP_INTPTR>(17332692813202302180ULL);static constexpr InterfaceMapEntry chaos_iface_map_InterfaceDispatchSubjects_ImplWithDefault[] = {
	{ chaos_type_id_InterfaceDispatchSubjects_IWithDefault, 5, 1 }
};
inline TypeInfoV0 chaos_mt_InterfaceDispatchSubjects_ImplWithDefault = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_InterfaceDispatchSubjects_ImplWithDefault, 10131636431187859815ULL, 1u, 32, 1, 1}, {chaos_iface_map_InterfaceDispatchSubjects_ImplWithDefault, nullptr, 1, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_InterfaceDispatchSubjects_ImplWithDefault = static_cast<CHAOS_IL2CPP_INTPTR>(10131636431187859815ULL);inline TypeInfoV0 chaos_mt_InterfaceDispatchSubjects_InterfaceDispatchSubjects = {{nullptr, nullptr, 16224833485471026240ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_InterfaceDispatchSubjects_InterfaceDispatchSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(16224833485471026240ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_Assembly = {{nullptr, nullptr, 5474029880995115448ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_Assembly = static_cast<CHAOS_IL2CPP_INTPTR>(5474029880995115448ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName = {{nullptr, nullptr, 17082367815459723707ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_AssemblyName = static_cast<CHAOS_IL2CPP_INTPTR>(17082367815459723707ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo = {{nullptr, nullptr, 4137207361503509124ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_ConstructorInfo = static_cast<CHAOS_IL2CPP_INTPTR>(4137207361503509124ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo = {{nullptr, nullptr, 17040031516751226236ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_FieldInfo = static_cast<CHAOS_IL2CPP_INTPTR>(17040031516751226236ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo = {{nullptr, nullptr, 10748947813473285525ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_MethodInfo = static_cast<CHAOS_IL2CPP_INTPTR>(10748947813473285525ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_String = {{nullptr, nullptr, 1782325859292956794ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_String = static_cast<CHAOS_IL2CPP_INTPTR>(1782325859292956794ULL);inline TypeInfoV0 chaos_mt_InterfaceDispatchSubjects_IBase = {{nullptr, nullptr, 15449752221026509750ULL, 0u, 32, 3, 0}, {nullptr, nullptr, 0, 0, 0, 0}};
inline TypeInfoV0 chaos_mt_InterfaceDispatchSubjects_ICalculator = {{nullptr, nullptr, 771964806139090035ULL, 0u, 32, 3, 0}, {nullptr, nullptr, 0, 0, 0, 0}};
inline TypeInfoV0 chaos_mt_InterfaceDispatchSubjects_IDerived = {{nullptr, nullptr, 3305311391869622176ULL, 0u, 32, 3, 0}, {nullptr, nullptr, 0, 0, 0, 0}};
inline TypeInfoV0 chaos_mt_InterfaceDispatchSubjects_ISimple = {{nullptr, nullptr, 14787273767725517097ULL, 0u, 32, 3, 0}, {nullptr, nullptr, 0, 0, 0, 0}};
inline TypeInfoV0 chaos_mt_InterfaceDispatchSubjects_IWithDefault = {{nullptr, nullptr, 8355604655153802020ULL, 0u, 32, 3, 0}, {nullptr, nullptr, 0, 0, 0, 0}};

// ── Virtual method table arrays ──
extern "C" void InterfaceDispatchSubjects_CalcAdd__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_CalcAdd_Add_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_CalcAdd_Multiply_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
const void* chaos_vtable_InterfaceDispatchSubjects_CalcAdd[] =
{
	reinterpret_cast<void*>(InterfaceDispatchSubjects_CalcAdd__ctor),
	reinterpret_cast<void*>(InterfaceDispatchSubjects_CalcAdd_Add_System_Int32_System_Int32),
	reinterpret_cast<void*>(InterfaceDispatchSubjects_CalcAdd_Multiply_System_Int32_System_Int32),
};
extern "C" void InterfaceDispatchSubjects_CalcMul__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_CalcMul_Add_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_CalcMul_Multiply_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
const void* chaos_vtable_InterfaceDispatchSubjects_CalcMul[] =
{
	reinterpret_cast<void*>(InterfaceDispatchSubjects_CalcMul__ctor),
	reinterpret_cast<void*>(InterfaceDispatchSubjects_CalcMul_Add_System_Int32_System_Int32),
	reinterpret_cast<void*>(InterfaceDispatchSubjects_CalcMul_Multiply_System_Int32_System_Int32),
};
extern "C" void InterfaceDispatchSubjects_ImplDiamond__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplDiamond_BaseMethod(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplDiamond_DerivedMethod(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
const void* chaos_vtable_InterfaceDispatchSubjects_ImplDiamond[] =
{
	reinterpret_cast<void*>(InterfaceDispatchSubjects_ImplDiamond__ctor),
	nullptr,
	nullptr,
	reinterpret_cast<void*>(InterfaceDispatchSubjects_ImplDiamond_BaseMethod),
	reinterpret_cast<void*>(InterfaceDispatchSubjects_ImplDiamond_DerivedMethod),
};
extern "C" void InterfaceDispatchSubjects_ImplDiamond2__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplDiamond2_BaseMethod(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplDiamond2_DerivedMethod(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
const void* chaos_vtable_InterfaceDispatchSubjects_ImplDiamond2[] =
{
	reinterpret_cast<void*>(InterfaceDispatchSubjects_ImplDiamond2__ctor),
	nullptr,
	nullptr,
	reinterpret_cast<void*>(InterfaceDispatchSubjects_ImplDiamond2_BaseMethod),
	reinterpret_cast<void*>(InterfaceDispatchSubjects_ImplDiamond2_DerivedMethod),
};
extern "C" void InterfaceDispatchSubjects_ImplSimple__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplSimple_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
const void* chaos_vtable_InterfaceDispatchSubjects_ImplSimple[] =
{
	reinterpret_cast<void*>(InterfaceDispatchSubjects_ImplSimple__ctor),
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	reinterpret_cast<void*>(InterfaceDispatchSubjects_ImplSimple_GetValue),
};
extern "C" void InterfaceDispatchSubjects_ImplWithDefault__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
const void* chaos_vtable_InterfaceDispatchSubjects_ImplWithDefault[] =
{
	reinterpret_cast<void*>(InterfaceDispatchSubjects_ImplWithDefault__ctor),
};
static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_InterfaceDispatchSubjects_CalcAdd[] =
{
	{ 0x00000023u, reinterpret_cast<void*>(&InterfaceDispatchSubjects_CalcAdd__ctor) },
	{ 0x00000021u, reinterpret_cast<void*>(&InterfaceDispatchSubjects_CalcAdd_Add_System_Int32_System_Int32) },
	{ 0x00000022u, reinterpret_cast<void*>(&InterfaceDispatchSubjects_CalcAdd_Multiply_System_Int32_System_Int32) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_InterfaceDispatchSubjects_CalcMul[] =
{
	{ 0x00000026u, reinterpret_cast<void*>(&InterfaceDispatchSubjects_CalcMul__ctor) },
	{ 0x00000024u, reinterpret_cast<void*>(&InterfaceDispatchSubjects_CalcMul_Add_System_Int32_System_Int32) },
	{ 0x00000025u, reinterpret_cast<void*>(&InterfaceDispatchSubjects_CalcMul_Multiply_System_Int32_System_Int32) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_InterfaceDispatchSubjects_ImplDiamond[] =
{
	{ 0x00000029u, reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplDiamond__ctor) },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0x00000027u, reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplDiamond_BaseMethod) },
	{ 0x00000028u, reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplDiamond_DerivedMethod) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_InterfaceDispatchSubjects_ImplDiamond2[] =
{
	{ 0x0000002Cu, reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplDiamond2__ctor) },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0x0000002Au, reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplDiamond2_BaseMethod) },
	{ 0x0000002Bu, reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplDiamond2_DerivedMethod) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_InterfaceDispatchSubjects_ImplSimple[] =
{
	{ 0x0000001Fu, reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplSimple__ctor) },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0x0000001Eu, reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplSimple_GetValue) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_InterfaceDispatchSubjects_ImplWithDefault[] =
{
	{ 0x00000020u, reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplWithDefault__ctor) },
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

struct chaos_type_InterfaceDispatchSubjects_CalcAdd : public chaos_type_System_Private_CoreLib_System_Object
{
};

struct chaos_type_InterfaceDispatchSubjects_CalcMul : public chaos_type_System_Private_CoreLib_System_Object
{
};

struct chaos_type_InterfaceDispatchSubjects_ImplDiamond : public chaos_type_System_Private_CoreLib_System_Object
{
};

struct chaos_type_InterfaceDispatchSubjects_ImplDiamond2 : public chaos_type_System_Private_CoreLib_System_Object
{
};

struct chaos_type_InterfaceDispatchSubjects_ImplSimple : public chaos_type_System_Private_CoreLib_System_Object
{
};

struct chaos_type_InterfaceDispatchSubjects_ImplWithDefault : public chaos_type_System_Private_CoreLib_System_Object
{
};

struct chaos_type_InterfaceDispatchSubjects_InterfaceDispatchSubjects
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

CHAOS_IL2CPP_INT32 chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = 0;

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

extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_0(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_1(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_2(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_3(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_4(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_5(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_6(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_7(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_8(void);
extern "C" void InterfaceDispatchSubjects_CalcAdd__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_CalcAdd_Add_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_CalcAdd_Multiply_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" void InterfaceDispatchSubjects_CalcMul__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_CalcMul_Add_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_CalcMul_Multiply_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" void InterfaceDispatchSubjects_ImplDiamond__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplDiamond_BaseMethod(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplDiamond_DerivedMethod(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void InterfaceDispatchSubjects_ImplDiamond2__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplDiamond2_BaseMethod(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplDiamond2_DerivedMethod(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void InterfaceDispatchSubjects_ImplSimple__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplSimple_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void InterfaceDispatchSubjects_ImplWithDefault__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_Run_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestAsCheck(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondBase(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondDerived(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondMulti(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestIsCheck(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestMultiImplCalc(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplDefault(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplSimple(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_IWithDefault_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[35] = {
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
	2u,
	4u,
	4u,
	6u,
	8u,
	8u,
	8u,
	8u,
	8u,
	8u,
	8u,
	8u,
	8u,
	8u,
	9u,
	9u,
	9u,
	9u,
	9u,
	9u,
	9u,
	9u,
	9u,
	9u,
};

static constexpr struct {
	::ChaosAbiManifestV0 header;
	::ChaosAbiMethodEntryV0 entries[34];
	CHAOS_IL2CPP_UINT8 params[9];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		34u,
		9u,
		3015791750u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_0
		{ 0u, 0u },  // InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_1
		{ 0u, 0u },  // InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_2
		{ 0u, 0u },  // InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_3
		{ 0u, 0u },  // InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_4
		{ 0u, 0u },  // InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_5
		{ 0u, 0u },  // InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_6
		{ 0u, 0u },  // InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_7
		{ 0u, 0u },  // InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_8
		{ 0u, 0u },  // InterfaceDispatchSubjects_CalcAdd__ctor
		{ 1u, 2u },  // InterfaceDispatchSubjects_CalcAdd_Add_System_Int32_System_Int32
		{ 1u, 2u },  // InterfaceDispatchSubjects_CalcAdd_Multiply_System_Int32_System_Int32
		{ 0u, 0u },  // InterfaceDispatchSubjects_CalcMul__ctor
		{ 1u, 2u },  // InterfaceDispatchSubjects_CalcMul_Add_System_Int32_System_Int32
		{ 1u, 2u },  // InterfaceDispatchSubjects_CalcMul_Multiply_System_Int32_System_Int32
		{ 0u, 0u },  // InterfaceDispatchSubjects_ImplDiamond__ctor
		{ 1u, 0u },  // InterfaceDispatchSubjects_ImplDiamond_BaseMethod
		{ 1u, 0u },  // InterfaceDispatchSubjects_ImplDiamond_DerivedMethod
		{ 0u, 0u },  // InterfaceDispatchSubjects_ImplDiamond2__ctor
		{ 1u, 0u },  // InterfaceDispatchSubjects_ImplDiamond2_BaseMethod
		{ 1u, 0u },  // InterfaceDispatchSubjects_ImplDiamond2_DerivedMethod
		{ 0u, 0u },  // InterfaceDispatchSubjects_ImplSimple__ctor
		{ 1u, 0u },  // InterfaceDispatchSubjects_ImplSimple_GetValue
		{ 0u, 0u },  // InterfaceDispatchSubjects_ImplWithDefault__ctor
		{ 1u, 1u },  // InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_Run_System_Int32
		{ 1u, 0u },  // InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestAsCheck
		{ 1u, 0u },  // InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondBase
		{ 1u, 0u },  // InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondDerived
		{ 1u, 0u },  // InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondMulti
		{ 1u, 0u },  // InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestIsCheck
		{ 1u, 0u },  // InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestMultiImplCalc
		{ 1u, 0u },  // InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplDefault
		{ 1u, 0u },  // InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplSimple
		{ 1u, 0u },  // InterfaceDispatchSubjects_IWithDefault_GetValue
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

	static constexpr CHAOS_IL2CPP_UINT32 s_type_flags[14] = {
		0u,
		1548u,
		1556u,
		1556u,
		1556u,
		1556u,
		1556u,
		1536u,
		1536u,
		1536u,
		1536u,
		1536u,
		1536u,
		1548u,
	};

	static constexpr const char* s_type_names[14] = {
		"<Module>",
		"InterfaceDispatchSubjects",
		"ISimple",
		"IWithDefault",
		"ICalculator",
		"IBase",
		"IDerived",
		"ImplSimple",
		"ImplWithDefault",
		"CalcAdd",
		"CalcMul",
		"ImplDiamond",
		"ImplDiamond2",
		"InterfaceDispatchNativeEntry",
	};

	static constexpr const char* s_type_namespaces[14] = {
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

	static constexpr CHAOS_IL2CPP_UINT32 s_type_parent_tokens[14] = {
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

	static const TypeInfoHot* const s_type_info_ptrs[14] = {
		nullptr,
		&chaos_mt_InterfaceDispatchSubjects_InterfaceDispatchSubjects.hot,
		&chaos_mt_InterfaceDispatchSubjects_ISimple.hot,
		&chaos_mt_InterfaceDispatchSubjects_IWithDefault.hot,
		&chaos_mt_InterfaceDispatchSubjects_ICalculator.hot,
		&chaos_mt_InterfaceDispatchSubjects_IBase.hot,
		&chaos_mt_InterfaceDispatchSubjects_IDerived.hot,
		&chaos_mt_InterfaceDispatchSubjects_ImplSimple.hot,
		&chaos_mt_InterfaceDispatchSubjects_ImplWithDefault.hot,
		&chaos_mt_InterfaceDispatchSubjects_CalcAdd.hot,
		&chaos_mt_InterfaceDispatchSubjects_CalcMul.hot,
		&chaos_mt_InterfaceDispatchSubjects_ImplDiamond.hot,
		&chaos_mt_InterfaceDispatchSubjects_ImplDiamond2.hot,
		nullptr,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_offset[15] = {
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

	static constexpr CHAOS_IL2CPP_UINT32 s_generic_param_constraint_offset[15] = {
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
		/* .name_utf8         = */ "InterfaceDispatchSubjects",
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
		/* .type_count        = */ 14u,
	/* .custom_attribute_blob       = */ nullptr,  // Tier 1 — deferred
	/* .custom_attribute_offset     = */ nullptr,
	/* .custom_attribute_entity_count = */ 0u,
	/* .custom_attribute_materializer = */ nullptr,

		/* .abi_manifest      = */ s_abi_manifest,
	};
	static const CHAOS_IL2CPP_UINT32 s_native_aot_module_id =
		::chaos::il2cpp::runtime_core::RegisterModule("InterfaceDispatchSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[34] = {
	{ ".ctor", 0x00000023u, 0u },  // CalcAdd
	{ "Add", 0x00000021u, 2u },  // CalcAdd
	{ "Multiply", 0x00000022u, 2u },  // CalcAdd
	{ ".ctor", 0x00000026u, 0u },  // CalcMul
	{ "Add", 0x00000024u, 2u },  // CalcMul
	{ "Multiply", 0x00000025u, 2u },  // CalcMul
	{ "GetValue", 0x00000019u, 0u },  // IWithDefault
	{ ".ctor", 0x00000029u, 0u },  // ImplDiamond
	{ "BaseMethod", 0x00000027u, 0u },  // ImplDiamond
	{ "DerivedMethod", 0x00000028u, 0u },  // ImplDiamond
	{ ".ctor", 0x0000002Cu, 0u },  // ImplDiamond2
	{ "BaseMethod", 0x0000002Au, 0u },  // ImplDiamond2
	{ "DerivedMethod", 0x0000002Bu, 0u },  // ImplDiamond2
	{ ".ctor", 0x0000001Fu, 0u },  // ImplSimple
	{ "GetValue", 0x0000001Eu, 0u },  // ImplSimple
	{ ".ctor", 0x00000020u, 0u },  // ImplWithDefault
	{ "Run", 0x0000002Du, 1u },  // InterfaceDispatchNativeEntry
	{ "TestAsCheck", 0x00000032u, 0u },  // InterfaceDispatchNativeEntry
	{ "TestDiamondBase", 0x00000033u, 0u },  // InterfaceDispatchNativeEntry
	{ "TestDiamondDerived", 0x00000034u, 0u },  // InterfaceDispatchNativeEntry
	{ "TestDiamondMulti", 0x00000035u, 0u },  // InterfaceDispatchNativeEntry
	{ "TestIsCheck", 0x00000031u, 0u },  // InterfaceDispatchNativeEntry
	{ "TestMultiImplCalc", 0x00000030u, 0u },  // InterfaceDispatchNativeEntry
	{ "TestSingleImplDefault", 0x0000002Fu, 0u },  // InterfaceDispatchNativeEntry
	{ "TestSingleImplSimple", 0x0000002Eu, 0u },  // InterfaceDispatchNativeEntry
	{ "Subject_0", 0x0000000Fu, 0u },  // InterfaceDispatchSubjects
	{ "Subject_1", 0x00000010u, 0u },  // InterfaceDispatchSubjects
	{ "Subject_2", 0x00000011u, 0u },  // InterfaceDispatchSubjects
	{ "Subject_3", 0x00000012u, 0u },  // InterfaceDispatchSubjects
	{ "Subject_4", 0x00000013u, 0u },  // InterfaceDispatchSubjects
	{ "Subject_5", 0x00000014u, 0u },  // InterfaceDispatchSubjects
	{ "Subject_6", 0x00000015u, 0u },  // InterfaceDispatchSubjects
	{ "Subject_7", 0x00000016u, 0u },  // InterfaceDispatchSubjects
	{ "Subject_8", 0x00000017u, 0u },  // InterfaceDispatchSubjects
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[9] = {
	{ "CalcAdd", "", 0u, 3u },
	{ "CalcMul", "", 3u, 3u },
	{ "IWithDefault", "", 6u, 1u },
	{ "ImplDiamond", "", 7u, 3u },
	{ "ImplDiamond2", "", 10u, 3u },
	{ "ImplSimple", "", 13u, 2u },
	{ "ImplWithDefault", "", 15u, 1u },
	{ "InterfaceDispatchNativeEntry", "", 16u, 9u },
	{ "InterfaceDispatchSubjects", "", 25u, 9u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[34] = {
	{ 0x0000000Fu, 0u },
	{ 0x00000010u, 1u },
	{ 0x00000011u, 2u },
	{ 0x00000012u, 3u },
	{ 0x00000013u, 4u },
	{ 0x00000014u, 5u },
	{ 0x00000015u, 6u },
	{ 0x00000016u, 7u },
	{ 0x00000017u, 8u },
	{ 0x00000019u, 33u },
	{ 0x0000001Eu, 22u },
	{ 0x0000001Fu, 21u },
	{ 0x00000020u, 23u },
	{ 0x00000021u, 10u },
	{ 0x00000022u, 11u },
	{ 0x00000023u, 9u },
	{ 0x00000024u, 13u },
	{ 0x00000025u, 14u },
	{ 0x00000026u, 12u },
	{ 0x00000027u, 16u },
	{ 0x00000028u, 17u },
	{ 0x00000029u, 15u },
	{ 0x0000002Au, 19u },
	{ 0x0000002Bu, 20u },
	{ 0x0000002Cu, 18u },
	{ 0x0000002Du, 24u },
	{ 0x0000002Eu, 32u },
	{ 0x0000002Fu, 31u },
	{ 0x00000030u, 30u },
	{ 0x00000031u, 29u },
	{ 0x00000032u, 25u },
	{ 0x00000033u, 26u },
	{ 0x00000034u, 27u },
	{ 0x00000035u, 28u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[34] = {
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // InterfaceDispatchSubjects::Subject_0
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // InterfaceDispatchSubjects::Subject_1
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // InterfaceDispatchSubjects::Subject_2
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // InterfaceDispatchSubjects::Subject_3
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // InterfaceDispatchSubjects::Subject_4
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // InterfaceDispatchSubjects::Subject_5
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // InterfaceDispatchSubjects::Subject_6
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // InterfaceDispatchSubjects::Subject_7
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // InterfaceDispatchSubjects::Subject_8
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_CalcAdd__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // CalcAdd::.ctor
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_CalcAdd_Add_System_Int32_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // CalcAdd::Add
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_CalcAdd_Multiply_System_Int32_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // CalcAdd::Multiply
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_CalcMul__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // CalcMul::.ctor
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_CalcMul_Add_System_Int32_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // CalcMul::Add
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_CalcMul_Multiply_System_Int32_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // CalcMul::Multiply
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplDiamond__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ImplDiamond::.ctor
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplDiamond_BaseMethod), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ImplDiamond::BaseMethod
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplDiamond_DerivedMethod), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ImplDiamond::DerivedMethod
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplDiamond2__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ImplDiamond2::.ctor
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplDiamond2_BaseMethod), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ImplDiamond2::BaseMethod
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplDiamond2_DerivedMethod), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ImplDiamond2::DerivedMethod
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplSimple__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ImplSimple::.ctor
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplSimple_GetValue), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ImplSimple::GetValue
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplWithDefault__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ImplWithDefault::.ctor
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_Run_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // InterfaceDispatchNativeEntry::Run
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestAsCheck), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // InterfaceDispatchNativeEntry::TestAsCheck
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondBase), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // InterfaceDispatchNativeEntry::TestDiamondBase
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondDerived), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // InterfaceDispatchNativeEntry::TestDiamondDerived
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondMulti), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // InterfaceDispatchNativeEntry::TestDiamondMulti
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestIsCheck), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // InterfaceDispatchNativeEntry::TestIsCheck
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestMultiImplCalc), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // InterfaceDispatchNativeEntry::TestMultiImplCalc
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplDefault), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // InterfaceDispatchNativeEntry::TestSingleImplDefault
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplSimple), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // InterfaceDispatchNativeEntry::TestSingleImplSimple
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_IWithDefault_GetValue), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // IWithDefault::GetValue
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"InterfaceDispatchSubjects",
	s_hotpatch_types,
	9u,
	s_hotpatch_methods,
	34u,
	s_hotpatch_slots,
	34u,
	s_hotpatch_entries,
	34u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[4] = {
	"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode",
	"System.Private.CoreLib/System.Object::.ctor:System.Void()",
	"InterfaceDispatchSubjects/ISimple",
	"InterfaceDispatchSubjects/ICalculator",
};

extern "C" void* kChaosExternalRuntimeFnTable[4] = {
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__),
	nullptr,
	nullptr,
};

extern "C" int32_t kChaosExternalRuntimeCount = 4;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[34])() = {
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_0),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_1),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_2),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_3),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_4),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_5),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_6),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_7),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_8),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_CalcAdd__ctor),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_CalcAdd_Add_System_Int32_System_Int32),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_CalcAdd_Multiply_System_Int32_System_Int32),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_CalcMul__ctor),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_CalcMul_Add_System_Int32_System_Int32),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_CalcMul_Multiply_System_Int32_System_Int32),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_ImplDiamond__ctor),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_ImplDiamond_BaseMethod),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_ImplDiamond_DerivedMethod),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_ImplDiamond2__ctor),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_ImplDiamond2_BaseMethod),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_ImplDiamond2_DerivedMethod),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_ImplSimple__ctor),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_ImplSimple_GetValue),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_ImplWithDefault__ctor),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_Run_System_Int32),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestAsCheck),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondBase),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondDerived),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondMulti),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestIsCheck),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestMultiImplCalc),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplDefault),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplSimple),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_IWithDefault_GetValue),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
static void (*kBenchmarkWrappers[34])() = {
	[]() {kAotMethods[0]();},
	[]() {kAotMethods[1]();},
	[]() {kAotMethods[2]();},
	[]() {kAotMethods[3]();},
	[]() {kAotMethods[4]();},
	[]() {kAotMethods[5]();},
	[]() {kAotMethods[6]();},
	[]() {kAotMethods[7]();},
	[]() {kAotMethods[8]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[9])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[10])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0,0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[11])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0,0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[12])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[13])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0,0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[14])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0,0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[15])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[16])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[17])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[18])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[19])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[20])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[21])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[22])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[23])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[24])(0);},
	[]() {kAotMethods[25]();},
	[]() {kAotMethods[26]();},
	[]() {kAotMethods[27]();},
	[]() {kAotMethods[28]();},
	[]() {kAotMethods[29]();},
	[]() {kAotMethods[30]();},
	[]() {kAotMethods[31]();},
	[]() {kAotMethods[32]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[33])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
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
static void* const kMethodPointers[34] = {
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_0),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_1),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_2),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_3),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_4),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_5),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_6),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_7),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_8),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_CalcAdd__ctor),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_CalcAdd_Add_System_Int32_System_Int32),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_CalcAdd_Multiply_System_Int32_System_Int32),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_CalcMul__ctor),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_CalcMul_Add_System_Int32_System_Int32),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_CalcMul_Multiply_System_Int32_System_Int32),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplDiamond__ctor),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplDiamond_BaseMethod),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplDiamond_DerivedMethod),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplDiamond2__ctor),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplDiamond2_BaseMethod),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplDiamond2_DerivedMethod),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplSimple__ctor),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplSimple_GetValue),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplWithDefault__ctor),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_Run_System_Int32),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestAsCheck),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondBase),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondDerived),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondMulti),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestIsCheck),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestMultiImplCalc),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplDefault),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplSimple),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_IWithDefault_GetValue),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
// ── VTable descriptors (for BootstrapRuntime TypeVTable registration) ──
static const VTableDescriptorV0 kChaosVTableDescriptors[] = {
	{
		CHAOS_IL2CPP_UINT64_C(0x2838932695390A7C),
		0x00000009u,
		0u,
		3u,
		reinterpret_cast<const void*>(kSlots_InterfaceDispatchSubjects_CalcAdd),
		reinterpret_cast<const void**>(chaos_vtable_InterfaceDispatchSubjects_CalcAdd),
		3u,
		1,
		{0, 0, 0},
		chaos_iface_map_InterfaceDispatchSubjects_CalcAdd,
		1u,
	},
	{
		CHAOS_IL2CPP_UINT64_C(0xBF55B526598C6BA9),
		0x0000000Au,
		0u,
		3u,
		reinterpret_cast<const void*>(kSlots_InterfaceDispatchSubjects_CalcMul),
		reinterpret_cast<const void**>(chaos_vtable_InterfaceDispatchSubjects_CalcMul),
		3u,
		1,
		{0, 0, 0},
		chaos_iface_map_InterfaceDispatchSubjects_CalcMul,
		1u,
	},
	{
		CHAOS_IL2CPP_UINT64_C(0x50E6F8CE3066E29E),
		0x0000000Bu,
		0u,
		5u,
		reinterpret_cast<const void*>(kSlots_InterfaceDispatchSubjects_ImplDiamond),
		reinterpret_cast<const void**>(chaos_vtable_InterfaceDispatchSubjects_ImplDiamond),
		5u,
		1,
		{0, 0, 0},
		chaos_iface_map_InterfaceDispatchSubjects_ImplDiamond,
		2u,
	},
	{
		CHAOS_IL2CPP_UINT64_C(0xDF5B725C3ED32A44),
		0x0000000Cu,
		0u,
		5u,
		reinterpret_cast<const void*>(kSlots_InterfaceDispatchSubjects_ImplDiamond2),
		reinterpret_cast<const void**>(chaos_vtable_InterfaceDispatchSubjects_ImplDiamond2),
		5u,
		1,
		{0, 0, 0},
		chaos_iface_map_InterfaceDispatchSubjects_ImplDiamond2,
		2u,
	},
	{
		CHAOS_IL2CPP_UINT64_C(0xF08A1847F3E3A4E4),
		0x00000007u,
		0u,
		6u,
		reinterpret_cast<const void*>(kSlots_InterfaceDispatchSubjects_ImplSimple),
		reinterpret_cast<const void**>(chaos_vtable_InterfaceDispatchSubjects_ImplSimple),
		6u,
		1,
		{0, 0, 0},
		chaos_iface_map_InterfaceDispatchSubjects_ImplSimple,
		1u,
	},
	{
		CHAOS_IL2CPP_UINT64_C(0x8C9ACDA78A5BD967),
		0x00000008u,
		0u,
		1u,
		reinterpret_cast<const void*>(kSlots_InterfaceDispatchSubjects_ImplWithDefault),
		reinterpret_cast<const void**>(chaos_vtable_InterfaceDispatchSubjects_ImplWithDefault),
		1u,
		1,
		{0, 0, 0},
		chaos_iface_map_InterfaceDispatchSubjects_ImplWithDefault,
		1u,
	},
};
extern "C" const CodeRegistrationV0 chaos_codegen_code_registration
	= {
	.struct_size               = sizeof(CodeRegistrationV0),
	.method_pointers           = kMethodPointers,
	.method_pointer_count      = 34u,
	.reverse_pinvoke_wrappers  = nullptr,
	.reverse_pinvoke_wrapper_count = 0u,
	.invoker_pointers          = nullptr,
	.invoker_pointer_count     = 0u,
	.unresolved_virtual_calls = nullptr,
	.unresolved_virtual_call_count = 0u,
	.type_capabilities       = nullptr,
	.type_capability_count   = 0u,
	.vtable_descriptors = kChaosVTableDescriptors,
	.vtable_descriptor_count = 6u,
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
	.image_name_utf8    = "InterfaceDispatchSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_InterfaceDispatchSubjects_InterfaceDispatchSubjects[1] = {
	{ 0u, "InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode", "_exitCode", "System.Int32", 0LL },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_InterfaceDispatchSubjects_InterfaceDispatchSubjects[9] = {
	{ 0u, "InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_0:System.Void()", "Subject_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_1:System.Void()", "Subject_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_2:System.Void()", "Subject_2", "System.Void", 0, nullptr, 0u },
	{ 0u, "InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_3:System.Void()", "Subject_3", "System.Void", 0, nullptr, 0u },
	{ 0u, "InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_4:System.Void()", "Subject_4", "System.Void", 0, nullptr, 0u },
	{ 0u, "InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_5:System.Void()", "Subject_5", "System.Void", 0, nullptr, 0u },
	{ 0u, "InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_6:System.Void()", "Subject_6", "System.Void", 0, nullptr, 0u },
	{ 0u, "InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_7:System.Void()", "Subject_7", "System.Void", 0, nullptr, 0u },
	{ 0u, "InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_8:System.Void()", "Subject_8", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_InterfaceDispatchSubjects_CalcAdd[3] = {
	{ 0u, "InterfaceDispatchSubjects/CalcAdd::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "InterfaceDispatchSubjects/CalcAdd::Add:System.Int32(System.Int32,System.Int32)", "Add", "System.Void", 0, nullptr, 0u },
	{ 0u, "InterfaceDispatchSubjects/CalcAdd::Multiply:System.Int32(System.Int32,System.Int32)", "Multiply", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_InterfaceDispatchSubjects_CalcMul[3] = {
	{ 0u, "InterfaceDispatchSubjects/CalcMul::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "InterfaceDispatchSubjects/CalcMul::Add:System.Int32(System.Int32,System.Int32)", "Add", "System.Void", 0, nullptr, 0u },
	{ 0u, "InterfaceDispatchSubjects/CalcMul::Multiply:System.Int32(System.Int32,System.Int32)", "Multiply", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_InterfaceDispatchSubjects_ImplDiamond[3] = {
	{ 0u, "InterfaceDispatchSubjects/ImplDiamond::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "InterfaceDispatchSubjects/ImplDiamond::BaseMethod:System.Int32()", "BaseMethod", "System.Void", 0, nullptr, 0u },
	{ 0u, "InterfaceDispatchSubjects/ImplDiamond::DerivedMethod:System.Int32()", "DerivedMethod", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_InterfaceDispatchSubjects_ImplDiamond2[3] = {
	{ 0u, "InterfaceDispatchSubjects/ImplDiamond2::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "InterfaceDispatchSubjects/ImplDiamond2::BaseMethod:System.Int32()", "BaseMethod", "System.Void", 0, nullptr, 0u },
	{ 0u, "InterfaceDispatchSubjects/ImplDiamond2::DerivedMethod:System.Int32()", "DerivedMethod", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_InterfaceDispatchSubjects_ImplSimple[2] = {
	{ 0u, "InterfaceDispatchSubjects/ImplSimple::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "InterfaceDispatchSubjects/ImplSimple::GetValue:System.Int32()", "GetValue", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_InterfaceDispatchSubjects_ImplWithDefault[1] = {
	{ 0u, "InterfaceDispatchSubjects/ImplWithDefault::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_InterfaceDispatchSubjects_InterfaceDispatchNativeEntry[9] = {
	{ 0u, "InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::Run:System.Int32(System.Int32)", "Run", "System.Void", 0, nullptr, 0u },
	{ 0u, "InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestAsCheck:System.Int32()", "TestAsCheck", "System.Void", 0, nullptr, 0u },
	{ 0u, "InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondBase:System.Int32()", "TestDiamondBase", "System.Void", 0, nullptr, 0u },
	{ 0u, "InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondDerived:System.Int32()", "TestDiamondDerived", "System.Void", 0, nullptr, 0u },
	{ 0u, "InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondMulti:System.Int32()", "TestDiamondMulti", "System.Void", 0, nullptr, 0u },
	{ 0u, "InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestIsCheck:System.Int32()", "TestIsCheck", "System.Void", 0, nullptr, 0u },
	{ 0u, "InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestMultiImplCalc:System.Int32()", "TestMultiImplCalc", "System.Void", 0, nullptr, 0u },
	{ 0u, "InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestSingleImplDefault:System.Int32()", "TestSingleImplDefault", "System.Void", 0, nullptr, 0u },
	{ 0u, "InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestSingleImplSimple:System.Int32()", "TestSingleImplSimple", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_InterfaceDispatchSubjects_IWithDefault[1] = {
	{ 0u, "InterfaceDispatchSubjects/IWithDefault::GetValue:System.Int32()", "GetValue", "System.Void", 0, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[9] = {
	{ 0u, "InterfaceDispatchSubjects/InterfaceDispatchSubjects", "InterfaceDispatchSubjects/InterfaceDispatchSubjects", "", "InterfaceDispatchSubjects", "InterfaceDispatchSubjects", nullptr, kReflFields_InterfaceDispatchSubjects_InterfaceDispatchSubjects, 1u, nullptr, 0u,
	kReflMethods_InterfaceDispatchSubjects_InterfaceDispatchSubjects, 9u },
	{ 0u, "InterfaceDispatchSubjects/CalcAdd", "InterfaceDispatchSubjects/CalcAdd", "", "CalcAdd", "CalcAdd", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_InterfaceDispatchSubjects_CalcAdd, 3u },
	{ 0u, "InterfaceDispatchSubjects/CalcMul", "InterfaceDispatchSubjects/CalcMul", "", "CalcMul", "CalcMul", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_InterfaceDispatchSubjects_CalcMul, 3u },
	{ 0u, "InterfaceDispatchSubjects/ImplDiamond", "InterfaceDispatchSubjects/ImplDiamond", "", "ImplDiamond", "ImplDiamond", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_InterfaceDispatchSubjects_ImplDiamond, 3u },
	{ 0u, "InterfaceDispatchSubjects/ImplDiamond2", "InterfaceDispatchSubjects/ImplDiamond2", "", "ImplDiamond2", "ImplDiamond2", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_InterfaceDispatchSubjects_ImplDiamond2, 3u },
	{ 0u, "InterfaceDispatchSubjects/ImplSimple", "InterfaceDispatchSubjects/ImplSimple", "", "ImplSimple", "ImplSimple", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_InterfaceDispatchSubjects_ImplSimple, 2u },
	{ 0u, "InterfaceDispatchSubjects/ImplWithDefault", "InterfaceDispatchSubjects/ImplWithDefault", "", "ImplWithDefault", "ImplWithDefault", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_InterfaceDispatchSubjects_ImplWithDefault, 1u },
	{ 0u, "InterfaceDispatchSubjects/InterfaceDispatchNativeEntry", "InterfaceDispatchSubjects/InterfaceDispatchNativeEntry", "", "InterfaceDispatchNativeEntry", "InterfaceDispatchNativeEntry", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_InterfaceDispatchSubjects_InterfaceDispatchNativeEntry, 9u },
	{ 0u, "InterfaceDispatchSubjects/IWithDefault", "InterfaceDispatchSubjects/IWithDefault", "", "IWithDefault", "IWithDefault", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_InterfaceDispatchSubjects_IWithDefault, 1u },
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

static const ReflectionQueryImageDescriptor kReflImage = { "InterfaceDispatchSubjects", kReflTypePtrs, 9u };

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
// Managed method: InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_0()
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_0(void)
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
				chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = chaos_value;
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
				chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = chaos_value;
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
				chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	return;
}

// Managed method: InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_1()
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_1(void)
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
				chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = chaos_value;
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
				chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = chaos_value;
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
				chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	return;
}

// Managed method: InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_2()
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_2(void)
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
				chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = chaos_value;
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
				chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = chaos_value;
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
				chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	return;
}

// Managed method: InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_3()
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_3(void)
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
				chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = chaos_value;
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
				chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = chaos_value;
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
				chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	return;
}

// Managed method: InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_4()
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_4(void)
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
				chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = chaos_value;
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
				chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = chaos_value;
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
				chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	return;
}

// Managed method: InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_5()
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_5(void)
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
				chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = chaos_value;
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
				chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = chaos_value;
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
				chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	return;
}

// Managed method: InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_6()
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_6(void)
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
				chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = chaos_value;
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
				chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = chaos_value;
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
				chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	return;
}

// Managed method: InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_7()
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_7(void)
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
				chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = chaos_value;
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
				chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = chaos_value;
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
				chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	return;
}

// Managed method: InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_8()
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_8(void)
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
				chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = chaos_value;
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
				chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = chaos_value;
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
				chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	return;
}

// Managed method: InterfaceDispatchSubjects/CalcAdd::.ctor()
extern "C" void InterfaceDispatchSubjects_CalcAdd__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: InterfaceDispatchSubjects/CalcAdd::Add(System.Int32,System.Int32)
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_CalcAdd_Add_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2)
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

// Managed method: InterfaceDispatchSubjects/CalcAdd::Multiply(System.Int32,System.Int32)
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_CalcAdd_Multiply_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2)
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

// Managed method: InterfaceDispatchSubjects/CalcMul::.ctor()
extern "C" void InterfaceDispatchSubjects_CalcMul__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: InterfaceDispatchSubjects/CalcMul::Add(System.Int32,System.Int32)
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_CalcMul_Add_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2)
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
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: InterfaceDispatchSubjects/CalcMul::Multiply(System.Int32,System.Int32)
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_CalcMul_Multiply_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2)
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

// Managed method: InterfaceDispatchSubjects/ImplDiamond::.ctor()
extern "C" void InterfaceDispatchSubjects_ImplDiamond__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: InterfaceDispatchSubjects/ImplDiamond::BaseMethod()
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplDiamond_BaseMethod(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(100);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: InterfaceDispatchSubjects/ImplDiamond::DerivedMethod()
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplDiamond_DerivedMethod(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(200);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: InterfaceDispatchSubjects/ImplDiamond2::.ctor()
extern "C" void InterfaceDispatchSubjects_ImplDiamond2__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: InterfaceDispatchSubjects/ImplDiamond2::BaseMethod()
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplDiamond2_BaseMethod(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(300);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: InterfaceDispatchSubjects/ImplDiamond2::DerivedMethod()
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplDiamond2_DerivedMethod(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(400);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: InterfaceDispatchSubjects/ImplSimple::.ctor()
extern "C" void InterfaceDispatchSubjects_ImplSimple__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: InterfaceDispatchSubjects/ImplSimple::GetValue()
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplSimple_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: InterfaceDispatchSubjects/ImplWithDefault::.ctor()
extern "C" void InterfaceDispatchSubjects_ImplWithDefault__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::Run(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_Run_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
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
	CHAOS_IL2CPP_INTPTR _s35{};
	CHAOS_IL2CPP_INTPTR _s36{};
	CHAOS_IL2CPP_INTPTR _s37{};
	CHAOS_IL2CPP_INTPTR _s38{};
	CHAOS_IL2CPP_INTPTR _s39{};
	CHAOS_IL2CPP_INTPTR _s40{};
	CHAOS_IL2CPP_INTPTR _s41{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 == 0)
		{
			_s0 = chaos_args[0];
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			{
				if (_s0 == 0)
				{
					_s0 = chaos_args[0];
					_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
					{
						if (_s0 == 0)
						{
							_s0 = chaos_args[0];
							_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
							{
								if (_s0 == 0)
								{
									_s0 = chaos_args[0];
									_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
									_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
									{
										if (_s0 == 0)
										{
											_s0 = chaos_args[0];
											_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
											_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
											{
												if (_s0 == 0)
												{
													_s0 = chaos_args[0];
													_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(6);
													_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
													{
														if (_s0 == 0)
														{
															_s0 = chaos_args[0];
															_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(7);
															_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
															{
																if (_s0 == 0)
																{
																	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1);
																	chaos_locals[1] = _s0;
																}
																else
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
																			_d_hpresult = InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondMulti();
																		}
																		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
																	}
																	chaos_locals[1] = _s0;
																}
															}
														}
														else
														{
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
																	_d_hpresult = InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondDerived();
																}
																_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
															}
															chaos_locals[1] = _s0;
														}
													}
												}
												else
												{
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
															_d_hpresult = InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondBase();
														}
														_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
													}
													chaos_locals[1] = _s0;
												}
											}
										}
										else
										{
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
													_d_hpresult = InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestAsCheck();
												}
												_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
											}
											chaos_locals[1] = _s0;
										}
									}
								}
								else
								{
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
											_d_hpresult = InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestIsCheck();
										}
										_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
									}
									chaos_locals[1] = _s0;
								}
							}
						}
						else
						{
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
									_d_hpresult = InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestMultiImplCalc();
								}
								_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
							}
							chaos_locals[1] = _s0;
						}
					}
				}
				else
				{
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
							_d_hpresult = InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplDefault();
						}
						_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
					}
					chaos_locals[1] = _s0;
				}
			}
		}
		else
		{
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
					_d_hpresult = InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplSimple();
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
			chaos_locals[1] = _s0;
		}
		_s0 = chaos_locals[1];
		return static_cast<CHAOS_IL2CPP_INT32>(_s0);
	}
}

// Managed method: InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestAsCheck()
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestAsCheck(void)
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


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_InterfaceDispatchSubjects_ImplSimple, {});
		chaos_object->header.type_info = &chaos_mt_InterfaceDispatchSubjects_ImplSimple.hot;
		InterfaceDispatchSubjects_ImplSimple__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	{
		const auto chaos_value = _s0;
		auto chaos_matches = false;
		if (chaos_value != 0)
		{
			auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_value);
			chaos_matches = chaos_does_type_implement_interface(chaos_object_get_type_info(chaos_header), &chaos_mt_InterfaceDispatchSubjects_ISimple.hot);
		}
		_s0 = chaos_matches ? chaos_value : 0;
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	_s1 = 0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s0) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 == 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1);
			chaos_locals[3] = _s0;
		}
		else
		{
			_s0 = chaos_locals[1];
			{
				const auto chaos_raw_arg_0 = _s0;
				const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				CHAOS_IL2CPP_INT32 chaos_callvirt_result{};
				chaos_callvirt_result = (*reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)>(chaos_vtable_resolve(chaos_object_get_type_info(reinterpret_cast<void*>(chaos_arg_0))->vtable_array, 5u)))(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_callvirt_result);
			}
			chaos_locals[3] = _s0;
		}
		_s0 = chaos_locals[3];
		return static_cast<CHAOS_IL2CPP_INT32>(_s0);
	}
}

// Managed method: InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondBase()
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondBase(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_InterfaceDispatchSubjects_ImplDiamond, {});
		chaos_object->header.type_info = &chaos_mt_InterfaceDispatchSubjects_ImplDiamond.hot;
		InterfaceDispatchSubjects_ImplDiamond__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
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
		chaos_callvirt_result = (*reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)>(chaos_vtable_resolve(chaos_object_get_type_info(reinterpret_cast<void*>(chaos_arg_0))->vtable_array, 3u)))(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_callvirt_result);
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondDerived()
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondDerived(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_InterfaceDispatchSubjects_ImplDiamond, {});
		chaos_object->header.type_info = &chaos_mt_InterfaceDispatchSubjects_ImplDiamond.hot;
		InterfaceDispatchSubjects_ImplDiamond__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
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
		chaos_callvirt_result = (*reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)>(chaos_vtable_resolve(chaos_object_get_type_info(reinterpret_cast<void*>(chaos_arg_0))->vtable_array, 4u)))(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_callvirt_result);
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondMulti()
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondMulti(void)
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
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_InterfaceDispatchSubjects_ImplDiamond, {});
		chaos_object->header.type_info = &chaos_mt_InterfaceDispatchSubjects_ImplDiamond.hot;
		InterfaceDispatchSubjects_ImplDiamond__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[0] = _s0;
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_InterfaceDispatchSubjects_ImplDiamond2, {});
		chaos_object->header.type_info = &chaos_mt_InterfaceDispatchSubjects_ImplDiamond2.hot;
		InterfaceDispatchSubjects_ImplDiamond2__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[0];
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		CHAOS_IL2CPP_INT32 chaos_callvirt_result{};
		chaos_callvirt_result = (*reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)>(chaos_vtable_resolve(chaos_object_get_type_info(reinterpret_cast<void*>(chaos_arg_0))->vtable_array, 3u)))(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_callvirt_result);
	}
	_s1 = chaos_locals[0];
	{
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		CHAOS_IL2CPP_INT32 chaos_callvirt_result{};
		chaos_callvirt_result = (*reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)>(chaos_vtable_resolve(chaos_object_get_type_info(reinterpret_cast<void*>(chaos_arg_0))->vtable_array, 4u)))(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_callvirt_result);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	_s1 = chaos_locals[1];
	{
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		CHAOS_IL2CPP_INT32 chaos_callvirt_result{};
		chaos_callvirt_result = (*reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)>(chaos_vtable_resolve(chaos_object_get_type_info(reinterpret_cast<void*>(chaos_arg_0))->vtable_array, 3u)))(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_callvirt_result);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	_s1 = chaos_locals[1];
	{
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		CHAOS_IL2CPP_INT32 chaos_callvirt_result{};
		chaos_callvirt_result = (*reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)>(chaos_vtable_resolve(chaos_object_get_type_info(reinterpret_cast<void*>(chaos_arg_0))->vtable_array, 4u)))(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_callvirt_result);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	chaos_locals[2] = _s0;
	_s0 = chaos_locals[2];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestIsCheck()
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestIsCheck(void)
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
	CHAOS_IL2CPP_INTPTR _s27{};
	CHAOS_IL2CPP_INTPTR _s28{};
	CHAOS_IL2CPP_INTPTR _s29{};
	CHAOS_IL2CPP_INTPTR _s30{};
	CHAOS_IL2CPP_INTPTR _s31{};


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_InterfaceDispatchSubjects_ImplSimple, {});
		chaos_object->header.type_info = &chaos_mt_InterfaceDispatchSubjects_ImplSimple.hot;
		InterfaceDispatchSubjects_ImplSimple__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[0] = _s0;
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_InterfaceDispatchSubjects_CalcAdd, {});
		chaos_object->header.type_info = &chaos_mt_InterfaceDispatchSubjects_CalcAdd.hot;
		InterfaceDispatchSubjects_CalcAdd__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[1] = _s0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[2] = _s0;
	_s0 = chaos_locals[0];
	{
		const auto chaos_value = _s0;
		auto chaos_matches = false;
		if (chaos_value != 0)
		{
			auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_value);
			chaos_matches = chaos_does_type_implement_interface(chaos_object_get_type_info(chaos_header), &chaos_mt_InterfaceDispatchSubjects_ISimple.hot);
		}
		_s0 = chaos_matches ? chaos_value : 0;
	}
	_s1 = 0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s0) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
		{
			_s0 = chaos_locals[2];
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(10);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
			chaos_locals[2] = _s0;
		}
		_s0 = chaos_locals[1];
		{
			const auto chaos_value = _s0;
			auto chaos_matches = false;
			if (chaos_value != 0)
			{
				auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_value);
				chaos_matches = chaos_does_type_implement_interface(chaos_object_get_type_info(chaos_header), &chaos_mt_InterfaceDispatchSubjects_ISimple.hot);
			}
			_s0 = chaos_matches ? chaos_value : 0;
		}
		_s1 = 0;
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s0) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) ? 1 : 0);
		{
			if (_s0 != 0)
			{
				_s0 = chaos_locals[2];
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(100);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
				chaos_locals[2] = _s0;
			}
			_s0 = chaos_locals[1];
			{
				const auto chaos_value = _s0;
				auto chaos_matches = false;
				if (chaos_value != 0)
				{
					auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_value);
					chaos_matches = chaos_does_type_implement_interface(chaos_object_get_type_info(chaos_header), &chaos_mt_InterfaceDispatchSubjects_ICalculator.hot);
				}
				_s0 = chaos_matches ? chaos_value : 0;
			}
			_s1 = 0;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s0) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) ? 1 : 0);
			{
				if (_s0 != 0)
				{
					_s0 = chaos_locals[2];
					_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1000);
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
					chaos_locals[2] = _s0;
				}
				_s0 = chaos_locals[2];
				chaos_locals[6] = _s0;
				_s0 = chaos_locals[6];
				return static_cast<CHAOS_IL2CPP_INT32>(_s0);
			}
		}
	}
}

// Managed method: InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestMultiImplCalc()
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestMultiImplCalc(void)
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
	CHAOS_IL2CPP_INTPTR _s11{};


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_InterfaceDispatchSubjects_CalcAdd, {});
		chaos_object->header.type_info = &chaos_mt_InterfaceDispatchSubjects_CalcAdd.hot;
		InterfaceDispatchSubjects_CalcAdd__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[0] = _s0;
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_InterfaceDispatchSubjects_CalcMul, {});
		chaos_object->header.type_info = &chaos_mt_InterfaceDispatchSubjects_CalcMul.hot;
		InterfaceDispatchSubjects_CalcMul__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[1] = _s0;
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
		chaos_callvirt_result = (*reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2)>(chaos_vtable_resolve(chaos_object_get_type_info(reinterpret_cast<void*>(chaos_arg_0))->vtable_array, 1u)))(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2));
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_callvirt_result);
	}
	_s1 = chaos_locals[1];
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
	{
		const auto chaos_raw_arg_2 = _s3;
		const auto chaos_arg_2 = chaos_raw_arg_2;
		const auto chaos_raw_arg_1 = _s2;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		CHAOS_IL2CPP_INT32 chaos_callvirt_result{};
		chaos_callvirt_result = (*reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2)>(chaos_vtable_resolve(chaos_object_get_type_info(reinterpret_cast<void*>(chaos_arg_0))->vtable_array, 2u)))(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2));
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_callvirt_result);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	chaos_locals[2] = _s0;
	_s0 = chaos_locals[2];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestSingleImplDefault()
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplDefault(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_InterfaceDispatchSubjects_ImplWithDefault, {});
		chaos_object->header.type_info = &chaos_mt_InterfaceDispatchSubjects_ImplWithDefault.hot;
		InterfaceDispatchSubjects_ImplWithDefault__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
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
		chaos_callvirt_result = (*reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)>(chaos_vtable_resolve(chaos_object_get_type_info(reinterpret_cast<void*>(chaos_arg_0))->vtable_array, 5u)))(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_callvirt_result);
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestSingleImplSimple()
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplSimple(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_InterfaceDispatchSubjects_ImplSimple, {});
		chaos_object->header.type_info = &chaos_mt_InterfaceDispatchSubjects_ImplSimple.hot;
		InterfaceDispatchSubjects_ImplSimple__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
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
		chaos_callvirt_result = (*reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)>(chaos_vtable_resolve(chaos_object_get_type_info(reinterpret_cast<void*>(chaos_arg_0))->vtable_array, 5u)))(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_callvirt_result);
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: InterfaceDispatchSubjects/IWithDefault::GetValue()
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_IWithDefault_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}



}  // namespace chaos::il2cpp::codegen::InterfaceDispatchSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 34;

// ── JIT Method Entry Table ───────────────────────────────────────────
// Auto-generated by chaos-il2cpp codegen for --mode jit.
#include <cstdint>
#include "jit_registration.h"

static const char kMethodJson_0[] = "{\"methodId\":\"interface-dispatch-subjects.interface-dispatch-subjects.subject-0\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_0:System.Void()\",\"signature\":\"System.Void InterfaceDispatchSubjects::Subject_0()\",\"identity\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects\",\"definitionSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_0:System.Void()\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_0:System.Void()\",\"methodId\":\"interface-dispatch-subjects.interface-dispatch-subjects.subject-0\",\"signature\":\"System.Void InterfaceDispatchSubjects::Subject_0()\"},\"nativeSymbol\":\"InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":33,\"handlerOffset\":34,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":4,\"resultType\":\"System.Int32\",\"callee\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::Run:System.Int32(System.Int32)\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::Run:System.Int32(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::Run:System.Int32(System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_Run_System_Int32\",\"targetParameterCount\":1,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":11,\"resultType\":\"System.Int32\",\"callee\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::Run:System.Int32(System.Int32)\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::Run:System.Int32(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::Run:System.Int32(System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_Run_System_Int32\",\"targetParameterCount\":1,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":16,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":19,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":22,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":31,\"ilOffset\":23,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\",\"ilOffset\":26,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":45,\"ilOffset\":32,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":34,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":36,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\",\"ilOffset\":37,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":45,\"ilOffset\":43,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":45,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"InterfaceDispatchSubjects\"}";
static const char kMethodJson_1[] = "{\"methodId\":\"interface-dispatch-subjects.interface-dispatch-subjects.subject-1\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_1:System.Void()\",\"signature\":\"System.Void InterfaceDispatchSubjects::Subject_1()\",\"identity\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects\",\"definitionSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_1:System.Void()\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_1:System.Void()\",\"methodId\":\"interface-dispatch-subjects.interface-dispatch-subjects.subject-1\",\"signature\":\"System.Void InterfaceDispatchSubjects::Subject_1()\"},\"nativeSymbol\":\"InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":29,\"handlerOffset\":30,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"call\",\"opCode\":14,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"callee\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestSingleImplSimple:System.Int32()\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestSingleImplSimple:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestSingleImplSimple:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplSimple\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"callee\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestSingleImplSimple:System.Int32()\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestSingleImplSimple:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestSingleImplSimple:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplSimple\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":17,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":27,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\",\"ilOffset\":22,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\",\"ilOffset\":33,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":41,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"InterfaceDispatchSubjects\"}";
static const char kMethodJson_2[] = "{\"methodId\":\"interface-dispatch-subjects.interface-dispatch-subjects.subject-2\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_2:System.Void()\",\"signature\":\"System.Void InterfaceDispatchSubjects::Subject_2()\",\"identity\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects\",\"definitionSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_2:System.Void()\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_2:System.Void()\",\"methodId\":\"interface-dispatch-subjects.interface-dispatch-subjects.subject-2\",\"signature\":\"System.Void InterfaceDispatchSubjects::Subject_2()\"},\"nativeSymbol\":\"InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_2\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":29,\"handlerOffset\":30,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"call\",\"opCode\":14,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"callee\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestSingleImplDefault:System.Int32()\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestSingleImplDefault:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestSingleImplDefault:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplDefault\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"callee\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestSingleImplDefault:System.Int32()\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestSingleImplDefault:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestSingleImplDefault:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplDefault\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":17,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":27,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\",\"ilOffset\":22,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\",\"ilOffset\":33,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":41,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"InterfaceDispatchSubjects\"}";
static const char kMethodJson_3[] = "{\"methodId\":\"interface-dispatch-subjects.interface-dispatch-subjects.subject-3\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_3:System.Void()\",\"signature\":\"System.Void InterfaceDispatchSubjects::Subject_3()\",\"identity\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects\",\"definitionSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_3:System.Void()\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_3:System.Void()\",\"methodId\":\"interface-dispatch-subjects.interface-dispatch-subjects.subject-3\",\"signature\":\"System.Void InterfaceDispatchSubjects::Subject_3()\"},\"nativeSymbol\":\"InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_3\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":29,\"handlerOffset\":30,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"call\",\"opCode\":14,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"callee\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestMultiImplCalc:System.Int32()\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestMultiImplCalc:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestMultiImplCalc:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestMultiImplCalc\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"callee\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestMultiImplCalc:System.Int32()\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestMultiImplCalc:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestMultiImplCalc:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestMultiImplCalc\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":17,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":27,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\",\"ilOffset\":22,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\",\"ilOffset\":33,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":41,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"InterfaceDispatchSubjects\"}";
static const char kMethodJson_4[] = "{\"methodId\":\"interface-dispatch-subjects.interface-dispatch-subjects.subject-4\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_4:System.Void()\",\"signature\":\"System.Void InterfaceDispatchSubjects::Subject_4()\",\"identity\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects\",\"definitionSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_4:System.Void()\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_4:System.Void()\",\"methodId\":\"interface-dispatch-subjects.interface-dispatch-subjects.subject-4\",\"signature\":\"System.Void InterfaceDispatchSubjects::Subject_4()\"},\"nativeSymbol\":\"InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_4\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":29,\"handlerOffset\":30,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"call\",\"opCode\":14,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"callee\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestIsCheck:System.Int32()\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestIsCheck:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestIsCheck:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestIsCheck\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"callee\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestIsCheck:System.Int32()\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestIsCheck:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestIsCheck:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestIsCheck\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":17,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":27,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\",\"ilOffset\":22,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\",\"ilOffset\":33,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":41,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"InterfaceDispatchSubjects\"}";
static const char kMethodJson_5[] = "{\"methodId\":\"interface-dispatch-subjects.interface-dispatch-subjects.subject-5\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_5:System.Void()\",\"signature\":\"System.Void InterfaceDispatchSubjects::Subject_5()\",\"identity\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects\",\"definitionSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_5:System.Void()\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_5:System.Void()\",\"methodId\":\"interface-dispatch-subjects.interface-dispatch-subjects.subject-5\",\"signature\":\"System.Void InterfaceDispatchSubjects::Subject_5()\"},\"nativeSymbol\":\"InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_5\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":29,\"handlerOffset\":30,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"call\",\"opCode\":14,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"callee\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestAsCheck:System.Int32()\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestAsCheck:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestAsCheck:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestAsCheck\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"callee\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestAsCheck:System.Int32()\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestAsCheck:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestAsCheck:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestAsCheck\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":17,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":27,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\",\"ilOffset\":22,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\",\"ilOffset\":33,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":41,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"InterfaceDispatchSubjects\"}";
static const char kMethodJson_6[] = "{\"methodId\":\"interface-dispatch-subjects.interface-dispatch-subjects.subject-6\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_6:System.Void()\",\"signature\":\"System.Void InterfaceDispatchSubjects::Subject_6()\",\"identity\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects\",\"definitionSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_6:System.Void()\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_6:System.Void()\",\"methodId\":\"interface-dispatch-subjects.interface-dispatch-subjects.subject-6\",\"signature\":\"System.Void InterfaceDispatchSubjects::Subject_6()\"},\"nativeSymbol\":\"InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_6\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":29,\"handlerOffset\":30,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"call\",\"opCode\":14,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"callee\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondBase:System.Int32()\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondBase:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondBase:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondBase\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"callee\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondBase:System.Int32()\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondBase:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondBase:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondBase\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":17,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":27,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\",\"ilOffset\":22,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\",\"ilOffset\":33,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":41,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"InterfaceDispatchSubjects\"}";
static const char kMethodJson_7[] = "{\"methodId\":\"interface-dispatch-subjects.interface-dispatch-subjects.subject-7\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_7:System.Void()\",\"signature\":\"System.Void InterfaceDispatchSubjects::Subject_7()\",\"identity\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects\",\"definitionSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_7:System.Void()\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_7:System.Void()\",\"methodId\":\"interface-dispatch-subjects.interface-dispatch-subjects.subject-7\",\"signature\":\"System.Void InterfaceDispatchSubjects::Subject_7()\"},\"nativeSymbol\":\"InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_7\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":29,\"handlerOffset\":30,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"call\",\"opCode\":14,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"callee\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondDerived:System.Int32()\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondDerived:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondDerived:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondDerived\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"callee\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondDerived:System.Int32()\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondDerived:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondDerived:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondDerived\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":17,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":27,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\",\"ilOffset\":22,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\",\"ilOffset\":33,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":41,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"InterfaceDispatchSubjects\"}";
static const char kMethodJson_8[] = "{\"methodId\":\"interface-dispatch-subjects.interface-dispatch-subjects.subject-8\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_8:System.Void()\",\"signature\":\"System.Void InterfaceDispatchSubjects::Subject_8()\",\"identity\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects\",\"definitionSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_8:System.Void()\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_8:System.Void()\",\"methodId\":\"interface-dispatch-subjects.interface-dispatch-subjects.subject-8\",\"signature\":\"System.Void InterfaceDispatchSubjects::Subject_8()\"},\"nativeSymbol\":\"InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_8\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":29,\"handlerOffset\":30,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"call\",\"opCode\":14,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"callee\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondMulti:System.Int32()\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondMulti:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondMulti:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondMulti\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"callee\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondMulti:System.Int32()\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondMulti:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondMulti:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondMulti\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":17,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":27,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\",\"ilOffset\":22,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\",\"ilOffset\":33,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":41,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"InterfaceDispatchSubjects\"}";
static const char kMethodJson_9[] = "{\"methodId\":\"interface-dispatch-subjects.calc-add.ctor\",\"subjectId\":\"InterfaceDispatchSubjects/CalcAdd::.ctor:System.Void()\",\"signature\":\"System.Void CalcAdd::.ctor()\",\"identity\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/CalcAdd\",\"definitionSubjectId\":\"InterfaceDispatchSubjects/CalcAdd::.ctor:System.Void()\",\"subjectId\":\"InterfaceDispatchSubjects/CalcAdd::.ctor:System.Void()\",\"methodId\":\"interface-dispatch-subjects.calc-add.ctor\",\"signature\":\"System.Void CalcAdd::.ctor()\"},\"nativeSymbol\":\"InterfaceDispatchSubjects_CalcAdd__ctor\",\"isStatic\":false,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"InterfaceDispatchSubjects/CalcAdd\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":7,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"InterfaceDispatchSubjects\"}";
static const char kMethodJson_10[] = "{\"methodId\":\"interface-dispatch-subjects.calc-add.add\",\"subjectId\":\"InterfaceDispatchSubjects/CalcAdd::Add:System.Int32(System.Int32,System.Int32)\",\"signature\":\"System.Int32 CalcAdd::Add(System.Int32,System.Int32)\",\"identity\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/CalcAdd\",\"definitionSubjectId\":\"InterfaceDispatchSubjects/CalcAdd::Add:System.Int32(System.Int32,System.Int32)\",\"subjectId\":\"InterfaceDispatchSubjects/CalcAdd::Add:System.Int32(System.Int32,System.Int32)\",\"methodId\":\"interface-dispatch-subjects.calc-add.add\",\"signature\":\"System.Int32 CalcAdd::Add(System.Int32,System.Int32)\"},\"nativeSymbol\":\"InterfaceDispatchSubjects_CalcAdd_Add_System_Int32_System_Int32\",\"isStatic\":false,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":2,\"parameterAbis\":[{\"carrierKindCode\":1,\"typeShape\":2},{\"carrierKindCode\":1,\"typeShape\":2}],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":0,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":2,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"add\",\"opCode\":25,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":3,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"InterfaceDispatchSubjects\"}";
static const char kMethodJson_11[] = "{\"methodId\":\"interface-dispatch-subjects.calc-add.multiply\",\"subjectId\":\"InterfaceDispatchSubjects/CalcAdd::Multiply:System.Int32(System.Int32,System.Int32)\",\"signature\":\"System.Int32 CalcAdd::Multiply(System.Int32,System.Int32)\",\"identity\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/CalcAdd\",\"definitionSubjectId\":\"InterfaceDispatchSubjects/CalcAdd::Multiply:System.Int32(System.Int32,System.Int32)\",\"subjectId\":\"InterfaceDispatchSubjects/CalcAdd::Multiply:System.Int32(System.Int32,System.Int32)\",\"methodId\":\"interface-dispatch-subjects.calc-add.multiply\",\"signature\":\"System.Int32 CalcAdd::Multiply(System.Int32,System.Int32)\"},\"nativeSymbol\":\"InterfaceDispatchSubjects_CalcAdd_Multiply_System_Int32_System_Int32\",\"isStatic\":false,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":2,\"parameterAbis\":[{\"carrierKindCode\":1,\"typeShape\":2},{\"carrierKindCode\":1,\"typeShape\":2}],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":0,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":2,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"mul\",\"opCode\":27,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":3,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"InterfaceDispatchSubjects\"}";
static const char kMethodJson_12[] = "{\"methodId\":\"interface-dispatch-subjects.calc-mul.ctor\",\"subjectId\":\"InterfaceDispatchSubjects/CalcMul::.ctor:System.Void()\",\"signature\":\"System.Void CalcMul::.ctor()\",\"identity\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/CalcMul\",\"definitionSubjectId\":\"InterfaceDispatchSubjects/CalcMul::.ctor:System.Void()\",\"subjectId\":\"InterfaceDispatchSubjects/CalcMul::.ctor:System.Void()\",\"methodId\":\"interface-dispatch-subjects.calc-mul.ctor\",\"signature\":\"System.Void CalcMul::.ctor()\"},\"nativeSymbol\":\"InterfaceDispatchSubjects_CalcMul__ctor\",\"isStatic\":false,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"InterfaceDispatchSubjects/CalcMul\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":7,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"InterfaceDispatchSubjects\"}";
static const char kMethodJson_13[] = "{\"methodId\":\"interface-dispatch-subjects.calc-mul.add\",\"subjectId\":\"InterfaceDispatchSubjects/CalcMul::Add:System.Int32(System.Int32,System.Int32)\",\"signature\":\"System.Int32 CalcMul::Add(System.Int32,System.Int32)\",\"identity\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/CalcMul\",\"definitionSubjectId\":\"InterfaceDispatchSubjects/CalcMul::Add:System.Int32(System.Int32,System.Int32)\",\"subjectId\":\"InterfaceDispatchSubjects/CalcMul::Add:System.Int32(System.Int32,System.Int32)\",\"methodId\":\"interface-dispatch-subjects.calc-mul.add\",\"signature\":\"System.Int32 CalcMul::Add(System.Int32,System.Int32)\"},\"nativeSymbol\":\"InterfaceDispatchSubjects_CalcMul_Add_System_Int32_System_Int32\",\"isStatic\":false,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":2,\"parameterAbis\":[{\"carrierKindCode\":1,\"typeShape\":2},{\"carrierKindCode\":1,\"typeShape\":2}],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":0,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":2,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"add\",\"opCode\":25,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"add\",\"opCode\":25,\"ilOffset\":4,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":5,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"InterfaceDispatchSubjects\"}";
static const char kMethodJson_14[] = "{\"methodId\":\"interface-dispatch-subjects.calc-mul.multiply\",\"subjectId\":\"InterfaceDispatchSubjects/CalcMul::Multiply:System.Int32(System.Int32,System.Int32)\",\"signature\":\"System.Int32 CalcMul::Multiply(System.Int32,System.Int32)\",\"identity\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/CalcMul\",\"definitionSubjectId\":\"InterfaceDispatchSubjects/CalcMul::Multiply:System.Int32(System.Int32,System.Int32)\",\"subjectId\":\"InterfaceDispatchSubjects/CalcMul::Multiply:System.Int32(System.Int32,System.Int32)\",\"methodId\":\"interface-dispatch-subjects.calc-mul.multiply\",\"signature\":\"System.Int32 CalcMul::Multiply(System.Int32,System.Int32)\"},\"nativeSymbol\":\"InterfaceDispatchSubjects_CalcMul_Multiply_System_Int32_System_Int32\",\"isStatic\":false,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":2,\"parameterAbis\":[{\"carrierKindCode\":1,\"typeShape\":2},{\"carrierKindCode\":1,\"typeShape\":2}],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":0,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":2,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"mul\",\"opCode\":27,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":2,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"mul\",\"opCode\":27,\"ilOffset\":4,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":5,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"InterfaceDispatchSubjects\"}";
static const char kMethodJson_15[] = "{\"methodId\":\"interface-dispatch-subjects.impl-diamond.ctor\",\"subjectId\":\"InterfaceDispatchSubjects/ImplDiamond::.ctor:System.Void()\",\"signature\":\"System.Void ImplDiamond::.ctor()\",\"identity\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/ImplDiamond\",\"definitionSubjectId\":\"InterfaceDispatchSubjects/ImplDiamond::.ctor:System.Void()\",\"subjectId\":\"InterfaceDispatchSubjects/ImplDiamond::.ctor:System.Void()\",\"methodId\":\"interface-dispatch-subjects.impl-diamond.ctor\",\"signature\":\"System.Void ImplDiamond::.ctor()\"},\"nativeSymbol\":\"InterfaceDispatchSubjects_ImplDiamond__ctor\",\"isStatic\":false,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"InterfaceDispatchSubjects/ImplDiamond\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":7,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"InterfaceDispatchSubjects\"}";
static const char kMethodJson_16[] = "{\"methodId\":\"interface-dispatch-subjects.impl-diamond.base-method\",\"subjectId\":\"InterfaceDispatchSubjects/ImplDiamond::BaseMethod:System.Int32()\",\"signature\":\"System.Int32 ImplDiamond::BaseMethod()\",\"identity\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/ImplDiamond\",\"definitionSubjectId\":\"InterfaceDispatchSubjects/ImplDiamond::BaseMethod:System.Int32()\",\"subjectId\":\"InterfaceDispatchSubjects/ImplDiamond::BaseMethod:System.Int32()\",\"methodId\":\"interface-dispatch-subjects.impl-diamond.base-method\",\"signature\":\"System.Int32 ImplDiamond::BaseMethod()\"},\"nativeSymbol\":\"InterfaceDispatchSubjects_ImplDiamond_BaseMethod\",\"isStatic\":false,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":100,\"ilOffset\":0,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":2,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"InterfaceDispatchSubjects\"}";
static const char kMethodJson_17[] = "{\"methodId\":\"interface-dispatch-subjects.impl-diamond.derived-method\",\"subjectId\":\"InterfaceDispatchSubjects/ImplDiamond::DerivedMethod:System.Int32()\",\"signature\":\"System.Int32 ImplDiamond::DerivedMethod()\",\"identity\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/ImplDiamond\",\"definitionSubjectId\":\"InterfaceDispatchSubjects/ImplDiamond::DerivedMethod:System.Int32()\",\"subjectId\":\"InterfaceDispatchSubjects/ImplDiamond::DerivedMethod:System.Int32()\",\"methodId\":\"interface-dispatch-subjects.impl-diamond.derived-method\",\"signature\":\"System.Int32 ImplDiamond::DerivedMethod()\"},\"nativeSymbol\":\"InterfaceDispatchSubjects_ImplDiamond_DerivedMethod\",\"isStatic\":false,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":200,\"ilOffset\":0,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":5,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"InterfaceDispatchSubjects\"}";
static const char kMethodJson_18[] = "{\"methodId\":\"interface-dispatch-subjects.impl-diamond2.ctor\",\"subjectId\":\"InterfaceDispatchSubjects/ImplDiamond2::.ctor:System.Void()\",\"signature\":\"System.Void ImplDiamond2::.ctor()\",\"identity\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/ImplDiamond2\",\"definitionSubjectId\":\"InterfaceDispatchSubjects/ImplDiamond2::.ctor:System.Void()\",\"subjectId\":\"InterfaceDispatchSubjects/ImplDiamond2::.ctor:System.Void()\",\"methodId\":\"interface-dispatch-subjects.impl-diamond2.ctor\",\"signature\":\"System.Void ImplDiamond2::.ctor()\"},\"nativeSymbol\":\"InterfaceDispatchSubjects_ImplDiamond2__ctor\",\"isStatic\":false,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"InterfaceDispatchSubjects/ImplDiamond2\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":7,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"InterfaceDispatchSubjects\"}";
static const char kMethodJson_19[] = "{\"methodId\":\"interface-dispatch-subjects.impl-diamond2.base-method\",\"subjectId\":\"InterfaceDispatchSubjects/ImplDiamond2::BaseMethod:System.Int32()\",\"signature\":\"System.Int32 ImplDiamond2::BaseMethod()\",\"identity\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/ImplDiamond2\",\"definitionSubjectId\":\"InterfaceDispatchSubjects/ImplDiamond2::BaseMethod:System.Int32()\",\"subjectId\":\"InterfaceDispatchSubjects/ImplDiamond2::BaseMethod:System.Int32()\",\"methodId\":\"interface-dispatch-subjects.impl-diamond2.base-method\",\"signature\":\"System.Int32 ImplDiamond2::BaseMethod()\"},\"nativeSymbol\":\"InterfaceDispatchSubjects_ImplDiamond2_BaseMethod\",\"isStatic\":false,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":300,\"ilOffset\":0,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":5,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"InterfaceDispatchSubjects\"}";
static const char kMethodJson_20[] = "{\"methodId\":\"interface-dispatch-subjects.impl-diamond2.derived-method\",\"subjectId\":\"InterfaceDispatchSubjects/ImplDiamond2::DerivedMethod:System.Int32()\",\"signature\":\"System.Int32 ImplDiamond2::DerivedMethod()\",\"identity\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/ImplDiamond2\",\"definitionSubjectId\":\"InterfaceDispatchSubjects/ImplDiamond2::DerivedMethod:System.Int32()\",\"subjectId\":\"InterfaceDispatchSubjects/ImplDiamond2::DerivedMethod:System.Int32()\",\"methodId\":\"interface-dispatch-subjects.impl-diamond2.derived-method\",\"signature\":\"System.Int32 ImplDiamond2::DerivedMethod()\"},\"nativeSymbol\":\"InterfaceDispatchSubjects_ImplDiamond2_DerivedMethod\",\"isStatic\":false,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":400,\"ilOffset\":0,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":5,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"InterfaceDispatchSubjects\"}";
static const char kMethodJson_21[] = "{\"methodId\":\"interface-dispatch-subjects.impl-simple.ctor\",\"subjectId\":\"InterfaceDispatchSubjects/ImplSimple::.ctor:System.Void()\",\"signature\":\"System.Void ImplSimple::.ctor()\",\"identity\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/ImplSimple\",\"definitionSubjectId\":\"InterfaceDispatchSubjects/ImplSimple::.ctor:System.Void()\",\"subjectId\":\"InterfaceDispatchSubjects/ImplSimple::.ctor:System.Void()\",\"methodId\":\"interface-dispatch-subjects.impl-simple.ctor\",\"signature\":\"System.Void ImplSimple::.ctor()\"},\"nativeSymbol\":\"InterfaceDispatchSubjects_ImplSimple__ctor\",\"isStatic\":false,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"InterfaceDispatchSubjects/ImplSimple\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":7,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"InterfaceDispatchSubjects\"}";
static const char kMethodJson_22[] = "{\"methodId\":\"interface-dispatch-subjects.impl-simple.get-value\",\"subjectId\":\"InterfaceDispatchSubjects/ImplSimple::GetValue:System.Int32()\",\"signature\":\"System.Int32 ImplSimple::GetValue()\",\"identity\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/ImplSimple\",\"definitionSubjectId\":\"InterfaceDispatchSubjects/ImplSimple::GetValue:System.Int32()\",\"subjectId\":\"InterfaceDispatchSubjects/ImplSimple::GetValue:System.Int32()\",\"methodId\":\"interface-dispatch-subjects.impl-simple.get-value\",\"signature\":\"System.Int32 ImplSimple::GetValue()\"},\"nativeSymbol\":\"InterfaceDispatchSubjects_ImplSimple_GetValue\",\"isStatic\":false,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":0,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":2,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"InterfaceDispatchSubjects\"}";
static const char kMethodJson_23[] = "{\"methodId\":\"interface-dispatch-subjects.impl-with-default.ctor\",\"subjectId\":\"InterfaceDispatchSubjects/ImplWithDefault::.ctor:System.Void()\",\"signature\":\"System.Void ImplWithDefault::.ctor()\",\"identity\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/ImplWithDefault\",\"definitionSubjectId\":\"InterfaceDispatchSubjects/ImplWithDefault::.ctor:System.Void()\",\"subjectId\":\"InterfaceDispatchSubjects/ImplWithDefault::.ctor:System.Void()\",\"methodId\":\"interface-dispatch-subjects.impl-with-default.ctor\",\"signature\":\"System.Void ImplWithDefault::.ctor()\"},\"nativeSymbol\":\"InterfaceDispatchSubjects_ImplWithDefault__ctor\",\"isStatic\":false,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"InterfaceDispatchSubjects/ImplWithDefault\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":7,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"InterfaceDispatchSubjects\"}";
static const char kMethodJson_24[] = "{\"methodId\":\"interface-dispatch-subjects.interface-dispatch-native-entry.run\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::Run:System.Int32(System.Int32)\",\"signature\":\"System.Int32 InterfaceDispatchNativeEntry::Run(System.Int32)\",\"identity\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry\",\"definitionSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::Run:System.Int32(System.Int32)\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::Run:System.Int32(System.Int32)\",\"methodId\":\"interface-dispatch-subjects.interface-dispatch-native-entry.run\",\"signature\":\"System.Int32 InterfaceDispatchNativeEntry::Run(System.Int32)\"},\"nativeSymbol\":\"InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_Run_System_Int32\",\"isStatic\":true,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":1,\"parameterAbis\":[{\"carrierKindCode\":1,\"typeShape\":2}],\"localCount\":9,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":6,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":17,\"ilOffset\":7,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"callee\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestSingleImplSimple:System.Int32()\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestSingleImplSimple:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestSingleImplSimple:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplSimple\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":14,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":143,\"ilOffset\":15,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":19,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":22,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":33,\"ilOffset\":23,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"callee\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestSingleImplDefault:System.Int32()\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestSingleImplDefault:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestSingleImplDefault:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplDefault\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":30,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":143,\"ilOffset\":31,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":33,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":2,\"ilOffset\":34,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":35,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":3,\"ilOffset\":37,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":3,\"ilOffset\":38,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":49,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":41,\"resultType\":\"System.Int32\",\"callee\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestMultiImplCalc:System.Int32()\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestMultiImplCalc:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestMultiImplCalc:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestMultiImplCalc\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":46,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":143,\"ilOffset\":47,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":49,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":3,\"ilOffset\":50,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":51,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":4,\"ilOffset\":53,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":4,\"ilOffset\":55,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":67,\"ilOffset\":57,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":59,\"resultType\":\"System.Int32\",\"callee\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestIsCheck:System.Int32()\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestIsCheck:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestIsCheck:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestIsCheck\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":64,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":143,\"ilOffset\":65,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":67,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":4,\"ilOffset\":68,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":69,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":5,\"ilOffset\":71,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":5,\"ilOffset\":73,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":85,\"ilOffset\":75,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":77,\"resultType\":\"System.Int32\",\"callee\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestAsCheck:System.Int32()\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestAsCheck:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestAsCheck:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestAsCheck\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":82,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":143,\"ilOffset\":83,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":85,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":5,\"ilOffset\":86,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":87,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":6,\"ilOffset\":89,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":6,\"ilOffset\":91,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":103,\"ilOffset\":93,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":95,\"resultType\":\"System.Int32\",\"callee\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondBase:System.Int32()\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondBase:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondBase:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondBase\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":100,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":143,\"ilOffset\":101,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":103,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":6,\"ilOffset\":104,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":105,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":7,\"ilOffset\":107,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":7,\"ilOffset\":109,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":121,\"ilOffset\":111,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":113,\"resultType\":\"System.Int32\",\"callee\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondDerived:System.Int32()\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondDerived:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondDerived:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondDerived\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":118,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":143,\"ilOffset\":119,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":121,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":7,\"ilOffset\":122,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":123,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":8,\"ilOffset\":125,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":8,\"ilOffset\":127,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":139,\"ilOffset\":129,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":131,\"resultType\":\"System.Int32\",\"callee\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondMulti:System.Int32()\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondMulti:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondMulti:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondMulti\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":136,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":143,\"ilOffset\":137,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":-1,\"ilOffset\":139,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":140,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":143,\"ilOffset\":141,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":143,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":144,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"InterfaceDispatchSubjects\"}";
static const char kMethodJson_25[] = "{\"methodId\":\"interface-dispatch-subjects.interface-dispatch-native-entry.test-as-check\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestAsCheck:System.Int32()\",\"signature\":\"System.Int32 InterfaceDispatchNativeEntry::TestAsCheck()\",\"identity\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry\",\"definitionSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestAsCheck:System.Int32()\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestAsCheck:System.Int32()\",\"methodId\":\"interface-dispatch-subjects.interface-dispatch-native-entry.test-as-check\",\"signature\":\"System.Int32 InterfaceDispatchNativeEntry::TestAsCheck()\"},\"nativeSymbol\":\"InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestAsCheck\",\"isStatic\":true,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":4,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":1,\"resultType\":\"InterfaceDispatchSubjects/ImplSimple\",\"callee\":\"InterfaceDispatchSubjects/ImplSimple::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/ImplSimple::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/ImplSimple\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"baseTypeSubjectId\":\"System.Private.CoreLib/System.Object\",\"implementedInterfaceSubjectIds\":[\"InterfaceDispatchSubjects/ISimple\"],\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":6,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"isinst\",\"opCode\":38,\"operand\":\"InterfaceDispatchSubjects/ISimple\",\"ilOffset\":8,\"resultType\":\"InterfaceDispatchSubjects/ISimple\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"type\",\"subjectId\":\"InterfaceDispatchSubjects/ISimple\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/ISimple\",\"typeShape\":3,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"implementedInterfaceSubjectIds\":[],\"declaringTypeShape\":0},\"runtimeServiceKind\":8,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":14,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":15,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"cgt.un\",\"opCode\":33,\"ilOffset\":16,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":18,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":19,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":31,\"ilOffset\":20,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":22,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"callee\":\"InterfaceDispatchSubjects/ISimple::GetValue:System.Int32()\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/ISimple::GetValue:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/ISimple::GetValue:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"InterfaceDispatchSubjects_ISimple_GetValue\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":2,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":3,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":35,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":-1,\"ilOffset\":31,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":3,\"ilOffset\":32,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":35,\"ilOffset\":33,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":3,\"ilOffset\":35,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":36,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"InterfaceDispatchSubjects\"}";
static const char kMethodJson_26[] = "{\"methodId\":\"interface-dispatch-subjects.interface-dispatch-native-entry.test-diamond-base\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondBase:System.Int32()\",\"signature\":\"System.Int32 InterfaceDispatchNativeEntry::TestDiamondBase()\",\"identity\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry\",\"definitionSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondBase:System.Int32()\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondBase:System.Int32()\",\"methodId\":\"interface-dispatch-subjects.interface-dispatch-native-entry.test-diamond-base\",\"signature\":\"System.Int32 InterfaceDispatchNativeEntry::TestDiamondBase()\"},\"nativeSymbol\":\"InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondBase\",\"isStatic\":true,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":1,\"resultType\":\"InterfaceDispatchSubjects/ImplDiamond\",\"callee\":\"InterfaceDispatchSubjects/ImplDiamond::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/ImplDiamond::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/ImplDiamond\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"baseTypeSubjectId\":\"System.Private.CoreLib/System.Object\",\"implementedInterfaceSubjectIds\":[\"InterfaceDispatchSubjects/IBase\",\"InterfaceDispatchSubjects/IDerived\"],\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":6,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"callee\":\"InterfaceDispatchSubjects/IBase::BaseMethod:System.Int32()\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/IBase::BaseMethod:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/IBase::BaseMethod:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"InterfaceDispatchSubjects_IBase_BaseMethod\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":2,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":16,\"ilOffset\":14,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":16,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":17,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"InterfaceDispatchSubjects\"}";
static const char kMethodJson_27[] = "{\"methodId\":\"interface-dispatch-subjects.interface-dispatch-native-entry.test-diamond-derived\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondDerived:System.Int32()\",\"signature\":\"System.Int32 InterfaceDispatchNativeEntry::TestDiamondDerived()\",\"identity\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry\",\"definitionSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondDerived:System.Int32()\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondDerived:System.Int32()\",\"methodId\":\"interface-dispatch-subjects.interface-dispatch-native-entry.test-diamond-derived\",\"signature\":\"System.Int32 InterfaceDispatchNativeEntry::TestDiamondDerived()\"},\"nativeSymbol\":\"InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondDerived\",\"isStatic\":true,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":1,\"resultType\":\"InterfaceDispatchSubjects/ImplDiamond\",\"callee\":\"InterfaceDispatchSubjects/ImplDiamond::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/ImplDiamond::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/ImplDiamond\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"baseTypeSubjectId\":\"System.Private.CoreLib/System.Object\",\"implementedInterfaceSubjectIds\":[\"InterfaceDispatchSubjects/IBase\",\"InterfaceDispatchSubjects/IDerived\"],\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":6,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"callee\":\"InterfaceDispatchSubjects/IDerived::DerivedMethod:System.Int32()\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/IDerived::DerivedMethod:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/IDerived::DerivedMethod:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"InterfaceDispatchSubjects_IDerived_DerivedMethod\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":2,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":16,\"ilOffset\":14,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":16,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":17,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"InterfaceDispatchSubjects\"}";
static const char kMethodJson_28[] = "{\"methodId\":\"interface-dispatch-subjects.interface-dispatch-native-entry.test-diamond-multi\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondMulti:System.Int32()\",\"signature\":\"System.Int32 InterfaceDispatchNativeEntry::TestDiamondMulti()\",\"identity\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry\",\"definitionSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondMulti:System.Int32()\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondMulti:System.Int32()\",\"methodId\":\"interface-dispatch-subjects.interface-dispatch-native-entry.test-diamond-multi\",\"signature\":\"System.Int32 InterfaceDispatchNativeEntry::TestDiamondMulti()\"},\"nativeSymbol\":\"InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondMulti\",\"isStatic\":true,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":3,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":1,\"resultType\":\"InterfaceDispatchSubjects/ImplDiamond\",\"callee\":\"InterfaceDispatchSubjects/ImplDiamond::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/ImplDiamond::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/ImplDiamond\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"baseTypeSubjectId\":\"System.Private.CoreLib/System.Object\",\"implementedInterfaceSubjectIds\":[\"InterfaceDispatchSubjects/IBase\",\"InterfaceDispatchSubjects/IDerived\"],\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":6,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":7,\"resultType\":\"InterfaceDispatchSubjects/ImplDiamond2\",\"callee\":\"InterfaceDispatchSubjects/ImplDiamond2::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/ImplDiamond2::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/ImplDiamond2\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"baseTypeSubjectId\":\"System.Private.CoreLib/System.Object\",\"implementedInterfaceSubjectIds\":[\"InterfaceDispatchSubjects/IBase\",\"InterfaceDispatchSubjects/IDerived\"],\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":12,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":13,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"callee\":\"InterfaceDispatchSubjects/IBase::BaseMethod:System.Int32()\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/IBase::BaseMethod:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/IBase::BaseMethod:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"InterfaceDispatchSubjects_IBase_BaseMethod\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":2,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":19,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":20,\"resultType\":\"System.Int32\",\"callee\":\"InterfaceDispatchSubjects/IDerived::DerivedMethod:System.Int32()\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/IDerived::DerivedMethod:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/IDerived::DerivedMethod:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"InterfaceDispatchSubjects_IDerived_DerivedMethod\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":2,\"isPreserveSig\":false},{\"op\":\"add\",\"opCode\":25,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":26,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":27,\"resultType\":\"System.Int32\",\"callee\":\"InterfaceDispatchSubjects/IBase::BaseMethod:System.Int32()\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/IBase::BaseMethod:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/IBase::BaseMethod:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"InterfaceDispatchSubjects_IBase_BaseMethod\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":2,\"isPreserveSig\":false},{\"op\":\"add\",\"opCode\":25,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":33,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":34,\"resultType\":\"System.Int32\",\"callee\":\"InterfaceDispatchSubjects/IDerived::DerivedMethod:System.Int32()\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/IDerived::DerivedMethod:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/IDerived::DerivedMethod:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"InterfaceDispatchSubjects_IDerived_DerivedMethod\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":2,\"isPreserveSig\":false},{\"op\":\"add\",\"opCode\":25,\"ilOffset\":39,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":40,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":43,\"ilOffset\":41,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":43,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":44,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"InterfaceDispatchSubjects\"}";
static const char kMethodJson_29[] = "{\"methodId\":\"interface-dispatch-subjects.interface-dispatch-native-entry.test-is-check\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestIsCheck:System.Int32()\",\"signature\":\"System.Int32 InterfaceDispatchNativeEntry::TestIsCheck()\",\"identity\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry\",\"definitionSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestIsCheck:System.Int32()\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestIsCheck:System.Int32()\",\"methodId\":\"interface-dispatch-subjects.interface-dispatch-native-entry.test-is-check\",\"signature\":\"System.Int32 InterfaceDispatchNativeEntry::TestIsCheck()\"},\"nativeSymbol\":\"InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestIsCheck\",\"isStatic\":true,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":7,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":1,\"resultType\":\"InterfaceDispatchSubjects/ImplSimple\",\"callee\":\"InterfaceDispatchSubjects/ImplSimple::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/ImplSimple::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/ImplSimple\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"baseTypeSubjectId\":\"System.Private.CoreLib/System.Object\",\"implementedInterfaceSubjectIds\":[\"InterfaceDispatchSubjects/ISimple\"],\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":6,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":7,\"resultType\":\"InterfaceDispatchSubjects/CalcAdd\",\"callee\":\"InterfaceDispatchSubjects/CalcAdd::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/CalcAdd::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/CalcAdd\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"baseTypeSubjectId\":\"System.Private.CoreLib/System.Object\",\"implementedInterfaceSubjectIds\":[\"InterfaceDispatchSubjects/ICalculator\"],\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":12,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":13,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":14,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":15,\"isPreserveSig\":false},{\"op\":\"isinst\",\"opCode\":38,\"operand\":\"InterfaceDispatchSubjects/ISimple\",\"ilOffset\":16,\"resultType\":\"InterfaceDispatchSubjects/ISimple\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"type\",\"subjectId\":\"InterfaceDispatchSubjects/ISimple\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/ISimple\",\"typeShape\":3,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"implementedInterfaceSubjectIds\":[],\"declaringTypeShape\":0},\"runtimeServiceKind\":8,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":21,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"cgt.un\",\"opCode\":33,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":3,\"ilOffset\":24,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":3,\"ilOffset\":25,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":33,\"ilOffset\":26,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":28,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":10,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"add\",\"opCode\":25,\"ilOffset\":31,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":32,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":33,\"isPreserveSig\":false},{\"op\":\"isinst\",\"opCode\":38,\"operand\":\"InterfaceDispatchSubjects/ISimple\",\"ilOffset\":34,\"resultType\":\"InterfaceDispatchSubjects/ISimple\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"type\",\"subjectId\":\"InterfaceDispatchSubjects/ISimple\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/ISimple\",\"typeShape\":3,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"implementedInterfaceSubjectIds\":[],\"declaringTypeShape\":0},\"runtimeServiceKind\":8,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":39,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"cgt.un\",\"opCode\":33,\"ilOffset\":40,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":4,\"ilOffset\":42,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":4,\"ilOffset\":44,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":53,\"ilOffset\":46,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":48,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":100,\"ilOffset\":49,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"add\",\"opCode\":25,\"ilOffset\":51,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":52,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":53,\"isPreserveSig\":false},{\"op\":\"isinst\",\"opCode\":38,\"operand\":\"InterfaceDispatchSubjects/ICalculator\",\"ilOffset\":54,\"resultType\":\"InterfaceDispatchSubjects/ICalculator\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"type\",\"subjectId\":\"InterfaceDispatchSubjects/ICalculator\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/ICalculator\",\"typeShape\":3,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"implementedInterfaceSubjectIds\":[],\"declaringTypeShape\":0},\"runtimeServiceKind\":8,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":59,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"cgt.un\",\"opCode\":33,\"ilOffset\":60,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":5,\"ilOffset\":62,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":5,\"ilOffset\":64,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":76,\"ilOffset\":66,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":68,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1000,\"ilOffset\":69,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"add\",\"opCode\":25,\"ilOffset\":74,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":75,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":76,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":6,\"ilOffset\":77,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":81,\"ilOffset\":79,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":6,\"ilOffset\":81,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":83,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"InterfaceDispatchSubjects\"}";
static const char kMethodJson_30[] = "{\"methodId\":\"interface-dispatch-subjects.interface-dispatch-native-entry.test-multi-impl-calc\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestMultiImplCalc:System.Int32()\",\"signature\":\"System.Int32 InterfaceDispatchNativeEntry::TestMultiImplCalc()\",\"identity\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry\",\"definitionSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestMultiImplCalc:System.Int32()\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestMultiImplCalc:System.Int32()\",\"methodId\":\"interface-dispatch-subjects.interface-dispatch-native-entry.test-multi-impl-calc\",\"signature\":\"System.Int32 InterfaceDispatchNativeEntry::TestMultiImplCalc()\"},\"nativeSymbol\":\"InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestMultiImplCalc\",\"isStatic\":true,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":3,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":1,\"resultType\":\"InterfaceDispatchSubjects/CalcAdd\",\"callee\":\"InterfaceDispatchSubjects/CalcAdd::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/CalcAdd::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/CalcAdd\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"baseTypeSubjectId\":\"System.Private.CoreLib/System.Object\",\"implementedInterfaceSubjectIds\":[\"InterfaceDispatchSubjects/ICalculator\"],\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":6,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":7,\"resultType\":\"InterfaceDispatchSubjects/CalcMul\",\"callee\":\"InterfaceDispatchSubjects/CalcMul::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/CalcMul::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/CalcMul\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"baseTypeSubjectId\":\"System.Private.CoreLib/System.Object\",\"implementedInterfaceSubjectIds\":[\"InterfaceDispatchSubjects/ICalculator\"],\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":12,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":13,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":10,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":5,\"ilOffset\":16,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"callee\":\"InterfaceDispatchSubjects/CalcAdd::Add:System.Int32(System.Int32,System.Int32)\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/CalcAdd::Add:System.Int32(System.Int32,System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/CalcAdd::Add:System.Int32(System.Int32,System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"InterfaceDispatchSubjects_CalcAdd_Add_System_Int32_System_Int32\",\"targetParameterCount\":2,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":2,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":22,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":3,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":4,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"callee\":\"InterfaceDispatchSubjects/CalcMul::Multiply:System.Int32(System.Int32,System.Int32)\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/CalcMul::Multiply:System.Int32(System.Int32,System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/CalcMul::Multiply:System.Int32(System.Int32,System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"InterfaceDispatchSubjects_CalcMul_Multiply_System_Int32_System_Int32\",\"targetParameterCount\":2,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":2,\"isPreserveSig\":false},{\"op\":\"add\",\"opCode\":25,\"ilOffset\":30,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":31,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":34,\"ilOffset\":32,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":34,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":35,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"InterfaceDispatchSubjects\"}";
static const char kMethodJson_31[] = "{\"methodId\":\"interface-dispatch-subjects.interface-dispatch-native-entry.test-single-impl-default\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestSingleImplDefault:System.Int32()\",\"signature\":\"System.Int32 InterfaceDispatchNativeEntry::TestSingleImplDefault()\",\"identity\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry\",\"definitionSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestSingleImplDefault:System.Int32()\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestSingleImplDefault:System.Int32()\",\"methodId\":\"interface-dispatch-subjects.interface-dispatch-native-entry.test-single-impl-default\",\"signature\":\"System.Int32 InterfaceDispatchNativeEntry::TestSingleImplDefault()\"},\"nativeSymbol\":\"InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplDefault\",\"isStatic\":true,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":1,\"resultType\":\"InterfaceDispatchSubjects/ImplWithDefault\",\"callee\":\"InterfaceDispatchSubjects/ImplWithDefault::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/ImplWithDefault::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/ImplWithDefault\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"baseTypeSubjectId\":\"System.Private.CoreLib/System.Object\",\"implementedInterfaceSubjectIds\":[\"InterfaceDispatchSubjects/IWithDefault\"],\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":6,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"callee\":\"InterfaceDispatchSubjects/IWithDefault::GetValue:System.Int32()\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/IWithDefault::GetValue:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/IWithDefault::GetValue:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"InterfaceDispatchSubjects_IWithDefault_GetValue\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":2,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":16,\"ilOffset\":14,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":16,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":17,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"InterfaceDispatchSubjects\"}";
static const char kMethodJson_32[] = "{\"methodId\":\"interface-dispatch-subjects.interface-dispatch-native-entry.test-single-impl-simple\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestSingleImplSimple:System.Int32()\",\"signature\":\"System.Int32 InterfaceDispatchNativeEntry::TestSingleImplSimple()\",\"identity\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry\",\"definitionSubjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestSingleImplSimple:System.Int32()\",\"subjectId\":\"InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestSingleImplSimple:System.Int32()\",\"methodId\":\"interface-dispatch-subjects.interface-dispatch-native-entry.test-single-impl-simple\",\"signature\":\"System.Int32 InterfaceDispatchNativeEntry::TestSingleImplSimple()\"},\"nativeSymbol\":\"InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplSimple\",\"isStatic\":true,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":1,\"resultType\":\"InterfaceDispatchSubjects/ImplSimple\",\"callee\":\"InterfaceDispatchSubjects/ImplSimple::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/ImplSimple::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/ImplSimple\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"baseTypeSubjectId\":\"System.Private.CoreLib/System.Object\",\"implementedInterfaceSubjectIds\":[\"InterfaceDispatchSubjects/ISimple\"],\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":6,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"callee\":\"InterfaceDispatchSubjects/ISimple::GetValue:System.Int32()\",\"reference\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"InterfaceDispatchSubjects/ISimple::GetValue:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"InterfaceDispatchSubjects\",\"subjectId\":\"InterfaceDispatchSubjects/ISimple::GetValue:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"InterfaceDispatchSubjects_ISimple_GetValue\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":2,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":16,\"ilOffset\":14,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":16,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":17,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"InterfaceDispatchSubjects\"}";
static const char kMethodJson_33[] = "{\"methodId\":\"interface-dispatch-subjects.i-with-default.get-value\",\"subjectId\":\"InterfaceDispatchSubjects/IWithDefault::GetValue:System.Int32()\",\"signature\":\"System.Int32 IWithDefault::GetValue()\",\"identity\":{\"assemblyName\":\"InterfaceDispatchSubjects\",\"declaringTypeSubjectId\":\"InterfaceDispatchSubjects/IWithDefault\",\"definitionSubjectId\":\"InterfaceDispatchSubjects/IWithDefault::GetValue:System.Int32()\",\"subjectId\":\"InterfaceDispatchSubjects/IWithDefault::GetValue:System.Int32()\",\"methodId\":\"interface-dispatch-subjects.i-with-default.get-value\",\"signature\":\"System.Int32 IWithDefault::GetValue()\"},\"nativeSymbol\":\"InterfaceDispatchSubjects_IWithDefault_GetValue\",\"isStatic\":false,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":0,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"InterfaceDispatchSubjects\"}";

extern "C" const JitMethodEntry kChaosJitMethodEntries[34] =
{
    { kMethodJson_0, 5844u, 0x0000000Fu, 0u },
    { kMethodJson_1, 5675u, 0x00000010u, 0u },
    { kMethodJson_2, 5683u, 0x00000011u, 0u },
    { kMethodJson_3, 5651u, 0x00000012u, 0u },
    { kMethodJson_4, 5603u, 0x00000013u, 0u },
    { kMethodJson_5, 5603u, 0x00000014u, 0u },
    { kMethodJson_6, 5635u, 0x00000015u, 0u },
    { kMethodJson_7, 5659u, 0x00000016u, 0u },
    { kMethodJson_8, 5643u, 0x00000017u, 0u },
    { kMethodJson_9, 1833u, 0x00000023u, 0u },
    { kMethodJson_10, 1622u, 0x00000021u, 0u },
    { kMethodJson_11, 1662u, 0x00000022u, 0u },
    { kMethodJson_12, 1833u, 0x00000026u, 0u },
    { kMethodJson_13, 1812u, 0x00000024u, 0u },
    { kMethodJson_14, 1852u, 0x00000025u, 0u },
    { kMethodJson_15, 1873u, 0x00000029u, 0u },
    { kMethodJson_16, 1308u, 0x00000027u, 0u },
    { kMethodJson_17, 1332u, 0x00000028u, 0u },
    { kMethodJson_18, 1883u, 0x0000002Cu, 0u },
    { kMethodJson_19, 1317u, 0x0000002Au, 0u },
    { kMethodJson_20, 1341u, 0x0000002Bu, 0u },
    { kMethodJson_21, 1863u, 0x0000001Fu, 0u },
    { kMethodJson_22, 1282u, 0x0000001Eu, 0u },
    { kMethodJson_23, 1915u, 0x00000020u, 0u },
    { kMethodJson_24, 14736u, 0x0000002Du, 0u },
    { kMethodJson_25, 4889u, 0x00000032u, 0u },
    { kMethodJson_26, 3353u, 0x00000033u, 0u },
    { kMethodJson_27, 3401u, 0x00000034u, 0u },
    { kMethodJson_28, 6962u, 0x00000035u, 0u },
    { kMethodJson_29, 8095u, 0x00000031u, 0u },
    { kMethodJson_30, 5641u, 0x00000030u, 0u },
    { kMethodJson_31, 3409u, 0x0000002Fu, 0u },
    { kMethodJson_32, 3356u, 0x0000002Eu, 0u },
    { kMethodJson_33, 1301u, 0x00000019u, 0u }
};

extern "C" const uint32_t kChaosJitMethodEntryCount = 34u;

extern "C" void ChaosJitRegisterAll() {
    RegisterJitMethods(kChaosJitMethodEntries, kChaosJitMethodEntryCount);
}