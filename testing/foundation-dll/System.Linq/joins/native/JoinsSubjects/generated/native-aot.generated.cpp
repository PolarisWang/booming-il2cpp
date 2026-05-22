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

namespace chaos::il2cpp::codegen::JoinsSubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


const void* chaos_vtable_JoinsSubjects_JoinsSubjects___c[];
const void* chaos_vtable_JoinsSubjects_JoinsSubjects___c__DisplayClass5_0[];
const void* chaos_vtable_JoinsSubjects_JoinsSubjects___c__DisplayClass6_0[];
const void* chaos_vtable_JoinsSubjects_JoinsSubjects___c__DisplayClass7_0[];
const void* chaos_vtable_JoinsSubjects_JoinsSubjects___c__DisplayClass8_0[];
inline TypeInfoV0 chaos_mt_JoinsSubjects__PrivateImplementationDetails_ = {{nullptr, nullptr, 11669710270508038053ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_JoinsSubjects__PrivateImplementationDetails_ = static_cast<CHAOS_IL2CPP_INTPTR>(11669710270508038053ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Object = {{nullptr, nullptr, 15228727185366376748ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_INTPTR>(15228727185366376748ULL);
inline TypeInfoV0 chaos_mt_JoinsSubjects_JoinsSubjects___c = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_JoinsSubjects_JoinsSubjects___c, 5474710593607388636ULL, 8u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_JoinsSubjects_JoinsSubjects___c = static_cast<CHAOS_IL2CPP_INTPTR>(5474710593607388636ULL);
inline TypeInfoV0 chaos_mt_JoinsSubjects_JoinsSubjects___c__DisplayClass5_0 = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_JoinsSubjects_JoinsSubjects___c__DisplayClass5_0, 5735147315386674384ULL, 10u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_JoinsSubjects_JoinsSubjects___c__DisplayClass5_0 = static_cast<CHAOS_IL2CPP_INTPTR>(5735147315386674384ULL);
inline TypeInfoV0 chaos_mt_JoinsSubjects_JoinsSubjects___c__DisplayClass6_0 = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_JoinsSubjects_JoinsSubjects___c__DisplayClass6_0, 6261706732526482117ULL, 12u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_JoinsSubjects_JoinsSubjects___c__DisplayClass6_0 = static_cast<CHAOS_IL2CPP_INTPTR>(6261706732526482117ULL);
inline TypeInfoV0 chaos_mt_JoinsSubjects_JoinsSubjects___c__DisplayClass7_0 = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_JoinsSubjects_JoinsSubjects___c__DisplayClass7_0, 7033149379453002170ULL, 14u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_JoinsSubjects_JoinsSubjects___c__DisplayClass7_0 = static_cast<CHAOS_IL2CPP_INTPTR>(7033149379453002170ULL);
inline TypeInfoV0 chaos_mt_JoinsSubjects_JoinsSubjects___c__DisplayClass8_0 = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_JoinsSubjects_JoinsSubjects___c__DisplayClass8_0, 7621035157155796239ULL, 16u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_JoinsSubjects_JoinsSubjects___c__DisplayClass8_0 = static_cast<CHAOS_IL2CPP_INTPTR>(7621035157155796239ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Delegate = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, nullptr, 7451128447593600616ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Delegate = static_cast<CHAOS_IL2CPP_INTPTR>(7451128447593600616ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_MulticastDelegate = {{&chaos_mt_System_Private_CoreLib_System_Delegate.hot, nullptr, 6681393039041505440ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_MulticastDelegate = static_cast<CHAOS_IL2CPP_INTPTR>(6681393039041505440ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Boolean_ = {{&chaos_mt_System_Private_CoreLib_System_MulticastDelegate.hot, nullptr, 6749479132849532215ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Func_System_Int32_System_Boolean_ = static_cast<CHAOS_IL2CPP_INTPTR>(6749479132849532215ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Collections_Generic_IEnumerable_System_Int32__ = {{&chaos_mt_System_Private_CoreLib_System_MulticastDelegate.hot, nullptr, 13589737125985385701ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Func_System_Int32_System_Collections_Generic_IEnumerable_System_Int32__ = static_cast<CHAOS_IL2CPP_INTPTR>(13589737125985385701ULL);
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
extern "C" void JoinsSubjects_JoinsSubjects___c__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_0_b__1_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_1_b__2_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_3_b__4_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_4_b__5_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_5_b__6_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_6_b__7_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_7_b__8_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
const void* chaos_vtable_JoinsSubjects_JoinsSubjects___c[] =
{
	reinterpret_cast<void*>(JoinsSubjects_JoinsSubjects___c__ctor),
	reinterpret_cast<void*>(JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_0_b__1_0_System_Int32),
	reinterpret_cast<void*>(JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_1_b__2_0_System_Int32),
	reinterpret_cast<void*>(JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_3_b__4_0_System_Int32),
	reinterpret_cast<void*>(JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_4_b__5_0_System_Int32),
	reinterpret_cast<void*>(JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_5_b__6_0_System_Int32),
	reinterpret_cast<void*>(JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_6_b__7_0_System_Int32),
	reinterpret_cast<void*>(JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_7_b__8_0_System_Int32),
};
extern "C" void JoinsSubjects_JoinsSubjects___c__DisplayClass5_0__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INTPTR JoinsSubjects_JoinsSubjects___c__DisplayClass5_0_CustomEntrySubject_4_b__1_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 JoinsSubjects_JoinsSubjects___c__DisplayClass5_0_CustomEntrySubject_4_b__2_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
const void* chaos_vtable_JoinsSubjects_JoinsSubjects___c__DisplayClass5_0[] =
{
	reinterpret_cast<void*>(JoinsSubjects_JoinsSubjects___c__DisplayClass5_0__ctor),
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	reinterpret_cast<void*>(JoinsSubjects_JoinsSubjects___c__DisplayClass5_0_CustomEntrySubject_4_b__1_System_Int32),
	reinterpret_cast<void*>(JoinsSubjects_JoinsSubjects___c__DisplayClass5_0_CustomEntrySubject_4_b__2_System_Int32),
};
extern "C" void JoinsSubjects_JoinsSubjects___c__DisplayClass6_0__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INTPTR JoinsSubjects_JoinsSubjects___c__DisplayClass6_0_CustomEntrySubject_5_b__1_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 JoinsSubjects_JoinsSubjects___c__DisplayClass6_0_CustomEntrySubject_5_b__2_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
const void* chaos_vtable_JoinsSubjects_JoinsSubjects___c__DisplayClass6_0[] =
{
	reinterpret_cast<void*>(JoinsSubjects_JoinsSubjects___c__DisplayClass6_0__ctor),
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	reinterpret_cast<void*>(JoinsSubjects_JoinsSubjects___c__DisplayClass6_0_CustomEntrySubject_5_b__1_System_Int32),
	reinterpret_cast<void*>(JoinsSubjects_JoinsSubjects___c__DisplayClass6_0_CustomEntrySubject_5_b__2_System_Int32),
};
extern "C" void JoinsSubjects_JoinsSubjects___c__DisplayClass7_0__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INTPTR JoinsSubjects_JoinsSubjects___c__DisplayClass7_0_CustomEntrySubject_6_b__1_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 JoinsSubjects_JoinsSubjects___c__DisplayClass7_0_CustomEntrySubject_6_b__2_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
const void* chaos_vtable_JoinsSubjects_JoinsSubjects___c__DisplayClass7_0[] =
{
	reinterpret_cast<void*>(JoinsSubjects_JoinsSubjects___c__DisplayClass7_0__ctor),
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
	reinterpret_cast<void*>(JoinsSubjects_JoinsSubjects___c__DisplayClass7_0_CustomEntrySubject_6_b__1_System_Int32),
	reinterpret_cast<void*>(JoinsSubjects_JoinsSubjects___c__DisplayClass7_0_CustomEntrySubject_6_b__2_System_Int32),
};
extern "C" void JoinsSubjects_JoinsSubjects___c__DisplayClass8_0__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INTPTR JoinsSubjects_JoinsSubjects___c__DisplayClass8_0_CustomEntrySubject_7_b__1_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 JoinsSubjects_JoinsSubjects___c__DisplayClass8_0_CustomEntrySubject_7_b__2_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
const void* chaos_vtable_JoinsSubjects_JoinsSubjects___c__DisplayClass8_0[] =
{
	reinterpret_cast<void*>(JoinsSubjects_JoinsSubjects___c__DisplayClass8_0__ctor),
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
	reinterpret_cast<void*>(JoinsSubjects_JoinsSubjects___c__DisplayClass8_0_CustomEntrySubject_7_b__1_System_Int32),
	reinterpret_cast<void*>(JoinsSubjects_JoinsSubjects___c__DisplayClass8_0_CustomEntrySubject_7_b__2_System_Int32),
};
static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_JoinsSubjects_JoinsSubjects___c[] =
{
	{ 0x00000026u, reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c__ctor) },
	{ 0x00000027u, reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_0_b__1_0_System_Int32) },
	{ 0x00000028u, reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_1_b__2_0_System_Int32) },
	{ 0x00000029u, reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_3_b__4_0_System_Int32) },
	{ 0x0000002Au, reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_4_b__5_0_System_Int32) },
	{ 0x0000002Bu, reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_5_b__6_0_System_Int32) },
	{ 0x0000002Cu, reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_6_b__7_0_System_Int32) },
	{ 0x0000002Du, reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_7_b__8_0_System_Int32) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_JoinsSubjects_JoinsSubjects___c__DisplayClass5_0[] =
{
	{ 0x0000002Eu, reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass5_0__ctor) },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0x0000002Fu, reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass5_0_CustomEntrySubject_4_b__1_System_Int32) },
	{ 0x00000030u, reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass5_0_CustomEntrySubject_4_b__2_System_Int32) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_JoinsSubjects_JoinsSubjects___c__DisplayClass6_0[] =
{
	{ 0x00000031u, reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass6_0__ctor) },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0x00000032u, reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass6_0_CustomEntrySubject_5_b__1_System_Int32) },
	{ 0x00000033u, reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass6_0_CustomEntrySubject_5_b__2_System_Int32) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_JoinsSubjects_JoinsSubjects___c__DisplayClass7_0[] =
{
	{ 0x00000034u, reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass7_0__ctor) },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0x00000035u, reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass7_0_CustomEntrySubject_6_b__1_System_Int32) },
	{ 0x00000036u, reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass7_0_CustomEntrySubject_6_b__2_System_Int32) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_JoinsSubjects_JoinsSubjects___c__DisplayClass8_0[] =
{
	{ 0x00000037u, reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass8_0__ctor) },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0x00000038u, reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass8_0_CustomEntrySubject_7_b__1_System_Int32) },
	{ 0x00000039u, reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass8_0_CustomEntrySubject_7_b__2_System_Int32) },
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

struct chaos_type_JoinsSubjects__PrivateImplementationDetails_
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR field_JoinsSubjects__PrivateImplementationDetails___4636993D3E1DA4E9D6B8F87B79E8F7C6D018580D52661950EABC3845C5897A4D = 0;
	CHAOS_IL2CPP_INTPTR field_JoinsSubjects__PrivateImplementationDetails___A0FEBE8D859AC7FE1E225E18F8632CFA942D34C2F58386AE0B982B55FD4EB116 = 0;
	CHAOS_IL2CPP_INTPTR field_JoinsSubjects__PrivateImplementationDetails___A10494D90314704E24CA5786F6376A6097558F10BB7880B539C8B80312DCA080 = 0;
	CHAOS_IL2CPP_INTPTR field_JoinsSubjects__PrivateImplementationDetails___CBF03EFCA28AF3C84560CE82FEAE66B8AE0A9321EC14BE8F40D06EB0AD7BFE73 = 0;
	CHAOS_IL2CPP_INTPTR field_JoinsSubjects__PrivateImplementationDetails___D8D4F55129E2966F2C74F35D6AC1769A713230A8B8371D030F18C6DEA1AAFD77 = 0;
};

struct chaos_type_System_Private_CoreLib_System_Object
{
	ThinLockableHeader header{};
};

struct chaos_type_JoinsSubjects_JoinsSubjects___c : public chaos_type_System_Private_CoreLib_System_Object
{
};

struct chaos_type_JoinsSubjects_JoinsSubjects___c__DisplayClass5_0 : public chaos_type_System_Private_CoreLib_System_Object
{
	CHAOS_IL2CPP_INTPTR field_JoinsSubjects_JoinsSubjects___c__DisplayClass5_0__o = 0;
};

struct chaos_type_JoinsSubjects_JoinsSubjects___c__DisplayClass6_0 : public chaos_type_System_Private_CoreLib_System_Object
{
	CHAOS_IL2CPP_INTPTR field_JoinsSubjects_JoinsSubjects___c__DisplayClass6_0__o = 0;
};

struct chaos_type_JoinsSubjects_JoinsSubjects___c__DisplayClass7_0 : public chaos_type_System_Private_CoreLib_System_Object
{
	CHAOS_IL2CPP_INTPTR field_JoinsSubjects_JoinsSubjects___c__DisplayClass7_0__i = 0;
};

struct chaos_type_JoinsSubjects_JoinsSubjects___c__DisplayClass8_0 : public chaos_type_System_Private_CoreLib_System_Object
{
	CHAOS_IL2CPP_INTPTR field_JoinsSubjects_JoinsSubjects___c__DisplayClass8_0__i = 0;
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

struct chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Collections_Generic_IEnumerable_System_Int32__ : public chaos_type_System_Private_CoreLib_System_MulticastDelegate
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

CHAOS_IL2CPP_INTPTR chaos_static_JoinsSubjects_JoinsSubjects___c____9 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_JoinsSubjects_JoinsSubjects___c____9__1_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_JoinsSubjects_JoinsSubjects___c____9__2_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_JoinsSubjects_JoinsSubjects___c____9__4_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_JoinsSubjects_JoinsSubjects___c____9__5_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_JoinsSubjects_JoinsSubjects___c____9__6_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_JoinsSubjects_JoinsSubjects___c____9__7_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_JoinsSubjects_JoinsSubjects___c____9__8_0 = 0;

const CHAOS_IL2CPP_UINT8 chaos_static_field_data_JoinsSubjects__PrivateImplementationDetails___4636993D3E1DA4E9D6B8F87B79E8F7C6D018580D52661950EABC3845C5897A4D[] =
{
	0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
};

const CHAOS_IL2CPP_UINT8 chaos_static_field_data_JoinsSubjects__PrivateImplementationDetails___A0FEBE8D859AC7FE1E225E18F8632CFA942D34C2F58386AE0B982B55FD4EB116[] =
{
	0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
	0x05, 0x00, 0x00, 0x00,
};

const CHAOS_IL2CPP_UINT8 chaos_static_field_data_JoinsSubjects__PrivateImplementationDetails___A10494D90314704E24CA5786F6376A6097558F10BB7880B539C8B80312DCA080[] =
{
	0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
};

const CHAOS_IL2CPP_UINT8 chaos_static_field_data_JoinsSubjects__PrivateImplementationDetails___CBF03EFCA28AF3C84560CE82FEAE66B8AE0A9321EC14BE8F40D06EB0AD7BFE73[] =
{
	0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
	0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
};

const CHAOS_IL2CPP_UINT8 chaos_static_field_data_JoinsSubjects__PrivateImplementationDetails___D8D4F55129E2966F2C74F35D6AC1769A713230A8B8371D030F18C6DEA1AAFD77[] =
{
	0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
	0x02, 0x00, 0x00, 0x00,
};

bool chaos_try_get_static_field_data(
	CHAOS_IL2CPP_INTPTR chaos_field_handle,
	const CHAOS_IL2CPP_UINT8*& chaos_data,
	CHAOS_IL2CPP_SIZE& chaos_size)
{
	if (chaos_field_handle == static_cast<CHAOS_IL2CPP_INTPTR>(77819907u))
	{
		chaos_data = chaos_static_field_data_JoinsSubjects__PrivateImplementationDetails___4636993D3E1DA4E9D6B8F87B79E8F7C6D018580D52661950EABC3845C5897A4D;
		chaos_size = sizeof(chaos_static_field_data_JoinsSubjects__PrivateImplementationDetails___4636993D3E1DA4E9D6B8F87B79E8F7C6D018580D52661950EABC3845C5897A4D);
		return true;
	}
	if (chaos_field_handle == static_cast<CHAOS_IL2CPP_INTPTR>(67360599u))
	{
		chaos_data = chaos_static_field_data_JoinsSubjects__PrivateImplementationDetails___A0FEBE8D859AC7FE1E225E18F8632CFA942D34C2F58386AE0B982B55FD4EB116;
		chaos_size = sizeof(chaos_static_field_data_JoinsSubjects__PrivateImplementationDetails___A0FEBE8D859AC7FE1E225E18F8632CFA942D34C2F58386AE0B982B55FD4EB116);
		return true;
	}
	if (chaos_field_handle == static_cast<CHAOS_IL2CPP_INTPTR>(83683924u))
	{
		chaos_data = chaos_static_field_data_JoinsSubjects__PrivateImplementationDetails___A10494D90314704E24CA5786F6376A6097558F10BB7880B539C8B80312DCA080;
		chaos_size = sizeof(chaos_static_field_data_JoinsSubjects__PrivateImplementationDetails___A10494D90314704E24CA5786F6376A6097558F10BB7880B539C8B80312DCA080);
		return true;
	}
	if (chaos_field_handle == static_cast<CHAOS_IL2CPP_INTPTR>(77833573u))
	{
		chaos_data = chaos_static_field_data_JoinsSubjects__PrivateImplementationDetails___CBF03EFCA28AF3C84560CE82FEAE66B8AE0A9321EC14BE8F40D06EB0AD7BFE73;
		chaos_size = sizeof(chaos_static_field_data_JoinsSubjects__PrivateImplementationDetails___CBF03EFCA28AF3C84560CE82FEAE66B8AE0A9321EC14BE8F40D06EB0AD7BFE73);
		return true;
	}
	if (chaos_field_handle == static_cast<CHAOS_IL2CPP_INTPTR>(78109589u))
	{
		chaos_data = chaos_static_field_data_JoinsSubjects__PrivateImplementationDetails___D8D4F55129E2966F2C74F35D6AC1769A713230A8B8371D030F18C6DEA1AAFD77;
		chaos_size = sizeof(chaos_static_field_data_JoinsSubjects__PrivateImplementationDetails___D8D4F55129E2966F2C74F35D6AC1769A713230A8B8371D030F18C6DEA1AAFD77);
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

CHAOS_IL2CPP_ONCE_FLAG chaos_type_init_once_JoinsSubjects_JoinsSubjects___c;

void chaos_ensure_type_initialized_JoinsSubjects_JoinsSubjects___c()
{
	CHAOS_IL2CPP_CALL_ONCE(chaos_type_init_once_JoinsSubjects_JoinsSubjects___c, []()
	{
	if (chaos_static_JoinsSubjects_JoinsSubjects___c____9 == 0)
	{
	    auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_JoinsSubjects_JoinsSubjects___c);
	    chaos_object->header.type_info = &chaos_mt_JoinsSubjects_JoinsSubjects___c.hot;
		const auto chaos_arg_0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
		JoinsSubjects_JoinsSubjects___c__ctor(chaos_arg_0);
	    chaos_static_JoinsSubjects_JoinsSubjects___c____9 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	});
}

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };

extern "C" void JoinsSubjects_JoinsSubjects_CustomEntrySubject_0(void);
extern "C" CHAOS_IL2CPP_INT32 JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_0_b__1_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void JoinsSubjects_JoinsSubjects_CustomEntrySubject_1(void);
extern "C" CHAOS_IL2CPP_INT32 JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_1_b__2_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void JoinsSubjects_JoinsSubjects_CustomEntrySubject_2(void);
extern "C" void JoinsSubjects_JoinsSubjects_CustomEntrySubject_3(void);
extern "C" CHAOS_IL2CPP_INT32 JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_3_b__4_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void JoinsSubjects_JoinsSubjects_CustomEntrySubject_4(void);
extern "C" CHAOS_IL2CPP_INTPTR JoinsSubjects_JoinsSubjects___c__DisplayClass5_0_CustomEntrySubject_4_b__1_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 JoinsSubjects_JoinsSubjects___c__DisplayClass5_0_CustomEntrySubject_4_b__2_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_4_b__5_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void JoinsSubjects_JoinsSubjects_CustomEntrySubject_5(void);
extern "C" CHAOS_IL2CPP_INTPTR JoinsSubjects_JoinsSubjects___c__DisplayClass6_0_CustomEntrySubject_5_b__1_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 JoinsSubjects_JoinsSubjects___c__DisplayClass6_0_CustomEntrySubject_5_b__2_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_5_b__6_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void JoinsSubjects_JoinsSubjects_CustomEntrySubject_6(void);
extern "C" CHAOS_IL2CPP_INTPTR JoinsSubjects_JoinsSubjects___c__DisplayClass7_0_CustomEntrySubject_6_b__1_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 JoinsSubjects_JoinsSubjects___c__DisplayClass7_0_CustomEntrySubject_6_b__2_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_6_b__7_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void JoinsSubjects_JoinsSubjects_CustomEntrySubject_7(void);
extern "C" CHAOS_IL2CPP_INTPTR JoinsSubjects_JoinsSubjects___c__DisplayClass8_0_CustomEntrySubject_7_b__1_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 JoinsSubjects_JoinsSubjects___c__DisplayClass8_0_CustomEntrySubject_7_b__2_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_7_b__8_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void JoinsSubjects_JoinsSubjects___c__DisplayClass5_0__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void JoinsSubjects_JoinsSubjects___c__DisplayClass6_0__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void JoinsSubjects_JoinsSubjects___c__DisplayClass7_0__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void JoinsSubjects_JoinsSubjects___c__DisplayClass8_0__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void JoinsSubjects_JoinsSubjects___c_cctor(void);
extern "C" void JoinsSubjects_JoinsSubjects___c__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);


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
	1u,
	1u,
	2u,
	2u,
	2u,
	3u,
	3u,
	4u,
	5u,
	6u,
	6u,
	7u,
	8u,
	9u,
	9u,
	10u,
	11u,
	12u,
	12u,
	13u,
	14u,
	15u,
	15u,
	15u,
	15u,
	15u,
	15u,
	15u,
};

static constexpr struct {
	::ChaosAbiManifestV0 header;
	::ChaosAbiMethodEntryV0 entries[29];
	CHAOS_IL2CPP_UINT8 params[15];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		29u,
		15u,
		3825157024u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // JoinsSubjects_JoinsSubjects_CustomEntrySubject_0
		{ 1u, 1u },  // JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_0_b__1_0_System_Int32
		{ 0u, 0u },  // JoinsSubjects_JoinsSubjects_CustomEntrySubject_1
		{ 1u, 1u },  // JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_1_b__2_0_System_Int32
		{ 0u, 0u },  // JoinsSubjects_JoinsSubjects_CustomEntrySubject_2
		{ 0u, 0u },  // JoinsSubjects_JoinsSubjects_CustomEntrySubject_3
		{ 1u, 1u },  // JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_3_b__4_0_System_Int32
		{ 0u, 0u },  // JoinsSubjects_JoinsSubjects_CustomEntrySubject_4
		{ 2u, 1u },  // JoinsSubjects_JoinsSubjects___c__DisplayClass5_0_CustomEntrySubject_4_b__1_System_Int32
		{ 1u, 1u },  // JoinsSubjects_JoinsSubjects___c__DisplayClass5_0_CustomEntrySubject_4_b__2_System_Int32
		{ 2u, 1u },  // JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_4_b__5_0_System_Int32
		{ 0u, 0u },  // JoinsSubjects_JoinsSubjects_CustomEntrySubject_5
		{ 2u, 1u },  // JoinsSubjects_JoinsSubjects___c__DisplayClass6_0_CustomEntrySubject_5_b__1_System_Int32
		{ 1u, 1u },  // JoinsSubjects_JoinsSubjects___c__DisplayClass6_0_CustomEntrySubject_5_b__2_System_Int32
		{ 2u, 1u },  // JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_5_b__6_0_System_Int32
		{ 0u, 0u },  // JoinsSubjects_JoinsSubjects_CustomEntrySubject_6
		{ 2u, 1u },  // JoinsSubjects_JoinsSubjects___c__DisplayClass7_0_CustomEntrySubject_6_b__1_System_Int32
		{ 1u, 1u },  // JoinsSubjects_JoinsSubjects___c__DisplayClass7_0_CustomEntrySubject_6_b__2_System_Int32
		{ 2u, 1u },  // JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_6_b__7_0_System_Int32
		{ 0u, 0u },  // JoinsSubjects_JoinsSubjects_CustomEntrySubject_7
		{ 2u, 1u },  // JoinsSubjects_JoinsSubjects___c__DisplayClass8_0_CustomEntrySubject_7_b__1_System_Int32
		{ 1u, 1u },  // JoinsSubjects_JoinsSubjects___c__DisplayClass8_0_CustomEntrySubject_7_b__2_System_Int32
		{ 2u, 1u },  // JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_7_b__8_0_System_Int32
		{ 0u, 0u },  // JoinsSubjects_JoinsSubjects___c__DisplayClass5_0__ctor
		{ 0u, 0u },  // JoinsSubjects_JoinsSubjects___c__DisplayClass6_0__ctor
		{ 0u, 0u },  // JoinsSubjects_JoinsSubjects___c__DisplayClass7_0__ctor
		{ 0u, 0u },  // JoinsSubjects_JoinsSubjects___c__DisplayClass8_0__ctor
		{ 0u, 0u },  // JoinsSubjects_JoinsSubjects___c_cctor
		{ 0u, 0u },  // JoinsSubjects_JoinsSubjects___c__ctor
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

	static constexpr CHAOS_IL2CPP_UINT32 s_type_flags[11] = {
		0u,
		1548u,
		8u,
		1544u,
		1544u,
		1544u,
		1544u,
		1544u,
		1545u,
		1545u,
		1545u,
	};

	static constexpr const char* s_type_names[11] = {
		"<Module>",
		"JoinsSubjects",
		"<PrivateImplementationDetails>",
		"<>c",
		"<>c__DisplayClass5_0",
		"<>c__DisplayClass6_0",
		"<>c__DisplayClass7_0",
		"<>c__DisplayClass8_0",
		"__StaticArrayInitTypeSize=12",
		"__StaticArrayInitTypeSize=20",
		"__StaticArrayInitTypeSize=24",
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
		nullptr,
		&chaos_mt_JoinsSubjects__PrivateImplementationDetails_.hot,
		&chaos_mt_JoinsSubjects_JoinsSubjects___c.hot,
		&chaos_mt_JoinsSubjects_JoinsSubjects___c__DisplayClass5_0.hot,
		&chaos_mt_JoinsSubjects_JoinsSubjects___c__DisplayClass6_0.hot,
		&chaos_mt_JoinsSubjects_JoinsSubjects___c__DisplayClass7_0.hot,
		&chaos_mt_JoinsSubjects_JoinsSubjects___c__DisplayClass8_0.hot,
		nullptr,
		nullptr,
		nullptr,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_children[8] = {
		33554436u,
		33554437u,
		33554438u,
		33554439u,
		33554440u,
		33554441u,
		33554442u,
		33554443u,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_offset[12] = {
		0u,
		0u,
		5u,
		8u,
		8u,
		8u,
		8u,
		8u,
		8u,
		8u,
		8u,
		8u,
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
		/* .name_utf8         = */ "JoinsSubjects",
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
		::chaos::il2cpp::runtime_core::RegisterModule("JoinsSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[29] = {
	{ "CustomEntrySubject_0", 0x0000001Du, 0u },  // JoinsSubjects
	{ "CustomEntrySubject_1", 0x0000001Eu, 0u },  // JoinsSubjects
	{ "CustomEntrySubject_2", 0x0000001Fu, 0u },  // JoinsSubjects
	{ "CustomEntrySubject_3", 0x00000020u, 0u },  // JoinsSubjects
	{ "CustomEntrySubject_4", 0x00000021u, 0u },  // JoinsSubjects
	{ "CustomEntrySubject_5", 0x00000022u, 0u },  // JoinsSubjects
	{ "CustomEntrySubject_6", 0x00000023u, 0u },  // JoinsSubjects
	{ "CustomEntrySubject_7", 0x00000024u, 0u },  // JoinsSubjects
	{ "<CustomEntrySubject_0>b__1_0", 0x00000027u, 1u },  // JoinsSubjects+<>c
	{ "<CustomEntrySubject_1>b__2_0", 0x00000028u, 1u },  // JoinsSubjects+<>c
	{ "<CustomEntrySubject_3>b__4_0", 0x00000029u, 1u },  // JoinsSubjects+<>c
	{ "<CustomEntrySubject_4>b__5_0", 0x0000002Au, 1u },  // JoinsSubjects+<>c
	{ "<CustomEntrySubject_5>b__6_0", 0x0000002Bu, 1u },  // JoinsSubjects+<>c
	{ "<CustomEntrySubject_6>b__7_0", 0x0000002Cu, 1u },  // JoinsSubjects+<>c
	{ "<CustomEntrySubject_7>b__8_0", 0x0000002Du, 1u },  // JoinsSubjects+<>c
	{ ".cctor", 0x00000025u, 0u },  // JoinsSubjects+<>c
	{ ".ctor", 0x00000026u, 0u },  // JoinsSubjects+<>c
	{ "<CustomEntrySubject_4>b__1", 0x0000002Fu, 1u },  // JoinsSubjects+<>c__DisplayClass5_0
	{ "<CustomEntrySubject_4>b__2", 0x00000030u, 1u },  // JoinsSubjects+<>c__DisplayClass5_0
	{ ".ctor", 0x0000002Eu, 0u },  // JoinsSubjects+<>c__DisplayClass5_0
	{ "<CustomEntrySubject_5>b__1", 0x00000032u, 1u },  // JoinsSubjects+<>c__DisplayClass6_0
	{ "<CustomEntrySubject_5>b__2", 0x00000033u, 1u },  // JoinsSubjects+<>c__DisplayClass6_0
	{ ".ctor", 0x00000031u, 0u },  // JoinsSubjects+<>c__DisplayClass6_0
	{ "<CustomEntrySubject_6>b__1", 0x00000035u, 1u },  // JoinsSubjects+<>c__DisplayClass7_0
	{ "<CustomEntrySubject_6>b__2", 0x00000036u, 1u },  // JoinsSubjects+<>c__DisplayClass7_0
	{ ".ctor", 0x00000034u, 0u },  // JoinsSubjects+<>c__DisplayClass7_0
	{ "<CustomEntrySubject_7>b__1", 0x00000038u, 1u },  // JoinsSubjects+<>c__DisplayClass8_0
	{ "<CustomEntrySubject_7>b__2", 0x00000039u, 1u },  // JoinsSubjects+<>c__DisplayClass8_0
	{ ".ctor", 0x00000037u, 0u },  // JoinsSubjects+<>c__DisplayClass8_0
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[6] = {
	{ "JoinsSubjects", "", 0u, 8u },
	{ "JoinsSubjects+<>c", "", 8u, 9u },
	{ "JoinsSubjects+<>c__DisplayClass5_0", "", 17u, 3u },
	{ "JoinsSubjects+<>c__DisplayClass6_0", "", 20u, 3u },
	{ "JoinsSubjects+<>c__DisplayClass7_0", "", 23u, 3u },
	{ "JoinsSubjects+<>c__DisplayClass8_0", "", 26u, 3u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[29] = {
	{ 0x0000001Du, 0u },
	{ 0x0000001Eu, 2u },
	{ 0x0000001Fu, 4u },
	{ 0x00000020u, 5u },
	{ 0x00000021u, 7u },
	{ 0x00000022u, 11u },
	{ 0x00000023u, 15u },
	{ 0x00000024u, 19u },
	{ 0x00000025u, 27u },
	{ 0x00000026u, 28u },
	{ 0x00000027u, 1u },
	{ 0x00000028u, 3u },
	{ 0x00000029u, 6u },
	{ 0x0000002Au, 10u },
	{ 0x0000002Bu, 14u },
	{ 0x0000002Cu, 18u },
	{ 0x0000002Du, 22u },
	{ 0x0000002Eu, 23u },
	{ 0x0000002Fu, 8u },
	{ 0x00000030u, 9u },
	{ 0x00000031u, 24u },
	{ 0x00000032u, 12u },
	{ 0x00000033u, 13u },
	{ 0x00000034u, 25u },
	{ 0x00000035u, 16u },
	{ 0x00000036u, 17u },
	{ 0x00000037u, 26u },
	{ 0x00000038u, 20u },
	{ 0x00000039u, 21u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[29] = {
	{ reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects_CustomEntrySubject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // JoinsSubjects::CustomEntrySubject_0
	{ reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_0_b__1_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // JoinsSubjects+<>c::<CustomEntrySubject_0>b__1_0
	{ reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects_CustomEntrySubject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // JoinsSubjects::CustomEntrySubject_1
	{ reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_1_b__2_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // JoinsSubjects+<>c::<CustomEntrySubject_1>b__2_0
	{ reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects_CustomEntrySubject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // JoinsSubjects::CustomEntrySubject_2
	{ reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects_CustomEntrySubject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // JoinsSubjects::CustomEntrySubject_3
	{ reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_3_b__4_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // JoinsSubjects+<>c::<CustomEntrySubject_3>b__4_0
	{ reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects_CustomEntrySubject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // JoinsSubjects::CustomEntrySubject_4
	{ reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass5_0_CustomEntrySubject_4_b__1_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // JoinsSubjects+<>c__DisplayClass5_0::<CustomEntrySubject_4>b__1
	{ reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass5_0_CustomEntrySubject_4_b__2_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // JoinsSubjects+<>c__DisplayClass5_0::<CustomEntrySubject_4>b__2
	{ reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_4_b__5_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // JoinsSubjects+<>c::<CustomEntrySubject_4>b__5_0
	{ reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects_CustomEntrySubject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // JoinsSubjects::CustomEntrySubject_5
	{ reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass6_0_CustomEntrySubject_5_b__1_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // JoinsSubjects+<>c__DisplayClass6_0::<CustomEntrySubject_5>b__1
	{ reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass6_0_CustomEntrySubject_5_b__2_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // JoinsSubjects+<>c__DisplayClass6_0::<CustomEntrySubject_5>b__2
	{ reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_5_b__6_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // JoinsSubjects+<>c::<CustomEntrySubject_5>b__6_0
	{ reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects_CustomEntrySubject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // JoinsSubjects::CustomEntrySubject_6
	{ reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass7_0_CustomEntrySubject_6_b__1_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // JoinsSubjects+<>c__DisplayClass7_0::<CustomEntrySubject_6>b__1
	{ reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass7_0_CustomEntrySubject_6_b__2_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // JoinsSubjects+<>c__DisplayClass7_0::<CustomEntrySubject_6>b__2
	{ reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_6_b__7_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // JoinsSubjects+<>c::<CustomEntrySubject_6>b__7_0
	{ reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects_CustomEntrySubject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // JoinsSubjects::CustomEntrySubject_7
	{ reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass8_0_CustomEntrySubject_7_b__1_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // JoinsSubjects+<>c__DisplayClass8_0::<CustomEntrySubject_7>b__1
	{ reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass8_0_CustomEntrySubject_7_b__2_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // JoinsSubjects+<>c__DisplayClass8_0::<CustomEntrySubject_7>b__2
	{ reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_7_b__8_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // JoinsSubjects+<>c::<CustomEntrySubject_7>b__8_0
	{ reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass5_0__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // JoinsSubjects+<>c__DisplayClass5_0::.ctor
	{ reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass6_0__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // JoinsSubjects+<>c__DisplayClass6_0::.ctor
	{ reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass7_0__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // JoinsSubjects+<>c__DisplayClass7_0::.ctor
	{ reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass8_0__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // JoinsSubjects+<>c__DisplayClass8_0::.ctor
	{ reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c_cctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // JoinsSubjects+<>c::.cctor
	{ reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // JoinsSubjects+<>c::.ctor
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"JoinsSubjects",
	s_hotpatch_types,
	6u,
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

extern "C" const char* kChaosExternalRuntimeSubjects[35] = {
	"System.Private.CoreLib/System.Int32",
	"JoinsSubjects/<PrivateImplementationDetails>::D8D4F55129E2966F2C74F35D6AC1769A713230A8B8371D030F18C6DEA1AAFD77",
	"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)",
	"JoinsSubjects/JoinsSubjects+<>c::<>9__1_0",
	"JoinsSubjects/JoinsSubjects+<>c::<>9",
	"System.Private.CoreLib/System.Func<System.Int32,System.Int32>::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Linq/Enumerable::GroupBy<System.Int32,System.Int32>:System.Collections.Generic.IEnumerable<IGrouping<System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)",
	"System.Private.CoreLib/System.Collections.Generic.IEnumerable<IGrouping<System.Int32,System.Int32>>::GetEnumerator:System.Collections.Generic.IEnumerator<IGrouping<System.Int32,System.Int32>>()",
	"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()",
	"JoinsSubjects/JoinsSubjects+<>c::<>9__2_0",
	"System.Collections/System.Collections.Generic.EqualityComparer<System.Int32>::get_Default:System.Collections.Generic.EqualityComparer<System.Int32>()",
	"System.Linq/Enumerable::GroupBy<System.Int32,System.Int32>:System.Collections.Generic.IEnumerable<IGrouping<System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)",
	"JoinsSubjects/<PrivateImplementationDetails>::CBF03EFCA28AF3C84560CE82FEAE66B8AE0A9321EC14BE8F40D06EB0AD7BFE73",
	"System.Linq/Enumerable::Distinct<System.Int32>:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>)",
	"System.Private.CoreLib/System.Collections.Generic.IEnumerable<System.Int32>::GetEnumerator:System.Collections.Generic.IEnumerator<System.Int32>()",
	"JoinsSubjects/<PrivateImplementationDetails>::A0FEBE8D859AC7FE1E225E18F8632CFA942D34C2F58386AE0B982B55FD4EB116",
	"JoinsSubjects/JoinsSubjects+<>c::<>9__4_0",
	"System.Linq/Enumerable::OrderBy<System.Int32,System.Int32>:IOrderedEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)",
	"JoinsSubjects/<PrivateImplementationDetails>::4636993D3E1DA4E9D6B8F87B79E8F7C6D018580D52661950EABC3845C5897A4D",
	"JoinsSubjects/JoinsSubjects+<>c::<>9__5_0",
	"System.Private.CoreLib/System.Func<System.Int32,System.Collections.Generic.IEnumerable<System.Int32>>::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Linq/Enumerable::SelectMany<System.Int32,System.Int32>:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Collections.Generic.IEnumerable<System.Int32>>)",
	"JoinsSubjects/JoinsSubjects+<>c__DisplayClass5_0::o",
	"JoinsSubjects/<PrivateImplementationDetails>::A10494D90314704E24CA5786F6376A6097558F10BB7880B539C8B80312DCA080",
	"System.Private.CoreLib/System.Func<System.Int32,System.Boolean>::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Linq/Enumerable::Where<System.Int32>:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)",
	"System.Linq/Enumerable::DefaultIfEmpty<System.Int32>:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)",
	"System.Linq/Enumerable::Select<System.Int32,System.Int32>:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)",
	"JoinsSubjects/JoinsSubjects+<>c::<>9__6_0",
	"JoinsSubjects/JoinsSubjects+<>c__DisplayClass6_0::o",
	"JoinsSubjects/JoinsSubjects+<>c::<>9__7_0",
	"JoinsSubjects/JoinsSubjects+<>c__DisplayClass7_0::i",
	"JoinsSubjects/JoinsSubjects+<>c::<>9__8_0",
	"JoinsSubjects/JoinsSubjects+<>c__DisplayClass8_0::i",
	"System.Private.CoreLib/System.Object::.ctor:System.Void()",
};

extern "C" void* kChaosExternalRuntimeFnTable[35] = {
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
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__),
};

extern "C" int32_t kChaosExternalRuntimeCount = 35;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[29])() = {
	reinterpret_cast<void(*)()>(&JoinsSubjects_JoinsSubjects_CustomEntrySubject_0),
	reinterpret_cast<void(*)()>(&JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_0_b__1_0_System_Int32),
	reinterpret_cast<void(*)()>(&JoinsSubjects_JoinsSubjects_CustomEntrySubject_1),
	reinterpret_cast<void(*)()>(&JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_1_b__2_0_System_Int32),
	reinterpret_cast<void(*)()>(&JoinsSubjects_JoinsSubjects_CustomEntrySubject_2),
	reinterpret_cast<void(*)()>(&JoinsSubjects_JoinsSubjects_CustomEntrySubject_3),
	reinterpret_cast<void(*)()>(&JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_3_b__4_0_System_Int32),
	reinterpret_cast<void(*)()>(&JoinsSubjects_JoinsSubjects_CustomEntrySubject_4),
	reinterpret_cast<void(*)()>(&JoinsSubjects_JoinsSubjects___c__DisplayClass5_0_CustomEntrySubject_4_b__1_System_Int32),
	reinterpret_cast<void(*)()>(&JoinsSubjects_JoinsSubjects___c__DisplayClass5_0_CustomEntrySubject_4_b__2_System_Int32),
	reinterpret_cast<void(*)()>(&JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_4_b__5_0_System_Int32),
	reinterpret_cast<void(*)()>(&JoinsSubjects_JoinsSubjects_CustomEntrySubject_5),
	reinterpret_cast<void(*)()>(&JoinsSubjects_JoinsSubjects___c__DisplayClass6_0_CustomEntrySubject_5_b__1_System_Int32),
	reinterpret_cast<void(*)()>(&JoinsSubjects_JoinsSubjects___c__DisplayClass6_0_CustomEntrySubject_5_b__2_System_Int32),
	reinterpret_cast<void(*)()>(&JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_5_b__6_0_System_Int32),
	reinterpret_cast<void(*)()>(&JoinsSubjects_JoinsSubjects_CustomEntrySubject_6),
	reinterpret_cast<void(*)()>(&JoinsSubjects_JoinsSubjects___c__DisplayClass7_0_CustomEntrySubject_6_b__1_System_Int32),
	reinterpret_cast<void(*)()>(&JoinsSubjects_JoinsSubjects___c__DisplayClass7_0_CustomEntrySubject_6_b__2_System_Int32),
	reinterpret_cast<void(*)()>(&JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_6_b__7_0_System_Int32),
	reinterpret_cast<void(*)()>(&JoinsSubjects_JoinsSubjects_CustomEntrySubject_7),
	reinterpret_cast<void(*)()>(&JoinsSubjects_JoinsSubjects___c__DisplayClass8_0_CustomEntrySubject_7_b__1_System_Int32),
	reinterpret_cast<void(*)()>(&JoinsSubjects_JoinsSubjects___c__DisplayClass8_0_CustomEntrySubject_7_b__2_System_Int32),
	reinterpret_cast<void(*)()>(&JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_7_b__8_0_System_Int32),
	reinterpret_cast<void(*)()>(&JoinsSubjects_JoinsSubjects___c__DisplayClass5_0__ctor),
	reinterpret_cast<void(*)()>(&JoinsSubjects_JoinsSubjects___c__DisplayClass6_0__ctor),
	reinterpret_cast<void(*)()>(&JoinsSubjects_JoinsSubjects___c__DisplayClass7_0__ctor),
	reinterpret_cast<void(*)()>(&JoinsSubjects_JoinsSubjects___c__DisplayClass8_0__ctor),
	reinterpret_cast<void(*)()>(&JoinsSubjects_JoinsSubjects___c_cctor),
	reinterpret_cast<void(*)()>(&JoinsSubjects_JoinsSubjects___c__ctor),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
extern "C" void (*kBenchmarkWrappers[29])() = {
	[]() {kAotMethods[0]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[1])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[2]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[3])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[4]();},
	[]() {kAotMethods[5]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[6])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[7]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[8])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[9])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[10])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[11]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[12])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[13])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[14])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[15]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[16])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[17])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[18])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[19]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[20])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[21])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[22])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[23])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[24])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[25])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[26])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {kAotMethods[27]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[28])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
};

// ── Subject entry index mapping ─────────────────────────────────
// Maps subject index (0-based sequential) to kAotMethod index.
// Used by runtime-entry.cpp to route --benchmark N to the correct
// AOT method slot, since kAotMethods[] includes lambdas/closures
// that shift subject methods to non-contiguous indices.
extern "C" const int kSubjectEntryCount = 8;
extern "C" const int kSubjectEntryIndices[8] = {
	0,
	2,
	4,
	5,
	7,
	11,
	15,
	19,
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

	/* ── Entry 0: JoinsSubjects_JoinsSubjects_CustomEntrySubject_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry0;
	/* ── Entry 1: JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_0_b__1_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry1;
	/* ── Entry 2: JoinsSubjects_JoinsSubjects_CustomEntrySubject_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry2;
	/* ── Entry 3: JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_1_b__2_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry3;
	/* ── Entry 4: JoinsSubjects_JoinsSubjects_CustomEntrySubject_2 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry4;
	/* ── Entry 5: JoinsSubjects_JoinsSubjects_CustomEntrySubject_3 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry5;
	/* ── Entry 6: JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_3_b__4_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry6;
	/* ── Entry 7: JoinsSubjects_JoinsSubjects_CustomEntrySubject_4 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry7;
	/* ── Entry 8: JoinsSubjects_JoinsSubjects___c__DisplayClass5_0_CustomEntrySubject_4_b__1_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry8;
	/* ── Entry 9: JoinsSubjects_JoinsSubjects___c__DisplayClass5_0_CustomEntrySubject_4_b__2_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry9;
	/* ── Entry 10: JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_4_b__5_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry10;
	/* ── Entry 11: JoinsSubjects_JoinsSubjects_CustomEntrySubject_5 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry11;
	/* ── Entry 12: JoinsSubjects_JoinsSubjects___c__DisplayClass6_0_CustomEntrySubject_5_b__1_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry12;
	/* ── Entry 13: JoinsSubjects_JoinsSubjects___c__DisplayClass6_0_CustomEntrySubject_5_b__2_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry13;
	/* ── Entry 14: JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_5_b__6_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry14;
	/* ── Entry 15: JoinsSubjects_JoinsSubjects_CustomEntrySubject_6 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry15;
	/* ── Entry 16: JoinsSubjects_JoinsSubjects___c__DisplayClass7_0_CustomEntrySubject_6_b__1_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry16;
	/* ── Entry 17: JoinsSubjects_JoinsSubjects___c__DisplayClass7_0_CustomEntrySubject_6_b__2_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry17;
	/* ── Entry 18: JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_6_b__7_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry18;
	/* ── Entry 19: JoinsSubjects_JoinsSubjects_CustomEntrySubject_7 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry19;
	/* ── Entry 20: JoinsSubjects_JoinsSubjects___c__DisplayClass8_0_CustomEntrySubject_7_b__1_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry20;
	/* ── Entry 21: JoinsSubjects_JoinsSubjects___c__DisplayClass8_0_CustomEntrySubject_7_b__2_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry21;
	/* ── Entry 22: JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_7_b__8_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry22;
	/* ── Entry 23: JoinsSubjects_JoinsSubjects___c__DisplayClass5_0__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry23;
	/* ── Entry 24: JoinsSubjects_JoinsSubjects___c__DisplayClass6_0__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry24;
	/* ── Entry 25: JoinsSubjects_JoinsSubjects___c__DisplayClass7_0__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry25;
	/* ── Entry 26: JoinsSubjects_JoinsSubjects___c__DisplayClass8_0__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry26;
	/* ── Entry 27: JoinsSubjects_JoinsSubjects___c_cctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry27;
	/* ── Entry 28: JoinsSubjects_JoinsSubjects___c__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry28;
} kChaosGcSlotMapsSection = {
	/* entry0 = JoinsSubjects_JoinsSubjects_CustomEntrySubject_0 */
	.entry0 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&JoinsSubjects_JoinsSubjects_CustomEntrySubject_0),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry1 = JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_0_b__1_0_System_Int32 */
	.entry1 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_0_b__1_0_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 16u, 24u }
	},
	/* entry2 = JoinsSubjects_JoinsSubjects_CustomEntrySubject_1 */
	.entry2 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&JoinsSubjects_JoinsSubjects_CustomEntrySubject_1),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry3 = JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_1_b__2_0_System_Int32 */
	.entry3 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_1_b__2_0_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 16u, 24u }
	},
	/* entry4 = JoinsSubjects_JoinsSubjects_CustomEntrySubject_2 */
	.entry4 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&JoinsSubjects_JoinsSubjects_CustomEntrySubject_2),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry5 = JoinsSubjects_JoinsSubjects_CustomEntrySubject_3 */
	.entry5 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&JoinsSubjects_JoinsSubjects_CustomEntrySubject_3),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry6 = JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_3_b__4_0_System_Int32 */
	.entry6 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_3_b__4_0_System_Int32),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 16u }
	},
	/* entry7 = JoinsSubjects_JoinsSubjects_CustomEntrySubject_4 */
	.entry7 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&JoinsSubjects_JoinsSubjects_CustomEntrySubject_4),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry8 = JoinsSubjects_JoinsSubjects___c__DisplayClass5_0_CustomEntrySubject_4_b__1_System_Int32 */
	.entry8 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass5_0_CustomEntrySubject_4_b__1_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 16u, 24u, 32u }
	},
	/* entry9 = JoinsSubjects_JoinsSubjects___c__DisplayClass5_0_CustomEntrySubject_4_b__2_System_Int32 */
	.entry9 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass5_0_CustomEntrySubject_4_b__2_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 16u, 24u, 32u }
	},
	/* entry10 = JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_4_b__5_0_System_Int32 */
	.entry10 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_4_b__5_0_System_Int32),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry11 = JoinsSubjects_JoinsSubjects_CustomEntrySubject_5 */
	.entry11 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&JoinsSubjects_JoinsSubjects_CustomEntrySubject_5),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry12 = JoinsSubjects_JoinsSubjects___c__DisplayClass6_0_CustomEntrySubject_5_b__1_System_Int32 */
	.entry12 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass6_0_CustomEntrySubject_5_b__1_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 16u, 24u, 32u }
	},
	/* entry13 = JoinsSubjects_JoinsSubjects___c__DisplayClass6_0_CustomEntrySubject_5_b__2_System_Int32 */
	.entry13 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass6_0_CustomEntrySubject_5_b__2_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 16u, 24u, 32u }
	},
	/* entry14 = JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_5_b__6_0_System_Int32 */
	.entry14 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_5_b__6_0_System_Int32),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry15 = JoinsSubjects_JoinsSubjects_CustomEntrySubject_6 */
	.entry15 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&JoinsSubjects_JoinsSubjects_CustomEntrySubject_6),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry16 = JoinsSubjects_JoinsSubjects___c__DisplayClass7_0_CustomEntrySubject_6_b__1_System_Int32 */
	.entry16 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass7_0_CustomEntrySubject_6_b__1_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 16u, 24u, 32u }
	},
	/* entry17 = JoinsSubjects_JoinsSubjects___c__DisplayClass7_0_CustomEntrySubject_6_b__2_System_Int32 */
	.entry17 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass7_0_CustomEntrySubject_6_b__2_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 16u, 24u, 32u }
	},
	/* entry18 = JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_6_b__7_0_System_Int32 */
	.entry18 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_6_b__7_0_System_Int32),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry19 = JoinsSubjects_JoinsSubjects_CustomEntrySubject_7 */
	.entry19 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&JoinsSubjects_JoinsSubjects_CustomEntrySubject_7),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry20 = JoinsSubjects_JoinsSubjects___c__DisplayClass8_0_CustomEntrySubject_7_b__1_System_Int32 */
	.entry20 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass8_0_CustomEntrySubject_7_b__1_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 16u, 24u, 32u }
	},
	/* entry21 = JoinsSubjects_JoinsSubjects___c__DisplayClass8_0_CustomEntrySubject_7_b__2_System_Int32 */
	.entry21 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass8_0_CustomEntrySubject_7_b__2_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 16u, 24u, 32u }
	},
	/* entry22 = JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_7_b__8_0_System_Int32 */
	.entry22 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_7_b__8_0_System_Int32),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry23 = JoinsSubjects_JoinsSubjects___c__DisplayClass5_0__ctor */
	.entry23 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass5_0__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry24 = JoinsSubjects_JoinsSubjects___c__DisplayClass6_0__ctor */
	.entry24 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass6_0__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry25 = JoinsSubjects_JoinsSubjects___c__DisplayClass7_0__ctor */
	.entry25 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass7_0__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry26 = JoinsSubjects_JoinsSubjects___c__DisplayClass8_0__ctor */
	.entry26 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass8_0__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry27 = JoinsSubjects_JoinsSubjects___c_cctor */
	.entry27 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&JoinsSubjects_JoinsSubjects___c_cctor),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry28 = JoinsSubjects_JoinsSubjects___c__ctor */
	.entry28 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&JoinsSubjects_JoinsSubjects___c__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 1008u;

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[29] = {
	reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects_CustomEntrySubject_0),
	reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_0_b__1_0_System_Int32),
	reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects_CustomEntrySubject_1),
	reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_1_b__2_0_System_Int32),
	reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects_CustomEntrySubject_2),
	reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects_CustomEntrySubject_3),
	reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_3_b__4_0_System_Int32),
	reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects_CustomEntrySubject_4),
	reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass5_0_CustomEntrySubject_4_b__1_System_Int32),
	reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass5_0_CustomEntrySubject_4_b__2_System_Int32),
	reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_4_b__5_0_System_Int32),
	reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects_CustomEntrySubject_5),
	reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass6_0_CustomEntrySubject_5_b__1_System_Int32),
	reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass6_0_CustomEntrySubject_5_b__2_System_Int32),
	reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_5_b__6_0_System_Int32),
	reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects_CustomEntrySubject_6),
	reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass7_0_CustomEntrySubject_6_b__1_System_Int32),
	reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass7_0_CustomEntrySubject_6_b__2_System_Int32),
	reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_6_b__7_0_System_Int32),
	reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects_CustomEntrySubject_7),
	reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass8_0_CustomEntrySubject_7_b__1_System_Int32),
	reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass8_0_CustomEntrySubject_7_b__2_System_Int32),
	reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_7_b__8_0_System_Int32),
	reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass5_0__ctor),
	reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass6_0__ctor),
	reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass7_0__ctor),
	reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c__DisplayClass8_0__ctor),
	reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c_cctor),
	reinterpret_cast<void*>(&JoinsSubjects_JoinsSubjects___c__ctor),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
// ── VTable descriptors (for BootstrapRuntime TypeVTable registration) ──
static const VTableDescriptorV0 kChaosVTableDescriptors[] = {
	{
		CHAOS_IL2CPP_UINT64_C(0x4BFA144754094DDC),
		0x00000002u,
		0u,
		8u,
		reinterpret_cast<const void*>(kSlots_JoinsSubjects_JoinsSubjects___c),
		reinterpret_cast<const void**>(chaos_vtable_JoinsSubjects_JoinsSubjects___c),
		8u,
		1,
		{0, 0, 0},
		nullptr,
		0u,
	},
	{
		CHAOS_IL2CPP_UINT64_C(0x4F975618E4286CD0),
		0x00000003u,
		0u,
		10u,
		reinterpret_cast<const void*>(kSlots_JoinsSubjects_JoinsSubjects___c__DisplayClass5_0),
		reinterpret_cast<const void**>(chaos_vtable_JoinsSubjects_JoinsSubjects___c__DisplayClass5_0),
		10u,
		1,
		{0, 0, 0},
		nullptr,
		0u,
	},
	{
		CHAOS_IL2CPP_UINT64_C(0x56E60D18E7E9B2C5),
		0x00000004u,
		0u,
		12u,
		reinterpret_cast<const void*>(kSlots_JoinsSubjects_JoinsSubjects___c__DisplayClass6_0),
		reinterpret_cast<const void**>(chaos_vtable_JoinsSubjects_JoinsSubjects___c__DisplayClass6_0),
		12u,
		1,
		{0, 0, 0},
		nullptr,
		0u,
	},
	{
		CHAOS_IL2CPP_UINT64_C(0x619AC418EE8E21BA),
		0x00000005u,
		0u,
		14u,
		reinterpret_cast<const void*>(kSlots_JoinsSubjects_JoinsSubjects___c__DisplayClass7_0),
		reinterpret_cast<const void**>(chaos_vtable_JoinsSubjects_JoinsSubjects___c__DisplayClass7_0),
		14u,
		1,
		{0, 0, 0},
		nullptr,
		0u,
	},
	{
		CHAOS_IL2CPP_UINT64_C(0x69C35B18F308D50F),
		0x00000006u,
		0u,
		16u,
		reinterpret_cast<const void*>(kSlots_JoinsSubjects_JoinsSubjects___c__DisplayClass8_0),
		reinterpret_cast<const void**>(chaos_vtable_JoinsSubjects_JoinsSubjects___c__DisplayClass8_0),
		16u,
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
	.vtable_descriptor_count = 5u,
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
	.image_name_utf8    = "JoinsSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_JoinsSubjects_JoinsSubjects[1] = {
	{ 0u, "JoinsSubjects/JoinsSubjects::_exitCode", "_exitCode", "System.Int32", 0LL },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_JoinsSubjects_JoinsSubjects[8] = {
	{ 0u, "JoinsSubjects/JoinsSubjects::CustomEntrySubject_0:System.Void()", "CustomEntrySubject_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "JoinsSubjects/JoinsSubjects::CustomEntrySubject_1:System.Void()", "CustomEntrySubject_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "JoinsSubjects/JoinsSubjects::CustomEntrySubject_2:System.Void()", "CustomEntrySubject_2", "System.Void", 0, nullptr, 0u },
	{ 0u, "JoinsSubjects/JoinsSubjects::CustomEntrySubject_3:System.Void()", "CustomEntrySubject_3", "System.Void", 0, nullptr, 0u },
	{ 0u, "JoinsSubjects/JoinsSubjects::CustomEntrySubject_4:System.Void()", "CustomEntrySubject_4", "System.Void", 0, nullptr, 0u },
	{ 0u, "JoinsSubjects/JoinsSubjects::CustomEntrySubject_5:System.Void()", "CustomEntrySubject_5", "System.Void", 0, nullptr, 0u },
	{ 0u, "JoinsSubjects/JoinsSubjects::CustomEntrySubject_6:System.Void()", "CustomEntrySubject_6", "System.Void", 0, nullptr, 0u },
	{ 0u, "JoinsSubjects/JoinsSubjects::CustomEntrySubject_7:System.Void()", "CustomEntrySubject_7", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_JoinsSubjects_JoinsSubjects___c[9] = {
	{ 0u, "JoinsSubjects/JoinsSubjects+<>c::<CustomEntrySubject_0>b__1_0:System.Int32(System.Int32)", "<CustomEntrySubject_0>b__1_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "JoinsSubjects/JoinsSubjects+<>c::<CustomEntrySubject_1>b__2_0:System.Int32(System.Int32)", "<CustomEntrySubject_1>b__2_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "JoinsSubjects/JoinsSubjects+<>c::<CustomEntrySubject_3>b__4_0:System.Int32(System.Int32)", "<CustomEntrySubject_3>b__4_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "JoinsSubjects/JoinsSubjects+<>c::<CustomEntrySubject_4>b__5_0:System.Collections.Generic.IEnumerable<System.Int32>(System.Int32)", "<CustomEntrySubject_4>b__5_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "JoinsSubjects/JoinsSubjects+<>c::<CustomEntrySubject_5>b__6_0:System.Collections.Generic.IEnumerable<System.Int32>(System.Int32)", "<CustomEntrySubject_5>b__6_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "JoinsSubjects/JoinsSubjects+<>c::<CustomEntrySubject_6>b__7_0:System.Collections.Generic.IEnumerable<System.Int32>(System.Int32)", "<CustomEntrySubject_6>b__7_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "JoinsSubjects/JoinsSubjects+<>c::<CustomEntrySubject_7>b__8_0:System.Collections.Generic.IEnumerable<System.Int32>(System.Int32)", "<CustomEntrySubject_7>b__8_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "JoinsSubjects/JoinsSubjects+<>c::.cctor:System.Void()", ".cctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "JoinsSubjects/JoinsSubjects+<>c::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_JoinsSubjects_JoinsSubjects___c__DisplayClass5_0[3] = {
	{ 0u, "JoinsSubjects/JoinsSubjects+<>c__DisplayClass5_0::<CustomEntrySubject_4>b__1:System.Boolean(System.Int32)", "<CustomEntrySubject_4>b__1", "System.Void", 0, nullptr, 0u },
	{ 0u, "JoinsSubjects/JoinsSubjects+<>c__DisplayClass5_0::<CustomEntrySubject_4>b__2:System.Int32(System.Int32)", "<CustomEntrySubject_4>b__2", "System.Void", 0, nullptr, 0u },
	{ 0u, "JoinsSubjects/JoinsSubjects+<>c__DisplayClass5_0::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_JoinsSubjects_JoinsSubjects___c__DisplayClass6_0[3] = {
	{ 0u, "JoinsSubjects/JoinsSubjects+<>c__DisplayClass6_0::<CustomEntrySubject_5>b__1:System.Boolean(System.Int32)", "<CustomEntrySubject_5>b__1", "System.Void", 0, nullptr, 0u },
	{ 0u, "JoinsSubjects/JoinsSubjects+<>c__DisplayClass6_0::<CustomEntrySubject_5>b__2:System.Int32(System.Int32)", "<CustomEntrySubject_5>b__2", "System.Void", 0, nullptr, 0u },
	{ 0u, "JoinsSubjects/JoinsSubjects+<>c__DisplayClass6_0::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_JoinsSubjects_JoinsSubjects___c__DisplayClass7_0[3] = {
	{ 0u, "JoinsSubjects/JoinsSubjects+<>c__DisplayClass7_0::<CustomEntrySubject_6>b__1:System.Boolean(System.Int32)", "<CustomEntrySubject_6>b__1", "System.Void", 0, nullptr, 0u },
	{ 0u, "JoinsSubjects/JoinsSubjects+<>c__DisplayClass7_0::<CustomEntrySubject_6>b__2:System.Int32(System.Int32)", "<CustomEntrySubject_6>b__2", "System.Void", 0, nullptr, 0u },
	{ 0u, "JoinsSubjects/JoinsSubjects+<>c__DisplayClass7_0::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_JoinsSubjects_JoinsSubjects___c__DisplayClass8_0[3] = {
	{ 0u, "JoinsSubjects/JoinsSubjects+<>c__DisplayClass8_0::<CustomEntrySubject_7>b__1:System.Boolean(System.Int32)", "<CustomEntrySubject_7>b__1", "System.Void", 0, nullptr, 0u },
	{ 0u, "JoinsSubjects/JoinsSubjects+<>c__DisplayClass8_0::<CustomEntrySubject_7>b__2:System.Int32(System.Int32)", "<CustomEntrySubject_7>b__2", "System.Void", 0, nullptr, 0u },
	{ 0u, "JoinsSubjects/JoinsSubjects+<>c__DisplayClass8_0::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[6] = {
	{ 0u, "JoinsSubjects/JoinsSubjects", "JoinsSubjects/JoinsSubjects", "", "JoinsSubjects", "JoinsSubjects", nullptr, kReflFields_JoinsSubjects_JoinsSubjects, 1u, nullptr, 0u,
	kReflMethods_JoinsSubjects_JoinsSubjects, 8u },
	{ 0u, "JoinsSubjects/JoinsSubjects+<>c", "JoinsSubjects/JoinsSubjects+<>c", "", "JoinsSubjects+<>c", "JoinsSubjects+<>c", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_JoinsSubjects_JoinsSubjects___c, 9u },
	{ 0u, "JoinsSubjects/JoinsSubjects+<>c__DisplayClass5_0", "JoinsSubjects/JoinsSubjects+<>c__DisplayClass5_0", "", "JoinsSubjects+<>c__DisplayClass5_0", "JoinsSubjects+<>c__DisplayClass5_0", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_JoinsSubjects_JoinsSubjects___c__DisplayClass5_0, 3u },
	{ 0u, "JoinsSubjects/JoinsSubjects+<>c__DisplayClass6_0", "JoinsSubjects/JoinsSubjects+<>c__DisplayClass6_0", "", "JoinsSubjects+<>c__DisplayClass6_0", "JoinsSubjects+<>c__DisplayClass6_0", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_JoinsSubjects_JoinsSubjects___c__DisplayClass6_0, 3u },
	{ 0u, "JoinsSubjects/JoinsSubjects+<>c__DisplayClass7_0", "JoinsSubjects/JoinsSubjects+<>c__DisplayClass7_0", "", "JoinsSubjects+<>c__DisplayClass7_0", "JoinsSubjects+<>c__DisplayClass7_0", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_JoinsSubjects_JoinsSubjects___c__DisplayClass7_0, 3u },
	{ 0u, "JoinsSubjects/JoinsSubjects+<>c__DisplayClass8_0", "JoinsSubjects/JoinsSubjects+<>c__DisplayClass8_0", "", "JoinsSubjects+<>c__DisplayClass8_0", "JoinsSubjects+<>c__DisplayClass8_0", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_JoinsSubjects_JoinsSubjects___c__DisplayClass8_0, 3u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[6] = {
	&kReflTypes[0],
	&kReflTypes[1],
	&kReflTypes[2],
	&kReflTypes[3],
	&kReflTypes[4],
	&kReflTypes[5],
};

static const ReflectionQueryImageDescriptor kReflImage = { "JoinsSubjects", kReflTypePtrs, 6u };

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
// Managed method: JoinsSubjects/JoinsSubjects::CustomEntrySubject_0()
extern "C" void JoinsSubjects_JoinsSubjects_CustomEntrySubject_0(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(78109589u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_JoinsSubjects_JoinsSubjects___c();
	_s1 = chaos_static_JoinsSubjects_JoinsSubjects___c____9__1_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_JoinsSubjects_JoinsSubjects___c();
			_s1 = chaos_static_JoinsSubjects_JoinsSubjects___c____9;
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
			chaos_ensure_type_initialized_JoinsSubjects_JoinsSubjects___c();
				chaos_static_JoinsSubjects_JoinsSubjects___c____9__1_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[6])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[7])();
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[8])();
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: JoinsSubjects/JoinsSubjects+<>c::<CustomEntrySubject_0>b__1_0(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_0_b__1_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
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
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosRem(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: JoinsSubjects/JoinsSubjects::CustomEntrySubject_1()
extern "C" void JoinsSubjects_JoinsSubjects_CustomEntrySubject_1(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(78109589u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_JoinsSubjects_JoinsSubjects___c();
	_s1 = chaos_static_JoinsSubjects_JoinsSubjects___c____9__2_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_JoinsSubjects_JoinsSubjects___c();
			_s1 = chaos_static_JoinsSubjects_JoinsSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 3)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INT32 {
					auto& _d_entry = s_hotpatch_entries[3];
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
			chaos_ensure_type_initialized_JoinsSubjects_JoinsSubjects___c();
				chaos_static_JoinsSubjects_JoinsSubjects___c____9__2_0 = chaos_value;
			}
		}
		{
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[10])();
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			const auto chaos_arg_2 = _s6;
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[11])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[7])();
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[8])();
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: JoinsSubjects/JoinsSubjects+<>c::<CustomEntrySubject_1>b__2_0(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_1_b__2_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
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
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosRem(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: JoinsSubjects/JoinsSubjects::CustomEntrySubject_2()
extern "C" void JoinsSubjects_JoinsSubjects_CustomEntrySubject_2(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(6);
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(77833573u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[13])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[14])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[8])();
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: JoinsSubjects/JoinsSubjects::CustomEntrySubject_3()
extern "C" void JoinsSubjects_JoinsSubjects_CustomEntrySubject_3(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(67360599u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_JoinsSubjects_JoinsSubjects___c();
	_s1 = chaos_static_JoinsSubjects_JoinsSubjects___c____9__4_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_JoinsSubjects_JoinsSubjects___c();
			_s1 = chaos_static_JoinsSubjects_JoinsSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 6)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INT32 {
					auto& _d_entry = s_hotpatch_entries[6];
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
			chaos_ensure_type_initialized_JoinsSubjects_JoinsSubjects___c();
				chaos_static_JoinsSubjects_JoinsSubjects___c____9__4_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[17])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[14])();
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[8])();
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: JoinsSubjects/JoinsSubjects+<>c::<CustomEntrySubject_3>b__4_0(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_3_b__4_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: JoinsSubjects/JoinsSubjects::CustomEntrySubject_4()
extern "C" void JoinsSubjects_JoinsSubjects_CustomEntrySubject_4(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(77819907u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_JoinsSubjects_JoinsSubjects___c();
	_s1 = chaos_static_JoinsSubjects_JoinsSubjects___c____9__5_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_JoinsSubjects_JoinsSubjects___c();
			_s1 = chaos_static_JoinsSubjects_JoinsSubjects___c____9;
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
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Collections_Generic_IEnumerable_System_Int32__, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Collections_Generic_IEnumerable_System_Int32__.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_JoinsSubjects_JoinsSubjects___c();
				chaos_static_JoinsSubjects_JoinsSubjects___c____9__5_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[21])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[14])();
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[8])();
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: JoinsSubjects/JoinsSubjects+<>c__DisplayClass5_0::<CustomEntrySubject_4>b__1(System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR JoinsSubjects_JoinsSubjects___c__DisplayClass5_0_CustomEntrySubject_4_b__1_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
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
	_s1 = chaos_args[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_JoinsSubjects_JoinsSubjects___c__DisplayClass5_0*>(_s1);
		_s1 = chaos_object->field_JoinsSubjects_JoinsSubjects___c__DisplayClass5_0__o;
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	return _s0;
}

// Managed method: JoinsSubjects/JoinsSubjects+<>c__DisplayClass5_0::<CustomEntrySubject_4>b__2(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 JoinsSubjects_JoinsSubjects___c__DisplayClass5_0_CustomEntrySubject_4_b__2_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
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

	_s0 = chaos_args[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_JoinsSubjects_JoinsSubjects___c__DisplayClass5_0*>(_s0);
		_s0 = chaos_object->field_JoinsSubjects_JoinsSubjects___c__DisplayClass5_0__o;
	}
	_s1 = chaos_args[1];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: JoinsSubjects/JoinsSubjects+<>c::<CustomEntrySubject_4>b__5_0(System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_4_b__5_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
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
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_JoinsSubjects_JoinsSubjects___c__DisplayClass5_0, {});
		chaos_object->header.type_info = &chaos_mt_JoinsSubjects_JoinsSubjects___c__DisplayClass5_0.hot;
		JoinsSubjects_JoinsSubjects___c__DisplayClass5_0__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s1 = chaos_args[1];
	{
		auto chaos_value = _s1;
		auto* chaos_object = reinterpret_cast<chaos_type_JoinsSubjects_JoinsSubjects___c__DisplayClass5_0*>(_s0);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_object->field_JoinsSubjects_JoinsSubjects___c__DisplayClass5_0__o));
		chaos_object->field_JoinsSubjects_JoinsSubjects___c__DisplayClass5_0__o = chaos_value;
		chaos_gc_dirty_card(chaos_object);
	}
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(83683924u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	_s1 = chaos_locals[0];
	{
		// Hotpatch-aware ldftn wrapper (slot 8)
		static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
			auto& _d_entry = s_hotpatch_entries[8];
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
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[25])(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[26])(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = chaos_locals[0];
	{
		// Hotpatch-aware ldftn wrapper (slot 9)
		static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INT32 {
			auto& _d_entry = s_hotpatch_entries[9];
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
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[27])(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return _s0;
}

// Managed method: JoinsSubjects/JoinsSubjects::CustomEntrySubject_5()
extern "C" void JoinsSubjects_JoinsSubjects_CustomEntrySubject_5(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(77819907u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_JoinsSubjects_JoinsSubjects___c();
	_s1 = chaos_static_JoinsSubjects_JoinsSubjects___c____9__6_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_JoinsSubjects_JoinsSubjects___c();
			_s1 = chaos_static_JoinsSubjects_JoinsSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 14)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
					auto& _d_entry = s_hotpatch_entries[14];
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
			chaos_ensure_type_initialized_JoinsSubjects_JoinsSubjects___c();
				chaos_static_JoinsSubjects_JoinsSubjects___c____9__6_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[21])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[14])();
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[8])();
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: JoinsSubjects/JoinsSubjects+<>c__DisplayClass6_0::<CustomEntrySubject_5>b__1(System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR JoinsSubjects_JoinsSubjects___c__DisplayClass6_0_CustomEntrySubject_5_b__1_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
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
	_s1 = chaos_args[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_JoinsSubjects_JoinsSubjects___c__DisplayClass6_0*>(_s1);
		_s1 = chaos_object->field_JoinsSubjects_JoinsSubjects___c__DisplayClass6_0__o;
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	return _s0;
}

// Managed method: JoinsSubjects/JoinsSubjects+<>c__DisplayClass6_0::<CustomEntrySubject_5>b__2(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 JoinsSubjects_JoinsSubjects___c__DisplayClass6_0_CustomEntrySubject_5_b__2_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
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

	_s0 = chaos_args[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_JoinsSubjects_JoinsSubjects___c__DisplayClass6_0*>(_s0);
		_s0 = chaos_object->field_JoinsSubjects_JoinsSubjects___c__DisplayClass6_0__o;
	}
	_s1 = chaos_args[1];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: JoinsSubjects/JoinsSubjects+<>c::<CustomEntrySubject_5>b__6_0(System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_5_b__6_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
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
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_JoinsSubjects_JoinsSubjects___c__DisplayClass6_0, {});
		chaos_object->header.type_info = &chaos_mt_JoinsSubjects_JoinsSubjects___c__DisplayClass6_0.hot;
		JoinsSubjects_JoinsSubjects___c__DisplayClass6_0__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s1 = chaos_args[1];
	{
		auto chaos_value = _s1;
		auto* chaos_object = reinterpret_cast<chaos_type_JoinsSubjects_JoinsSubjects___c__DisplayClass6_0*>(_s0);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_object->field_JoinsSubjects_JoinsSubjects___c__DisplayClass6_0__o));
		chaos_object->field_JoinsSubjects_JoinsSubjects___c__DisplayClass6_0__o = chaos_value;
		chaos_gc_dirty_card(chaos_object);
	}
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(83683924u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	_s1 = chaos_locals[0];
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
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[25])(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[26])(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = chaos_locals[0];
	{
		// Hotpatch-aware ldftn wrapper (slot 13)
		static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INT32 {
			auto& _d_entry = s_hotpatch_entries[13];
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
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[27])(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return _s0;
}

// Managed method: JoinsSubjects/JoinsSubjects::CustomEntrySubject_6()
extern "C" void JoinsSubjects_JoinsSubjects_CustomEntrySubject_6(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(83683924u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_JoinsSubjects_JoinsSubjects___c();
	_s1 = chaos_static_JoinsSubjects_JoinsSubjects___c____9__7_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_JoinsSubjects_JoinsSubjects___c();
			_s1 = chaos_static_JoinsSubjects_JoinsSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 18)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
					auto& _d_entry = s_hotpatch_entries[18];
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
			chaos_ensure_type_initialized_JoinsSubjects_JoinsSubjects___c();
				chaos_static_JoinsSubjects_JoinsSubjects___c____9__7_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[21])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[14])();
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[8])();
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: JoinsSubjects/JoinsSubjects+<>c__DisplayClass7_0::<CustomEntrySubject_6>b__1(System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR JoinsSubjects_JoinsSubjects___c__DisplayClass7_0_CustomEntrySubject_6_b__1_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
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
	_s1 = chaos_args[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_JoinsSubjects_JoinsSubjects___c__DisplayClass7_0*>(_s1);
		_s1 = chaos_object->field_JoinsSubjects_JoinsSubjects___c__DisplayClass7_0__i;
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	return _s0;
}

// Managed method: JoinsSubjects/JoinsSubjects+<>c__DisplayClass7_0::<CustomEntrySubject_6>b__2(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 JoinsSubjects_JoinsSubjects___c__DisplayClass7_0_CustomEntrySubject_6_b__2_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
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
	_s1 = chaos_args[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_JoinsSubjects_JoinsSubjects___c__DisplayClass7_0*>(_s1);
		_s1 = chaos_object->field_JoinsSubjects_JoinsSubjects___c__DisplayClass7_0__i;
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: JoinsSubjects/JoinsSubjects+<>c::<CustomEntrySubject_6>b__7_0(System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_6_b__7_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
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
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_JoinsSubjects_JoinsSubjects___c__DisplayClass7_0, {});
		chaos_object->header.type_info = &chaos_mt_JoinsSubjects_JoinsSubjects___c__DisplayClass7_0.hot;
		JoinsSubjects_JoinsSubjects___c__DisplayClass7_0__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s1 = chaos_args[1];
	{
		auto chaos_value = _s1;
		auto* chaos_object = reinterpret_cast<chaos_type_JoinsSubjects_JoinsSubjects___c__DisplayClass7_0*>(_s0);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_object->field_JoinsSubjects_JoinsSubjects___c__DisplayClass7_0__i));
		chaos_object->field_JoinsSubjects_JoinsSubjects___c__DisplayClass7_0__i = chaos_value;
		chaos_gc_dirty_card(chaos_object);
	}
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(77819907u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	_s1 = chaos_locals[0];
	{
		// Hotpatch-aware ldftn wrapper (slot 16)
		static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
			auto& _d_entry = s_hotpatch_entries[16];
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
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[25])(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[26])(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = chaos_locals[0];
	{
		// Hotpatch-aware ldftn wrapper (slot 17)
		static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INT32 {
			auto& _d_entry = s_hotpatch_entries[17];
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
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[27])(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return _s0;
}

// Managed method: JoinsSubjects/JoinsSubjects::CustomEntrySubject_7()
extern "C" void JoinsSubjects_JoinsSubjects_CustomEntrySubject_7(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(83683924u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_JoinsSubjects_JoinsSubjects___c();
	_s1 = chaos_static_JoinsSubjects_JoinsSubjects___c____9__8_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_JoinsSubjects_JoinsSubjects___c();
			_s1 = chaos_static_JoinsSubjects_JoinsSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 22)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
					auto& _d_entry = s_hotpatch_entries[22];
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
			chaos_ensure_type_initialized_JoinsSubjects_JoinsSubjects___c();
				chaos_static_JoinsSubjects_JoinsSubjects___c____9__8_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[21])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[14])();
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[8])();
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: JoinsSubjects/JoinsSubjects+<>c__DisplayClass8_0::<CustomEntrySubject_7>b__1(System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR JoinsSubjects_JoinsSubjects___c__DisplayClass8_0_CustomEntrySubject_7_b__1_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
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
	_s1 = chaos_args[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_JoinsSubjects_JoinsSubjects___c__DisplayClass8_0*>(_s1);
		_s1 = chaos_object->field_JoinsSubjects_JoinsSubjects___c__DisplayClass8_0__i;
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	return _s0;
}

// Managed method: JoinsSubjects/JoinsSubjects+<>c__DisplayClass8_0::<CustomEntrySubject_7>b__2(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 JoinsSubjects_JoinsSubjects___c__DisplayClass8_0_CustomEntrySubject_7_b__2_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
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
	_s1 = chaos_args[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_JoinsSubjects_JoinsSubjects___c__DisplayClass8_0*>(_s1);
		_s1 = chaos_object->field_JoinsSubjects_JoinsSubjects___c__DisplayClass8_0__i;
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: JoinsSubjects/JoinsSubjects+<>c::<CustomEntrySubject_7>b__8_0(System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR JoinsSubjects_JoinsSubjects___c_CustomEntrySubject_7_b__8_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
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
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_JoinsSubjects_JoinsSubjects___c__DisplayClass8_0, {});
		chaos_object->header.type_info = &chaos_mt_JoinsSubjects_JoinsSubjects___c__DisplayClass8_0.hot;
		JoinsSubjects_JoinsSubjects___c__DisplayClass8_0__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s1 = chaos_args[1];
	{
		auto chaos_value = _s1;
		auto* chaos_object = reinterpret_cast<chaos_type_JoinsSubjects_JoinsSubjects___c__DisplayClass8_0*>(_s0);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_object->field_JoinsSubjects_JoinsSubjects___c__DisplayClass8_0__i));
		chaos_object->field_JoinsSubjects_JoinsSubjects___c__DisplayClass8_0__i = chaos_value;
		chaos_gc_dirty_card(chaos_object);
	}
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(77819907u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	_s1 = chaos_locals[0];
	{
		// Hotpatch-aware ldftn wrapper (slot 20)
		static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
			auto& _d_entry = s_hotpatch_entries[20];
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
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[25])(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[26])(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = chaos_locals[0];
	{
		// Hotpatch-aware ldftn wrapper (slot 21)
		static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INT32 {
			auto& _d_entry = s_hotpatch_entries[21];
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
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[27])(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return _s0;
}

// Managed method: JoinsSubjects/JoinsSubjects+<>c__DisplayClass5_0::.ctor()
extern "C" void JoinsSubjects_JoinsSubjects___c__DisplayClass5_0__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: JoinsSubjects/JoinsSubjects+<>c__DisplayClass6_0::.ctor()
extern "C" void JoinsSubjects_JoinsSubjects___c__DisplayClass6_0__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: JoinsSubjects/JoinsSubjects+<>c__DisplayClass7_0::.ctor()
extern "C" void JoinsSubjects_JoinsSubjects___c__DisplayClass7_0__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: JoinsSubjects/JoinsSubjects+<>c__DisplayClass8_0::.ctor()
extern "C" void JoinsSubjects_JoinsSubjects___c__DisplayClass8_0__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: JoinsSubjects/JoinsSubjects+<>c::.cctor()
extern "C" void JoinsSubjects_JoinsSubjects___c_cctor(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};

	chaos_ensure_type_initialized_JoinsSubjects_JoinsSubjects___c();

	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_JoinsSubjects_JoinsSubjects___c, {});
		chaos_object->header.type_info = &chaos_mt_JoinsSubjects_JoinsSubjects___c.hot;
		JoinsSubjects_JoinsSubjects___c__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	{
		auto chaos_value = _s0;
	chaos_ensure_type_initialized_JoinsSubjects_JoinsSubjects___c();
		chaos_static_JoinsSubjects_JoinsSubjects___c____9 = chaos_value;
	}
	return;
}

// Managed method: JoinsSubjects/JoinsSubjects+<>c::.ctor()
extern "C" void JoinsSubjects_JoinsSubjects___c__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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



}  // namespace chaos::il2cpp::codegen::JoinsSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 29;

extern "C" void ChaosJitRegisterAll() {}