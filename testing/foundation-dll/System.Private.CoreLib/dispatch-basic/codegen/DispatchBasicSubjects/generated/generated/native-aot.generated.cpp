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
#include "enum_stubs.h"

// Forward declaration for dispatch table entries (defined in runtime_stubs.cpp)
extern "C" void InterpreterEntryDirect(
    CHAOS_IL2CPP_UINTPTR method_key,
    void*     args_buf,
    void*     ret_buf) noexcept;

#pragma warning(push)
#pragma warning(disable: 4065 4244)

// Forward declarations (pipeline fix: used before extern "C" decl)
extern "C" CHAOS_IL2CPP_INT32 DispatchBasicSubjects_BasicImpl_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void DispatchBasicSubjects_BasicImpl__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 DispatchBasicSubjects_Calculator_Add_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INT32 DispatchBasicSubjects_Calculator_Subtract_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" void DispatchBasicSubjects_Calculator__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 DispatchBasicSubjects_DispatchBasicNativeEntry_Run_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 DispatchBasicSubjects_DispatchBasicNativeEntry_TestDualGetStatus(void);
extern "C" CHAOS_IL2CPP_INT32 DispatchBasicSubjects_DispatchBasicNativeEntry_TestIDispatchAdd(void);
extern "C" CHAOS_IL2CPP_INT32 DispatchBasicSubjects_DispatchBasicNativeEntry_TestIDispatchSub(void);
extern "C" CHAOS_IL2CPP_INT32 DispatchBasicSubjects_DispatchBasicNativeEntry_TestIUnknownGetValue(void);
extern "C" void DispatchBasicSubjects_DispatchBasicSubjects_Subject_0(void);
extern "C" void DispatchBasicSubjects_DispatchBasicSubjects_Subject_1(void);
extern "C" void DispatchBasicSubjects_DispatchBasicSubjects_Subject_2(void);
extern "C" void DispatchBasicSubjects_DispatchBasicSubjects_Subject_3(void);
extern "C" void DispatchBasicSubjects_DispatchBasicSubjects_Subject_4(void);
extern "C" CHAOS_IL2CPP_INT32 DispatchBasicSubjects_StatusProvider_GetStatusCode(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void DispatchBasicSubjects_StatusProvider__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void DispatchBasicSubjects_System_Runtime_InteropServices_ComInterfaceTypeAttribute__ctor_ComInterfaceType(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);

namespace chaos::il2cpp::codegen::DispatchBasicSubjects {

// Bring runtime_core and jit declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;
using namespace chaos::il2cpp::jit;

// Runtime prelude dependencies included at translation unit level


const void* chaos_vtable_DispatchBasicSubjects_BasicImpl[];
const void* chaos_vtable_DispatchBasicSubjects_Calculator[];
const void* chaos_vtable_DispatchBasicSubjects_StatusProvider[];
inline MethodTable chaos_mt_System_Private_CoreLib_System_Object = {nullptr, nullptr, 15228727185366376748ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_INTPTR>(15228727185366376748ULL);
inline MethodTable chaos_mt_DispatchBasicSubjects_BasicImpl = {reinterpret_cast<const MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Object), chaos_vtable_DispatchBasicSubjects_BasicImpl, 5722114412902280165ULL, 2u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_DispatchBasicSubjects_BasicImpl = static_cast<CHAOS_IL2CPP_INTPTR>(5722114412902280165ULL);
inline MethodTable chaos_mt_DispatchBasicSubjects_Calculator = {reinterpret_cast<const MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Object), chaos_vtable_DispatchBasicSubjects_Calculator, 5786799786101565947ULL, 4u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_DispatchBasicSubjects_Calculator = static_cast<CHAOS_IL2CPP_INTPTR>(5786799786101565947ULL);
inline MethodTable chaos_mt_DispatchBasicSubjects_DispatchBasicSubjects = {nullptr, nullptr, 13619514316044602496ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_DispatchBasicSubjects_DispatchBasicSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(13619514316044602496ULL);
inline MethodTable chaos_mt_DispatchBasicSubjects_StatusProvider = {reinterpret_cast<const MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Object), chaos_vtable_DispatchBasicSubjects_StatusProvider, 11732973143578276300ULL, 5u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_DispatchBasicSubjects_StatusProvider = static_cast<CHAOS_IL2CPP_INTPTR>(11732973143578276300ULL);
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

// ── Virtual method table arrays ──
extern "C" void DispatchBasicSubjects_BasicImpl__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 DispatchBasicSubjects_BasicImpl_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
const void* chaos_vtable_DispatchBasicSubjects_BasicImpl[] =
{
	reinterpret_cast<void*>(DispatchBasicSubjects_BasicImpl__ctor),
	reinterpret_cast<void*>(DispatchBasicSubjects_BasicImpl_GetValue),
};
extern "C" void DispatchBasicSubjects_Calculator__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 DispatchBasicSubjects_Calculator_Add_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INT32 DispatchBasicSubjects_Calculator_Subtract_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
const void* chaos_vtable_DispatchBasicSubjects_Calculator[] =
{
	reinterpret_cast<void*>(DispatchBasicSubjects_Calculator__ctor),
	nullptr,
	reinterpret_cast<void*>(DispatchBasicSubjects_Calculator_Add_System_Int32_System_Int32),
	reinterpret_cast<void*>(DispatchBasicSubjects_Calculator_Subtract_System_Int32_System_Int32),
};
extern "C" void DispatchBasicSubjects_StatusProvider__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 DispatchBasicSubjects_StatusProvider_GetStatusCode(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
const void* chaos_vtable_DispatchBasicSubjects_StatusProvider[] =
{
	reinterpret_cast<void*>(DispatchBasicSubjects_StatusProvider__ctor),
	nullptr,
	nullptr,
	nullptr,
	reinterpret_cast<void*>(DispatchBasicSubjects_StatusProvider_GetStatusCode),
};
static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_DispatchBasicSubjects_BasicImpl[] =
{
	{ 0x0000001Au, reinterpret_cast<void*>(&DispatchBasicSubjects_BasicImpl__ctor) },
	{ 0x00000019u, reinterpret_cast<void*>(&DispatchBasicSubjects_BasicImpl_GetValue) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_DispatchBasicSubjects_Calculator[] =
{
	{ 0x00000016u, reinterpret_cast<void*>(&DispatchBasicSubjects_Calculator__ctor) },
	{ 0u, nullptr },
	{ 0x00000014u, reinterpret_cast<void*>(&DispatchBasicSubjects_Calculator_Add_System_Int32_System_Int32) },
	{ 0x00000015u, reinterpret_cast<void*>(&DispatchBasicSubjects_Calculator_Subtract_System_Int32_System_Int32) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_DispatchBasicSubjects_StatusProvider[] =
{
	{ 0x00000018u, reinterpret_cast<void*>(&DispatchBasicSubjects_StatusProvider__ctor) },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0x00000017u, reinterpret_cast<void*>(&DispatchBasicSubjects_StatusProvider_GetStatusCode) },
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

struct chaos_type_DispatchBasicSubjects_BasicImpl : public chaos_type_System_Private_CoreLib_System_Object
{
};

struct chaos_type_DispatchBasicSubjects_Calculator : public chaos_type_System_Private_CoreLib_System_Object
{
};

struct chaos_type_DispatchBasicSubjects_DispatchBasicSubjects
{
	ThinLockableHeader header{};
};

struct chaos_type_DispatchBasicSubjects_StatusProvider : public chaos_type_System_Private_CoreLib_System_Object
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

	registry.Register(15228727185366376748ULL, sizeof(chaos_type_System_Private_CoreLib_System_Object), nullptr, 0);
	registry.Register(5722114412902280165ULL, sizeof(chaos_type_DispatchBasicSubjects_BasicImpl), nullptr, 0);
	registry.Register(5786799786101565947ULL, sizeof(chaos_type_DispatchBasicSubjects_Calculator), nullptr, 0);
	registry.Register(13619514316044602496ULL, sizeof(chaos_type_DispatchBasicSubjects_DispatchBasicSubjects), nullptr, 0);
	registry.Register(11732973143578276300ULL, sizeof(chaos_type_DispatchBasicSubjects_StatusProvider), nullptr, 0);
	registry.Register(5474029880995115448ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_Assembly), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_Assembly, 1);
	registry.Register(17082367815459723707ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName, 1);
	registry.Register(4137207361503509124ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, 5);
	registry.Register(17040031516751226236ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, 2);
	registry.Register(10748947813473285525ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, 5);
	registry.Register(1782325859292956794ULL, sizeof(chaos_type_System_Private_CoreLib_System_String), nullptr, 0);

	// Register MethodTable address ranges for IsValidTypeInfoPointer.
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Object), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Object) + sizeof(chaos_mt_System_Private_CoreLib_System_Object));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_DispatchBasicSubjects_BasicImpl), reinterpret_cast<uintptr_t>(&chaos_mt_DispatchBasicSubjects_BasicImpl) + sizeof(chaos_mt_DispatchBasicSubjects_BasicImpl));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_DispatchBasicSubjects_Calculator), reinterpret_cast<uintptr_t>(&chaos_mt_DispatchBasicSubjects_Calculator) + sizeof(chaos_mt_DispatchBasicSubjects_Calculator));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_DispatchBasicSubjects_DispatchBasicSubjects), reinterpret_cast<uintptr_t>(&chaos_mt_DispatchBasicSubjects_DispatchBasicSubjects) + sizeof(chaos_mt_DispatchBasicSubjects_DispatchBasicSubjects));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_DispatchBasicSubjects_StatusProvider), reinterpret_cast<uintptr_t>(&chaos_mt_DispatchBasicSubjects_StatusProvider) + sizeof(chaos_mt_DispatchBasicSubjects_StatusProvider));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_Assembly), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_Assembly) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_Assembly));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_String), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_String) + sizeof(chaos_mt_System_Private_CoreLib_System_String));
}

CHAOS_IL2CPP_INT32 chaos_static_DispatchBasicSubjects_DispatchBasicSubjects___exitCode = 0;

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

// Bridge/import thunk for: DispatchBasicSubjects/DispatchBasicSubjects::_exitCode
extern "C" void chaos_bridge_thunk_0(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[0])();
}

// Bridge/import thunk for: System.Private.CoreLib/System.Attribute::.ctor:System.Void()
extern "C" void chaos_bridge_thunk_1(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[2])();
}

extern "C" void DispatchBasicSubjects_DispatchBasicSubjects_Subject_0(void);
extern "C" void DispatchBasicSubjects_DispatchBasicSubjects_Subject_1(void);
extern "C" void DispatchBasicSubjects_DispatchBasicSubjects_Subject_2(void);
extern "C" void DispatchBasicSubjects_DispatchBasicSubjects_Subject_3(void);
extern "C" void DispatchBasicSubjects_DispatchBasicSubjects_Subject_4(void);
extern "C" void DispatchBasicSubjects_BasicImpl__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 DispatchBasicSubjects_BasicImpl_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void DispatchBasicSubjects_Calculator__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 DispatchBasicSubjects_Calculator_Add_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INT32 DispatchBasicSubjects_Calculator_Subtract_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INT32 DispatchBasicSubjects_DispatchBasicNativeEntry_Run_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 DispatchBasicSubjects_DispatchBasicNativeEntry_TestDualGetStatus(void);
extern "C" CHAOS_IL2CPP_INT32 DispatchBasicSubjects_DispatchBasicNativeEntry_TestIDispatchAdd(void);
extern "C" CHAOS_IL2CPP_INT32 DispatchBasicSubjects_DispatchBasicNativeEntry_TestIDispatchSub(void);
extern "C" CHAOS_IL2CPP_INT32 DispatchBasicSubjects_DispatchBasicNativeEntry_TestIUnknownGetValue(void);
extern "C" void DispatchBasicSubjects_StatusProvider__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 DispatchBasicSubjects_StatusProvider_GetStatusCode(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void DispatchBasicSubjects_System_Runtime_InteropServices_ComInterfaceTypeAttribute__ctor_ComInterfaceType(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[19] = {
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
	5u,
	5u,
	5u,
	5u,
	5u,
	5u,
	5u,
	6u,
};

static constexpr struct {
	::ChaosAbiManifestV0 header;
	::ChaosAbiMethodEntryV0 entries[18];
	CHAOS_IL2CPP_UINT8 params[6];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		18u,
		6u,
		3898181747u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // DispatchBasicSubjects_DispatchBasicSubjects_Subject_0
		{ 0u, 0u },  // DispatchBasicSubjects_DispatchBasicSubjects_Subject_1
		{ 0u, 0u },  // DispatchBasicSubjects_DispatchBasicSubjects_Subject_2
		{ 0u, 0u },  // DispatchBasicSubjects_DispatchBasicSubjects_Subject_3
		{ 0u, 0u },  // DispatchBasicSubjects_DispatchBasicSubjects_Subject_4
		{ 0u, 0u },  // DispatchBasicSubjects_BasicImpl__ctor
		{ 1u, 0u },  // DispatchBasicSubjects_BasicImpl_GetValue
		{ 0u, 0u },  // DispatchBasicSubjects_Calculator__ctor
		{ 1u, 2u },  // DispatchBasicSubjects_Calculator_Add_System_Int32_System_Int32
		{ 1u, 2u },  // DispatchBasicSubjects_Calculator_Subtract_System_Int32_System_Int32
		{ 1u, 1u },  // DispatchBasicSubjects_DispatchBasicNativeEntry_Run_System_Int32
		{ 1u, 0u },  // DispatchBasicSubjects_DispatchBasicNativeEntry_TestDualGetStatus
		{ 1u, 0u },  // DispatchBasicSubjects_DispatchBasicNativeEntry_TestIDispatchAdd
		{ 1u, 0u },  // DispatchBasicSubjects_DispatchBasicNativeEntry_TestIDispatchSub
		{ 1u, 0u },  // DispatchBasicSubjects_DispatchBasicNativeEntry_TestIUnknownGetValue
		{ 0u, 0u },  // DispatchBasicSubjects_StatusProvider__ctor
		{ 1u, 0u },  // DispatchBasicSubjects_StatusProvider_GetStatusCode
		{ 0u, 1u },  // DispatchBasicSubjects_System_Runtime_InteropServices_ComInterfaceTypeAttribute__ctor_ComInterfaceType
	},
	{
		1u,
		1u,
		1u,
		1u,
		1u,
		2u,
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
		1556u,
		1536u,
		1536u,
		1536u,
		1548u,
		1544u,
	};

	static constexpr const char* s_type_names[10] = {
		"<Module>",
		"DispatchBasicSubjects",
		"ICalculator",
		"IStatusProvider",
		"IBasicOp",
		"Calculator",
		"StatusProvider",
		"BasicImpl",
		"DispatchBasicNativeEntry",
		"ComInterfaceTypeAttribute",
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
		"System.Runtime.InteropServices",
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
		&chaos_mt_DispatchBasicSubjects_DispatchBasicSubjects.hot,
		nullptr,
		nullptr,
		nullptr,
		&chaos_mt_DispatchBasicSubjects_Calculator.hot,
		&chaos_mt_DispatchBasicSubjects_StatusProvider.hot,
		&chaos_mt_DispatchBasicSubjects_BasicImpl.hot,
		nullptr,
		nullptr,
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
		0u,
		0u,
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
		/* .name_utf8         = */ "DispatchBasicSubjects",
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
		/* .type_count        = */ 10u,
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
		::chaos::il2cpp::runtime_core::RegisterModule("DispatchBasicSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[18] = {
	{ ".ctor", 0x0000001Au, 0u },  // BasicImpl
	{ "GetValue", 0x00000019u, 0u },  // BasicImpl
	{ ".ctor", 0x00000016u, 0u },  // Calculator
	{ "Add", 0x00000014u, 2u },  // Calculator
	{ "Subtract", 0x00000015u, 2u },  // Calculator
	{ "Run", 0x0000001Bu, 1u },  // DispatchBasicNativeEntry
	{ "TestDualGetStatus", 0x0000001Eu, 0u },  // DispatchBasicNativeEntry
	{ "TestIDispatchAdd", 0x0000001Cu, 0u },  // DispatchBasicNativeEntry
	{ "TestIDispatchSub", 0x0000001Du, 0u },  // DispatchBasicNativeEntry
	{ "TestIUnknownGetValue", 0x0000001Fu, 0u },  // DispatchBasicNativeEntry
	{ "Subject_0", 0x0000000Bu, 0u },  // DispatchBasicSubjects
	{ "Subject_1", 0x0000000Cu, 0u },  // DispatchBasicSubjects
	{ "Subject_2", 0x0000000Du, 0u },  // DispatchBasicSubjects
	{ "Subject_3", 0x0000000Eu, 0u },  // DispatchBasicSubjects
	{ "Subject_4", 0x0000000Fu, 0u },  // DispatchBasicSubjects
	{ ".ctor", 0x00000018u, 0u },  // StatusProvider
	{ "GetStatusCode", 0x00000017u, 0u },  // StatusProvider
	{ ".ctor", 0x00000020u, 1u },  // System.Runtime.InteropServices.ComInterfaceTypeAttribute
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[6] = {
	{ "BasicImpl", "", 0u, 2u },
	{ "Calculator", "", 2u, 3u },
	{ "DispatchBasicNativeEntry", "", 5u, 5u },
	{ "DispatchBasicSubjects", "", 10u, 5u },
	{ "StatusProvider", "", 15u, 2u },
	{ "System.Runtime.InteropServices.ComInterfaceTypeAttribute", "System.Runtime.InteropServices", 17u, 1u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[18] = {
	{ 0x0000000Bu, 0u },
	{ 0x0000000Cu, 1u },
	{ 0x0000000Du, 2u },
	{ 0x0000000Eu, 3u },
	{ 0x0000000Fu, 4u },
	{ 0x00000014u, 8u },
	{ 0x00000015u, 9u },
	{ 0x00000016u, 7u },
	{ 0x00000017u, 16u },
	{ 0x00000018u, 15u },
	{ 0x00000019u, 6u },
	{ 0x0000001Au, 5u },
	{ 0x0000001Bu, 10u },
	{ 0x0000001Cu, 12u },
	{ 0x0000001Du, 13u },
	{ 0x0000001Eu, 11u },
	{ 0x0000001Fu, 14u },
	{ 0x00000020u, 17u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[18] = {
	{ reinterpret_cast<void*>(&DispatchBasicSubjects_DispatchBasicSubjects_Subject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // DispatchBasicSubjects::Subject_0
	{ reinterpret_cast<void*>(&DispatchBasicSubjects_DispatchBasicSubjects_Subject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // DispatchBasicSubjects::Subject_1
	{ reinterpret_cast<void*>(&DispatchBasicSubjects_DispatchBasicSubjects_Subject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // DispatchBasicSubjects::Subject_2
	{ reinterpret_cast<void*>(&DispatchBasicSubjects_DispatchBasicSubjects_Subject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // DispatchBasicSubjects::Subject_3
	{ reinterpret_cast<void*>(&DispatchBasicSubjects_DispatchBasicSubjects_Subject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // DispatchBasicSubjects::Subject_4
	{ reinterpret_cast<void*>(&DispatchBasicSubjects_BasicImpl__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // BasicImpl::.ctor
	{ reinterpret_cast<void*>(&DispatchBasicSubjects_BasicImpl_GetValue), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // BasicImpl::GetValue
	{ reinterpret_cast<void*>(&DispatchBasicSubjects_Calculator__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // Calculator::.ctor
	{ reinterpret_cast<void*>(&DispatchBasicSubjects_Calculator_Add_System_Int32_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // Calculator::Add
	{ reinterpret_cast<void*>(&DispatchBasicSubjects_Calculator_Subtract_System_Int32_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // Calculator::Subtract
	{ reinterpret_cast<void*>(&DispatchBasicSubjects_DispatchBasicNativeEntry_Run_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // DispatchBasicNativeEntry::Run
	{ reinterpret_cast<void*>(&DispatchBasicSubjects_DispatchBasicNativeEntry_TestDualGetStatus), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // DispatchBasicNativeEntry::TestDualGetStatus
	{ reinterpret_cast<void*>(&DispatchBasicSubjects_DispatchBasicNativeEntry_TestIDispatchAdd), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // DispatchBasicNativeEntry::TestIDispatchAdd
	{ reinterpret_cast<void*>(&DispatchBasicSubjects_DispatchBasicNativeEntry_TestIDispatchSub), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // DispatchBasicNativeEntry::TestIDispatchSub
	{ reinterpret_cast<void*>(&DispatchBasicSubjects_DispatchBasicNativeEntry_TestIUnknownGetValue), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // DispatchBasicNativeEntry::TestIUnknownGetValue
	{ reinterpret_cast<void*>(&DispatchBasicSubjects_StatusProvider__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // StatusProvider::.ctor
	{ reinterpret_cast<void*>(&DispatchBasicSubjects_StatusProvider_GetStatusCode), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // StatusProvider::GetStatusCode
	{ reinterpret_cast<void*>(&DispatchBasicSubjects_System_Runtime_InteropServices_ComInterfaceTypeAttribute__ctor_ComInterfaceType), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // System.Runtime.InteropServices.ComInterfaceTypeAttribute::.ctor
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"DispatchBasicSubjects",
	s_hotpatch_types,
	6u,
	s_hotpatch_methods,
	18u,
	s_hotpatch_slots,
	18u,
	s_hotpatch_entries,
	18u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[3] = {
	"DispatchBasicSubjects/DispatchBasicSubjects::_exitCode",
	"System.Private.CoreLib/System.Object::.ctor:System.Void()",
	"System.Private.CoreLib/System.Attribute::.ctor:System.Void()",
};

extern "C" void* kChaosExternalRuntimeFnTable[3] = {
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__),
	nullptr,
};

extern "C" int32_t kChaosExternalRuntimeCount = 3;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[18])() = {
	reinterpret_cast<void(*)()>(&DispatchBasicSubjects_DispatchBasicSubjects_Subject_0),
	reinterpret_cast<void(*)()>(&DispatchBasicSubjects_DispatchBasicSubjects_Subject_1),
	reinterpret_cast<void(*)()>(&DispatchBasicSubjects_DispatchBasicSubjects_Subject_2),
	reinterpret_cast<void(*)()>(&DispatchBasicSubjects_DispatchBasicSubjects_Subject_3),
	reinterpret_cast<void(*)()>(&DispatchBasicSubjects_DispatchBasicSubjects_Subject_4),
	reinterpret_cast<void(*)()>(&DispatchBasicSubjects_BasicImpl__ctor),
	reinterpret_cast<void(*)()>(&DispatchBasicSubjects_BasicImpl_GetValue),
	reinterpret_cast<void(*)()>(&DispatchBasicSubjects_Calculator__ctor),
	reinterpret_cast<void(*)()>(&DispatchBasicSubjects_Calculator_Add_System_Int32_System_Int32),
	reinterpret_cast<void(*)()>(&DispatchBasicSubjects_Calculator_Subtract_System_Int32_System_Int32),
	reinterpret_cast<void(*)()>(&DispatchBasicSubjects_DispatchBasicNativeEntry_Run_System_Int32),
	reinterpret_cast<void(*)()>(&DispatchBasicSubjects_DispatchBasicNativeEntry_TestDualGetStatus),
	reinterpret_cast<void(*)()>(&DispatchBasicSubjects_DispatchBasicNativeEntry_TestIDispatchAdd),
	reinterpret_cast<void(*)()>(&DispatchBasicSubjects_DispatchBasicNativeEntry_TestIDispatchSub),
	reinterpret_cast<void(*)()>(&DispatchBasicSubjects_DispatchBasicNativeEntry_TestIUnknownGetValue),
	reinterpret_cast<void(*)()>(&DispatchBasicSubjects_StatusProvider__ctor),
	reinterpret_cast<void(*)()>(&DispatchBasicSubjects_StatusProvider_GetStatusCode),
	reinterpret_cast<void(*)()>(&DispatchBasicSubjects_System_Runtime_InteropServices_ComInterfaceTypeAttribute__ctor_ComInterfaceType),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
extern "C" void (*kBenchmarkWrappers[18])() = {
	[]() {kAotMethods[0]();},
	[]() {kAotMethods[1]();},
	[]() {kAotMethods[2]();},
	[]() {kAotMethods[3]();},
	[]() {kAotMethods[4]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[5])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[6])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[7])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[8])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0,0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[9])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0,0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[10])(0);},
	[]() {kAotMethods[11]();},
	[]() {kAotMethods[12]();},
	[]() {kAotMethods[13]();},
	[]() {kAotMethods[14]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[15])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[16])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[17])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
};

// ── Subject entry index mapping ─────────────────────────────────
// Maps subject index (0-based sequential) to kAotMethod index.
// Used by runtime-entry.cpp to route --benchmark N to the correct
// AOT method slot, since kAotMethods[] includes lambdas/closures
// that shift subject methods to non-contiguous indices.
extern "C" const int kSubjectEntryCount = 5;
extern "C" const int kSubjectEntryIndices[5] = {
	0,
	1,
	2,
	3,
	4,
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

	/* ── Entry 0: DispatchBasicSubjects_DispatchBasicSubjects_Subject_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry0;
	/* ── Entry 1: DispatchBasicSubjects_DispatchBasicSubjects_Subject_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry1;
	/* ── Entry 2: DispatchBasicSubjects_DispatchBasicSubjects_Subject_2 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry2;
	/* ── Entry 3: DispatchBasicSubjects_DispatchBasicSubjects_Subject_3 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry3;
	/* ── Entry 4: DispatchBasicSubjects_DispatchBasicSubjects_Subject_4 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry4;
	/* ── Entry 5: DispatchBasicSubjects_BasicImpl__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry5;
	/* ── Entry 6: DispatchBasicSubjects_BasicImpl_GetValue ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry6;
	/* ── Entry 7: DispatchBasicSubjects_Calculator__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry7;
	/* ── Entry 8: DispatchBasicSubjects_Calculator_Add_System_Int32_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry8;
	/* ── Entry 9: DispatchBasicSubjects_Calculator_Subtract_System_Int32_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry9;
	/* ── Entry 10: DispatchBasicSubjects_DispatchBasicNativeEntry_Run_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry10;
	/* ── Entry 11: DispatchBasicSubjects_DispatchBasicNativeEntry_TestDualGetStatus ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry11;
	/* ── Entry 12: DispatchBasicSubjects_DispatchBasicNativeEntry_TestIDispatchAdd ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry12;
	/* ── Entry 13: DispatchBasicSubjects_DispatchBasicNativeEntry_TestIDispatchSub ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry13;
	/* ── Entry 14: DispatchBasicSubjects_DispatchBasicNativeEntry_TestIUnknownGetValue ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry14;
	/* ── Entry 15: DispatchBasicSubjects_StatusProvider__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry15;
	/* ── Entry 16: DispatchBasicSubjects_StatusProvider_GetStatusCode ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry16;
	/* ── Entry 17: DispatchBasicSubjects_System_Runtime_InteropServices_ComInterfaceTypeAttribute__ctor_ComInterfaceType ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry17;
} kChaosGcSlotMapsSection = {
	/* entry0 = DispatchBasicSubjects_DispatchBasicSubjects_Subject_0 */
	.entry0 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&DispatchBasicSubjects_DispatchBasicSubjects_Subject_0),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry1 = DispatchBasicSubjects_DispatchBasicSubjects_Subject_1 */
	.entry1 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&DispatchBasicSubjects_DispatchBasicSubjects_Subject_1),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry2 = DispatchBasicSubjects_DispatchBasicSubjects_Subject_2 */
	.entry2 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&DispatchBasicSubjects_DispatchBasicSubjects_Subject_2),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry3 = DispatchBasicSubjects_DispatchBasicSubjects_Subject_3 */
	.entry3 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&DispatchBasicSubjects_DispatchBasicSubjects_Subject_3),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry4 = DispatchBasicSubjects_DispatchBasicSubjects_Subject_4 */
	.entry4 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&DispatchBasicSubjects_DispatchBasicSubjects_Subject_4),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry5 = DispatchBasicSubjects_BasicImpl__ctor */
	.entry5 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&DispatchBasicSubjects_BasicImpl__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry6 = DispatchBasicSubjects_BasicImpl_GetValue */
	.entry6 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&DispatchBasicSubjects_BasicImpl_GetValue),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry7 = DispatchBasicSubjects_Calculator__ctor */
	.entry7 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&DispatchBasicSubjects_Calculator__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry8 = DispatchBasicSubjects_Calculator_Add_System_Int32_System_Int32 */
	.entry8 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&DispatchBasicSubjects_Calculator_Add_System_Int32_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 24u, 32u }
	},
	/* entry9 = DispatchBasicSubjects_Calculator_Subtract_System_Int32_System_Int32 */
	.entry9 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&DispatchBasicSubjects_Calculator_Subtract_System_Int32_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 24u, 32u }
	},
	/* entry10 = DispatchBasicSubjects_DispatchBasicNativeEntry_Run_System_Int32 */
	.entry10 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&DispatchBasicSubjects_DispatchBasicNativeEntry_Run_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 8u, 16u, 24u, 32u }
	},
	/* entry11 = DispatchBasicSubjects_DispatchBasicNativeEntry_TestDualGetStatus */
	.entry11 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&DispatchBasicSubjects_DispatchBasicNativeEntry_TestDualGetStatus),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry12 = DispatchBasicSubjects_DispatchBasicNativeEntry_TestIDispatchAdd */
	.entry12 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&DispatchBasicSubjects_DispatchBasicNativeEntry_TestIDispatchAdd),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry13 = DispatchBasicSubjects_DispatchBasicNativeEntry_TestIDispatchSub */
	.entry13 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&DispatchBasicSubjects_DispatchBasicNativeEntry_TestIDispatchSub),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry14 = DispatchBasicSubjects_DispatchBasicNativeEntry_TestIUnknownGetValue */
	.entry14 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&DispatchBasicSubjects_DispatchBasicNativeEntry_TestIUnknownGetValue),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry15 = DispatchBasicSubjects_StatusProvider__ctor */
	.entry15 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&DispatchBasicSubjects_StatusProvider__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry16 = DispatchBasicSubjects_StatusProvider_GetStatusCode */
	.entry16 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&DispatchBasicSubjects_StatusProvider_GetStatusCode),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry17 = DispatchBasicSubjects_System_Runtime_InteropServices_ComInterfaceTypeAttribute__ctor_ComInterfaceType */
	.entry17 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&DispatchBasicSubjects_System_Runtime_InteropServices_ComInterfaceTypeAttribute__ctor_ComInterfaceType),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 576u;

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[18] = {
	reinterpret_cast<void*>(&DispatchBasicSubjects_DispatchBasicSubjects_Subject_0),
	reinterpret_cast<void*>(&DispatchBasicSubjects_DispatchBasicSubjects_Subject_1),
	reinterpret_cast<void*>(&DispatchBasicSubjects_DispatchBasicSubjects_Subject_2),
	reinterpret_cast<void*>(&DispatchBasicSubjects_DispatchBasicSubjects_Subject_3),
	reinterpret_cast<void*>(&DispatchBasicSubjects_DispatchBasicSubjects_Subject_4),
	reinterpret_cast<void*>(&DispatchBasicSubjects_BasicImpl__ctor),
	reinterpret_cast<void*>(&DispatchBasicSubjects_BasicImpl_GetValue),
	reinterpret_cast<void*>(&DispatchBasicSubjects_Calculator__ctor),
	reinterpret_cast<void*>(&DispatchBasicSubjects_Calculator_Add_System_Int32_System_Int32),
	reinterpret_cast<void*>(&DispatchBasicSubjects_Calculator_Subtract_System_Int32_System_Int32),
	reinterpret_cast<void*>(&DispatchBasicSubjects_DispatchBasicNativeEntry_Run_System_Int32),
	reinterpret_cast<void*>(&DispatchBasicSubjects_DispatchBasicNativeEntry_TestDualGetStatus),
	reinterpret_cast<void*>(&DispatchBasicSubjects_DispatchBasicNativeEntry_TestIDispatchAdd),
	reinterpret_cast<void*>(&DispatchBasicSubjects_DispatchBasicNativeEntry_TestIDispatchSub),
	reinterpret_cast<void*>(&DispatchBasicSubjects_DispatchBasicNativeEntry_TestIUnknownGetValue),
	reinterpret_cast<void*>(&DispatchBasicSubjects_StatusProvider__ctor),
	reinterpret_cast<void*>(&DispatchBasicSubjects_StatusProvider_GetStatusCode),
	reinterpret_cast<void*>(&DispatchBasicSubjects_System_Runtime_InteropServices_ComInterfaceTypeAttribute__ctor_ComInterfaceType),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
// ── VTable descriptors (for BootstrapRuntime TypeVTable registration) ──
static const VTableDescriptorV0 kChaosVTableDescriptors[] = {
	{
		CHAOS_IL2CPP_UINT64_C(0x4F6908BDD12EEBE5),
		0x00000007u,
		0u,
		2u,
		reinterpret_cast<const void*>(kSlots_DispatchBasicSubjects_BasicImpl),
		reinterpret_cast<const void**>(chaos_vtable_DispatchBasicSubjects_BasicImpl),
		2u,
		1,
		{0, 0, 0},
		nullptr,
		0u,
	},
	{
		CHAOS_IL2CPP_UINT64_C(0x504ED7BEE4E3E9FB),
		0x00000005u,
		0u,
		4u,
		reinterpret_cast<const void*>(kSlots_DispatchBasicSubjects_Calculator),
		reinterpret_cast<const void**>(chaos_vtable_DispatchBasicSubjects_Calculator),
		4u,
		1,
		{0, 0, 0},
		nullptr,
		0u,
	},
	{
		CHAOS_IL2CPP_UINT64_C(0xA2D3E4E8FE2E71CC),
		0x00000006u,
		0u,
		5u,
		reinterpret_cast<const void*>(kSlots_DispatchBasicSubjects_StatusProvider),
		reinterpret_cast<const void**>(chaos_vtable_DispatchBasicSubjects_StatusProvider),
		5u,
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
	.method_pointer_count      = 18u,
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
	.image_name_utf8    = "DispatchBasicSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_DispatchBasicSubjects_DispatchBasicSubjects[1] = {
	{ 0x04000001u, "DispatchBasicSubjects/DispatchBasicSubjects::_exitCode", "_exitCode", "System.Int32", 0LL, 3u },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_DispatchBasicSubjects_DispatchBasicSubjects[5] = {
	{ 0x0000000Bu, "DispatchBasicSubjects/DispatchBasicSubjects::Subject_0:System.Void()", "Subject_0", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Cu, "DispatchBasicSubjects/DispatchBasicSubjects::Subject_1:System.Void()", "Subject_1", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Du, "DispatchBasicSubjects/DispatchBasicSubjects::Subject_2:System.Void()", "Subject_2", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Eu, "DispatchBasicSubjects/DispatchBasicSubjects::Subject_3:System.Void()", "Subject_3", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Fu, "DispatchBasicSubjects/DispatchBasicSubjects::Subject_4:System.Void()", "Subject_4", "System.Void", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_DispatchBasicSubjects_BasicImpl[2] = {
	{ 0x0000001Au, "DispatchBasicSubjects/BasicImpl::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000019u, "DispatchBasicSubjects/BasicImpl::GetValue:System.Int32()", "GetValue", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_DispatchBasicSubjects_Calculator[3] = {
	{ 0x00000016u, "DispatchBasicSubjects/Calculator::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000014u, "DispatchBasicSubjects/Calculator::Add:System.Int32(System.Int32,System.Int32)", "Add", "System.Int32", 2, nullptr, 0u, nullptr, 0u },
	{ 0x00000015u, "DispatchBasicSubjects/Calculator::Subtract:System.Int32(System.Int32,System.Int32)", "Subtract", "System.Int32", 2, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_DispatchBasicSubjects_DispatchBasicNativeEntry[5] = {
	{ 0x0000001Bu, "DispatchBasicSubjects/DispatchBasicNativeEntry::Run:System.Int32(System.Int32)", "Run", "System.Int32", 1, nullptr, 0u, nullptr, 0u },
	{ 0x0000001Eu, "DispatchBasicSubjects/DispatchBasicNativeEntry::TestDualGetStatus:System.Int32()", "TestDualGetStatus", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000001Cu, "DispatchBasicSubjects/DispatchBasicNativeEntry::TestIDispatchAdd:System.Int32()", "TestIDispatchAdd", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000001Du, "DispatchBasicSubjects/DispatchBasicNativeEntry::TestIDispatchSub:System.Int32()", "TestIDispatchSub", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000001Fu, "DispatchBasicSubjects/DispatchBasicNativeEntry::TestIUnknownGetValue:System.Int32()", "TestIUnknownGetValue", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_DispatchBasicSubjects_StatusProvider[2] = {
	{ 0x00000018u, "DispatchBasicSubjects/StatusProvider::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000017u, "DispatchBasicSubjects/StatusProvider::GetStatusCode:System.Int32()", "GetStatusCode", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_DispatchBasicSubjects_System_Runtime_InteropServices_ComInterfaceTypeAttribute[1] = {
	{ 0x00000020u, "DispatchBasicSubjects/System.Runtime.InteropServices.ComInterfaceTypeAttribute::.ctor:System.Void(ComInterfaceType)", ".ctor", "System.Void", 1, nullptr, 0u, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[6] = {
	{ 0x00000001u, "DispatchBasicSubjects/DispatchBasicSubjects", "DispatchBasicSubjects/DispatchBasicSubjects", "", "DispatchBasicSubjects", "DispatchBasicSubjects", nullptr, kReflFields_DispatchBasicSubjects_DispatchBasicSubjects, 1u, nullptr, 0u,
nullptr, 0u,    kReflMethods_DispatchBasicSubjects_DispatchBasicSubjects, 5u, nullptr, 0u, 0u },
	{ 0x00000007u, "DispatchBasicSubjects/BasicImpl", "DispatchBasicSubjects/BasicImpl", "", "BasicImpl", "BasicImpl", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_DispatchBasicSubjects_BasicImpl, 2u, nullptr, 0u, 0u },
	{ 0x00000005u, "DispatchBasicSubjects/Calculator", "DispatchBasicSubjects/Calculator", "", "Calculator", "Calculator", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_DispatchBasicSubjects_Calculator, 3u, nullptr, 0u, 0u },
	{ 0x00000008u, "DispatchBasicSubjects/DispatchBasicNativeEntry", "DispatchBasicSubjects/DispatchBasicNativeEntry", "", "DispatchBasicNativeEntry", "DispatchBasicNativeEntry", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_DispatchBasicSubjects_DispatchBasicNativeEntry, 5u, nullptr, 0u, 0u },
	{ 0x00000006u, "DispatchBasicSubjects/StatusProvider", "DispatchBasicSubjects/StatusProvider", "", "StatusProvider", "StatusProvider", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_DispatchBasicSubjects_StatusProvider, 2u, nullptr, 0u, 0u },
	{ 0x00000009u, "DispatchBasicSubjects/System.Runtime.InteropServices.ComInterfaceTypeAttribute", "DispatchBasicSubjects/System.Runtime.InteropServices.ComInterfaceTypeAttribute", "System.Runtime.InteropServices", "System.Runtime.InteropServices.ComInterfaceTypeAttribute", "System.Runtime.InteropServices.ComInterfaceTypeAttribute", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_DispatchBasicSubjects_System_Runtime_InteropServices_ComInterfaceTypeAttribute, 1u, nullptr, 0u, 0u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[6] = {
	&kReflTypes[0],
	&kReflTypes[1],
	&kReflTypes[2],
	&kReflTypes[3],
	&kReflTypes[4],
	&kReflTypes[5],
};

static const ReflectionQueryImageDescriptor kReflImage = { "DispatchBasicSubjects", kReflTypePtrs, 6u, 1, 0, 0, 0 };

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
// Managed method: DispatchBasicSubjects/DispatchBasicSubjects::Subject_0()
extern "C" void DispatchBasicSubjects_DispatchBasicSubjects_Subject_0(void)
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
				auto& _d10 = s_hotpatch_entries[10];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d10)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d10))
				{
					alignas(16) uint8_t _d_ab[4];
					ArgBuffer _d_bw(_d_ab);
					_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d10.method_key, _d_ab, &_d_hpresult);
				}
				else
				{
					_d_hpresult = DispatchBasicSubjects_DispatchBasicNativeEntry_Run_System_Int32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_0 = _s1;
				auto& _d10 = s_hotpatch_entries[10];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d10)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d10))
				{
					alignas(16) uint8_t _d_ab[4];
					ArgBuffer _d_bw(_d_ab);
					_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d10.method_key, _d_ab, &_d_hpresult);
				}
				else
				{
					_d_hpresult = DispatchBasicSubjects_DispatchBasicNativeEntry_Run_System_Int32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
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
				chaos_static_DispatchBasicSubjects_DispatchBasicSubjects___exitCode = chaos_value;
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
				chaos_static_DispatchBasicSubjects_DispatchBasicSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: DispatchBasicSubjects/DispatchBasicSubjects::Subject_1()
extern "C" void DispatchBasicSubjects_DispatchBasicSubjects_Subject_1(void)
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
				// Inlined: DispatchBasicSubjects/DispatchBasicNativeEntry::TestIDispatchAdd:System.Int32()
				{
					auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_DispatchBasicSubjects_Calculator, {});
					chaos_object->header.type_info = &chaos_mt_DispatchBasicSubjects_Calculator.hot;
					DispatchBasicSubjects_Calculator__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
					_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
				}
				chaos_locals[1] = _s0;
				_s0 = chaos_locals[1];
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(20);
				_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(7);
				{
					// Inlined: DispatchBasicSubjects/Calculator::Add:System.Int32(System.Int32,System.Int32)
					auto chaos_inline_arg_2 = _s2;
					auto chaos_inline_arg_1 = _s1;
					auto chaos_inline_arg_0 = _s0;
					_s0 = chaos_inline_arg_1;
					_s1 = chaos_inline_arg_2;
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
				}
				chaos_locals[2] = _s0;
				// br (handled via structured EH branches)
				_s0 = chaos_locals[2];
			}
			{
				// Inlined: DispatchBasicSubjects/DispatchBasicNativeEntry::TestIDispatchAdd:System.Int32()
				{
					auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_DispatchBasicSubjects_Calculator, {});
					chaos_object->header.type_info = &chaos_mt_DispatchBasicSubjects_Calculator.hot;
					DispatchBasicSubjects_Calculator__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
					_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
				}
				chaos_locals[1] = _s1;
				_s1 = chaos_locals[1];
				_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(20);
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(7);
				{
					// Inlined: DispatchBasicSubjects/Calculator::Add:System.Int32(System.Int32,System.Int32)
					auto chaos_inline_arg_2 = _s3;
					auto chaos_inline_arg_1 = _s2;
					auto chaos_inline_arg_0 = _s1;
					_s1 = chaos_inline_arg_1;
					_s2 = chaos_inline_arg_2;
					_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s1), static_cast<CHAOS_IL2CPP_INT32>(_s2)));
				}
				chaos_locals[2] = _s1;
				// br (handled via structured EH branches)
				_s1 = chaos_locals[2];
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
				chaos_static_DispatchBasicSubjects_DispatchBasicSubjects___exitCode = chaos_value;
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
				chaos_static_DispatchBasicSubjects_DispatchBasicSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: DispatchBasicSubjects/DispatchBasicSubjects::Subject_2()
extern "C" void DispatchBasicSubjects_DispatchBasicSubjects_Subject_2(void)
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
				// Inlined: DispatchBasicSubjects/DispatchBasicNativeEntry::TestIDispatchSub:System.Int32()
				{
					auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_DispatchBasicSubjects_Calculator, {});
					chaos_object->header.type_info = &chaos_mt_DispatchBasicSubjects_Calculator.hot;
					DispatchBasicSubjects_Calculator__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
					_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
				}
				chaos_locals[1] = _s0;
				_s0 = chaos_locals[1];
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(50);
				_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(8);
				{
					// Inlined: DispatchBasicSubjects/Calculator::Subtract:System.Int32(System.Int32,System.Int32)
					auto chaos_inline_arg_2 = _s2;
					auto chaos_inline_arg_1 = _s1;
					auto chaos_inline_arg_0 = _s0;
					_s0 = chaos_inline_arg_1;
					_s1 = chaos_inline_arg_2;
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapSub(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
				}
				chaos_locals[2] = _s0;
				// br (handled via structured EH branches)
				_s0 = chaos_locals[2];
			}
			{
				// Inlined: DispatchBasicSubjects/DispatchBasicNativeEntry::TestIDispatchSub:System.Int32()
				{
					auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_DispatchBasicSubjects_Calculator, {});
					chaos_object->header.type_info = &chaos_mt_DispatchBasicSubjects_Calculator.hot;
					DispatchBasicSubjects_Calculator__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
					_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
				}
				chaos_locals[1] = _s1;
				_s1 = chaos_locals[1];
				_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(50);
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(8);
				{
					// Inlined: DispatchBasicSubjects/Calculator::Subtract:System.Int32(System.Int32,System.Int32)
					auto chaos_inline_arg_2 = _s3;
					auto chaos_inline_arg_1 = _s2;
					auto chaos_inline_arg_0 = _s1;
					_s1 = chaos_inline_arg_1;
					_s2 = chaos_inline_arg_2;
					_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapSub(static_cast<CHAOS_IL2CPP_INT32>(_s1), static_cast<CHAOS_IL2CPP_INT32>(_s2)));
				}
				chaos_locals[2] = _s1;
				// br (handled via structured EH branches)
				_s1 = chaos_locals[2];
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
				chaos_static_DispatchBasicSubjects_DispatchBasicSubjects___exitCode = chaos_value;
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
				chaos_static_DispatchBasicSubjects_DispatchBasicSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: DispatchBasicSubjects/DispatchBasicSubjects::Subject_3()
extern "C" void DispatchBasicSubjects_DispatchBasicSubjects_Subject_3(void)
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
				// Inlined: DispatchBasicSubjects/DispatchBasicNativeEntry::TestDualGetStatus:System.Int32()
				{
					auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_DispatchBasicSubjects_StatusProvider, {});
					chaos_object->header.type_info = &chaos_mt_DispatchBasicSubjects_StatusProvider.hot;
					DispatchBasicSubjects_StatusProvider__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
					_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
				}
				chaos_locals[1] = _s0;
				_s0 = chaos_locals[1];
				{
					// Inlined: DispatchBasicSubjects/StatusProvider::GetStatusCode:System.Int32()
					auto chaos_inline_arg_0 = _s0;
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(200);
				}
				chaos_locals[2] = _s0;
				// br (handled via structured EH branches)
				_s0 = chaos_locals[2];
			}
			{
				// Inlined: DispatchBasicSubjects/DispatchBasicNativeEntry::TestDualGetStatus:System.Int32()
				{
					auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_DispatchBasicSubjects_StatusProvider, {});
					chaos_object->header.type_info = &chaos_mt_DispatchBasicSubjects_StatusProvider.hot;
					DispatchBasicSubjects_StatusProvider__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
					_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
				}
				chaos_locals[1] = _s1;
				_s1 = chaos_locals[1];
				{
					// Inlined: DispatchBasicSubjects/StatusProvider::GetStatusCode:System.Int32()
					auto chaos_inline_arg_0 = _s1;
					_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(200);
				}
				chaos_locals[2] = _s1;
				// br (handled via structured EH branches)
				_s1 = chaos_locals[2];
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
				chaos_static_DispatchBasicSubjects_DispatchBasicSubjects___exitCode = chaos_value;
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
				chaos_static_DispatchBasicSubjects_DispatchBasicSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: DispatchBasicSubjects/DispatchBasicSubjects::Subject_4()
extern "C" void DispatchBasicSubjects_DispatchBasicSubjects_Subject_4(void)
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
				// Inlined: DispatchBasicSubjects/DispatchBasicNativeEntry::TestIUnknownGetValue:System.Int32()
				{
					auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_DispatchBasicSubjects_BasicImpl, {});
					chaos_object->header.type_info = &chaos_mt_DispatchBasicSubjects_BasicImpl.hot;
					DispatchBasicSubjects_BasicImpl__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
					_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
				}
				chaos_locals[1] = _s0;
				_s0 = chaos_locals[1];
				{
					// Inlined: DispatchBasicSubjects/BasicImpl::GetValue:System.Int32()
					auto chaos_inline_arg_0 = _s0;
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
				}
				chaos_locals[2] = _s0;
				// br (handled via structured EH branches)
				_s0 = chaos_locals[2];
			}
			{
				// Inlined: DispatchBasicSubjects/DispatchBasicNativeEntry::TestIUnknownGetValue:System.Int32()
				{
					auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_DispatchBasicSubjects_BasicImpl, {});
					chaos_object->header.type_info = &chaos_mt_DispatchBasicSubjects_BasicImpl.hot;
					DispatchBasicSubjects_BasicImpl__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
					_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
				}
				chaos_locals[1] = _s1;
				_s1 = chaos_locals[1];
				{
					// Inlined: DispatchBasicSubjects/BasicImpl::GetValue:System.Int32()
					auto chaos_inline_arg_0 = _s1;
					_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
				}
				chaos_locals[2] = _s1;
				// br (handled via structured EH branches)
				_s1 = chaos_locals[2];
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
				chaos_static_DispatchBasicSubjects_DispatchBasicSubjects___exitCode = chaos_value;
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
				chaos_static_DispatchBasicSubjects_DispatchBasicSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: DispatchBasicSubjects/BasicImpl::.ctor()
extern "C" void DispatchBasicSubjects_BasicImpl__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: DispatchBasicSubjects/BasicImpl::GetValue()
extern "C" CHAOS_IL2CPP_INT32 DispatchBasicSubjects_BasicImpl_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: DispatchBasicSubjects/Calculator::.ctor()
extern "C" void DispatchBasicSubjects_Calculator__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: DispatchBasicSubjects/Calculator::Add(System.Int32,System.Int32)
extern "C" CHAOS_IL2CPP_INT32 DispatchBasicSubjects_Calculator_Add_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2)
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

// Managed method: DispatchBasicSubjects/Calculator::Subtract(System.Int32,System.Int32)
extern "C" CHAOS_IL2CPP_INT32 DispatchBasicSubjects_Calculator_Subtract_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2)
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
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapSub(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: DispatchBasicSubjects/DispatchBasicNativeEntry::Run(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 DispatchBasicSubjects_DispatchBasicNativeEntry_Run_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
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
					// Inlined: DispatchBasicSubjects/DispatchBasicNativeEntry::TestIDispatchAdd:System.Int32()
					{
						auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_DispatchBasicSubjects_Calculator, {});
						chaos_object->header.type_info = &chaos_mt_DispatchBasicSubjects_Calculator.hot;
						DispatchBasicSubjects_Calculator__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
						_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
					}
					chaos_locals[3] = _s0;
					_s0 = chaos_locals[3];
					_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(20);
					_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(7);
					{
						// Inlined: DispatchBasicSubjects/Calculator::Add:System.Int32(System.Int32,System.Int32)
						auto chaos_inline_arg_2 = _s2;
						auto chaos_inline_arg_1 = _s1;
						auto chaos_inline_arg_0 = _s0;
						_s0 = chaos_inline_arg_1;
						_s1 = chaos_inline_arg_2;
						_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
					}
					chaos_locals[4] = _s0;
					// br (handled via structured EH branches)
					_s0 = chaos_locals[4];
				}
				chaos_locals[2] = _s0;
			case 1:
				{
					// Inlined: DispatchBasicSubjects/DispatchBasicNativeEntry::TestIDispatchSub:System.Int32()
					{
						auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_DispatchBasicSubjects_Calculator, {});
						chaos_object->header.type_info = &chaos_mt_DispatchBasicSubjects_Calculator.hot;
						DispatchBasicSubjects_Calculator__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
						_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
					}
					chaos_locals[3] = _s0;
					_s0 = chaos_locals[3];
					_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(50);
					_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(8);
					{
						// Inlined: DispatchBasicSubjects/Calculator::Subtract:System.Int32(System.Int32,System.Int32)
						auto chaos_inline_arg_2 = _s2;
						auto chaos_inline_arg_1 = _s1;
						auto chaos_inline_arg_0 = _s0;
						_s0 = chaos_inline_arg_1;
						_s1 = chaos_inline_arg_2;
						_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapSub(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
					}
					chaos_locals[4] = _s0;
					// br (handled via structured EH branches)
					_s0 = chaos_locals[4];
				}
				chaos_locals[2] = _s0;
			case 2:
				{
					// Inlined: DispatchBasicSubjects/DispatchBasicNativeEntry::TestDualGetStatus:System.Int32()
					{
						auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_DispatchBasicSubjects_StatusProvider, {});
						chaos_object->header.type_info = &chaos_mt_DispatchBasicSubjects_StatusProvider.hot;
						DispatchBasicSubjects_StatusProvider__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
						_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
					}
					chaos_locals[3] = _s0;
					_s0 = chaos_locals[3];
					{
						// Inlined: DispatchBasicSubjects/StatusProvider::GetStatusCode:System.Int32()
						auto chaos_inline_arg_0 = _s0;
						_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(200);
					}
					chaos_locals[4] = _s0;
					// br (handled via structured EH branches)
					_s0 = chaos_locals[4];
				}
				chaos_locals[2] = _s0;
			case 3:
			{
				{
					// Inlined: DispatchBasicSubjects/DispatchBasicNativeEntry::TestIUnknownGetValue:System.Int32()
					{
						auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_DispatchBasicSubjects_BasicImpl, {});
						chaos_object->header.type_info = &chaos_mt_DispatchBasicSubjects_BasicImpl.hot;
						DispatchBasicSubjects_BasicImpl__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
						_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
					}
					chaos_locals[3] = _s0;
					_s0 = chaos_locals[3];
					{
						// Inlined: DispatchBasicSubjects/BasicImpl::GetValue:System.Int32()
						auto chaos_inline_arg_0 = _s0;
						_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
					}
					chaos_locals[4] = _s0;
					// br (handled via structured EH branches)
					_s0 = chaos_locals[4];
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

// Managed method: DispatchBasicSubjects/DispatchBasicNativeEntry::TestDualGetStatus()
extern "C" CHAOS_IL2CPP_INT32 DispatchBasicSubjects_DispatchBasicNativeEntry_TestDualGetStatus(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_DispatchBasicSubjects_StatusProvider, {});
		chaos_object->header.type_info = &chaos_mt_DispatchBasicSubjects_StatusProvider.hot;
		DispatchBasicSubjects_StatusProvider__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		// Inlined: DispatchBasicSubjects/StatusProvider::GetStatusCode:System.Int32()
		auto chaos_inline_arg_0 = _s0;
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(200);
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: DispatchBasicSubjects/DispatchBasicNativeEntry::TestIDispatchAdd()
extern "C" CHAOS_IL2CPP_INT32 DispatchBasicSubjects_DispatchBasicNativeEntry_TestIDispatchAdd(void)
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
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_DispatchBasicSubjects_Calculator, {});
		chaos_object->header.type_info = &chaos_mt_DispatchBasicSubjects_Calculator.hot;
		DispatchBasicSubjects_Calculator__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(20);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(7);
	{
		// Inlined: DispatchBasicSubjects/Calculator::Add:System.Int32(System.Int32,System.Int32)
		auto chaos_inline_arg_2 = _s2;
		auto chaos_inline_arg_1 = _s1;
		auto chaos_inline_arg_0 = _s0;
		_s0 = chaos_inline_arg_1;
		_s1 = chaos_inline_arg_2;
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: DispatchBasicSubjects/DispatchBasicNativeEntry::TestIDispatchSub()
extern "C" CHAOS_IL2CPP_INT32 DispatchBasicSubjects_DispatchBasicNativeEntry_TestIDispatchSub(void)
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
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_DispatchBasicSubjects_Calculator, {});
		chaos_object->header.type_info = &chaos_mt_DispatchBasicSubjects_Calculator.hot;
		DispatchBasicSubjects_Calculator__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(50);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(8);
	{
		// Inlined: DispatchBasicSubjects/Calculator::Subtract:System.Int32(System.Int32,System.Int32)
		auto chaos_inline_arg_2 = _s2;
		auto chaos_inline_arg_1 = _s1;
		auto chaos_inline_arg_0 = _s0;
		_s0 = chaos_inline_arg_1;
		_s1 = chaos_inline_arg_2;
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapSub(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: DispatchBasicSubjects/DispatchBasicNativeEntry::TestIUnknownGetValue()
extern "C" CHAOS_IL2CPP_INT32 DispatchBasicSubjects_DispatchBasicNativeEntry_TestIUnknownGetValue(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_DispatchBasicSubjects_BasicImpl, {});
		chaos_object->header.type_info = &chaos_mt_DispatchBasicSubjects_BasicImpl.hot;
		DispatchBasicSubjects_BasicImpl__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		// Inlined: DispatchBasicSubjects/BasicImpl::GetValue:System.Int32()
		auto chaos_inline_arg_0 = _s0;
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: DispatchBasicSubjects/StatusProvider::.ctor()
extern "C" void DispatchBasicSubjects_StatusProvider__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: DispatchBasicSubjects/StatusProvider::GetStatusCode()
extern "C" CHAOS_IL2CPP_INT32 DispatchBasicSubjects_StatusProvider_GetStatusCode(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(200);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: DispatchBasicSubjects/System.Runtime.InteropServices.ComInterfaceTypeAttribute::.ctor(ComInterfaceType)
extern "C" void DispatchBasicSubjects_System_Runtime_InteropServices_ComInterfaceTypeAttribute__ctor_ComInterfaceType(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[0];
	{
		reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[2])();
	}
	return;
}



}  // namespace chaos::il2cpp::codegen::DispatchBasicSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 18;

extern "C" void ChaosJitRegisterAll() {}