#include <chaos/common.h>
#include <chaos/type_info.h>
#include "runtime_core.h"
#include <chaos/eh.h>
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
#include "enum_stubs.h"
#include "enum_metadata.generated.h"

// Forward declaration for dispatch table entries (defined in runtime_stubs.cpp)
extern "C" void InterpreterEntryDirect(
    CHAOS_IL2CPP_UINTPTR method_key,
    void*     args_buf,
    void*     ret_buf) noexcept;

#pragma warning(push)
#pragma warning(disable: 4065 4244)

// Forward declarations (pipeline fix: used before extern "C" decl)
extern "C" CHAOS_IL2CPP_INT32 CcwBasicSubjects_CcwBasicNativeEntry_Run_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 CcwBasicSubjects_CcwBasicNativeEntry_TestConstantValue(void);
extern "C" CHAOS_IL2CPP_INT32 CcwBasicSubjects_CcwBasicNativeEntry_TestDualInterface(void);
extern "C" CHAOS_IL2CPP_INT32 CcwBasicSubjects_CcwBasicNativeEntry_TestSimpleMathAdd(void);
extern "C" CHAOS_IL2CPP_INT32 CcwBasicSubjects_CcwBasicNativeEntry_TestSimpleMathMul(void);
extern "C" void CcwBasicSubjects_CcwBasicSubjects_Subject_0(void);
extern "C" void CcwBasicSubjects_CcwBasicSubjects_Subject_1(void);
extern "C" void CcwBasicSubjects_CcwBasicSubjects_Subject_2(void);
extern "C" void CcwBasicSubjects_CcwBasicSubjects_Subject_3(void);
extern "C" void CcwBasicSubjects_CcwBasicSubjects_Subject_4(void);
extern "C" CHAOS_IL2CPP_INT32 CcwBasicSubjects_ConstantFortyTwo_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CcwBasicSubjects_ConstantFortyTwo__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 CcwBasicSubjects_DualInterfaceImpl_Add_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INT32 CcwBasicSubjects_DualInterfaceImpl_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 CcwBasicSubjects_DualInterfaceImpl_Multiply_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" void CcwBasicSubjects_DualInterfaceImpl__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 CcwBasicSubjects_SimpleMath_Add_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INT32 CcwBasicSubjects_SimpleMath_Multiply_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" void CcwBasicSubjects_SimpleMath__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);

namespace chaos::il2cpp::codegen::CcwBasicSubjects {

// Bring runtime_core and jit declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;
using namespace chaos::il2cpp::jit;

// Runtime prelude dependencies included at translation unit level


inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_CcwBasicSubjects_IConstantValue = static_cast<CHAOS_IL2CPP_INTPTR>(11127514427160927756ULL);
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_CcwBasicSubjects_ISimpleMath = static_cast<CHAOS_IL2CPP_INTPTR>(502509295808532539ULL);
const void* chaos_vtable_CcwBasicSubjects_ConstantFortyTwo[];
const void* chaos_vtable_CcwBasicSubjects_DualInterfaceImpl[];
const void* chaos_vtable_CcwBasicSubjects_SimpleMath[];
inline MethodTable chaos_mt_CcwBasicSubjects_CcwBasicSubjects = {nullptr, nullptr, 5516802378886089046ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_CcwBasicSubjects_CcwBasicSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(5516802378886089046ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_Object = {nullptr, nullptr, 15228727185366376748ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_INTPTR>(15228727185366376748ULL);
static constexpr InterfaceMapEntry chaos_iface_map_CcwBasicSubjects_ConstantFortyTwo[] = {
	{ chaos_type_id_CcwBasicSubjects_IConstantValue, 1, 1 }
};
inline MethodTable chaos_mt_CcwBasicSubjects_ConstantFortyTwo = {reinterpret_cast<const MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Object), chaos_vtable_CcwBasicSubjects_ConstantFortyTwo, 9147543264357235806ULL, 2u, 32, 1, 1, chaos_iface_map_CcwBasicSubjects_ConstantFortyTwo, nullptr, 1, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_CcwBasicSubjects_ConstantFortyTwo = static_cast<CHAOS_IL2CPP_INTPTR>(9147543264357235806ULL);
static constexpr InterfaceMapEntry chaos_iface_map_CcwBasicSubjects_DualInterfaceImpl[] = {
	{ chaos_type_id_CcwBasicSubjects_ISimpleMath, 2, 2 },
	{ chaos_type_id_CcwBasicSubjects_IConstantValue, 1, 1 }
};
inline MethodTable chaos_mt_CcwBasicSubjects_DualInterfaceImpl = {reinterpret_cast<const MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Object), chaos_vtable_CcwBasicSubjects_DualInterfaceImpl, 14234888545717869799ULL, 4u, 32, 1, 1, chaos_iface_map_CcwBasicSubjects_DualInterfaceImpl, nullptr, 2, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_CcwBasicSubjects_DualInterfaceImpl = static_cast<CHAOS_IL2CPP_INTPTR>(14234888545717869799ULL);
static constexpr InterfaceMapEntry chaos_iface_map_CcwBasicSubjects_SimpleMath[] = {
	{ chaos_type_id_CcwBasicSubjects_ISimpleMath, 2, 2 }
};
inline MethodTable chaos_mt_CcwBasicSubjects_SimpleMath = {reinterpret_cast<const MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Object), chaos_vtable_CcwBasicSubjects_SimpleMath, 4431683709018832856ULL, 4u, 32, 1, 1, chaos_iface_map_CcwBasicSubjects_SimpleMath, nullptr, 1, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_CcwBasicSubjects_SimpleMath = static_cast<CHAOS_IL2CPP_INTPTR>(4431683709018832856ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_Assembly = {nullptr, nullptr, 5474029880995115448ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_Assembly = static_cast<CHAOS_IL2CPP_INTPTR>(5474029880995115448ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName = {nullptr, nullptr, 17082367815459723707ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_AssemblyName = static_cast<CHAOS_IL2CPP_INTPTR>(17082367815459723707ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo = {nullptr, nullptr, 4137207361503509124ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_ConstructorInfo = static_cast<CHAOS_IL2CPP_INTPTR>(4137207361503509124ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo = {nullptr, nullptr, 17040031516751226236ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_FieldInfo = static_cast<CHAOS_IL2CPP_INTPTR>(17040031516751226236ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo = {nullptr, nullptr, 10748947813473285525ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_MethodInfo = static_cast<CHAOS_IL2CPP_INTPTR>(10748947813473285525ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_String = {nullptr, nullptr, 1782325859292956794ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_String = static_cast<CHAOS_IL2CPP_INTPTR>(1782325859292956794ULL);
inline MethodTable chaos_mt_CcwBasicSubjects_IConstantValue = {nullptr, nullptr, 11127514427160927756ULL, 0u, 32, 3, 0, nullptr, nullptr, 0, 0, 0, 0};
inline MethodTable chaos_mt_CcwBasicSubjects_ISimpleMath = {nullptr, nullptr, 502509295808532539ULL, 0u, 32, 3, 0, nullptr, nullptr, 0, 0, 0, 0};

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
		return chaos_array->element_type_shape == chaos_type_shape_reference            && chaos_is_type_compatible(chaos_mt_System_Private_CoreLib_System_String.AsTypeInfoHot(), chaos_array->element_type_info);
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


// ── GcTypeLayout pointer offset arrays (computed via offsetof) ──
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_Assembly[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_Assembly, runtime_assembly_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, declaring_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, runtime_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, generic_definition_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, generic_argument_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, declaring_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, declaring_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, runtime_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, generic_definition_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, generic_argument_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, runtime_name_value))};

extern "C" void ChaosRegisterGcLayouts() {
	auto& registry = chaos::il2cpp::runtime_core::GcLayoutRegistry::Instance();

	registry.Register(5516802378886089046ULL, sizeof(chaos_type_CcwBasicSubjects_CcwBasicSubjects), nullptr, 0);
	registry.Register(15228727185366376748ULL, sizeof(chaos_type_System_Private_CoreLib_System_Object), nullptr, 0);
	registry.Register(9147543264357235806ULL, sizeof(chaos_type_CcwBasicSubjects_ConstantFortyTwo), nullptr, 0);
	registry.Register(14234888545717869799ULL, sizeof(chaos_type_CcwBasicSubjects_DualInterfaceImpl), nullptr, 0);
	registry.Register(4431683709018832856ULL, sizeof(chaos_type_CcwBasicSubjects_SimpleMath), nullptr, 0);
	registry.Register(5474029880995115448ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_Assembly), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_Assembly, 1);
	registry.Register(17082367815459723707ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName, 1);
	registry.Register(4137207361503509124ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, 5);
	registry.Register(17040031516751226236ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, 2);
	registry.Register(10748947813473285525ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, 5);
	registry.Register(1782325859292956794ULL, sizeof(chaos_type_System_Private_CoreLib_System_String), nullptr, 0);

	// Register MethodTable address ranges for IsValidTypeInfoPointer.
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_CcwBasicSubjects_CcwBasicSubjects), reinterpret_cast<uintptr_t>(&chaos_mt_CcwBasicSubjects_CcwBasicSubjects) + sizeof(chaos_mt_CcwBasicSubjects_CcwBasicSubjects));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Object), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Object) + sizeof(chaos_mt_System_Private_CoreLib_System_Object));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_CcwBasicSubjects_ConstantFortyTwo), reinterpret_cast<uintptr_t>(&chaos_mt_CcwBasicSubjects_ConstantFortyTwo) + sizeof(chaos_mt_CcwBasicSubjects_ConstantFortyTwo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_CcwBasicSubjects_DualInterfaceImpl), reinterpret_cast<uintptr_t>(&chaos_mt_CcwBasicSubjects_DualInterfaceImpl) + sizeof(chaos_mt_CcwBasicSubjects_DualInterfaceImpl));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_CcwBasicSubjects_SimpleMath), reinterpret_cast<uintptr_t>(&chaos_mt_CcwBasicSubjects_SimpleMath) + sizeof(chaos_mt_CcwBasicSubjects_SimpleMath));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_Assembly), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_Assembly) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_Assembly));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_String), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_String) + sizeof(chaos_mt_System_Private_CoreLib_System_String));
}

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


// ── Bridge/import thunks ──
extern "C" void* kChaosExternalRuntimeFnTable[];

// Bridge/import thunk for: CcwBasicSubjects/CcwBasicSubjects::_exitCode
extern "C" void chaos_bridge_thunk_0(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[0])();
}

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
		chaos_mt_CcwBasicSubjects_CcwBasicSubjects.AsTypeInfoHot(),
		chaos_mt_CcwBasicSubjects_ISimpleMath.AsTypeInfoHot(),
		chaos_mt_CcwBasicSubjects_IConstantValue.AsTypeInfoHot(),
		chaos_mt_CcwBasicSubjects_SimpleMath.AsTypeInfoHot(),
		chaos_mt_CcwBasicSubjects_ConstantFortyTwo.AsTypeInfoHot(),
		chaos_mt_CcwBasicSubjects_DualInterfaceImpl.AsTypeInfoHot(),
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
	/* .custom_attribute_blob       = */ nullptr,  // Tier 1 -- deferred
	/* .custom_attribute_offset     = */ nullptr,
	/* .custom_attribute_entity_count = */ 0u,
	/* .custom_attribute_materializer = */ nullptr,
	/* .custom_attribute_method_offset   = */ nullptr,
	/* .custom_attribute_field_offset    = */ nullptr,
	/* .custom_attribute_property_offset = */ nullptr,
	/* .custom_attribute_param_offset    = */ nullptr,
	/* .custom_attribute_method_count    = */ 0u,
	/* .custom_attribute_field_count     = */ 0u,
	/* .custom_attribute_property_count  = */ 0u,
	/* .custom_attribute_param_count   = */ 0u,
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
// s_hotpatch_entries — external linkage for verification_dispatch
// NOTE: `static` is deliberately omitted so that the verification dispatch
// TU (verification_dispatch.generated.cpp) can access this array via
// GetHotpatchEntries(). Without external linkage, the linker cannot resolve
// the symbol across translation units.
extern "C" HotpatchEntryV0 s_hotpatch_entries[19] = {
	{ reinterpret_cast<void*>(&CcwBasicSubjects_CcwBasicSubjects_Subject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // CcwBasicSubjects::Subject_0
	{ reinterpret_cast<void*>(&CcwBasicSubjects_CcwBasicSubjects_Subject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // CcwBasicSubjects::Subject_1
	{ reinterpret_cast<void*>(&CcwBasicSubjects_CcwBasicSubjects_Subject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // CcwBasicSubjects::Subject_2
	{ reinterpret_cast<void*>(&CcwBasicSubjects_CcwBasicSubjects_Subject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // CcwBasicSubjects::Subject_3
	{ reinterpret_cast<void*>(&CcwBasicSubjects_CcwBasicSubjects_Subject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // CcwBasicSubjects::Subject_4
	{ reinterpret_cast<void*>(&CcwBasicSubjects_CcwBasicNativeEntry_Run_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // CcwBasicNativeEntry::Run
	{ reinterpret_cast<void*>(&CcwBasicSubjects_CcwBasicNativeEntry_TestConstantValue), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // CcwBasicNativeEntry::TestConstantValue
	{ reinterpret_cast<void*>(&CcwBasicSubjects_CcwBasicNativeEntry_TestDualInterface), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // CcwBasicNativeEntry::TestDualInterface
	{ reinterpret_cast<void*>(&CcwBasicSubjects_CcwBasicNativeEntry_TestSimpleMathAdd), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // CcwBasicNativeEntry::TestSimpleMathAdd
	{ reinterpret_cast<void*>(&CcwBasicSubjects_CcwBasicNativeEntry_TestSimpleMathMul), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // CcwBasicNativeEntry::TestSimpleMathMul
	{ reinterpret_cast<void*>(&CcwBasicSubjects_ConstantFortyTwo__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ConstantFortyTwo::.ctor
	{ reinterpret_cast<void*>(&CcwBasicSubjects_ConstantFortyTwo_GetValue), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ConstantFortyTwo::GetValue
	{ reinterpret_cast<void*>(&CcwBasicSubjects_DualInterfaceImpl__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // DualInterfaceImpl::.ctor
	{ reinterpret_cast<void*>(&CcwBasicSubjects_DualInterfaceImpl_Add_System_Int32_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // DualInterfaceImpl::Add
	{ reinterpret_cast<void*>(&CcwBasicSubjects_DualInterfaceImpl_GetValue), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // DualInterfaceImpl::GetValue
	{ reinterpret_cast<void*>(&CcwBasicSubjects_DualInterfaceImpl_Multiply_System_Int32_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // DualInterfaceImpl::Multiply
	{ reinterpret_cast<void*>(&CcwBasicSubjects_SimpleMath__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // SimpleMath::.ctor
	{ reinterpret_cast<void*>(&CcwBasicSubjects_SimpleMath_Add_System_Int32_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // SimpleMath::Add
	{ reinterpret_cast<void*>(&CcwBasicSubjects_SimpleMath_Multiply_System_Int32_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // SimpleMath::Multiply
};

// Accessor functions for verification_dispatch.generated.cpp
extern "C" const HotpatchEntryV0* GetHotpatchEntries() noexcept { return s_hotpatch_entries; }
extern "C" int32_t GetHotpatchEntryCount() noexcept { return 19; }

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
// ── Method table (kMethodTable[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
// NOTE: Pure data only — dispatch routing is in <chaos/hotpatch_dispatch.h>.
// JIT mode uses direct_ptr (precode trampoline → JIT compilation).

static void (*kMethodTable[19])() = {
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

// ── Default-arg thunks (kDefaultArgThunks[]) ────────────────────────────
// Each thunk supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_null_instance = 0;
extern "C" void (*kDefaultArgThunks[19])() = {
	[]() {kMethodTable[0]();},
	[]() {kMethodTable[1]();},
	[]() {kMethodTable[2]();},
	[]() {kMethodTable[3]();},
	[]() {kMethodTable[4]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[5])(0);},
	[]() {kMethodTable[6]();},
	[]() {kMethodTable[7]();},
	[]() {kMethodTable[8]();},
	[]() {kMethodTable[9]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[10])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[11])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[12])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[13])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance),0,0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[14])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[15])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance),0,0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[16])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[17])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance),0,0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[18])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance),0,0);},
};

// ── Subject slot map (kSubjectSlotMap[]) ────────────────────────────
// Maps subject index (0-based sequential) to kMethodTable index.
// External consumers use this to route --benchmark N to the correct
// AOT method slot, since kMethodTable[] includes lambdas/closures
// that shift subject methods to non-contiguous indices.
extern "C" const int kSubjectEntryCount = 5;
extern "C" const int kSubjectSlotMap[5] = {
	0,
	1,
	2,
	3,
	4,
};
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

	/* ── Entry 0: CcwBasicSubjects_CcwBasicSubjects_Subject_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry0;
	/* ── Entry 1: CcwBasicSubjects_CcwBasicSubjects_Subject_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry1;
	/* ── Entry 2: CcwBasicSubjects_CcwBasicSubjects_Subject_2 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry2;
	/* ── Entry 3: CcwBasicSubjects_CcwBasicSubjects_Subject_3 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry3;
	/* ── Entry 4: CcwBasicSubjects_CcwBasicSubjects_Subject_4 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry4;
	/* ── Entry 5: CcwBasicSubjects_CcwBasicNativeEntry_Run_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry5;
	/* ── Entry 6: CcwBasicSubjects_CcwBasicNativeEntry_TestConstantValue ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry6;
	/* ── Entry 7: CcwBasicSubjects_CcwBasicNativeEntry_TestDualInterface ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[8];
	} entry7;
	/* ── Entry 8: CcwBasicSubjects_CcwBasicNativeEntry_TestSimpleMathAdd ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry8;
	/* ── Entry 9: CcwBasicSubjects_CcwBasicNativeEntry_TestSimpleMathMul ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry9;
	/* ── Entry 10: CcwBasicSubjects_ConstantFortyTwo__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry10;
	/* ── Entry 11: CcwBasicSubjects_ConstantFortyTwo_GetValue ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry11;
	/* ── Entry 12: CcwBasicSubjects_DualInterfaceImpl__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry12;
	/* ── Entry 13: CcwBasicSubjects_DualInterfaceImpl_Add_System_Int32_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry13;
	/* ── Entry 14: CcwBasicSubjects_DualInterfaceImpl_GetValue ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry14;
	/* ── Entry 15: CcwBasicSubjects_DualInterfaceImpl_Multiply_System_Int32_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry15;
	/* ── Entry 16: CcwBasicSubjects_SimpleMath__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry16;
	/* ── Entry 17: CcwBasicSubjects_SimpleMath_Add_System_Int32_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry17;
	/* ── Entry 18: CcwBasicSubjects_SimpleMath_Multiply_System_Int32_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry18;
} kChaosGcSlotMapsSection = {
	/* entry0 = CcwBasicSubjects_CcwBasicSubjects_Subject_0 */
	.entry0 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CcwBasicSubjects_CcwBasicSubjects_Subject_0),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry1 = CcwBasicSubjects_CcwBasicSubjects_Subject_1 */
	.entry1 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CcwBasicSubjects_CcwBasicSubjects_Subject_1),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry2 = CcwBasicSubjects_CcwBasicSubjects_Subject_2 */
	.entry2 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CcwBasicSubjects_CcwBasicSubjects_Subject_2),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry3 = CcwBasicSubjects_CcwBasicSubjects_Subject_3 */
	.entry3 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CcwBasicSubjects_CcwBasicSubjects_Subject_3),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry4 = CcwBasicSubjects_CcwBasicSubjects_Subject_4 */
	.entry4 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CcwBasicSubjects_CcwBasicSubjects_Subject_4),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry5 = CcwBasicSubjects_CcwBasicNativeEntry_Run_System_Int32 */
	.entry5 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&CcwBasicSubjects_CcwBasicNativeEntry_Run_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 8u, 16u, 24u, 32u }
	},
	/* entry6 = CcwBasicSubjects_CcwBasicNativeEntry_TestConstantValue */
	.entry6 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CcwBasicSubjects_CcwBasicNativeEntry_TestConstantValue),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry7 = CcwBasicSubjects_CcwBasicNativeEntry_TestDualInterface */
	.entry7 = {
		/* entry_total_size = 52 */ 52u,
		/* code_address */ reinterpret_cast<const void*>(&CcwBasicSubjects_CcwBasicNativeEntry_TestDualInterface),
		/* frame_size = 64 */ 64u,
		/* num_gc_slots = 8 */ 8u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u }
	},
	/* entry8 = CcwBasicSubjects_CcwBasicNativeEntry_TestSimpleMathAdd */
	.entry8 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&CcwBasicSubjects_CcwBasicNativeEntry_TestSimpleMathAdd),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry9 = CcwBasicSubjects_CcwBasicNativeEntry_TestSimpleMathMul */
	.entry9 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&CcwBasicSubjects_CcwBasicNativeEntry_TestSimpleMathMul),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry10 = CcwBasicSubjects_ConstantFortyTwo__ctor */
	.entry10 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&CcwBasicSubjects_ConstantFortyTwo__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry11 = CcwBasicSubjects_ConstantFortyTwo_GetValue */
	.entry11 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&CcwBasicSubjects_ConstantFortyTwo_GetValue),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry12 = CcwBasicSubjects_DualInterfaceImpl__ctor */
	.entry12 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&CcwBasicSubjects_DualInterfaceImpl__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry13 = CcwBasicSubjects_DualInterfaceImpl_Add_System_Int32_System_Int32 */
	.entry13 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CcwBasicSubjects_DualInterfaceImpl_Add_System_Int32_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 24u, 32u }
	},
	/* entry14 = CcwBasicSubjects_DualInterfaceImpl_GetValue */
	.entry14 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&CcwBasicSubjects_DualInterfaceImpl_GetValue),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry15 = CcwBasicSubjects_DualInterfaceImpl_Multiply_System_Int32_System_Int32 */
	.entry15 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CcwBasicSubjects_DualInterfaceImpl_Multiply_System_Int32_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 24u, 32u }
	},
	/* entry16 = CcwBasicSubjects_SimpleMath__ctor */
	.entry16 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&CcwBasicSubjects_SimpleMath__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry17 = CcwBasicSubjects_SimpleMath_Add_System_Int32_System_Int32 */
	.entry17 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CcwBasicSubjects_SimpleMath_Add_System_Int32_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 24u, 32u }
	},
	/* entry18 = CcwBasicSubjects_SimpleMath_Multiply_System_Int32_System_Int32 */
	.entry18 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CcwBasicSubjects_SimpleMath_Multiply_System_Int32_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 24u, 32u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 628u;

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
	.image_name_utf8    = "CcwBasicSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_CcwBasicSubjects_CcwBasicSubjects[1] = {
	{ 0x04000001u, "CcwBasicSubjects/CcwBasicSubjects::_exitCode", "_exitCode", "System.Int32", 0LL, 3u },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_CcwBasicSubjects_CcwBasicSubjects[5] = {
	{ 0x00000009u, "CcwBasicSubjects/CcwBasicSubjects::Subject_0:System.Void()", "Subject_0", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Au, "CcwBasicSubjects/CcwBasicSubjects::Subject_1:System.Void()", "Subject_1", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Bu, "CcwBasicSubjects/CcwBasicSubjects::Subject_2:System.Void()", "Subject_2", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Cu, "CcwBasicSubjects/CcwBasicSubjects::Subject_3:System.Void()", "Subject_3", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Du, "CcwBasicSubjects/CcwBasicSubjects::Subject_4:System.Void()", "Subject_4", "System.Void", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_CcwBasicSubjects_CcwBasicNativeEntry[5] = {
	{ 0x0000001Au, "CcwBasicSubjects/CcwBasicNativeEntry::Run:System.Int32(System.Int32)", "Run", "System.Int32", 1, nullptr, 0u, nullptr, 0u },
	{ 0x0000001Du, "CcwBasicSubjects/CcwBasicNativeEntry::TestConstantValue:System.Int32()", "TestConstantValue", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000001Eu, "CcwBasicSubjects/CcwBasicNativeEntry::TestDualInterface:System.Int32()", "TestDualInterface", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000001Bu, "CcwBasicSubjects/CcwBasicNativeEntry::TestSimpleMathAdd:System.Int32()", "TestSimpleMathAdd", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000001Cu, "CcwBasicSubjects/CcwBasicNativeEntry::TestSimpleMathMul:System.Int32()", "TestSimpleMathMul", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_CcwBasicSubjects_ConstantFortyTwo[2] = {
	{ 0x00000015u, "CcwBasicSubjects/ConstantFortyTwo::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000014u, "CcwBasicSubjects/ConstantFortyTwo::GetValue:System.Int32()", "GetValue", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_CcwBasicSubjects_DualInterfaceImpl[4] = {
	{ 0x00000019u, "CcwBasicSubjects/DualInterfaceImpl::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000016u, "CcwBasicSubjects/DualInterfaceImpl::Add:System.Int32(System.Int32,System.Int32)", "Add", "System.Int32", 2, nullptr, 0u, nullptr, 0u },
	{ 0x00000018u, "CcwBasicSubjects/DualInterfaceImpl::GetValue:System.Int32()", "GetValue", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000017u, "CcwBasicSubjects/DualInterfaceImpl::Multiply:System.Int32(System.Int32,System.Int32)", "Multiply", "System.Int32", 2, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_CcwBasicSubjects_SimpleMath[3] = {
	{ 0x00000013u, "CcwBasicSubjects/SimpleMath::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000011u, "CcwBasicSubjects/SimpleMath::Add:System.Int32(System.Int32,System.Int32)", "Add", "System.Int32", 2, nullptr, 0u, nullptr, 0u },
	{ 0x00000012u, "CcwBasicSubjects/SimpleMath::Multiply:System.Int32(System.Int32,System.Int32)", "Multiply", "System.Int32", 2, nullptr, 0u, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[5] = {
	{ 0x00000001u, "CcwBasicSubjects/CcwBasicSubjects", "CcwBasicSubjects/CcwBasicSubjects", "", "CcwBasicSubjects", "CcwBasicSubjects", nullptr, kReflFields_CcwBasicSubjects_CcwBasicSubjects, 1u, nullptr, 0u,
nullptr, 0u,    kReflMethods_CcwBasicSubjects_CcwBasicSubjects, 5u, nullptr, 0u, 0u },
	{ 0x00000007u, "CcwBasicSubjects/CcwBasicNativeEntry", "CcwBasicSubjects/CcwBasicNativeEntry", "", "CcwBasicNativeEntry", "CcwBasicNativeEntry", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_CcwBasicSubjects_CcwBasicNativeEntry, 5u, nullptr, 0u, 0u },
	{ 0x00000005u, "CcwBasicSubjects/ConstantFortyTwo", "CcwBasicSubjects/ConstantFortyTwo", "", "ConstantFortyTwo", "ConstantFortyTwo", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_CcwBasicSubjects_ConstantFortyTwo, 2u, nullptr, 0u, 0u },
	{ 0x00000006u, "CcwBasicSubjects/DualInterfaceImpl", "CcwBasicSubjects/DualInterfaceImpl", "", "DualInterfaceImpl", "DualInterfaceImpl", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_CcwBasicSubjects_DualInterfaceImpl, 4u, nullptr, 0u, 0u },
	{ 0x00000004u, "CcwBasicSubjects/SimpleMath", "CcwBasicSubjects/SimpleMath", "", "SimpleMath", "SimpleMath", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_CcwBasicSubjects_SimpleMath, 3u, nullptr, 0u, 0u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[5] = {
	&kReflTypes[0],
	&kReflTypes[1],
	&kReflTypes[2],
	&kReflTypes[3],
	&kReflTypes[4],
};

static const ReflectionQueryImageDescriptor kReflImage = { "CcwBasicSubjects", kReflTypePtrs, 5u, 1, 0, 0, 0 };

// Fake ImageHandle that ResolveSubjectId will decode back to kReflImage.
// BootstrapRuntime's aot_image_handle fallback discovers this via
// LookupModule(mid)->image at lines 311-321 of bootstrap.cpp.
	// Register generated enum metadata (tables + dispatch + type descriptors).
	// ChaosRegisterEnumGeneratedMetadata is defined in enum_metadata.generated.h.
	static const CHAOS_IL2CPP_UINT32 s_enum_registered = []() noexcept {
		ChaosRegisterEnumGeneratedMetadata();
		return 1u;
	}();

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
	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_0 = _s0;
				auto& _d5 = s_hotpatch_entries[5];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d5)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d5))
				{
					alignas(16) uint8_t _d_ab[4];
					ArgBuffer _d_bw(_d_ab);
					_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d5.method_key, _d_ab, &_d_hpresult);
				}
				else
				{
					_d_hpresult = CcwBasicSubjects_CcwBasicNativeEntry_Run_System_Int32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_0 = _s1;
				auto& _d5 = s_hotpatch_entries[5];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d5)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d5))
				{
					alignas(16) uint8_t _d_ab[4];
					ArgBuffer _d_bw(_d_ab);
					_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d5.method_key, _d_ab, &_d_hpresult);
				}
				else
				{
					_d_hpresult = CcwBasicSubjects_CcwBasicNativeEntry_Run_System_Int32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
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
				chaos_static_CcwBasicSubjects_CcwBasicSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (!chaos_eh_match_type(CHAOS_EH_EXCEPTION_OBJ, chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
		{ CHAOS_EH_RETHROW; }
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_CcwBasicSubjects_CcwBasicSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: CcwBasicSubjects/CcwBasicSubjects::Subject_1()
extern "C" void CcwBasicSubjects_CcwBasicSubjects_Subject_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_EH_TRY
			{
				auto& _d8 = s_hotpatch_entries[8];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d8)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d8))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d8.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = CcwBasicSubjects_CcwBasicNativeEntry_TestSimpleMathAdd();
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
			{
				auto& _d8 = s_hotpatch_entries[8];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d8)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d8))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d8.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = CcwBasicSubjects_CcwBasicNativeEntry_TestSimpleMathAdd();
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
				chaos_static_CcwBasicSubjects_CcwBasicSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (!chaos_eh_match_type(CHAOS_EH_EXCEPTION_OBJ, chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
		{ CHAOS_EH_RETHROW; }
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_CcwBasicSubjects_CcwBasicSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: CcwBasicSubjects/CcwBasicSubjects::Subject_2()
extern "C" void CcwBasicSubjects_CcwBasicSubjects_Subject_2(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_EH_TRY
			{
				auto& _d9 = s_hotpatch_entries[9];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d9)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d9))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d9.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = CcwBasicSubjects_CcwBasicNativeEntry_TestSimpleMathMul();
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
			{
				auto& _d9 = s_hotpatch_entries[9];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d9)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d9))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d9.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = CcwBasicSubjects_CcwBasicNativeEntry_TestSimpleMathMul();
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
				chaos_static_CcwBasicSubjects_CcwBasicSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (!chaos_eh_match_type(CHAOS_EH_EXCEPTION_OBJ, chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
		{ CHAOS_EH_RETHROW; }
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_CcwBasicSubjects_CcwBasicSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: CcwBasicSubjects/CcwBasicSubjects::Subject_3()
extern "C" void CcwBasicSubjects_CcwBasicSubjects_Subject_3(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_EH_TRY
			{
				// Inlined: CcwBasicSubjects/CcwBasicNativeEntry::TestConstantValue:System.Int32()
				CHAOS_IL2CPP_INTPTR chaos_inline_retval0{};
				{
					auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_CcwBasicSubjects_ConstantFortyTwo, {});
					chaos_object->header.type_info = chaos_mt_CcwBasicSubjects_ConstantFortyTwo.AsTypeInfoHot();
					CcwBasicSubjects_ConstantFortyTwo__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
					_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
				}
				chaos_locals[1] = _s0;
				_s0 = chaos_locals[1];
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
				chaos_locals[2] = _s0;
				goto chaos_inline_label_0_0;
			chaos_inline_label_0_0:
			{
				_s0 = chaos_locals[2];
			}
				chaos_inline_retval0 = _s0;
				goto chaos_inline_end0;
			chaos_inline_end0:
				;
				_s0 = chaos_inline_retval0;
			}
			{
				// Inlined: CcwBasicSubjects/CcwBasicNativeEntry::TestConstantValue:System.Int32()
				CHAOS_IL2CPP_INTPTR chaos_inline_retval1{};
				{
					auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_CcwBasicSubjects_ConstantFortyTwo, {});
					chaos_object->header.type_info = chaos_mt_CcwBasicSubjects_ConstantFortyTwo.AsTypeInfoHot();
					CcwBasicSubjects_ConstantFortyTwo__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
					_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
				}
				chaos_locals[1] = _s1;
				_s1 = chaos_locals[1];
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
				chaos_locals[2] = _s1;
				goto chaos_inline_label_1_0;
			chaos_inline_label_1_0:
			{
				_s1 = chaos_locals[2];
			}
				chaos_inline_retval1 = _s1;
				goto chaos_inline_end1;
			chaos_inline_end1:
				;
				_s1 = chaos_inline_retval1;
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
				chaos_static_CcwBasicSubjects_CcwBasicSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (!chaos_eh_match_type(CHAOS_EH_EXCEPTION_OBJ, chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
		{ CHAOS_EH_RETHROW; }
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_CcwBasicSubjects_CcwBasicSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: CcwBasicSubjects/CcwBasicSubjects::Subject_4()
extern "C" void CcwBasicSubjects_CcwBasicSubjects_Subject_4(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_EH_TRY
			{
				auto& _d7 = s_hotpatch_entries[7];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d7)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d7))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d7.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = CcwBasicSubjects_CcwBasicNativeEntry_TestDualInterface();
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
			{
				auto& _d7 = s_hotpatch_entries[7];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d7)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d7))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d7.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = CcwBasicSubjects_CcwBasicNativeEntry_TestDualInterface();
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
				chaos_static_CcwBasicSubjects_CcwBasicSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (!chaos_eh_match_type(CHAOS_EH_EXCEPTION_OBJ, chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
		{ CHAOS_EH_RETHROW; }
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_CcwBasicSubjects_CcwBasicSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: CcwBasicSubjects/CcwBasicNativeEntry::Run(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 CcwBasicSubjects_CcwBasicNativeEntry_Run_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	_s0 = chaos_args[0];
	{
		const auto chaos_switch_value = static_cast<CHAOS_IL2CPP_INT32>(_s0);
		switch (chaos_switch_value)
		{
			case 0:
				{
					// Inlined: CcwBasicSubjects/CcwBasicNativeEntry::TestSimpleMathAdd:System.Int32()
					CHAOS_IL2CPP_INTPTR chaos_inline_retval0{};
					{
						auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_CcwBasicSubjects_SimpleMath, {});
						chaos_object->header.type_info = chaos_mt_CcwBasicSubjects_SimpleMath.AsTypeInfoHot();
						CcwBasicSubjects_SimpleMath__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
						_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
					}
					chaos_locals[3] = _s0;
					_s0 = chaos_locals[3];
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
					chaos_locals[4] = _s0;
					goto chaos_inline_label_0_0;
				chaos_inline_label_0_0:
				{
					_s0 = chaos_locals[4];
				}
					chaos_inline_retval0 = _s0;
					goto chaos_inline_end0;
				chaos_inline_end0:
					;
					_s0 = chaos_inline_retval0;
				}
				chaos_locals[2] = _s0;
			case 1:
				{
					// Inlined: CcwBasicSubjects/CcwBasicNativeEntry::TestSimpleMathMul:System.Int32()
					CHAOS_IL2CPP_INTPTR chaos_inline_retval1{};
					{
						auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_CcwBasicSubjects_SimpleMath, {});
						chaos_object->header.type_info = chaos_mt_CcwBasicSubjects_SimpleMath.AsTypeInfoHot();
						CcwBasicSubjects_SimpleMath__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
						_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
					}
					chaos_locals[3] = _s0;
					_s0 = chaos_locals[3];
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
					chaos_locals[4] = _s0;
					goto chaos_inline_label_1_0;
				chaos_inline_label_1_0:
				{
					_s0 = chaos_locals[4];
				}
					chaos_inline_retval1 = _s0;
					goto chaos_inline_end1;
				chaos_inline_end1:
					;
					_s0 = chaos_inline_retval1;
				}
				chaos_locals[2] = _s0;
			case 2:
				{
					// Inlined: CcwBasicSubjects/CcwBasicNativeEntry::TestConstantValue:System.Int32()
					CHAOS_IL2CPP_INTPTR chaos_inline_retval2{};
					{
						auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_CcwBasicSubjects_ConstantFortyTwo, {});
						chaos_object->header.type_info = chaos_mt_CcwBasicSubjects_ConstantFortyTwo.AsTypeInfoHot();
						CcwBasicSubjects_ConstantFortyTwo__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
						_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
					}
					chaos_locals[3] = _s0;
					_s0 = chaos_locals[3];
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
					chaos_locals[4] = _s0;
					goto chaos_inline_label_2_0;
				chaos_inline_label_2_0:
				{
					_s0 = chaos_locals[4];
				}
					chaos_inline_retval2 = _s0;
					goto chaos_inline_end2;
				chaos_inline_end2:
					;
					_s0 = chaos_inline_retval2;
				}
				chaos_locals[2] = _s0;
			case 3:
			{
				{
					auto& _d7 = s_hotpatch_entries[7];
					CHAOS_IL2CPP_INT32 _d_hpresult{};
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d7)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d7))
					{
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d7.method_key, nullptr, &_d_hpresult);
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
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_CcwBasicSubjects_ConstantFortyTwo, {});
		chaos_object->header.type_info = chaos_mt_CcwBasicSubjects_ConstantFortyTwo.AsTypeInfoHot();
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
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_CcwBasicSubjects_DualInterfaceImpl, {});
		chaos_object->header.type_info = chaos_mt_CcwBasicSubjects_DualInterfaceImpl.AsTypeInfoHot();
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
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_CcwBasicSubjects_SimpleMath, {});
		chaos_object->header.type_info = chaos_mt_CcwBasicSubjects_SimpleMath.AsTypeInfoHot();
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
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_CcwBasicSubjects_SimpleMath, {});
		chaos_object->header.type_info = chaos_mt_CcwBasicSubjects_SimpleMath.AsTypeInfoHot();
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
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0{};
	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		ChaosObjectCtor(chaos_arg_0);
	}
	return;
}

// Managed method: CcwBasicSubjects/ConstantFortyTwo::GetValue()
extern "C" CHAOS_IL2CPP_INT32 CcwBasicSubjects_ConstantFortyTwo_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0{};
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: CcwBasicSubjects/DualInterfaceImpl::.ctor()
extern "C" void CcwBasicSubjects_DualInterfaceImpl__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0{};
	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		ChaosObjectCtor(chaos_arg_0);
	}
	return;
}

// Managed method: CcwBasicSubjects/DualInterfaceImpl::Add(System.Int32,System.Int32)
extern "C" CHAOS_IL2CPP_INT32 CcwBasicSubjects_DualInterfaceImpl_Add_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);
	chaos_args[2] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_2);
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
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
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0{};
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(100);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: CcwBasicSubjects/DualInterfaceImpl::Multiply(System.Int32,System.Int32)
extern "C" CHAOS_IL2CPP_INT32 CcwBasicSubjects_DualInterfaceImpl_Multiply_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);
	chaos_args[2] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_2);
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
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
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0{};
	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		ChaosObjectCtor(chaos_arg_0);
	}
	return;
}

// Managed method: CcwBasicSubjects/SimpleMath::Add(System.Int32,System.Int32)
extern "C" CHAOS_IL2CPP_INT32 CcwBasicSubjects_SimpleMath_Add_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);
	chaos_args[2] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_2);
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
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
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);
	chaos_args[2] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_2);
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	_s0 = chaos_args[1];
	_s1 = chaos_args[2];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapMul(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}



}  // namespace chaos::il2cpp::codegen::CcwBasicSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 19;

extern "C" void ChaosJitRegisterAll() {}