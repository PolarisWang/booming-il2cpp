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

namespace chaos::il2cpp::codegen::AggregationSubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


const void* chaos_vtable_AggregationSubjects_AggregationSubjects___c[];
inline TypeInfoV0 chaos_mt_AggregationSubjects__PrivateImplementationDetails_ = {{nullptr, nullptr, 14270401547070425600ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_AggregationSubjects__PrivateImplementationDetails_ = static_cast<CHAOS_IL2CPP_INTPTR>(14270401547070425600ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Object = {{nullptr, nullptr, 15228727185366376748ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_INTPTR>(15228727185366376748ULL);
inline TypeInfoV0 chaos_mt_AggregationSubjects_AggregationSubjects___c = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_AggregationSubjects_AggregationSubjects___c, 1143199702210551904ULL, 15u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_AggregationSubjects_AggregationSubjects___c = static_cast<CHAOS_IL2CPP_INTPTR>(1143199702210551904ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Collections_Generic_KeyValuePair_System_Char_System_Int32_ = {{nullptr, nullptr, 12173462286921513749ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Collections_Generic_KeyValuePair_System_Char_System_Int32_ = static_cast<CHAOS_IL2CPP_INTPTR>(12173462286921513749ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Delegate = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, nullptr, 7451128447593600616ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Delegate = static_cast<CHAOS_IL2CPP_INTPTR>(7451128447593600616ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_MulticastDelegate = {{&chaos_mt_System_Private_CoreLib_System_Delegate.hot, nullptr, 6681393039041505440ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_MulticastDelegate = static_cast<CHAOS_IL2CPP_INTPTR>(6681393039041505440ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Func_IGrouping_System_Char_System_String__System_Collections_Generic_KeyValuePair_System_Char_System_Int32__ = {{&chaos_mt_System_Private_CoreLib_System_MulticastDelegate.hot, nullptr, 17192082924382063223ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Func_IGrouping_System_Char_System_String__System_Collections_Generic_KeyValuePair_System_Char_System_Int32__ = static_cast<CHAOS_IL2CPP_INTPTR>(17192082924382063223ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Boolean_ = {{&chaos_mt_System_Private_CoreLib_System_MulticastDelegate.hot, nullptr, 6749479132849532215ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Func_System_Int32_System_Boolean_ = static_cast<CHAOS_IL2CPP_INTPTR>(6749479132849532215ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Int32_System_Int32_ = {{&chaos_mt_System_Private_CoreLib_System_MulticastDelegate.hot, nullptr, 2159441345668172316ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Func_System_Int32_System_Int32_System_Int32_ = static_cast<CHAOS_IL2CPP_INTPTR>(2159441345668172316ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_String_System_Int32_ = {{&chaos_mt_System_Private_CoreLib_System_MulticastDelegate.hot, nullptr, 6787385182411023715ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Func_System_Int32_System_String_System_Int32_ = static_cast<CHAOS_IL2CPP_INTPTR>(6787385182411023715ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_String_ = {{&chaos_mt_System_Private_CoreLib_System_MulticastDelegate.hot, nullptr, 1829495092403028206ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Func_System_Int32_System_String_ = static_cast<CHAOS_IL2CPP_INTPTR>(1829495092403028206ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Func_System_String_System_Char_ = {{&chaos_mt_System_Private_CoreLib_System_MulticastDelegate.hot, nullptr, 2082649114719688082ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Func_System_String_System_Char_ = static_cast<CHAOS_IL2CPP_INTPTR>(2082649114719688082ULL);
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
extern "C" void AggregationSubjects_AggregationSubjects___c__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_0_b__1_0_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INT32 AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_1_b__2_0_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INT32 AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_2_b__3_0_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INTPTR AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_2_b__3_1_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_3_b__4_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_3_b__4_1_IGrouping_System_Char_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_3_b__4_2_System_Int32_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INTPTR AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_4_b__5_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_4_b__5_1_IGrouping_System_Char_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_4_b__5_2_System_Int32_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INTPTR AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_6_b__7_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_7_b__8_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_7_b__8_1_IGrouping_System_Char_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_9_b__10_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
const void* chaos_vtable_AggregationSubjects_AggregationSubjects___c[] =
{
	reinterpret_cast<void*>(AggregationSubjects_AggregationSubjects___c__ctor),
	reinterpret_cast<void*>(AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_0_b__1_0_System_Int32_System_Int32),
	reinterpret_cast<void*>(AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_1_b__2_0_System_Int32_System_Int32),
	reinterpret_cast<void*>(AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_2_b__3_0_System_Int32_System_Int32),
	reinterpret_cast<void*>(AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_2_b__3_1_System_Int32),
	reinterpret_cast<void*>(AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_3_b__4_0_System_String),
	reinterpret_cast<void*>(AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_3_b__4_1_IGrouping_System_Char_System_String),
	reinterpret_cast<void*>(AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_3_b__4_2_System_Int32_System_String),
	reinterpret_cast<void*>(AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_4_b__5_0_System_String),
	reinterpret_cast<void*>(AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_4_b__5_1_IGrouping_System_Char_System_String),
	reinterpret_cast<void*>(AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_4_b__5_2_System_Int32_System_String),
	reinterpret_cast<void*>(AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_6_b__7_0_System_Int32),
	reinterpret_cast<void*>(AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_7_b__8_0_System_String),
	reinterpret_cast<void*>(AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_7_b__8_1_IGrouping_System_Char_System_String),
	reinterpret_cast<void*>(AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_9_b__10_0_System_Int32),
};
static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_AggregationSubjects_AggregationSubjects___c[] =
{
	{ 0x00000027u, reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c__ctor) },
	{ 0x00000028u, reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_0_b__1_0_System_Int32_System_Int32) },
	{ 0x00000029u, reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_1_b__2_0_System_Int32_System_Int32) },
	{ 0x0000002Au, reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_2_b__3_0_System_Int32_System_Int32) },
	{ 0x0000002Bu, reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_2_b__3_1_System_Int32) },
	{ 0x0000002Cu, reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_3_b__4_0_System_String) },
	{ 0x0000002Du, reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_3_b__4_1_IGrouping_System_Char_System_String) },
	{ 0x0000002Eu, reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_3_b__4_2_System_Int32_System_String) },
	{ 0x0000002Fu, reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_4_b__5_0_System_String) },
	{ 0x00000030u, reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_4_b__5_1_IGrouping_System_Char_System_String) },
	{ 0x00000031u, reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_4_b__5_2_System_Int32_System_String) },
	{ 0x00000032u, reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_6_b__7_0_System_Int32) },
	{ 0x00000033u, reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_7_b__8_0_System_String) },
	{ 0x00000034u, reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_7_b__8_1_IGrouping_System_Char_System_String) },
	{ 0x00000035u, reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_9_b__10_0_System_Int32) },
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

struct chaos_type_AggregationSubjects__PrivateImplementationDetails_
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR field_AggregationSubjects__PrivateImplementationDetails___272BC3456B7CE85DE2CE18D1964316879E840A1201A4664E967EF42BA3F76B96 = 0;
	CHAOS_IL2CPP_INTPTR field_AggregationSubjects__PrivateImplementationDetails___4636993D3E1DA4E9D6B8F87B79E8F7C6D018580D52661950EABC3845C5897A4D = 0;
	CHAOS_IL2CPP_INTPTR field_AggregationSubjects__PrivateImplementationDetails___4F6ADDC9659D6FB90FE94B6688A79F2A1FA8D36EC43F8F3E1D9B6528C448A384 = 0;
	CHAOS_IL2CPP_INTPTR field_AggregationSubjects__PrivateImplementationDetails___6E28B4D41AF8A88650219653A728E0FD6760BB2B1DFEB7A9EB8DEB5B4E7010E0 = 0;
};

struct chaos_type_System_Private_CoreLib_System_Object
{
	ThinLockableHeader header{};
};

struct chaos_type_AggregationSubjects_AggregationSubjects___c : public chaos_type_System_Private_CoreLib_System_Object
{
};

struct chaos_type_System_Private_CoreLib_System_Collections_Generic_KeyValuePair_System_Char_System_Int32_
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

struct chaos_type_System_Private_CoreLib_System_Func_IGrouping_System_Char_System_String__System_Collections_Generic_KeyValuePair_System_Char_System_Int32__ : public chaos_type_System_Private_CoreLib_System_MulticastDelegate
{
};

struct chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Boolean_ : public chaos_type_System_Private_CoreLib_System_MulticastDelegate
{
};

struct chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Int32_System_Int32_ : public chaos_type_System_Private_CoreLib_System_MulticastDelegate
{
};

struct chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_String_System_Int32_ : public chaos_type_System_Private_CoreLib_System_MulticastDelegate
{
};

struct chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_String_ : public chaos_type_System_Private_CoreLib_System_MulticastDelegate
{
};

struct chaos_type_System_Private_CoreLib_System_Func_System_String_System_Char_ : public chaos_type_System_Private_CoreLib_System_MulticastDelegate
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

CHAOS_IL2CPP_INTPTR chaos_static_AggregationSubjects_AggregationSubjects___c____9 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_AggregationSubjects_AggregationSubjects___c____9__10_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_AggregationSubjects_AggregationSubjects___c____9__1_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_AggregationSubjects_AggregationSubjects___c____9__2_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_AggregationSubjects_AggregationSubjects___c____9__3_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_AggregationSubjects_AggregationSubjects___c____9__3_1 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_AggregationSubjects_AggregationSubjects___c____9__4_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_AggregationSubjects_AggregationSubjects___c____9__4_1 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_AggregationSubjects_AggregationSubjects___c____9__4_2 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_AggregationSubjects_AggregationSubjects___c____9__5_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_AggregationSubjects_AggregationSubjects___c____9__5_1 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_AggregationSubjects_AggregationSubjects___c____9__5_2 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_AggregationSubjects_AggregationSubjects___c____9__7_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_AggregationSubjects_AggregationSubjects___c____9__8_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_AggregationSubjects_AggregationSubjects___c____9__8_1 = 0;


	// AOT-baked string table: sorted by StringId for binary search at runtime.
	constexpr chaos::il2cpp::string_table::StringEntry chaos_aot_string_entries[] = {
		{ 1519142413177129655U, "blueberry", 9u },
		{ 3806586984662747281U, "banana", 6u },
		{ 5968103895666186489U, "cherry", 6u },
		{ 8595791296793083327U, "apple", 5u },
		{ 8904889996531994591U, "avocado", 7u },
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

const CHAOS_IL2CPP_UINT8 chaos_static_field_data_AggregationSubjects__PrivateImplementationDetails___272BC3456B7CE85DE2CE18D1964316879E840A1201A4664E967EF42BA3F76B96[] =
{
	0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
	0x05, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
	0x09, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00,
};

const CHAOS_IL2CPP_UINT8 chaos_static_field_data_AggregationSubjects__PrivateImplementationDetails___4636993D3E1DA4E9D6B8F87B79E8F7C6D018580D52661950EABC3845C5897A4D[] =
{
	0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
};

const CHAOS_IL2CPP_UINT8 chaos_static_field_data_AggregationSubjects__PrivateImplementationDetails___4F6ADDC9659D6FB90FE94B6688A79F2A1FA8D36EC43F8F3E1D9B6528C448A384[] =
{
	0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
	0x05, 0x00, 0x00, 0x00,
};

const CHAOS_IL2CPP_UINT8 chaos_static_field_data_AggregationSubjects__PrivateImplementationDetails___6E28B4D41AF8A88650219653A728E0FD6760BB2B1DFEB7A9EB8DEB5B4E7010E0[] =
{
	0x0A, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00,
	0x32, 0x00, 0x00, 0x00,
};

bool chaos_try_get_static_field_data(
	CHAOS_IL2CPP_INTPTR chaos_field_handle,
	const CHAOS_IL2CPP_UINT8*& chaos_data,
	CHAOS_IL2CPP_SIZE& chaos_size)
{
	if (chaos_field_handle == static_cast<CHAOS_IL2CPP_INTPTR>(80959911u))
	{
		chaos_data = chaos_static_field_data_AggregationSubjects__PrivateImplementationDetails___272BC3456B7CE85DE2CE18D1964316879E840A1201A4664E967EF42BA3F76B96;
		chaos_size = sizeof(chaos_static_field_data_AggregationSubjects__PrivateImplementationDetails___272BC3456B7CE85DE2CE18D1964316879E840A1201A4664E967EF42BA3F76B96);
		return true;
	}
	if (chaos_field_handle == static_cast<CHAOS_IL2CPP_INTPTR>(73306142u))
	{
		chaos_data = chaos_static_field_data_AggregationSubjects__PrivateImplementationDetails___4636993D3E1DA4E9D6B8F87B79E8F7C6D018580D52661950EABC3845C5897A4D;
		chaos_size = sizeof(chaos_static_field_data_AggregationSubjects__PrivateImplementationDetails___4636993D3E1DA4E9D6B8F87B79E8F7C6D018580D52661950EABC3845C5897A4D);
		return true;
	}
	if (chaos_field_handle == static_cast<CHAOS_IL2CPP_INTPTR>(79080714u))
	{
		chaos_data = chaos_static_field_data_AggregationSubjects__PrivateImplementationDetails___4F6ADDC9659D6FB90FE94B6688A79F2A1FA8D36EC43F8F3E1D9B6528C448A384;
		chaos_size = sizeof(chaos_static_field_data_AggregationSubjects__PrivateImplementationDetails___4F6ADDC9659D6FB90FE94B6688A79F2A1FA8D36EC43F8F3E1D9B6528C448A384);
		return true;
	}
	if (chaos_field_handle == static_cast<CHAOS_IL2CPP_INTPTR>(77252407u))
	{
		chaos_data = chaos_static_field_data_AggregationSubjects__PrivateImplementationDetails___6E28B4D41AF8A88650219653A728E0FD6760BB2B1DFEB7A9EB8DEB5B4E7010E0;
		chaos_size = sizeof(chaos_static_field_data_AggregationSubjects__PrivateImplementationDetails___6E28B4D41AF8A88650219653A728E0FD6760BB2B1DFEB7A9EB8DEB5B4E7010E0);
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

static void chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers__InitializeArray_System_Void_System_Array_System_RuntimeFieldHandle_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	chaos_initialize_array_from_field_data_int32(chaos_fn_arg_0, chaos_fn_arg_1);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
	if (chaos_arg_0 == 0)
	{
		CHAOS_IL2CPP_FAIL();
	}

	if (chaos_is_string_id(chaos_arg_0))
	{
		return static_cast<CHAOS_IL2CPP_INT32>(
			chaos::il2cpp::string_table::Resolve(
				chaos_extract_string_id(chaos_arg_0)).byte_count);
	}

	auto* chaos_string = reinterpret_cast<CHAOS_IL2CPP_STRING_TYPE*>(chaos_arg_0);
	return static_cast<CHAOS_IL2CPP_INT32>(chaos_string->length);
}

CHAOS_IL2CPP_ONCE_FLAG chaos_type_init_once_AggregationSubjects_AggregationSubjects___c;

void chaos_ensure_type_initialized_AggregationSubjects_AggregationSubjects___c()
{
	CHAOS_IL2CPP_CALL_ONCE(chaos_type_init_once_AggregationSubjects_AggregationSubjects___c, []()
	{
	if (chaos_static_AggregationSubjects_AggregationSubjects___c____9 == 0)
	{
	    auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_AggregationSubjects_AggregationSubjects___c);
	    chaos_object->header.type_info = &chaos_mt_AggregationSubjects_AggregationSubjects___c.hot;
		const auto chaos_arg_0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
		AggregationSubjects_AggregationSubjects___c__ctor(chaos_arg_0);
	    chaos_static_AggregationSubjects_AggregationSubjects___c____9 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	});
}

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };

extern "C" void AggregationSubjects_AggregationSubjects_CustomEntrySubject_0(void);
extern "C" CHAOS_IL2CPP_INT32 AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_0_b__1_0_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" void AggregationSubjects_AggregationSubjects_CustomEntrySubject_1(void);
extern "C" CHAOS_IL2CPP_INT32 AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_1_b__2_0_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" void AggregationSubjects_AggregationSubjects_CustomEntrySubject_2(void);
extern "C" CHAOS_IL2CPP_INT32 AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_2_b__3_0_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INTPTR AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_2_b__3_1_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void AggregationSubjects_AggregationSubjects_CustomEntrySubject_3(void);
extern "C" CHAOS_IL2CPP_INTPTR AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_3_b__4_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_3_b__4_1_IGrouping_System_Char_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_3_b__4_2_System_Int32_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2);
extern "C" void AggregationSubjects_AggregationSubjects_CustomEntrySubject_4(void);
extern "C" CHAOS_IL2CPP_INTPTR AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_4_b__5_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_4_b__5_1_IGrouping_System_Char_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_4_b__5_2_System_Int32_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2);
extern "C" void AggregationSubjects_AggregationSubjects_CustomEntrySubject_5(void);
extern "C" void AggregationSubjects_AggregationSubjects_CustomEntrySubject_6(void);
extern "C" CHAOS_IL2CPP_INTPTR AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_6_b__7_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void AggregationSubjects_AggregationSubjects_CustomEntrySubject_7(void);
extern "C" CHAOS_IL2CPP_INTPTR AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_7_b__8_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_7_b__8_1_IGrouping_System_Char_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" void AggregationSubjects_AggregationSubjects_CustomEntrySubject_8(void);
extern "C" void AggregationSubjects_AggregationSubjects_CustomEntrySubject_9(void);
extern "C" CHAOS_IL2CPP_INTPTR AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_9_b__10_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void AggregationSubjects_AggregationSubjects_CustomEntrySubject_10(void);
extern "C" void AggregationSubjects_AggregationSubjects___c_cctor(void);
extern "C" void AggregationSubjects_AggregationSubjects___c__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[28] = {
	0u,
	0u,
	2u,
	2u,
	4u,
	4u,
	6u,
	7u,
	7u,
	8u,
	9u,
	11u,
	11u,
	12u,
	13u,
	15u,
	15u,
	15u,
	16u,
	16u,
	17u,
	18u,
	18u,
	18u,
	19u,
	19u,
	19u,
	19u,
};

static constexpr struct {
	::ChaosAbiManifestV0 header;
	::ChaosAbiMethodEntryV0 entries[27];
	CHAOS_IL2CPP_UINT8 params[19];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		27u,
		19u,
		2017715926u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // AggregationSubjects_AggregationSubjects_CustomEntrySubject_0
		{ 1u, 2u },  // AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_0_b__1_0_System_Int32_System_Int32
		{ 0u, 0u },  // AggregationSubjects_AggregationSubjects_CustomEntrySubject_1
		{ 1u, 2u },  // AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_1_b__2_0_System_Int32_System_Int32
		{ 0u, 0u },  // AggregationSubjects_AggregationSubjects_CustomEntrySubject_2
		{ 1u, 2u },  // AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_2_b__3_0_System_Int32_System_Int32
		{ 2u, 1u },  // AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_2_b__3_1_System_Int32
		{ 0u, 0u },  // AggregationSubjects_AggregationSubjects_CustomEntrySubject_3
		{ 2u, 1u },  // AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_3_b__4_0_System_String
		{ 2u, 1u },  // AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_3_b__4_1_IGrouping_System_Char_System_String
		{ 1u, 2u },  // AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_3_b__4_2_System_Int32_System_String
		{ 0u, 0u },  // AggregationSubjects_AggregationSubjects_CustomEntrySubject_4
		{ 2u, 1u },  // AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_4_b__5_0_System_String
		{ 2u, 1u },  // AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_4_b__5_1_IGrouping_System_Char_System_String
		{ 1u, 2u },  // AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_4_b__5_2_System_Int32_System_String
		{ 0u, 0u },  // AggregationSubjects_AggregationSubjects_CustomEntrySubject_5
		{ 0u, 0u },  // AggregationSubjects_AggregationSubjects_CustomEntrySubject_6
		{ 2u, 1u },  // AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_6_b__7_0_System_Int32
		{ 0u, 0u },  // AggregationSubjects_AggregationSubjects_CustomEntrySubject_7
		{ 2u, 1u },  // AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_7_b__8_0_System_String
		{ 2u, 1u },  // AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_7_b__8_1_IGrouping_System_Char_System_String
		{ 0u, 0u },  // AggregationSubjects_AggregationSubjects_CustomEntrySubject_8
		{ 0u, 0u },  // AggregationSubjects_AggregationSubjects_CustomEntrySubject_9
		{ 2u, 1u },  // AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_9_b__10_0_System_Int32
		{ 0u, 0u },  // AggregationSubjects_AggregationSubjects_CustomEntrySubject_10
		{ 0u, 0u },  // AggregationSubjects_AggregationSubjects___c_cctor
		{ 0u, 0u },  // AggregationSubjects_AggregationSubjects___c__ctor
	},
	{
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		2u,
		2u,
		1u,
		2u,
		2u,
		2u,
		1u,
		2u,
		1u,
		2u,
		2u,
		1u,
	},
};
static const ::ChaosAbiManifestV0* const s_abi_manifest =
	reinterpret_cast<const ::ChaosAbiManifestV0*>(&s_abi_manifest_storage);
// ── Module registration ──

	static constexpr CHAOS_IL2CPP_UINT32 s_type_flags[7] = {
		0u,
		1548u,
		8u,
		1544u,
		1545u,
		1545u,
		1545u,
	};

	static constexpr const char* s_type_names[7] = {
		"<Module>",
		"AggregationSubjects",
		"<PrivateImplementationDetails>",
		"<>c",
		"__StaticArrayInitTypeSize=12",
		"__StaticArrayInitTypeSize=20",
		"__StaticArrayInitTypeSize=40",
	};

	static constexpr const char* s_type_namespaces[7] = {
		"",
		"",
		"",
		"",
		"",
		"",
		"",
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_type_parent_tokens[7] = {
		0u,
		0u,
		0u,
		0u,
		0u,
		0u,
		0u,
	};

	static const TypeInfoHot* const s_type_info_ptrs[7] = {
		nullptr,
		nullptr,
		&chaos_mt_AggregationSubjects__PrivateImplementationDetails_.hot,
		&chaos_mt_AggregationSubjects_AggregationSubjects___c.hot,
		nullptr,
		nullptr,
		nullptr,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_children[4] = {
		33554436u,
		33554437u,
		33554438u,
		33554439u,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_offset[8] = {
		0u,
		0u,
		1u,
		4u,
		4u,
		4u,
		4u,
		4u,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_generic_param_constraint_offset[8] = {
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
		/* .name_utf8         = */ "AggregationSubjects",
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
		/* .type_count        = */ 7u,
	/* .custom_attribute_blob       = */ nullptr,  // Tier 1 — deferred
	/* .custom_attribute_offset     = */ nullptr,
	/* .custom_attribute_entity_count = */ 0u,
	/* .custom_attribute_materializer = */ nullptr,

		/* .abi_manifest      = */ s_abi_manifest,
	};
	static const CHAOS_IL2CPP_UINT32 s_native_aot_module_id =
		::chaos::il2cpp::runtime_core::RegisterModule("AggregationSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[27] = {
	{ "CustomEntrySubject_0", 0x0000001Bu, 0u },  // AggregationSubjects
	{ "CustomEntrySubject_1", 0x0000001Cu, 0u },  // AggregationSubjects
	{ "CustomEntrySubject_2", 0x0000001Du, 0u },  // AggregationSubjects
	{ "CustomEntrySubject_3", 0x0000001Eu, 0u },  // AggregationSubjects
	{ "CustomEntrySubject_4", 0x0000001Fu, 0u },  // AggregationSubjects
	{ "CustomEntrySubject_5", 0x00000020u, 0u },  // AggregationSubjects
	{ "CustomEntrySubject_6", 0x00000021u, 0u },  // AggregationSubjects
	{ "CustomEntrySubject_7", 0x00000022u, 0u },  // AggregationSubjects
	{ "CustomEntrySubject_8", 0x00000023u, 0u },  // AggregationSubjects
	{ "CustomEntrySubject_9", 0x00000024u, 0u },  // AggregationSubjects
	{ "CustomEntrySubject_10", 0x00000025u, 0u },  // AggregationSubjects
	{ "<CustomEntrySubject_0>b__1_0", 0x00000028u, 2u },  // AggregationSubjects+<>c
	{ "<CustomEntrySubject_1>b__2_0", 0x00000029u, 2u },  // AggregationSubjects+<>c
	{ "<CustomEntrySubject_2>b__3_0", 0x0000002Au, 2u },  // AggregationSubjects+<>c
	{ "<CustomEntrySubject_2>b__3_1", 0x0000002Bu, 1u },  // AggregationSubjects+<>c
	{ "<CustomEntrySubject_3>b__4_0", 0x0000002Cu, 1u },  // AggregationSubjects+<>c
	{ "<CustomEntrySubject_3>b__4_1", 0x0000002Du, 1u },  // AggregationSubjects+<>c
	{ "<CustomEntrySubject_3>b__4_2", 0x0000002Eu, 2u },  // AggregationSubjects+<>c
	{ "<CustomEntrySubject_4>b__5_0", 0x0000002Fu, 1u },  // AggregationSubjects+<>c
	{ "<CustomEntrySubject_4>b__5_1", 0x00000030u, 1u },  // AggregationSubjects+<>c
	{ "<CustomEntrySubject_4>b__5_2", 0x00000031u, 2u },  // AggregationSubjects+<>c
	{ "<CustomEntrySubject_6>b__7_0", 0x00000032u, 1u },  // AggregationSubjects+<>c
	{ "<CustomEntrySubject_7>b__8_0", 0x00000033u, 1u },  // AggregationSubjects+<>c
	{ "<CustomEntrySubject_7>b__8_1", 0x00000034u, 1u },  // AggregationSubjects+<>c
	{ "<CustomEntrySubject_9>b__10_0", 0x00000035u, 1u },  // AggregationSubjects+<>c
	{ ".cctor", 0x00000026u, 0u },  // AggregationSubjects+<>c
	{ ".ctor", 0x00000027u, 0u },  // AggregationSubjects+<>c
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[2] = {
	{ "AggregationSubjects", "", 0u, 11u },
	{ "AggregationSubjects+<>c", "", 11u, 16u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[27] = {
	{ 0x0000001Bu, 0u },
	{ 0x0000001Cu, 2u },
	{ 0x0000001Du, 4u },
	{ 0x0000001Eu, 7u },
	{ 0x0000001Fu, 11u },
	{ 0x00000020u, 15u },
	{ 0x00000021u, 16u },
	{ 0x00000022u, 18u },
	{ 0x00000023u, 21u },
	{ 0x00000024u, 22u },
	{ 0x00000025u, 24u },
	{ 0x00000026u, 25u },
	{ 0x00000027u, 26u },
	{ 0x00000028u, 1u },
	{ 0x00000029u, 3u },
	{ 0x0000002Au, 5u },
	{ 0x0000002Bu, 6u },
	{ 0x0000002Cu, 8u },
	{ 0x0000002Du, 9u },
	{ 0x0000002Eu, 10u },
	{ 0x0000002Fu, 12u },
	{ 0x00000030u, 13u },
	{ 0x00000031u, 14u },
	{ 0x00000032u, 17u },
	{ 0x00000033u, 19u },
	{ 0x00000034u, 20u },
	{ 0x00000035u, 23u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[27] = {
	{ reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects_CustomEntrySubject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AggregationSubjects::CustomEntrySubject_0
	{ reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_0_b__1_0_System_Int32_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AggregationSubjects+<>c::<CustomEntrySubject_0>b__1_0
	{ reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects_CustomEntrySubject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AggregationSubjects::CustomEntrySubject_1
	{ reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_1_b__2_0_System_Int32_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AggregationSubjects+<>c::<CustomEntrySubject_1>b__2_0
	{ reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects_CustomEntrySubject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AggregationSubjects::CustomEntrySubject_2
	{ reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_2_b__3_0_System_Int32_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AggregationSubjects+<>c::<CustomEntrySubject_2>b__3_0
	{ reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_2_b__3_1_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AggregationSubjects+<>c::<CustomEntrySubject_2>b__3_1
	{ reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects_CustomEntrySubject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AggregationSubjects::CustomEntrySubject_3
	{ reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_3_b__4_0_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AggregationSubjects+<>c::<CustomEntrySubject_3>b__4_0
	{ reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_3_b__4_1_IGrouping_System_Char_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AggregationSubjects+<>c::<CustomEntrySubject_3>b__4_1
	{ reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_3_b__4_2_System_Int32_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AggregationSubjects+<>c::<CustomEntrySubject_3>b__4_2
	{ reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects_CustomEntrySubject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AggregationSubjects::CustomEntrySubject_4
	{ reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_4_b__5_0_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AggregationSubjects+<>c::<CustomEntrySubject_4>b__5_0
	{ reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_4_b__5_1_IGrouping_System_Char_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AggregationSubjects+<>c::<CustomEntrySubject_4>b__5_1
	{ reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_4_b__5_2_System_Int32_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AggregationSubjects+<>c::<CustomEntrySubject_4>b__5_2
	{ reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects_CustomEntrySubject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AggregationSubjects::CustomEntrySubject_5
	{ reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects_CustomEntrySubject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AggregationSubjects::CustomEntrySubject_6
	{ reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_6_b__7_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AggregationSubjects+<>c::<CustomEntrySubject_6>b__7_0
	{ reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects_CustomEntrySubject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AggregationSubjects::CustomEntrySubject_7
	{ reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_7_b__8_0_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AggregationSubjects+<>c::<CustomEntrySubject_7>b__8_0
	{ reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_7_b__8_1_IGrouping_System_Char_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AggregationSubjects+<>c::<CustomEntrySubject_7>b__8_1
	{ reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects_CustomEntrySubject_8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AggregationSubjects::CustomEntrySubject_8
	{ reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects_CustomEntrySubject_9), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AggregationSubjects::CustomEntrySubject_9
	{ reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_9_b__10_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AggregationSubjects+<>c::<CustomEntrySubject_9>b__10_0
	{ reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects_CustomEntrySubject_10), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AggregationSubjects::CustomEntrySubject_10
	{ reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c_cctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AggregationSubjects+<>c::.cctor
	{ reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AggregationSubjects+<>c::.ctor
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"AggregationSubjects",
	s_hotpatch_types,
	2u,
	s_hotpatch_methods,
	27u,
	s_hotpatch_slots,
	27u,
	s_hotpatch_entries,
	27u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[48] = {
	"System.Private.CoreLib/System.Int32",
	"AggregationSubjects/<PrivateImplementationDetails>::4F6ADDC9659D6FB90FE94B6688A79F2A1FA8D36EC43F8F3E1D9B6528C448A384",
	"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)",
	"AggregationSubjects/AggregationSubjects+<>c::<>9__1_0",
	"AggregationSubjects/AggregationSubjects+<>c::<>9",
	"System.Private.CoreLib/System.Func<System.Int32,System.Int32,System.Int32>::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Linq/Enumerable::Aggregate<System.Int32>:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32,System.Int32>)",
	"AggregationSubjects/AggregationSubjects+<>c::<>9__2_0",
	"System.Linq/Enumerable::Aggregate<System.Int32,System.Int32>:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32,System.Func<System.Int32,System.Int32,System.Int32>)",
	"AggregationSubjects/<PrivateImplementationDetails>::4636993D3E1DA4E9D6B8F87B79E8F7C6D018580D52661950EABC3845C5897A4D",
	"AggregationSubjects/AggregationSubjects+<>c::<>9__3_0",
	"AggregationSubjects/AggregationSubjects+<>c::<>9__3_1",
	"System.Private.CoreLib/System.Func<System.Int32,System.String>::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Linq/Enumerable::Aggregate<System.Int32,System.Int32,System.String>:System.String(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32,System.Func<System.Int32,System.Int32,System.Int32>,System.Func<System.Int32,System.String>)",
	"System.Private.CoreLib/System.Int32::ToString:System.String()",
	"System.Private.CoreLib/System.String",
	"AggregationSubjects/AggregationSubjects+<>c::<>9__4_0",
	"System.Private.CoreLib/System.Func<System.String,System.Char>::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Linq/Enumerable::GroupBy<System.String,System.Char>:System.Collections.Generic.IEnumerable<IGrouping<System.Char,System.String>>(System.Collections.Generic.IEnumerable<System.String>,System.Func<System.String,System.Char>)",
	"AggregationSubjects/AggregationSubjects+<>c::<>9__4_1",
	"System.Private.CoreLib/System.Func<IGrouping<System.Char,System.String>,System.Collections.Generic.KeyValuePair<System.Char,System.Int32>>::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Linq/Enumerable::Select<IGrouping<System.Char,System.String>,System.Collections.Generic.KeyValuePair<System.Char,System.Int32>>:System.Collections.Generic.IEnumerable<System.Collections.Generic.KeyValuePair<System.Char,System.Int32>>(System.Collections.Generic.IEnumerable<IGrouping<System.Char,System.String>>,System.Func<IGrouping<System.Char,System.String>,System.Collections.Generic.KeyValuePair<System.Char,System.Int32>>)",
	"System.Private.CoreLib/System.Collections.Generic.IEnumerable<System.Collections.Generic.KeyValuePair<System.Char,System.Int32>>::GetEnumerator:System.Collections.Generic.IEnumerator<System.Collections.Generic.KeyValuePair<System.Char,System.Int32>>()",
	"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()",
	"System.Private.CoreLib/System.String::get_Chars:System.Char(System.Int32)",
	"System.Linq/IGrouping<System.Char,System.String>::get_Key:System.Char()",
	"AggregationSubjects/AggregationSubjects+<>c::<>9__4_2",
	"System.Private.CoreLib/System.Func<System.Int32,System.String,System.Int32>::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Linq/Enumerable::Aggregate<System.String,System.Int32>:System.Int32(System.Collections.Generic.IEnumerable<System.String>,System.Int32,System.Func<System.Int32,System.String,System.Int32>)",
	"System.Private.CoreLib/System.Collections.Generic.KeyValuePair<System.Char,System.Int32>::.ctor:System.Void(System.Char,System.Int32)",
	"System.Private.CoreLib/System.String::get_Length:System.Int32()",
	"AggregationSubjects/AggregationSubjects+<>c::<>9__5_0",
	"AggregationSubjects/AggregationSubjects+<>c::<>9__5_1",
	"AggregationSubjects/AggregationSubjects+<>c::<>9__5_2",
	"AggregationSubjects/<PrivateImplementationDetails>::6E28B4D41AF8A88650219653A728E0FD6760BB2B1DFEB7A9EB8DEB5B4E7010E0",
	"System.Linq/Enumerable::Count<System.Int32>:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>)",
	"AggregationSubjects/<PrivateImplementationDetails>::272BC3456B7CE85DE2CE18D1964316879E840A1201A4664E967EF42BA3F76B96",
	"AggregationSubjects/AggregationSubjects+<>c::<>9__7_0",
	"System.Private.CoreLib/System.Func<System.Int32,System.Boolean>::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Linq/Enumerable::Count<System.Int32>:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)",
	"AggregationSubjects/AggregationSubjects+<>c::<>9__8_0",
	"AggregationSubjects/AggregationSubjects+<>c::<>9__8_1",
	"System.Linq/Enumerable::Count<System.String>:System.Int32(System.Collections.Generic.IEnumerable<System.String>)",
	"System.Linq/Enumerable::LongCount<System.Int32>:System.Int64(System.Collections.Generic.IEnumerable<System.Int32>)",
	"AggregationSubjects/AggregationSubjects+<>c::<>9__10_0",
	"System.Linq/Enumerable::LongCount<System.Int32>:System.Int64(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)",
	"System.Linq/Enumerable::TryGetNonEnumeratedCount<System.Int32>:System.Boolean(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32&)",
	"System.Private.CoreLib/System.Object::.ctor:System.Void()",
};

extern "C" void* kChaosExternalRuntimeFnTable[48] = {
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
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Int32__ToString_System_String__),
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
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__),
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

extern "C" int32_t kChaosExternalRuntimeCount = 48;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[27])() = {
	reinterpret_cast<void(*)()>(&AggregationSubjects_AggregationSubjects_CustomEntrySubject_0),
	reinterpret_cast<void(*)()>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_0_b__1_0_System_Int32_System_Int32),
	reinterpret_cast<void(*)()>(&AggregationSubjects_AggregationSubjects_CustomEntrySubject_1),
	reinterpret_cast<void(*)()>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_1_b__2_0_System_Int32_System_Int32),
	reinterpret_cast<void(*)()>(&AggregationSubjects_AggregationSubjects_CustomEntrySubject_2),
	reinterpret_cast<void(*)()>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_2_b__3_0_System_Int32_System_Int32),
	reinterpret_cast<void(*)()>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_2_b__3_1_System_Int32),
	reinterpret_cast<void(*)()>(&AggregationSubjects_AggregationSubjects_CustomEntrySubject_3),
	reinterpret_cast<void(*)()>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_3_b__4_0_System_String),
	reinterpret_cast<void(*)()>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_3_b__4_1_IGrouping_System_Char_System_String),
	reinterpret_cast<void(*)()>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_3_b__4_2_System_Int32_System_String),
	reinterpret_cast<void(*)()>(&AggregationSubjects_AggregationSubjects_CustomEntrySubject_4),
	reinterpret_cast<void(*)()>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_4_b__5_0_System_String),
	reinterpret_cast<void(*)()>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_4_b__5_1_IGrouping_System_Char_System_String),
	reinterpret_cast<void(*)()>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_4_b__5_2_System_Int32_System_String),
	reinterpret_cast<void(*)()>(&AggregationSubjects_AggregationSubjects_CustomEntrySubject_5),
	reinterpret_cast<void(*)()>(&AggregationSubjects_AggregationSubjects_CustomEntrySubject_6),
	reinterpret_cast<void(*)()>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_6_b__7_0_System_Int32),
	reinterpret_cast<void(*)()>(&AggregationSubjects_AggregationSubjects_CustomEntrySubject_7),
	reinterpret_cast<void(*)()>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_7_b__8_0_System_String),
	reinterpret_cast<void(*)()>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_7_b__8_1_IGrouping_System_Char_System_String),
	reinterpret_cast<void(*)()>(&AggregationSubjects_AggregationSubjects_CustomEntrySubject_8),
	reinterpret_cast<void(*)()>(&AggregationSubjects_AggregationSubjects_CustomEntrySubject_9),
	reinterpret_cast<void(*)()>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_9_b__10_0_System_Int32),
	reinterpret_cast<void(*)()>(&AggregationSubjects_AggregationSubjects_CustomEntrySubject_10),
	reinterpret_cast<void(*)()>(&AggregationSubjects_AggregationSubjects___c_cctor),
	reinterpret_cast<void(*)()>(&AggregationSubjects_AggregationSubjects___c__ctor),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
extern "C" void (*kBenchmarkWrappers[27])() = {
	[]() {kAotMethods[0]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[1])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0,0);},
	[]() {kAotMethods[2]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[3])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0,0);},
	[]() {kAotMethods[4]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[5])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0,0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[6])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[7]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[8])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),chaos_make_string_id_value(8595791296793083327ULL));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[9])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[10])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0,chaos_make_string_id_value(8595791296793083327ULL));},
	[]() {kAotMethods[11]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[12])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),chaos_make_string_id_value(8595791296793083327ULL));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[13])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[14])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0,chaos_make_string_id_value(8595791296793083327ULL));},
	[]() {kAotMethods[15]();},
	[]() {kAotMethods[16]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[17])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[18]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[19])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),chaos_make_string_id_value(8595791296793083327ULL));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[20])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[21]();},
	[]() {kAotMethods[22]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[23])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[24]();},
	[]() {kAotMethods[25]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[26])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
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
static void* const kMethodPointers[27] = {
	reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects_CustomEntrySubject_0),
	reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_0_b__1_0_System_Int32_System_Int32),
	reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects_CustomEntrySubject_1),
	reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_1_b__2_0_System_Int32_System_Int32),
	reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects_CustomEntrySubject_2),
	reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_2_b__3_0_System_Int32_System_Int32),
	reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_2_b__3_1_System_Int32),
	reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects_CustomEntrySubject_3),
	reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_3_b__4_0_System_String),
	reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_3_b__4_1_IGrouping_System_Char_System_String),
	reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_3_b__4_2_System_Int32_System_String),
	reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects_CustomEntrySubject_4),
	reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_4_b__5_0_System_String),
	reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_4_b__5_1_IGrouping_System_Char_System_String),
	reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_4_b__5_2_System_Int32_System_String),
	reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects_CustomEntrySubject_5),
	reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects_CustomEntrySubject_6),
	reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_6_b__7_0_System_Int32),
	reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects_CustomEntrySubject_7),
	reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_7_b__8_0_System_String),
	reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_7_b__8_1_IGrouping_System_Char_System_String),
	reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects_CustomEntrySubject_8),
	reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects_CustomEntrySubject_9),
	reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_9_b__10_0_System_Int32),
	reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects_CustomEntrySubject_10),
	reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c_cctor),
	reinterpret_cast<void*>(&AggregationSubjects_AggregationSubjects___c__ctor),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
// ── VTable descriptors (for BootstrapRuntime TypeVTable registration) ──
static const VTableDescriptorV0 kChaosVTableDescriptors[] = {
	{
		CHAOS_IL2CPP_UINT64_C(0x0FDD76127D97A060),
		0x00000002u,
		0u,
		15u,
		reinterpret_cast<const void*>(kSlots_AggregationSubjects_AggregationSubjects___c),
		reinterpret_cast<const void**>(chaos_vtable_AggregationSubjects_AggregationSubjects___c),
		15u,
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
	.method_pointer_count      = 27u,
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
	.image_name_utf8    = "AggregationSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_AggregationSubjects_AggregationSubjects[1] = {
	{ 0u, "AggregationSubjects/AggregationSubjects::_exitCode", "_exitCode", "System.Int32", 0LL },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_AggregationSubjects_AggregationSubjects[11] = {
	{ 0u, "AggregationSubjects/AggregationSubjects::CustomEntrySubject_0:System.Void()", "CustomEntrySubject_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "AggregationSubjects/AggregationSubjects::CustomEntrySubject_1:System.Void()", "CustomEntrySubject_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "AggregationSubjects/AggregationSubjects::CustomEntrySubject_2:System.Void()", "CustomEntrySubject_2", "System.Void", 0, nullptr, 0u },
	{ 0u, "AggregationSubjects/AggregationSubjects::CustomEntrySubject_3:System.Void()", "CustomEntrySubject_3", "System.Void", 0, nullptr, 0u },
	{ 0u, "AggregationSubjects/AggregationSubjects::CustomEntrySubject_4:System.Void()", "CustomEntrySubject_4", "System.Void", 0, nullptr, 0u },
	{ 0u, "AggregationSubjects/AggregationSubjects::CustomEntrySubject_5:System.Void()", "CustomEntrySubject_5", "System.Void", 0, nullptr, 0u },
	{ 0u, "AggregationSubjects/AggregationSubjects::CustomEntrySubject_6:System.Void()", "CustomEntrySubject_6", "System.Void", 0, nullptr, 0u },
	{ 0u, "AggregationSubjects/AggregationSubjects::CustomEntrySubject_7:System.Void()", "CustomEntrySubject_7", "System.Void", 0, nullptr, 0u },
	{ 0u, "AggregationSubjects/AggregationSubjects::CustomEntrySubject_8:System.Void()", "CustomEntrySubject_8", "System.Void", 0, nullptr, 0u },
	{ 0u, "AggregationSubjects/AggregationSubjects::CustomEntrySubject_9:System.Void()", "CustomEntrySubject_9", "System.Void", 0, nullptr, 0u },
	{ 0u, "AggregationSubjects/AggregationSubjects::CustomEntrySubject_10:System.Void()", "CustomEntrySubject_10", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_AggregationSubjects_AggregationSubjects___c[16] = {
	{ 0u, "AggregationSubjects/AggregationSubjects+<>c::<CustomEntrySubject_0>b__1_0:System.Int32(System.Int32,System.Int32)", "<CustomEntrySubject_0>b__1_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "AggregationSubjects/AggregationSubjects+<>c::<CustomEntrySubject_1>b__2_0:System.Int32(System.Int32,System.Int32)", "<CustomEntrySubject_1>b__2_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "AggregationSubjects/AggregationSubjects+<>c::<CustomEntrySubject_2>b__3_0:System.Int32(System.Int32,System.Int32)", "<CustomEntrySubject_2>b__3_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "AggregationSubjects/AggregationSubjects+<>c::<CustomEntrySubject_2>b__3_1:System.String(System.Int32)", "<CustomEntrySubject_2>b__3_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "AggregationSubjects/AggregationSubjects+<>c::<CustomEntrySubject_3>b__4_0:System.Char(System.String)", "<CustomEntrySubject_3>b__4_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "AggregationSubjects/AggregationSubjects+<>c::<CustomEntrySubject_3>b__4_1:System.Collections.Generic.KeyValuePair<System.Char,System.Int32>(IGrouping<System.Char,System.String>)", "<CustomEntrySubject_3>b__4_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "AggregationSubjects/AggregationSubjects+<>c::<CustomEntrySubject_3>b__4_2:System.Int32(System.Int32,System.String)", "<CustomEntrySubject_3>b__4_2", "System.Void", 0, nullptr, 0u },
	{ 0u, "AggregationSubjects/AggregationSubjects+<>c::<CustomEntrySubject_4>b__5_0:System.Char(System.String)", "<CustomEntrySubject_4>b__5_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "AggregationSubjects/AggregationSubjects+<>c::<CustomEntrySubject_4>b__5_1:System.Collections.Generic.KeyValuePair<System.Char,System.Int32>(IGrouping<System.Char,System.String>)", "<CustomEntrySubject_4>b__5_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "AggregationSubjects/AggregationSubjects+<>c::<CustomEntrySubject_4>b__5_2:System.Int32(System.Int32,System.String)", "<CustomEntrySubject_4>b__5_2", "System.Void", 0, nullptr, 0u },
	{ 0u, "AggregationSubjects/AggregationSubjects+<>c::<CustomEntrySubject_6>b__7_0:System.Boolean(System.Int32)", "<CustomEntrySubject_6>b__7_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "AggregationSubjects/AggregationSubjects+<>c::<CustomEntrySubject_7>b__8_0:System.Char(System.String)", "<CustomEntrySubject_7>b__8_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "AggregationSubjects/AggregationSubjects+<>c::<CustomEntrySubject_7>b__8_1:System.Collections.Generic.KeyValuePair<System.Char,System.Int32>(IGrouping<System.Char,System.String>)", "<CustomEntrySubject_7>b__8_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "AggregationSubjects/AggregationSubjects+<>c::<CustomEntrySubject_9>b__10_0:System.Boolean(System.Int32)", "<CustomEntrySubject_9>b__10_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "AggregationSubjects/AggregationSubjects+<>c::.cctor:System.Void()", ".cctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "AggregationSubjects/AggregationSubjects+<>c::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[2] = {
	{ 0u, "AggregationSubjects/AggregationSubjects", "AggregationSubjects/AggregationSubjects", "", "AggregationSubjects", "AggregationSubjects", nullptr, kReflFields_AggregationSubjects_AggregationSubjects, 1u, nullptr, 0u,
	kReflMethods_AggregationSubjects_AggregationSubjects, 11u },
	{ 0u, "AggregationSubjects/AggregationSubjects+<>c", "AggregationSubjects/AggregationSubjects+<>c", "", "AggregationSubjects+<>c", "AggregationSubjects+<>c", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_AggregationSubjects_AggregationSubjects___c, 16u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[2] = {
	&kReflTypes[0],
	&kReflTypes[1],
};

static const ReflectionQueryImageDescriptor kReflImage = { "AggregationSubjects", kReflTypePtrs, 2u };

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
// Managed method: AggregationSubjects/AggregationSubjects::CustomEntrySubject_0()
extern "C" void AggregationSubjects_AggregationSubjects_CustomEntrySubject_0(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(79080714u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s1);
		chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers__InitializeArray_System_Void_System_Array_System_RuntimeFieldHandle_(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_AggregationSubjects_AggregationSubjects___c();
	_s1 = chaos_static_AggregationSubjects_AggregationSubjects___c____9__1_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_AggregationSubjects_AggregationSubjects___c();
			_s1 = chaos_static_AggregationSubjects_AggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 1)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2) -> CHAOS_IL2CPP_INT32 {
					auto& _d_entry = s_hotpatch_entries[1];
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
			chaos_ensure_type_initialized_AggregationSubjects_AggregationSubjects___c();
				chaos_static_AggregationSubjects_AggregationSubjects___c____9__1_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[6])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		chaos_locals[0] = _s4;
		return;
	}
}

// Managed method: AggregationSubjects/AggregationSubjects+<>c::<CustomEntrySubject_0>b__1_0(System.Int32,System.Int32)
extern "C" CHAOS_IL2CPP_INT32 AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_0_b__1_0_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2)
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

// Managed method: AggregationSubjects/AggregationSubjects::CustomEntrySubject_1()
extern "C" void AggregationSubjects_AggregationSubjects_CustomEntrySubject_1(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(79080714u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s1);
		chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers__InitializeArray_System_Void_System_Array_System_RuntimeFieldHandle_(chaos_arg_0, chaos_arg_1);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_ensure_type_initialized_AggregationSubjects_AggregationSubjects___c();
	_s2 = chaos_static_AggregationSubjects_AggregationSubjects___c____9__2_0;
	_s3 = _s2;
	{
		if (_s3 == 0)
		{
			chaos_ensure_type_initialized_AggregationSubjects_AggregationSubjects___c();
			_s2 = chaos_static_AggregationSubjects_AggregationSubjects___c____9;
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
			chaos_ensure_type_initialized_AggregationSubjects_AggregationSubjects___c();
				chaos_static_AggregationSubjects_AggregationSubjects___c____9__2_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_2 = _s6;
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[8])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		chaos_locals[0] = _s4;
		return;
	}
}

// Managed method: AggregationSubjects/AggregationSubjects+<>c::<CustomEntrySubject_1>b__2_0(System.Int32,System.Int32)
extern "C" CHAOS_IL2CPP_INT32 AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_1_b__2_0_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2)
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

// Managed method: AggregationSubjects/AggregationSubjects::CustomEntrySubject_2()
extern "C" void AggregationSubjects_AggregationSubjects_CustomEntrySubject_2(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(73306142u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s1);
		chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers__InitializeArray_System_Void_System_Array_System_RuntimeFieldHandle_(chaos_arg_0, chaos_arg_1);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_ensure_type_initialized_AggregationSubjects_AggregationSubjects___c();
	_s2 = chaos_static_AggregationSubjects_AggregationSubjects___c____9__3_0;
	_s3 = _s2;
	{
		if (_s3 == 0)
		{
			chaos_ensure_type_initialized_AggregationSubjects_AggregationSubjects___c();
			_s2 = chaos_static_AggregationSubjects_AggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 5)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2) -> CHAOS_IL2CPP_INT32 {
					auto& _d_entry = s_hotpatch_entries[5];
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
			chaos_ensure_type_initialized_AggregationSubjects_AggregationSubjects___c();
				chaos_static_AggregationSubjects_AggregationSubjects___c____9__3_0 = chaos_value;
			}
		}
		chaos_ensure_type_initialized_AggregationSubjects_AggregationSubjects___c();
		_s7 = chaos_static_AggregationSubjects_AggregationSubjects___c____9__3_1;
		_s8 = _s7;
		{
			if (_s8 == 0)
			{
				chaos_ensure_type_initialized_AggregationSubjects_AggregationSubjects___c();
				_s7 = chaos_static_AggregationSubjects_AggregationSubjects___c____9;
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
				_s8 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
				}
				{
					const auto chaos_method_ptr = _s8;
					const auto chaos_target = _s7;
					auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_String_, {});
					chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_String_.hot;
					chaos_object->chaos_delegate_target = chaos_target;
					chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
					_s7 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
				}
				_s8 = _s7;
				{
					auto chaos_value = _s8;
				chaos_ensure_type_initialized_AggregationSubjects_AggregationSubjects___c();
					chaos_static_AggregationSubjects_AggregationSubjects___c____9__3_1 = chaos_value;
				}
			}
			{
				const auto chaos_arg_3 = _s7;
				const auto chaos_arg_2 = _s6;
				const auto chaos_arg_1 = _s5;
				const auto chaos_arg_0 = _s4;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[13])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3);
				_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[0] = _s4;
			return;
		}
	}
}

// Managed method: AggregationSubjects/AggregationSubjects+<>c::<CustomEntrySubject_2>b__3_0(System.Int32,System.Int32)
extern "C" CHAOS_IL2CPP_INT32 AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_2_b__3_0_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2)
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

// Managed method: AggregationSubjects/AggregationSubjects+<>c::<CustomEntrySubject_2>b__3_1(System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_2_b__3_1_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_args[1]);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Int32__ToString_System_String__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return _s0;
}

// Managed method: AggregationSubjects/AggregationSubjects::CustomEntrySubject_3()
extern "C" void AggregationSubjects_AggregationSubjects_CustomEntrySubject_3(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_String.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{{
		_s3 = CHAOS_IL2CPP_STRING_ID("apple");
	}}
	{
		auto chaos_value_raw = _s3;
		if (chaos_is_string_id(chaos_value_raw))
		{
			chaos_value_raw = chaos_string_materialize(chaos_value_raw);
		}
		auto chaos_value = chaos_value_raw;
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
		if (!chaos_is_array_store_compatible(chaos_array, chaos_value))
		{
			CHAOS_IL2CPP_FAIL();
		}
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)]));
		chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)] = chaos_value;
		GC_END_STUBBORN_CHANGE(chaos_array);
		chaos_gc_dirty_card(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{{
		_s3 = CHAOS_IL2CPP_STRING_ID("banana");
	}}
	{
		auto chaos_value_raw = _s3;
		if (chaos_is_string_id(chaos_value_raw))
		{
			chaos_value_raw = chaos_string_materialize(chaos_value_raw);
		}
		auto chaos_value = chaos_value_raw;
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s2);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s1);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)]));
		chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)] = chaos_value;
		GC_END_STUBBORN_CHANGE(chaos_array);
		chaos_gc_dirty_card(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	{{
		_s3 = CHAOS_IL2CPP_STRING_ID("avocado");
	}}
	{
		auto chaos_value_raw = _s3;
		if (chaos_is_string_id(chaos_value_raw))
		{
			chaos_value_raw = chaos_string_materialize(chaos_value_raw);
		}
		auto chaos_value = chaos_value_raw;
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s2);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s1);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)]));
		chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)] = chaos_value;
		GC_END_STUBBORN_CHANGE(chaos_array);
		chaos_gc_dirty_card(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
	{{
		_s3 = CHAOS_IL2CPP_STRING_ID("blueberry");
	}}
	{
		auto chaos_value_raw = _s3;
		if (chaos_is_string_id(chaos_value_raw))
		{
			chaos_value_raw = chaos_string_materialize(chaos_value_raw);
		}
		auto chaos_value = chaos_value_raw;
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s2);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s1);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)]));
		chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)] = chaos_value;
		GC_END_STUBBORN_CHANGE(chaos_array);
		chaos_gc_dirty_card(chaos_array);
	}
	chaos_ensure_type_initialized_AggregationSubjects_AggregationSubjects___c();
	_s1 = chaos_static_AggregationSubjects_AggregationSubjects___c____9__4_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_AggregationSubjects_AggregationSubjects___c();
			_s1 = chaos_static_AggregationSubjects_AggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 8)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
					auto& _d_entry = s_hotpatch_entries[8];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[16];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_1));
						CHAOS_IL2CPP_INTPTR _d_ret{};
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, &_d_ret);
						return _d_ret;
					}
					return reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1);
				};
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s2;
				const auto chaos_target = _s1;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_String_System_Char_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_String_System_Char_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_AggregationSubjects_AggregationSubjects___c();
				chaos_static_AggregationSubjects_AggregationSubjects___c____9__4_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s6;
			const auto chaos_arg_0 = _s5;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[18])(chaos_arg_0, chaos_arg_1);
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		chaos_ensure_type_initialized_AggregationSubjects_AggregationSubjects___c();
		_s6 = chaos_static_AggregationSubjects_AggregationSubjects___c____9__4_1;
		_s7 = _s6;
		{
			if (_s7 == 0)
			{
				chaos_ensure_type_initialized_AggregationSubjects_AggregationSubjects___c();
				_s6 = chaos_static_AggregationSubjects_AggregationSubjects___c____9;
				{
					// Hotpatch-aware ldftn wrapper (slot 9)
					static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
						auto& _d_entry = s_hotpatch_entries[9];
						if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
							&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
						{
							alignas(16) uint8_t _d_ab[16];
							ArgBuffer _d_bw(_d_ab);
							_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
							_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_1));
							CHAOS_IL2CPP_INTPTR _d_ret{};
							::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
								_d_entry.method_key, _d_ab, &_d_ret);
							return _d_ret;
						}
						return reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1);
					};
				_s7 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
				}
				{
					const auto chaos_method_ptr = _s7;
					const auto chaos_target = _s6;
					auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_IGrouping_System_Char_System_String__System_Collections_Generic_KeyValuePair_System_Char_System_Int32__, {});
					chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_IGrouping_System_Char_System_String__System_Collections_Generic_KeyValuePair_System_Char_System_Int32__.hot;
					chaos_object->chaos_delegate_target = chaos_target;
					chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
					_s6 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
				}
				_s7 = _s6;
				{
					auto chaos_value = _s7;
				chaos_ensure_type_initialized_AggregationSubjects_AggregationSubjects___c();
					chaos_static_AggregationSubjects_AggregationSubjects___c____9__4_1 = chaos_value;
				}
			}
			{
				const auto chaos_arg_1 = _s8;
				const auto chaos_arg_0 = _s7;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[21])(chaos_arg_0, chaos_arg_1);
				_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[22])();
				_s8 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[23])();
				_s9 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			return;
		}
	}
}

// Managed method: AggregationSubjects/AggregationSubjects+<>c::<CustomEntrySubject_3>b__4_0(System.String)
extern "C" CHAOS_IL2CPP_INTPTR AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_3_b__4_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[24])(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return _s1;
}

// Managed method: AggregationSubjects/AggregationSubjects+<>c::<CustomEntrySubject_3>b__4_1(IGrouping<System.Char,System.String>)
extern "C" CHAOS_IL2CPP_INTPTR AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_3_b__4_1_IGrouping_System_Char_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
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
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[25])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s2 = chaos_args[1];
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_ensure_type_initialized_AggregationSubjects_AggregationSubjects___c();
	_s4 = chaos_static_AggregationSubjects_AggregationSubjects___c____9__4_2;
	_s5 = _s4;
	{
		if (_s5 == 0)
		{
			chaos_ensure_type_initialized_AggregationSubjects_AggregationSubjects___c();
			_s4 = chaos_static_AggregationSubjects_AggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 10)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2) -> CHAOS_IL2CPP_INT32 {
					auto& _d_entry = s_hotpatch_entries[10];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[20];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
						_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_fn_arg_1));
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_2));
						CHAOS_IL2CPP_INT32 _d_ret{};
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, &_d_ret);
						return _d_ret;
					}
					return reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32, CHAOS_IL2CPP_INTPTR)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1, chaos_fn_arg_2);
				};
			_s5 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s5;
				const auto chaos_target = _s4;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_String_System_Int32_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_String_System_Int32_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s5 = _s4;
			{
				auto chaos_value = _s5;
			chaos_ensure_type_initialized_AggregationSubjects_AggregationSubjects___c();
				chaos_static_AggregationSubjects_AggregationSubjects___c____9__4_2 = chaos_value;
			}
		}
		{
			const auto chaos_arg_2 = _s4;
			const auto chaos_arg_1 = _s3;
			const auto chaos_arg_0 = _s2;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[28])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Collections_Generic_KeyValuePair_System_Char_System_Int32_, {});
			chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Collections_Generic_KeyValuePair_System_Char_System_Int32_.hot;
			_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
		}
		return _s1;
	}
}

// Managed method: AggregationSubjects/AggregationSubjects+<>c::<CustomEntrySubject_3>b__4_2(System.Int32,System.String)
extern "C" CHAOS_IL2CPP_INT32 AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_3_b__4_2_System_Int32_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2)
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
	_s1 = chaos_args[2];
	{
		const auto chaos_arg_0 = _s1;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: AggregationSubjects/AggregationSubjects::CustomEntrySubject_4()
extern "C" void AggregationSubjects_AggregationSubjects_CustomEntrySubject_4(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_String.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{{
		_s3 = CHAOS_IL2CPP_STRING_ID("apple");
	}}
	{
		auto chaos_value_raw = _s3;
		if (chaos_is_string_id(chaos_value_raw))
		{
			chaos_value_raw = chaos_string_materialize(chaos_value_raw);
		}
		auto chaos_value = chaos_value_raw;
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s2);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s1);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)]));
		chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)] = chaos_value;
		GC_END_STUBBORN_CHANGE(chaos_array);
		chaos_gc_dirty_card(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{{
		_s3 = CHAOS_IL2CPP_STRING_ID("banana");
	}}
	{
		auto chaos_value_raw = _s3;
		if (chaos_is_string_id(chaos_value_raw))
		{
			chaos_value_raw = chaos_string_materialize(chaos_value_raw);
		}
		auto chaos_value = chaos_value_raw;
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s2);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s1);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)]));
		chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)] = chaos_value;
		GC_END_STUBBORN_CHANGE(chaos_array);
		chaos_gc_dirty_card(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	{{
		_s3 = CHAOS_IL2CPP_STRING_ID("avocado");
	}}
	{
		auto chaos_value_raw = _s3;
		if (chaos_is_string_id(chaos_value_raw))
		{
			chaos_value_raw = chaos_string_materialize(chaos_value_raw);
		}
		auto chaos_value = chaos_value_raw;
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s2);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s1);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)]));
		chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)] = chaos_value;
		GC_END_STUBBORN_CHANGE(chaos_array);
		chaos_gc_dirty_card(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
	{{
		_s3 = CHAOS_IL2CPP_STRING_ID("blueberry");
	}}
	{
		auto chaos_value_raw = _s3;
		if (chaos_is_string_id(chaos_value_raw))
		{
			chaos_value_raw = chaos_string_materialize(chaos_value_raw);
		}
		auto chaos_value = chaos_value_raw;
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s2);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s1);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)]));
		chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)] = chaos_value;
		GC_END_STUBBORN_CHANGE(chaos_array);
		chaos_gc_dirty_card(chaos_array);
	}
	chaos_ensure_type_initialized_AggregationSubjects_AggregationSubjects___c();
	_s1 = chaos_static_AggregationSubjects_AggregationSubjects___c____9__5_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_AggregationSubjects_AggregationSubjects___c();
			_s1 = chaos_static_AggregationSubjects_AggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 12)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
					auto& _d_entry = s_hotpatch_entries[12];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[16];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_1));
						CHAOS_IL2CPP_INTPTR _d_ret{};
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, &_d_ret);
						return _d_ret;
					}
					return reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1);
				};
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s2;
				const auto chaos_target = _s1;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_String_System_Char_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_String_System_Char_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_AggregationSubjects_AggregationSubjects___c();
				chaos_static_AggregationSubjects_AggregationSubjects___c____9__5_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s6;
			const auto chaos_arg_0 = _s5;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[18])(chaos_arg_0, chaos_arg_1);
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		chaos_ensure_type_initialized_AggregationSubjects_AggregationSubjects___c();
		_s6 = chaos_static_AggregationSubjects_AggregationSubjects___c____9__5_1;
		_s7 = _s6;
		{
			if (_s7 == 0)
			{
				chaos_ensure_type_initialized_AggregationSubjects_AggregationSubjects___c();
				_s6 = chaos_static_AggregationSubjects_AggregationSubjects___c____9;
				{
					// Hotpatch-aware ldftn wrapper (slot 13)
					static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
						auto& _d_entry = s_hotpatch_entries[13];
						if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
							&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
						{
							alignas(16) uint8_t _d_ab[16];
							ArgBuffer _d_bw(_d_ab);
							_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
							_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_1));
							CHAOS_IL2CPP_INTPTR _d_ret{};
							::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
								_d_entry.method_key, _d_ab, &_d_ret);
							return _d_ret;
						}
						return reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1);
					};
				_s7 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
				}
				{
					const auto chaos_method_ptr = _s7;
					const auto chaos_target = _s6;
					auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_IGrouping_System_Char_System_String__System_Collections_Generic_KeyValuePair_System_Char_System_Int32__, {});
					chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_IGrouping_System_Char_System_String__System_Collections_Generic_KeyValuePair_System_Char_System_Int32__.hot;
					chaos_object->chaos_delegate_target = chaos_target;
					chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
					_s6 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
				}
				_s7 = _s6;
				{
					auto chaos_value = _s7;
				chaos_ensure_type_initialized_AggregationSubjects_AggregationSubjects___c();
					chaos_static_AggregationSubjects_AggregationSubjects___c____9__5_1 = chaos_value;
				}
			}
			{
				const auto chaos_arg_1 = _s8;
				const auto chaos_arg_0 = _s7;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[21])(chaos_arg_0, chaos_arg_1);
				_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[22])();
				_s8 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[23])();
				_s9 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			return;
		}
	}
}

// Managed method: AggregationSubjects/AggregationSubjects+<>c::<CustomEntrySubject_4>b__5_0(System.String)
extern "C" CHAOS_IL2CPP_INTPTR AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_4_b__5_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[24])(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return _s1;
}

// Managed method: AggregationSubjects/AggregationSubjects+<>c::<CustomEntrySubject_4>b__5_1(IGrouping<System.Char,System.String>)
extern "C" CHAOS_IL2CPP_INTPTR AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_4_b__5_1_IGrouping_System_Char_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
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
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[25])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s2 = chaos_args[1];
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_ensure_type_initialized_AggregationSubjects_AggregationSubjects___c();
	_s4 = chaos_static_AggregationSubjects_AggregationSubjects___c____9__5_2;
	_s5 = _s4;
	{
		if (_s5 == 0)
		{
			chaos_ensure_type_initialized_AggregationSubjects_AggregationSubjects___c();
			_s4 = chaos_static_AggregationSubjects_AggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 14)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2) -> CHAOS_IL2CPP_INT32 {
					auto& _d_entry = s_hotpatch_entries[14];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[20];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
						_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_fn_arg_1));
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_2));
						CHAOS_IL2CPP_INT32 _d_ret{};
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, &_d_ret);
						return _d_ret;
					}
					return reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32, CHAOS_IL2CPP_INTPTR)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1, chaos_fn_arg_2);
				};
			_s5 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s5;
				const auto chaos_target = _s4;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_String_System_Int32_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_String_System_Int32_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s5 = _s4;
			{
				auto chaos_value = _s5;
			chaos_ensure_type_initialized_AggregationSubjects_AggregationSubjects___c();
				chaos_static_AggregationSubjects_AggregationSubjects___c____9__5_2 = chaos_value;
			}
		}
		{
			const auto chaos_arg_2 = _s4;
			const auto chaos_arg_1 = _s3;
			const auto chaos_arg_0 = _s2;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[28])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Collections_Generic_KeyValuePair_System_Char_System_Int32_, {});
			chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Collections_Generic_KeyValuePair_System_Char_System_Int32_.hot;
			_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
		}
		return _s1;
	}
}

// Managed method: AggregationSubjects/AggregationSubjects+<>c::<CustomEntrySubject_4>b__5_2(System.Int32,System.String)
extern "C" CHAOS_IL2CPP_INT32 AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_4_b__5_2_System_Int32_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2)
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
	_s1 = chaos_args[2];
	{
		const auto chaos_arg_0 = _s1;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: AggregationSubjects/AggregationSubjects::CustomEntrySubject_5()
extern "C" void AggregationSubjects_AggregationSubjects_CustomEntrySubject_5(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};


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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(77252407u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s1);
		chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers__InitializeArray_System_Void_System_Array_System_RuntimeFieldHandle_(chaos_arg_0, chaos_arg_1);
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[35])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[0] = _s0;
	return;
}

// Managed method: AggregationSubjects/AggregationSubjects::CustomEntrySubject_6()
extern "C" void AggregationSubjects_AggregationSubjects_CustomEntrySubject_6(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(10);
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(80959911u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s1);
		chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers__InitializeArray_System_Void_System_Array_System_RuntimeFieldHandle_(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_AggregationSubjects_AggregationSubjects___c();
	_s1 = chaos_static_AggregationSubjects_AggregationSubjects___c____9__7_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_AggregationSubjects_AggregationSubjects___c();
			_s1 = chaos_static_AggregationSubjects_AggregationSubjects___c____9;
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
			chaos_ensure_type_initialized_AggregationSubjects_AggregationSubjects___c();
				chaos_static_AggregationSubjects_AggregationSubjects___c____9__7_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[39])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		chaos_locals[0] = _s4;
		return;
	}
}

// Managed method: AggregationSubjects/AggregationSubjects+<>c::<CustomEntrySubject_6>b__7_0(System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_6_b__7_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0) > static_cast<CHAOS_IL2CPP_INT32>(_s1) ? 1 : 0);
	return _s0;
}

// Managed method: AggregationSubjects/AggregationSubjects::CustomEntrySubject_7()
extern "C" void AggregationSubjects_AggregationSubjects_CustomEntrySubject_7(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_String.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{{
		_s3 = CHAOS_IL2CPP_STRING_ID("apple");
	}}
	{
		auto chaos_value_raw = _s3;
		if (chaos_is_string_id(chaos_value_raw))
		{
			chaos_value_raw = chaos_string_materialize(chaos_value_raw);
		}
		auto chaos_value = chaos_value_raw;
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
		if (!chaos_is_array_store_compatible(chaos_array, chaos_value))
		{
			CHAOS_IL2CPP_FAIL();
		}
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)]));
		chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)] = chaos_value;
		GC_END_STUBBORN_CHANGE(chaos_array);
		chaos_gc_dirty_card(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{{
		_s3 = CHAOS_IL2CPP_STRING_ID("banana");
	}}
	{
		auto chaos_value_raw = _s3;
		if (chaos_is_string_id(chaos_value_raw))
		{
			chaos_value_raw = chaos_string_materialize(chaos_value_raw);
		}
		auto chaos_value = chaos_value_raw;
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s2);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s1);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)]));
		chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)] = chaos_value;
		GC_END_STUBBORN_CHANGE(chaos_array);
		chaos_gc_dirty_card(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	{{
		_s3 = CHAOS_IL2CPP_STRING_ID("avocado");
	}}
	{
		auto chaos_value_raw = _s3;
		if (chaos_is_string_id(chaos_value_raw))
		{
			chaos_value_raw = chaos_string_materialize(chaos_value_raw);
		}
		auto chaos_value = chaos_value_raw;
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s2);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s1);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)]));
		chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)] = chaos_value;
		GC_END_STUBBORN_CHANGE(chaos_array);
		chaos_gc_dirty_card(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
	{{
		_s3 = CHAOS_IL2CPP_STRING_ID("blueberry");
	}}
	{
		auto chaos_value_raw = _s3;
		if (chaos_is_string_id(chaos_value_raw))
		{
			chaos_value_raw = chaos_string_materialize(chaos_value_raw);
		}
		auto chaos_value = chaos_value_raw;
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s2);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s1);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)]));
		chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)] = chaos_value;
		GC_END_STUBBORN_CHANGE(chaos_array);
		chaos_gc_dirty_card(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
	{{
		_s3 = CHAOS_IL2CPP_STRING_ID("cherry");
	}}
	{
		auto chaos_value_raw = _s3;
		if (chaos_is_string_id(chaos_value_raw))
		{
			chaos_value_raw = chaos_string_materialize(chaos_value_raw);
		}
		auto chaos_value = chaos_value_raw;
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s2);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s1);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)]));
		chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)] = chaos_value;
		GC_END_STUBBORN_CHANGE(chaos_array);
		chaos_gc_dirty_card(chaos_array);
	}
	chaos_ensure_type_initialized_AggregationSubjects_AggregationSubjects___c();
	_s1 = chaos_static_AggregationSubjects_AggregationSubjects___c____9__8_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_AggregationSubjects_AggregationSubjects___c();
			_s1 = chaos_static_AggregationSubjects_AggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 19)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
					auto& _d_entry = s_hotpatch_entries[19];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[16];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_1));
						CHAOS_IL2CPP_INTPTR _d_ret{};
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, &_d_ret);
						return _d_ret;
					}
					return reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1);
				};
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s2;
				const auto chaos_target = _s1;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_String_System_Char_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_String_System_Char_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_AggregationSubjects_AggregationSubjects___c();
				chaos_static_AggregationSubjects_AggregationSubjects___c____9__8_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s7;
			const auto chaos_arg_0 = _s6;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[18])(chaos_arg_0, chaos_arg_1);
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		chaos_ensure_type_initialized_AggregationSubjects_AggregationSubjects___c();
		_s7 = chaos_static_AggregationSubjects_AggregationSubjects___c____9__8_1;
		_s8 = _s7;
		{
			if (_s8 == 0)
			{
				chaos_ensure_type_initialized_AggregationSubjects_AggregationSubjects___c();
				_s7 = chaos_static_AggregationSubjects_AggregationSubjects___c____9;
				{
					// Hotpatch-aware ldftn wrapper (slot 20)
					static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
						auto& _d_entry = s_hotpatch_entries[20];
						if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
							&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
						{
							alignas(16) uint8_t _d_ab[16];
							ArgBuffer _d_bw(_d_ab);
							_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
							_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_1));
							CHAOS_IL2CPP_INTPTR _d_ret{};
							::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
								_d_entry.method_key, _d_ab, &_d_ret);
							return _d_ret;
						}
						return reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1);
					};
				_s8 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
				}
				{
					const auto chaos_method_ptr = _s8;
					const auto chaos_target = _s7;
					auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_IGrouping_System_Char_System_String__System_Collections_Generic_KeyValuePair_System_Char_System_Int32__, {});
					chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_IGrouping_System_Char_System_String__System_Collections_Generic_KeyValuePair_System_Char_System_Int32__.hot;
					chaos_object->chaos_delegate_target = chaos_target;
					chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
					_s7 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
				}
				_s8 = _s7;
				{
					auto chaos_value = _s8;
				chaos_ensure_type_initialized_AggregationSubjects_AggregationSubjects___c();
					chaos_static_AggregationSubjects_AggregationSubjects___c____9__8_1 = chaos_value;
				}
			}
			{
				const auto chaos_arg_1 = _s9;
				const auto chaos_arg_0 = _s8;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[21])(chaos_arg_0, chaos_arg_1);
				_s8 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[22])();
				_s9 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[23])();
				_s10 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			return;
		}
	}
}

// Managed method: AggregationSubjects/AggregationSubjects+<>c::<CustomEntrySubject_7>b__8_0(System.String)
extern "C" CHAOS_IL2CPP_INTPTR AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_7_b__8_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[24])(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return _s1;
}

// Managed method: AggregationSubjects/AggregationSubjects+<>c::<CustomEntrySubject_7>b__8_1(IGrouping<System.Char,System.String>)
extern "C" CHAOS_IL2CPP_INTPTR AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_7_b__8_1_IGrouping_System_Char_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
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

	_s0 = chaos_args[1];
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[25])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s2 = chaos_args[1];
	{
		const auto chaos_arg_0 = _s2;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[42])(chaos_arg_0);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Collections_Generic_KeyValuePair_System_Char_System_Int32_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Collections_Generic_KeyValuePair_System_Char_System_Int32_.hot;
		_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	return _s1;
}

// Managed method: AggregationSubjects/AggregationSubjects::CustomEntrySubject_8()
extern "C" void AggregationSubjects_AggregationSubjects_CustomEntrySubject_8(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};


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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(73306142u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s1);
		chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers__InitializeArray_System_Void_System_Array_System_RuntimeFieldHandle_(chaos_arg_0, chaos_arg_1);
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT64(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[43])(chaos_arg_0);
		_s0 = ChaosStoreInt64(chaos_result);
	}
	chaos_locals[0] = _s0;
	return;
}

// Managed method: AggregationSubjects/AggregationSubjects::CustomEntrySubject_9()
extern "C" void AggregationSubjects_AggregationSubjects_CustomEntrySubject_9(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(79080714u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s1);
		chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers__InitializeArray_System_Void_System_Array_System_RuntimeFieldHandle_(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_AggregationSubjects_AggregationSubjects___c();
	_s1 = chaos_static_AggregationSubjects_AggregationSubjects___c____9__10_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_AggregationSubjects_AggregationSubjects___c();
			_s1 = chaos_static_AggregationSubjects_AggregationSubjects___c____9;
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
			chaos_ensure_type_initialized_AggregationSubjects_AggregationSubjects___c();
				chaos_static_AggregationSubjects_AggregationSubjects___c____9__10_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT64(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[45])(chaos_arg_0, chaos_arg_1);
			_s4 = ChaosStoreInt64(chaos_result);
		}
		chaos_locals[0] = _s4;
		return;
	}
}

// Managed method: AggregationSubjects/AggregationSubjects+<>c::<CustomEntrySubject_9>b__10_0(System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR AggregationSubjects_AggregationSubjects___c_CustomEntrySubject_9_b__10_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
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

	_s0 = chaos_args[1];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosRem(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	return _s0;
}

// Managed method: AggregationSubjects/AggregationSubjects::CustomEntrySubject_10()
extern "C" void AggregationSubjects_AggregationSubjects_CustomEntrySubject_10(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};


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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(79080714u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s1);
		chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers__InitializeArray_System_Void_System_Array_System_RuntimeFieldHandle_(chaos_arg_0, chaos_arg_1);
	}
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[46])(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[1] = _s0;
	return;
}

// Managed method: AggregationSubjects/AggregationSubjects+<>c::.cctor()
extern "C" void AggregationSubjects_AggregationSubjects___c_cctor(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};

	chaos_ensure_type_initialized_AggregationSubjects_AggregationSubjects___c();

	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_AggregationSubjects_AggregationSubjects___c, {});
		chaos_object->header.type_info = &chaos_mt_AggregationSubjects_AggregationSubjects___c.hot;
		AggregationSubjects_AggregationSubjects___c__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	{
		auto chaos_value = _s0;
	chaos_ensure_type_initialized_AggregationSubjects_AggregationSubjects___c();
		chaos_static_AggregationSubjects_AggregationSubjects___c____9 = chaos_value;
	}
	return;
}

// Managed method: AggregationSubjects/AggregationSubjects+<>c::.ctor()
extern "C" void AggregationSubjects_AggregationSubjects___c__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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



}  // namespace chaos::il2cpp::codegen::AggregationSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 27;

extern "C" void ChaosJitRegisterAll() {}