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

namespace chaos::il2cpp::codegen::ProjectionSubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


const void* chaos_vtable_ProjectionSubjects_ProjectionSubjects___c[];
inline TypeInfoV0 chaos_mt_ProjectionSubjects__PrivateImplementationDetails_ = {{nullptr, nullptr, 12505752665576921463ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_ProjectionSubjects__PrivateImplementationDetails_ = static_cast<CHAOS_IL2CPP_INTPTR>(12505752665576921463ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Object = {{nullptr, nullptr, 15228727185366376748ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_INTPTR>(15228727185366376748ULL);
inline TypeInfoV0 chaos_mt_ProjectionSubjects_ProjectionSubjects___c = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_ProjectionSubjects_ProjectionSubjects___c, 13737965862345364542ULL, 10u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_ProjectionSubjects_ProjectionSubjects___c = static_cast<CHAOS_IL2CPP_INTPTR>(13737965862345364542ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Delegate = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, nullptr, 7451128447593600616ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Delegate = static_cast<CHAOS_IL2CPP_INTPTR>(7451128447593600616ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_MulticastDelegate = {{&chaos_mt_System_Private_CoreLib_System_Delegate.hot, nullptr, 6681393039041505440ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_MulticastDelegate = static_cast<CHAOS_IL2CPP_INTPTR>(6681393039041505440ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Collections_Generic_IEnumerable_System_Int32__ = {{&chaos_mt_System_Private_CoreLib_System_MulticastDelegate.hot, nullptr, 13589737125985385701ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Func_System_Int32_System_Collections_Generic_IEnumerable_System_Int32__ = static_cast<CHAOS_IL2CPP_INTPTR>(13589737125985385701ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Int32_System_Collections_Generic_IEnumerable_System_Int32__ = {{&chaos_mt_System_Private_CoreLib_System_MulticastDelegate.hot, nullptr, 7529773789651897342ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Func_System_Int32_System_Int32_System_Collections_Generic_IEnumerable_System_Int32__ = static_cast<CHAOS_IL2CPP_INTPTR>(7529773789651897342ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Int32_System_Int32_ = {{&chaos_mt_System_Private_CoreLib_System_MulticastDelegate.hot, nullptr, 2159441345668172316ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Func_System_Int32_System_Int32_System_Int32_ = static_cast<CHAOS_IL2CPP_INTPTR>(2159441345668172316ULL);
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

// ── Virtual method table arrays ──
extern "C" void ProjectionSubjects_ProjectionSubjects___c__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_0_b__1_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_1_b__2_0_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INTPTR ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_2_b__3_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_3_b__4_0_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INTPTR ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_4_b__5_0_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INT32 ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_4_b__5_1_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INTPTR ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_5_b__6_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_5_b__6_1_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INT32 ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_6_b__7_0_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
const void* chaos_vtable_ProjectionSubjects_ProjectionSubjects___c[] =
{
	reinterpret_cast<void*>(ProjectionSubjects_ProjectionSubjects___c__ctor),
	reinterpret_cast<void*>(ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_0_b__1_0_System_Int32),
	reinterpret_cast<void*>(ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_1_b__2_0_System_Int32_System_Int32),
	reinterpret_cast<void*>(ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_2_b__3_0_System_Int32),
	reinterpret_cast<void*>(ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_3_b__4_0_System_Int32_System_Int32),
	reinterpret_cast<void*>(ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_4_b__5_0_System_Int32_System_Int32),
	reinterpret_cast<void*>(ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_4_b__5_1_System_Int32_System_Int32),
	reinterpret_cast<void*>(ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_5_b__6_0_System_Int32),
	reinterpret_cast<void*>(ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_5_b__6_1_System_Int32_System_Int32),
	reinterpret_cast<void*>(ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_6_b__7_0_System_Int32_System_Int32),
};
static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_ProjectionSubjects_ProjectionSubjects___c[] =
{
	{ 0x0000001Du, reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects___c__ctor) },
	{ 0x0000001Eu, reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_0_b__1_0_System_Int32) },
	{ 0x0000001Fu, reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_1_b__2_0_System_Int32_System_Int32) },
	{ 0x00000020u, reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_2_b__3_0_System_Int32) },
	{ 0x00000021u, reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_3_b__4_0_System_Int32_System_Int32) },
	{ 0x00000022u, reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_4_b__5_0_System_Int32_System_Int32) },
	{ 0x00000023u, reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_4_b__5_1_System_Int32_System_Int32) },
	{ 0x00000024u, reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_5_b__6_0_System_Int32) },
	{ 0x00000025u, reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_5_b__6_1_System_Int32_System_Int32) },
	{ 0x00000026u, reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_6_b__7_0_System_Int32_System_Int32) },
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

struct chaos_type_ProjectionSubjects__PrivateImplementationDetails_
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR field_ProjectionSubjects__PrivateImplementationDetails___4636993D3E1DA4E9D6B8F87B79E8F7C6D018580D52661950EABC3845C5897A4D = 0;
	CHAOS_IL2CPP_INTPTR field_ProjectionSubjects__PrivateImplementationDetails___97CA1592048640A5368B4EC7C6934311567E09D50E7639918EC82C3D2A187CDA = 0;
	CHAOS_IL2CPP_INTPTR field_ProjectionSubjects__PrivateImplementationDetails___9AC9CF706FBD14D039E0436219C5D852927E5F69295F2EF423AE897345197B2A = 0;
};

struct chaos_type_System_Private_CoreLib_System_Object
{
	ThinLockableHeader header{};
};

struct chaos_type_ProjectionSubjects_ProjectionSubjects___c : public chaos_type_System_Private_CoreLib_System_Object
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

struct chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Collections_Generic_IEnumerable_System_Int32__ : public chaos_type_System_Private_CoreLib_System_MulticastDelegate
{
};

struct chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Int32_System_Collections_Generic_IEnumerable_System_Int32__ : public chaos_type_System_Private_CoreLib_System_MulticastDelegate
{
};

struct chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Int32_System_Int32_ : public chaos_type_System_Private_CoreLib_System_MulticastDelegate
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

CHAOS_IL2CPP_INTPTR chaos_static_ProjectionSubjects_ProjectionSubjects___c____9 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_ProjectionSubjects_ProjectionSubjects___c____9__1_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_ProjectionSubjects_ProjectionSubjects___c____9__2_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_ProjectionSubjects_ProjectionSubjects___c____9__3_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_ProjectionSubjects_ProjectionSubjects___c____9__4_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_ProjectionSubjects_ProjectionSubjects___c____9__5_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_ProjectionSubjects_ProjectionSubjects___c____9__5_1 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_ProjectionSubjects_ProjectionSubjects___c____9__6_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_ProjectionSubjects_ProjectionSubjects___c____9__6_1 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_ProjectionSubjects_ProjectionSubjects___c____9__7_0 = 0;

const CHAOS_IL2CPP_UINT8 chaos_static_field_data_ProjectionSubjects__PrivateImplementationDetails___4636993D3E1DA4E9D6B8F87B79E8F7C6D018580D52661950EABC3845C5897A4D[] =
{
	0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
};

const CHAOS_IL2CPP_UINT8 chaos_static_field_data_ProjectionSubjects__PrivateImplementationDetails___97CA1592048640A5368B4EC7C6934311567E09D50E7639918EC82C3D2A187CDA[] =
{
	0x0A, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00,
};

const CHAOS_IL2CPP_UINT8 chaos_static_field_data_ProjectionSubjects__PrivateImplementationDetails___9AC9CF706FBD14D039E0436219C5D852927E5F69295F2EF423AE897345197B2A[] =
{
	0x64, 0x00, 0x00, 0x00, 0xC8, 0x00, 0x00, 0x00, 0x2C, 0x01, 0x00, 0x00,
};

bool chaos_try_get_static_field_data(
	CHAOS_IL2CPP_INTPTR chaos_field_handle,
	const CHAOS_IL2CPP_UINT8*& chaos_data,
	CHAOS_IL2CPP_SIZE& chaos_size)
{
	if (chaos_field_handle == static_cast<CHAOS_IL2CPP_INTPTR>(74387353u))
	{
		chaos_data = chaos_static_field_data_ProjectionSubjects__PrivateImplementationDetails___4636993D3E1DA4E9D6B8F87B79E8F7C6D018580D52661950EABC3845C5897A4D;
		chaos_size = sizeof(chaos_static_field_data_ProjectionSubjects__PrivateImplementationDetails___4636993D3E1DA4E9D6B8F87B79E8F7C6D018580D52661950EABC3845C5897A4D);
		return true;
	}
	if (chaos_field_handle == static_cast<CHAOS_IL2CPP_INTPTR>(77829180u))
	{
		chaos_data = chaos_static_field_data_ProjectionSubjects__PrivateImplementationDetails___97CA1592048640A5368B4EC7C6934311567E09D50E7639918EC82C3D2A187CDA;
		chaos_size = sizeof(chaos_static_field_data_ProjectionSubjects__PrivateImplementationDetails___97CA1592048640A5368B4EC7C6934311567E09D50E7639918EC82C3D2A187CDA);
		return true;
	}
	if (chaos_field_handle == static_cast<CHAOS_IL2CPP_INTPTR>(81008348u))
	{
		chaos_data = chaos_static_field_data_ProjectionSubjects__PrivateImplementationDetails___9AC9CF706FBD14D039E0436219C5D852927E5F69295F2EF423AE897345197B2A;
		chaos_size = sizeof(chaos_static_field_data_ProjectionSubjects__PrivateImplementationDetails___9AC9CF706FBD14D039E0436219C5D852927E5F69295F2EF423AE897345197B2A);
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

CHAOS_IL2CPP_ONCE_FLAG chaos_type_init_once_ProjectionSubjects_ProjectionSubjects___c;

void chaos_ensure_type_initialized_ProjectionSubjects_ProjectionSubjects___c()
{
	CHAOS_IL2CPP_CALL_ONCE(chaos_type_init_once_ProjectionSubjects_ProjectionSubjects___c, []()
	{
	if (chaos_static_ProjectionSubjects_ProjectionSubjects___c____9 == 0)
	{
	    auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_ProjectionSubjects_ProjectionSubjects___c);
	    chaos_object->header.type_info = &chaos_mt_ProjectionSubjects_ProjectionSubjects___c.hot;
		const auto chaos_arg_0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
		ProjectionSubjects_ProjectionSubjects___c__ctor(chaos_arg_0);
	    chaos_static_ProjectionSubjects_ProjectionSubjects___c____9 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	});
}

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };

extern "C" void ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_0(void);
extern "C" CHAOS_IL2CPP_INT32 ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_0_b__1_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_1(void);
extern "C" CHAOS_IL2CPP_INT32 ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_1_b__2_0_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" void ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_2(void);
extern "C" CHAOS_IL2CPP_INTPTR ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_2_b__3_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_3(void);
extern "C" CHAOS_IL2CPP_INTPTR ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_3_b__4_0_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" void ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_4(void);
extern "C" CHAOS_IL2CPP_INTPTR ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_4_b__5_0_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INT32 ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_4_b__5_1_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" void ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_5(void);
extern "C" CHAOS_IL2CPP_INTPTR ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_5_b__6_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_5_b__6_1_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" void ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_6(void);
extern "C" CHAOS_IL2CPP_INT32 ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_6_b__7_0_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" void ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_7(void);
extern "C" void ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_8(void);
extern "C" void ProjectionSubjects_ProjectionSubjects___c_cctor(void);
extern "C" void ProjectionSubjects_ProjectionSubjects___c__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[21] = {
	0u,
	0u,
	1u,
	1u,
	3u,
	3u,
	4u,
	4u,
	6u,
	6u,
	8u,
	10u,
	10u,
	11u,
	13u,
	13u,
	15u,
	15u,
	15u,
	15u,
	15u,
};

static constexpr struct {
	::ChaosAbiManifestV0 header;
	::ChaosAbiMethodEntryV0 entries[20];
	CHAOS_IL2CPP_UINT8 params[15];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		20u,
		15u,
		1183352748u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_0
		{ 1u, 1u },  // ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_0_b__1_0_System_Int32
		{ 0u, 0u },  // ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_1
		{ 1u, 2u },  // ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_1_b__2_0_System_Int32_System_Int32
		{ 0u, 0u },  // ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_2
		{ 2u, 1u },  // ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_2_b__3_0_System_Int32
		{ 0u, 0u },  // ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_3
		{ 2u, 2u },  // ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_3_b__4_0_System_Int32_System_Int32
		{ 0u, 0u },  // ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_4
		{ 2u, 2u },  // ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_4_b__5_0_System_Int32_System_Int32
		{ 1u, 2u },  // ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_4_b__5_1_System_Int32_System_Int32
		{ 0u, 0u },  // ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_5
		{ 2u, 1u },  // ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_5_b__6_0_System_Int32
		{ 1u, 2u },  // ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_5_b__6_1_System_Int32_System_Int32
		{ 0u, 0u },  // ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_6
		{ 1u, 2u },  // ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_6_b__7_0_System_Int32_System_Int32
		{ 0u, 0u },  // ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_7
		{ 0u, 0u },  // ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_8
		{ 0u, 0u },  // ProjectionSubjects_ProjectionSubjects___c_cctor
		{ 0u, 0u },  // ProjectionSubjects_ProjectionSubjects___c__ctor
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
		1u,
		1u,
		1u,
	},
};
static const ::ChaosAbiManifestV0* const s_abi_manifest =
	reinterpret_cast<const ::ChaosAbiManifestV0*>(&s_abi_manifest_storage);
// ── Module registration ──

	static constexpr CHAOS_IL2CPP_UINT32 s_type_flags[5] = {
		0u,
		1548u,
		8u,
		1544u,
		1545u,
	};

	static constexpr const char* s_type_names[5] = {
		"<Module>",
		"ProjectionSubjects",
		"<PrivateImplementationDetails>",
		"<>c",
		"__StaticArrayInitTypeSize=12",
	};

	static constexpr const char* s_type_namespaces[5] = {
		"",
		"",
		"",
		"",
		"",
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_type_parent_tokens[5] = {
		0u,
		0u,
		0u,
		0u,
		0u,
	};

	static const TypeInfoHot* const s_type_info_ptrs[5] = {
		nullptr,
		nullptr,
		&chaos_mt_ProjectionSubjects__PrivateImplementationDetails_.hot,
		&chaos_mt_ProjectionSubjects_ProjectionSubjects___c.hot,
		nullptr,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_children[2] = {
		33554436u,
		33554437u,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_offset[6] = {
		0u,
		0u,
		1u,
		2u,
		2u,
		2u,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_generic_param_constraint_offset[6] = {
		0u,
		0u,
		0u,
		0u,
		0u,
		0u,
	};



	static const ::chaos::il2cpp::runtime_core::ModuleDescriptor s_native_aot_module = {
		/* .name_utf8         = */ "ProjectionSubjects",
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
		/* .type_count        = */ 5u,
	/* .custom_attribute_blob       = */ nullptr,  // Tier 1 — deferred
	/* .custom_attribute_offset     = */ nullptr,
	/* .custom_attribute_entity_count = */ 0u,
	/* .custom_attribute_materializer = */ nullptr,

		/* .abi_manifest      = */ s_abi_manifest,
	};
	static const CHAOS_IL2CPP_UINT32 s_native_aot_module_id =
		::chaos::il2cpp::runtime_core::RegisterModule("ProjectionSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[20] = {
	{ "CustomEntrySubject_0", 0x00000013u, 0u },  // ProjectionSubjects
	{ "CustomEntrySubject_1", 0x00000014u, 0u },  // ProjectionSubjects
	{ "CustomEntrySubject_2", 0x00000015u, 0u },  // ProjectionSubjects
	{ "CustomEntrySubject_3", 0x00000016u, 0u },  // ProjectionSubjects
	{ "CustomEntrySubject_4", 0x00000017u, 0u },  // ProjectionSubjects
	{ "CustomEntrySubject_5", 0x00000018u, 0u },  // ProjectionSubjects
	{ "CustomEntrySubject_6", 0x00000019u, 0u },  // ProjectionSubjects
	{ "CustomEntrySubject_7", 0x0000001Au, 0u },  // ProjectionSubjects
	{ "CustomEntrySubject_8", 0x0000001Bu, 0u },  // ProjectionSubjects
	{ "<CustomEntrySubject_0>b__1_0", 0x0000001Eu, 1u },  // ProjectionSubjects+<>c
	{ "<CustomEntrySubject_1>b__2_0", 0x0000001Fu, 2u },  // ProjectionSubjects+<>c
	{ "<CustomEntrySubject_2>b__3_0", 0x00000020u, 1u },  // ProjectionSubjects+<>c
	{ "<CustomEntrySubject_3>b__4_0", 0x00000021u, 2u },  // ProjectionSubjects+<>c
	{ "<CustomEntrySubject_4>b__5_0", 0x00000022u, 2u },  // ProjectionSubjects+<>c
	{ "<CustomEntrySubject_4>b__5_1", 0x00000023u, 2u },  // ProjectionSubjects+<>c
	{ "<CustomEntrySubject_5>b__6_0", 0x00000024u, 1u },  // ProjectionSubjects+<>c
	{ "<CustomEntrySubject_5>b__6_1", 0x00000025u, 2u },  // ProjectionSubjects+<>c
	{ "<CustomEntrySubject_6>b__7_0", 0x00000026u, 2u },  // ProjectionSubjects+<>c
	{ ".cctor", 0x0000001Cu, 0u },  // ProjectionSubjects+<>c
	{ ".ctor", 0x0000001Du, 0u },  // ProjectionSubjects+<>c
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[2] = {
	{ "ProjectionSubjects", "", 0u, 9u },
	{ "ProjectionSubjects+<>c", "", 9u, 11u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[20] = {
	{ 0x00000013u, 0u },
	{ 0x00000014u, 2u },
	{ 0x00000015u, 4u },
	{ 0x00000016u, 6u },
	{ 0x00000017u, 8u },
	{ 0x00000018u, 11u },
	{ 0x00000019u, 14u },
	{ 0x0000001Au, 16u },
	{ 0x0000001Bu, 17u },
	{ 0x0000001Cu, 18u },
	{ 0x0000001Du, 19u },
	{ 0x0000001Eu, 1u },
	{ 0x0000001Fu, 3u },
	{ 0x00000020u, 5u },
	{ 0x00000021u, 7u },
	{ 0x00000022u, 9u },
	{ 0x00000023u, 10u },
	{ 0x00000024u, 12u },
	{ 0x00000025u, 13u },
	{ 0x00000026u, 15u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[20] = {
	{ reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ProjectionSubjects::CustomEntrySubject_0
	{ reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_0_b__1_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ProjectionSubjects+<>c::<CustomEntrySubject_0>b__1_0
	{ reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ProjectionSubjects::CustomEntrySubject_1
	{ reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_1_b__2_0_System_Int32_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ProjectionSubjects+<>c::<CustomEntrySubject_1>b__2_0
	{ reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ProjectionSubjects::CustomEntrySubject_2
	{ reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_2_b__3_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ProjectionSubjects+<>c::<CustomEntrySubject_2>b__3_0
	{ reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ProjectionSubjects::CustomEntrySubject_3
	{ reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_3_b__4_0_System_Int32_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ProjectionSubjects+<>c::<CustomEntrySubject_3>b__4_0
	{ reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ProjectionSubjects::CustomEntrySubject_4
	{ reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_4_b__5_0_System_Int32_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ProjectionSubjects+<>c::<CustomEntrySubject_4>b__5_0
	{ reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_4_b__5_1_System_Int32_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ProjectionSubjects+<>c::<CustomEntrySubject_4>b__5_1
	{ reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ProjectionSubjects::CustomEntrySubject_5
	{ reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_5_b__6_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ProjectionSubjects+<>c::<CustomEntrySubject_5>b__6_0
	{ reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_5_b__6_1_System_Int32_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ProjectionSubjects+<>c::<CustomEntrySubject_5>b__6_1
	{ reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ProjectionSubjects::CustomEntrySubject_6
	{ reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_6_b__7_0_System_Int32_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ProjectionSubjects+<>c::<CustomEntrySubject_6>b__7_0
	{ reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ProjectionSubjects::CustomEntrySubject_7
	{ reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ProjectionSubjects::CustomEntrySubject_8
	{ reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects___c_cctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ProjectionSubjects+<>c::.cctor
	{ reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects___c__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ProjectionSubjects+<>c::.ctor
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"ProjectionSubjects",
	s_hotpatch_types,
	2u,
	s_hotpatch_methods,
	20u,
	s_hotpatch_slots,
	20u,
	s_hotpatch_entries,
	20u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[33] = {
	"System.Private.CoreLib/System.Int32",
	"ProjectionSubjects/<PrivateImplementationDetails>::4636993D3E1DA4E9D6B8F87B79E8F7C6D018580D52661950EABC3845C5897A4D",
	"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)",
	"ProjectionSubjects/ProjectionSubjects+<>c::<>9__1_0",
	"ProjectionSubjects/ProjectionSubjects+<>c::<>9",
	"System.Private.CoreLib/System.Func<System.Int32,System.Int32>::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Linq/Enumerable::Select<System.Int32,System.Int32>:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)",
	"System.Private.CoreLib/System.Collections.Generic.IEnumerable<System.Int32>::GetEnumerator:System.Collections.Generic.IEnumerator<System.Int32>()",
	"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()",
	"ProjectionSubjects/<PrivateImplementationDetails>::97CA1592048640A5368B4EC7C6934311567E09D50E7639918EC82C3D2A187CDA",
	"ProjectionSubjects/ProjectionSubjects+<>c::<>9__2_0",
	"System.Private.CoreLib/System.Func<System.Int32,System.Int32,System.Int32>::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Linq/Enumerable::Select<System.Int32,System.Int32>:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32,System.Int32>)",
	"ProjectionSubjects/ProjectionSubjects+<>c::<>9__3_0",
	"System.Private.CoreLib/System.Func<System.Int32,System.Collections.Generic.IEnumerable<System.Int32>>::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Linq/Enumerable::SelectMany<System.Int32,System.Int32>:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Collections.Generic.IEnumerable<System.Int32>>)",
	"ProjectionSubjects/ProjectionSubjects+<>c::<>9__4_0",
	"System.Private.CoreLib/System.Func<System.Int32,System.Int32,System.Collections.Generic.IEnumerable<System.Int32>>::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Linq/Enumerable::SelectMany<System.Int32,System.Int32>:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32,System.Collections.Generic.IEnumerable<System.Int32>>)",
	"ProjectionSubjects/ProjectionSubjects+<>c::<>9__5_0",
	"ProjectionSubjects/ProjectionSubjects+<>c::<>9__5_1",
	"System.Linq/Enumerable::SelectMany<System.Int32,System.Int32,System.Int32>:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32,System.Collections.Generic.IEnumerable<System.Int32>>,System.Func<System.Int32,System.Int32,System.Int32>)",
	"ProjectionSubjects/ProjectionSubjects+<>c::<>9__6_0",
	"ProjectionSubjects/ProjectionSubjects+<>c::<>9__6_1",
	"System.Linq/Enumerable::SelectMany<System.Int32,System.Int32,System.Int32>:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Collections.Generic.IEnumerable<System.Int32>>,System.Func<System.Int32,System.Int32,System.Int32>)",
	"ProjectionSubjects/ProjectionSubjects+<>c::<>9__7_0",
	"System.Linq/Enumerable::Zip<System.Int32,System.Int32,System.Int32>:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32,System.Int32>)",
	"System.Linq/Enumerable::Zip<System.Int32,System.Int32>:System.Collections.Generic.IEnumerable<System.ValueTuple<System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>)",
	"System.Private.CoreLib/System.Collections.Generic.IEnumerable<System.ValueTuple<System.Int32,System.Int32>>::GetEnumerator:System.Collections.Generic.IEnumerator<System.ValueTuple<System.Int32,System.Int32>>()",
	"ProjectionSubjects/<PrivateImplementationDetails>::9AC9CF706FBD14D039E0436219C5D852927E5F69295F2EF423AE897345197B2A",
	"System.Linq/Enumerable::Zip<System.Int32,System.Int32,System.Int32>:System.Collections.Generic.IEnumerable<System.ValueTuple<System.Int32,System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>)",
	"System.Private.CoreLib/System.Collections.Generic.IEnumerable<System.ValueTuple<System.Int32,System.Int32,System.Int32>>::GetEnumerator:System.Collections.Generic.IEnumerator<System.ValueTuple<System.Int32,System.Int32,System.Int32>>()",
	"System.Private.CoreLib/System.Object::.ctor:System.Void()",
};

extern "C" void* kChaosExternalRuntimeFnTable[33] = {
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
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__),
};

extern "C" int32_t kChaosExternalRuntimeCount = 33;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[20])() = {
	reinterpret_cast<void(*)()>(&ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_0),
	reinterpret_cast<void(*)()>(&ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_0_b__1_0_System_Int32),
	reinterpret_cast<void(*)()>(&ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_1),
	reinterpret_cast<void(*)()>(&ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_1_b__2_0_System_Int32_System_Int32),
	reinterpret_cast<void(*)()>(&ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_2),
	reinterpret_cast<void(*)()>(&ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_2_b__3_0_System_Int32),
	reinterpret_cast<void(*)()>(&ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_3),
	reinterpret_cast<void(*)()>(&ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_3_b__4_0_System_Int32_System_Int32),
	reinterpret_cast<void(*)()>(&ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_4),
	reinterpret_cast<void(*)()>(&ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_4_b__5_0_System_Int32_System_Int32),
	reinterpret_cast<void(*)()>(&ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_4_b__5_1_System_Int32_System_Int32),
	reinterpret_cast<void(*)()>(&ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_5),
	reinterpret_cast<void(*)()>(&ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_5_b__6_0_System_Int32),
	reinterpret_cast<void(*)()>(&ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_5_b__6_1_System_Int32_System_Int32),
	reinterpret_cast<void(*)()>(&ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_6),
	reinterpret_cast<void(*)()>(&ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_6_b__7_0_System_Int32_System_Int32),
	reinterpret_cast<void(*)()>(&ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_7),
	reinterpret_cast<void(*)()>(&ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_8),
	reinterpret_cast<void(*)()>(&ProjectionSubjects_ProjectionSubjects___c_cctor),
	reinterpret_cast<void(*)()>(&ProjectionSubjects_ProjectionSubjects___c__ctor),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
extern "C" void (*kBenchmarkWrappers[20])() = {
	[]() {kAotMethods[0]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[1])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[2]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[3])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0,0);},
	[]() {kAotMethods[4]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[5])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[6]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[7])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0,0);},
	[]() {kAotMethods[8]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[9])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0,0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[10])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0,0);},
	[]() {kAotMethods[11]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[12])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[13])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0,0);},
	[]() {kAotMethods[14]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[15])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0,0);},
	[]() {kAotMethods[16]();},
	[]() {kAotMethods[17]();},
	[]() {kAotMethods[18]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[19])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
};

// ── Subject entry index mapping ─────────────────────────────────
// Maps subject index (0-based sequential) to kAotMethod index.
// Used by runtime-entry.cpp to route --benchmark N to the correct
// AOT method slot, since kAotMethods[] includes lambdas/closures
// that shift subject methods to non-contiguous indices.
extern "C" const int kSubjectEntryCount = 9;
extern "C" const int kSubjectEntryIndices[9] = {
	0,
	2,
	4,
	6,
	8,
	11,
	14,
	16,
	17,
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

	/* ── Entry 0: ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry0;
	/* ── Entry 1: ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_0_b__1_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry1;
	/* ── Entry 2: ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry2;
	/* ── Entry 3: ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_1_b__2_0_System_Int32_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry3;
	/* ── Entry 4: ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_2 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry4;
	/* ── Entry 5: ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_2_b__3_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry5;
	/* ── Entry 6: ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_3 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry6;
	/* ── Entry 7: ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_3_b__4_0_System_Int32_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry7;
	/* ── Entry 8: ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_4 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[8];
	} entry8;
	/* ── Entry 9: ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_4_b__5_0_System_Int32_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry9;
	/* ── Entry 10: ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_4_b__5_1_System_Int32_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry10;
	/* ── Entry 11: ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_5 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[8];
	} entry11;
	/* ── Entry 12: ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_5_b__6_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry12;
	/* ── Entry 13: ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_5_b__6_1_System_Int32_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry13;
	/* ── Entry 14: ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_6 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[10];
	} entry14;
	/* ── Entry 15: ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_6_b__7_0_System_Int32_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry15;
	/* ── Entry 16: ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_7 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[9];
	} entry16;
	/* ── Entry 17: ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_8 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[11];
	} entry17;
	/* ── Entry 18: ProjectionSubjects_ProjectionSubjects___c_cctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry18;
	/* ── Entry 19: ProjectionSubjects_ProjectionSubjects___c__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry19;
} kChaosGcSlotMapsSection = {
	/* entry0 = ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_0 */
	.entry0 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_0),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry1 = ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_0_b__1_0_System_Int32 */
	.entry1 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_0_b__1_0_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 16u, 24u }
	},
	/* entry2 = ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_1 */
	.entry2 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_1),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry3 = ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_1_b__2_0_System_Int32_System_Int32 */
	.entry3 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_1_b__2_0_System_Int32_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 24u, 32u }
	},
	/* entry4 = ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_2 */
	.entry4 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_2),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry5 = ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_2_b__3_0_System_Int32 */
	.entry5 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_2_b__3_0_System_Int32),
		/* frame_size = 64 */ 64u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 16u, 24u, 32u, 40u, 48u, 56u }
	},
	/* entry6 = ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_3 */
	.entry6 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_3),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry7 = ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_3_b__4_0_System_Int32_System_Int32 */
	.entry7 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_3_b__4_0_System_Int32_System_Int32),
		/* frame_size = 64 */ 64u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 24u, 32u, 40u, 48u, 56u }
	},
	/* entry8 = ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_4 */
	.entry8 = {
		/* entry_total_size = 52 */ 52u,
		/* code_address */ reinterpret_cast<const void*>(&ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_4),
		/* frame_size = 64 */ 64u,
		/* num_gc_slots = 8 */ 8u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u }
	},
	/* entry9 = ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_4_b__5_0_System_Int32_System_Int32 */
	.entry9 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_4_b__5_0_System_Int32_System_Int32),
		/* frame_size = 64 */ 64u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 24u, 32u, 40u, 48u, 56u }
	},
	/* entry10 = ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_4_b__5_1_System_Int32_System_Int32 */
	.entry10 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_4_b__5_1_System_Int32_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 24u, 32u }
	},
	/* entry11 = ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_5 */
	.entry11 = {
		/* entry_total_size = 52 */ 52u,
		/* code_address */ reinterpret_cast<const void*>(&ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_5),
		/* frame_size = 64 */ 64u,
		/* num_gc_slots = 8 */ 8u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u }
	},
	/* entry12 = ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_5_b__6_0_System_Int32 */
	.entry12 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_5_b__6_0_System_Int32),
		/* frame_size = 64 */ 64u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 16u, 24u, 32u, 40u, 48u, 56u }
	},
	/* entry13 = ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_5_b__6_1_System_Int32_System_Int32 */
	.entry13 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_5_b__6_1_System_Int32_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 24u, 32u }
	},
	/* entry14 = ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_6 */
	.entry14 = {
		/* entry_total_size = 60 */ 60u,
		/* code_address */ reinterpret_cast<const void*>(&ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_6),
		/* frame_size = 80 */ 80u,
		/* num_gc_slots = 10 */ 10u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u, 72u }
	},
	/* entry15 = ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_6_b__7_0_System_Int32_System_Int32 */
	.entry15 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_6_b__7_0_System_Int32_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 24u, 32u }
	},
	/* entry16 = ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_7 */
	.entry16 = {
		/* entry_total_size = 56 */ 56u,
		/* code_address */ reinterpret_cast<const void*>(&ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_7),
		/* frame_size = 72 */ 72u,
		/* num_gc_slots = 9 */ 9u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u }
	},
	/* entry17 = ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_8 */
	.entry17 = {
		/* entry_total_size = 64 */ 64u,
		/* code_address */ reinterpret_cast<const void*>(&ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_8),
		/* frame_size = 88 */ 88u,
		/* num_gc_slots = 11 */ 11u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u, 72u, 80u }
	},
	/* entry18 = ProjectionSubjects_ProjectionSubjects___c_cctor */
	.entry18 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ProjectionSubjects_ProjectionSubjects___c_cctor),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry19 = ProjectionSubjects_ProjectionSubjects___c__ctor */
	.entry19 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ProjectionSubjects_ProjectionSubjects___c__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 872u;

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[20] = {
	reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_0),
	reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_0_b__1_0_System_Int32),
	reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_1),
	reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_1_b__2_0_System_Int32_System_Int32),
	reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_2),
	reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_2_b__3_0_System_Int32),
	reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_3),
	reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_3_b__4_0_System_Int32_System_Int32),
	reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_4),
	reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_4_b__5_0_System_Int32_System_Int32),
	reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_4_b__5_1_System_Int32_System_Int32),
	reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_5),
	reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_5_b__6_0_System_Int32),
	reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_5_b__6_1_System_Int32_System_Int32),
	reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_6),
	reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_6_b__7_0_System_Int32_System_Int32),
	reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_7),
	reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_8),
	reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects___c_cctor),
	reinterpret_cast<void*>(&ProjectionSubjects_ProjectionSubjects___c__ctor),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
// ── VTable descriptors (for BootstrapRuntime TypeVTable registration) ──
static const VTableDescriptorV0 kChaosVTableDescriptors[] = {
	{
		CHAOS_IL2CPP_UINT64_C(0xBEA70F2A3989D43E),
		0x00000002u,
		0u,
		10u,
		reinterpret_cast<const void*>(kSlots_ProjectionSubjects_ProjectionSubjects___c),
		reinterpret_cast<const void**>(chaos_vtable_ProjectionSubjects_ProjectionSubjects___c),
		10u,
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
	.method_pointer_count      = 20u,
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
	.image_name_utf8    = "ProjectionSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_ProjectionSubjects_ProjectionSubjects[1] = {
	{ 0u, "ProjectionSubjects/ProjectionSubjects::_exitCode", "_exitCode", "System.Int32", 0LL },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_ProjectionSubjects_ProjectionSubjects[9] = {
	{ 0u, "ProjectionSubjects/ProjectionSubjects::CustomEntrySubject_0:System.Void()", "CustomEntrySubject_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ProjectionSubjects/ProjectionSubjects::CustomEntrySubject_1:System.Void()", "CustomEntrySubject_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "ProjectionSubjects/ProjectionSubjects::CustomEntrySubject_2:System.Void()", "CustomEntrySubject_2", "System.Void", 0, nullptr, 0u },
	{ 0u, "ProjectionSubjects/ProjectionSubjects::CustomEntrySubject_3:System.Void()", "CustomEntrySubject_3", "System.Void", 0, nullptr, 0u },
	{ 0u, "ProjectionSubjects/ProjectionSubjects::CustomEntrySubject_4:System.Void()", "CustomEntrySubject_4", "System.Void", 0, nullptr, 0u },
	{ 0u, "ProjectionSubjects/ProjectionSubjects::CustomEntrySubject_5:System.Void()", "CustomEntrySubject_5", "System.Void", 0, nullptr, 0u },
	{ 0u, "ProjectionSubjects/ProjectionSubjects::CustomEntrySubject_6:System.Void()", "CustomEntrySubject_6", "System.Void", 0, nullptr, 0u },
	{ 0u, "ProjectionSubjects/ProjectionSubjects::CustomEntrySubject_7:System.Void()", "CustomEntrySubject_7", "System.Void", 0, nullptr, 0u },
	{ 0u, "ProjectionSubjects/ProjectionSubjects::CustomEntrySubject_8:System.Void()", "CustomEntrySubject_8", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_ProjectionSubjects_ProjectionSubjects___c[11] = {
	{ 0u, "ProjectionSubjects/ProjectionSubjects+<>c::<CustomEntrySubject_0>b__1_0:System.Int32(System.Int32)", "<CustomEntrySubject_0>b__1_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ProjectionSubjects/ProjectionSubjects+<>c::<CustomEntrySubject_1>b__2_0:System.Int32(System.Int32,System.Int32)", "<CustomEntrySubject_1>b__2_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ProjectionSubjects/ProjectionSubjects+<>c::<CustomEntrySubject_2>b__3_0:System.Collections.Generic.IEnumerable<System.Int32>(System.Int32)", "<CustomEntrySubject_2>b__3_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ProjectionSubjects/ProjectionSubjects+<>c::<CustomEntrySubject_3>b__4_0:System.Collections.Generic.IEnumerable<System.Int32>(System.Int32,System.Int32)", "<CustomEntrySubject_3>b__4_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ProjectionSubjects/ProjectionSubjects+<>c::<CustomEntrySubject_4>b__5_0:System.Collections.Generic.IEnumerable<System.Int32>(System.Int32,System.Int32)", "<CustomEntrySubject_4>b__5_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ProjectionSubjects/ProjectionSubjects+<>c::<CustomEntrySubject_4>b__5_1:System.Int32(System.Int32,System.Int32)", "<CustomEntrySubject_4>b__5_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "ProjectionSubjects/ProjectionSubjects+<>c::<CustomEntrySubject_5>b__6_0:System.Collections.Generic.IEnumerable<System.Int32>(System.Int32)", "<CustomEntrySubject_5>b__6_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ProjectionSubjects/ProjectionSubjects+<>c::<CustomEntrySubject_5>b__6_1:System.Int32(System.Int32,System.Int32)", "<CustomEntrySubject_5>b__6_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "ProjectionSubjects/ProjectionSubjects+<>c::<CustomEntrySubject_6>b__7_0:System.Int32(System.Int32,System.Int32)", "<CustomEntrySubject_6>b__7_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ProjectionSubjects/ProjectionSubjects+<>c::.cctor:System.Void()", ".cctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "ProjectionSubjects/ProjectionSubjects+<>c::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[2] = {
	{ 0u, "ProjectionSubjects/ProjectionSubjects", "ProjectionSubjects/ProjectionSubjects", "", "ProjectionSubjects", "ProjectionSubjects", nullptr, kReflFields_ProjectionSubjects_ProjectionSubjects, 1u, nullptr, 0u,
	kReflMethods_ProjectionSubjects_ProjectionSubjects, 9u },
	{ 0u, "ProjectionSubjects/ProjectionSubjects+<>c", "ProjectionSubjects/ProjectionSubjects+<>c", "", "ProjectionSubjects+<>c", "ProjectionSubjects+<>c", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_ProjectionSubjects_ProjectionSubjects___c, 11u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[2] = {
	&kReflTypes[0],
	&kReflTypes[1],
};

static const ReflectionQueryImageDescriptor kReflImage = { "ProjectionSubjects", kReflTypePtrs, 2u };

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
// Managed method: ProjectionSubjects/ProjectionSubjects::CustomEntrySubject_0()
extern "C" void ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_0(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(74387353u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_ProjectionSubjects_ProjectionSubjects___c();
	_s1 = chaos_static_ProjectionSubjects_ProjectionSubjects___c____9__1_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_ProjectionSubjects_ProjectionSubjects___c();
			_s1 = chaos_static_ProjectionSubjects_ProjectionSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 1)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INT32 {
					auto& _d_entry = s_hotpatch_entries[1];
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
			chaos_ensure_type_initialized_ProjectionSubjects_ProjectionSubjects___c();
				chaos_static_ProjectionSubjects_ProjectionSubjects___c____9__1_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[6])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		chaos_locals[1] = _s4;
		_s4 = chaos_locals[1];
		{
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[7])();
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		chaos_locals[2] = _s5;
		_s5 = chaos_locals[2];
		{
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[8])();
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: ProjectionSubjects/ProjectionSubjects+<>c::<CustomEntrySubject_0>b__1_0(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_0_b__1_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapMul(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ProjectionSubjects/ProjectionSubjects::CustomEntrySubject_1()
extern "C" void ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_1(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(77829180u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_ProjectionSubjects_ProjectionSubjects___c();
	_s1 = chaos_static_ProjectionSubjects_ProjectionSubjects___c____9__2_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_ProjectionSubjects_ProjectionSubjects___c();
			_s1 = chaos_static_ProjectionSubjects_ProjectionSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 3)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2) -> CHAOS_IL2CPP_INT32 {
					auto& _d_entry = s_hotpatch_entries[3];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[16];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
						_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_fn_arg_1));
						_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_fn_arg_2));
						CHAOS_IL2CPP_INT32 _d_ret{};
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, &_d_ret);
						return _d_ret;
					}
					return reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32, CHAOS_IL2CPP_INT32)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1, chaos_fn_arg_2);
				};
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s2;
				const auto chaos_target = _s1;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Int32_System_Int32_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Int32_System_Int32_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_ProjectionSubjects_ProjectionSubjects___c();
				chaos_static_ProjectionSubjects_ProjectionSubjects___c____9__2_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[12])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		chaos_locals[1] = _s4;
		_s4 = chaos_locals[1];
		{
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[7])();
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		chaos_locals[2] = _s5;
		_s5 = chaos_locals[2];
		{
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[8])();
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: ProjectionSubjects/ProjectionSubjects+<>c::<CustomEntrySubject_1>b__2_0(System.Int32,System.Int32)
extern "C" CHAOS_IL2CPP_INT32 ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_1_b__2_0_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2)
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

// Managed method: ProjectionSubjects/ProjectionSubjects::CustomEntrySubject_2()
extern "C" void ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_2(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(74387353u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_ProjectionSubjects_ProjectionSubjects___c();
	_s1 = chaos_static_ProjectionSubjects_ProjectionSubjects___c____9__3_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_ProjectionSubjects_ProjectionSubjects___c();
			_s1 = chaos_static_ProjectionSubjects_ProjectionSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 5)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
					auto& _d_entry = s_hotpatch_entries[5];
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
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Collections_Generic_IEnumerable_System_Int32__, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Collections_Generic_IEnumerable_System_Int32__.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_ProjectionSubjects_ProjectionSubjects___c();
				chaos_static_ProjectionSubjects_ProjectionSubjects___c____9__3_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[15])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		chaos_locals[1] = _s4;
		_s4 = chaos_locals[1];
		{
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[7])();
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		chaos_locals[2] = _s5;
		_s5 = chaos_locals[2];
		{
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[8])();
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: ProjectionSubjects/ProjectionSubjects+<>c::<CustomEntrySubject_2>b__3_0(System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_2_b__3_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
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
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

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
	_s3 = chaos_args[1];
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
	_s3 = chaos_args[1];
	_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(10);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapMul(static_cast<CHAOS_IL2CPP_INT32>(_s3), static_cast<CHAOS_IL2CPP_INT32>(_s4)));
	{
		auto chaos_value_raw = _s3;
		const auto chaos_value = chaos_value_raw;
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s2);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s1);
		chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)] = chaos_value;
	}
	return _s0;
}

// Managed method: ProjectionSubjects/ProjectionSubjects::CustomEntrySubject_3()
extern "C" void ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_3(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(74387353u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_ProjectionSubjects_ProjectionSubjects___c();
	_s1 = chaos_static_ProjectionSubjects_ProjectionSubjects___c____9__4_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_ProjectionSubjects_ProjectionSubjects___c();
			_s1 = chaos_static_ProjectionSubjects_ProjectionSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 7)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2) -> CHAOS_IL2CPP_INTPTR {
					auto& _d_entry = s_hotpatch_entries[7];
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
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Int32_System_Collections_Generic_IEnumerable_System_Int32__, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Int32_System_Collections_Generic_IEnumerable_System_Int32__.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_ProjectionSubjects_ProjectionSubjects___c();
				chaos_static_ProjectionSubjects_ProjectionSubjects___c____9__4_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[18])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		chaos_locals[1] = _s4;
		_s4 = chaos_locals[1];
		{
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[7])();
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		chaos_locals[2] = _s5;
		_s5 = chaos_locals[2];
		{
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[8])();
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: ProjectionSubjects/ProjectionSubjects+<>c::<CustomEntrySubject_3>b__4_0(System.Int32,System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_3_b__4_0_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_args{};
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
	chaos_args[2] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_2);

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
	_s3 = chaos_args[1];
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
	_s3 = chaos_args[2];
	{
		auto chaos_value_raw = _s3;
		const auto chaos_value = chaos_value_raw;
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s2);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s1);
		chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)] = chaos_value;
	}
	return _s0;
}

// Managed method: ProjectionSubjects/ProjectionSubjects::CustomEntrySubject_4()
extern "C" void ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_4(void)
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
	CHAOS_IL2CPP_INTPTR _s16{};
	CHAOS_IL2CPP_INTPTR _s17{};
	CHAOS_IL2CPP_INTPTR _s18{};
	CHAOS_IL2CPP_INTPTR _s19{};
	CHAOS_IL2CPP_INTPTR _s20{};
	CHAOS_IL2CPP_INTPTR _s21{};


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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(74387353u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_ProjectionSubjects_ProjectionSubjects___c();
	_s1 = chaos_static_ProjectionSubjects_ProjectionSubjects___c____9__5_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_ProjectionSubjects_ProjectionSubjects___c();
			_s1 = chaos_static_ProjectionSubjects_ProjectionSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 9)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2) -> CHAOS_IL2CPP_INTPTR {
					auto& _d_entry = s_hotpatch_entries[9];
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
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Int32_System_Collections_Generic_IEnumerable_System_Int32__, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Int32_System_Collections_Generic_IEnumerable_System_Int32__.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_ProjectionSubjects_ProjectionSubjects___c();
				chaos_static_ProjectionSubjects_ProjectionSubjects___c____9__5_0 = chaos_value;
			}
		}
		chaos_ensure_type_initialized_ProjectionSubjects_ProjectionSubjects___c();
		_s6 = chaos_static_ProjectionSubjects_ProjectionSubjects___c____9__5_1;
		_s7 = _s6;
		{
			if (_s7 == 0)
			{
				chaos_ensure_type_initialized_ProjectionSubjects_ProjectionSubjects___c();
				_s6 = chaos_static_ProjectionSubjects_ProjectionSubjects___c____9;
				{
					// Hotpatch-aware ldftn wrapper (slot 10)
					static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2) -> CHAOS_IL2CPP_INT32 {
						auto& _d_entry = s_hotpatch_entries[10];
						if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
							&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
						{
							alignas(16) uint8_t _d_ab[16];
							ArgBuffer _d_bw(_d_ab);
							_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
							_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_fn_arg_1));
							_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_fn_arg_2));
							CHAOS_IL2CPP_INT32 _d_ret{};
							::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
								_d_entry.method_key, _d_ab, &_d_ret);
							return _d_ret;
						}
						return reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32, CHAOS_IL2CPP_INT32)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1, chaos_fn_arg_2);
					};
				_s7 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
				}
				{
					const auto chaos_method_ptr = _s7;
					const auto chaos_target = _s6;
					auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Int32_System_Int32_, {});
					chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Int32_System_Int32_.hot;
					chaos_object->chaos_delegate_target = chaos_target;
					chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
					_s6 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
				}
				_s7 = _s6;
				{
					auto chaos_value = _s7;
				chaos_ensure_type_initialized_ProjectionSubjects_ProjectionSubjects___c();
					chaos_static_ProjectionSubjects_ProjectionSubjects___c____9__5_1 = chaos_value;
				}
			}
			{
				const auto chaos_arg_2 = _s6;
				const auto chaos_arg_1 = _s5;
				const auto chaos_arg_0 = _s4;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[21])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
				_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[1] = _s4;
			_s4 = chaos_locals[1];
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[7])();
				_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[2] = _s5;
			_s5 = chaos_locals[2];
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[8])();
				_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			return;
		}
	}
}

// Managed method: ProjectionSubjects/ProjectionSubjects+<>c::<CustomEntrySubject_4>b__5_0(System.Int32,System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_4_b__5_0_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_args{};
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
	chaos_args[2] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_2);

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
	_s3 = chaos_args[1];
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
	_s3 = chaos_args[2];
	{
		auto chaos_value_raw = _s3;
		const auto chaos_value = chaos_value_raw;
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s2);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s1);
		chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)] = chaos_value;
	}
	return _s0;
}

// Managed method: ProjectionSubjects/ProjectionSubjects+<>c::<CustomEntrySubject_4>b__5_1(System.Int32,System.Int32)
extern "C" CHAOS_IL2CPP_INT32 ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_4_b__5_1_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2)
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

// Managed method: ProjectionSubjects/ProjectionSubjects::CustomEntrySubject_5()
extern "C" void ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_5(void)
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
	CHAOS_IL2CPP_INTPTR _s16{};
	CHAOS_IL2CPP_INTPTR _s17{};
	CHAOS_IL2CPP_INTPTR _s18{};
	CHAOS_IL2CPP_INTPTR _s19{};
	CHAOS_IL2CPP_INTPTR _s20{};
	CHAOS_IL2CPP_INTPTR _s21{};


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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(74387353u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_ProjectionSubjects_ProjectionSubjects___c();
	_s1 = chaos_static_ProjectionSubjects_ProjectionSubjects___c____9__6_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_ProjectionSubjects_ProjectionSubjects___c();
			_s1 = chaos_static_ProjectionSubjects_ProjectionSubjects___c____9;
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
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Collections_Generic_IEnumerable_System_Int32__, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Collections_Generic_IEnumerable_System_Int32__.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_ProjectionSubjects_ProjectionSubjects___c();
				chaos_static_ProjectionSubjects_ProjectionSubjects___c____9__6_0 = chaos_value;
			}
		}
		chaos_ensure_type_initialized_ProjectionSubjects_ProjectionSubjects___c();
		_s6 = chaos_static_ProjectionSubjects_ProjectionSubjects___c____9__6_1;
		_s7 = _s6;
		{
			if (_s7 == 0)
			{
				chaos_ensure_type_initialized_ProjectionSubjects_ProjectionSubjects___c();
				_s6 = chaos_static_ProjectionSubjects_ProjectionSubjects___c____9;
				{
					// Hotpatch-aware ldftn wrapper (slot 13)
					static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2) -> CHAOS_IL2CPP_INT32 {
						auto& _d_entry = s_hotpatch_entries[13];
						if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
							&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
						{
							alignas(16) uint8_t _d_ab[16];
							ArgBuffer _d_bw(_d_ab);
							_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
							_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_fn_arg_1));
							_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_fn_arg_2));
							CHAOS_IL2CPP_INT32 _d_ret{};
							::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
								_d_entry.method_key, _d_ab, &_d_ret);
							return _d_ret;
						}
						return reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32, CHAOS_IL2CPP_INT32)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1, chaos_fn_arg_2);
					};
				_s7 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
				}
				{
					const auto chaos_method_ptr = _s7;
					const auto chaos_target = _s6;
					auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Int32_System_Int32_, {});
					chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Int32_System_Int32_.hot;
					chaos_object->chaos_delegate_target = chaos_target;
					chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
					_s6 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
				}
				_s7 = _s6;
				{
					auto chaos_value = _s7;
				chaos_ensure_type_initialized_ProjectionSubjects_ProjectionSubjects___c();
					chaos_static_ProjectionSubjects_ProjectionSubjects___c____9__6_1 = chaos_value;
				}
			}
			{
				const auto chaos_arg_2 = _s6;
				const auto chaos_arg_1 = _s5;
				const auto chaos_arg_0 = _s4;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[24])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
				_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[1] = _s4;
			_s4 = chaos_locals[1];
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[7])();
				_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[2] = _s5;
			_s5 = chaos_locals[2];
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[8])();
				_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			return;
		}
	}
}

// Managed method: ProjectionSubjects/ProjectionSubjects+<>c::<CustomEntrySubject_5>b__6_0(System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_5_b__6_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
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
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

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
	_s3 = chaos_args[1];
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
	_s3 = chaos_args[1];
	_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(10);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapMul(static_cast<CHAOS_IL2CPP_INT32>(_s3), static_cast<CHAOS_IL2CPP_INT32>(_s4)));
	{
		auto chaos_value_raw = _s3;
		const auto chaos_value = chaos_value_raw;
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s2);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s1);
		chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)] = chaos_value;
	}
	return _s0;
}

// Managed method: ProjectionSubjects/ProjectionSubjects+<>c::<CustomEntrySubject_5>b__6_1(System.Int32,System.Int32)
extern "C" CHAOS_IL2CPP_INT32 ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_5_b__6_1_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2)
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

// Managed method: ProjectionSubjects/ProjectionSubjects::CustomEntrySubject_6()
extern "C" void ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_6(void)
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
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};
	CHAOS_IL2CPP_INTPTR _s17{};
	CHAOS_IL2CPP_INTPTR _s18{};
	CHAOS_IL2CPP_INTPTR _s19{};
	CHAOS_IL2CPP_INTPTR _s20{};


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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(74387353u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_locals[0] = _s0;
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(77829180u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[0];
	_s1 = chaos_locals[1];
	chaos_ensure_type_initialized_ProjectionSubjects_ProjectionSubjects___c();
	_s2 = chaos_static_ProjectionSubjects_ProjectionSubjects___c____9__7_0;
	_s3 = _s2;
	{
		if (_s3 == 0)
		{
			chaos_ensure_type_initialized_ProjectionSubjects_ProjectionSubjects___c();
			_s2 = chaos_static_ProjectionSubjects_ProjectionSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 15)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2) -> CHAOS_IL2CPP_INT32 {
					auto& _d_entry = s_hotpatch_entries[15];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[16];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
						_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_fn_arg_1));
						_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_fn_arg_2));
						CHAOS_IL2CPP_INT32 _d_ret{};
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, &_d_ret);
						return _d_ret;
					}
					return reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32, CHAOS_IL2CPP_INT32)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1, chaos_fn_arg_2);
				};
			_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s3;
				const auto chaos_target = _s2;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Int32_System_Int32_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Int32_System_Int32_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s3 = _s2;
			{
				auto chaos_value = _s3;
			chaos_ensure_type_initialized_ProjectionSubjects_ProjectionSubjects___c();
				chaos_static_ProjectionSubjects_ProjectionSubjects___c____9__7_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_2 = _s10;
			const auto chaos_arg_1 = _s9;
			const auto chaos_arg_0 = _s8;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[26])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
			_s8 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		chaos_locals[2] = _s8;
		_s8 = chaos_locals[2];
		{
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[7])();
			_s9 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		chaos_locals[3] = _s9;
		_s9 = chaos_locals[3];
		{
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[8])();
			_s10 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: ProjectionSubjects/ProjectionSubjects+<>c::<CustomEntrySubject_6>b__7_0(System.Int32,System.Int32)
extern "C" CHAOS_IL2CPP_INT32 ProjectionSubjects_ProjectionSubjects___c_CustomEntrySubject_6_b__7_0_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2)
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

// Managed method: ProjectionSubjects/ProjectionSubjects::CustomEntrySubject_7()
extern "C" void ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_7(void)
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
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(74387353u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_locals[0] = _s0;
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(77829180u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[0];
	_s1 = chaos_locals[1];
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[27])(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[2] = _s0;
	_s0 = chaos_locals[2];
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[28])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[3] = _s1;
	_s1 = chaos_locals[3];
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[8])();
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: ProjectionSubjects/ProjectionSubjects::CustomEntrySubject_8()
extern "C" void ProjectionSubjects_ProjectionSubjects_CustomEntrySubject_8(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_locals{};
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(74387353u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_locals[0] = _s0;
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(77829180u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_locals[1] = _s0;
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(81008348u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_locals[2] = _s0;
	_s0 = chaos_locals[0];
	_s1 = chaos_locals[1];
	_s2 = chaos_locals[2];
	{
		const auto chaos_arg_2 = _s2;
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[30])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[3] = _s0;
	_s0 = chaos_locals[3];
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[31])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[4] = _s1;
	_s1 = chaos_locals[4];
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[8])();
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: ProjectionSubjects/ProjectionSubjects+<>c::.cctor()
extern "C" void ProjectionSubjects_ProjectionSubjects___c_cctor(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};

	chaos_ensure_type_initialized_ProjectionSubjects_ProjectionSubjects___c();

	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_ProjectionSubjects_ProjectionSubjects___c, {});
		chaos_object->header.type_info = &chaos_mt_ProjectionSubjects_ProjectionSubjects___c.hot;
		ProjectionSubjects_ProjectionSubjects___c__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	{
		auto chaos_value = _s0;
	chaos_ensure_type_initialized_ProjectionSubjects_ProjectionSubjects___c();
		chaos_static_ProjectionSubjects_ProjectionSubjects___c____9 = chaos_value;
	}
	return;
}

// Managed method: ProjectionSubjects/ProjectionSubjects+<>c::.ctor()
extern "C" void ProjectionSubjects_ProjectionSubjects___c__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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



}  // namespace chaos::il2cpp::codegen::ProjectionSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 20;

extern "C" void ChaosJitRegisterAll() {}