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
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ShuffleIndexSubjects::CustomEntrySubject_0
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ShuffleIndexSubjects::CustomEntrySubject_1
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ShuffleIndexSubjects::CustomEntrySubject_2
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ShuffleIndexSubjects::CustomEntrySubject_3
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ShuffleIndexSubjects::CustomEntrySubject_4
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ShuffleIndexSubjects::CustomEntrySubject_5
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_5_b__6_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ShuffleIndexSubjects+<>c::<CustomEntrySubject_5>b__6_0
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ShuffleIndexSubjects::CustomEntrySubject_6
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ShuffleIndexSubjects::CustomEntrySubject_7
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ShuffleIndexSubjects::CustomEntrySubject_8
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_8_b__9_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ShuffleIndexSubjects+<>c::<CustomEntrySubject_8>b__9_0
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_9), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ShuffleIndexSubjects::CustomEntrySubject_9
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_9_b__10_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ShuffleIndexSubjects+<>c::<CustomEntrySubject_9>b__10_0
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_10), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ShuffleIndexSubjects::CustomEntrySubject_10
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_10_b__11_0_System_Int32_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ShuffleIndexSubjects+<>c::<CustomEntrySubject_10>b__11_0
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_11), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ShuffleIndexSubjects::CustomEntrySubject_11
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_12), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ShuffleIndexSubjects::CustomEntrySubject_12
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_12_b__13_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ShuffleIndexSubjects+<>c::<CustomEntrySubject_12>b__13_0
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_13), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ShuffleIndexSubjects::CustomEntrySubject_13
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_14), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ShuffleIndexSubjects::CustomEntrySubject_14
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_15), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ShuffleIndexSubjects::CustomEntrySubject_15
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_15_b__16_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ShuffleIndexSubjects+<>c::<CustomEntrySubject_15>b__16_0
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_16), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ShuffleIndexSubjects::CustomEntrySubject_16
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_16_b__17_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ShuffleIndexSubjects+<>c::<CustomEntrySubject_16>b__17_0
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_17), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ShuffleIndexSubjects::CustomEntrySubject_17
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_17_b__18_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ShuffleIndexSubjects+<>c::<CustomEntrySubject_17>b__18_0
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_18), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ShuffleIndexSubjects::CustomEntrySubject_18
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_19), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ShuffleIndexSubjects::CustomEntrySubject_19
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_19_b__20_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ShuffleIndexSubjects+<>c::<CustomEntrySubject_19>b__20_0
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_20), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ShuffleIndexSubjects::CustomEntrySubject_20
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_21), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ShuffleIndexSubjects::CustomEntrySubject_21
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_22), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ShuffleIndexSubjects::CustomEntrySubject_22
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_22_b__23_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ShuffleIndexSubjects+<>c::<CustomEntrySubject_22>b__23_0
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_23), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ShuffleIndexSubjects::CustomEntrySubject_23
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_23_b__24_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ShuffleIndexSubjects+<>c::<CustomEntrySubject_23>b__24_0
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_cctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ShuffleIndexSubjects+<>c::.cctor
	{ reinterpret_cast<void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ShuffleIndexSubjects+<>c::.ctor
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

// ── Subject entry index mapping ─────────────────────────────────
// Maps subject index (0-based sequential) to kAotMethod index.
// Used by runtime-entry.cpp to route --benchmark N to the correct
// AOT method slot, since kAotMethods[] includes lambdas/closures
// that shift subject methods to non-contiguous indices.
extern "C" const int kSubjectEntryCount = 24;
extern "C" const int kSubjectEntryIndices[24] = {
	0,
	1,
	2,
	3,
	4,
	5,
	7,
	8,
	9,
	11,
	13,
	15,
	16,
	18,
	19,
	20,
	22,
	24,
	26,
	27,
	29,
	30,
	31,
	33,
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

	/* ── Entry 0: ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry0;
	/* ── Entry 1: ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry1;
	/* ── Entry 2: ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_2 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry2;
	/* ── Entry 3: ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_3 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry3;
	/* ── Entry 4: ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_4 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry4;
	/* ── Entry 5: ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_5 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry5;
	/* ── Entry 6: ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_5_b__6_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry6;
	/* ── Entry 7: ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_6 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry7;
	/* ── Entry 8: ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_7 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry8;
	/* ── Entry 9: ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_8 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry9;
	/* ── Entry 10: ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_8_b__9_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry10;
	/* ── Entry 11: ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_9 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry11;
	/* ── Entry 12: ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_9_b__10_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry12;
	/* ── Entry 13: ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_10 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry13;
	/* ── Entry 14: ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_10_b__11_0_System_Int32_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry14;
	/* ── Entry 15: ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_11 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry15;
	/* ── Entry 16: ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_12 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry16;
	/* ── Entry 17: ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_12_b__13_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry17;
	/* ── Entry 18: ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_13 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry18;
	/* ── Entry 19: ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_14 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry19;
	/* ── Entry 20: ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_15 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry20;
	/* ── Entry 21: ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_15_b__16_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry21;
	/* ── Entry 22: ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_16 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry22;
	/* ── Entry 23: ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_16_b__17_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry23;
	/* ── Entry 24: ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_17 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry24;
	/* ── Entry 25: ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_17_b__18_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry25;
	/* ── Entry 26: ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_18 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry26;
	/* ── Entry 27: ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_19 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry27;
	/* ── Entry 28: ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_19_b__20_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry28;
	/* ── Entry 29: ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_20 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry29;
	/* ── Entry 30: ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_21 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry30;
	/* ── Entry 31: ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_22 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry31;
	/* ── Entry 32: ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_22_b__23_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry32;
	/* ── Entry 33: ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_23 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry33;
	/* ── Entry 34: ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_23_b__24_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry34;
	/* ── Entry 35: ShuffleIndexSubjects_ShuffleIndexSubjects___c_cctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry35;
	/* ── Entry 36: ShuffleIndexSubjects_ShuffleIndexSubjects___c__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry36;
} kChaosGcSlotMapsSection = {
	/* entry0 = ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_0 */
	.entry0 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_0),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry1 = ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_1 */
	.entry1 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_1),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry2 = ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_2 */
	.entry2 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_2),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry3 = ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_3 */
	.entry3 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_3),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry4 = ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_4 */
	.entry4 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_4),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry5 = ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_5 */
	.entry5 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_5),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry6 = ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_5_b__6_0_System_Int32 */
	.entry6 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_5_b__6_0_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 16u, 24u }
	},
	/* entry7 = ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_6 */
	.entry7 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_6),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry8 = ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_7 */
	.entry8 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_7),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry9 = ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_8 */
	.entry9 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_8),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry10 = ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_8_b__9_0_System_Int32 */
	.entry10 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_8_b__9_0_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 16u, 24u }
	},
	/* entry11 = ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_9 */
	.entry11 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_9),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry12 = ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_9_b__10_0_System_Int32 */
	.entry12 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_9_b__10_0_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 16u, 24u }
	},
	/* entry13 = ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_10 */
	.entry13 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_10),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry14 = ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_10_b__11_0_System_Int32_System_Int32 */
	.entry14 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_10_b__11_0_System_Int32_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 24u, 32u }
	},
	/* entry15 = ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_11 */
	.entry15 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_11),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry16 = ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_12 */
	.entry16 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_12),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry17 = ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_12_b__13_0_System_Int32 */
	.entry17 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_12_b__13_0_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 16u, 24u }
	},
	/* entry18 = ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_13 */
	.entry18 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_13),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry19 = ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_14 */
	.entry19 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_14),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry20 = ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_15 */
	.entry20 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_15),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry21 = ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_15_b__16_0_System_Int32 */
	.entry21 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_15_b__16_0_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 16u, 24u }
	},
	/* entry22 = ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_16 */
	.entry22 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_16),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry23 = ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_16_b__17_0_System_Int32 */
	.entry23 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_16_b__17_0_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 16u, 24u }
	},
	/* entry24 = ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_17 */
	.entry24 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_17),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry25 = ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_17_b__18_0_System_Int32 */
	.entry25 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_17_b__18_0_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 16u, 24u }
	},
	/* entry26 = ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_18 */
	.entry26 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_18),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry27 = ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_19 */
	.entry27 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_19),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry28 = ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_19_b__20_0_System_Int32 */
	.entry28 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_19_b__20_0_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 16u, 24u }
	},
	/* entry29 = ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_20 */
	.entry29 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_20),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry30 = ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_21 */
	.entry30 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_21),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry31 = ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_22 */
	.entry31 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_22),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry32 = ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_22_b__23_0_System_Int32 */
	.entry32 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_22_b__23_0_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 16u, 24u }
	},
	/* entry33 = ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_23 */
	.entry33 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_23),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry34 = ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_23_b__24_0_System_Int32 */
	.entry34 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_23_b__24_0_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 16u, 24u }
	},
	/* entry35 = ShuffleIndexSubjects_ShuffleIndexSubjects___c_cctor */
	.entry35 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c_cctor),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry36 = ShuffleIndexSubjects_ShuffleIndexSubjects___c__ctor */
	.entry36 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ShuffleIndexSubjects_ShuffleIndexSubjects___c__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 1384u;

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
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
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
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
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
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
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
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
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
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
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
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
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
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
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
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
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
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
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
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
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
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
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
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
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
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
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
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
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
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
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
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
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
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
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
		ChaosObjectCtor(chaos_arg_0);
	}
	return;
}



}  // namespace chaos::il2cpp::codegen::ShuffleIndexSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 37;

// ── JIT Method Entry Table ───────────────────────────────────────────
// Auto-generated by chaos-il2cpp codegen for --mode jit.
#include <cstdint>
#include "jit_registration.h"

static const char kMethodJson_0[] = "{\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-0\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_0:System.Void()\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_0()\",\"identity\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\",\"definitionSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_0:System.Void()\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_0:System.Void()\",\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-0\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_0()\"},\"nativeSymbol\":\"ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":5,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Int32\",\"ilOffset\":2,\"resultType\":\"System.Int32[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Int32\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::6E28B4D41AF8A88650219653A728E0FD6760BB2B1DFEB7A9EB8DEB5B4E7010E0\",\"ilOffset\":8,\"resultType\":\"System.RuntimeFieldHandle\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::6E28B4D41AF8A88650219653A728E0FD6760BB2B1DFEB7A9EB8DEB5B4E7010E0\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::6E28B4D41AF8A88650219653A728E0FD6760BB2B1DFEB7A9EB8DEB5B4E7010E0\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"\\u003CPrivateImplementationDetails\\u003E\\u002B__StaticArrayInitTypeSize=20\"},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":13,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":19,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":2,\"ilOffset\":20,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"callee\":\"System.Linq/Enumerable::ElementAt\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Int32)\",\"reference\":{\"assemblyName\":\"System.Linq\",\"subjectKind\":\"method\",\"subjectId\":\"System.Linq/Enumerable::ElementAt\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Linq\",\"subjectId\":\"System.Linq/Enumerable::ElementAt\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Int32)\",\"openDefinitionSubjectId\":\"System.Linq/Enumerable::ElementAt:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Int32)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::ElementAt:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Int32);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::ElementAt:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Int32);type=[];method=[System.Int32]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::ElementAt:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Int32)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::ElementAt:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Int32);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::ElementAt:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Int32);type=[];method=[System.Int32]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Linq/Enumerable::ElementAt\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Int32)\",\"definitionSubjectId\":\"System.Linq/Enumerable::ElementAt:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Int32)\",\"displaySubjectId\":\"System.Linq/Enumerable::ElementAt\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Int32)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::ElementAt:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Int32)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":26,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":27,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ShuffleIndexSubjects\"}";
static const char kMethodJson_1[] = "{\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-1\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_1:System.Void()\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_1()\",\"identity\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\",\"definitionSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_1:System.Void()\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_1:System.Void()\",\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-1\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_1()\"},\"nativeSymbol\":\"ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":5,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Int32\",\"ilOffset\":2,\"resultType\":\"System.Int32[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Int32\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::6E28B4D41AF8A88650219653A728E0FD6760BB2B1DFEB7A9EB8DEB5B4E7010E0\",\"ilOffset\":8,\"resultType\":\"System.RuntimeFieldHandle\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::6E28B4D41AF8A88650219653A728E0FD6760BB2B1DFEB7A9EB8DEB5B4E7010E0\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::6E28B4D41AF8A88650219653A728E0FD6760BB2B1DFEB7A9EB8DEB5B4E7010E0\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"\\u003CPrivateImplementationDetails\\u003E\\u002B__StaticArrayInitTypeSize=20\"},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":13,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":19,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":2,\"ilOffset\":20,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":22,\"resultType\":\"System.Private.CoreLib/System.Index\",\"callee\":\"System.Private.CoreLib/System.Index::.ctor:System.Void(System.Int32,System.Boolean)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Index::.ctor:System.Void(System.Int32,System.Boolean)\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Index\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":27,\"resultType\":\"System.Int32\",\"callee\":\"System.Linq/Enumerable::ElementAt\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Index)\",\"reference\":{\"assemblyName\":\"System.Linq\",\"subjectKind\":\"method\",\"subjectId\":\"System.Linq/Enumerable::ElementAt\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Index)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Linq\",\"subjectId\":\"System.Linq/Enumerable::ElementAt\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Index)\",\"openDefinitionSubjectId\":\"System.Linq/Enumerable::ElementAt:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Index)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::ElementAt:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Index);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::ElementAt:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Index);type=[];method=[System.Int32]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::ElementAt:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Index)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::ElementAt:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Index);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::ElementAt:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Index);type=[];method=[System.Int32]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Linq/Enumerable::ElementAt\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Index)\",\"definitionSubjectId\":\"System.Linq/Enumerable::ElementAt:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Index)\",\"displaySubjectId\":\"System.Linq/Enumerable::ElementAt\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Index)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::ElementAt:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Index)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":32,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":33,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ShuffleIndexSubjects\"}";
static const char kMethodJson_2[] = "{\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-2\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_2:System.Void()\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_2()\",\"identity\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\",\"definitionSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_2:System.Void()\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_2:System.Void()\",\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-2\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_2()\"},\"nativeSymbol\":\"ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_2\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":2,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Int32\",\"ilOffset\":2,\"resultType\":\"System.Int32[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Int32\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":10,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stelem\",\"opCode\":45,\"operand\":\"System.Int32\",\"ilOffset\":11,\"resultType\":\"System.Void\",\"runtimeServiceKind\":13,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":12,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":20,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stelem\",\"opCode\":45,\"operand\":\"System.Int32\",\"ilOffset\":16,\"resultType\":\"System.Void\",\"runtimeServiceKind\":13,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":17,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":5,\"ilOffset\":19,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":20,\"resultType\":\"System.Int32\",\"callee\":\"System.Linq/Enumerable::ElementAtOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Int32)\",\"reference\":{\"assemblyName\":\"System.Linq\",\"subjectKind\":\"method\",\"subjectId\":\"System.Linq/Enumerable::ElementAtOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Linq\",\"subjectId\":\"System.Linq/Enumerable::ElementAtOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Int32)\",\"openDefinitionSubjectId\":\"System.Linq/Enumerable::ElementAtOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Int32)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::ElementAtOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Int32);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::ElementAtOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Int32);type=[];method=[System.Int32]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::ElementAtOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Int32)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::ElementAtOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Int32);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::ElementAtOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Int32);type=[];method=[System.Int32]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Linq/Enumerable::ElementAtOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Int32)\",\"definitionSubjectId\":\"System.Linq/Enumerable::ElementAtOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Int32)\",\"displaySubjectId\":\"System.Linq/Enumerable::ElementAtOrDefault\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Int32)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::ElementAtOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Int32)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":25,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":26,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ShuffleIndexSubjects\"}";
static const char kMethodJson_3[] = "{\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-3\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_3:System.Void()\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_3()\",\"identity\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\",\"definitionSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_3:System.Void()\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_3:System.Void()\",\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-3\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_3()\"},\"nativeSymbol\":\"ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_3\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":2,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Int32\",\"ilOffset\":2,\"resultType\":\"System.Int32[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Int32\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":10,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stelem\",\"opCode\":45,\"operand\":\"System.Int32\",\"ilOffset\":11,\"resultType\":\"System.Void\",\"runtimeServiceKind\":13,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":12,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":20,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stelem\",\"opCode\":45,\"operand\":\"System.Int32\",\"ilOffset\":16,\"resultType\":\"System.Void\",\"runtimeServiceKind\":13,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":17,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":5,\"ilOffset\":19,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":20,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":21,\"resultType\":\"System.Private.CoreLib/System.Index\",\"callee\":\"System.Private.CoreLib/System.Index::.ctor:System.Void(System.Int32,System.Boolean)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Index::.ctor:System.Void(System.Int32,System.Boolean)\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Index\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"callee\":\"System.Linq/Enumerable::ElementAtOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Index)\",\"reference\":{\"assemblyName\":\"System.Linq\",\"subjectKind\":\"method\",\"subjectId\":\"System.Linq/Enumerable::ElementAtOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Index)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Linq\",\"subjectId\":\"System.Linq/Enumerable::ElementAtOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Index)\",\"openDefinitionSubjectId\":\"System.Linq/Enumerable::ElementAtOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Index)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::ElementAtOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Index);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::ElementAtOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Index);type=[];method=[System.Int32]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::ElementAtOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Index)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::ElementAtOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Index);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::ElementAtOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Index);type=[];method=[System.Int32]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Linq/Enumerable::ElementAtOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Index)\",\"definitionSubjectId\":\"System.Linq/Enumerable::ElementAtOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Index)\",\"displaySubjectId\":\"System.Linq/Enumerable::ElementAtOrDefault\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Index)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::ElementAtOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Index)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":31,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":32,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ShuffleIndexSubjects\"}";
static const char kMethodJson_4[] = "{\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-4\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_4:System.Void()\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_4()\",\"identity\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\",\"definitionSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_4:System.Void()\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_4:System.Void()\",\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-4\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_4()\"},\"nativeSymbol\":\"ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_4\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":3,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Int32\",\"ilOffset\":2,\"resultType\":\"System.Int32[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Int32\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::97CA1592048640A5368B4EC7C6934311567E09D50E7639918EC82C3D2A187CDA\",\"ilOffset\":8,\"resultType\":\"System.RuntimeFieldHandle\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::97CA1592048640A5368B4EC7C6934311567E09D50E7639918EC82C3D2A187CDA\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::97CA1592048640A5368B4EC7C6934311567E09D50E7639918EC82C3D2A187CDA\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"\\u003CPrivateImplementationDetails\\u003E\\u002B__StaticArrayInitTypeSize=12\"},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":13,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":19,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":20,\"resultType\":\"System.Int32\",\"callee\":\"System.Linq/Enumerable::First\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E)\",\"reference\":{\"assemblyName\":\"System.Linq\",\"subjectKind\":\"method\",\"subjectId\":\"System.Linq/Enumerable::First\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Linq\",\"subjectId\":\"System.Linq/Enumerable::First\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E)\",\"openDefinitionSubjectId\":\"System.Linq/Enumerable::First:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::First:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::First:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E);type=[];method=[System.Int32]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::First:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::First:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::First:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E);type=[];method=[System.Int32]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Linq/Enumerable::First\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E)\",\"definitionSubjectId\":\"System.Linq/Enumerable::First:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E)\",\"displaySubjectId\":\"System.Linq/Enumerable::First\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::First:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":25,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":26,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ShuffleIndexSubjects\"}";
static const char kMethodJson_5[] = "{\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-5\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_5:System.Void()\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_5()\",\"identity\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\",\"definitionSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_5:System.Void()\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_5:System.Void()\",\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-5\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_5()\"},\"nativeSymbol\":\"ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_5\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":5,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Int32\",\"ilOffset\":2,\"resultType\":\"System.Int32[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Int32\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::4F6ADDC9659D6FB90FE94B6688A79F2A1FA8D36EC43F8F3E1D9B6528C448A384\",\"ilOffset\":8,\"resultType\":\"System.RuntimeFieldHandle\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::4F6ADDC9659D6FB90FE94B6688A79F2A1FA8D36EC43F8F3E1D9B6528C448A384\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::4F6ADDC9659D6FB90FE94B6688A79F2A1FA8D36EC43F8F3E1D9B6528C448A384\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"\\u003CPrivateImplementationDetails\\u003E\\u002B__StaticArrayInitTypeSize=20\"},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":13,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":19,\"isPreserveSig\":false},{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__6_0\",\"ilOffset\":20,\"resultType\":\"System.Func\\u003CSystem.Int32,System.Boolean\\u003E\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__6_0\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__6_0\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Func\\u003CSystem.Int32,System.Boolean\\u003E\"},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":25,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":51,\"ilOffset\":26,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":28,\"isPreserveSig\":false},{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9\",\"ilOffset\":29,\"resultType\":\"ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\"},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"ldftn\",\"opCode\":73,\"operand\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_5\\u003Eb__6_0:System.Boolean(System.Int32)\",\"ilOffset\":34,\"resultType\":\"System.IntPtr\",\"callee\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_5\\u003Eb__6_0:System.Boolean(System.Int32)\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_5\\u003Eb__6_0:System.Boolean(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_5\\u003Eb__6_0:System.Boolean(System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":40,\"resultType\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Boolean\\u003E\",\"callee\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Boolean\\u003E::.ctor:System.Void(System.Object,System.IntPtr)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Boolean\\u003E::.ctor:System.Void(System.Object,System.IntPtr)\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Boolean\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":45,\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__6_0\",\"ilOffset\":46,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__6_0\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__6_0\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Func\\u003CSystem.Int32,System.Boolean\\u003E\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":51,\"resultType\":\"System.Int32\",\"callee\":\"System.Linq/Enumerable::First\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Boolean\\u003E)\",\"reference\":{\"assemblyName\":\"System.Linq\",\"subjectKind\":\"method\",\"subjectId\":\"System.Linq/Enumerable::First\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Boolean\\u003E)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Linq\",\"subjectId\":\"System.Linq/Enumerable::First\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Boolean\\u003E)\",\"openDefinitionSubjectId\":\"System.Linq/Enumerable::First:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::First:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::First:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E);type=[];method=[System.Int32]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::First:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::First:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::First:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E);type=[];method=[System.Int32]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Linq/Enumerable::First\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Boolean\\u003E)\",\"definitionSubjectId\":\"System.Linq/Enumerable::First:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E)\",\"displaySubjectId\":\"System.Linq/Enumerable::First\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Boolean\\u003E)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::First:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":56,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":57,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ShuffleIndexSubjects\"}";
static const char kMethodJson_6[] = "{\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects-c.custom-entry-subject-5-b-6-0\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_5\\u003Eb__6_0:System.Boolean(System.Int32)\",\"signature\":\"System.Boolean ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_5\\u003Eb__6_0(System.Int32)\",\"identity\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"definitionSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_5\\u003Eb__6_0:System.Boolean(System.Int32)\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_5\\u003Eb__6_0:System.Boolean(System.Int32)\",\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects-c.custom-entry-subject-5-b-6-0\",\"signature\":\"System.Boolean ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_5\\u003Eb__6_0(System.Int32)\"},\"nativeSymbol\":\"ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_5_b__6_0_System_Int32\",\"isStatic\":false,\"returnType\":\"System.Boolean\",\"returnAbi\":{\"carrierKindCode\":2,\"typeShape\":2},\"parameterCount\":1,\"parameterAbis\":[{\"carrierKindCode\":1,\"typeShape\":2}],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":0,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":3,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"cgt\",\"opCode\":33,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":4,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ShuffleIndexSubjects\"}";
static const char kMethodJson_7[] = "{\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-6\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_6:System.Void()\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_6()\",\"identity\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\",\"definitionSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_6:System.Void()\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_6:System.Void()\",\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-6\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_6()\"},\"nativeSymbol\":\"ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_6\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":3,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Int32\",\"ilOffset\":2,\"resultType\":\"System.Int32[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Int32\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::97CA1592048640A5368B4EC7C6934311567E09D50E7639918EC82C3D2A187CDA\",\"ilOffset\":8,\"resultType\":\"System.RuntimeFieldHandle\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::97CA1592048640A5368B4EC7C6934311567E09D50E7639918EC82C3D2A187CDA\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::97CA1592048640A5368B4EC7C6934311567E09D50E7639918EC82C3D2A187CDA\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"\\u003CPrivateImplementationDetails\\u003E\\u002B__StaticArrayInitTypeSize=12\"},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":13,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":19,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":20,\"resultType\":\"System.Int32\",\"callee\":\"System.Linq/Enumerable::FirstOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E)\",\"reference\":{\"assemblyName\":\"System.Linq\",\"subjectKind\":\"method\",\"subjectId\":\"System.Linq/Enumerable::FirstOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Linq\",\"subjectId\":\"System.Linq/Enumerable::FirstOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E)\",\"openDefinitionSubjectId\":\"System.Linq/Enumerable::FirstOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::FirstOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::FirstOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E);type=[];method=[System.Int32]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::FirstOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::FirstOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::FirstOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E);type=[];method=[System.Int32]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Linq/Enumerable::FirstOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E)\",\"definitionSubjectId\":\"System.Linq/Enumerable::FirstOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E)\",\"displaySubjectId\":\"System.Linq/Enumerable::FirstOrDefault\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::FirstOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":25,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":26,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ShuffleIndexSubjects\"}";
static const char kMethodJson_8[] = "{\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-7\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_7:System.Void()\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_7()\",\"identity\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\",\"definitionSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_7:System.Void()\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_7:System.Void()\",\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-7\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_7()\"},\"nativeSymbol\":\"ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_7\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Int32\",\"ilOffset\":2,\"resultType\":\"System.Int32[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Int32\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":7,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":11,\"resultType\":\"System.Int32\",\"callee\":\"System.Linq/Enumerable::FirstOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Int32)\",\"reference\":{\"assemblyName\":\"System.Linq\",\"subjectKind\":\"method\",\"subjectId\":\"System.Linq/Enumerable::FirstOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Linq\",\"subjectId\":\"System.Linq/Enumerable::FirstOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Int32)\",\"openDefinitionSubjectId\":\"System.Linq/Enumerable::FirstOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::FirstOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::FirstOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0);type=[];method=[System.Int32]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::FirstOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::FirstOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::FirstOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0);type=[];method=[System.Int32]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Linq/Enumerable::FirstOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Int32)\",\"definitionSubjectId\":\"System.Linq/Enumerable::FirstOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0)\",\"displaySubjectId\":\"System.Linq/Enumerable::FirstOrDefault\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Int32)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::FirstOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":16,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":17,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ShuffleIndexSubjects\"}";
static const char kMethodJson_9[] = "{\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-8\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_8:System.Void()\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_8()\",\"identity\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\",\"definitionSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_8:System.Void()\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_8:System.Void()\",\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-8\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_8()\"},\"nativeSymbol\":\"ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_8\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":5,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Int32\",\"ilOffset\":2,\"resultType\":\"System.Int32[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Int32\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::4F6ADDC9659D6FB90FE94B6688A79F2A1FA8D36EC43F8F3E1D9B6528C448A384\",\"ilOffset\":8,\"resultType\":\"System.RuntimeFieldHandle\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::4F6ADDC9659D6FB90FE94B6688A79F2A1FA8D36EC43F8F3E1D9B6528C448A384\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::4F6ADDC9659D6FB90FE94B6688A79F2A1FA8D36EC43F8F3E1D9B6528C448A384\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"\\u003CPrivateImplementationDetails\\u003E\\u002B__StaticArrayInitTypeSize=20\"},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":13,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":19,\"isPreserveSig\":false},{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__9_0\",\"ilOffset\":20,\"resultType\":\"System.Func\\u003CSystem.Int32,System.Boolean\\u003E\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__9_0\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__9_0\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Func\\u003CSystem.Int32,System.Boolean\\u003E\"},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":25,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":51,\"ilOffset\":26,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":28,\"isPreserveSig\":false},{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9\",\"ilOffset\":29,\"resultType\":\"ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\"},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"ldftn\",\"opCode\":73,\"operand\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_8\\u003Eb__9_0:System.Boolean(System.Int32)\",\"ilOffset\":34,\"resultType\":\"System.IntPtr\",\"callee\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_8\\u003Eb__9_0:System.Boolean(System.Int32)\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_8\\u003Eb__9_0:System.Boolean(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_8\\u003Eb__9_0:System.Boolean(System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":40,\"resultType\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Boolean\\u003E\",\"callee\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Boolean\\u003E::.ctor:System.Void(System.Object,System.IntPtr)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Boolean\\u003E::.ctor:System.Void(System.Object,System.IntPtr)\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Boolean\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":45,\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__9_0\",\"ilOffset\":46,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__9_0\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__9_0\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Func\\u003CSystem.Int32,System.Boolean\\u003E\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":51,\"resultType\":\"System.Int32\",\"callee\":\"System.Linq/Enumerable::FirstOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Boolean\\u003E)\",\"reference\":{\"assemblyName\":\"System.Linq\",\"subjectKind\":\"method\",\"subjectId\":\"System.Linq/Enumerable::FirstOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Boolean\\u003E)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Linq\",\"subjectId\":\"System.Linq/Enumerable::FirstOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Boolean\\u003E)\",\"openDefinitionSubjectId\":\"System.Linq/Enumerable::FirstOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::FirstOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::FirstOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E);type=[];method=[System.Int32]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::FirstOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::FirstOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::FirstOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E);type=[];method=[System.Int32]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Linq/Enumerable::FirstOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Boolean\\u003E)\",\"definitionSubjectId\":\"System.Linq/Enumerable::FirstOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E)\",\"displaySubjectId\":\"System.Linq/Enumerable::FirstOrDefault\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Boolean\\u003E)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::FirstOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":56,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":57,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ShuffleIndexSubjects\"}";
static const char kMethodJson_10[] = "{\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects-c.custom-entry-subject-8-b-9-0\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_8\\u003Eb__9_0:System.Boolean(System.Int32)\",\"signature\":\"System.Boolean ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_8\\u003Eb__9_0(System.Int32)\",\"identity\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"definitionSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_8\\u003Eb__9_0:System.Boolean(System.Int32)\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_8\\u003Eb__9_0:System.Boolean(System.Int32)\",\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects-c.custom-entry-subject-8-b-9-0\",\"signature\":\"System.Boolean ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_8\\u003Eb__9_0(System.Int32)\"},\"nativeSymbol\":\"ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_8_b__9_0_System_Int32\",\"isStatic\":false,\"returnType\":\"System.Boolean\",\"returnAbi\":{\"carrierKindCode\":2,\"typeShape\":2},\"parameterCount\":1,\"parameterAbis\":[{\"carrierKindCode\":1,\"typeShape\":2}],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":0,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":10,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"cgt\",\"opCode\":33,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":5,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ShuffleIndexSubjects\"}";
static const char kMethodJson_11[] = "{\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-9\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_9:System.Void()\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_9()\",\"identity\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\",\"definitionSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_9:System.Void()\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_9:System.Void()\",\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-9\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_9()\"},\"nativeSymbol\":\"ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_9\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":5,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Int32\",\"ilOffset\":2,\"resultType\":\"System.Int32[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Int32\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::4F6ADDC9659D6FB90FE94B6688A79F2A1FA8D36EC43F8F3E1D9B6528C448A384\",\"ilOffset\":8,\"resultType\":\"System.RuntimeFieldHandle\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::4F6ADDC9659D6FB90FE94B6688A79F2A1FA8D36EC43F8F3E1D9B6528C448A384\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::4F6ADDC9659D6FB90FE94B6688A79F2A1FA8D36EC43F8F3E1D9B6528C448A384\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"\\u003CPrivateImplementationDetails\\u003E\\u002B__StaticArrayInitTypeSize=20\"},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":13,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":19,\"isPreserveSig\":false},{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__10_0\",\"ilOffset\":20,\"resultType\":\"System.Func\\u003CSystem.Int32,System.Boolean\\u003E\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__10_0\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__10_0\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Func\\u003CSystem.Int32,System.Boolean\\u003E\"},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":25,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":51,\"ilOffset\":26,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":28,\"isPreserveSig\":false},{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9\",\"ilOffset\":29,\"resultType\":\"ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\"},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"ldftn\",\"opCode\":73,\"operand\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_9\\u003Eb__10_0:System.Boolean(System.Int32)\",\"ilOffset\":34,\"resultType\":\"System.IntPtr\",\"callee\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_9\\u003Eb__10_0:System.Boolean(System.Int32)\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_9\\u003Eb__10_0:System.Boolean(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_9\\u003Eb__10_0:System.Boolean(System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":40,\"resultType\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Boolean\\u003E\",\"callee\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Boolean\\u003E::.ctor:System.Void(System.Object,System.IntPtr)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Boolean\\u003E::.ctor:System.Void(System.Object,System.IntPtr)\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Boolean\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":45,\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__10_0\",\"ilOffset\":46,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__10_0\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__10_0\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Func\\u003CSystem.Int32,System.Boolean\\u003E\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":99,\"ilOffset\":51,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":53,\"resultType\":\"System.Int32\",\"callee\":\"System.Linq/Enumerable::FirstOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Boolean\\u003E,System.Int32)\",\"reference\":{\"assemblyName\":\"System.Linq\",\"subjectKind\":\"method\",\"subjectId\":\"System.Linq/Enumerable::FirstOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Boolean\\u003E,System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Linq\",\"subjectId\":\"System.Linq/Enumerable::FirstOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Boolean\\u003E,System.Int32)\",\"openDefinitionSubjectId\":\"System.Linq/Enumerable::FirstOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E,!!0)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::FirstOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E,!!0);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::FirstOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E,!!0);type=[];method=[System.Int32]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::FirstOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E,!!0)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::FirstOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E,!!0);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::FirstOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E,!!0);type=[];method=[System.Int32]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Linq/Enumerable::FirstOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Boolean\\u003E,System.Int32)\",\"definitionSubjectId\":\"System.Linq/Enumerable::FirstOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E,!!0)\",\"displaySubjectId\":\"System.Linq/Enumerable::FirstOrDefault\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Boolean\\u003E,System.Int32)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::FirstOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E,!!0)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":58,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":59,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ShuffleIndexSubjects\"}";
static const char kMethodJson_12[] = "{\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects-c.custom-entry-subject-9-b-10-0\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_9\\u003Eb__10_0:System.Boolean(System.Int32)\",\"signature\":\"System.Boolean ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_9\\u003Eb__10_0(System.Int32)\",\"identity\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"definitionSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_9\\u003Eb__10_0:System.Boolean(System.Int32)\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_9\\u003Eb__10_0:System.Boolean(System.Int32)\",\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects-c.custom-entry-subject-9-b-10-0\",\"signature\":\"System.Boolean ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_9\\u003Eb__10_0(System.Int32)\"},\"nativeSymbol\":\"ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_9_b__10_0_System_Int32\",\"isStatic\":false,\"returnType\":\"System.Boolean\",\"returnAbi\":{\"carrierKindCode\":2,\"typeShape\":2},\"parameterCount\":1,\"parameterAbis\":[{\"carrierKindCode\":1,\"typeShape\":2}],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":0,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":10,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"cgt\",\"opCode\":33,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":5,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ShuffleIndexSubjects\"}";
static const char kMethodJson_13[] = 
    "{\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-10\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_10:System.Void()\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_10()\",\"identity\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\",\"definitionSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_10:System.Void()\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_10:System.Void()\",\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-10\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_10()\"},\"nativeSymbol\":\"ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_10\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":3,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":3,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Int32\",\"ilOffset\":2,\"resultType\":\"System.Int32[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Int32\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::97CA1592048640A5368B4EC7C6934311567E09D50E7639918EC82C3D2A187CDA\",\"ilOffset\":8,\"resultType\":\"System.RuntimeFieldHandle\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::97CA1592048640A5368B4EC7C6934311567E09D50E7639918EC82C3D2A187CDA\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::97CA1592048640A5368B4EC7C6934311567E09D50E7639918EC82C3D2A187CDA\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"\\u003CPrivateImplementationDetails\\u003E\\u002B__StaticArrayInitTypeSize=12\"},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":13,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":19,\"isPreserveSig\":false},{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__11_0\",\"ilOffset\":20,\"resultType\":\"System.Func\\u003CSystem.Int32,System.Int32,System.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E\\u003E\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__11_0\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__11_0\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Func\\u003CSystem.Int32,System.Int32,System.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E\\u003E\"},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":25,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":51,\"ilOffset\":26,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":28,\"isPreserveSig\":false},{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9\",\"ilOffset\":29,\"resultType\":\"ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\"},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"ldftn\",\"opCode\":73,\"operand\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_10\\u003Eb__11_0:System.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E(System.Int32,System.Int32)\",\"ilOffset\":34,\"resultType\":\"System.IntPtr\",\"callee\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_10\\u003Eb__11_0:System.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E(System.Int32,System.Int32)\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_10\\u003Eb__11_0:System.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E(System.Int32,System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_10\\u003Eb__11_0:System.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E(System.Int32,System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":40,\"resultType\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Int32,System.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E\\u003E\",\"callee\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Int32,System.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E\\u003E::.ctor:System.Void(System.Object,System.IntPtr)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Int32,System.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E\\u003E::.ctor:System.Void(System.Object,System.IntPtr)\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Int32,System.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":45,\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__11_0\",\"ilOffset\":46,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__11_0\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__11_0\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Func\\u003CSystem.Int32,System.Int32,System.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E\\u003E\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":51,\"resultType\":\"System.Collections.Generic.IEnumerable\\u003CSystem.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E\\u003E\",\"callee\":\"System.Linq/Enumerable::Select\\u003CSystem.Int32,System.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E\\u003E:System.Collections.Generic.IEnumerable\\u003CSystem.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Int32,System.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E\\u003E)\",\"reference\":{\"assemblyName\":\"System.Linq\",\"subjectKind\":\"method\",\"subjectId\":\"System.Linq/Enumerable::Select\\u003CSystem.Int32,System.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E\\u003E:System.Collections.Generic.IEnumerable\\u003CSystem.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Int32,System.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E\\u003E)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Linq\",\"subjectId\":\"System.Linq/Enumerable::Select\\u003CSystem.Int32,System.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E\\u003E:System.Collections.Generic.IEnumerable\\u003CSystem.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Int32,System.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E\\u003E)\",\"openDefinitionSubjectId\":\"System.Linq/Enumerable::Select:System.Collections.Generic.IEnumerable\\u003C!!1\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Int32,!!1\\u003E)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::Select:System.Collections.Generic.IEnumerable\\u003C!!1\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Int32,!!1\\u003E);type=[];method=[System.Int32,System.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::Select:System.Collections.Generic.IEnumerable\\u003C!!1\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Int32,!!1\\u003E);type=[];method=[System.Int32,System.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::Select:System.Collections.Generic.IEnumerable\\u003C!!1\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Int32,!!1\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\",\"System.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::Select:System.Collections.Generic.IEnumerable\\u003C!!1\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Int32,!!1\\u003E);type=[];method=[System.Int32,System.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::Select:System.Collections.Generic.IEnumerable\\u003C!!1\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Int32,!!1\\u003E);type=[];method=[System.Int32,System.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Linq/Enumerable::Select\\u003CSystem.Int32,System.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E\\u003E:System.Collections.Generic.IEnumerable\\u003CSystem.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Int32,System.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E\\u003E)\",\"definitionSubjectId\":\"System.Linq/Enumerable::Select:System.Collections.Generic.IEnumerable\\u003C!!1\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Int32,!!1\\u003E)\",\"displaySubjectId\":\"System.Linq/Enumerable::Select\\u003CSystem.Int32,System.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Int32,System.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E\\u003E)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::Select:System.Collections.Generic.IEnumerable\\u003C!!1\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Int32,!!1\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\",\"System.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":56,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":57,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":58,\"resultType\":\"System.Collections.Generic.IEnumerator\\u003CSystem.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E\\u003E\",\"callee\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E\\u003E()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.ValueTuple\\u003CSystem.Int32,System.I"
    "nt32\\u003E\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E\\u003E()\",\"openDefinitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"typeArguments\":[\"System.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E\\u003E()\",\"definitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"displaySubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E\\u003E::GetEnumerator()\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"typeArguments\":[\"System.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":63,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":64,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":65,\"resultType\":\"System.Boolean\",\"callee\":\"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":70,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":71,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ShuffleIndexSubjects\"}"
    ;
static const char kMethodJson_14[] = "{\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects-c.custom-entry-subject-10-b-11-0\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_10\\u003Eb__11_0:System.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E(System.Int32,System.Int32)\",\"signature\":\"System.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_10\\u003Eb__11_0(System.Int32,System.Int32)\",\"identity\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"definitionSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_10\\u003Eb__11_0:System.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E(System.Int32,System.Int32)\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_10\\u003Eb__11_0:System.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E(System.Int32,System.Int32)\",\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects-c.custom-entry-subject-10-b-11-0\",\"signature\":\"System.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_10\\u003Eb__11_0(System.Int32,System.Int32)\"},\"nativeSymbol\":\"ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_10_b__11_0_System_Int32_System_Int32\",\"isStatic\":false,\"returnType\":\"System.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E\",\"returnAbi\":{\"carrierKindCode\":2,\"typeShape\":1},\"parameterCount\":2,\"parameterAbis\":[{\"carrierKindCode\":1,\"typeShape\":2},{\"carrierKindCode\":1,\"typeShape\":2}],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":2,\"ilOffset\":0,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":2,\"resultType\":\"System.Private.CoreLib/System.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E\",\"callee\":\"System.Private.CoreLib/System.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E::.ctor:System.Void(System.Int32,System.Int32)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E::.ctor:System.Void(System.Int32,System.Int32)\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.ValueTuple\\u003CSystem.Int32,System.Int32\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":7,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ShuffleIndexSubjects\"}";
static const char kMethodJson_15[] = "{\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-11\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_11:System.Void()\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_11()\",\"identity\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\",\"definitionSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_11:System.Void()\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_11:System.Void()\",\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-11\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_11()\"},\"nativeSymbol\":\"ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_11\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":3,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Int32\",\"ilOffset\":2,\"resultType\":\"System.Int32[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Int32\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::97CA1592048640A5368B4EC7C6934311567E09D50E7639918EC82C3D2A187CDA\",\"ilOffset\":8,\"resultType\":\"System.RuntimeFieldHandle\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::97CA1592048640A5368B4EC7C6934311567E09D50E7639918EC82C3D2A187CDA\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::97CA1592048640A5368B4EC7C6934311567E09D50E7639918EC82C3D2A187CDA\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"\\u003CPrivateImplementationDetails\\u003E\\u002B__StaticArrayInitTypeSize=12\"},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":13,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":19,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":20,\"resultType\":\"System.Int32\",\"callee\":\"System.Linq/Enumerable::Last\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E)\",\"reference\":{\"assemblyName\":\"System.Linq\",\"subjectKind\":\"method\",\"subjectId\":\"System.Linq/Enumerable::Last\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Linq\",\"subjectId\":\"System.Linq/Enumerable::Last\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E)\",\"openDefinitionSubjectId\":\"System.Linq/Enumerable::Last:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::Last:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::Last:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E);type=[];method=[System.Int32]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::Last:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::Last:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::Last:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E);type=[];method=[System.Int32]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Linq/Enumerable::Last\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E)\",\"definitionSubjectId\":\"System.Linq/Enumerable::Last:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E)\",\"displaySubjectId\":\"System.Linq/Enumerable::Last\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::Last:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":25,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":26,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ShuffleIndexSubjects\"}";
static const char kMethodJson_16[] = "{\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-12\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_12:System.Void()\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_12()\",\"identity\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\",\"definitionSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_12:System.Void()\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_12:System.Void()\",\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-12\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_12()\"},\"nativeSymbol\":\"ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_12\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":5,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Int32\",\"ilOffset\":2,\"resultType\":\"System.Int32[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Int32\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::4F6ADDC9659D6FB90FE94B6688A79F2A1FA8D36EC43F8F3E1D9B6528C448A384\",\"ilOffset\":8,\"resultType\":\"System.RuntimeFieldHandle\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::4F6ADDC9659D6FB90FE94B6688A79F2A1FA8D36EC43F8F3E1D9B6528C448A384\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::4F6ADDC9659D6FB90FE94B6688A79F2A1FA8D36EC43F8F3E1D9B6528C448A384\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"\\u003CPrivateImplementationDetails\\u003E\\u002B__StaticArrayInitTypeSize=20\"},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":13,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":19,\"isPreserveSig\":false},{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__13_0\",\"ilOffset\":20,\"resultType\":\"System.Func\\u003CSystem.Int32,System.Boolean\\u003E\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__13_0\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__13_0\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Func\\u003CSystem.Int32,System.Boolean\\u003E\"},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":25,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":51,\"ilOffset\":26,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":28,\"isPreserveSig\":false},{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9\",\"ilOffset\":29,\"resultType\":\"ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\"},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"ldftn\",\"opCode\":73,\"operand\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_12\\u003Eb__13_0:System.Boolean(System.Int32)\",\"ilOffset\":34,\"resultType\":\"System.IntPtr\",\"callee\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_12\\u003Eb__13_0:System.Boolean(System.Int32)\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_12\\u003Eb__13_0:System.Boolean(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_12\\u003Eb__13_0:System.Boolean(System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":40,\"resultType\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Boolean\\u003E\",\"callee\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Boolean\\u003E::.ctor:System.Void(System.Object,System.IntPtr)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Boolean\\u003E::.ctor:System.Void(System.Object,System.IntPtr)\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Boolean\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":45,\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__13_0\",\"ilOffset\":46,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__13_0\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__13_0\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Func\\u003CSystem.Int32,System.Boolean\\u003E\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":51,\"resultType\":\"System.Int32\",\"callee\":\"System.Linq/Enumerable::Last\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Boolean\\u003E)\",\"reference\":{\"assemblyName\":\"System.Linq\",\"subjectKind\":\"method\",\"subjectId\":\"System.Linq/Enumerable::Last\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Boolean\\u003E)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Linq\",\"subjectId\":\"System.Linq/Enumerable::Last\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Boolean\\u003E)\",\"openDefinitionSubjectId\":\"System.Linq/Enumerable::Last:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::Last:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::Last:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E);type=[];method=[System.Int32]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::Last:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::Last:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::Last:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E);type=[];method=[System.Int32]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Linq/Enumerable::Last\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Boolean\\u003E)\",\"definitionSubjectId\":\"System.Linq/Enumerable::Last:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E)\",\"displaySubjectId\":\"System.Linq/Enumerable::Last\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Boolean\\u003E)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::Last:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":56,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":57,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ShuffleIndexSubjects\"}";
static const char kMethodJson_17[] = "{\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects-c.custom-entry-subject-12-b-13-0\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_12\\u003Eb__13_0:System.Boolean(System.Int32)\",\"signature\":\"System.Boolean ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_12\\u003Eb__13_0(System.Int32)\",\"identity\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"definitionSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_12\\u003Eb__13_0:System.Boolean(System.Int32)\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_12\\u003Eb__13_0:System.Boolean(System.Int32)\",\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects-c.custom-entry-subject-12-b-13-0\",\"signature\":\"System.Boolean ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_12\\u003Eb__13_0(System.Int32)\"},\"nativeSymbol\":\"ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_12_b__13_0_System_Int32\",\"isStatic\":false,\"returnType\":\"System.Boolean\",\"returnAbi\":{\"carrierKindCode\":2,\"typeShape\":2},\"parameterCount\":1,\"parameterAbis\":[{\"carrierKindCode\":1,\"typeShape\":2}],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":0,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":4,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"clt\",\"opCode\":32,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":4,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ShuffleIndexSubjects\"}";
static const char kMethodJson_18[] = "{\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-13\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_13:System.Void()\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_13()\",\"identity\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\",\"definitionSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_13:System.Void()\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_13:System.Void()\",\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-13\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_13()\"},\"nativeSymbol\":\"ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_13\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":3,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Int32\",\"ilOffset\":2,\"resultType\":\"System.Int32[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Int32\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::97CA1592048640A5368B4EC7C6934311567E09D50E7639918EC82C3D2A187CDA\",\"ilOffset\":8,\"resultType\":\"System.RuntimeFieldHandle\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::97CA1592048640A5368B4EC7C6934311567E09D50E7639918EC82C3D2A187CDA\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::97CA1592048640A5368B4EC7C6934311567E09D50E7639918EC82C3D2A187CDA\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"\\u003CPrivateImplementationDetails\\u003E\\u002B__StaticArrayInitTypeSize=12\"},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":13,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":19,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":20,\"resultType\":\"System.Int32\",\"callee\":\"System.Linq/Enumerable::LastOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E)\",\"reference\":{\"assemblyName\":\"System.Linq\",\"subjectKind\":\"method\",\"subjectId\":\"System.Linq/Enumerable::LastOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Linq\",\"subjectId\":\"System.Linq/Enumerable::LastOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E)\",\"openDefinitionSubjectId\":\"System.Linq/Enumerable::LastOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::LastOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::LastOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E);type=[];method=[System.Int32]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::LastOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::LastOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::LastOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E);type=[];method=[System.Int32]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Linq/Enumerable::LastOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E)\",\"definitionSubjectId\":\"System.Linq/Enumerable::LastOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E)\",\"displaySubjectId\":\"System.Linq/Enumerable::LastOrDefault\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::LastOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":25,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":26,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ShuffleIndexSubjects\"}";
static const char kMethodJson_19[] = "{\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-14\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_14:System.Void()\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_14()\",\"identity\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\",\"definitionSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_14:System.Void()\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_14:System.Void()\",\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-14\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_14()\"},\"nativeSymbol\":\"ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_14\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Int32\",\"ilOffset\":2,\"resultType\":\"System.Int32[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Int32\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":7,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":11,\"resultType\":\"System.Int32\",\"callee\":\"System.Linq/Enumerable::LastOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Int32)\",\"reference\":{\"assemblyName\":\"System.Linq\",\"subjectKind\":\"method\",\"subjectId\":\"System.Linq/Enumerable::LastOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Linq\",\"subjectId\":\"System.Linq/Enumerable::LastOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Int32)\",\"openDefinitionSubjectId\":\"System.Linq/Enumerable::LastOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::LastOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::LastOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0);type=[];method=[System.Int32]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::LastOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::LastOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::LastOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0);type=[];method=[System.Int32]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Linq/Enumerable::LastOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Int32)\",\"definitionSubjectId\":\"System.Linq/Enumerable::LastOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0)\",\"displaySubjectId\":\"System.Linq/Enumerable::LastOrDefault\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Int32)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::LastOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":16,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":17,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ShuffleIndexSubjects\"}";
static const char kMethodJson_20[] = "{\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-15\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_15:System.Void()\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_15()\",\"identity\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\",\"definitionSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_15:System.Void()\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_15:System.Void()\",\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-15\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_15()\"},\"nativeSymbol\":\"ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_15\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":5,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Int32\",\"ilOffset\":2,\"resultType\":\"System.Int32[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Int32\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::4F6ADDC9659D6FB90FE94B6688A79F2A1FA8D36EC43F8F3E1D9B6528C448A384\",\"ilOffset\":8,\"resultType\":\"System.RuntimeFieldHandle\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::4F6ADDC9659D6FB90FE94B6688A79F2A1FA8D36EC43F8F3E1D9B6528C448A384\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::4F6ADDC9659D6FB90FE94B6688A79F2A1FA8D36EC43F8F3E1D9B6528C448A384\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"\\u003CPrivateImplementationDetails\\u003E\\u002B__StaticArrayInitTypeSize=20\"},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":13,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":19,\"isPreserveSig\":false},{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__16_0\",\"ilOffset\":20,\"resultType\":\"System.Func\\u003CSystem.Int32,System.Boolean\\u003E\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__16_0\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__16_0\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Func\\u003CSystem.Int32,System.Boolean\\u003E\"},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":25,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":51,\"ilOffset\":26,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":28,\"isPreserveSig\":false},{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9\",\"ilOffset\":29,\"resultType\":\"ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\"},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"ldftn\",\"opCode\":73,\"operand\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_15\\u003Eb__16_0:System.Boolean(System.Int32)\",\"ilOffset\":34,\"resultType\":\"System.IntPtr\",\"callee\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_15\\u003Eb__16_0:System.Boolean(System.Int32)\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_15\\u003Eb__16_0:System.Boolean(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_15\\u003Eb__16_0:System.Boolean(System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":40,\"resultType\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Boolean\\u003E\",\"callee\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Boolean\\u003E::.ctor:System.Void(System.Object,System.IntPtr)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Boolean\\u003E::.ctor:System.Void(System.Object,System.IntPtr)\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Boolean\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":45,\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__16_0\",\"ilOffset\":46,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__16_0\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__16_0\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Func\\u003CSystem.Int32,System.Boolean\\u003E\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":51,\"resultType\":\"System.Int32\",\"callee\":\"System.Linq/Enumerable::LastOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Boolean\\u003E)\",\"reference\":{\"assemblyName\":\"System.Linq\",\"subjectKind\":\"method\",\"subjectId\":\"System.Linq/Enumerable::LastOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Boolean\\u003E)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Linq\",\"subjectId\":\"System.Linq/Enumerable::LastOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Boolean\\u003E)\",\"openDefinitionSubjectId\":\"System.Linq/Enumerable::LastOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::LastOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::LastOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E);type=[];method=[System.Int32]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::LastOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::LastOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::LastOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E);type=[];method=[System.Int32]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Linq/Enumerable::LastOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Boolean\\u003E)\",\"definitionSubjectId\":\"System.Linq/Enumerable::LastOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E)\",\"displaySubjectId\":\"System.Linq/Enumerable::LastOrDefault\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Boolean\\u003E)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::LastOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":56,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":57,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ShuffleIndexSubjects\"}";
static const char kMethodJson_21[] = "{\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects-c.custom-entry-subject-15-b-16-0\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_15\\u003Eb__16_0:System.Boolean(System.Int32)\",\"signature\":\"System.Boolean ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_15\\u003Eb__16_0(System.Int32)\",\"identity\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"definitionSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_15\\u003Eb__16_0:System.Boolean(System.Int32)\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_15\\u003Eb__16_0:System.Boolean(System.Int32)\",\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects-c.custom-entry-subject-15-b-16-0\",\"signature\":\"System.Boolean ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_15\\u003Eb__16_0(System.Int32)\"},\"nativeSymbol\":\"ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_15_b__16_0_System_Int32\",\"isStatic\":false,\"returnType\":\"System.Boolean\",\"returnAbi\":{\"carrierKindCode\":2,\"typeShape\":2},\"parameterCount\":1,\"parameterAbis\":[{\"carrierKindCode\":1,\"typeShape\":2}],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":0,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":10,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"cgt\",\"opCode\":33,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":5,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ShuffleIndexSubjects\"}";
static const char kMethodJson_22[] = "{\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-16\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_16:System.Void()\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_16()\",\"identity\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\",\"definitionSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_16:System.Void()\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_16:System.Void()\",\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-16\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_16()\"},\"nativeSymbol\":\"ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_16\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":5,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Int32\",\"ilOffset\":2,\"resultType\":\"System.Int32[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Int32\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::4F6ADDC9659D6FB90FE94B6688A79F2A1FA8D36EC43F8F3E1D9B6528C448A384\",\"ilOffset\":8,\"resultType\":\"System.RuntimeFieldHandle\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::4F6ADDC9659D6FB90FE94B6688A79F2A1FA8D36EC43F8F3E1D9B6528C448A384\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::4F6ADDC9659D6FB90FE94B6688A79F2A1FA8D36EC43F8F3E1D9B6528C448A384\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"\\u003CPrivateImplementationDetails\\u003E\\u002B__StaticArrayInitTypeSize=20\"},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":13,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":19,\"isPreserveSig\":false},{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__17_0\",\"ilOffset\":20,\"resultType\":\"System.Func\\u003CSystem.Int32,System.Boolean\\u003E\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__17_0\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__17_0\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Func\\u003CSystem.Int32,System.Boolean\\u003E\"},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":25,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":51,\"ilOffset\":26,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":28,\"isPreserveSig\":false},{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9\",\"ilOffset\":29,\"resultType\":\"ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\"},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"ldftn\",\"opCode\":73,\"operand\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_16\\u003Eb__17_0:System.Boolean(System.Int32)\",\"ilOffset\":34,\"resultType\":\"System.IntPtr\",\"callee\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_16\\u003Eb__17_0:System.Boolean(System.Int32)\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_16\\u003Eb__17_0:System.Boolean(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_16\\u003Eb__17_0:System.Boolean(System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":40,\"resultType\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Boolean\\u003E\",\"callee\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Boolean\\u003E::.ctor:System.Void(System.Object,System.IntPtr)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Boolean\\u003E::.ctor:System.Void(System.Object,System.IntPtr)\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Boolean\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":45,\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__17_0\",\"ilOffset\":46,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__17_0\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__17_0\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Func\\u003CSystem.Int32,System.Boolean\\u003E\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":99,\"ilOffset\":51,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":53,\"resultType\":\"System.Int32\",\"callee\":\"System.Linq/Enumerable::LastOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Boolean\\u003E,System.Int32)\",\"reference\":{\"assemblyName\":\"System.Linq\",\"subjectKind\":\"method\",\"subjectId\":\"System.Linq/Enumerable::LastOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Boolean\\u003E,System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Linq\",\"subjectId\":\"System.Linq/Enumerable::LastOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Boolean\\u003E,System.Int32)\",\"openDefinitionSubjectId\":\"System.Linq/Enumerable::LastOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E,!!0)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::LastOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E,!!0);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::LastOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E,!!0);type=[];method=[System.Int32]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::LastOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E,!!0)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::LastOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E,!!0);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::LastOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E,!!0);type=[];method=[System.Int32]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Linq/Enumerable::LastOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Boolean\\u003E,System.Int32)\",\"definitionSubjectId\":\"System.Linq/Enumerable::LastOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E,!!0)\",\"displaySubjectId\":\"System.Linq/Enumerable::LastOrDefault\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Boolean\\u003E,System.Int32)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::LastOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E,!!0)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":58,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":59,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ShuffleIndexSubjects\"}";
static const char kMethodJson_23[] = "{\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects-c.custom-entry-subject-16-b-17-0\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_16\\u003Eb__17_0:System.Boolean(System.Int32)\",\"signature\":\"System.Boolean ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_16\\u003Eb__17_0(System.Int32)\",\"identity\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"definitionSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_16\\u003Eb__17_0:System.Boolean(System.Int32)\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_16\\u003Eb__17_0:System.Boolean(System.Int32)\",\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects-c.custom-entry-subject-16-b-17-0\",\"signature\":\"System.Boolean ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_16\\u003Eb__17_0(System.Int32)\"},\"nativeSymbol\":\"ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_16_b__17_0_System_Int32\",\"isStatic\":false,\"returnType\":\"System.Boolean\",\"returnAbi\":{\"carrierKindCode\":2,\"typeShape\":2},\"parameterCount\":1,\"parameterAbis\":[{\"carrierKindCode\":1,\"typeShape\":2}],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":0,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":10,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"cgt\",\"opCode\":33,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":5,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ShuffleIndexSubjects\"}";
static const char kMethodJson_24[] = 
    "{\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-17\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_17:System.Void()\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_17()\",\"identity\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\",\"definitionSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_17:System.Void()\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_17:System.Void()\",\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-17\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_17()\"},\"nativeSymbol\":\"ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_17\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":3,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":5,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Int32\",\"ilOffset\":2,\"resultType\":\"System.Int32[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Int32\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::4F6ADDC9659D6FB90FE94B6688A79F2A1FA8D36EC43F8F3E1D9B6528C448A384\",\"ilOffset\":8,\"resultType\":\"System.RuntimeFieldHandle\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::4F6ADDC9659D6FB90FE94B6688A79F2A1FA8D36EC43F8F3E1D9B6528C448A384\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::4F6ADDC9659D6FB90FE94B6688A79F2A1FA8D36EC43F8F3E1D9B6528C448A384\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"\\u003CPrivateImplementationDetails\\u003E\\u002B__StaticArrayInitTypeSize=20\"},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":13,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":19,\"isPreserveSig\":false},{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__18_0\",\"ilOffset\":20,\"resultType\":\"System.Func\\u003CSystem.Int32,System.Int32\\u003E\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__18_0\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__18_0\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Func\\u003CSystem.Int32,System.Int32\\u003E\"},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":25,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":51,\"ilOffset\":26,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":28,\"isPreserveSig\":false},{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9\",\"ilOffset\":29,\"resultType\":\"ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\"},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"ldftn\",\"opCode\":73,\"operand\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_17\\u003Eb__18_0:System.Int32(System.Int32)\",\"ilOffset\":34,\"resultType\":\"System.IntPtr\",\"callee\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_17\\u003Eb__18_0:System.Int32(System.Int32)\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_17\\u003Eb__18_0:System.Int32(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_17\\u003Eb__18_0:System.Int32(System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":40,\"resultType\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Int32\\u003E\",\"callee\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Int32\\u003E::.ctor:System.Void(System.Object,System.IntPtr)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Int32\\u003E::.ctor:System.Void(System.Object,System.IntPtr)\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Int32\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":45,\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__18_0\",\"ilOffset\":46,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__18_0\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__18_0\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Func\\u003CSystem.Int32,System.Int32\\u003E\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":51,\"resultType\":\"IOrderedEnumerable\\u003CSystem.Int32\\u003E\",\"callee\":\"System.Linq/Enumerable::OrderBy\\u003CSystem.Int32,System.Int32\\u003E:IOrderedEnumerable\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Int32\\u003E)\",\"reference\":{\"assemblyName\":\"System.Linq\",\"subjectKind\":\"method\",\"subjectId\":\"System.Linq/Enumerable::OrderBy\\u003CSystem.Int32,System.Int32\\u003E:IOrderedEnumerable\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Int32\\u003E)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Linq\",\"subjectId\":\"System.Linq/Enumerable::OrderBy\\u003CSystem.Int32,System.Int32\\u003E:IOrderedEnumerable\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Int32\\u003E)\",\"openDefinitionSubjectId\":\"System.Linq/Enumerable::OrderBy:IOrderedEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,!!1\\u003E)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::OrderBy:IOrderedEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,!!1\\u003E);type=[];method=[System.Int32,System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::OrderBy:IOrderedEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,!!1\\u003E);type=[];method=[System.Int32,System.Int32]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::OrderBy:IOrderedEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,!!1\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\",\"System.Int32\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::OrderBy:IOrderedEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,!!1\\u003E);type=[];method=[System.Int32,System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::OrderBy:IOrderedEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,!!1\\u003E);type=[];method=[System.Int32,System.Int32]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Linq/Enumerable::OrderBy\\u003CSystem.Int32,System.Int32\\u003E:IOrderedEnumerable\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Int32\\u003E)\",\"definitionSubjectId\":\"System.Linq/Enumerable::OrderBy:IOrderedEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,!!1\\u003E)\",\"displaySubjectId\":\"System.Linq/Enumerable::OrderBy\\u003CSystem.Int32,System.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Int32\\u003E)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::OrderBy:IOrderedEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,!!1\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\",\"System.Int32\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":56,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":57,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":58,\"resultType\":\"System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E\",\"callee\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E()\",\"openDefinitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.Int32];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.Int32];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E()\",\"definitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"displaySubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator()\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerabl"
    "e\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":63,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":64,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":65,\"resultType\":\"System.Boolean\",\"callee\":\"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":70,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":71,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ShuffleIndexSubjects\"}"
    ;
static const char kMethodJson_25[] = "{\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects-c.custom-entry-subject-17-b-18-0\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_17\\u003Eb__18_0:System.Int32(System.Int32)\",\"signature\":\"System.Int32 ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_17\\u003Eb__18_0(System.Int32)\",\"identity\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"definitionSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_17\\u003Eb__18_0:System.Int32(System.Int32)\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_17\\u003Eb__18_0:System.Int32(System.Int32)\",\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects-c.custom-entry-subject-17-b-18-0\",\"signature\":\"System.Int32 ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_17\\u003Eb__18_0(System.Int32)\"},\"nativeSymbol\":\"ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_17_b__18_0_System_Int32\",\"isStatic\":false,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":1,\"parameterAbis\":[{\"carrierKindCode\":1,\"typeShape\":2}],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"call\",\"opCode\":14,\"ilOffset\":0,\"resultType\":\"System.Random\",\"callee\":\"System.Private.CoreLib/System.Random::get_Shared:System.Random()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Random::get_Shared:System.Random()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Random::get_Shared:System.Random()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":5,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Random::Next:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Random::Next:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Random::Next:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":10,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ShuffleIndexSubjects\"}";
static const char kMethodJson_26[] = "{\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-18\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_18:System.Void()\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_18()\",\"identity\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\",\"definitionSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_18:System.Void()\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_18:System.Void()\",\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-18\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_18()\"},\"nativeSymbol\":\"ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_18\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Int32\",\"ilOffset\":2,\"resultType\":\"System.Int32[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Int32\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stelem\",\"opCode\":45,\"operand\":\"System.Int32\",\"ilOffset\":11,\"resultType\":\"System.Void\",\"runtimeServiceKind\":13,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":12,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":13,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"callee\":\"System.Linq/Enumerable::Single\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E)\",\"reference\":{\"assemblyName\":\"System.Linq\",\"subjectKind\":\"method\",\"subjectId\":\"System.Linq/Enumerable::Single\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Linq\",\"subjectId\":\"System.Linq/Enumerable::Single\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E)\",\"openDefinitionSubjectId\":\"System.Linq/Enumerable::Single:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::Single:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::Single:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E);type=[];method=[System.Int32]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::Single:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::Single:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::Single:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E);type=[];method=[System.Int32]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Linq/Enumerable::Single\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E)\",\"definitionSubjectId\":\"System.Linq/Enumerable::Single:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E)\",\"displaySubjectId\":\"System.Linq/Enumerable::Single\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::Single:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":19,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":20,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ShuffleIndexSubjects\"}";
static const char kMethodJson_27[] = "{\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-19\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_19:System.Void()\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_19()\",\"identity\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\",\"definitionSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_19:System.Void()\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_19:System.Void()\",\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-19\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_19()\"},\"nativeSymbol\":\"ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_19\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":5,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Int32\",\"ilOffset\":2,\"resultType\":\"System.Int32[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Int32\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::6E28B4D41AF8A88650219653A728E0FD6760BB2B1DFEB7A9EB8DEB5B4E7010E0\",\"ilOffset\":8,\"resultType\":\"System.RuntimeFieldHandle\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::6E28B4D41AF8A88650219653A728E0FD6760BB2B1DFEB7A9EB8DEB5B4E7010E0\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::6E28B4D41AF8A88650219653A728E0FD6760BB2B1DFEB7A9EB8DEB5B4E7010E0\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"\\u003CPrivateImplementationDetails\\u003E\\u002B__StaticArrayInitTypeSize=20\"},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":13,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":19,\"isPreserveSig\":false},{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__20_0\",\"ilOffset\":20,\"resultType\":\"System.Func\\u003CSystem.Int32,System.Boolean\\u003E\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__20_0\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__20_0\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Func\\u003CSystem.Int32,System.Boolean\\u003E\"},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":25,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":51,\"ilOffset\":26,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":28,\"isPreserveSig\":false},{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9\",\"ilOffset\":29,\"resultType\":\"ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\"},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"ldftn\",\"opCode\":73,\"operand\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_19\\u003Eb__20_0:System.Boolean(System.Int32)\",\"ilOffset\":34,\"resultType\":\"System.IntPtr\",\"callee\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_19\\u003Eb__20_0:System.Boolean(System.Int32)\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_19\\u003Eb__20_0:System.Boolean(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_19\\u003Eb__20_0:System.Boolean(System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":40,\"resultType\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Boolean\\u003E\",\"callee\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Boolean\\u003E::.ctor:System.Void(System.Object,System.IntPtr)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Boolean\\u003E::.ctor:System.Void(System.Object,System.IntPtr)\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Boolean\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":45,\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__20_0\",\"ilOffset\":46,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__20_0\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__20_0\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Func\\u003CSystem.Int32,System.Boolean\\u003E\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":51,\"resultType\":\"System.Int32\",\"callee\":\"System.Linq/Enumerable::Single\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Boolean\\u003E)\",\"reference\":{\"assemblyName\":\"System.Linq\",\"subjectKind\":\"method\",\"subjectId\":\"System.Linq/Enumerable::Single\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Boolean\\u003E)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Linq\",\"subjectId\":\"System.Linq/Enumerable::Single\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Boolean\\u003E)\",\"openDefinitionSubjectId\":\"System.Linq/Enumerable::Single:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::Single:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::Single:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E);type=[];method=[System.Int32]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::Single:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::Single:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::Single:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E);type=[];method=[System.Int32]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Linq/Enumerable::Single\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Boolean\\u003E)\",\"definitionSubjectId\":\"System.Linq/Enumerable::Single:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E)\",\"displaySubjectId\":\"System.Linq/Enumerable::Single\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Boolean\\u003E)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::Single:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":56,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":57,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ShuffleIndexSubjects\"}";
static const char kMethodJson_28[] = "{\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects-c.custom-entry-subject-19-b-20-0\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_19\\u003Eb__20_0:System.Boolean(System.Int32)\",\"signature\":\"System.Boolean ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_19\\u003Eb__20_0(System.Int32)\",\"identity\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"definitionSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_19\\u003Eb__20_0:System.Boolean(System.Int32)\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_19\\u003Eb__20_0:System.Boolean(System.Int32)\",\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects-c.custom-entry-subject-19-b-20-0\",\"signature\":\"System.Boolean ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_19\\u003Eb__20_0(System.Int32)\"},\"nativeSymbol\":\"ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_19_b__20_0_System_Int32\",\"isStatic\":false,\"returnType\":\"System.Boolean\",\"returnAbi\":{\"carrierKindCode\":2,\"typeShape\":2},\"parameterCount\":1,\"parameterAbis\":[{\"carrierKindCode\":1,\"typeShape\":2}],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":0,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":30,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":5,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ShuffleIndexSubjects\"}";
static const char kMethodJson_29[] = "{\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-20\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_20:System.Void()\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_20()\",\"identity\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\",\"definitionSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_20:System.Void()\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_20:System.Void()\",\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-20\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_20()\"},\"nativeSymbol\":\"ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_20\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Int32\",\"ilOffset\":2,\"resultType\":\"System.Int32[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Int32\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stelem\",\"opCode\":45,\"operand\":\"System.Int32\",\"ilOffset\":11,\"resultType\":\"System.Void\",\"runtimeServiceKind\":13,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":12,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":13,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"callee\":\"System.Linq/Enumerable::SingleOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E)\",\"reference\":{\"assemblyName\":\"System.Linq\",\"subjectKind\":\"method\",\"subjectId\":\"System.Linq/Enumerable::SingleOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Linq\",\"subjectId\":\"System.Linq/Enumerable::SingleOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E)\",\"openDefinitionSubjectId\":\"System.Linq/Enumerable::SingleOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::SingleOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::SingleOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E);type=[];method=[System.Int32]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::SingleOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::SingleOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::SingleOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E);type=[];method=[System.Int32]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Linq/Enumerable::SingleOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E)\",\"definitionSubjectId\":\"System.Linq/Enumerable::SingleOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E)\",\"displaySubjectId\":\"System.Linq/Enumerable::SingleOrDefault\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::SingleOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":19,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":20,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ShuffleIndexSubjects\"}";
static const char kMethodJson_30[] = "{\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-21\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_21:System.Void()\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_21()\",\"identity\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\",\"definitionSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_21:System.Void()\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_21:System.Void()\",\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-21\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_21()\"},\"nativeSymbol\":\"ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_21\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Int32\",\"ilOffset\":2,\"resultType\":\"System.Int32[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Int32\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":7,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":99,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":11,\"resultType\":\"System.Int32\",\"callee\":\"System.Linq/Enumerable::SingleOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Int32)\",\"reference\":{\"assemblyName\":\"System.Linq\",\"subjectKind\":\"method\",\"subjectId\":\"System.Linq/Enumerable::SingleOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Linq\",\"subjectId\":\"System.Linq/Enumerable::SingleOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Int32)\",\"openDefinitionSubjectId\":\"System.Linq/Enumerable::SingleOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::SingleOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::SingleOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0);type=[];method=[System.Int32]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::SingleOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::SingleOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::SingleOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0);type=[];method=[System.Int32]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Linq/Enumerable::SingleOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Int32)\",\"definitionSubjectId\":\"System.Linq/Enumerable::SingleOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0)\",\"displaySubjectId\":\"System.Linq/Enumerable::SingleOrDefault\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Int32)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::SingleOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":16,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":17,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ShuffleIndexSubjects\"}";
static const char kMethodJson_31[] = "{\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-22\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_22:System.Void()\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_22()\",\"identity\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\",\"definitionSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_22:System.Void()\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_22:System.Void()\",\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-22\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_22()\"},\"nativeSymbol\":\"ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_22\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":5,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Int32\",\"ilOffset\":2,\"resultType\":\"System.Int32[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Int32\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::6E28B4D41AF8A88650219653A728E0FD6760BB2B1DFEB7A9EB8DEB5B4E7010E0\",\"ilOffset\":8,\"resultType\":\"System.RuntimeFieldHandle\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::6E28B4D41AF8A88650219653A728E0FD6760BB2B1DFEB7A9EB8DEB5B4E7010E0\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::6E28B4D41AF8A88650219653A728E0FD6760BB2B1DFEB7A9EB8DEB5B4E7010E0\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"\\u003CPrivateImplementationDetails\\u003E\\u002B__StaticArrayInitTypeSize=20\"},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":13,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":19,\"isPreserveSig\":false},{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__23_0\",\"ilOffset\":20,\"resultType\":\"System.Func\\u003CSystem.Int32,System.Boolean\\u003E\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__23_0\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__23_0\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Func\\u003CSystem.Int32,System.Boolean\\u003E\"},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":25,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":51,\"ilOffset\":26,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":28,\"isPreserveSig\":false},{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9\",\"ilOffset\":29,\"resultType\":\"ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\"},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"ldftn\",\"opCode\":73,\"operand\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_22\\u003Eb__23_0:System.Boolean(System.Int32)\",\"ilOffset\":34,\"resultType\":\"System.IntPtr\",\"callee\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_22\\u003Eb__23_0:System.Boolean(System.Int32)\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_22\\u003Eb__23_0:System.Boolean(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_22\\u003Eb__23_0:System.Boolean(System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":40,\"resultType\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Boolean\\u003E\",\"callee\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Boolean\\u003E::.ctor:System.Void(System.Object,System.IntPtr)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Boolean\\u003E::.ctor:System.Void(System.Object,System.IntPtr)\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Boolean\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":45,\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__23_0\",\"ilOffset\":46,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__23_0\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__23_0\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Func\\u003CSystem.Int32,System.Boolean\\u003E\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":51,\"resultType\":\"System.Int32\",\"callee\":\"System.Linq/Enumerable::SingleOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Boolean\\u003E)\",\"reference\":{\"assemblyName\":\"System.Linq\",\"subjectKind\":\"method\",\"subjectId\":\"System.Linq/Enumerable::SingleOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Boolean\\u003E)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Linq\",\"subjectId\":\"System.Linq/Enumerable::SingleOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Boolean\\u003E)\",\"openDefinitionSubjectId\":\"System.Linq/Enumerable::SingleOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::SingleOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::SingleOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E);type=[];method=[System.Int32]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::SingleOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::SingleOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::SingleOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E);type=[];method=[System.Int32]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Linq/Enumerable::SingleOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Boolean\\u003E)\",\"definitionSubjectId\":\"System.Linq/Enumerable::SingleOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E)\",\"displaySubjectId\":\"System.Linq/Enumerable::SingleOrDefault\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Boolean\\u003E)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::SingleOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":56,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":57,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ShuffleIndexSubjects\"}";
static const char kMethodJson_32[] = "{\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects-c.custom-entry-subject-22-b-23-0\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_22\\u003Eb__23_0:System.Boolean(System.Int32)\",\"signature\":\"System.Boolean ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_22\\u003Eb__23_0(System.Int32)\",\"identity\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"definitionSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_22\\u003Eb__23_0:System.Boolean(System.Int32)\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_22\\u003Eb__23_0:System.Boolean(System.Int32)\",\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects-c.custom-entry-subject-22-b-23-0\",\"signature\":\"System.Boolean ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_22\\u003Eb__23_0(System.Int32)\"},\"nativeSymbol\":\"ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_22_b__23_0_System_Int32\",\"isStatic\":false,\"returnType\":\"System.Boolean\",\"returnAbi\":{\"carrierKindCode\":2,\"typeShape\":2},\"parameterCount\":1,\"parameterAbis\":[{\"carrierKindCode\":1,\"typeShape\":2}],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":0,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":100,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":5,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ShuffleIndexSubjects\"}";
static const char kMethodJson_33[] = "{\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-23\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_23:System.Void()\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_23()\",\"identity\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\",\"definitionSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_23:System.Void()\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects::CustomEntrySubject_23:System.Void()\",\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects.custom-entry-subject-23\",\"signature\":\"System.Void ShuffleIndexSubjects::CustomEntrySubject_23()\"},\"nativeSymbol\":\"ShuffleIndexSubjects_ShuffleIndexSubjects_CustomEntrySubject_23\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":5,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Int32\",\"ilOffset\":2,\"resultType\":\"System.Int32[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Int32\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::6E28B4D41AF8A88650219653A728E0FD6760BB2B1DFEB7A9EB8DEB5B4E7010E0\",\"ilOffset\":8,\"resultType\":\"System.RuntimeFieldHandle\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::6E28B4D41AF8A88650219653A728E0FD6760BB2B1DFEB7A9EB8DEB5B4E7010E0\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E::6E28B4D41AF8A88650219653A728E0FD6760BB2B1DFEB7A9EB8DEB5B4E7010E0\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/\\u003CPrivateImplementationDetails\\u003E\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"\\u003CPrivateImplementationDetails\\u003E\\u002B__StaticArrayInitTypeSize=20\"},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":13,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":19,\"isPreserveSig\":false},{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__24_0\",\"ilOffset\":20,\"resultType\":\"System.Func\\u003CSystem.Int32,System.Boolean\\u003E\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__24_0\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__24_0\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Func\\u003CSystem.Int32,System.Boolean\\u003E\"},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":25,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":51,\"ilOffset\":26,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":28,\"isPreserveSig\":false},{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9\",\"ilOffset\":29,\"resultType\":\"ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\"},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"ldftn\",\"opCode\":73,\"operand\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_23\\u003Eb__24_0:System.Boolean(System.Int32)\",\"ilOffset\":34,\"resultType\":\"System.IntPtr\",\"callee\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_23\\u003Eb__24_0:System.Boolean(System.Int32)\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_23\\u003Eb__24_0:System.Boolean(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_23\\u003Eb__24_0:System.Boolean(System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":40,\"resultType\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Boolean\\u003E\",\"callee\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Boolean\\u003E::.ctor:System.Void(System.Object,System.IntPtr)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Boolean\\u003E::.ctor:System.Void(System.Object,System.IntPtr)\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Func\\u003CSystem.Int32,System.Boolean\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":45,\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__24_0\",\"ilOffset\":46,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__24_0\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__24_0\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Func\\u003CSystem.Int32,System.Boolean\\u003E\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":99,\"ilOffset\":51,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":53,\"resultType\":\"System.Int32\",\"callee\":\"System.Linq/Enumerable::SingleOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Boolean\\u003E,System.Int32)\",\"reference\":{\"assemblyName\":\"System.Linq\",\"subjectKind\":\"method\",\"subjectId\":\"System.Linq/Enumerable::SingleOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Boolean\\u003E,System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Linq\",\"subjectId\":\"System.Linq/Enumerable::SingleOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Boolean\\u003E,System.Int32)\",\"openDefinitionSubjectId\":\"System.Linq/Enumerable::SingleOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E,!!0)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::SingleOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E,!!0);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::SingleOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E,!!0);type=[];method=[System.Int32]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::SingleOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E,!!0)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::SingleOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E,!!0);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::SingleOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E,!!0);type=[];method=[System.Int32]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Linq/Enumerable::SingleOrDefault\\u003CSystem.Int32\\u003E:System.Int32(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Boolean\\u003E,System.Int32)\",\"definitionSubjectId\":\"System.Linq/Enumerable::SingleOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E,!!0)\",\"displaySubjectId\":\"System.Linq/Enumerable::SingleOrDefault\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Func\\u003CSystem.Int32,System.Boolean\\u003E,System.Int32)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::SingleOrDefault:!!0(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Func\\u003C!!0,System.Boolean\\u003E,!!0)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":58,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":59,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ShuffleIndexSubjects\"}";
static const char kMethodJson_34[] = "{\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects-c.custom-entry-subject-23-b-24-0\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_23\\u003Eb__24_0:System.Boolean(System.Int32)\",\"signature\":\"System.Boolean ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_23\\u003Eb__24_0(System.Int32)\",\"identity\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"definitionSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_23\\u003Eb__24_0:System.Boolean(System.Int32)\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_23\\u003Eb__24_0:System.Boolean(System.Int32)\",\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects-c.custom-entry-subject-23-b-24-0\",\"signature\":\"System.Boolean ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_23\\u003Eb__24_0(System.Int32)\"},\"nativeSymbol\":\"ShuffleIndexSubjects_ShuffleIndexSubjects___c_CustomEntrySubject_23_b__24_0_System_Int32\",\"isStatic\":false,\"returnType\":\"System.Boolean\",\"returnAbi\":{\"carrierKindCode\":2,\"typeShape\":2},\"parameterCount\":1,\"parameterAbis\":[{\"carrierKindCode\":1,\"typeShape\":2}],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":0,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":100,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":5,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ShuffleIndexSubjects\"}";
static const char kMethodJson_35[] = "{\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects-c.cctor\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::.cctor:System.Void()\",\"signature\":\"System.Void ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::.cctor()\",\"identity\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"definitionSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::.cctor:System.Void()\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::.cctor:System.Void()\",\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects-c.cctor\",\"signature\":\"System.Void ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::.cctor()\"},\"nativeSymbol\":\"ShuffleIndexSubjects_ShuffleIndexSubjects___c_cctor\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":0,\"resultType\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"callee\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":true,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"baseTypeSubjectId\":\"System.Private.CoreLib/System.Object\",\"implementedInterfaceSubjectIds\":[],\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9\",\"ilOffset\":5,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ShuffleIndexSubjects\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":10,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ShuffleIndexSubjects\"}";
static const char kMethodJson_36[] = "{\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects-c.ctor\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::.ctor:System.Void()\",\"signature\":\"System.Void ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::.ctor()\",\"identity\":{\"assemblyName\":\"ShuffleIndexSubjects\",\"declaringTypeSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"definitionSubjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::.ctor:System.Void()\",\"subjectId\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::.ctor:System.Void()\",\"methodId\":\"shuffle-index-subjects.shuffle-index-subjects-c.ctor\",\"signature\":\"System.Void ShuffleIndexSubjects\\u002B\\u003C\\u003Ec::.ctor()\"},\"nativeSymbol\":\"ShuffleIndexSubjects_ShuffleIndexSubjects___c__ctor\",\"isStatic\":false,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"ShuffleIndexSubjects/ShuffleIndexSubjects\\u002B\\u003C\\u003Ec\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":7,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ShuffleIndexSubjects\"}";

extern "C" const JitMethodEntry kChaosJitMethodEntries[37] =
{
    { kMethodJson_0, 6923u, 0x00000016u, 0u },
    { kMethodJson_1, 7678u, 0x00000017u, 0u },
    { kMethodJson_2, 5974u, 0x00000018u, 0u },
    { kMethodJson_3, 6729u, 0x00000019u, 0u },
    { kMethodJson_4, 6599u, 0x0000001Au, 0u },
    { kMethodJson_5, 11727u, 0x0000001Bu, 0u },
    { kMethodJson_6, 1966u, 0x00000030u, 0u },
    { kMethodJson_7, 6716u, 0x0000001Cu, 0u },
    { kMethodJson_8, 5035u, 0x0000001Du, 0u },
    { kMethodJson_9, 11844u, 0x0000001Eu, 0u },
    { kMethodJson_10, 1967u, 0x00000031u, 0u },
    { kMethodJson_11, 12055u, 0x0000001Fu, 0u },
    { kMethodJson_12, 1975u, 0x00000032u, 0u },
    { kMethodJson_13, 18924u, 0x00000020u, 0u },
    { kMethodJson_14, 3062u, 0x00000033u, 0u },
    { kMethodJson_15, 6594u, 0x00000021u, 0u },
    { kMethodJson_16, 11736u, 0x00000022u, 0u },
    { kMethodJson_17, 1982u, 0x00000034u, 0u },
    { kMethodJson_18, 6711u, 0x00000023u, 0u },
    { kMethodJson_19, 5030u, 0x00000024u, 0u },
    { kMethodJson_20, 11853u, 0x00000025u, 0u },
    { kMethodJson_21, 1983u, 0x00000035u, 0u },
    { kMethodJson_22, 12054u, 0x00000026u, 0u },
    { kMethodJson_23, 1983u, 0x00000036u, 0u },
    { kMethodJson_24, 16349u, 0x00000027u, 0u },
    { kMethodJson_25, 2894u, 0x00000037u, 0u },
    { kMethodJson_26, 5146u, 0x00000028u, 0u },
    { kMethodJson_27, 11762u, 0x00000029u, 0u },
    { kMethodJson_28, 1983u, 0x00000038u, 0u },
    { kMethodJson_29, 5263u, 0x0000002Au, 0u },
    { kMethodJson_30, 5056u, 0x0000002Bu, 0u },
    { kMethodJson_31, 11879u, 0x0000002Cu, 0u },
    { kMethodJson_32, 1984u, 0x00000039u, 0u },
    { kMethodJson_33, 12080u, 0x0000002Du, 0u },
    { kMethodJson_34, 1984u, 0x0000003Au, 0u },
    { kMethodJson_35, 2906u, 0x0000002Eu, 0u },
    { kMethodJson_36, 2056u, 0x0000002Fu, 0u }
};

extern "C" const uint32_t kChaosJitMethodEntryCount = 37u;

extern "C" void ChaosJitRegisterAll() {
    RegisterJitMethods(kChaosJitMethodEntries, kChaosJitMethodEntryCount);
}