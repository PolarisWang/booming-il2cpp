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
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_0(void);
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_1(void);
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_10(void);
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_11(void);
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_12(void);
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_2(void);
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_3(void);
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_4(void);
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_5(void);
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_6(void);
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_7(void);
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_8(void);
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_9(void);
extern "C" CHAOS_IL2CPP_INT32 ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c_Subject_3_b__4_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c_cctor(void);

// Forward declarations (pipeline fix: used before extern "C" decl)
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_0(void);
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_1(void);
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_10(void);
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_11(void);
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_12(void);
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_2(void);
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_3(void);
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_4(void);
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_5(void);
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_6(void);
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_7(void);
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_8(void);
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_9(void);
extern "C" CHAOS_IL2CPP_INT32 ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c_Subject_3_b__4_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c_cctor(void);

namespace chaos::il2cpp::codegen::ArrayIndexingCopySubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


const void* chaos_vtable_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c[];
inline TypeInfoV0 chaos_mt_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects = {{nullptr, nullptr, 18134131238671616544ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects = static_cast<CHAOS_IL2CPP_INTPTR>(18134131238671616544ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Object = {{nullptr, nullptr, 15228727185366376748ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_INTPTR>(15228727185366376748ULL);
inline TypeInfoV0 chaos_mt_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c, 11884632647042276952ULL, 2u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c = static_cast<CHAOS_IL2CPP_INTPTR>(11884632647042276952ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Byte = {{nullptr, nullptr, 3217969471912086765ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Byte = static_cast<CHAOS_IL2CPP_INTPTR>(3217969471912086765ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Delegate = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, nullptr, 7451128447593600616ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Delegate = static_cast<CHAOS_IL2CPP_INTPTR>(7451128447593600616ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_MulticastDelegate = {{&chaos_mt_System_Private_CoreLib_System_Delegate.hot, nullptr, 6681393039041505440ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_MulticastDelegate = static_cast<CHAOS_IL2CPP_INTPTR>(6681393039041505440ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Func_System_Int32_ = {{&chaos_mt_System_Private_CoreLib_System_MulticastDelegate.hot, nullptr, 8866194404114377402ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Func_System_Int32_ = static_cast<CHAOS_IL2CPP_INTPTR>(8866194404114377402ULL);
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

// ── Virtual method table arrays ──
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c_Subject_3_b__4_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
const void* chaos_vtable_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c[] =
{
	reinterpret_cast<void*>(ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c__ctor),
	reinterpret_cast<void*>(ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c_Subject_3_b__4_0),
};
static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c[] =
{
	{ 0x00000014u, reinterpret_cast<void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c__ctor) },
	{ 0x00000015u, reinterpret_cast<void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c_Subject_3_b__4_0) },
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

struct chaos_type_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Private_CoreLib_System_Object
{
	ThinLockableHeader header{};
};

struct chaos_type_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c : public chaos_type_System_Private_CoreLib_System_Object
{
};

struct chaos_type_System_Private_CoreLib_System_Byte
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


// ── GcTypeLayout pointer offset arrays (computed via offsetof) ──
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Delegate[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Delegate, chaos_delegate_target)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Delegate, chaos_delegate_invocation_list))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_MulticastDelegate[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_MulticastDelegate, chaos_delegate_target)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_MulticastDelegate, chaos_delegate_invocation_list))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_Assembly[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_Assembly, runtime_assembly_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, declaring_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, runtime_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, generic_definition_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, generic_argument_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, declaring_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, declaring_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, runtime_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, generic_definition_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, generic_argument_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, runtime_name_value))};

extern "C" void ChaosRegisterGcLayouts() {
	auto& registry = chaos::il2cpp::runtime_core::GcLayoutRegistry::Instance();

	registry.Register(18134131238671616544ULL, sizeof(chaos_type_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects), nullptr, 0);
	registry.Register(15228727185366376748ULL, sizeof(chaos_type_System_Private_CoreLib_System_Object), nullptr, 0);
	registry.Register(11884632647042276952ULL, sizeof(chaos_type_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c), nullptr, 0);
	registry.Register(3217969471912086765ULL, sizeof(chaos_type_System_Private_CoreLib_System_Byte), nullptr, 0);
	registry.Register(7451128447593600616ULL, sizeof(chaos_type_System_Private_CoreLib_System_Delegate), kGcOffsets_chaos_type_System_Private_CoreLib_System_Delegate, 2);
	registry.Register(6681393039041505440ULL, sizeof(chaos_type_System_Private_CoreLib_System_MulticastDelegate), kGcOffsets_chaos_type_System_Private_CoreLib_System_MulticastDelegate, 2);
	registry.Register(8866194404114377402ULL, sizeof(chaos_type_System_Private_CoreLib_System_Func_System_Int32_), nullptr, 0);
	registry.Register(11009693519287992193ULL, sizeof(chaos_type_System_Private_CoreLib_System_Int32), nullptr, 0);
	registry.Register(5474029880995115448ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_Assembly), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_Assembly, 1);
	registry.Register(17082367815459723707ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName, 1);
	registry.Register(4137207361503509124ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, 5);
	registry.Register(17040031516751226236ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, 2);
	registry.Register(10748947813473285525ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, 5);
	registry.Register(1782325859292956794ULL, sizeof(chaos_type_System_Private_CoreLib_System_String), nullptr, 0);

	// Register TypeInfoV0 address ranges for IsValidTypeInfoPointer.
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects), reinterpret_cast<uintptr_t>(&chaos_mt_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects) + sizeof(chaos_mt_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Object), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Object) + sizeof(chaos_mt_System_Private_CoreLib_System_Object));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c), reinterpret_cast<uintptr_t>(&chaos_mt_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c) + sizeof(chaos_mt_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Byte), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Byte) + sizeof(chaos_mt_System_Private_CoreLib_System_Byte));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Delegate), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Delegate) + sizeof(chaos_mt_System_Private_CoreLib_System_Delegate));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_MulticastDelegate), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_MulticastDelegate) + sizeof(chaos_mt_System_Private_CoreLib_System_MulticastDelegate));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Func_System_Int32_), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Func_System_Int32_) + sizeof(chaos_mt_System_Private_CoreLib_System_Func_System_Int32_));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Int32), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Int32) + sizeof(chaos_mt_System_Private_CoreLib_System_Int32));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_Assembly), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_Assembly) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_Assembly));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_String), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_String) + sizeof(chaos_mt_System_Private_CoreLib_System_String));
}

CHAOS_IL2CPP_INTPTR chaos_static_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c____9 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c____9__4_0 = 0;
CHAOS_IL2CPP_INT32 chaos_static_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___exitCode = 0;

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Array__BinarySearch_System_Byte__System_Int32_System_Byte___System_Byte_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
	return ChaosArrayBinarySearch(chaos_arg_0, chaos_arg_1);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Array__BinarySearch_System_Byte__System_Int32_System_Byte___System_Int32_System_Int32_System_Byte_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2, CHAOS_IL2CPP_INTPTR chaos_arg_3)
{
	return ChaosArrayBinarySearchRange(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2), chaos_arg_3);
}

static void chaos_external_runtime_System_Private_CoreLib_System_Array__Clear_System_Void_System_Array_System_Int32_System_Int32_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2)
{
	ChaosArrayClear(chaos_fn_arg_0, chaos_fn_arg_1, chaos_fn_arg_2);
}

static void chaos_external_runtime_System_Private_CoreLib_System_Array__Copy_System_Void_System_Array_System_Array_System_Int32_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2)
{
	(void)chaos_arg_0; (void)chaos_arg_1; (void)chaos_arg_2;
}

static void chaos_external_runtime_System_Private_CoreLib_System_Array__Copy_System_Void_System_Array_System_Int32_System_Array_System_Int32_System_Int32_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2, CHAOS_IL2CPP_INT32 chaos_arg_3, CHAOS_IL2CPP_INT32 chaos_arg_4)
{
	ChaosArrayCopy(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3, chaos_arg_4);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Array__GetLength_System_Int32_System_Int32_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	return ChaosArrayGetLength(chaos_fn_arg_0, chaos_fn_arg_1);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Array__GetValue_System_Object_System_Int32_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1)
{
	return ChaosArrayGetValue(chaos_arg_0, chaos_arg_1);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Array__IndexOf_System_Byte__System_Int32_System_Byte___System_Byte_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
	return ChaosArrayIndexOf(chaos_arg_0, chaos_arg_1);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Array__LastIndexOf_System_Byte__System_Int32_System_Byte___System_Byte_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
	return ChaosArrayLastIndexOf(chaos_arg_0, chaos_arg_1);
}

static void chaos_external_runtime_System_Private_CoreLib_System_Array__Reverse_System_Byte__System_Void_System_Byte___(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
	ChaosArrayReverse(chaos_arg_0);
}

static void chaos_external_runtime_System_Private_CoreLib_System_Array__Sort_System_Byte__System_Void_System_Byte___(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
	ChaosArraySort(chaos_arg_0);
}

static void chaos_external_runtime_System_Private_CoreLib_System_Array__Sort_System_Byte__System_Void_System_Byte___System_Collections_Generic_IComparer_System_Byte__(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
	(void)chaos_arg_0; (void)chaos_arg_1;
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Object__GetHashCode_System_Int32__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosObjectGetHashCode(chaos_fn_arg_0);
}

static void chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	ChaosObjectCtor(chaos_fn_arg_0);
}

CHAOS_IL2CPP_ONCE_FLAG chaos_type_init_once_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c;

void chaos_ensure_type_initialized_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c()
{
	CHAOS_IL2CPP_CALL_ONCE(chaos_type_init_once_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c, []()
	{
	if (chaos_static_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c____9 == 0)
	{
	    auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c);
	    chaos_object->header.type_info = &chaos_mt_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c.hot;
		const auto chaos_arg_0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
		ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c__ctor(chaos_arg_0);
	    chaos_static_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c____9 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	});
}

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };


// ── Bridge/import thunks ──
extern "C" void* kChaosExternalRuntimeFnTable[];

// Bridge/import thunk for: System.Private.CoreLib/System.Byte
extern "C" void chaos_bridge_thunk_0(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[0])();
}

// Bridge/import thunk for: ArrayIndexingCopySubjects/ArrayIndexingCopySubjects::_exitCode
extern "C" void chaos_bridge_thunk_1(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[2])();
}

// Bridge/import thunk for: ArrayIndexingCopySubjects/ArrayIndexingCopySubjects+<>c::<>9__4_0
extern "C" void chaos_bridge_thunk_2(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[5])();
}

// Bridge/import thunk for: ArrayIndexingCopySubjects/ArrayIndexingCopySubjects+<>c::<>9
extern "C" void chaos_bridge_thunk_3(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[6])();
}

// Bridge/import thunk for: System.Private.CoreLib/System.Func<System.Int32>::.ctor:System.Void(System.Object,System.IntPtr)
extern "C" void chaos_bridge_thunk_4(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[7])(chaos_fn_arg_0, chaos_fn_arg_1);
}

// Bridge/import thunk for: System.Private.CoreLib/System.Func<System.Int32>::Invoke:System.Int32()
extern "C" CHAOS_IL2CPP_INT32 chaos_bridge_thunk_5(void)
{
	auto result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)()>(kChaosExternalRuntimeFnTable[8])();
	return result;
}

// Bridge/import thunk for: System.Private.CoreLib/System.Array::Resize<System.Byte>:System.Void(System.Byte[]&,System.Int32)
extern "C" void chaos_bridge_thunk_6(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[9])(chaos_fn_arg_0, chaos_fn_arg_1);
}

// Bridge/import thunk for: System.Private.CoreLib/System.Array::Empty<System.Byte>:System.Byte[]()
extern "C" CHAOS_IL2CPP_INTPTR chaos_bridge_thunk_7(void)
{
	auto result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)()>(kChaosExternalRuntimeFnTable[10])();
	return result;
}

// Bridge/import thunk for: System.Collections/System.Collections.Generic.Comparer<System.Byte>::get_Default:System.Collections.Generic.Comparer<System.Byte>()
extern "C" CHAOS_IL2CPP_INTPTR chaos_bridge_thunk_8(void)
{
	auto result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)()>(kChaosExternalRuntimeFnTable[12])();
	return result;
}

// Bridge/import thunk for: System.Private.CoreLib/System.Int32
extern "C" void chaos_bridge_thunk_9(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[19])();
}

extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_0(void);
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_1(void);
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_2(void);
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_3(void);
extern "C" CHAOS_IL2CPP_INT32 ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c_Subject_3_b__4_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_4(void);
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_5(void);
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_6(void);
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_7(void);
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_8(void);
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_9(void);
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_10(void);
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_11(void);
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_12(void);
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c_cctor(void);
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[17] = {
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
	::ChaosAbiMethodEntryV0 entries[16];
	CHAOS_IL2CPP_UINT8 params[1];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		16u,
		0u,
		1583872740u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_0
		{ 0u, 0u },  // ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_1
		{ 0u, 0u },  // ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_2
		{ 0u, 0u },  // ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_3
		{ 1u, 0u },  // ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c_Subject_3_b__4_0
		{ 0u, 0u },  // ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_4
		{ 0u, 0u },  // ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_5
		{ 0u, 0u },  // ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_6
		{ 0u, 0u },  // ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_7
		{ 0u, 0u },  // ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_8
		{ 0u, 0u },  // ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_9
		{ 0u, 0u },  // ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_10
		{ 0u, 0u },  // ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_11
		{ 0u, 0u },  // ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_12
		{ 0u, 0u },  // ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c_cctor
		{ 0u, 0u },  // ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c__ctor
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
		1544u,
	};

	static constexpr const char* s_type_names[3] = {
		"<Module>",
		"ArrayIndexingCopySubjects",
		"<>c",
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
		&chaos_mt_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects.hot,
		&chaos_mt_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c.hot,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_children[1] = {
		33554435u,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_offset[4] = {
		0u,
		0u,
		1u,
		1u,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_generic_param_constraint_offset[4] = {
		0u,
		0u,
		0u,
		0u,
	};



	static const ::chaos::il2cpp::runtime_core::ModuleDescriptor s_native_aot_module = {
		/* .name_utf8         = */ "ArrayIndexingCopySubjects",
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
		/* .type_count        = */ 3u,
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
		::chaos::il2cpp::runtime_core::RegisterModule("ArrayIndexingCopySubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[16] = {
	{ "Subject_0", 0x00000006u, 0u },  // ArrayIndexingCopySubjects
	{ "Subject_1", 0x00000007u, 0u },  // ArrayIndexingCopySubjects
	{ "Subject_2", 0x00000008u, 0u },  // ArrayIndexingCopySubjects
	{ "Subject_3", 0x00000009u, 0u },  // ArrayIndexingCopySubjects
	{ "Subject_4", 0x0000000Au, 0u },  // ArrayIndexingCopySubjects
	{ "Subject_5", 0x0000000Bu, 0u },  // ArrayIndexingCopySubjects
	{ "Subject_6", 0x0000000Cu, 0u },  // ArrayIndexingCopySubjects
	{ "Subject_7", 0x0000000Du, 0u },  // ArrayIndexingCopySubjects
	{ "Subject_8", 0x0000000Eu, 0u },  // ArrayIndexingCopySubjects
	{ "Subject_9", 0x0000000Fu, 0u },  // ArrayIndexingCopySubjects
	{ "Subject_10", 0x00000010u, 0u },  // ArrayIndexingCopySubjects
	{ "Subject_11", 0x00000011u, 0u },  // ArrayIndexingCopySubjects
	{ "Subject_12", 0x00000012u, 0u },  // ArrayIndexingCopySubjects
	{ "<Subject_3>b__4_0", 0x00000015u, 0u },  // ArrayIndexingCopySubjects+<>c
	{ ".cctor", 0x00000013u, 0u },  // ArrayIndexingCopySubjects+<>c
	{ ".ctor", 0x00000014u, 0u },  // ArrayIndexingCopySubjects+<>c
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[2] = {
	{ "ArrayIndexingCopySubjects", "", 0u, 13u },
	{ "ArrayIndexingCopySubjects+<>c", "", 13u, 3u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[16] = {
	{ 0x00000006u, 0u },
	{ 0x00000007u, 1u },
	{ 0x00000008u, 2u },
	{ 0x00000009u, 3u },
	{ 0x0000000Au, 5u },
	{ 0x0000000Bu, 6u },
	{ 0x0000000Cu, 7u },
	{ 0x0000000Du, 8u },
	{ 0x0000000Eu, 9u },
	{ 0x0000000Fu, 10u },
	{ 0x00000010u, 11u },
	{ 0x00000011u, 12u },
	{ 0x00000012u, 13u },
	{ 0x00000013u, 14u },
	{ 0x00000014u, 15u },
	{ 0x00000015u, 4u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[16] = {
	{ reinterpret_cast<void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ArrayIndexingCopySubjects::Subject_0
	{ reinterpret_cast<void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ArrayIndexingCopySubjects::Subject_1
	{ reinterpret_cast<void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ArrayIndexingCopySubjects::Subject_2
	{ reinterpret_cast<void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ArrayIndexingCopySubjects::Subject_3
	{ reinterpret_cast<void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c_Subject_3_b__4_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ArrayIndexingCopySubjects+<>c::<Subject_3>b__4_0
	{ reinterpret_cast<void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ArrayIndexingCopySubjects::Subject_4
	{ reinterpret_cast<void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ArrayIndexingCopySubjects::Subject_5
	{ reinterpret_cast<void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ArrayIndexingCopySubjects::Subject_6
	{ reinterpret_cast<void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ArrayIndexingCopySubjects::Subject_7
	{ reinterpret_cast<void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ArrayIndexingCopySubjects::Subject_8
	{ reinterpret_cast<void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_9), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ArrayIndexingCopySubjects::Subject_9
	{ reinterpret_cast<void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_10), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ArrayIndexingCopySubjects::Subject_10
	{ reinterpret_cast<void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_11), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ArrayIndexingCopySubjects::Subject_11
	{ reinterpret_cast<void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_12), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ArrayIndexingCopySubjects::Subject_12
	{ reinterpret_cast<void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c_cctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ArrayIndexingCopySubjects+<>c::.cctor
	{ reinterpret_cast<void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ArrayIndexingCopySubjects+<>c::.ctor
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"ArrayIndexingCopySubjects",
	s_hotpatch_types,
	2u,
	s_hotpatch_methods,
	16u,
	s_hotpatch_slots,
	16u,
	s_hotpatch_entries,
	16u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[24] = {
	"System.Private.CoreLib/System.Byte",
	"System.Private.CoreLib/System.Array::Copy:System.Void(System.Array,System.Array,System.Int32)",
	"ArrayIndexingCopySubjects/ArrayIndexingCopySubjects::_exitCode",
	"System.Private.CoreLib/System.Array::Copy:System.Void(System.Array,System.Int32,System.Array,System.Int32,System.Int32)",
	"System.Private.CoreLib/System.Array::Clear:System.Void(System.Array,System.Int32,System.Int32)",
	"ArrayIndexingCopySubjects/ArrayIndexingCopySubjects+<>c::<>9__4_0",
	"ArrayIndexingCopySubjects/ArrayIndexingCopySubjects+<>c::<>9",
	"System.Private.CoreLib/System.Func<System.Int32>::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Private.CoreLib/System.Func<System.Int32>::Invoke:System.Int32()",
	"System.Private.CoreLib/System.Array::Resize<System.Byte>:System.Void(System.Byte[]&,System.Int32)",
	"System.Private.CoreLib/System.Array::Empty<System.Byte>:System.Byte[]()",
	"System.Private.CoreLib/System.Array::Sort<System.Byte>:System.Void(System.Byte[])",
	"System.Collections/System.Collections.Generic.Comparer<System.Byte>::get_Default:System.Collections.Generic.Comparer<System.Byte>()",
	"System.Private.CoreLib/System.Array::Sort<System.Byte>:System.Void(System.Byte[],System.Collections.Generic.IComparer<System.Byte>)",
	"System.Private.CoreLib/System.Array::BinarySearch<System.Byte>:System.Int32(System.Byte[],System.Byte)",
	"System.Private.CoreLib/System.Array::BinarySearch<System.Byte>:System.Int32(System.Byte[],System.Int32,System.Int32,System.Byte)",
	"System.Private.CoreLib/System.Array::IndexOf<System.Byte>:System.Int32(System.Byte[],System.Byte)",
	"System.Private.CoreLib/System.Array::LastIndexOf<System.Byte>:System.Int32(System.Byte[],System.Byte)",
	"System.Private.CoreLib/System.Array::Reverse<System.Byte>:System.Void(System.Byte[])",
	"System.Private.CoreLib/System.Int32",
	"System.Private.CoreLib/System.Array::GetLength:System.Int32(System.Int32)",
	"System.Private.CoreLib/System.Array::GetValue:System.Object(System.Int32)",
	"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()",
	"System.Private.CoreLib/System.Object::.ctor:System.Void()",
};

extern "C" void* kChaosExternalRuntimeFnTable[24] = {
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Array__Copy_System_Void_System_Array_System_Array_System_Int32_),
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Array__Copy_System_Void_System_Array_System_Int32_System_Array_System_Int32_System_Int32_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Array__Clear_System_Void_System_Array_System_Int32_System_Int32_),
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Array__Sort_System_Byte__System_Void_System_Byte___),
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Array__Sort_System_Byte__System_Void_System_Byte___System_Collections_Generic_IComparer_System_Byte__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Array__BinarySearch_System_Byte__System_Int32_System_Byte___System_Byte_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Array__BinarySearch_System_Byte__System_Int32_System_Byte___System_Int32_System_Int32_System_Byte_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Array__IndexOf_System_Byte__System_Int32_System_Byte___System_Byte_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Array__LastIndexOf_System_Byte__System_Int32_System_Byte___System_Byte_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Array__Reverse_System_Byte__System_Void_System_Byte___),
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Array__GetLength_System_Int32_System_Int32_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Array__GetValue_System_Object_System_Int32_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Object__GetHashCode_System_Int32__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__),
};

extern "C" int32_t kChaosExternalRuntimeCount = 24;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[16])() = {
	reinterpret_cast<void(*)()>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_0),
	reinterpret_cast<void(*)()>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_1),
	reinterpret_cast<void(*)()>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_2),
	reinterpret_cast<void(*)()>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_3),
	reinterpret_cast<void(*)()>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c_Subject_3_b__4_0),
	reinterpret_cast<void(*)()>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_4),
	reinterpret_cast<void(*)()>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_5),
	reinterpret_cast<void(*)()>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_6),
	reinterpret_cast<void(*)()>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_7),
	reinterpret_cast<void(*)()>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_8),
	reinterpret_cast<void(*)()>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_9),
	reinterpret_cast<void(*)()>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_10),
	reinterpret_cast<void(*)()>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_11),
	reinterpret_cast<void(*)()>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_12),
	reinterpret_cast<void(*)()>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c_cctor),
	reinterpret_cast<void(*)()>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c__ctor),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
extern "C" void (*kBenchmarkWrappers[16])() = {
	[]() {kAotMethods[0]();},
	[]() {kAotMethods[1]();},
	[]() {kAotMethods[2]();},
	[]() {kAotMethods[3]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[4])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
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
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[15])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
};

// ── Subject entry index mapping ─────────────────────────────────
// Maps subject index (0-based sequential) to kAotMethod index.
// Used by runtime-entry.cpp to route --benchmark N to the correct
// AOT method slot, since kAotMethods[] includes lambdas/closures
// that shift subject methods to non-contiguous indices.
extern "C" const int kSubjectEntryCount = 13;
extern "C" const int kSubjectEntryIndices[13] = {
	0,
	1,
	2,
	3,
	5,
	6,
	7,
	8,
	9,
	10,
	11,
	12,
	13,
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

	/* ── Entry 0: ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry0;
	/* ── Entry 1: ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry1;
	/* ── Entry 2: ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_2 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry2;
	/* ── Entry 3: ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_3 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry3;
	/* ── Entry 4: ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c_Subject_3_b__4_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry4;
	/* ── Entry 5: ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_4 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry5;
	/* ── Entry 6: ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_5 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry6;
	/* ── Entry 7: ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_6 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry7;
	/* ── Entry 8: ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_7 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[8];
	} entry8;
	/* ── Entry 9: ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_8 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry9;
	/* ── Entry 10: ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_9 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry10;
	/* ── Entry 11: ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_10 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry11;
	/* ── Entry 12: ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_11 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry12;
	/* ── Entry 13: ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_12 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry13;
	/* ── Entry 14: ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c_cctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry14;
	/* ── Entry 15: ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry15;
} kChaosGcSlotMapsSection = {
	/* entry0 = ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_0 */
	.entry0 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_0),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry1 = ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_1 */
	.entry1 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_1),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry2 = ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_2 */
	.entry2 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_2),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry3 = ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_3 */
	.entry3 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_3),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry4 = ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c_Subject_3_b__4_0 */
	.entry4 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c_Subject_3_b__4_0),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry5 = ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_4 */
	.entry5 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_4),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry6 = ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_5 */
	.entry6 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_5),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry7 = ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_6 */
	.entry7 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_6),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry8 = ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_7 */
	.entry8 = {
		/* entry_total_size = 52 */ 52u,
		/* code_address */ reinterpret_cast<const void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_7),
		/* frame_size = 64 */ 64u,
		/* num_gc_slots = 8 */ 8u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u }
	},
	/* entry9 = ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_8 */
	.entry9 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_8),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry10 = ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_9 */
	.entry10 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_9),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry11 = ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_10 */
	.entry11 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_10),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry12 = ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_11 */
	.entry12 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_11),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry13 = ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_12 */
	.entry13 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_12),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry14 = ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c_cctor */
	.entry14 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c_cctor),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry15 = ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c__ctor */
	.entry15 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 604u;

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[16] = {
	reinterpret_cast<void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_0),
	reinterpret_cast<void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_1),
	reinterpret_cast<void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_2),
	reinterpret_cast<void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_3),
	reinterpret_cast<void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c_Subject_3_b__4_0),
	reinterpret_cast<void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_4),
	reinterpret_cast<void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_5),
	reinterpret_cast<void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_6),
	reinterpret_cast<void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_7),
	reinterpret_cast<void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_8),
	reinterpret_cast<void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_9),
	reinterpret_cast<void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_10),
	reinterpret_cast<void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_11),
	reinterpret_cast<void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_12),
	reinterpret_cast<void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c_cctor),
	reinterpret_cast<void*>(&ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c__ctor),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
// ── VTable descriptors (for BootstrapRuntime TypeVTable registration) ──
static const VTableDescriptorV0 kChaosVTableDescriptors[] = {
	{
		CHAOS_IL2CPP_UINT64_C(0xA4EEB26CCCF71E58),
		0x00000002u,
		0u,
		2u,
		reinterpret_cast<const void*>(kSlots_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c),
		reinterpret_cast<const void**>(chaos_vtable_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c),
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
	.method_pointer_count      = 16u,
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
	.image_name_utf8    = "ArrayIndexingCopySubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects[1] = {
	{ 0x04000001u, "ArrayIndexingCopySubjects/ArrayIndexingCopySubjects::_exitCode", "_exitCode", "System.Int32", 0LL, 3u },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects[13] = {
	{ 0x00000006u, "ArrayIndexingCopySubjects/ArrayIndexingCopySubjects::Subject_0:System.Void()", "Subject_0", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000007u, "ArrayIndexingCopySubjects/ArrayIndexingCopySubjects::Subject_1:System.Void()", "Subject_1", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000008u, "ArrayIndexingCopySubjects/ArrayIndexingCopySubjects::Subject_2:System.Void()", "Subject_2", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000009u, "ArrayIndexingCopySubjects/ArrayIndexingCopySubjects::Subject_3:System.Void()", "Subject_3", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Au, "ArrayIndexingCopySubjects/ArrayIndexingCopySubjects::Subject_4:System.Void()", "Subject_4", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Bu, "ArrayIndexingCopySubjects/ArrayIndexingCopySubjects::Subject_5:System.Void()", "Subject_5", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Cu, "ArrayIndexingCopySubjects/ArrayIndexingCopySubjects::Subject_6:System.Void()", "Subject_6", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Du, "ArrayIndexingCopySubjects/ArrayIndexingCopySubjects::Subject_7:System.Void()", "Subject_7", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Eu, "ArrayIndexingCopySubjects/ArrayIndexingCopySubjects::Subject_8:System.Void()", "Subject_8", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Fu, "ArrayIndexingCopySubjects/ArrayIndexingCopySubjects::Subject_9:System.Void()", "Subject_9", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000010u, "ArrayIndexingCopySubjects/ArrayIndexingCopySubjects::Subject_10:System.Void()", "Subject_10", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000011u, "ArrayIndexingCopySubjects/ArrayIndexingCopySubjects::Subject_11:System.Void()", "Subject_11", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000012u, "ArrayIndexingCopySubjects/ArrayIndexingCopySubjects::Subject_12:System.Void()", "Subject_12", "System.Void", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c[3] = {
	{ 0x00000015u, "ArrayIndexingCopySubjects/ArrayIndexingCopySubjects+<>c::<Subject_3>b__4_0:System.Int32()", "<Subject_3>b__4_0", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000013u, "ArrayIndexingCopySubjects/ArrayIndexingCopySubjects+<>c::.cctor:System.Void()", ".cctor", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000014u, "ArrayIndexingCopySubjects/ArrayIndexingCopySubjects+<>c::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[2] = {
	{ 0x00000001u, "ArrayIndexingCopySubjects/ArrayIndexingCopySubjects", "ArrayIndexingCopySubjects/ArrayIndexingCopySubjects", "", "ArrayIndexingCopySubjects", "ArrayIndexingCopySubjects", nullptr, kReflFields_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects, 1u, nullptr, 0u,
nullptr, 0u,    kReflMethods_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects, 13u, nullptr, 0u, 0u },
	{ 0x00000002u, "ArrayIndexingCopySubjects/ArrayIndexingCopySubjects+<>c", "ArrayIndexingCopySubjects/ArrayIndexingCopySubjects+<>c", "", "ArrayIndexingCopySubjects+<>c", "ArrayIndexingCopySubjects+<>c", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c, 3u, nullptr, 0u, 0u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[2] = {
	&kReflTypes[0],
	&kReflTypes[1],
};

static const ReflectionQueryImageDescriptor kReflImage = { "ArrayIndexingCopySubjects", kReflTypePtrs, 2u, 1, 0, 0, 0 };

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
// Managed method: ArrayIndexingCopySubjects/ArrayIndexingCopySubjects::Subject_0()
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_0(void)
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


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
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
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
			{
				const auto chaos_length = static_cast<CHAOS_IL2CPP_INT32>(_s1);
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
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
			}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
			{
				const auto chaos_arg_2 = _s2;
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				chaos_external_runtime_System_Private_CoreLib_System_Array__Copy_System_Void_System_Array_System_Array_System_Int32_(chaos_arg_0, chaos_arg_1, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2));
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
				chaos_static_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ArrayIndexingCopySubjects/ArrayIndexingCopySubjects::Subject_1()
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_1(void)
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
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
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
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
			{
				const auto chaos_length = static_cast<CHAOS_IL2CPP_INT32>(_s2);
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
				_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
			}
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
			{
				const auto chaos_arg_4 = _s4;
				const auto chaos_arg_3 = _s3;
				const auto chaos_arg_2 = _s2;
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				chaos_external_runtime_System_Private_CoreLib_System_Array__Copy_System_Void_System_Array_System_Int32_System_Array_System_Int32_System_Int32_(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1), chaos_arg_2, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_3), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_4));
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
				chaos_static_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ArrayIndexingCopySubjects/ArrayIndexingCopySubjects::Subject_2()
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_2(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
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
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
			{
				const auto chaos_arg_2 = _s2;
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				ChaosArrayClear(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2));
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
				chaos_static_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ArrayIndexingCopySubjects/ArrayIndexingCopySubjects::Subject_3()
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_3(void)
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
			chaos_ensure_type_initialized_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c();
			_s0 = chaos_static_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c____9__4_0;
			_s1 = _s0;
			{
				if (_s1 == 0)
				{
					chaos_ensure_type_initialized_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c();
					_s0 = chaos_static_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c____9;
					{
						// Hotpatch-aware ldftn wrapper (slot 4)
						static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> CHAOS_IL2CPP_INT32 {
							auto& _d_entry = s_hotpatch_entries[4];
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
							{
								alignas(16) uint8_t _d_ab[8];
								ArgBuffer _d_bw(_d_ab);
								_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
								CHAOS_IL2CPP_INT32 _d_ret{};
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d_entry.method_key, _d_ab, &_d_ret);
								return _d_ret;
							}
							return reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(_d_entry.direct_ptr)(chaos_fn_arg_0);
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
					chaos_ensure_type_initialized_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c();
						chaos_static_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c____9__4_0 = chaos_value;
					}
				}
				{
					const auto chaos_delegate_value = _s0;
					if (chaos_delegate_value == 0)
					{
						::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
					}
					auto* chaos_delegate = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Func_System_Int32_*>(chaos_delegate_value);
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
							auto* chaos_invocation_delegate = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Func_System_Int32_*>(chaos_invocation_delegate_value);
							if (chaos_invocation_delegate->chaos_delegate_method_ptr == 0)
							{
								CHAOS_IL2CPP_FAIL();
							}
							if (chaos_invocation_delegate->chaos_delegate_target == 0)
							{
								const auto chaos_open_function = reinterpret_cast<CHAOS_IL2CPP_INT32(*)()>(chaos_invocation_delegate->chaos_delegate_method_ptr);
								chaos_result = chaos_open_function();
							}
							else
							{
								const auto chaos_closed_function = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR chaos_delegate_target)>(chaos_invocation_delegate->chaos_delegate_method_ptr);
								chaos_result = chaos_closed_function(chaos_invocation_delegate->chaos_delegate_target);
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
						uint64_t __chaos_args_buf[1] = {0};
						uint64_t __chaos_ret_buf[2] = {};
						if (chaos_delegate->chaos_delegate_method_token != 0)
						{
							__chaos_hotpatch_taken = ::chaos::il2cpp::runtime_core::DelegateHotpatchCheckpoint(
								chaos_delegate->chaos_delegate_method_token,
								__chaos_args_buf, __chaos_ret_buf, 0);
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
							CHAOS_IL2CPP_INT32 chaos_result{};
							if (chaos_delegate->chaos_delegate_target == 0)
							{
								const auto chaos_open_function = reinterpret_cast<CHAOS_IL2CPP_INT32(*)()>(chaos_delegate->chaos_delegate_method_ptr);
								chaos_result = chaos_open_function();
							}
							else
							{
								const auto chaos_closed_function = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR chaos_delegate_target)>(chaos_delegate->chaos_delegate_method_ptr);
								chaos_result = chaos_closed_function(chaos_delegate->chaos_delegate_target);
							}
							_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
						}
					}
				}
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
				chaos_static_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ArrayIndexingCopySubjects/ArrayIndexingCopySubjects+<>c::<Subject_3>b__4_0()
extern "C" CHAOS_IL2CPP_INT32 ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c_Subject_3_b__4_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
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
	chaos_locals[0] = _s0;
	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(8);
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[9])(chaos_arg_0, chaos_arg_1);
	}
	_s0 = chaos_locals[0];
	_s0 = [&](){ auto* _c_arr = reinterpret_cast<chaos_managed_array*>(_s0); return _c_arr ? static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_c_arr->length)) : CHAOS_IL2CPP_INTPTR{}; }();
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ArrayIndexingCopySubjects/ArrayIndexingCopySubjects::Subject_4()
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_4(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	CHAOS_EH_TRY
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[10])();
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				chaos_external_runtime_System_Private_CoreLib_System_Array__Sort_System_Byte__System_Void_System_Byte___(chaos_arg_0);
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
				chaos_static_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ArrayIndexingCopySubjects/ArrayIndexingCopySubjects::Subject_5()
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_5(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};


	CHAOS_EH_TRY
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
				chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Byte.hot;
				chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
				chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
			}
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[12])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				chaos_external_runtime_System_Private_CoreLib_System_Array__Sort_System_Byte__System_Void_System_Byte___System_Collections_Generic_IComparer_System_Byte__(chaos_arg_0, chaos_arg_1);
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
				chaos_static_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ArrayIndexingCopySubjects/ArrayIndexingCopySubjects::Subject_6()
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_6(void)
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
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
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
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Array__BinarySearch_System_Byte__System_Int32_System_Byte___System_Byte_(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
			{
				const auto chaos_length = static_cast<CHAOS_IL2CPP_INT32>(_s1);
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
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
			}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Array__BinarySearch_System_Byte__System_Int32_System_Byte___System_Byte_(chaos_arg_0, chaos_arg_1);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
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
				chaos_static_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___exitCode = chaos_value;
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
				chaos_static_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ArrayIndexingCopySubjects/ArrayIndexingCopySubjects::Subject_7()
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_7(void)
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
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};
	CHAOS_IL2CPP_INTPTR _s17{};
	CHAOS_IL2CPP_INTPTR _s18{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
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
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_3 = _s3;
				const auto chaos_arg_2 = _s2;
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Array__BinarySearch_System_Byte__System_Int32_System_Byte___System_Int32_System_Int32_System_Byte_(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2), chaos_arg_3);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
			{
				const auto chaos_length = static_cast<CHAOS_IL2CPP_INT32>(_s1);
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
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
			}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_3 = _s4;
				const auto chaos_arg_2 = _s3;
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Array__BinarySearch_System_Byte__System_Int32_System_Byte___System_Int32_System_Int32_System_Byte_(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2), chaos_arg_3);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
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
				chaos_static_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___exitCode = chaos_value;
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
				chaos_static_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ArrayIndexingCopySubjects/ArrayIndexingCopySubjects::Subject_8()
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_8(void)
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
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
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
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Array__IndexOf_System_Byte__System_Int32_System_Byte___System_Byte_(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
			{
				const auto chaos_length = static_cast<CHAOS_IL2CPP_INT32>(_s1);
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
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
			}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Array__IndexOf_System_Byte__System_Int32_System_Byte___System_Byte_(chaos_arg_0, chaos_arg_1);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
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
				chaos_static_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___exitCode = chaos_value;
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
				chaos_static_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ArrayIndexingCopySubjects/ArrayIndexingCopySubjects::Subject_9()
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_9(void)
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
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
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
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Array__LastIndexOf_System_Byte__System_Int32_System_Byte___System_Byte_(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
			{
				const auto chaos_length = static_cast<CHAOS_IL2CPP_INT32>(_s1);
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
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
			}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Array__LastIndexOf_System_Byte__System_Int32_System_Byte___System_Byte_(chaos_arg_0, chaos_arg_1);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
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
				chaos_static_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___exitCode = chaos_value;
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
				chaos_static_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ArrayIndexingCopySubjects/ArrayIndexingCopySubjects::Subject_10()
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_10(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	CHAOS_EH_TRY
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[10])();
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				chaos_external_runtime_System_Private_CoreLib_System_Array__Reverse_System_Byte__System_Void_System_Byte___(chaos_arg_0);
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
				chaos_static_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ArrayIndexingCopySubjects/ArrayIndexingCopySubjects::Subject_11()
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_11(void)
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
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
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
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosArrayGetLength(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
			{
				const auto chaos_length = static_cast<CHAOS_IL2CPP_INT32>(_s1);
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
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
			}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosArrayGetLength(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
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
				chaos_static_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___exitCode = chaos_value;
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
				chaos_static_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ArrayIndexingCopySubjects/ArrayIndexingCopySubjects::Subject_12()
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects_Subject_12(void)
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
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};


	CHAOS_EH_TRY
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
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Array__GetValue_System_Object_System_Int32_(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				const auto chaos_length = static_cast<CHAOS_IL2CPP_INT32>(_s1);
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
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
			}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Array__GetValue_System_Object_System_Int32_(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
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
				chaos_static_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___exitCode = chaos_value;
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
				chaos_static_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ArrayIndexingCopySubjects/ArrayIndexingCopySubjects+<>c::.cctor()
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c_cctor(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};

	chaos_ensure_type_initialized_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c();

	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c, {});
		chaos_object->header.type_info = &chaos_mt_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c.hot;
		ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	{
		auto chaos_value = _s0;
	chaos_ensure_type_initialized_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c();
		chaos_static_ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c____9 = chaos_value;
	}
	return;
}

// Managed method: ArrayIndexingCopySubjects/ArrayIndexingCopySubjects+<>c::.ctor()
extern "C" void ArrayIndexingCopySubjects_ArrayIndexingCopySubjects___c__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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



}  // namespace chaos::il2cpp::codegen::ArrayIndexingCopySubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 16;

extern "C" void ChaosJitRegisterAll() {}