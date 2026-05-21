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
#include "enum_metadata.generated.h"

// Forward declaration for dispatch table entries (defined in runtime_stubs.cpp)
extern "C" void InterpreterEntryDirect(
    CHAOS_IL2CPP_UINTPTR method_key,
    void*     args_buf,
    void*     ret_buf) noexcept;

#pragma warning(push)
#pragma warning(disable: 4065 4244)

namespace chaos::il2cpp::codegen::ShuffleIndexSubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


const void* chaos_vtable_ShuffleIndexSubjects_ShuffleIndexSubjects___c[];
inline TypeInfoV0 chaos_mt_ShuffleIndexSubjects__PrivateImplementationDetails_ = {{nullptr, nullptr, 6416785671501470329ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_ShuffleIndexSubjects__PrivateImplementationDetails_ = static_cast<CHAOS_IL2CPP_INTPTR>(6416785671501470329ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Object = {{nullptr, nullptr, 15228727185366376748ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_INTPTR>(15228727185366376748ULL);
inline TypeInfoV0 chaos_mt_ShuffleIndexSubjects_ShuffleIndexSubjects___c = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_ShuffleIndexSubjects_ShuffleIndexSubjects___c, 8198999182645464478ULL, 12u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_ShuffleIndexSubjects_ShuffleIndexSubjects___c = static_cast<CHAOS_IL2CPP_INTPTR>(8198999182645464478ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Delegate = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, nullptr, 7451128447593600616ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Delegate = static_cast<CHAOS_IL2CPP_INTPTR>(7451128447593600616ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_MulticastDelegate = {{&chaos_mt_System_Private_CoreLib_System_Delegate.hot, nullptr, 6681393039041505440ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_MulticastDelegate = static_cast<CHAOS_IL2CPP_INTPTR>(6681393039041505440ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Boolean_ = {{&chaos_mt_System_Private_CoreLib_System_MulticastDelegate.hot, nullptr, 6749479132849532215ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Func_System_Int32_System_Boolean_ = static_cast<CHAOS_IL2CPP_INTPTR>(6749479132849532215ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Int32_System_ValueTuple_System_Int32_System_Int32__ = {{&chaos_mt_System_Private_CoreLib_System_MulticastDelegate.hot, nullptr, 17803061799386445533ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Func_System_Int32_System_Int32_System_ValueTuple_System_Int32_System_Int32__ = static_cast<CHAOS_IL2CPP_INTPTR>(17803061799386445533ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Int32_ = {{&chaos_mt_System_Private_CoreLib_System_MulticastDelegate.hot, nullptr, 143744549824129343ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Func_System_Int32_System_Int32_ = static_cast<CHAOS_IL2CPP_INTPTR>(143744549824129343ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Index = {{nullptr, nullptr, 2536260981658261941ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Index = static_cast<CHAOS_IL2CPP_INTPTR>(2536260981658261941ULL);
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
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_ValueTuple_System_Int32_System_Int32_ = {{nullptr, nullptr, 6159347430806283030ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_ValueTuple_System_Int32_System_Int32_ = static_cast<CHAOS_IL2CPP_INTPTR>(6159347430806283030ULL);

// ── Virtual method table arrays ──
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects___c__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INTPTR ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_10_b__11_0_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INTPTR ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_12_b__13_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_15_b__16_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_16_b__17_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_17_b__18_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_19_b__20_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_22_b__23_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_23_b__24_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_5_b__6_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_8_b__9_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_9_b__10_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
const void* chaos_vtable_ShuffleIndexSubjects_ShuffleIndexSubjects___c[] =
{
	reinterpret_cast<void*>(ShuffleIndexSubjects_ShuffleIndexSubjects___c__ctor),
	reinterpret_cast<void*>(ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_10_b__11_0_System_Int32_System_Int32),
	reinterpret_cast<void*>(ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_12_b__13_0_System_Int32),
	reinterpret_cast<void*>(ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_15_b__16_0_System_Int32),
	reinterpret_cast<void*>(ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_16_b__17_0_System_Int32),
	reinterpret_cast<void*>(ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_17_b__18_0_System_Int32),
	reinterpret_cast<void*>(ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_19_b__20_0_System_Int32),
	reinterpret_cast<void*>(ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_22_b__23_0_System_Int32),
	reinterpret_cast<void*>(ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_23_b__24_0_System_Int32),
	reinterpret_cast<void*>(ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_5_b__6_0_System_Int32),
	reinterpret_cast<void*>(ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_8_b__9_0_System_Int32),
	reinterpret_cast<void*>(ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_9_b__10_0_System_Int32),
};
static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_ShuffleIndexSubjects_ShuffleIndexSubjects___c[] =
{
	{ 0x0000002Fu, reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c__ctor) },
	{ 0x00000033u, reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_10_b__11_0_System_Int32_System_Int32) },
	{ 0x00000034u, reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_12_b__13_0_System_Int32) },
	{ 0x00000035u, reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_15_b__16_0_System_Int32) },
	{ 0x00000036u, reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_16_b__17_0_System_Int32) },
	{ 0x00000037u, reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_17_b__18_0_System_Int32) },
	{ 0x00000038u, reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_19_b__20_0_System_Int32) },
	{ 0x00000039u, reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_22_b__23_0_System_Int32) },
	{ 0x0000003Au, reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_23_b__24_0_System_Int32) },
	{ 0x00000030u, reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_5_b__6_0_System_Int32) },
	{ 0x00000031u, reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_8_b__9_0_System_Int32) },
	{ 0x00000032u, reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_9_b__10_0_System_Int32) },
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

struct chaos_type_ShuffleIndexSubjects__PrivateImplementationDetails_
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR field_ShuffleIndexSubjects__PrivateImplementationDetails___4F6ADDC9659D6FB90FE94B6688A79F2A1FA8D36EC43F8F3E1D9B6528C448A384 = 0;
	CHAOS_IL2CPP_INTPTR field_ShuffleIndexSubjects__PrivateImplementationDetails___6E28B4D41AF8A88650219653A728E0FD6760BB2B1DFEB7A9EB8DEB5B4E7010E0 = 0;
	CHAOS_IL2CPP_INTPTR field_ShuffleIndexSubjects__PrivateImplementationDetails___97CA1592048640A5368B4EC7C6934311567E09D50E7639918EC82C3D2A187CDA = 0;
};

struct chaos_type_System_Private_CoreLib_System_Object
{
	ThinLockableHeader header{};
};

struct chaos_type_ShuffleIndexSubjects_ShuffleIndexSubjects___c : public chaos_type_System_Private_CoreLib_System_Object
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

struct chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Boolean_ : public chaos_type_System_Private_CoreLib_System_MulticastDelegate
{
};

struct chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Int32_System_ValueTuple_System_Int32_System_Int32__ : public chaos_type_System_Private_CoreLib_System_MulticastDelegate
{
};

struct chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Int32_ : public chaos_type_System_Private_CoreLib_System_MulticastDelegate
{
};

struct chaos_type_System_Private_CoreLib_System_Index
{
	ThinLockableHeader header{};
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

struct chaos_type_System_Private_CoreLib_System_ValueTuple_System_Int32_System_Int32_
{
	ThinLockableHeader header{};
};

CHAOS_IL2CPP_INTPTR chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9__10_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9__11_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9__13_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9__16_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9__17_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9__18_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9__20_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9__23_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9__24_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9__6_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9__9_0 = 0;

const CHAOS_IL2CPP_UINT8 chaos_static_field_data_ShuffleIndexSubjects__PrivateImplementationDetails___4F6ADDC9659D6FB90FE94B6688A79F2A1FA8D36EC43F8F3E1D9B6528C448A384[] =
{
	0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
	0x05, 0x00, 0x00, 0x00,
};

const CHAOS_IL2CPP_UINT8 chaos_static_field_data_ShuffleIndexSubjects__PrivateImplementationDetails___6E28B4D41AF8A88650219653A728E0FD6760BB2B1DFEB7A9EB8DEB5B4E7010E0[] =
{
	0x0A, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00,
	0x32, 0x00, 0x00, 0x00,
};

const CHAOS_IL2CPP_UINT8 chaos_static_field_data_ShuffleIndexSubjects__PrivateImplementationDetails___97CA1592048640A5368B4EC7C6934311567E09D50E7639918EC82C3D2A187CDA[] =
{
	0x0A, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00,
};

bool chaos_try_get_static_field_data(
	CHAOS_IL2CPP_INTPTR chaos_field_handle,
	const CHAOS_IL2CPP_UINT8*& chaos_data,
	CHAOS_IL2CPP_SIZE& chaos_size)
{
	if (chaos_field_handle == static_cast<CHAOS_IL2CPP_INTPTR>(68691011u))
	{
		chaos_data = chaos_static_field_data_ShuffleIndexSubjects__PrivateImplementationDetails___4F6ADDC9659D6FB90FE94B6688A79F2A1FA8D36EC43F8F3E1D9B6528C448A384;
		chaos_size = sizeof(chaos_static_field_data_ShuffleIndexSubjects__PrivateImplementationDetails___4F6ADDC9659D6FB90FE94B6688A79F2A1FA8D36EC43F8F3E1D9B6528C448A384);
		return true;
	}
	if (chaos_field_handle == static_cast<CHAOS_IL2CPP_INTPTR>(70775822u))
	{
		chaos_data = chaos_static_field_data_ShuffleIndexSubjects__PrivateImplementationDetails___6E28B4D41AF8A88650219653A728E0FD6760BB2B1DFEB7A9EB8DEB5B4E7010E0;
		chaos_size = sizeof(chaos_static_field_data_ShuffleIndexSubjects__PrivateImplementationDetails___6E28B4D41AF8A88650219653A728E0FD6760BB2B1DFEB7A9EB8DEB5B4E7010E0);
		return true;
	}
	if (chaos_field_handle == static_cast<CHAOS_IL2CPP_INTPTR>(81777074u))
	{
		chaos_data = chaos_static_field_data_ShuffleIndexSubjects__PrivateImplementationDetails___97CA1592048640A5368B4EC7C6934311567E09D50E7639918EC82C3D2A187CDA;
		chaos_size = sizeof(chaos_static_field_data_ShuffleIndexSubjects__PrivateImplementationDetails___97CA1592048640A5368B4EC7C6934311567E09D50E7639918EC82C3D2A187CDA);
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

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Random__Next_System_Int32__()
{
	return ChaosRandomNext(0);
}

static void chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers__InitializeArray_System_Void_System_Array_System_RuntimeFieldHandle_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	chaos_initialize_array_from_field_data_int32(chaos_fn_arg_0, chaos_fn_arg_1);
}

CHAOS_IL2CPP_ONCE_FLAG chaos_type_init_once_ShuffleIndexSubjects_ShuffleIndexSubjects___c;

void chaos_ensure_type_initialized_ShuffleIndexSubjects_ShuffleIndexSubjects___c()
{
	CHAOS_IL2CPP_CALL_ONCE(chaos_type_init_once_ShuffleIndexSubjects_ShuffleIndexSubjects___c, []()
	{
	if (chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9 == 0)
	{
	    auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_ShuffleIndexSubjects_ShuffleIndexSubjects___c);
	    chaos_object->header.type_info = &chaos_mt_ShuffleIndexSubjects_ShuffleIndexSubjects___c.hot;
		const auto chaos_arg_0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
		ShuffleIndexSubjects_ShuffleIndexSubjects___c__ctor(chaos_arg_0);
	    chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	});
}

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };

extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_0(void);
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_1(void);
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_2(void);
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_3(void);
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_4(void);
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_5(void);
extern "C" CHAOS_IL2CPP_INTPTR ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_5_b__6_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_6(void);
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_7(void);
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_8(void);
extern "C" CHAOS_IL2CPP_INTPTR ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_8_b__9_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_9(void);
extern "C" CHAOS_IL2CPP_INTPTR ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_9_b__10_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_10(void);
extern "C" CHAOS_IL2CPP_INTPTR ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_10_b__11_0_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_11(void);
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_12(void);
extern "C" CHAOS_IL2CPP_INTPTR ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_12_b__13_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_13(void);
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_14(void);
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_15(void);
extern "C" CHAOS_IL2CPP_INTPTR ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_15_b__16_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_16(void);
extern "C" CHAOS_IL2CPP_INTPTR ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_16_b__17_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_17(void);
extern "C" CHAOS_IL2CPP_INT32 ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_17_b__18_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_18(void);
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_19(void);
extern "C" CHAOS_IL2CPP_INTPTR ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_19_b__20_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_20(void);
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_21(void);
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_22(void);
extern "C" CHAOS_IL2CPP_INTPTR ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_22_b__23_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_23(void);
extern "C" CHAOS_IL2CPP_INTPTR ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_23_b__24_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects___c_cctor(void);
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects___c__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[38] = {
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
	2u,
	2u,
	3u,
	3u,
	5u,
	5u,
	5u,
	6u,
	6u,
	6u,
	6u,
	7u,
	7u,
	8u,
	8u,
	9u,
	9u,
	9u,
	10u,
	10u,
	10u,
	10u,
	11u,
	11u,
	12u,
	12u,
	12u,
};

static constexpr struct {
	::ChaosAbiManifestV0 header;
	::ChaosAbiMethodEntryV0 entries[37];
	CHAOS_IL2CPP_UINT8 params[12];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		37u,
		12u,
		734149154u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_0
		{ 0u, 0u },  // ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_1
		{ 0u, 0u },  // ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_2
		{ 0u, 0u },  // ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_3
		{ 0u, 0u },  // ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_4
		{ 0u, 0u },  // ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_5
		{ 2u, 1u },  // ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_5_b__6_0_System_Int32
		{ 0u, 0u },  // ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_6
		{ 0u, 0u },  // ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_7
		{ 0u, 0u },  // ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_8
		{ 2u, 1u },  // ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_8_b__9_0_System_Int32
		{ 0u, 0u },  // ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_9
		{ 2u, 1u },  // ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_9_b__10_0_System_Int32
		{ 0u, 0u },  // ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_10
		{ 2u, 2u },  // ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_10_b__11_0_System_Int32_System_Int32
		{ 0u, 0u },  // ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_11
		{ 0u, 0u },  // ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_12
		{ 2u, 1u },  // ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_12_b__13_0_System_Int32
		{ 0u, 0u },  // ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_13
		{ 0u, 0u },  // ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_14
		{ 0u, 0u },  // ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_15
		{ 2u, 1u },  // ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_15_b__16_0_System_Int32
		{ 0u, 0u },  // ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_16
		{ 2u, 1u },  // ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_16_b__17_0_System_Int32
		{ 0u, 0u },  // ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_17
		{ 1u, 1u },  // ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_17_b__18_0_System_Int32
		{ 0u, 0u },  // ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_18
		{ 0u, 0u },  // ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_19
		{ 2u, 1u },  // ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_19_b__20_0_System_Int32
		{ 0u, 0u },  // ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_20
		{ 0u, 0u },  // ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_21
		{ 0u, 0u },  // ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_22
		{ 2u, 1u },  // ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_22_b__23_0_System_Int32
		{ 0u, 0u },  // ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_23
		{ 2u, 1u },  // ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_23_b__24_0_System_Int32
		{ 0u, 0u },  // ShuffleIndexSubjects_ShuffleIndexSubjects___c_cctor
		{ 0u, 0u },  // ShuffleIndexSubjects_ShuffleIndexSubjects___c__ctor
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
		1u,
		1u,
		1u,
	},
};
static const ::ChaosAbiManifestV0* const s_abi_manifest =
	reinterpret_cast<const ::ChaosAbiManifestV0*>(&s_abi_manifest_storage);
// ── Module registration ──

	static constexpr CHAOS_IL2CPP_UINT32 s_type_flags[6] = {
		0u,
		1548u,
		8u,
		1544u,
		1545u,
		1545u,
	};

	static constexpr const char* s_type_names[6] = {
		"<Module>",
		"ShuffleIndexSubjects",
		"<PrivateImplementationDetails>",
		"<>c",
		"__StaticArrayInitTypeSize=12",
		"__StaticArrayInitTypeSize=20",
	};

	static constexpr const char* s_type_namespaces[6] = {
		"",
		"",
		"",
		"",
		"",
		"",
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_type_parent_tokens[6] = {
		0u,
		0u,
		0u,
		0u,
		0u,
		0u,
	};

	static const TypeInfoHot* const s_type_info_ptrs[6] = {
		nullptr,
		nullptr,
		&chaos_mt_ShuffleIndexSubjects__PrivateImplementationDetails_.hot,
		&chaos_mt_ShuffleIndexSubjects_ShuffleIndexSubjects___c.hot,
		nullptr,
		nullptr,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_children[3] = {
		33554436u,
		33554437u,
		33554438u,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_offset[7] = {
		0u,
		0u,
		1u,
		3u,
		3u,
		3u,
		3u,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_generic_param_constraint_offset[7] = {
		0u,
		0u,
		0u,
		0u,
		0u,
		0u,
		0u,
	};



	static const ::chaos::il2cpp::runtime_core::ModuleDescriptor s_native_aot_module = {
		/* .name_utf8         = */ "ShuffleIndexSubjects",
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
		/* .type_count        = */ 6u,
	/* .custom_attribute_blob       = */ nullptr,  // Tier 1 — deferred
	/* .custom_attribute_offset     = */ nullptr,
	/* .custom_attribute_entity_count = */ 0u,
	/* .custom_attribute_materializer = */ nullptr,

		/* .abi_manifest      = */ s_abi_manifest,
	};
	static const CHAOS_IL2CPP_UINT32 s_native_aot_module_id =
		::chaos::il2cpp::runtime_core::RegisterModule("ShuffleIndexSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[37] = {
	{ "CustomEntrySubject_0", 0x00000016u, 0u },  // ShuffleIndexSubjects
	{ "CustomEntrySubject_1", 0x00000017u, 0u },  // ShuffleIndexSubjects
	{ "CustomEntrySubject_2", 0x00000018u, 0u },  // ShuffleIndexSubjects
	{ "CustomEntrySubject_3", 0x00000019u, 0u },  // ShuffleIndexSubjects
	{ "CustomEntrySubject_4", 0x0000001Au, 0u },  // ShuffleIndexSubjects
	{ "CustomEntrySubject_5", 0x0000001Bu, 0u },  // ShuffleIndexSubjects
	{ "CustomEntrySubject_6", 0x0000001Cu, 0u },  // ShuffleIndexSubjects
	{ "CustomEntrySubject_7", 0x0000001Du, 0u },  // ShuffleIndexSubjects
	{ "CustomEntrySubject_8", 0x0000001Eu, 0u },  // ShuffleIndexSubjects
	{ "CustomEntrySubject_9", 0x0000001Fu, 0u },  // ShuffleIndexSubjects
	{ "CustomEntrySubject_10", 0x00000020u, 0u },  // ShuffleIndexSubjects
	{ "CustomEntrySubject_11", 0x00000021u, 0u },  // ShuffleIndexSubjects
	{ "CustomEntrySubject_12", 0x00000022u, 0u },  // ShuffleIndexSubjects
	{ "CustomEntrySubject_13", 0x00000023u, 0u },  // ShuffleIndexSubjects
	{ "CustomEntrySubject_14", 0x00000024u, 0u },  // ShuffleIndexSubjects
	{ "CustomEntrySubject_15", 0x00000025u, 0u },  // ShuffleIndexSubjects
	{ "CustomEntrySubject_16", 0x00000026u, 0u },  // ShuffleIndexSubjects
	{ "CustomEntrySubject_17", 0x00000027u, 0u },  // ShuffleIndexSubjects
	{ "CustomEntrySubject_18", 0x00000028u, 0u },  // ShuffleIndexSubjects
	{ "CustomEntrySubject_19", 0x00000029u, 0u },  // ShuffleIndexSubjects
	{ "CustomEntrySubject_20", 0x0000002Au, 0u },  // ShuffleIndexSubjects
	{ "CustomEntrySubject_21", 0x0000002Bu, 0u },  // ShuffleIndexSubjects
	{ "CustomEntrySubject_22", 0x0000002Cu, 0u },  // ShuffleIndexSubjects
	{ "CustomEntrySubject_23", 0x0000002Du, 0u },  // ShuffleIndexSubjects
	{ "<CustomEntrySubject_5>b__6_0", 0x00000030u, 1u },  // ShuffleIndexSubjects+<>c
	{ "<CustomEntrySubject_8>b__9_0", 0x00000031u, 1u },  // ShuffleIndexSubjects+<>c
	{ "<CustomEntrySubject_9>b__10_0", 0x00000032u, 1u },  // ShuffleIndexSubjects+<>c
	{ "<CustomEntrySubject_10>b__11_0", 0x00000033u, 2u },  // ShuffleIndexSubjects+<>c
	{ "<CustomEntrySubject_12>b__13_0", 0x00000034u, 1u },  // ShuffleIndexSubjects+<>c
	{ "<CustomEntrySubject_15>b__16_0", 0x00000035u, 1u },  // ShuffleIndexSubjects+<>c
	{ "<CustomEntrySubject_16>b__17_0", 0x00000036u, 1u },  // ShuffleIndexSubjects+<>c
	{ "<CustomEntrySubject_17>b__18_0", 0x00000037u, 1u },  // ShuffleIndexSubjects+<>c
	{ "<CustomEntrySubject_19>b__20_0", 0x00000038u, 1u },  // ShuffleIndexSubjects+<>c
	{ "<CustomEntrySubject_22>b__23_0", 0x00000039u, 1u },  // ShuffleIndexSubjects+<>c
	{ "<CustomEntrySubject_23>b__24_0", 0x0000003Au, 1u },  // ShuffleIndexSubjects+<>c
	{ ".cctor", 0x0000002Eu, 0u },  // ShuffleIndexSubjects+<>c
	{ ".ctor", 0x0000002Fu, 0u },  // ShuffleIndexSubjects+<>c
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[2] = {
	{ "ShuffleIndexSubjects", "", 0u, 24u },
	{ "ShuffleIndexSubjects+<>c", "", 24u, 13u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[37] = {
	{ 0x00000016u, 0u },
	{ 0x00000017u, 1u },
	{ 0x00000018u, 2u },
	{ 0x00000019u, 3u },
	{ 0x0000001Au, 4u },
	{ 0x0000001Bu, 5u },
	{ 0x0000001Cu, 7u },
	{ 0x0000001Du, 8u },
	{ 0x0000001Eu, 9u },
	{ 0x0000001Fu, 11u },
	{ 0x00000020u, 13u },
	{ 0x00000021u, 15u },
	{ 0x00000022u, 16u },
	{ 0x00000023u, 18u },
	{ 0x00000024u, 19u },
	{ 0x00000025u, 20u },
	{ 0x00000026u, 22u },
	{ 0x00000027u, 24u },
	{ 0x00000028u, 26u },
	{ 0x00000029u, 27u },
	{ 0x0000002Au, 29u },
	{ 0x0000002Bu, 30u },
	{ 0x0000002Cu, 31u },
	{ 0x0000002Du, 33u },
	{ 0x0000002Eu, 35u },
	{ 0x0000002Fu, 36u },
	{ 0x00000030u, 6u },
	{ 0x00000031u, 10u },
	{ 0x00000032u, 12u },
	{ 0x00000033u, 14u },
	{ 0x00000034u, 17u },
	{ 0x00000035u, 21u },
	{ 0x00000036u, 23u },
	{ 0x00000037u, 25u },
	{ 0x00000038u, 28u },
	{ 0x00000039u, 32u },
	{ 0x0000003Au, 34u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[37] = {
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ShuffleIndexSubjects::CustomEntrySubject_0
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ShuffleIndexSubjects::CustomEntrySubject_1
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ShuffleIndexSubjects::CustomEntrySubject_2
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ShuffleIndexSubjects::CustomEntrySubject_3
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ShuffleIndexSubjects::CustomEntrySubject_4
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ShuffleIndexSubjects::CustomEntrySubject_5
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_5_b__6_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ShuffleIndexSubjects+<>c::<CustomEntrySubject_5>b__6_0
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ShuffleIndexSubjects::CustomEntrySubject_6
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ShuffleIndexSubjects::CustomEntrySubject_7
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ShuffleIndexSubjects::CustomEntrySubject_8
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_8_b__9_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ShuffleIndexSubjects+<>c::<CustomEntrySubject_8>b__9_0
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_9), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ShuffleIndexSubjects::CustomEntrySubject_9
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_9_b__10_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ShuffleIndexSubjects+<>c::<CustomEntrySubject_9>b__10_0
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_10), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ShuffleIndexSubjects::CustomEntrySubject_10
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_10_b__11_0_System_Int32_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ShuffleIndexSubjects+<>c::<CustomEntrySubject_10>b__11_0
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_11), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ShuffleIndexSubjects::CustomEntrySubject_11
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_12), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ShuffleIndexSubjects::CustomEntrySubject_12
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_12_b__13_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ShuffleIndexSubjects+<>c::<CustomEntrySubject_12>b__13_0
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_13), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ShuffleIndexSubjects::CustomEntrySubject_13
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_14), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ShuffleIndexSubjects::CustomEntrySubject_14
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_15), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ShuffleIndexSubjects::CustomEntrySubject_15
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_15_b__16_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ShuffleIndexSubjects+<>c::<CustomEntrySubject_15>b__16_0
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_16), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ShuffleIndexSubjects::CustomEntrySubject_16
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_16_b__17_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ShuffleIndexSubjects+<>c::<CustomEntrySubject_16>b__17_0
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_17), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ShuffleIndexSubjects::CustomEntrySubject_17
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_17_b__18_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ShuffleIndexSubjects+<>c::<CustomEntrySubject_17>b__18_0
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_18), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ShuffleIndexSubjects::CustomEntrySubject_18
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_19), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ShuffleIndexSubjects::CustomEntrySubject_19
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_19_b__20_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ShuffleIndexSubjects+<>c::<CustomEntrySubject_19>b__20_0
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_20), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ShuffleIndexSubjects::CustomEntrySubject_20
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_21), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ShuffleIndexSubjects::CustomEntrySubject_21
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_22), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ShuffleIndexSubjects::CustomEntrySubject_22
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_22_b__23_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ShuffleIndexSubjects+<>c::<CustomEntrySubject_22>b__23_0
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_23), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ShuffleIndexSubjects::CustomEntrySubject_23
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_23_b__24_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ShuffleIndexSubjects+<>c::<CustomEntrySubject_23>b__24_0
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_cctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ShuffleIndexSubjects+<>c::.cctor
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ShuffleIndexSubjects+<>c::.ctor
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"ShuffleIndexSubjects",
	s_hotpatch_types,
	2u,
	s_hotpatch_methods,
	37u,
	s_hotpatch_slots,
	37u,
	s_hotpatch_entries,
	37u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[52] = {
	"System.Private.CoreLib/System.Int32",
	"ShuffleIndexSubjects/<PrivateImplementationDetails>::6E28B4D41AF8A88650219653A728E0FD6760BB2B1DFEB7A9EB8DEB5B4E7010E0",
	"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)",
	"System.Linq/Enumerable::ElementAt<System.Int32>:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)",
	"System.Private.CoreLib/System.Index::.ctor:System.Void(System.Int32,System.Boolean)",
	"System.Linq/Enumerable::ElementAt<System.Int32>:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Index)",
	"System.Linq/Enumerable::ElementAtOrDefault<System.Int32>:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)",
	"System.Linq/Enumerable::ElementAtOrDefault<System.Int32>:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Index)",
	"ShuffleIndexSubjects/<PrivateImplementationDetails>::97CA1592048640A5368B4EC7C6934311567E09D50E7639918EC82C3D2A187CDA",
	"System.Linq/Enumerable::First<System.Int32>:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>)",
	"ShuffleIndexSubjects/<PrivateImplementationDetails>::4F6ADDC9659D6FB90FE94B6688A79F2A1FA8D36EC43F8F3E1D9B6528C448A384",
	"ShuffleIndexSubjects/ShuffleIndexSubjects+<>c::<>9__6_0",
	"ShuffleIndexSubjects/ShuffleIndexSubjects+<>c::<>9",
	"System.Private.CoreLib/System.Func<System.Int32,System.Boolean>::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Linq/Enumerable::First<System.Int32>:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)",
	"System.Linq/Enumerable::FirstOrDefault<System.Int32>:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>)",
	"System.Linq/Enumerable::FirstOrDefault<System.Int32>:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)",
	"ShuffleIndexSubjects/ShuffleIndexSubjects+<>c::<>9__9_0",
	"System.Linq/Enumerable::FirstOrDefault<System.Int32>:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)",
	"ShuffleIndexSubjects/ShuffleIndexSubjects+<>c::<>9__10_0",
	"System.Linq/Enumerable::FirstOrDefault<System.Int32>:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>,System.Int32)",
	"ShuffleIndexSubjects/ShuffleIndexSubjects+<>c::<>9__11_0",
	"System.Private.CoreLib/System.Func<System.Int32,System.Int32,System.ValueTuple<System.Int32,System.Int32>>::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Linq/Enumerable::Select<System.Int32,System.ValueTuple<System.Int32,System.Int32>>:System.Collections.Generic.IEnumerable<System.ValueTuple<System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32,System.ValueTuple<System.Int32,System.Int32>>)",
	"System.Private.CoreLib/System.Collections.Generic.IEnumerable<System.ValueTuple<System.Int32,System.Int32>>::GetEnumerator:System.Collections.Generic.IEnumerator<System.ValueTuple<System.Int32,System.Int32>>()",
	"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()",
	"System.Private.CoreLib/System.ValueTuple<System.Int32,System.Int32>::.ctor:System.Void(System.Int32,System.Int32)",
	"System.Linq/Enumerable::Last<System.Int32>:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>)",
	"ShuffleIndexSubjects/ShuffleIndexSubjects+<>c::<>9__13_0",
	"System.Linq/Enumerable::Last<System.Int32>:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)",
	"System.Linq/Enumerable::LastOrDefault<System.Int32>:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>)",
	"System.Linq/Enumerable::LastOrDefault<System.Int32>:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)",
	"ShuffleIndexSubjects/ShuffleIndexSubjects+<>c::<>9__16_0",
	"System.Linq/Enumerable::LastOrDefault<System.Int32>:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)",
	"ShuffleIndexSubjects/ShuffleIndexSubjects+<>c::<>9__17_0",
	"System.Linq/Enumerable::LastOrDefault<System.Int32>:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>,System.Int32)",
	"ShuffleIndexSubjects/ShuffleIndexSubjects+<>c::<>9__18_0",
	"System.Private.CoreLib/System.Func<System.Int32,System.Int32>::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Linq/Enumerable::OrderBy<System.Int32,System.Int32>:IOrderedEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)",
	"System.Private.CoreLib/System.Collections.Generic.IEnumerable<System.Int32>::GetEnumerator:System.Collections.Generic.IEnumerator<System.Int32>()",
	"System.Private.CoreLib/System.Random::get_Shared:System.Random()",
	"System.Private.CoreLib/System.Random::Next:System.Int32()",
	"System.Linq/Enumerable::Single<System.Int32>:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>)",
	"ShuffleIndexSubjects/ShuffleIndexSubjects+<>c::<>9__20_0",
	"System.Linq/Enumerable::Single<System.Int32>:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)",
	"System.Linq/Enumerable::SingleOrDefault<System.Int32>:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>)",
	"System.Linq/Enumerable::SingleOrDefault<System.Int32>:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)",
	"ShuffleIndexSubjects/ShuffleIndexSubjects+<>c::<>9__23_0",
	"System.Linq/Enumerable::SingleOrDefault<System.Int32>:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)",
	"ShuffleIndexSubjects/ShuffleIndexSubjects+<>c::<>9__24_0",
	"System.Linq/Enumerable::SingleOrDefault<System.Int32>:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>,System.Int32)",
	"System.Private.CoreLib/System.Object::.ctor:System.Void()",
};

extern "C" void* kChaosExternalRuntimeFnTable[52] = {
	nullptr,
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers__InitializeArray_System_Void_System_Array_System_RuntimeFieldHandle_),
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
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Random__Next_System_Int32__),
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__),
};

extern "C" int32_t kChaosExternalRuntimeCount = 52;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[37])() = {
	reinterpret_cast<void(*)()>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_0),
	reinterpret_cast<void(*)()>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_1),
	reinterpret_cast<void(*)()>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_2),
	reinterpret_cast<void(*)()>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_3),
	reinterpret_cast<void(*)()>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_4),
	reinterpret_cast<void(*)()>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_5),
	reinterpret_cast<void(*)()>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_5_b__6_0_System_Int32),
	reinterpret_cast<void(*)()>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_6),
	reinterpret_cast<void(*)()>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_7),
	reinterpret_cast<void(*)()>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_8),
	reinterpret_cast<void(*)()>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_8_b__9_0_System_Int32),
	reinterpret_cast<void(*)()>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_9),
	reinterpret_cast<void(*)()>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_9_b__10_0_System_Int32),
	reinterpret_cast<void(*)()>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_10),
	reinterpret_cast<void(*)()>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_10_b__11_0_System_Int32_System_Int32),
	reinterpret_cast<void(*)()>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_11),
	reinterpret_cast<void(*)()>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_12),
	reinterpret_cast<void(*)()>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_12_b__13_0_System_Int32),
	reinterpret_cast<void(*)()>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_13),
	reinterpret_cast<void(*)()>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_14),
	reinterpret_cast<void(*)()>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_15),
	reinterpret_cast<void(*)()>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_15_b__16_0_System_Int32),
	reinterpret_cast<void(*)()>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_16),
	reinterpret_cast<void(*)()>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_16_b__17_0_System_Int32),
	reinterpret_cast<void(*)()>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_17),
	reinterpret_cast<void(*)()>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_17_b__18_0_System_Int32),
	reinterpret_cast<void(*)()>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_18),
	reinterpret_cast<void(*)()>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_19),
	reinterpret_cast<void(*)()>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_19_b__20_0_System_Int32),
	reinterpret_cast<void(*)()>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_20),
	reinterpret_cast<void(*)()>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_21),
	reinterpret_cast<void(*)()>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_22),
	reinterpret_cast<void(*)()>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_22_b__23_0_System_Int32),
	reinterpret_cast<void(*)()>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_23),
	reinterpret_cast<void(*)()>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_23_b__24_0_System_Int32),
	reinterpret_cast<void(*)()>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_cctor),
	reinterpret_cast<void(*)()>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c__ctor),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
extern "C" void (*kBenchmarkWrappers[37])() = {
	[]() {kAotMethods[0]();},
	[]() {kAotMethods[1]();},
	[]() {kAotMethods[2]();},
	[]() {kAotMethods[3]();},
	[]() {kAotMethods[4]();},
	[]() {kAotMethods[5]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[6])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[7]();},
	[]() {kAotMethods[8]();},
	[]() {kAotMethods[9]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[10])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[11]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[12])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[13]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[14])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0,0);},
	[]() {kAotMethods[15]();},
	[]() {kAotMethods[16]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[17])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[18]();},
	[]() {kAotMethods[19]();},
	[]() {kAotMethods[20]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[21])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[22]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[23])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[24]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[25])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[26]();},
	[]() {kAotMethods[27]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[28])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[29]();},
	[]() {kAotMethods[30]();},
	[]() {kAotMethods[31]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[32])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[33]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[34])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[35]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[36])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
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
static void* const kMethodPointers[37] = {
	reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_0),
	reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_1),
	reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_2),
	reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_3),
	reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_4),
	reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_5),
	reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_5_b__6_0_System_Int32),
	reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_6),
	reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_7),
	reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_8),
	reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_8_b__9_0_System_Int32),
	reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_9),
	reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_9_b__10_0_System_Int32),
	reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_10),
	reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_10_b__11_0_System_Int32_System_Int32),
	reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_11),
	reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_12),
	reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_12_b__13_0_System_Int32),
	reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_13),
	reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_14),
	reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_15),
	reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_15_b__16_0_System_Int32),
	reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_16),
	reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_16_b__17_0_System_Int32),
	reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_17),
	reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_17_b__18_0_System_Int32),
	reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_18),
	reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_19),
	reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_19_b__20_0_System_Int32),
	reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_20),
	reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_21),
	reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_22),
	reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_22_b__23_0_System_Int32),
	reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_23),
	reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_23_b__24_0_System_Int32),
	reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_cctor),
	reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c__ctor),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
// ── VTable descriptors (for BootstrapRuntime TypeVTable registration) ──
static const VTableDescriptorV0 kChaosVTableDescriptors[] = {
	{
		CHAOS_IL2CPP_UINT64_C(0x71C8B251034D0D9E),
		0x00000002u,
		0u,
		12u,
		reinterpret_cast<const void*>(kSlots_ShuffleIndexSubjects_ShuffleIndexSubjects___c),
		reinterpret_cast<const void**>(chaos_vtable_ShuffleIndexSubjects_ShuffleIndexSubjects___c),
		12u,
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
	.method_pointer_count      = 37u,
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
	.image_name_utf8    = "ShuffleIndexSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_ShuffleIndexSubjects_ShuffleIndexSubjects[1] = {
	{ 0u, "ShuffleIndexSubjects/ShuffleIndexSubjects::_exitCode", "_exitCode", "System.Int32", 0LL },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_ShuffleIndexSubjects_ShuffleIndexSubjects[24] = {
	{ 0u, "ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_0:System.Void()", "CustomEntrySubject_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_1:System.Void()", "CustomEntrySubject_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_2:System.Void()", "CustomEntrySubject_2", "System.Void", 0, nullptr, 0u },
	{ 0u, "ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_3:System.Void()", "CustomEntrySubject_3", "System.Void", 0, nullptr, 0u },
	{ 0u, "ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_4:System.Void()", "CustomEntrySubject_4", "System.Void", 0, nullptr, 0u },
	{ 0u, "ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_5:System.Void()", "CustomEntrySubject_5", "System.Void", 0, nullptr, 0u },
	{ 0u, "ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_6:System.Void()", "CustomEntrySubject_6", "System.Void", 0, nullptr, 0u },
	{ 0u, "ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_7:System.Void()", "CustomEntrySubject_7", "System.Void", 0, nullptr, 0u },
	{ 0u, "ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_8:System.Void()", "CustomEntrySubject_8", "System.Void", 0, nullptr, 0u },
	{ 0u, "ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_9:System.Void()", "CustomEntrySubject_9", "System.Void", 0, nullptr, 0u },
	{ 0u, "ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_10:System.Void()", "CustomEntrySubject_10", "System.Void", 0, nullptr, 0u },
	{ 0u, "ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_11:System.Void()", "CustomEntrySubject_11", "System.Void", 0, nullptr, 0u },
	{ 0u, "ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_12:System.Void()", "CustomEntrySubject_12", "System.Void", 0, nullptr, 0u },
	{ 0u, "ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_13:System.Void()", "CustomEntrySubject_13", "System.Void", 0, nullptr, 0u },
	{ 0u, "ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_14:System.Void()", "CustomEntrySubject_14", "System.Void", 0, nullptr, 0u },
	{ 0u, "ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_15:System.Void()", "CustomEntrySubject_15", "System.Void", 0, nullptr, 0u },
	{ 0u, "ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_16:System.Void()", "CustomEntrySubject_16", "System.Void", 0, nullptr, 0u },
	{ 0u, "ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_17:System.Void()", "CustomEntrySubject_17", "System.Void", 0, nullptr, 0u },
	{ 0u, "ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_18:System.Void()", "CustomEntrySubject_18", "System.Void", 0, nullptr, 0u },
	{ 0u, "ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_19:System.Void()", "CustomEntrySubject_19", "System.Void", 0, nullptr, 0u },
	{ 0u, "ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_20:System.Void()", "CustomEntrySubject_20", "System.Void", 0, nullptr, 0u },
	{ 0u, "ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_21:System.Void()", "CustomEntrySubject_21", "System.Void", 0, nullptr, 0u },
	{ 0u, "ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_22:System.Void()", "CustomEntrySubject_22", "System.Void", 0, nullptr, 0u },
	{ 0u, "ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_23:System.Void()", "CustomEntrySubject_23", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_ShuffleIndexSubjects_ShuffleIndexSubjects___c[13] = {
	{ 0u, "ShuffleIndexSubjects/ShuffleIndexSubjects+<>c::<CustomEntrySubject_5>b__6_0:System.Boolean(System.Int32)", "<CustomEntrySubject_5>b__6_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ShuffleIndexSubjects/ShuffleIndexSubjects+<>c::<CustomEntrySubject_8>b__9_0:System.Boolean(System.Int32)", "<CustomEntrySubject_8>b__9_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ShuffleIndexSubjects/ShuffleIndexSubjects+<>c::<CustomEntrySubject_9>b__10_0:System.Boolean(System.Int32)", "<CustomEntrySubject_9>b__10_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ShuffleIndexSubjects/ShuffleIndexSubjects+<>c::<CustomEntrySubject_10>b__11_0:System.ValueTuple<System.Int32,System.Int32>(System.Int32,System.Int32)", "<CustomEntrySubject_10>b__11_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ShuffleIndexSubjects/ShuffleIndexSubjects+<>c::<CustomEntrySubject_12>b__13_0:System.Boolean(System.Int32)", "<CustomEntrySubject_12>b__13_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ShuffleIndexSubjects/ShuffleIndexSubjects+<>c::<CustomEntrySubject_15>b__16_0:System.Boolean(System.Int32)", "<CustomEntrySubject_15>b__16_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ShuffleIndexSubjects/ShuffleIndexSubjects+<>c::<CustomEntrySubject_16>b__17_0:System.Boolean(System.Int32)", "<CustomEntrySubject_16>b__17_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ShuffleIndexSubjects/ShuffleIndexSubjects+<>c::<CustomEntrySubject_17>b__18_0:System.Int32(System.Int32)", "<CustomEntrySubject_17>b__18_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ShuffleIndexSubjects/ShuffleIndexSubjects+<>c::<CustomEntrySubject_19>b__20_0:System.Boolean(System.Int32)", "<CustomEntrySubject_19>b__20_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ShuffleIndexSubjects/ShuffleIndexSubjects+<>c::<CustomEntrySubject_22>b__23_0:System.Boolean(System.Int32)", "<CustomEntrySubject_22>b__23_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ShuffleIndexSubjects/ShuffleIndexSubjects+<>c::<CustomEntrySubject_23>b__24_0:System.Boolean(System.Int32)", "<CustomEntrySubject_23>b__24_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ShuffleIndexSubjects/ShuffleIndexSubjects+<>c::.cctor:System.Void()", ".cctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "ShuffleIndexSubjects/ShuffleIndexSubjects+<>c::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[2] = {
	{ 0u, "ShuffleIndexSubjects/ShuffleIndexSubjects", "ShuffleIndexSubjects/ShuffleIndexSubjects", "", "ShuffleIndexSubjects", "ShuffleIndexSubjects", nullptr, kReflFields_ShuffleIndexSubjects_ShuffleIndexSubjects, 1u, nullptr, 0u,
	kReflMethods_ShuffleIndexSubjects_ShuffleIndexSubjects, 24u },
	{ 0u, "ShuffleIndexSubjects/ShuffleIndexSubjects+<>c", "ShuffleIndexSubjects/ShuffleIndexSubjects+<>c", "", "ShuffleIndexSubjects+<>c", "ShuffleIndexSubjects+<>c", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_ShuffleIndexSubjects_ShuffleIndexSubjects___c, 13u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[2] = {
	&kReflTypes[0],
	&kReflTypes[1],
};

static const ReflectionQueryImageDescriptor kReflImage = { "ShuffleIndexSubjects", kReflTypePtrs, 2u };

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
// Managed method: ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_0()
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_0(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
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
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(70775822u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s1);
		chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers__InitializeArray_System_Void_System_Array_System_RuntimeFieldHandle_(chaos_arg_0, chaos_arg_1);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[3])(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_1()
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_1(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
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
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(70775822u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s1);
		chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers__InitializeArray_System_Void_System_Array_System_RuntimeFieldHandle_(chaos_arg_0, chaos_arg_1);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Index, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Index.hot;
		_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[5])(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_2()
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_2(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
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
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(10);
	{
		auto chaos_value_raw = _s3;
		const auto chaos_value = chaos_value_raw;
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s2);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s1);
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
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(20);
	{
		auto chaos_value_raw = _s3;
		const auto chaos_value = chaos_value_raw;
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s2);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s1);
		chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)] = chaos_value;
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[6])(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_3()
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_3(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
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
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(10);
	{
		auto chaos_value_raw = _s3;
		const auto chaos_value = chaos_value_raw;
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s2);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s1);
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
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(20);
	{
		auto chaos_value_raw = _s3;
		const auto chaos_value = chaos_value_raw;
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s2);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s1);
		chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)] = chaos_value;
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Index, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Index.hot;
		_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[7])(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_4()
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_4(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
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
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(81777074u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s1);
		chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers__InitializeArray_System_Void_System_Array_System_RuntimeFieldHandle_(chaos_arg_0, chaos_arg_1);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[9])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_5()
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_5(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
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
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(68691011u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s1);
		chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers__InitializeArray_System_Void_System_Array_System_RuntimeFieldHandle_(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_ShuffleIndexSubjects_ShuffleIndexSubjects___c();
	_s1 = chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9__6_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_ShuffleIndexSubjects_ShuffleIndexSubjects___c();
			_s1 = chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 6)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
					auto& _d_entry = s_hotpatch_entries[6];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[12];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
						_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_fn_arg_1));
						CHAOS_IL2CPP_INTPTR _d_ret{};
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, &_d_ret);
						return _d_ret;
					}
					return reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1);
				};
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s2;
				const auto chaos_target = _s1;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Boolean_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Boolean_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_ShuffleIndexSubjects_ShuffleIndexSubjects___c();
				chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9__6_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[14])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: ShuffleIndexSubjects/ShuffleIndexSubjects+<>c::<CustomEntrySubject_5>b__6_0(System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_5_b__6_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0) > static_cast<CHAOS_IL2CPP_INT32>(_s1) ? 1 : 0);
	return _s0;
}

// Managed method: ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_6()
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_6(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
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
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(81777074u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s1);
		chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers__InitializeArray_System_Void_System_Array_System_RuntimeFieldHandle_(chaos_arg_0, chaos_arg_1);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[15])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_7()
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_7(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
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
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[16])(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_8()
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_8(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
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
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(68691011u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s1);
		chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers__InitializeArray_System_Void_System_Array_System_RuntimeFieldHandle_(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_ShuffleIndexSubjects_ShuffleIndexSubjects___c();
	_s1 = chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9__9_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_ShuffleIndexSubjects_ShuffleIndexSubjects___c();
			_s1 = chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 10)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
					auto& _d_entry = s_hotpatch_entries[10];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[12];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
						_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_fn_arg_1));
						CHAOS_IL2CPP_INTPTR _d_ret{};
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, &_d_ret);
						return _d_ret;
					}
					return reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1);
				};
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s2;
				const auto chaos_target = _s1;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Boolean_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Boolean_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_ShuffleIndexSubjects_ShuffleIndexSubjects___c();
				chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9__9_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[18])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: ShuffleIndexSubjects/ShuffleIndexSubjects+<>c::<CustomEntrySubject_8>b__9_0(System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_8_b__9_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(10);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0) > static_cast<CHAOS_IL2CPP_INT32>(_s1) ? 1 : 0);
	return _s0;
}

// Managed method: ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_9()
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_9(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
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
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(68691011u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s1);
		chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers__InitializeArray_System_Void_System_Array_System_RuntimeFieldHandle_(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_ShuffleIndexSubjects_ShuffleIndexSubjects___c();
	_s1 = chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9__10_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_ShuffleIndexSubjects_ShuffleIndexSubjects___c();
			_s1 = chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 12)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
					auto& _d_entry = s_hotpatch_entries[12];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[12];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
						_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_fn_arg_1));
						CHAOS_IL2CPP_INTPTR _d_ret{};
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, &_d_ret);
						return _d_ret;
					}
					return reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1);
				};
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s2;
				const auto chaos_target = _s1;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Boolean_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Boolean_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_ShuffleIndexSubjects_ShuffleIndexSubjects___c();
				chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9__10_0 = chaos_value;
			}
		}
		_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(99);
		{
			const auto chaos_arg_2 = _s6;
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[20])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: ShuffleIndexSubjects/ShuffleIndexSubjects+<>c::<CustomEntrySubject_9>b__10_0(System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_9_b__10_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(10);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0) > static_cast<CHAOS_IL2CPP_INT32>(_s1) ? 1 : 0);
	return _s0;
}

// Managed method: ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_10()
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_10(void)
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
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
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
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(81777074u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s1);
		chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers__InitializeArray_System_Void_System_Array_System_RuntimeFieldHandle_(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_ShuffleIndexSubjects_ShuffleIndexSubjects___c();
	_s1 = chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9__11_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_ShuffleIndexSubjects_ShuffleIndexSubjects___c();
			_s1 = chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 14)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2) -> CHAOS_IL2CPP_INTPTR {
					auto& _d_entry = s_hotpatch_entries[14];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[16];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
						_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_fn_arg_1));
						_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_fn_arg_2));
						CHAOS_IL2CPP_INTPTR _d_ret{};
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, &_d_ret);
						return _d_ret;
					}
					return reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32, CHAOS_IL2CPP_INT32)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1, chaos_fn_arg_2);
				};
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s2;
				const auto chaos_target = _s1;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Int32_System_ValueTuple_System_Int32_System_Int32__, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Int32_System_ValueTuple_System_Int32_System_Int32__.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_ShuffleIndexSubjects_ShuffleIndexSubjects___c();
				chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9__11_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[23])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		chaos_locals[1] = _s4;
		_s4 = chaos_locals[1];
		{
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[24])();
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		chaos_locals[2] = _s5;
		_s5 = chaos_locals[2];
		{
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[25])();
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: ShuffleIndexSubjects/ShuffleIndexSubjects+<>c::<CustomEntrySubject_10>b__11_0(System.Int32,System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_10_b__11_0_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);
	chaos_args[2] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_2);

	_s0 = chaos_args[2];
	_s1 = chaos_args[1];
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_ValueTuple_System_Int32_System_Int32_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_ValueTuple_System_Int32_System_Int32_.hot;
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	return _s0;
}

// Managed method: ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_11()
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_11(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
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
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(81777074u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s1);
		chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers__InitializeArray_System_Void_System_Array_System_RuntimeFieldHandle_(chaos_arg_0, chaos_arg_1);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[27])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_12()
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_12(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
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
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(68691011u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s1);
		chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers__InitializeArray_System_Void_System_Array_System_RuntimeFieldHandle_(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_ShuffleIndexSubjects_ShuffleIndexSubjects___c();
	_s1 = chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9__13_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_ShuffleIndexSubjects_ShuffleIndexSubjects___c();
			_s1 = chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 17)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
					auto& _d_entry = s_hotpatch_entries[17];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[12];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
						_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_fn_arg_1));
						CHAOS_IL2CPP_INTPTR _d_ret{};
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, &_d_ret);
						return _d_ret;
					}
					return reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1);
				};
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s2;
				const auto chaos_target = _s1;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Boolean_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Boolean_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_ShuffleIndexSubjects_ShuffleIndexSubjects___c();
				chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9__13_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[29])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: ShuffleIndexSubjects/ShuffleIndexSubjects+<>c::<CustomEntrySubject_12>b__13_0(System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_12_b__13_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0) < static_cast<CHAOS_IL2CPP_INT32>(_s1) ? 1 : 0);
	return _s0;
}

// Managed method: ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_13()
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_13(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
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
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(81777074u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s1);
		chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers__InitializeArray_System_Void_System_Array_System_RuntimeFieldHandle_(chaos_arg_0, chaos_arg_1);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[30])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_14()
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_14(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
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
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[31])(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_15()
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_15(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
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
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(68691011u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s1);
		chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers__InitializeArray_System_Void_System_Array_System_RuntimeFieldHandle_(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_ShuffleIndexSubjects_ShuffleIndexSubjects___c();
	_s1 = chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9__16_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_ShuffleIndexSubjects_ShuffleIndexSubjects___c();
			_s1 = chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 21)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
					auto& _d_entry = s_hotpatch_entries[21];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[12];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
						_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_fn_arg_1));
						CHAOS_IL2CPP_INTPTR _d_ret{};
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, &_d_ret);
						return _d_ret;
					}
					return reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1);
				};
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s2;
				const auto chaos_target = _s1;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Boolean_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Boolean_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_ShuffleIndexSubjects_ShuffleIndexSubjects___c();
				chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9__16_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[33])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: ShuffleIndexSubjects/ShuffleIndexSubjects+<>c::<CustomEntrySubject_15>b__16_0(System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_15_b__16_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(10);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0) > static_cast<CHAOS_IL2CPP_INT32>(_s1) ? 1 : 0);
	return _s0;
}

// Managed method: ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_16()
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_16(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
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
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(68691011u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s1);
		chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers__InitializeArray_System_Void_System_Array_System_RuntimeFieldHandle_(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_ShuffleIndexSubjects_ShuffleIndexSubjects___c();
	_s1 = chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9__17_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_ShuffleIndexSubjects_ShuffleIndexSubjects___c();
			_s1 = chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 23)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
					auto& _d_entry = s_hotpatch_entries[23];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[12];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
						_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_fn_arg_1));
						CHAOS_IL2CPP_INTPTR _d_ret{};
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, &_d_ret);
						return _d_ret;
					}
					return reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1);
				};
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s2;
				const auto chaos_target = _s1;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Boolean_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Boolean_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_ShuffleIndexSubjects_ShuffleIndexSubjects___c();
				chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9__17_0 = chaos_value;
			}
		}
		_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(99);
		{
			const auto chaos_arg_2 = _s6;
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[35])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: ShuffleIndexSubjects/ShuffleIndexSubjects+<>c::<CustomEntrySubject_16>b__17_0(System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_16_b__17_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(10);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0) > static_cast<CHAOS_IL2CPP_INT32>(_s1) ? 1 : 0);
	return _s0;
}

// Managed method: ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_17()
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_17(void)
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
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
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
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(68691011u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s1);
		chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers__InitializeArray_System_Void_System_Array_System_RuntimeFieldHandle_(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_ShuffleIndexSubjects_ShuffleIndexSubjects___c();
	_s1 = chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9__18_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_ShuffleIndexSubjects_ShuffleIndexSubjects___c();
			_s1 = chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 25)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INT32 {
					auto& _d_entry = s_hotpatch_entries[25];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[12];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
						_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_fn_arg_1));
						CHAOS_IL2CPP_INT32 _d_ret{};
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, &_d_ret);
						return _d_ret;
					}
					return reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1);
				};
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s2;
				const auto chaos_target = _s1;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Int32_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Int32_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_ShuffleIndexSubjects_ShuffleIndexSubjects___c();
				chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9__18_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[38])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		chaos_locals[1] = _s4;
		_s4 = chaos_locals[1];
		{
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[39])();
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		chaos_locals[2] = _s5;
		_s5 = chaos_locals[2];
		{
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[25])();
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: ShuffleIndexSubjects/ShuffleIndexSubjects+<>c::<CustomEntrySubject_17>b__18_0(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_17_b__18_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[40])();
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Random__Next_System_Int32__();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s1);
}

// Managed method: ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_18()
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_18(void)
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
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		auto chaos_value_raw = _s3;
		const auto chaos_value = chaos_value_raw;
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s2);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s1);
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
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[42])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_19()
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_19(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
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
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(70775822u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s1);
		chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers__InitializeArray_System_Void_System_Array_System_RuntimeFieldHandle_(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_ShuffleIndexSubjects_ShuffleIndexSubjects___c();
	_s1 = chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9__20_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_ShuffleIndexSubjects_ShuffleIndexSubjects___c();
			_s1 = chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 28)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
					auto& _d_entry = s_hotpatch_entries[28];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[12];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
						_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_fn_arg_1));
						CHAOS_IL2CPP_INTPTR _d_ret{};
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, &_d_ret);
						return _d_ret;
					}
					return reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1);
				};
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s2;
				const auto chaos_target = _s1;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Boolean_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Boolean_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_ShuffleIndexSubjects_ShuffleIndexSubjects___c();
				chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9__20_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[44])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: ShuffleIndexSubjects/ShuffleIndexSubjects+<>c::<CustomEntrySubject_19>b__20_0(System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_19_b__20_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(30);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	return _s0;
}

// Managed method: ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_20()
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_20(void)
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
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		auto chaos_value_raw = _s3;
		const auto chaos_value = chaos_value_raw;
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s2);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s1);
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
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[45])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_21()
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_21(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
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
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(99);
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[46])(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_22()
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_22(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
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
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(70775822u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s1);
		chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers__InitializeArray_System_Void_System_Array_System_RuntimeFieldHandle_(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_ShuffleIndexSubjects_ShuffleIndexSubjects___c();
	_s1 = chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9__23_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_ShuffleIndexSubjects_ShuffleIndexSubjects___c();
			_s1 = chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 32)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
					auto& _d_entry = s_hotpatch_entries[32];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[12];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
						_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_fn_arg_1));
						CHAOS_IL2CPP_INTPTR _d_ret{};
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, &_d_ret);
						return _d_ret;
					}
					return reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1);
				};
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s2;
				const auto chaos_target = _s1;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Boolean_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Boolean_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_ShuffleIndexSubjects_ShuffleIndexSubjects___c();
				chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9__23_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[48])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: ShuffleIndexSubjects/ShuffleIndexSubjects+<>c::<CustomEntrySubject_22>b__23_0(System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_22_b__23_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(100);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	return _s0;
}

// Managed method: ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_23()
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_23(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
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
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(70775822u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s1);
		chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers__InitializeArray_System_Void_System_Array_System_RuntimeFieldHandle_(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_ShuffleIndexSubjects_ShuffleIndexSubjects___c();
	_s1 = chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9__24_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_ShuffleIndexSubjects_ShuffleIndexSubjects___c();
			_s1 = chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 34)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
					auto& _d_entry = s_hotpatch_entries[34];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[12];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
						_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_fn_arg_1));
						CHAOS_IL2CPP_INTPTR _d_ret{};
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, &_d_ret);
						return _d_ret;
					}
					return reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1);
				};
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s2;
				const auto chaos_target = _s1;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Boolean_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Boolean_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_ShuffleIndexSubjects_ShuffleIndexSubjects___c();
				chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9__24_0 = chaos_value;
			}
		}
		_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(99);
		{
			const auto chaos_arg_2 = _s6;
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[50])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: ShuffleIndexSubjects/ShuffleIndexSubjects+<>c::<CustomEntrySubject_23>b__24_0(System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_23_b__24_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(100);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	return _s0;
}

// Managed method: ShuffleIndexSubjects/ShuffleIndexSubjects+<>c::.cctor()
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects___c_cctor(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};

	chaos_ensure_type_initialized_ShuffleIndexSubjects_ShuffleIndexSubjects___c();

	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_ShuffleIndexSubjects_ShuffleIndexSubjects___c, {});
		chaos_object->header.type_info = &chaos_mt_ShuffleIndexSubjects_ShuffleIndexSubjects___c.hot;
		ShuffleIndexSubjects_ShuffleIndexSubjects___c__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	{
		auto chaos_value = _s0;
	chaos_ensure_type_initialized_ShuffleIndexSubjects_ShuffleIndexSubjects___c();
		chaos_static_ShuffleIndexSubjects_ShuffleIndexSubjects___c____9 = chaos_value;
	}
	return;
}

// Managed method: ShuffleIndexSubjects/ShuffleIndexSubjects+<>c::.ctor()
extern "C" void ShuffleIndexSubjects_ShuffleIndexSubjects___c__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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



}  // namespace chaos::il2cpp::codegen::ShuffleIndexSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 37;

extern "C" void ChaosJitRegisterAll() {}