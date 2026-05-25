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
extern "C" void CustomGenericsSubjects_CustomGenericsSubjects_Subject_0(void);
extern "C" void CustomGenericsSubjects_CustomGenericsSubjects_Subject_1(void);
extern "C" void CustomGenericsSubjects_CustomGenericsSubjects_Subject_2(void);
extern "C" void CustomGenericsSubjects_CustomGenericsSubjects_Subject_3(void);
extern "C" void CustomGenericsSubjects_CustomGenericsSubjects_Subject_4(void);
extern "C" void CustomGenericsSubjects_CustomGenericsSubjects_Subject_5(void);
extern "C" CHAOS_IL2CPP_INTPTR CustomGenericsSubjects_CustomGenerics_Box_1_Get(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INTPTR CustomGenericsSubjects_CustomGenerics_Box_1_IsDefault(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CustomGenericsSubjects_CustomGenerics_Box_1_Set_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" void CustomGenericsSubjects_CustomGenerics_Box_1__ctor_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 CustomGenericsSubjects_CustomGenerics_Box_System_Int32_Get(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INTPTR CustomGenericsSubjects_CustomGenerics_Box_System_Int32_IsDefault(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CustomGenericsSubjects_CustomGenerics_Box_System_Int32_Set_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void CustomGenericsSubjects_CustomGenerics_Box_System_Int32__ctor_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR CustomGenericsSubjects_CustomGenerics_Box__0_Get(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INTPTR CustomGenericsSubjects_CustomGenerics_Box__0_IsDefault(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CustomGenericsSubjects_CustomGenerics_Box__0_Set_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" void CustomGenericsSubjects_CustomGenerics_Box__0__ctor_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" void CustomGenericsSubjects_CustomGenerics_Pair_2_Deconstruct_0_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2);
extern "C" void CustomGenericsSubjects_CustomGenerics_Pair_2__ctor_0_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2);
extern "C" void CustomGenericsSubjects_CustomGenerics_Pair_System_Int32_System_String_Deconstruct_System_Int32_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2);
extern "C" void CustomGenericsSubjects_CustomGenerics_Pair_System_Int32_System_String__ctor_System_Int32_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2);
extern "C" void CustomGenericsSubjects_CustomGenerics_Pair__0__1_Deconstruct_0_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2);
extern "C" void CustomGenericsSubjects_CustomGenerics_Pair__0__1__ctor_0_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INTPTR CustomGenericsSubjects_CustomGenerics_Util_Echo_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 CustomGenericsSubjects_CustomGenerics_Util_Echo_System_Int32_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INTPTR CustomGenericsSubjects_CustomGenerics_Util_Equal_0_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR CustomGenericsSubjects_CustomGenerics_Util_Equal_System_Int32_System_Int32_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void CustomGenericsSubjects_CustomGenerics_Util__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INTPTR CustomGenericsSubjects_CustomGenerics_Wrapper_1_GetRefType(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CustomGenericsSubjects_CustomGenerics_Wrapper_1__ctor_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR CustomGenericsSubjects_CustomGenerics_Wrapper_System_String_GetRefType(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CustomGenericsSubjects_CustomGenerics_Wrapper_System_String__ctor_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR CustomGenericsSubjects_CustomGenerics_Wrapper__0_GetRefType(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CustomGenericsSubjects_CustomGenerics_Wrapper__0__ctor_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);

namespace chaos::il2cpp::codegen::CustomGenericsSubjects {

// Bring runtime_core and jit declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;
using namespace chaos::il2cpp::jit;

// Runtime prelude dependencies included at translation unit level


const void* chaos_vtable_CustomGenericsSubjects_CustomGenerics_Box__0_[];
const void* chaos_vtable_CustomGenericsSubjects_CustomGenerics_Box_System_Int32_[];
const void* chaos_vtable_CustomGenericsSubjects_CustomGenerics_Wrapper__0_[];
const void* chaos_vtable_CustomGenericsSubjects_CustomGenerics_Wrapper_System_String_[];
inline MethodTable chaos_mt_CustomGenericsSubjects_CustomGenerics_Box__0_ = {nullptr, chaos_vtable_CustomGenericsSubjects_CustomGenerics_Box__0_, 16631257188485470305ULL, 4u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_CustomGenericsSubjects_CustomGenerics_Box__0_ = static_cast<CHAOS_IL2CPP_INTPTR>(16631257188485470305ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_Object = {nullptr, nullptr, 15228727185366376748ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_INTPTR>(15228727185366376748ULL);
inline MethodTable chaos_mt_CustomGenericsSubjects_CustomGenerics_Box_System_Int32_ = {reinterpret_cast<const MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Object), chaos_vtable_CustomGenericsSubjects_CustomGenerics_Box_System_Int32_, 5207933901502182743ULL, 7u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_CustomGenericsSubjects_CustomGenerics_Box_System_Int32_ = static_cast<CHAOS_IL2CPP_INTPTR>(5207933901502182743ULL);
inline MethodTable chaos_mt_CustomGenericsSubjects_CustomGenerics_Wrapper__0_ = {nullptr, chaos_vtable_CustomGenericsSubjects_CustomGenerics_Wrapper__0_, 16395831064008788899ULL, 8u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_CustomGenericsSubjects_CustomGenerics_Wrapper__0_ = static_cast<CHAOS_IL2CPP_INTPTR>(16395831064008788899ULL);
inline MethodTable chaos_mt_CustomGenericsSubjects_CustomGenerics_Wrapper_System_String_ = {reinterpret_cast<const MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Object), chaos_vtable_CustomGenericsSubjects_CustomGenerics_Wrapper_System_String_, 1566872199215318928ULL, 9u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_CustomGenericsSubjects_CustomGenerics_Wrapper_System_String_ = static_cast<CHAOS_IL2CPP_INTPTR>(1566872199215318928ULL);
inline MethodTable chaos_mt_CustomGenericsSubjects_CustomGenericsSubjects = {nullptr, nullptr, 9789882378046446790ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_CustomGenericsSubjects_CustomGenericsSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(9789882378046446790ULL);
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
inline MethodTable chaos_mt_System_Private_CoreLib_System_Type = {reinterpret_cast<const MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Object), nullptr, 3222174513575444759ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Type = static_cast<CHAOS_IL2CPP_INTPTR>(3222174513575444759ULL);
inline MethodTable chaos_mt_CustomGenericsSubjects__0 = {nullptr, nullptr, 3855468534570849247ULL, 0u, 32, 2, 0, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_CustomGenericsSubjects__0 = static_cast<CHAOS_IL2CPP_INTPTR>(3855468534570849247ULL);
inline MethodTable chaos_mt_CustomGenericsSubjects_CustomGenerics_Pair__0__1_ = {nullptr, nullptr, 1159289307741911596ULL, 0u, 32, 2, 0, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_CustomGenericsSubjects_CustomGenerics_Pair__0__1_ = static_cast<CHAOS_IL2CPP_INTPTR>(1159289307741911596ULL);
inline MethodTable chaos_mt_CustomGenericsSubjects_CustomGenerics_Pair_System_Int32_System_String_ = {nullptr, nullptr, 10155332617276276152ULL, 0u, 32, 2, 0, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_CustomGenericsSubjects_CustomGenerics_Pair_System_Int32_System_String_ = static_cast<CHAOS_IL2CPP_INTPTR>(10155332617276276152ULL);
inline MethodTable chaos_mt_CustomGenericsSubjects_System_Int32 = {nullptr, nullptr, 13819540549784688885ULL, 0u, 32, 2, 0, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_CustomGenericsSubjects_System_Int32 = static_cast<CHAOS_IL2CPP_INTPTR>(13819540549784688885ULL);
inline MethodTable chaos_mt_CustomGenericsSubjects_System_String = {nullptr, nullptr, 17228698280304096606ULL, 0u, 32, 2, 0, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_CustomGenericsSubjects_System_String = static_cast<CHAOS_IL2CPP_INTPTR>(17228698280304096606ULL);
inline constexpr CHAOS_IL2CPP_INTPTR chaos_boxed_type_id_CustomGenericsSubjects__0 = static_cast<CHAOS_IL2CPP_INTPTR>(3855468534570849247ULL);
inline constexpr CHAOS_IL2CPP_INTPTR chaos_boxed_type_id_CustomGenericsSubjects_System_Int32 = static_cast<CHAOS_IL2CPP_INTPTR>(13819540549784688885ULL);
inline constexpr CHAOS_IL2CPP_INTPTR chaos_boxed_type_id_CustomGenericsSubjects_System_String = static_cast<CHAOS_IL2CPP_INTPTR>(17228698280304096606ULL);

// ── Virtual method table arrays ──
extern "C" void CustomGenericsSubjects_CustomGenerics_Box__0__ctor_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR CustomGenericsSubjects_CustomGenerics_Box__0_Get(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INTPTR CustomGenericsSubjects_CustomGenerics_Box__0_IsDefault(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CustomGenericsSubjects_CustomGenerics_Box__0_Set_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
const void* chaos_vtable_CustomGenericsSubjects_CustomGenerics_Box__0_[] =
{
	reinterpret_cast<void*>(CustomGenericsSubjects_CustomGenerics_Box__0__ctor_0),
	reinterpret_cast<void*>(CustomGenericsSubjects_CustomGenerics_Box__0_Get),
	reinterpret_cast<void*>(CustomGenericsSubjects_CustomGenerics_Box__0_IsDefault),
	reinterpret_cast<void*>(CustomGenericsSubjects_CustomGenerics_Box__0_Set_0),
};
extern "C" CHAOS_IL2CPP_INTPTR CustomGenericsSubjects_CustomGenerics_Box_System_Int32_IsDefault(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CustomGenericsSubjects_CustomGenerics_Box_System_Int32__ctor_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void chaos_stub_definition_CustomGenericsSubjects_CustomGenerics_Box_1___ctor_System_Void__0__type__System_Int32__method(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 CustomGenericsSubjects_CustomGenerics_Box_System_Int32_Get(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 chaos_stub_definition_CustomGenericsSubjects_CustomGenerics_Box_1__Get__0___type__System_Int32__method(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CustomGenericsSubjects_CustomGenerics_Box_System_Int32_Set_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void chaos_stub_definition_CustomGenericsSubjects_CustomGenerics_Box_1__Set_System_Void__0__type__System_Int32__method(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
const void* chaos_vtable_CustomGenericsSubjects_CustomGenerics_Box_System_Int32_[] =
{
	nullptr,
	nullptr,
	reinterpret_cast<void*>(CustomGenericsSubjects_CustomGenerics_Box_System_Int32_IsDefault),
	nullptr,
	reinterpret_cast<void*>(chaos_stub_definition_CustomGenericsSubjects_CustomGenerics_Box_1___ctor_System_Void__0__type__System_Int32__method),
	reinterpret_cast<void*>(chaos_stub_definition_CustomGenericsSubjects_CustomGenerics_Box_1__Get__0___type__System_Int32__method),
	reinterpret_cast<void*>(chaos_stub_definition_CustomGenericsSubjects_CustomGenerics_Box_1__Set_System_Void__0__type__System_Int32__method),
};
extern "C" void CustomGenericsSubjects_CustomGenerics_Wrapper__0__ctor_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR CustomGenericsSubjects_CustomGenerics_Wrapper__0_GetRefType(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
const void* chaos_vtable_CustomGenericsSubjects_CustomGenerics_Wrapper__0_[] =
{
	reinterpret_cast<void*>(CustomGenericsSubjects_CustomGenerics_Wrapper__0__ctor_0),
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	reinterpret_cast<void*>(CustomGenericsSubjects_CustomGenerics_Wrapper__0_GetRefType),
};
extern "C" CHAOS_IL2CPP_INTPTR CustomGenericsSubjects_CustomGenerics_Wrapper_System_String_GetRefType(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INTPTR chaos_stub_definition_CustomGenericsSubjects_CustomGenerics_Wrapper_1__GetRefType_System_Type___type__System_String__method(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CustomGenericsSubjects_CustomGenerics_Wrapper_System_String__ctor_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" void chaos_stub_definition_CustomGenericsSubjects_CustomGenerics_Wrapper_1___ctor_System_Void__0__type__System_String__method(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
const void* chaos_vtable_CustomGenericsSubjects_CustomGenerics_Wrapper_System_String_[] =
{
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	reinterpret_cast<void*>(chaos_stub_definition_CustomGenericsSubjects_CustomGenerics_Wrapper_1__GetRefType_System_Type___type__System_String__method),
	reinterpret_cast<void*>(chaos_stub_definition_CustomGenericsSubjects_CustomGenerics_Wrapper_1___ctor_System_Void__0__type__System_String__method),
};
static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_CustomGenericsSubjects_CustomGenerics_Box__0_[] =
{
	{ 0x00000034u, reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Box__0__ctor_0) },
	{ 0x00000035u, reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Box__0_Get) },
	{ 0x00000037u, reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Box__0_IsDefault) },
	{ 0x00000036u, reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Box__0_Set_0) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_CustomGenericsSubjects_CustomGenerics_Box_System_Int32_[] =
{
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0x0000002Fu, reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Box_System_Int32_IsDefault) },
	{ 0u, nullptr },
	{ 0x0000002Cu, reinterpret_cast<void*>(&chaos_stub_definition_CustomGenericsSubjects_CustomGenerics_Box_1___ctor_System_Void__0__type__System_Int32__method) },
	{ 0x0000002Du, reinterpret_cast<void*>(&chaos_stub_definition_CustomGenericsSubjects_CustomGenerics_Box_1__Get__0___type__System_Int32__method) },
	{ 0x0000002Eu, reinterpret_cast<void*>(&chaos_stub_definition_CustomGenericsSubjects_CustomGenerics_Box_1__Set_System_Void__0__type__System_Int32__method) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_CustomGenericsSubjects_CustomGenerics_Wrapper__0_[] =
{
	{ 0x0000003Au, reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Wrapper__0__ctor_0) },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0x0000003Bu, reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Wrapper__0_GetRefType) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_CustomGenericsSubjects_CustomGenerics_Wrapper_System_String_[] =
{
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0x00000033u, reinterpret_cast<void*>(&chaos_stub_definition_CustomGenericsSubjects_CustomGenerics_Wrapper_1__GetRefType_System_Type___type__System_String__method) },
	{ 0x00000032u, reinterpret_cast<void*>(&chaos_stub_definition_CustomGenericsSubjects_CustomGenerics_Wrapper_1___ctor_System_Void__0__type__System_String__method) },
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

struct chaos_type_CustomGenericsSubjects_CustomGenerics_Box__0_
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR field_CustomGenericsSubjects_CustomGenerics_Box__0____value = 0;
};

struct chaos_type_System_Private_CoreLib_System_Object
{
	ThinLockableHeader header{};
};

struct chaos_type_CustomGenericsSubjects_CustomGenerics_Box_System_Int32_ : public chaos_type_System_Private_CoreLib_System_Object
{
	CHAOS_IL2CPP_INT32 field_CustomGenericsSubjects_CustomGenerics_Box_System_Int32____value = 0;
};

struct chaos_type_CustomGenericsSubjects_CustomGenerics_Wrapper__0_
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR field_CustomGenericsSubjects_CustomGenerics_Wrapper__0___Ref = 0;
};

struct chaos_type_CustomGenericsSubjects_CustomGenerics_Wrapper_System_String_ : public chaos_type_System_Private_CoreLib_System_Object
{
	CHAOS_IL2CPP_INTPTR field_CustomGenericsSubjects_CustomGenerics_Wrapper_System_String___Ref = 0;
};

struct chaos_type_CustomGenericsSubjects_CustomGenericsSubjects
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

struct chaos_type_System_Private_CoreLib_System_Type : public chaos_type_System_Private_CoreLib_System_Object
{
	CHAOS_IL2CPP_INTPTR runtime_type_handle = 0;
	CHAOS_IL2CPP_INTPTR runtime_name_value = 0;
	CHAOS_IL2CPP_INT32 runtime_metadata_token_value = 0;
};

struct chaos_valuetype_CustomGenericsSubjects__0
{
	CHAOS_IL2CPP_INTPTR _backing = 0;
	chaos_valuetype_CustomGenericsSubjects__0() = default;
	// cppcheck-suppress noExplicitConstructor
	chaos_valuetype_CustomGenericsSubjects__0(CHAOS_IL2CPP_INTPTR v) noexcept : _backing(v) {}
};

struct chaos_valuetype_CustomGenericsSubjects_CustomGenerics_Pair__0__1_
{
	CHAOS_IL2CPP_INTPTR field_CustomGenericsSubjects_CustomGenerics_Pair__0__1___First = 0;
	CHAOS_IL2CPP_INTPTR field_CustomGenericsSubjects_CustomGenerics_Pair__0__1___Second = 0;
};

struct chaos_valuetype_CustomGenericsSubjects_CustomGenerics_Pair_System_Int32_System_String_
{
	CHAOS_IL2CPP_INT32 field_CustomGenericsSubjects_CustomGenerics_Pair_System_Int32_System_String___First = 0;
	CHAOS_IL2CPP_INTPTR field_CustomGenericsSubjects_CustomGenerics_Pair_System_Int32_System_String___Second = 0;
};

struct chaos_valuetype_CustomGenericsSubjects_System_Int32
{
	CHAOS_IL2CPP_INTPTR _backing = 0;
	chaos_valuetype_CustomGenericsSubjects_System_Int32() = default;
	// cppcheck-suppress noExplicitConstructor
	chaos_valuetype_CustomGenericsSubjects_System_Int32(CHAOS_IL2CPP_INTPTR v) noexcept : _backing(v) {}
};

struct chaos_valuetype_CustomGenericsSubjects_System_String
{
	CHAOS_IL2CPP_INTPTR _backing = 0;
	chaos_valuetype_CustomGenericsSubjects_System_String() = default;
	// cppcheck-suppress noExplicitConstructor
	chaos_valuetype_CustomGenericsSubjects_System_String(CHAOS_IL2CPP_INTPTR v) noexcept : _backing(v) {}
};

struct chaos_boxed_type_CustomGenericsSubjects__0
{
	PureTypeHeader header{};
	chaos_valuetype_CustomGenericsSubjects__0 value{};
};

struct chaos_boxed_type_CustomGenericsSubjects_System_Int32
{
	PureTypeHeader header{};
	chaos_valuetype_CustomGenericsSubjects_System_Int32 value{};
};

struct chaos_boxed_type_CustomGenericsSubjects_System_String
{
	PureTypeHeader header{};
	chaos_valuetype_CustomGenericsSubjects_System_String value{};
};

	bool chaos_object_equals(CHAOS_IL2CPP_INTPTR chaos_left_value, CHAOS_IL2CPP_INTPTR chaos_right_value) noexcept
	{
		// StringId fast path: tagged integers compare directly (O(1)).
		if (chaos_is_string_id(chaos_left_value) && chaos_is_string_id(chaos_right_value))
		{
			return chaos_left_value == chaos_right_value;
		}
		
		// Mixed StringId vs pointer: never equal (different representations).
		if (chaos_is_string_id(chaos_left_value) != chaos_is_string_id(chaos_right_value))
		{
			return false;
		}
		
		if (chaos_left_value == chaos_right_value)
		{
			return true;
		}
		
		if (chaos_left_value == 0 || chaos_right_value == 0)
		{
			return false;
		}
		
		const auto* chaos_left_ti = chaos_object_get_type_info(reinterpret_cast<const void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_left_value)));
		const auto* chaos_right_ti = chaos_object_get_type_info(reinterpret_cast<const void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_right_value)));
		if (chaos_left_ti != chaos_right_ti
			&& chaos_left_ti->stable_id != chaos_right_ti->stable_id)
		{
			return false;
		}
		if (chaos_left_ti == chaos_mt_System_Private_CoreLib_System_String.AsTypeInfoHot()
			|| chaos_left_ti->stable_id == chaos_mt_System_Private_CoreLib_System_String.AsTypeInfoHot()->stable_id)
		{
			auto* chaos_left_string = reinterpret_cast<chaos_type_System_Private_CoreLib_System_String*>(chaos_left_value);
			auto* chaos_right_string = reinterpret_cast<chaos_type_System_Private_CoreLib_System_String*>(chaos_right_value);
			if (chaos_left_string->length != chaos_right_string->length)
			{
				return false;
			}
		
			if (chaos_left_string->utf8_data == nullptr || chaos_right_string->utf8_data == nullptr)
			{
				return chaos_left_string->utf8_data == chaos_right_string->utf8_data;
			}
		
			return CHAOS_IL2CPP_STRCMP(chaos_left_string->utf8_data, chaos_right_string->utf8_data) == 0;
		}
		switch (chaos_left_ti->stable_id)
		{
			case chaos_boxed_type_id_CustomGenericsSubjects__0:
				return CHAOS_IL2CPP_MEMCMP(&reinterpret_cast<chaos_boxed_type_CustomGenericsSubjects__0*>(chaos_left_value)->value, &reinterpret_cast<chaos_boxed_type_CustomGenericsSubjects__0*>(chaos_right_value)->value, sizeof(chaos_valuetype_CustomGenericsSubjects__0)) == 0;
			case chaos_boxed_type_id_CustomGenericsSubjects_System_Int32:
				return CHAOS_IL2CPP_MEMCMP(&reinterpret_cast<chaos_boxed_type_CustomGenericsSubjects_System_Int32*>(chaos_left_value)->value, &reinterpret_cast<chaos_boxed_type_CustomGenericsSubjects_System_Int32*>(chaos_right_value)->value, sizeof(chaos_valuetype_CustomGenericsSubjects_System_Int32)) == 0;
			case chaos_boxed_type_id_CustomGenericsSubjects_System_String:
				return CHAOS_IL2CPP_MEMCMP(&reinterpret_cast<chaos_boxed_type_CustomGenericsSubjects_System_String*>(chaos_left_value)->value, &reinterpret_cast<chaos_boxed_type_CustomGenericsSubjects_System_String*>(chaos_right_value)->value, sizeof(chaos_valuetype_CustomGenericsSubjects_System_String)) == 0;
			default:
				return false;
		}
}

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
	chaos_string->header.type_info = chaos_mt_System_Private_CoreLib_System_String.AsTypeInfoHot();
	chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_view.byte_count);
	auto* owned_utf8 = static_cast<char*>(chaos::il2cpp::runtime_core::GcAllocateAtomic(chaos_view.byte_count + 1));
	CHAOS_IL2CPP_MEMCPY(owned_utf8, chaos_view.utf8_data, chaos_view.byte_count);
	owned_utf8[chaos_view.byte_count] = '\0';
	chaos_string->utf8_data = owned_utf8;
	chaos_string->string_id = chaos_id;
	return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
}

CHAOS_IL2CPP_INTPTR chaos_reflection_create_reference_array(const TypeInfo* chaos_element_type_info, CHAOS_IL2CPP_SIZE chaos_length)
{
	auto* chaos_array = CHAOS_IL2CPP_NEW_GC(chaos_managed_array);
	chaos_array->header.type_info = &chaos_type_info_managed_array.hot;
	chaos_array->element_type_shape = chaos_type_shape_reference;
	chaos_array->element_type_info = chaos_element_type_info;
	chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
	chaos_array->elements = chaos_length == 0 ? nullptr : static_cast<CHAOS_IL2CPP_INTPTR*>(CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, chaos_length));
	return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_type_name_value_from_handle(CHAOS_IL2CPP_INTPTR chaos_type_handle) noexcept
{
	switch (chaos_type_handle)
	{
		default:
			return 0;
	}
}

CHAOS_IL2CPP_INT32 chaos_reflection_get_type_metadata_token_from_handle(CHAOS_IL2CPP_INTPTR chaos_type_handle) noexcept
{
	switch (chaos_type_handle)
	{
		default:
			return 0;
	}
}

CHAOS_IL2CPP_INTPTR chaos_reflection_create_type_value(CHAOS_IL2CPP_INTPTR chaos_type_handle)
{
	auto* chaos_type = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Type);
	chaos_type->header.type_info = chaos_mt_System_Private_CoreLib_System_Type.AsTypeInfoHot();
	chaos_type->runtime_type_handle = chaos_type_handle;
	chaos_type->runtime_name_value = chaos_reflection_get_type_name_value_from_handle(chaos_type_handle);
	chaos_type->runtime_metadata_token_value = chaos_reflection_get_type_metadata_token_from_handle(chaos_type_handle);
	return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_type);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetMethod_name_value_from_handle(CHAOS_IL2CPP_INTPTR chaos_method_handle) noexcept
{
	switch (chaos_method_handle)
	{
		default:
			return 0;
	}
}

CHAOS_IL2CPP_INT32 ChaosReflectionGetMethod_metadata_token_from_handle(CHAOS_IL2CPP_INTPTR chaos_method_handle) noexcept
{
	switch (chaos_method_handle)
	{
		default:
			return 0;
	}
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetDeclaringType_handle_from_type_handle(CHAOS_IL2CPP_INTPTR chaos_type_handle) noexcept
{
	switch (chaos_type_handle)
	{
		default:
			return 0;
	}
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetTypeFromHandle(CHAOS_IL2CPP_INTPTR chaos_type_handle)
{
	return chaos_reflection_create_type_value(chaos_type_handle);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetTypeHandle(CHAOS_IL2CPP_INTPTR chaos_type_value) noexcept
{
	auto* chaos_type = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_type_value);
	return chaos_type->runtime_type_handle;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_object_type(CHAOS_IL2CPP_INTPTR chaos_object_value)
{
	if (chaos_object_value == 0)
	{
		return 0;
	}
	
	const auto* chaos_ti = chaos_object_get_type_info(reinterpret_cast<const void*>(chaos_object_value));
	switch (chaos_ti->stable_id)
	{
		default:
			return 0;
	}
}

CHAOS_IL2CPP_INTPTR chaos_reflection_create_instance(CHAOS_IL2CPP_INTPTR chaos_type_value)
{
	auto* chaos_type = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_type_value);
	if (chaos_type == nullptr)
	{
		return 0;
	}
	
	switch (chaos_type->runtime_type_handle)
	{
		default:
			return 0;
	}
}

CHAOS_IL2CPP_INTPTR chaos_reflection_resolve_method_handle(CHAOS_IL2CPP_INTPTR chaos_type_handle, const char* chaos_method_name) noexcept
{
	if (chaos_method_name == nullptr)
	{
		return 0;
	}

	switch (chaos_type_handle)
	{
		case static_cast<CHAOS_IL2CPP_INTPTR>(42630017u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, ".ctor") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107519786u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Get") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(115072797u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "IsDefault") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(103933076u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Set") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101714266u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(42635991u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, ".ctor") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(111985566u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Get") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116049769u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "IsDefault") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101882178u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Set") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(115856058u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(44868219u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, ".ctor") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(100796752u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Get") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109042415u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "IsDefault") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(104924478u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Set") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(115032036u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(40863852u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, ".ctor") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116706747u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Deconstruct") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(112966371u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(33922328u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, ".ctor") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107177803u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Deconstruct") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109276491u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(41369973u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, ".ctor") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107981882u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Deconstruct") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116337426u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(42651075u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, ".ctor") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(102503299u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Echo<System.Int32>") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(105415348u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Echo`1") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(106586374u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Equal<System.Int32>") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116826684u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Equal`1") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107898854u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(48000899u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, ".ctor") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(115140232u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "GetRefType") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(115467226u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(49376752u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, ".ctor") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(110129192u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "GetRefType") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(105710697u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(46404497u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, ".ctor") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(108035578u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "GetRefType") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109856276u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(40287974u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_0") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109071487u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_1") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(113747376u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_2") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(104483353u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_3") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(110378402u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_4") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109421811u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_5") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101094916u);
			}

			break;
		default:
			break;
	}

	return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetMethod(CHAOS_IL2CPP_INTPTR chaos_type_value, CHAOS_IL2CPP_INTPTR chaos_name_value, CHAOS_IL2CPP_INT32 chaos_binding_flags = CHAOS_IL2CPP_INT32(0))
{
	(void)chaos_binding_flags;
	auto* chaos_type = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_type_value);
	const auto* chaos_method_name = chaos_reflection_get_string_utf8(chaos_name_value);
	const auto chaos_method_handle = chaos_reflection_resolve_method_handle(chaos_type->runtime_type_handle, chaos_method_name);
	if (chaos_method_handle == 0)
	{
		return 0;
	}

	auto* chaos_method = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo);
	chaos_method->header.type_info = chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo.AsTypeInfoHot();
	chaos_method->declaring_type_handle = chaos_type->runtime_type_handle;
	chaos_method->runtime_method_handle = chaos_method_handle;
	chaos_method->generic_definition_method_handle = chaos_method_handle;
	chaos_method->runtime_name_value = ChaosReflectionGetMethod_name_value_from_handle(chaos_method_handle);
	chaos_method->runtime_metadata_token_value = ChaosReflectionGetMethod_metadata_token_from_handle(chaos_method_handle);
	return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_method);
}

CHAOS_IL2CPP_INTPTR chaos_reflection_create_closed_generic_method_handle(CHAOS_IL2CPP_INTPTR chaos_definition_handle, CHAOS_IL2CPP_INTPTR chaos_type_handle) noexcept
{
	CHAOS_IL2CPP_UINT32 hash = 2166136261u;
	const auto chaos_definition_bits = static_cast<CHAOS_IL2CPP_UINT64>(chaos_definition_handle);
	const auto chaos_type_bits = static_cast<CHAOS_IL2CPP_UINT64>(chaos_type_handle);
	hash ^= static_cast<CHAOS_IL2CPP_UINT32>(chaos_definition_bits & 0xFFFFFFFFu);
	hash *= 16777619u;
	hash ^= static_cast<CHAOS_IL2CPP_UINT32>(chaos_definition_bits >> 32);
	hash *= 16777619u;
	hash ^= static_cast<CHAOS_IL2CPP_UINT32>(chaos_type_bits & 0xFFFFFFFFu);
	hash *= 16777619u;
	hash ^= static_cast<CHAOS_IL2CPP_UINT32>(chaos_type_bits >> 32);
	hash *= 16777619u;
	auto row_index = hash & 0x00FFFFFFu;
	if (row_index == 0u)
	{
		row_index = 1u;
	}

	return static_cast<CHAOS_IL2CPP_INTPTR>(0x06000000u | row_index);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionMakeGenericMethod(CHAOS_IL2CPP_INTPTR chaos_method_value, CHAOS_IL2CPP_INTPTR chaos_type_array_value)
{
	if (chaos_method_value == 0 || chaos_type_array_value == 0)
	{
		return 0;
	}

	auto* chaos_method = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo*>(chaos_method_value);
	auto* chaos_type_array = reinterpret_cast<chaos_managed_array*>(chaos_type_array_value);
	if (chaos_type_array->length <= 0)
	{
		return 0;
	}

	const auto chaos_type_argument_value = chaos_type_array->elements[0];
	if (chaos_type_argument_value == 0)
	{
		return 0;
	}

	auto* chaos_type_argument = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_type_argument_value);
	auto* chaos_closed_method = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo);
	chaos_closed_method->header.type_info = chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo.AsTypeInfoHot();
	chaos_closed_method->declaring_type_handle = chaos_method->declaring_type_handle;
	chaos_closed_method->generic_definition_method_handle = chaos_method->generic_definition_method_handle != 0
		? chaos_method->generic_definition_method_handle
		: chaos_method->runtime_method_handle;
	chaos_closed_method->generic_argument_type_handle = chaos_type_argument->runtime_type_handle;
	chaos_closed_method->runtime_method_handle = chaos_reflection_create_closed_generic_method_handle(
		chaos_closed_method->generic_definition_method_handle,
		chaos_closed_method->generic_argument_type_handle);
	chaos_closed_method->runtime_name_value = chaos_method->runtime_name_value == 0
		? ChaosReflectionGetMethod_name_value_from_handle(chaos_closed_method->generic_definition_method_handle)
		: chaos_method->runtime_name_value;
	chaos_closed_method->runtime_metadata_token_value = chaos_method->runtime_metadata_token_value != 0
		? chaos_method->runtime_metadata_token_value
		: ChaosReflectionGetMethod_metadata_token_from_handle(chaos_closed_method->generic_definition_method_handle);
	return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_closed_method);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetMethod_handle(CHAOS_IL2CPP_INTPTR chaos_method_value) noexcept
{
	auto* chaos_method = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo*>(chaos_method_value);
	return chaos_method->runtime_method_handle;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionInvokeMethod(CHAOS_IL2CPP_INTPTR chaos_method_value, CHAOS_IL2CPP_INTPTR chaos_target_value, CHAOS_IL2CPP_INTPTR chaos_args_array_value)
{
	if (chaos_method_value == 0)
	{
		return 0;
	}

	auto* chaos_method = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo*>(chaos_method_value);
	auto* chaos_args = reinterpret_cast<chaos_managed_array*>(chaos_args_array_value);

	if (chaos_method->generic_definition_method_handle == static_cast<CHAOS_IL2CPP_INTPTR>(106586374u))
	{
		if (chaos_target_value != 0 || chaos_args == nullptr || chaos_args->length < static_cast<CHAOS_IL2CPP_INTPTR>(1))
		{
			return 0;
		}

		if (chaos_method->generic_argument_type_handle == static_cast<CHAOS_IL2CPP_INTPTR>(49037434u))
		{
			return CustomGenericsSubjects_CustomGenerics_Util_Echo_0(chaos_args->elements[0]);
		}

		return 0;
	}

	return 0;
}


// ── GcTypeLayout pointer offset arrays (computed via offsetof) ──
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_Assembly[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_Assembly, runtime_assembly_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, declaring_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, runtime_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, generic_definition_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, generic_argument_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, declaring_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, declaring_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, runtime_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, generic_definition_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, generic_argument_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Type[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Type, runtime_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Type, runtime_name_value))};

extern "C" void ChaosRegisterGcLayouts() {
	auto& registry = chaos::il2cpp::runtime_core::GcLayoutRegistry::Instance();

	registry.Register(16631257188485470305ULL, sizeof(chaos_type_CustomGenericsSubjects_CustomGenerics_Box__0_), nullptr, 0);
	registry.Register(15228727185366376748ULL, sizeof(chaos_type_System_Private_CoreLib_System_Object), nullptr, 0);
	registry.Register(5207933901502182743ULL, sizeof(chaos_type_CustomGenericsSubjects_CustomGenerics_Box_System_Int32_), nullptr, 0);
	registry.Register(16395831064008788899ULL, sizeof(chaos_type_CustomGenericsSubjects_CustomGenerics_Wrapper__0_), nullptr, 0);
	registry.Register(1566872199215318928ULL, sizeof(chaos_type_CustomGenericsSubjects_CustomGenerics_Wrapper_System_String_), nullptr, 0);
	registry.Register(9789882378046446790ULL, sizeof(chaos_type_CustomGenericsSubjects_CustomGenericsSubjects), nullptr, 0);
	registry.Register(5474029880995115448ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_Assembly), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_Assembly, 1);
	registry.Register(17082367815459723707ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName, 1);
	registry.Register(4137207361503509124ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, 5);
	registry.Register(17040031516751226236ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, 2);
	registry.Register(10748947813473285525ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, 5);
	registry.Register(1782325859292956794ULL, sizeof(chaos_type_System_Private_CoreLib_System_String), nullptr, 0);
	registry.Register(3222174513575444759ULL, sizeof(chaos_type_System_Private_CoreLib_System_Type), kGcOffsets_chaos_type_System_Private_CoreLib_System_Type, 2);

	// Register MethodTable address ranges for IsValidTypeInfoPointer.
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_CustomGenericsSubjects_CustomGenerics_Box__0_), reinterpret_cast<uintptr_t>(&chaos_mt_CustomGenericsSubjects_CustomGenerics_Box__0_) + sizeof(chaos_mt_CustomGenericsSubjects_CustomGenerics_Box__0_));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Object), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Object) + sizeof(chaos_mt_System_Private_CoreLib_System_Object));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_CustomGenericsSubjects_CustomGenerics_Box_System_Int32_), reinterpret_cast<uintptr_t>(&chaos_mt_CustomGenericsSubjects_CustomGenerics_Box_System_Int32_) + sizeof(chaos_mt_CustomGenericsSubjects_CustomGenerics_Box_System_Int32_));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_CustomGenericsSubjects_CustomGenerics_Wrapper__0_), reinterpret_cast<uintptr_t>(&chaos_mt_CustomGenericsSubjects_CustomGenerics_Wrapper__0_) + sizeof(chaos_mt_CustomGenericsSubjects_CustomGenerics_Wrapper__0_));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_CustomGenericsSubjects_CustomGenerics_Wrapper_System_String_), reinterpret_cast<uintptr_t>(&chaos_mt_CustomGenericsSubjects_CustomGenerics_Wrapper_System_String_) + sizeof(chaos_mt_CustomGenericsSubjects_CustomGenerics_Wrapper_System_String_));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_CustomGenericsSubjects_CustomGenericsSubjects), reinterpret_cast<uintptr_t>(&chaos_mt_CustomGenericsSubjects_CustomGenericsSubjects) + sizeof(chaos_mt_CustomGenericsSubjects_CustomGenericsSubjects));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_Assembly), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_Assembly) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_Assembly));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_String), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_String) + sizeof(chaos_mt_System_Private_CoreLib_System_String));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Type), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Type) + sizeof(chaos_mt_System_Private_CoreLib_System_Type));
}

CHAOS_IL2CPP_INT32 chaos_static_CustomGenericsSubjects_CustomGenericsSubjects___exitCode = 0;


	// AOT-baked string table: sorted by StringId for binary search at runtime.
	constexpr chaos::il2cpp::string_table::StringEntry chaos_aot_string_entries[] = {
		{ 385996094756309509U, "data", 4u },
		{ 1875936269717626031U, "one", 3u },
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

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Object__Equals_System_Boolean_System_Object_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	return chaos_object_equals(chaos_fn_arg_0, chaos_fn_arg_1);
}

static void chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	ChaosObjectCtor(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetTypeFromHandle(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__op_Inequality_System_Boolean_System_Type_System_Type_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	return ChaosTypeInequality(chaos_fn_arg_0, chaos_fn_arg_1);
}

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };


// ── Bridge/import thunks ──
extern "C" void* kChaosExternalRuntimeFnTable[];

// Bridge/import thunk for: CustomGenericsSubjects/CustomGenericsSubjects::_exitCode
extern "C" void chaos_bridge_thunk_0(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[0])();
}

// Bridge/import thunk for: System.Private.CoreLib/System.String::op_Inequality:System.Boolean(System.String,System.String)
extern "C" CHAOS_IL2CPP_INTPTR chaos_bridge_thunk_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	auto result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[1])(chaos_fn_arg_0, chaos_fn_arg_1);
	return result;
}

// Bridge/import thunk for: CustomGenericsSubjects/!1
extern "C" void chaos_bridge_thunk_10(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[14])();
}

// Bridge/import thunk for: CustomGenericsSubjects/CustomGenerics.Pair<System.Int32,System.String>::First
extern "C" void chaos_bridge_thunk_11(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[15])();
}

// Bridge/import thunk for: CustomGenericsSubjects/CustomGenerics.Pair<System.Int32,System.String>::Second
extern "C" void chaos_bridge_thunk_12(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[16])();
}

// Bridge/import thunk for: CustomGenericsSubjects/System.String
extern "C" void chaos_bridge_thunk_13(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[17])();
}

// Bridge/import thunk for: CustomGenericsSubjects/!!0::Equals:System.Boolean(!!0)
extern "C" CHAOS_IL2CPP_INTPTR chaos_bridge_thunk_14(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	auto result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[18])(chaos_fn_arg_0);
	return result;
}

// Bridge/import thunk for: System.Private.CoreLib/System.IEquatable<System.Int32>::Equals:System.Boolean(System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR chaos_bridge_thunk_15(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	auto result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[19])(chaos_fn_arg_0);
	return result;
}

// Bridge/import thunk for: CustomGenericsSubjects/CustomGenerics.Wrapper<!0>::Ref
extern "C" void chaos_bridge_thunk_16(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[20])();
}

// Bridge/import thunk for: System.Private.CoreLib/System.Object::GetType:System.Type()
extern "C" CHAOS_IL2CPP_INTPTR chaos_bridge_thunk_17(void)
{
	auto result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)()>(kChaosExternalRuntimeFnTable[21])();
	return result;
}

// Bridge/import thunk for: CustomGenericsSubjects/CustomGenerics.Wrapper<System.String>::Ref
extern "C" void chaos_bridge_thunk_18(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[22])();
}

// Bridge/import thunk for: System.Private.CoreLib/System.String
extern "C" void chaos_bridge_thunk_2(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[2])();
}

// Bridge/import thunk for: CustomGenericsSubjects/CustomGenerics.Box<!0>::_value
extern "C" void chaos_bridge_thunk_3(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[6])();
}

// Bridge/import thunk for: CustomGenericsSubjects/!0
extern "C" void chaos_bridge_thunk_4(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[7])();
}

// Bridge/import thunk for: CustomGenericsSubjects/!0::Equals:System.Boolean(System.Object)
extern "C" CHAOS_IL2CPP_INTPTR chaos_bridge_thunk_5(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	auto result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[8])(chaos_fn_arg_0);
	return result;
}

// Bridge/import thunk for: CustomGenericsSubjects/CustomGenerics.Box<System.Int32>::_value
extern "C" void chaos_bridge_thunk_6(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[10])();
}

// Bridge/import thunk for: CustomGenericsSubjects/System.Int32
extern "C" void chaos_bridge_thunk_7(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[11])();
}

// Bridge/import thunk for: CustomGenericsSubjects/CustomGenerics.Pair<!0,!1>::First
extern "C" void chaos_bridge_thunk_8(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[12])();
}

// Bridge/import thunk for: CustomGenericsSubjects/CustomGenerics.Pair<!0,!1>::Second
extern "C" void chaos_bridge_thunk_9(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[13])();
}

extern "C" void CustomGenericsSubjects_CustomGenericsSubjects_Subject_0(void);
extern "C" void CustomGenericsSubjects_CustomGenericsSubjects_Subject_1(void);
extern "C" void CustomGenericsSubjects_CustomGenericsSubjects_Subject_2(void);
extern "C" void CustomGenericsSubjects_CustomGenericsSubjects_Subject_3(void);
extern "C" void CustomGenericsSubjects_CustomGenericsSubjects_Subject_4(void);
extern "C" void CustomGenericsSubjects_CustomGenericsSubjects_Subject_5(void);
extern "C" void CustomGenericsSubjects_CustomGenerics_Box_1__ctor_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR CustomGenericsSubjects_CustomGenerics_Box_1_Get(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INTPTR CustomGenericsSubjects_CustomGenerics_Box_1_IsDefault(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CustomGenericsSubjects_CustomGenerics_Box_1_Set_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" void CustomGenericsSubjects_CustomGenerics_Box__0__ctor_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR CustomGenericsSubjects_CustomGenerics_Box__0_Get(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INTPTR CustomGenericsSubjects_CustomGenerics_Box__0_IsDefault(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CustomGenericsSubjects_CustomGenerics_Box__0_Set_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" void CustomGenericsSubjects_CustomGenerics_Box_System_Int32__ctor_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void chaos_stub_definition_CustomGenericsSubjects_CustomGenerics_Box_1___ctor_System_Void__0__type__System_Int32__method(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 CustomGenericsSubjects_CustomGenerics_Box_System_Int32_Get(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 chaos_stub_definition_CustomGenericsSubjects_CustomGenerics_Box_1__Get__0___type__System_Int32__method(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INTPTR CustomGenericsSubjects_CustomGenerics_Box_System_Int32_IsDefault(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CustomGenericsSubjects_CustomGenerics_Box_System_Int32_Set_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void chaos_stub_definition_CustomGenericsSubjects_CustomGenerics_Box_1__Set_System_Void__0__type__System_Int32__method(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void CustomGenericsSubjects_CustomGenerics_Pair_2__ctor_0_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2);
extern "C" void CustomGenericsSubjects_CustomGenerics_Pair_2_Deconstruct_0_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2);
extern "C" void CustomGenericsSubjects_CustomGenerics_Pair__0__1__ctor_0_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2);
extern "C" void CustomGenericsSubjects_CustomGenerics_Pair__0__1_Deconstruct_0_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2);
extern "C" void CustomGenericsSubjects_CustomGenerics_Pair_System_Int32_System_String__ctor_System_Int32_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2);
extern "C" void chaos_stub_definition_CustomGenericsSubjects_CustomGenerics_Pair_2___ctor_System_Void__0__1__type__System_Int32_System_String__method(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2);
extern "C" void CustomGenericsSubjects_CustomGenerics_Pair_System_Int32_System_String_Deconstruct_System_Int32_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2);
extern "C" void chaos_stub_definition_CustomGenericsSubjects_CustomGenerics_Pair_2__Deconstruct_System_Void__0___1___type__System_Int32_System_String__method(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2);
extern "C" void CustomGenericsSubjects_CustomGenerics_Util__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INTPTR CustomGenericsSubjects_CustomGenerics_Util_Echo_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 CustomGenericsSubjects_CustomGenerics_Util_Echo_System_Int32_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 chaos_stub_definition_CustomGenericsSubjects_CustomGenerics_Util__Echo_1___0___0__type____method__System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INTPTR CustomGenericsSubjects_CustomGenerics_Util_Equal_0_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR CustomGenericsSubjects_CustomGenerics_Util_Equal_System_Int32_System_Int32_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR chaos_stub_definition_CustomGenericsSubjects_CustomGenerics_Util__Equal_1_System_Boolean___0___0__type____method__System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void CustomGenericsSubjects_CustomGenerics_Wrapper_1__ctor_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR CustomGenericsSubjects_CustomGenerics_Wrapper_1_GetRefType(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CustomGenericsSubjects_CustomGenerics_Wrapper__0__ctor_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR CustomGenericsSubjects_CustomGenerics_Wrapper__0_GetRefType(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CustomGenericsSubjects_CustomGenerics_Wrapper_System_String__ctor_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" void chaos_stub_definition_CustomGenericsSubjects_CustomGenerics_Wrapper_1___ctor_System_Void__0__type__System_String__method(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR CustomGenericsSubjects_CustomGenerics_Wrapper_System_String_GetRefType(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INTPTR chaos_stub_definition_CustomGenericsSubjects_CustomGenerics_Wrapper_1__GetRefType_System_Type___type__System_String__method(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[36] = {
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
	2u,
	3u,
	3u,
	3u,
	4u,
	5u,
	5u,
	5u,
	6u,
	8u,
	10u,
	12u,
	14u,
	16u,
	18u,
	18u,
	19u,
	20u,
	22u,
	24u,
	25u,
	25u,
	26u,
	26u,
	27u,
	27u,
};

static constexpr struct {
	::ChaosAbiManifestV0 header;
	::ChaosAbiMethodEntryV0 entries[35];
	CHAOS_IL2CPP_UINT8 params[27];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		35u,
		27u,
		1855564128u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // CustomGenericsSubjects_CustomGenericsSubjects_Subject_0
		{ 0u, 0u },  // CustomGenericsSubjects_CustomGenericsSubjects_Subject_1
		{ 0u, 0u },  // CustomGenericsSubjects_CustomGenericsSubjects_Subject_2
		{ 0u, 0u },  // CustomGenericsSubjects_CustomGenericsSubjects_Subject_3
		{ 0u, 0u },  // CustomGenericsSubjects_CustomGenericsSubjects_Subject_4
		{ 0u, 0u },  // CustomGenericsSubjects_CustomGenericsSubjects_Subject_5
		{ 0u, 1u },  // CustomGenericsSubjects_CustomGenerics_Box_1__ctor_0
		{ 2u, 0u },  // CustomGenericsSubjects_CustomGenerics_Box_1_Get
		{ 2u, 0u },  // CustomGenericsSubjects_CustomGenerics_Box_1_IsDefault
		{ 0u, 1u },  // CustomGenericsSubjects_CustomGenerics_Box_1_Set_0
		{ 0u, 1u },  // CustomGenericsSubjects_CustomGenerics_Box__0__ctor_0
		{ 2u, 0u },  // CustomGenericsSubjects_CustomGenerics_Box__0_Get
		{ 2u, 0u },  // CustomGenericsSubjects_CustomGenerics_Box__0_IsDefault
		{ 0u, 1u },  // CustomGenericsSubjects_CustomGenerics_Box__0_Set_0
		{ 0u, 1u },  // CustomGenericsSubjects_CustomGenerics_Box_System_Int32__ctor_System_Int32
		{ 1u, 0u },  // CustomGenericsSubjects_CustomGenerics_Box_System_Int32_Get
		{ 2u, 0u },  // CustomGenericsSubjects_CustomGenerics_Box_System_Int32_IsDefault
		{ 0u, 1u },  // CustomGenericsSubjects_CustomGenerics_Box_System_Int32_Set_System_Int32
		{ 0u, 2u },  // CustomGenericsSubjects_CustomGenerics_Pair_2__ctor_0_1
		{ 0u, 2u },  // CustomGenericsSubjects_CustomGenerics_Pair_2_Deconstruct_0_1
		{ 0u, 2u },  // CustomGenericsSubjects_CustomGenerics_Pair__0__1__ctor_0_1
		{ 0u, 2u },  // CustomGenericsSubjects_CustomGenerics_Pair__0__1_Deconstruct_0_1
		{ 0u, 2u },  // CustomGenericsSubjects_CustomGenerics_Pair_System_Int32_System_String__ctor_System_Int32_System_String
		{ 0u, 2u },  // CustomGenericsSubjects_CustomGenerics_Pair_System_Int32_System_String_Deconstruct_System_Int32_System_String
		{ 0u, 0u },  // CustomGenericsSubjects_CustomGenerics_Util__ctor
		{ 2u, 1u },  // CustomGenericsSubjects_CustomGenerics_Util_Echo_0
		{ 1u, 1u },  // CustomGenericsSubjects_CustomGenerics_Util_Echo_System_Int32_System_Int32
		{ 2u, 2u },  // CustomGenericsSubjects_CustomGenerics_Util_Equal_0_0
		{ 2u, 2u },  // CustomGenericsSubjects_CustomGenerics_Util_Equal_System_Int32_System_Int32_System_Int32
		{ 0u, 1u },  // CustomGenericsSubjects_CustomGenerics_Wrapper_1__ctor_0
		{ 2u, 0u },  // CustomGenericsSubjects_CustomGenerics_Wrapper_1_GetRefType
		{ 0u, 1u },  // CustomGenericsSubjects_CustomGenerics_Wrapper__0__ctor_0
		{ 2u, 0u },  // CustomGenericsSubjects_CustomGenerics_Wrapper__0_GetRefType
		{ 0u, 1u },  // CustomGenericsSubjects_CustomGenerics_Wrapper_System_String__ctor_System_String
		{ 2u, 0u },  // CustomGenericsSubjects_CustomGenerics_Wrapper_System_String_GetRefType
	},
	{
		2u,
		2u,
		2u,
		2u,
		1u,
		1u,
		2u,
		2u,
		12u,
		12u,
		2u,
		2u,
		12u,
		12u,
		1u,
		2u,
		12u,
		12u,
		2u,
		1u,
		2u,
		2u,
		1u,
		1u,
		2u,
		2u,
		2u,
	},
};
static const ::ChaosAbiManifestV0* const s_abi_manifest =
	reinterpret_cast<const ::ChaosAbiManifestV0*>(&s_abi_manifest_storage);
// ── Module registration ──

	static constexpr CHAOS_IL2CPP_UINT32 s_type_flags[6] = {
		0u,
		1548u,
		192u,
		201u,
		0u,
		192u,
	};

	static constexpr const char* s_type_names[6] = {
		"<Module>",
		"CustomGenericsSubjects",
		"Box`1",
		"Pair`2",
		"Util",
		"Wrapper`1",
	};

	static constexpr const char* s_type_namespaces[6] = {
		"",
		"",
		"CustomGenerics",
		"CustomGenerics",
		"CustomGenerics",
		"CustomGenerics",
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
		chaos_mt_CustomGenericsSubjects_CustomGenericsSubjects.AsTypeInfoHot(),
		nullptr,
		nullptr,
		nullptr,
		nullptr,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_offset[7] = {
		0u,
		0u,
		0u,
		0u,
		0u,
		0u,
		0u,
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
		/* .name_utf8         = */ "CustomGenericsSubjects",
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
		/* .type_count        = */ 6u,
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
		::chaos::il2cpp::runtime_core::RegisterModule("CustomGenericsSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[35] = {
	{ "Subject_0", 0x00000019u, 0u },  // CustomGenericsSubjects
	{ "Subject_1", 0x0000001Au, 0u },  // CustomGenericsSubjects
	{ "Subject_2", 0x0000001Bu, 0u },  // CustomGenericsSubjects
	{ "Subject_3", 0x0000001Cu, 0u },  // CustomGenericsSubjects
	{ "Subject_4", 0x0000001Du, 0u },  // CustomGenericsSubjects
	{ "Subject_5", 0x0000001Eu, 0u },  // CustomGenericsSubjects
	{ ".ctor", 0x00000034u, 1u },  // CustomGenerics.Box<!0>
	{ "Get", 0x00000035u, 0u },  // CustomGenerics.Box<!0>
	{ "IsDefault", 0x00000037u, 0u },  // CustomGenerics.Box<!0>
	{ "Set", 0x00000036u, 1u },  // CustomGenerics.Box<!0>
	{ ".ctor", 0x0000001Fu, 1u },  // CustomGenerics.Box`1
	{ "Get", 0x00000020u, 0u },  // CustomGenerics.Box`1
	{ "IsDefault", 0x00000022u, 0u },  // CustomGenerics.Box`1
	{ "Set", 0x00000021u, 1u },  // CustomGenerics.Box`1
	{ ".ctor", 0x00000038u, 2u },  // CustomGenerics.Pair<!0,!1>
	{ "Deconstruct", 0x00000039u, 2u },  // CustomGenerics.Pair<!0,!1>
	{ ".ctor", 0x00000023u, 2u },  // CustomGenerics.Pair`2
	{ "Deconstruct", 0x00000024u, 2u },  // CustomGenerics.Pair`2
	{ ".ctor", 0x00000027u, 0u },  // CustomGenerics.Util
	{ "Echo`1", 0x00000025u, 1u },  // CustomGenerics.Util
	{ "Echo<System.Int32>", 0x0000002Au, 1u },  // CustomGenerics.Util
	{ "Equal`1", 0x00000026u, 2u },  // CustomGenerics.Util
	{ "Equal<System.Int32>", 0x0000002Bu, 2u },  // CustomGenerics.Util
	{ ".ctor", 0x0000003Au, 1u },  // CustomGenerics.Wrapper<!0>
	{ "GetRefType", 0x0000003Bu, 0u },  // CustomGenerics.Wrapper<!0>
	{ ".ctor", 0x00000028u, 1u },  // CustomGenerics.Wrapper`1
	{ "GetRefType", 0x00000029u, 0u },  // CustomGenerics.Wrapper`1
	{ ".ctor", 0x0000002Cu, 1u },  // CustomGenerics.Box<System.Int32>
	{ "Get", 0x0000002Du, 0u },  // CustomGenerics.Box<System.Int32>
	{ "IsDefault", 0x0000002Fu, 0u },  // CustomGenerics.Box<System.Int32>
	{ "Set", 0x0000002Eu, 1u },  // CustomGenerics.Box<System.Int32>
	{ ".ctor", 0x00000030u, 2u },  // CustomGenerics.Pair<System.Int32,System.String>
	{ "Deconstruct", 0x00000031u, 2u },  // CustomGenerics.Pair<System.Int32,System.String>
	{ ".ctor", 0x00000032u, 1u },  // CustomGenerics.Wrapper<System.String>
	{ "GetRefType", 0x00000033u, 0u },  // CustomGenerics.Wrapper<System.String>
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[11] = {
	{ "CustomGenericsSubjects", "", 0u, 6u },
	{ "CustomGenerics.Box<!0>", "CustomGenerics", 6u, 4u },
	{ "CustomGenerics.Box`1", "CustomGenerics", 10u, 4u },
	{ "CustomGenerics.Pair<!0,!1>", "CustomGenerics", 14u, 2u },
	{ "CustomGenerics.Pair`2", "CustomGenerics", 16u, 2u },
	{ "CustomGenerics.Util", "CustomGenerics", 18u, 5u },
	{ "CustomGenerics.Wrapper<!0>", "CustomGenerics", 23u, 2u },
	{ "CustomGenerics.Wrapper`1", "CustomGenerics", 25u, 2u },
	{ "CustomGenerics.Box<System.Int32>", "CustomGenerics.Box<System", 27u, 4u },
	{ "CustomGenerics.Pair<System.Int32,System.String>", "CustomGenerics.Pair<System.Int32,System", 31u, 2u },
	{ "CustomGenerics.Wrapper<System.String>", "CustomGenerics.Wrapper<System", 33u, 2u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[35] = {
	{ 0x00000019u, 0u },
	{ 0x0000001Au, 1u },
	{ 0x0000001Bu, 2u },
	{ 0x0000001Cu, 3u },
	{ 0x0000001Du, 4u },
	{ 0x0000001Eu, 5u },
	{ 0x0000001Fu, 6u },
	{ 0x00000020u, 7u },
	{ 0x00000021u, 9u },
	{ 0x00000022u, 8u },
	{ 0x00000023u, 18u },
	{ 0x00000024u, 19u },
	{ 0x00000025u, 25u },
	{ 0x00000026u, 27u },
	{ 0x00000027u, 24u },
	{ 0x00000028u, 29u },
	{ 0x00000029u, 30u },
	{ 0x0000002Au, 26u },
	{ 0x0000002Bu, 28u },
	{ 0x0000002Cu, 14u },
	{ 0x0000002Du, 15u },
	{ 0x0000002Eu, 17u },
	{ 0x0000002Fu, 16u },
	{ 0x00000030u, 22u },
	{ 0x00000031u, 23u },
	{ 0x00000032u, 33u },
	{ 0x00000033u, 34u },
	{ 0x00000034u, 10u },
	{ 0x00000035u, 11u },
	{ 0x00000036u, 13u },
	{ 0x00000037u, 12u },
	{ 0x00000038u, 20u },
	{ 0x00000039u, 21u },
	{ 0x0000003Au, 31u },
	{ 0x0000003Bu, 32u },
};

// Dispatch table (function pointers)
// s_hotpatch_entries — external linkage for verification_dispatch
// NOTE: `static` is deliberately omitted so that the verification dispatch
// TU (verification_dispatch.generated.cpp) can access this array via
// GetHotpatchEntries(). Without external linkage, the linker cannot resolve
// the symbol across translation units.
extern "C" HotpatchEntryV0 s_hotpatch_entries[35] = {
	{ reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenericsSubjects_Subject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // CustomGenericsSubjects::Subject_0
	{ reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenericsSubjects_Subject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // CustomGenericsSubjects::Subject_1
	{ reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenericsSubjects_Subject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // CustomGenericsSubjects::Subject_2
	{ reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenericsSubjects_Subject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // CustomGenericsSubjects::Subject_3
	{ reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenericsSubjects_Subject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // CustomGenericsSubjects::Subject_4
	{ reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenericsSubjects_Subject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // CustomGenericsSubjects::Subject_5
	{ reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Box_1__ctor_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // CustomGenerics.Box`1::.ctor
	{ reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Box_1_Get), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // CustomGenerics.Box`1::Get
	{ reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Box_1_IsDefault), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // CustomGenerics.Box`1::IsDefault
	{ reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Box_1_Set_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // CustomGenerics.Box`1::Set
	{ reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Box__0__ctor_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // CustomGenerics.Box<!0>::.ctor
	{ reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Box__0_Get), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // CustomGenerics.Box<!0>::Get
	{ reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Box__0_IsDefault), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // CustomGenerics.Box<!0>::IsDefault
	{ reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Box__0_Set_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // CustomGenerics.Box<!0>::Set
	{ reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Box_System_Int32__ctor_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // CustomGenerics.Box<System.Int32>::.ctor
	{ reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Box_System_Int32_Get), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // CustomGenerics.Box<System.Int32>::Get
	{ reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Box_System_Int32_IsDefault), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // CustomGenerics.Box<System.Int32>::IsDefault
	{ reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Box_System_Int32_Set_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // CustomGenerics.Box<System.Int32>::Set
	{ reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Pair_2__ctor_0_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // CustomGenerics.Pair`2::.ctor
	{ reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Pair_2_Deconstruct_0_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // CustomGenerics.Pair`2::Deconstruct
	{ reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Pair__0__1__ctor_0_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // CustomGenerics.Pair<!0,!1>::.ctor
	{ reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Pair__0__1_Deconstruct_0_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // CustomGenerics.Pair<!0,!1>::Deconstruct
	{ reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Pair_System_Int32_System_String__ctor_System_Int32_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // CustomGenerics.Pair<System.Int32,System.String>::.ctor
	{ reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Pair_System_Int32_System_String_Deconstruct_System_Int32_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // CustomGenerics.Pair<System.Int32,System.String>::Deconstruct
	{ reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Util__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // CustomGenerics.Util::.ctor
	{ reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Util_Echo_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // CustomGenerics.Util::Echo`1
	{ reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Util_Echo_System_Int32_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // CustomGenerics.Util::Echo<System.Int32>
	{ reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Util_Equal_0_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // CustomGenerics.Util::Equal`1
	{ reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Util_Equal_System_Int32_System_Int32_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // CustomGenerics.Util::Equal<System.Int32>
	{ reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Wrapper_1__ctor_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // CustomGenerics.Wrapper`1::.ctor
	{ reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Wrapper_1_GetRefType), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // CustomGenerics.Wrapper`1::GetRefType
	{ reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Wrapper__0__ctor_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // CustomGenerics.Wrapper<!0>::.ctor
	{ reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Wrapper__0_GetRefType), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // CustomGenerics.Wrapper<!0>::GetRefType
	{ reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Wrapper_System_String__ctor_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // CustomGenerics.Wrapper<System.String>::.ctor
	{ reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Wrapper_System_String_GetRefType), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // CustomGenerics.Wrapper<System.String>::GetRefType
};

// Accessor functions for verification_dispatch.generated.cpp
extern "C" const HotpatchEntryV0* GetHotpatchEntries() noexcept { return s_hotpatch_entries; }
extern "C" int32_t GetHotpatchEntryCount() noexcept { return 35; }

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"CustomGenericsSubjects",
	s_hotpatch_types,
	11u,
	s_hotpatch_methods,
	35u,
	s_hotpatch_slots,
	35u,
	s_hotpatch_entries,
	35u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[23] = {
	"CustomGenericsSubjects/CustomGenericsSubjects::_exitCode",
	"System.Private.CoreLib/System.String::op_Inequality:System.Boolean(System.String,System.String)",
	"System.Private.CoreLib/System.String",
	"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)",
	"System.Private.CoreLib/System.Type::op_Inequality:System.Boolean(System.Type,System.Type)",
	"System.Private.CoreLib/System.Object::.ctor:System.Void()",
	"CustomGenericsSubjects/CustomGenerics.Box<!0>::_value",
	"CustomGenericsSubjects/!0",
	"CustomGenericsSubjects/!0::Equals:System.Boolean(System.Object)",
	"System.Private.CoreLib/System.Object::Equals:System.Boolean(System.Object)",
	"CustomGenericsSubjects/CustomGenerics.Box<System.Int32>::_value",
	"CustomGenericsSubjects/System.Int32",
	"CustomGenericsSubjects/CustomGenerics.Pair<!0,!1>::First",
	"CustomGenericsSubjects/CustomGenerics.Pair<!0,!1>::Second",
	"CustomGenericsSubjects/!1",
	"CustomGenericsSubjects/CustomGenerics.Pair<System.Int32,System.String>::First",
	"CustomGenericsSubjects/CustomGenerics.Pair<System.Int32,System.String>::Second",
	"CustomGenericsSubjects/System.String",
	"CustomGenericsSubjects/!!0::Equals:System.Boolean(!!0)",
	"System.Private.CoreLib/System.IEquatable<System.Int32>::Equals:System.Boolean(System.Int32)",
	"CustomGenericsSubjects/CustomGenerics.Wrapper<!0>::Ref",
	"System.Private.CoreLib/System.Object::GetType:System.Type()",
	"CustomGenericsSubjects/CustomGenerics.Wrapper<System.String>::Ref",
};

extern "C" void* kChaosExternalRuntimeFnTable[23] = {
	nullptr,
	nullptr,
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__op_Inequality_System_Boolean_System_Type_System_Type_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__),
	nullptr,
	nullptr,
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Object__Equals_System_Boolean_System_Object_),
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
};

extern "C" int32_t kChaosExternalRuntimeCount = 23;
// (no method AOT entries for this module)
// ── Method table (kMethodTable[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
// NOTE: Pure data only — dispatch routing is in <chaos/hotpatch_dispatch.h>.
// JIT mode uses direct_ptr (precode trampoline → JIT compilation).

static void (*kMethodTable[35])() = {
	reinterpret_cast<void(*)()>(&CustomGenericsSubjects_CustomGenericsSubjects_Subject_0),
	reinterpret_cast<void(*)()>(&CustomGenericsSubjects_CustomGenericsSubjects_Subject_1),
	reinterpret_cast<void(*)()>(&CustomGenericsSubjects_CustomGenericsSubjects_Subject_2),
	reinterpret_cast<void(*)()>(&CustomGenericsSubjects_CustomGenericsSubjects_Subject_3),
	reinterpret_cast<void(*)()>(&CustomGenericsSubjects_CustomGenericsSubjects_Subject_4),
	reinterpret_cast<void(*)()>(&CustomGenericsSubjects_CustomGenericsSubjects_Subject_5),
	reinterpret_cast<void(*)()>(&CustomGenericsSubjects_CustomGenerics_Box_1__ctor_0),
	reinterpret_cast<void(*)()>(&CustomGenericsSubjects_CustomGenerics_Box_1_Get),
	reinterpret_cast<void(*)()>(&CustomGenericsSubjects_CustomGenerics_Box_1_IsDefault),
	reinterpret_cast<void(*)()>(&CustomGenericsSubjects_CustomGenerics_Box_1_Set_0),
	reinterpret_cast<void(*)()>(&CustomGenericsSubjects_CustomGenerics_Box__0__ctor_0),
	reinterpret_cast<void(*)()>(&CustomGenericsSubjects_CustomGenerics_Box__0_Get),
	reinterpret_cast<void(*)()>(&CustomGenericsSubjects_CustomGenerics_Box__0_IsDefault),
	reinterpret_cast<void(*)()>(&CustomGenericsSubjects_CustomGenerics_Box__0_Set_0),
	reinterpret_cast<void(*)()>(&CustomGenericsSubjects_CustomGenerics_Box_System_Int32__ctor_System_Int32),
	reinterpret_cast<void(*)()>(&CustomGenericsSubjects_CustomGenerics_Box_System_Int32_Get),
	reinterpret_cast<void(*)()>(&CustomGenericsSubjects_CustomGenerics_Box_System_Int32_IsDefault),
	reinterpret_cast<void(*)()>(&CustomGenericsSubjects_CustomGenerics_Box_System_Int32_Set_System_Int32),
	reinterpret_cast<void(*)()>(&CustomGenericsSubjects_CustomGenerics_Pair_2__ctor_0_1),
	reinterpret_cast<void(*)()>(&CustomGenericsSubjects_CustomGenerics_Pair_2_Deconstruct_0_1),
	reinterpret_cast<void(*)()>(&CustomGenericsSubjects_CustomGenerics_Pair__0__1__ctor_0_1),
	reinterpret_cast<void(*)()>(&CustomGenericsSubjects_CustomGenerics_Pair__0__1_Deconstruct_0_1),
	reinterpret_cast<void(*)()>(&CustomGenericsSubjects_CustomGenerics_Pair_System_Int32_System_String__ctor_System_Int32_System_String),
	reinterpret_cast<void(*)()>(&CustomGenericsSubjects_CustomGenerics_Pair_System_Int32_System_String_Deconstruct_System_Int32_System_String),
	reinterpret_cast<void(*)()>(&CustomGenericsSubjects_CustomGenerics_Util__ctor),
	reinterpret_cast<void(*)()>(&CustomGenericsSubjects_CustomGenerics_Util_Echo_0),
	reinterpret_cast<void(*)()>(&CustomGenericsSubjects_CustomGenerics_Util_Echo_System_Int32_System_Int32),
	reinterpret_cast<void(*)()>(&CustomGenericsSubjects_CustomGenerics_Util_Equal_0_0),
	reinterpret_cast<void(*)()>(&CustomGenericsSubjects_CustomGenerics_Util_Equal_System_Int32_System_Int32_System_Int32),
	reinterpret_cast<void(*)()>(&CustomGenericsSubjects_CustomGenerics_Wrapper_1__ctor_0),
	reinterpret_cast<void(*)()>(&CustomGenericsSubjects_CustomGenerics_Wrapper_1_GetRefType),
	reinterpret_cast<void(*)()>(&CustomGenericsSubjects_CustomGenerics_Wrapper__0__ctor_0),
	reinterpret_cast<void(*)()>(&CustomGenericsSubjects_CustomGenerics_Wrapper__0_GetRefType),
	reinterpret_cast<void(*)()>(&CustomGenericsSubjects_CustomGenerics_Wrapper_System_String__ctor_System_String),
	reinterpret_cast<void(*)()>(&CustomGenericsSubjects_CustomGenerics_Wrapper_System_String_GetRefType),
};

// ── Default-arg thunks (kDefaultArgThunks[]) ────────────────────────────
// Each thunk supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_null_instance = 0;
extern "C" void (*kDefaultArgThunks[35])() = {
	[]() {kMethodTable[0]();},
	[]() {kMethodTable[1]();},
	[]() {kMethodTable[2]();},
	[]() {kMethodTable[3]();},
	[]() {kMethodTable[4]();},
	[]() {kMethodTable[5]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[6])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance),0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[7])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[8])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[9])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance),0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[10])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance),0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[11])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[12])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[13])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance),0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[14])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance),0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[15])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[16])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[17])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance),0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[18])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance),0,0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[19])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance),([](){ static CHAOS_IL2CPP_UINT8 __b[sizeof(CHAOS_IL2CPP_INTPTR)] = {}; return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(__b); })(),([](){ static CHAOS_IL2CPP_UINT8 __b[sizeof(CHAOS_IL2CPP_INTPTR)] = {}; return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(__b); })());},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[20])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance),0,0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[21])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance),([](){ static CHAOS_IL2CPP_UINT8 __b[sizeof(CHAOS_IL2CPP_INTPTR)] = {}; return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(__b); })(),([](){ static CHAOS_IL2CPP_UINT8 __b[sizeof(CHAOS_IL2CPP_INTPTR)] = {}; return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(__b); })());},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[22])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance),0,chaos_make_string_id_value(385996094756309509ULL));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[23])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance),([](){ static CHAOS_IL2CPP_UINT8 __b[sizeof(CHAOS_IL2CPP_INTPTR)] = {}; return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(__b); })(),([](){ static CHAOS_IL2CPP_UINT8 __b[sizeof(CHAOS_IL2CPP_INTPTR)] = {}; return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(__b); })());},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[24])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[25])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[26])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[27])(0,0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[28])(0,0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[29])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance),0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[30])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[31])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance),0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[32])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[33])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance),chaos_make_string_id_value(385996094756309509ULL));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[34])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
};

// ── Subject slot map (kSubjectSlotMap[]) ────────────────────────────
// Maps subject index (0-based sequential) to kMethodTable index.
// External consumers use this to route --benchmark N to the correct
// AOT method slot, since kMethodTable[] includes lambdas/closures
// that shift subject methods to non-contiguous indices.
extern "C" const int kSubjectEntryCount = 6;
extern "C" const int kSubjectSlotMap[6] = {
	0,
	1,
	2,
	3,
	4,
	5,
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

	/* ── Entry 0: CustomGenericsSubjects_CustomGenericsSubjects_Subject_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry0;
	/* ── Entry 1: CustomGenericsSubjects_CustomGenericsSubjects_Subject_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry1;
	/* ── Entry 2: CustomGenericsSubjects_CustomGenericsSubjects_Subject_2 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry2;
	/* ── Entry 3: CustomGenericsSubjects_CustomGenericsSubjects_Subject_3 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry3;
	/* ── Entry 4: CustomGenericsSubjects_CustomGenericsSubjects_Subject_4 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry4;
	/* ── Entry 5: CustomGenericsSubjects_CustomGenericsSubjects_Subject_5 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry5;
	/* ── Entry 6: CustomGenericsSubjects_CustomGenerics_Box_1__ctor_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry6;
	/* ── Entry 7: CustomGenericsSubjects_CustomGenerics_Box_1_Get ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry7;
	/* ── Entry 8: CustomGenericsSubjects_CustomGenerics_Box_1_IsDefault ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry8;
	/* ── Entry 9: CustomGenericsSubjects_CustomGenerics_Box_1_Set_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry9;
	/* ── Entry 10: CustomGenericsSubjects_CustomGenerics_Box__0__ctor_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry10;
	/* ── Entry 11: CustomGenericsSubjects_CustomGenerics_Box__0_Get ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry11;
	/* ── Entry 12: CustomGenericsSubjects_CustomGenerics_Box__0_IsDefault ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry12;
	/* ── Entry 13: CustomGenericsSubjects_CustomGenerics_Box__0_Set_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry13;
	/* ── Entry 14: CustomGenericsSubjects_CustomGenerics_Box_System_Int32__ctor_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry14;
	/* ── Entry 15: CustomGenericsSubjects_CustomGenerics_Box_System_Int32_Get ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry15;
	/* ── Entry 16: CustomGenericsSubjects_CustomGenerics_Box_System_Int32_IsDefault ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry16;
	/* ── Entry 17: CustomGenericsSubjects_CustomGenerics_Box_System_Int32_Set_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry17;
	/* ── Entry 18: CustomGenericsSubjects_CustomGenerics_Pair_2__ctor_0_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry18;
	/* ── Entry 19: CustomGenericsSubjects_CustomGenerics_Pair_2_Deconstruct_0_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry19;
	/* ── Entry 20: CustomGenericsSubjects_CustomGenerics_Pair__0__1__ctor_0_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry20;
	/* ── Entry 21: CustomGenericsSubjects_CustomGenerics_Pair__0__1_Deconstruct_0_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry21;
	/* ── Entry 22: CustomGenericsSubjects_CustomGenerics_Pair_System_Int32_System_String__ctor_System_Int32_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry22;
	/* ── Entry 23: CustomGenericsSubjects_CustomGenerics_Pair_System_Int32_System_String_Deconstruct_System_Int32_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry23;
	/* ── Entry 24: CustomGenericsSubjects_CustomGenerics_Util__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry24;
	/* ── Entry 25: CustomGenericsSubjects_CustomGenerics_Util_Echo_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry25;
	/* ── Entry 26: CustomGenericsSubjects_CustomGenerics_Util_Echo_System_Int32_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry26;
	/* ── Entry 27: CustomGenericsSubjects_CustomGenerics_Util_Equal_0_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry27;
	/* ── Entry 28: CustomGenericsSubjects_CustomGenerics_Util_Equal_System_Int32_System_Int32_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry28;
	/* ── Entry 29: CustomGenericsSubjects_CustomGenerics_Wrapper_1__ctor_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry29;
	/* ── Entry 30: CustomGenericsSubjects_CustomGenerics_Wrapper_1_GetRefType ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry30;
	/* ── Entry 31: CustomGenericsSubjects_CustomGenerics_Wrapper__0__ctor_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry31;
	/* ── Entry 32: CustomGenericsSubjects_CustomGenerics_Wrapper__0_GetRefType ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry32;
	/* ── Entry 33: CustomGenericsSubjects_CustomGenerics_Wrapper_System_String__ctor_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry33;
	/* ── Entry 34: CustomGenericsSubjects_CustomGenerics_Wrapper_System_String_GetRefType ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry34;
} kChaosGcSlotMapsSection = {
	/* entry0 = CustomGenericsSubjects_CustomGenericsSubjects_Subject_0 */
	.entry0 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&CustomGenericsSubjects_CustomGenericsSubjects_Subject_0),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry1 = CustomGenericsSubjects_CustomGenericsSubjects_Subject_1 */
	.entry1 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&CustomGenericsSubjects_CustomGenericsSubjects_Subject_1),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry2 = CustomGenericsSubjects_CustomGenericsSubjects_Subject_2 */
	.entry2 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&CustomGenericsSubjects_CustomGenericsSubjects_Subject_2),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry3 = CustomGenericsSubjects_CustomGenericsSubjects_Subject_3 */
	.entry3 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&CustomGenericsSubjects_CustomGenericsSubjects_Subject_3),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry4 = CustomGenericsSubjects_CustomGenericsSubjects_Subject_4 */
	.entry4 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&CustomGenericsSubjects_CustomGenericsSubjects_Subject_4),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry5 = CustomGenericsSubjects_CustomGenericsSubjects_Subject_5 */
	.entry5 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&CustomGenericsSubjects_CustomGenericsSubjects_Subject_5),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry6 = CustomGenericsSubjects_CustomGenerics_Box_1__ctor_0 */
	.entry6 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&CustomGenericsSubjects_CustomGenerics_Box_1__ctor_0),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry7 = CustomGenericsSubjects_CustomGenerics_Box_1_Get */
	.entry7 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&CustomGenericsSubjects_CustomGenerics_Box_1_Get),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry8 = CustomGenericsSubjects_CustomGenerics_Box_1_IsDefault */
	.entry8 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&CustomGenericsSubjects_CustomGenerics_Box_1_IsDefault),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry9 = CustomGenericsSubjects_CustomGenerics_Box_1_Set_0 */
	.entry9 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&CustomGenericsSubjects_CustomGenerics_Box_1_Set_0),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry10 = CustomGenericsSubjects_CustomGenerics_Box__0__ctor_0 */
	.entry10 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&CustomGenericsSubjects_CustomGenerics_Box__0__ctor_0),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry11 = CustomGenericsSubjects_CustomGenerics_Box__0_Get */
	.entry11 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&CustomGenericsSubjects_CustomGenerics_Box__0_Get),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry12 = CustomGenericsSubjects_CustomGenerics_Box__0_IsDefault */
	.entry12 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&CustomGenericsSubjects_CustomGenerics_Box__0_IsDefault),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry13 = CustomGenericsSubjects_CustomGenerics_Box__0_Set_0 */
	.entry13 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&CustomGenericsSubjects_CustomGenerics_Box__0_Set_0),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry14 = CustomGenericsSubjects_CustomGenerics_Box_System_Int32__ctor_System_Int32 */
	.entry14 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CustomGenericsSubjects_CustomGenerics_Box_System_Int32__ctor_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 16u, 24u }
	},
	/* entry15 = CustomGenericsSubjects_CustomGenerics_Box_System_Int32_Get */
	.entry15 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&CustomGenericsSubjects_CustomGenerics_Box_System_Int32_Get),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry16 = CustomGenericsSubjects_CustomGenerics_Box_System_Int32_IsDefault */
	.entry16 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&CustomGenericsSubjects_CustomGenerics_Box_System_Int32_IsDefault),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry17 = CustomGenericsSubjects_CustomGenerics_Box_System_Int32_Set_System_Int32 */
	.entry17 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CustomGenericsSubjects_CustomGenerics_Box_System_Int32_Set_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 16u, 24u }
	},
	/* entry18 = CustomGenericsSubjects_CustomGenerics_Pair_2__ctor_0_1 */
	.entry18 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&CustomGenericsSubjects_CustomGenerics_Pair_2__ctor_0_1),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry19 = CustomGenericsSubjects_CustomGenerics_Pair_2_Deconstruct_0_1 */
	.entry19 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&CustomGenericsSubjects_CustomGenerics_Pair_2_Deconstruct_0_1),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry20 = CustomGenericsSubjects_CustomGenerics_Pair__0__1__ctor_0_1 */
	.entry20 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&CustomGenericsSubjects_CustomGenerics_Pair__0__1__ctor_0_1),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry21 = CustomGenericsSubjects_CustomGenerics_Pair__0__1_Deconstruct_0_1 */
	.entry21 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&CustomGenericsSubjects_CustomGenerics_Pair__0__1_Deconstruct_0_1),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry22 = CustomGenericsSubjects_CustomGenerics_Pair_System_Int32_System_String__ctor_System_Int32_System_String */
	.entry22 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&CustomGenericsSubjects_CustomGenerics_Pair_System_Int32_System_String__ctor_System_Int32_System_String),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 16u, 24u, 32u }
	},
	/* entry23 = CustomGenericsSubjects_CustomGenerics_Pair_System_Int32_System_String_Deconstruct_System_Int32_System_String */
	.entry23 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&CustomGenericsSubjects_CustomGenerics_Pair_System_Int32_System_String_Deconstruct_System_Int32_System_String),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 16u, 24u, 32u }
	},
	/* entry24 = CustomGenericsSubjects_CustomGenerics_Util__ctor */
	.entry24 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&CustomGenericsSubjects_CustomGenerics_Util__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry25 = CustomGenericsSubjects_CustomGenerics_Util_Echo_0 */
	.entry25 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&CustomGenericsSubjects_CustomGenerics_Util_Echo_0),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry26 = CustomGenericsSubjects_CustomGenerics_Util_Echo_System_Int32_System_Int32 */
	.entry26 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&CustomGenericsSubjects_CustomGenerics_Util_Echo_System_Int32_System_Int32),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 8u }
	},
	/* entry27 = CustomGenericsSubjects_CustomGenerics_Util_Equal_0_0 */
	.entry27 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&CustomGenericsSubjects_CustomGenerics_Util_Equal_0_0),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry28 = CustomGenericsSubjects_CustomGenerics_Util_Equal_System_Int32_System_Int32_System_Int32 */
	.entry28 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&CustomGenericsSubjects_CustomGenerics_Util_Equal_System_Int32_System_Int32_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 16u, 24u }
	},
	/* entry29 = CustomGenericsSubjects_CustomGenerics_Wrapper_1__ctor_0 */
	.entry29 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&CustomGenericsSubjects_CustomGenerics_Wrapper_1__ctor_0),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry30 = CustomGenericsSubjects_CustomGenerics_Wrapper_1_GetRefType */
	.entry30 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CustomGenericsSubjects_CustomGenerics_Wrapper_1_GetRefType),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry31 = CustomGenericsSubjects_CustomGenerics_Wrapper__0__ctor_0 */
	.entry31 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&CustomGenericsSubjects_CustomGenerics_Wrapper__0__ctor_0),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry32 = CustomGenericsSubjects_CustomGenerics_Wrapper__0_GetRefType */
	.entry32 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CustomGenericsSubjects_CustomGenerics_Wrapper__0_GetRefType),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry33 = CustomGenericsSubjects_CustomGenerics_Wrapper_System_String__ctor_System_String */
	.entry33 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&CustomGenericsSubjects_CustomGenerics_Wrapper_System_String__ctor_System_String),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry34 = CustomGenericsSubjects_CustomGenerics_Wrapper_System_String_GetRefType */
	.entry34 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CustomGenericsSubjects_CustomGenerics_Wrapper_System_String_GetRefType),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 1232u;

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[35] = {
	reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenericsSubjects_Subject_0),
	reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenericsSubjects_Subject_1),
	reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenericsSubjects_Subject_2),
	reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenericsSubjects_Subject_3),
	reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenericsSubjects_Subject_4),
	reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenericsSubjects_Subject_5),
	reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Box_1__ctor_0),
	reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Box_1_Get),
	reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Box_1_IsDefault),
	reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Box_1_Set_0),
	reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Box__0__ctor_0),
	reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Box__0_Get),
	reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Box__0_IsDefault),
	reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Box__0_Set_0),
	reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Box_System_Int32__ctor_System_Int32),
	reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Box_System_Int32_Get),
	reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Box_System_Int32_IsDefault),
	reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Box_System_Int32_Set_System_Int32),
	reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Pair_2__ctor_0_1),
	reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Pair_2_Deconstruct_0_1),
	reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Pair__0__1__ctor_0_1),
	reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Pair__0__1_Deconstruct_0_1),
	reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Pair_System_Int32_System_String__ctor_System_Int32_System_String),
	reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Pair_System_Int32_System_String_Deconstruct_System_Int32_System_String),
	reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Util__ctor),
	reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Util_Echo_0),
	reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Util_Echo_System_Int32_System_Int32),
	reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Util_Equal_0_0),
	reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Util_Equal_System_Int32_System_Int32_System_Int32),
	reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Wrapper_1__ctor_0),
	reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Wrapper_1_GetRefType),
	reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Wrapper__0__ctor_0),
	reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Wrapper__0_GetRefType),
	reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Wrapper_System_String__ctor_System_String),
	reinterpret_cast<void*>(&CustomGenericsSubjects_CustomGenerics_Wrapper_System_String_GetRefType),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
// ── VTable descriptors (for BootstrapRuntime TypeVTable registration) ──
static const VTableDescriptorV0 kChaosVTableDescriptors[] = {
	{
		CHAOS_IL2CPP_UINT64_C(0xE6CE184BF7D69861),
		0x00000009u,
		0u,
		4u,
		reinterpret_cast<const void*>(kSlots_CustomGenericsSubjects_CustomGenerics_Box__0_),
		reinterpret_cast<const void**>(chaos_vtable_CustomGenericsSubjects_CustomGenerics_Box__0_),
		4u,
		1,
		{0, 0, 0},
		nullptr,
		0u,
	},
	{
		CHAOS_IL2CPP_UINT64_C(0x48464C4A64C07557),
		0x00000006u,
		0u,
		7u,
		reinterpret_cast<const void*>(kSlots_CustomGenericsSubjects_CustomGenerics_Box_System_Int32_),
		reinterpret_cast<const void**>(chaos_vtable_CustomGenericsSubjects_CustomGenerics_Box_System_Int32_),
		7u,
		1,
		{0, 0, 0},
		nullptr,
		0u,
	},
	{
		CHAOS_IL2CPP_UINT64_C(0xE389B17BDF8F0BA3),
		0x0000000Bu,
		0u,
		8u,
		reinterpret_cast<const void*>(kSlots_CustomGenericsSubjects_CustomGenerics_Wrapper__0_),
		reinterpret_cast<const void**>(chaos_vtable_CustomGenericsSubjects_CustomGenerics_Wrapper__0_),
		8u,
		1,
		{0, 0, 0},
		nullptr,
		0u,
	},
	{
		CHAOS_IL2CPP_UINT64_C(0x15BEA5F6AAAA0790),
		0x00000008u,
		0u,
		9u,
		reinterpret_cast<const void*>(kSlots_CustomGenericsSubjects_CustomGenerics_Wrapper_System_String_),
		reinterpret_cast<const void**>(chaos_vtable_CustomGenericsSubjects_CustomGenerics_Wrapper_System_String_),
		9u,
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
	.method_pointer_count      = 35u,
	.reverse_pinvoke_wrappers  = nullptr,
	.reverse_pinvoke_wrapper_count = 0u,
	.invoker_pointers          = nullptr,
	.invoker_pointer_count     = 0u,
	.unresolved_virtual_calls = nullptr,
	.unresolved_virtual_call_count = 0u,
	.type_capabilities       = nullptr,
	.type_capability_count   = 0u,
	.vtable_descriptors = kChaosVTableDescriptors,
	.vtable_descriptor_count = 4u,
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
	.image_name_utf8    = "CustomGenericsSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_CustomGenericsSubjects_CustomGenericsSubjects[1] = {
	{ 0x04000001u, "CustomGenericsSubjects/CustomGenericsSubjects::_exitCode", "_exitCode", "System.Int32", 0LL, 3u },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_CustomGenericsSubjects_CustomGenericsSubjects[6] = {
	{ 0x00000019u, "CustomGenericsSubjects/CustomGenericsSubjects::Subject_0:System.Void()", "Subject_0", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000001Au, "CustomGenericsSubjects/CustomGenericsSubjects::Subject_1:System.Void()", "Subject_1", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000001Bu, "CustomGenericsSubjects/CustomGenericsSubjects::Subject_2:System.Void()", "Subject_2", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000001Cu, "CustomGenericsSubjects/CustomGenericsSubjects::Subject_3:System.Void()", "Subject_3", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000001Du, "CustomGenericsSubjects/CustomGenericsSubjects::Subject_4:System.Void()", "Subject_4", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000001Eu, "CustomGenericsSubjects/CustomGenericsSubjects::Subject_5:System.Void()", "Subject_5", "System.Void", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryFieldDescriptor kReflFields_CustomGenericsSubjects_CustomGenerics_Box_1[1] = {
	{ 0x04000002u, "CustomGenericsSubjects/CustomGenerics.Box`1::_value", "_value", "System.Int32", 0LL, 0u },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_CustomGenericsSubjects_CustomGenerics_Box_1[4] = {
	{ 0x0000001Fu, "CustomGenericsSubjects/CustomGenerics.Box`1::.ctor:System.Void(!0)", ".ctor", "System.Void", 1, nullptr, 0u, nullptr, 0u },
	{ 0x00000020u, "CustomGenericsSubjects/CustomGenerics.Box`1::Get:!0()", "Get", "!0", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000022u, "CustomGenericsSubjects/CustomGenerics.Box`1::IsDefault:System.Boolean()", "IsDefault", "System.Boolean", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000021u, "CustomGenericsSubjects/CustomGenerics.Box`1::Set:System.Void(!0)", "Set", "System.Void", 1, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_CustomGenericsSubjects_CustomGenerics_Box__0_[4] = {
	{ 0x00000034u, "CustomGenericsSubjects/CustomGenerics.Box<!0>::.ctor:System.Void(!0)", ".ctor", "System.Void", 1, nullptr, 0u, nullptr, 0u },
	{ 0x00000035u, "CustomGenericsSubjects/CustomGenerics.Box<!0>::Get:!0()", "Get", "!0", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000037u, "CustomGenericsSubjects/CustomGenerics.Box<!0>::IsDefault:System.Boolean()", "IsDefault", "System.Boolean", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000036u, "CustomGenericsSubjects/CustomGenerics.Box<!0>::Set:System.Void(!0)", "Set", "System.Void", 1, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_CustomGenericsSubjects_CustomGenerics_Box_System_Int32_[4] = {
	{ 0x0000002Cu, "CustomGenericsSubjects/CustomGenerics.Box<System.Int32>::.ctor:System.Void(System.Int32)", ".ctor", "System.Void", 1, nullptr, 0u, nullptr, 0u },
	{ 0x0000002Du, "CustomGenericsSubjects/CustomGenerics.Box<System.Int32>::Get:System.Int32()", "Get", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000002Fu, "CustomGenericsSubjects/CustomGenerics.Box<System.Int32>::IsDefault:System.Boolean()", "IsDefault", "System.Boolean", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000002Eu, "CustomGenericsSubjects/CustomGenerics.Box<System.Int32>::Set:System.Void(System.Int32)", "Set", "System.Void", 1, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryFieldDescriptor kReflFields_CustomGenericsSubjects_CustomGenerics_Pair_2[2] = {
	{ 0x04000003u, "CustomGenericsSubjects/CustomGenerics.Pair`2::First", "First", "System.Int32", 0LL, 1u },
	{ 0x04000004u, "CustomGenericsSubjects/CustomGenerics.Pair`2::Second", "Second", "System.Int32", 0LL, 1u },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_CustomGenericsSubjects_CustomGenerics_Pair_2[2] = {
	{ 0x00000023u, "CustomGenericsSubjects/CustomGenerics.Pair`2::.ctor:System.Void(!0,!1)", ".ctor", "System.Void", 2, nullptr, 0u, nullptr, 0u },
	{ 0x00000024u, "CustomGenericsSubjects/CustomGenerics.Pair`2::Deconstruct:System.Void(!0&,!1&)", "Deconstruct", "System.Void", 2, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_CustomGenericsSubjects_CustomGenerics_Pair__0__1_[2] = {
	{ 0x00000038u, "CustomGenericsSubjects/CustomGenerics.Pair<!0,!1>::.ctor:System.Void(!0,!1)", ".ctor", "System.Void", 2, nullptr, 0u, nullptr, 0u },
	{ 0x00000039u, "CustomGenericsSubjects/CustomGenerics.Pair<!0,!1>::Deconstruct:System.Void(!0&,!1&)", "Deconstruct", "System.Void", 2, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_CustomGenericsSubjects_CustomGenerics_Pair_System_Int32_System_String_[2] = {
	{ 0x00000030u, "CustomGenericsSubjects/CustomGenerics.Pair<System.Int32,System.String>::.ctor:System.Void(System.Int32,System.String)", ".ctor", "System.Void", 2, nullptr, 0u, nullptr, 0u },
	{ 0x00000031u, "CustomGenericsSubjects/CustomGenerics.Pair<System.Int32,System.String>::Deconstruct:System.Void(System.Int32&,System.String&)", "Deconstruct", "System.Void", 2, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_CustomGenericsSubjects_CustomGenerics_Util[5] = {
	{ 0x00000027u, "CustomGenericsSubjects/CustomGenerics.Util::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000025u, "CustomGenericsSubjects/CustomGenerics.Util::Echo`1:!!0(!!0)", "Echo`1", "!!0", 1, nullptr, 0u, nullptr, 0u },
	{ 0x0000002Au, "CustomGenericsSubjects/CustomGenerics.Util::Echo<System.Int32>:System.Int32(System.Int32)", "Echo<System.Int32>", "System.Int32", 1, nullptr, 0u, nullptr, 0u },
	{ 0x00000026u, "CustomGenericsSubjects/CustomGenerics.Util::Equal`1:System.Boolean(!!0,!!0)", "Equal`1", "System.Boolean", 2, nullptr, 0u, nullptr, 0u },
	{ 0x0000002Bu, "CustomGenericsSubjects/CustomGenerics.Util::Equal<System.Int32>:System.Boolean(System.Int32,System.Int32)", "Equal<System.Int32>", "System.Boolean", 2, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryFieldDescriptor kReflFields_CustomGenericsSubjects_CustomGenerics_Wrapper_1[1] = {
	{ 0x04000005u, "CustomGenericsSubjects/CustomGenerics.Wrapper`1::Ref", "Ref", "System.Int32", 0LL, 1u },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_CustomGenericsSubjects_CustomGenerics_Wrapper_1[2] = {
	{ 0x00000028u, "CustomGenericsSubjects/CustomGenerics.Wrapper`1::.ctor:System.Void(!0)", ".ctor", "System.Void", 1, nullptr, 0u, nullptr, 0u },
	{ 0x00000029u, "CustomGenericsSubjects/CustomGenerics.Wrapper`1::GetRefType:System.Type()", "GetRefType", "System.Type", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_CustomGenericsSubjects_CustomGenerics_Wrapper__0_[2] = {
	{ 0x0000003Au, "CustomGenericsSubjects/CustomGenerics.Wrapper<!0>::.ctor:System.Void(!0)", ".ctor", "System.Void", 1, nullptr, 0u, nullptr, 0u },
	{ 0x0000003Bu, "CustomGenericsSubjects/CustomGenerics.Wrapper<!0>::GetRefType:System.Type()", "GetRefType", "System.Type", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_CustomGenericsSubjects_CustomGenerics_Wrapper_System_String_[2] = {
	{ 0x00000032u, "CustomGenericsSubjects/CustomGenerics.Wrapper<System.String>::.ctor:System.Void(System.String)", ".ctor", "System.Void", 1, nullptr, 0u, nullptr, 0u },
	{ 0x00000033u, "CustomGenericsSubjects/CustomGenerics.Wrapper<System.String>::GetRefType:System.Type()", "GetRefType", "System.Type", 0, nullptr, 0u, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[11] = {
	{ 0x00000001u, "CustomGenericsSubjects/CustomGenericsSubjects", "CustomGenericsSubjects/CustomGenericsSubjects", "", "CustomGenericsSubjects", "CustomGenericsSubjects", nullptr, kReflFields_CustomGenericsSubjects_CustomGenericsSubjects, 1u, nullptr, 0u,
nullptr, 0u,    kReflMethods_CustomGenericsSubjects_CustomGenericsSubjects, 6u, nullptr, 0u, 0u },
	{ 0x00000002u, "CustomGenericsSubjects/CustomGenerics.Box`1", "CustomGenericsSubjects/CustomGenerics.Box`1", "CustomGenerics", "CustomGenerics.Box`1", "CustomGenerics.Box`1", nullptr, kReflFields_CustomGenericsSubjects_CustomGenerics_Box_1, 1u, nullptr, 0u,
nullptr, 0u,    kReflMethods_CustomGenericsSubjects_CustomGenerics_Box_1, 4u, nullptr, 0u, 0u },
	{ 0x00000009u, "CustomGenericsSubjects/CustomGenerics.Box<!0>", "CustomGenericsSubjects/CustomGenerics.Box<!0>", "CustomGenerics", "CustomGenerics.Box<!0>", "CustomGenerics.Box<!0>", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_CustomGenericsSubjects_CustomGenerics_Box__0_, 4u, nullptr, 0u, 0u },
	{ 0x00000006u, "CustomGenericsSubjects/CustomGenerics.Box<System.Int32>", "CustomGenericsSubjects/CustomGenerics.Box<System.Int32>", "CustomGenerics.Box<System", "CustomGenerics.Box<System.Int32>", "CustomGenerics.Box<System.Int32>", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_CustomGenericsSubjects_CustomGenerics_Box_System_Int32_, 4u, nullptr, 0u, 0u },
	{ 0x00000003u, "CustomGenericsSubjects/CustomGenerics.Pair`2", "CustomGenericsSubjects/CustomGenerics.Pair`2", "CustomGenerics", "CustomGenerics.Pair`2", "CustomGenerics.Pair`2", nullptr, kReflFields_CustomGenericsSubjects_CustomGenerics_Pair_2, 2u, nullptr, 0u,
nullptr, 0u,    kReflMethods_CustomGenericsSubjects_CustomGenerics_Pair_2, 2u, nullptr, 0u, 0u },
	{ 0x0000000Au, "CustomGenericsSubjects/CustomGenerics.Pair<!0,!1>", "CustomGenericsSubjects/CustomGenerics.Pair<!0,!1>", "CustomGenerics", "CustomGenerics.Pair<!0,!1>", "CustomGenerics.Pair<!0,!1>", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_CustomGenericsSubjects_CustomGenerics_Pair__0__1_, 2u, nullptr, 0u, 0u },
	{ 0x00000007u, "CustomGenericsSubjects/CustomGenerics.Pair<System.Int32,System.String>", "CustomGenericsSubjects/CustomGenerics.Pair<System.Int32,System.String>", "CustomGenerics.Pair<System.Int32,System", "CustomGenerics.Pair<System.Int32,System.String>", "CustomGenerics.Pair<System.Int32,System.String>", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_CustomGenericsSubjects_CustomGenerics_Pair_System_Int32_System_String_, 2u, nullptr, 0u, 0u },
	{ 0x00000004u, "CustomGenericsSubjects/CustomGenerics.Util", "CustomGenericsSubjects/CustomGenerics.Util", "CustomGenerics", "CustomGenerics.Util", "CustomGenerics.Util", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_CustomGenericsSubjects_CustomGenerics_Util, 5u, nullptr, 0u, 0u },
	{ 0x00000005u, "CustomGenericsSubjects/CustomGenerics.Wrapper`1", "CustomGenericsSubjects/CustomGenerics.Wrapper`1", "CustomGenerics", "CustomGenerics.Wrapper`1", "CustomGenerics.Wrapper`1", nullptr, kReflFields_CustomGenericsSubjects_CustomGenerics_Wrapper_1, 1u, nullptr, 0u,
nullptr, 0u,    kReflMethods_CustomGenericsSubjects_CustomGenerics_Wrapper_1, 2u, nullptr, 0u, 0u },
	{ 0x0000000Bu, "CustomGenericsSubjects/CustomGenerics.Wrapper<!0>", "CustomGenericsSubjects/CustomGenerics.Wrapper<!0>", "CustomGenerics", "CustomGenerics.Wrapper<!0>", "CustomGenerics.Wrapper<!0>", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_CustomGenericsSubjects_CustomGenerics_Wrapper__0_, 2u, nullptr, 0u, 0u },
	{ 0x00000008u, "CustomGenericsSubjects/CustomGenerics.Wrapper<System.String>", "CustomGenericsSubjects/CustomGenerics.Wrapper<System.String>", "CustomGenerics.Wrapper<System", "CustomGenerics.Wrapper<System.String>", "CustomGenerics.Wrapper<System.String>", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_CustomGenericsSubjects_CustomGenerics_Wrapper_System_String_, 2u, nullptr, 0u, 0u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[11] = {
	&kReflTypes[0],
	&kReflTypes[1],
	&kReflTypes[2],
	&kReflTypes[3],
	&kReflTypes[4],
	&kReflTypes[5],
	&kReflTypes[6],
	&kReflTypes[7],
	&kReflTypes[8],
	&kReflTypes[9],
	&kReflTypes[10],
};

static const ReflectionQueryImageDescriptor kReflImage = { "CustomGenericsSubjects", kReflTypePtrs, 11u, 1, 0, 0, 0 };

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
// Managed method: CustomGenericsSubjects/CustomGenericsSubjects::Subject_0()
extern "C" void CustomGenericsSubjects_CustomGenericsSubjects_Subject_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s0;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_CustomGenericsSubjects_CustomGenerics_Box_System_Int32_, {});
				chaos_object->header.type_info = chaos_mt_CustomGenericsSubjects_CustomGenerics_Box_System_Int32_.AsTypeInfoHot();
				chaos_stub_definition_CustomGenericsSubjects_CustomGenerics_Box_1___ctor_System_Void__0__type__System_Int32__method(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			{
				// Inlined: CustomGenericsSubjects/CustomGenerics.Box<System.Int32>::Get:System.Int32()
				auto chaos_inline_arg_0 = _s0;
				_s0 = chaos_inline_arg_0;
				{
					auto* chaos_object = reinterpret_cast<chaos_type_CustomGenericsSubjects_CustomGenerics_Box_System_Int32_*>(_s0);
					_s0 = chaos_object->field_CustomGenericsSubjects_CustomGenerics_Box_System_Int32____value;
				}
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[1] = _s0;
			_s0 = chaos_locals[1];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_CustomGenericsSubjects_CustomGenericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (!chaos_eh_match_type(CHAOS_EH_EXCEPTION_OBJ, chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
		{ CHAOS_EH_RETHROW; }
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_CustomGenericsSubjects_CustomGenericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: CustomGenericsSubjects/CustomGenericsSubjects::Subject_1()
extern "C" void CustomGenericsSubjects_CustomGenericsSubjects_Subject_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s0;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_CustomGenericsSubjects_CustomGenerics_Box_System_Int32_, {});
				chaos_object->header.type_info = chaos_mt_CustomGenericsSubjects_CustomGenerics_Box_System_Int32_.AsTypeInfoHot();
				chaos_stub_definition_CustomGenericsSubjects_CustomGenerics_Box_1___ctor_System_Void__0__type__System_Int32__method(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(100);
			{
				// Inlined: CustomGenericsSubjects/CustomGenerics.Box<System.Int32>::Set:System.Void(System.Int32)
				auto chaos_inline_arg_1 = _s1;
				auto chaos_inline_arg_0 = _s0;
				_s0 = chaos_inline_arg_0;
				_s1 = chaos_inline_arg_1;
				{
					auto chaos_value = _s1;
					if (chaos_is_string_id(chaos_value))
					{
						chaos_value = chaos_string_materialize(chaos_value);
					}
					auto* chaos_object = reinterpret_cast<chaos_type_CustomGenericsSubjects_CustomGenerics_Box_System_Int32_*>(_s0);
					BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_object->field_CustomGenericsSubjects_CustomGenerics_Box_System_Int32____value));
					chaos_object->field_CustomGenericsSubjects_CustomGenerics_Box_System_Int32____value = chaos_value;
					chaos_gc_dirty_card(chaos_object);
				}
			}
			_s0 = chaos_locals[0];
			{
				// Inlined: CustomGenericsSubjects/CustomGenerics.Box<System.Int32>::Get:System.Int32()
				auto chaos_inline_arg_0 = _s0;
				_s0 = chaos_inline_arg_0;
				{
					auto* chaos_object = reinterpret_cast<chaos_type_CustomGenericsSubjects_CustomGenerics_Box_System_Int32_*>(_s0);
					_s0 = chaos_object->field_CustomGenericsSubjects_CustomGenerics_Box_System_Int32____value;
				}
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(100);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[1] = _s0;
			_s0 = chaos_locals[1];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_CustomGenericsSubjects_CustomGenericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (!chaos_eh_match_type(CHAOS_EH_EXCEPTION_OBJ, chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
		{ CHAOS_EH_RETHROW; }
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_CustomGenericsSubjects_CustomGenericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: CustomGenericsSubjects/CustomGenericsSubjects::Subject_2()
extern "C" void CustomGenericsSubjects_CustomGenericsSubjects_Subject_2(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 4) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	CHAOS_EH_TRY
			_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				_s2 = CHAOS_IL2CPP_STRING_ID("one");
			}
			{
				// Inlined: CustomGenericsSubjects/CustomGenerics.Pair<System.Int32,System.String>::.ctor:System.Void(System.Int32,System.String)
				auto chaos_inline_arg_2 = _s2;
				auto chaos_inline_arg_1 = _s1;
				auto chaos_inline_arg_0 = _s0;
				_s0 = chaos_inline_arg_0;
				_s1 = chaos_inline_arg_1;
				{
					auto chaos_value = _s1;
					if (chaos_is_string_id(chaos_value))
					{
						chaos_value = chaos_string_materialize(chaos_value);
					}
					auto* chaos_value_owner = chaos_resolve_managed_value_pointer<chaos_valuetype_CustomGenericsSubjects_CustomGenerics_Pair_System_Int32_System_String_>(_s0);
					chaos_value_owner->field_CustomGenericsSubjects_CustomGenerics_Pair_System_Int32_System_String___First = chaos_value;
					if (chaos_is_gc_pointer(chaos_value_owner))
					{
						chaos_gc_dirty_card(chaos_value_owner);
					}
				}
				_s0 = chaos_inline_arg_0;
				_s1 = chaos_inline_arg_2;
				{
					auto chaos_value = _s1;
					if (chaos_is_string_id(chaos_value))
					{
						chaos_value = chaos_string_materialize(chaos_value);
					}
					auto* chaos_value_owner = chaos_resolve_managed_value_pointer<chaos_valuetype_CustomGenericsSubjects_CustomGenerics_Pair_System_Int32_System_String_>(_s0);
					chaos_value_owner->field_CustomGenericsSubjects_CustomGenerics_Pair_System_Int32_System_String___Second = chaos_value;
					if (chaos_is_gc_pointer(chaos_value_owner))
					{
						chaos_gc_dirty_card(chaos_value_owner);
					}
				}
			}
			_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
			_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[2]);
			{
				// Inlined: CustomGenericsSubjects/CustomGenerics.Pair<System.Int32,System.String>::Deconstruct:System.Void(System.Int32&,System.String&)
				auto chaos_inline_arg_2 = _s2;
				auto chaos_inline_arg_1 = _s1;
				auto chaos_inline_arg_0 = _s0;
				_s0 = chaos_inline_arg_1;
				_s1 = chaos_inline_arg_0;
				{
					auto* chaos_value = chaos_resolve_managed_value_pointer<chaos_valuetype_CustomGenericsSubjects_CustomGenerics_Pair_System_Int32_System_String_>(_s1);
					_s1 = chaos_value->field_CustomGenericsSubjects_CustomGenerics_Pair_System_Int32_System_String___First;
				}
				{
					const auto chaos_value = _s1;
					auto* chaos_destination = chaos_resolve_managed_value_pointer<CHAOS_IL2CPP_INTPTR>(_s0);
					*chaos_destination = chaos_value;
				}
				_s0 = chaos_inline_arg_2;
				_s1 = chaos_inline_arg_0;
				{
					auto* chaos_value = chaos_resolve_managed_value_pointer<chaos_valuetype_CustomGenericsSubjects_CustomGenerics_Pair_System_Int32_System_String_>(_s1);
					_s1 = chaos_value->field_CustomGenericsSubjects_CustomGenerics_Pair_System_Int32_System_String___Second;
				}
				{
					const auto chaos_value = _s1;
					auto* chaos_destination = chaos_resolve_managed_value_pointer<CHAOS_IL2CPP_INTPTR>(_s0);
					*chaos_destination = chaos_value;
				}
			}
			_s0 = chaos_locals[1];
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			// bne.un (structured EH branch)
			_s2 = chaos_locals[2];
			{
				_s3 = CHAOS_IL2CPP_STRING_ID("one");
			}
			{
				const auto chaos_arg_1 = _s3;
				const auto chaos_arg_0 = _s2;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[1])(chaos_arg_0, chaos_arg_1);
				_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// br (handled via structured EH branches)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			chaos_locals[3] = _s3;
			_s3 = chaos_locals[3];
			// brfalse (structured EH branch)
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s4;
				chaos_static_CustomGenericsSubjects_CustomGenericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (!chaos_eh_match_type(CHAOS_EH_EXCEPTION_OBJ, chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
		{ CHAOS_EH_RETHROW; }
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_CustomGenericsSubjects_CustomGenericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: CustomGenericsSubjects/CustomGenericsSubjects::Subject_3()
extern "C" void CustomGenericsSubjects_CustomGenericsSubjects_Subject_3(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				// Inlined: CustomGenericsSubjects/CustomGenerics.Util::Echo<System.Int32>:System.Int32(System.Int32)
				auto chaos_inline_arg_0 = _s0;
				_s0 = chaos_inline_arg_0;
			}
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[1] = _s0;
			_s0 = chaos_locals[1];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_CustomGenericsSubjects_CustomGenericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (!chaos_eh_match_type(CHAOS_EH_EXCEPTION_OBJ, chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
		{ CHAOS_EH_RETHROW; }
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_CustomGenericsSubjects_CustomGenericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: CustomGenericsSubjects/CustomGenericsSubjects::Subject_4()
extern "C" void CustomGenericsSubjects_CustomGenericsSubjects_Subject_4(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 4) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = CustomGenericsSubjects_CustomGenerics_Util_Equal_System_Int32_System_Int32_System_Int32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[2] = _s0;
			_s0 = chaos_locals[2];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_CustomGenericsSubjects_CustomGenericsSubjects___exitCode = chaos_value;
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = CustomGenericsSubjects_CustomGenerics_Util_Equal_System_Int32_System_Int32_System_Int32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[1] = _s1;
			_s1 = chaos_locals[1];
			chaos_locals[3] = _s1;
			_s1 = chaos_locals[3];
			// brfalse (structured EH branch)
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s2;
				chaos_static_CustomGenericsSubjects_CustomGenericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (!chaos_eh_match_type(CHAOS_EH_EXCEPTION_OBJ, chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
		{ CHAOS_EH_RETHROW; }
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_CustomGenericsSubjects_CustomGenericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: CustomGenericsSubjects/CustomGenericsSubjects::Subject_5()
extern "C" void CustomGenericsSubjects_CustomGenericsSubjects_Subject_5(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	CHAOS_EH_TRY
			{
				_s0 = CHAOS_IL2CPP_STRING_ID("data");
			}
			{
				const auto chaos_arg_1 = _s0;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_CustomGenericsSubjects_CustomGenerics_Wrapper_System_String_, {});
				chaos_object->header.type_info = chaos_mt_CustomGenericsSubjects_CustomGenerics_Wrapper_System_String_.AsTypeInfoHot();
				chaos_stub_definition_CustomGenericsSubjects_CustomGenerics_Wrapper_1___ctor_System_Void__0__type__System_String__method(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object), chaos_arg_1);
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			{
				// Inlined: CustomGenericsSubjects/CustomGenerics.Wrapper<System.String>::GetRefType:System.Type()
				auto chaos_inline_arg_0 = _s0;
				_s0 = chaos_inline_arg_0;
				{
					auto* chaos_object = reinterpret_cast<chaos_type_CustomGenericsSubjects_CustomGenerics_Wrapper_System_String_*>(_s0);
					_s0 = chaos_object->field_CustomGenericsSubjects_CustomGenerics_Wrapper_System_String___Ref;
				}
				{
					const auto chaos_value = _s0;
					auto* chaos_boxed = CHAOS_IL2CPP_NEW_GC(chaos_boxed_type_CustomGenericsSubjects_System_String, {});
					chaos_boxed->header.type_info = chaos_mt_CustomGenericsSubjects_System_String.AsTypeInfoHot();
					chaos_boxed->value = chaos_value;
					_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_boxed);
				}
				{
					const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[21])();
					_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
				}
			}
			chaos_locals[1] = _s1;
			_s1 = chaos_locals[1];
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(49037434u);
			{
				const auto chaos_arg_0 = _s2;
				const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
				_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosTypeInequality(chaos_arg_0, chaos_arg_1);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[2] = _s1;
			_s1 = chaos_locals[2];
			// brfalse (structured EH branch)
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s2;
				chaos_static_CustomGenericsSubjects_CustomGenericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (!chaos_eh_match_type(CHAOS_EH_EXCEPTION_OBJ, chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
		{ CHAOS_EH_RETHROW; }
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_CustomGenericsSubjects_CustomGenericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: CustomGenericsSubjects/CustomGenerics.Box`1::.ctor(!0)
extern "C" void CustomGenericsSubjects_CustomGenerics_Box_1__ctor_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		ChaosObjectCtor(chaos_arg_0);
	}
	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	{
		auto chaos_value = _s1;
		if (chaos_is_string_id(chaos_value))
		{
			chaos_value = chaos_string_materialize(chaos_value);
		}
		auto* chaos_object = reinterpret_cast<chaos_type_CustomGenericsSubjects_CustomGenerics_Box__0_*>(_s0);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_object->field_CustomGenericsSubjects_CustomGenerics_Box__0____value));
		chaos_object->field_CustomGenericsSubjects_CustomGenerics_Box__0____value = chaos_value;
		chaos_gc_dirty_card(chaos_object);
	}
	return;
}

// Managed method: CustomGenericsSubjects/CustomGenerics.Box`1::Get()
extern "C" CHAOS_IL2CPP_INTPTR CustomGenericsSubjects_CustomGenerics_Box_1_Get(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_CustomGenericsSubjects_CustomGenerics_Box__0_*>(_s0);
		_s0 = chaos_object->field_CustomGenericsSubjects_CustomGenerics_Box__0____value;
	}
	return _s0;
}

// Managed method: CustomGenericsSubjects/CustomGenerics.Box`1::IsDefault()
extern "C" CHAOS_IL2CPP_INTPTR CustomGenericsSubjects_CustomGenerics_Box_1_IsDefault(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_CustomGenericsSubjects_CustomGenerics_Box__0_*>(_s0);
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_object->field_CustomGenericsSubjects_CustomGenerics_Box__0____value);
	}
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		const auto chaos_address = _s1;
		if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)
		{
			auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));
			*chaos_slot = 0;
		}
		else
		{
			*reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = 0;
		}
	}
	_s1 = chaos_locals[0];
	{
		const auto chaos_value = _s1;
		auto* chaos_boxed = CHAOS_IL2CPP_NEW_GC(chaos_boxed_type_CustomGenericsSubjects__0, {});
		chaos_boxed->header.type_info = chaos_mt_CustomGenericsSubjects__0.AsTypeInfoHot();
		chaos_boxed->value = chaos_value;
		_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_boxed);
	}
	{
		if (_s1 == 0)
		{
			{
				auto* chaos_source = chaos_resolve_managed_value_pointer<CHAOS_IL2CPP_INTPTR>(_s0);
				_s0 = *chaos_source;
			}
			chaos_locals[0] = _s0;
			_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
		}
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
		{
			const auto chaos_address = _s3;
			if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)
			{
				auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));
				*chaos_slot = 0;
			}
			else
			{
				*reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = 0;
			}
		}
		_s3 = chaos_locals[1];
		{
			const auto chaos_value = _s3;
			auto* chaos_boxed = CHAOS_IL2CPP_NEW_GC(chaos_boxed_type_CustomGenericsSubjects__0, {});
			chaos_boxed->header.type_info = chaos_mt_CustomGenericsSubjects__0.AsTypeInfoHot();
			chaos_boxed->value = chaos_value;
			_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_boxed);
		}
		{
			const auto chaos_arg_0 = _s3;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[8])(chaos_arg_0);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return _s3;
	}
}

// Managed method: CustomGenericsSubjects/CustomGenerics.Box`1::Set(!0)
extern "C" void CustomGenericsSubjects_CustomGenerics_Box_1_Set_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	{
		auto chaos_value = _s1;
		if (chaos_is_string_id(chaos_value))
		{
			chaos_value = chaos_string_materialize(chaos_value);
		}
		auto* chaos_object = reinterpret_cast<chaos_type_CustomGenericsSubjects_CustomGenerics_Box__0_*>(_s0);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_object->field_CustomGenericsSubjects_CustomGenerics_Box__0____value));
		chaos_object->field_CustomGenericsSubjects_CustomGenerics_Box__0____value = chaos_value;
		chaos_gc_dirty_card(chaos_object);
	}
	return;
}

// Managed method: CustomGenericsSubjects/CustomGenerics.Box<!0>::.ctor(!0)
extern "C" void CustomGenericsSubjects_CustomGenerics_Box__0__ctor_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		ChaosObjectCtor(chaos_arg_0);
	}
	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	{
		auto chaos_value = _s1;
		if (chaos_is_string_id(chaos_value))
		{
			chaos_value = chaos_string_materialize(chaos_value);
		}
		auto* chaos_object = reinterpret_cast<chaos_type_CustomGenericsSubjects_CustomGenerics_Box__0_*>(_s0);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_object->field_CustomGenericsSubjects_CustomGenerics_Box__0____value));
		chaos_object->field_CustomGenericsSubjects_CustomGenerics_Box__0____value = chaos_value;
		chaos_gc_dirty_card(chaos_object);
	}
	return;
}

// Managed method: CustomGenericsSubjects/CustomGenerics.Box<!0>::Get()
extern "C" CHAOS_IL2CPP_INTPTR CustomGenericsSubjects_CustomGenerics_Box__0_Get(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_CustomGenericsSubjects_CustomGenerics_Box__0_*>(_s0);
		_s0 = chaos_object->field_CustomGenericsSubjects_CustomGenerics_Box__0____value;
	}
	return _s0;
}

// Managed method: CustomGenericsSubjects/CustomGenerics.Box<!0>::IsDefault()
extern "C" CHAOS_IL2CPP_INTPTR CustomGenericsSubjects_CustomGenerics_Box__0_IsDefault(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_CustomGenericsSubjects_CustomGenerics_Box__0_*>(_s0);
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_object->field_CustomGenericsSubjects_CustomGenerics_Box__0____value);
	}
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		const auto chaos_address = _s1;
		if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)
		{
			auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));
			*chaos_slot = 0;
		}
		else
		{
			*reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = 0;
		}
	}
	_s1 = chaos_locals[0];
	{
		const auto chaos_value = _s1;
		auto* chaos_boxed = CHAOS_IL2CPP_NEW_GC(chaos_boxed_type_CustomGenericsSubjects__0, {});
		chaos_boxed->header.type_info = chaos_mt_CustomGenericsSubjects__0.AsTypeInfoHot();
		chaos_boxed->value = chaos_value;
		_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_boxed);
	}
	{
		if (_s1 == 0)
		{
			{
				auto* chaos_source = chaos_resolve_managed_value_pointer<CHAOS_IL2CPP_INTPTR>(_s0);
				_s0 = *chaos_source;
			}
			chaos_locals[0] = _s0;
			_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
		}
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
		{
			const auto chaos_address = _s3;
			if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)
			{
				auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));
				*chaos_slot = 0;
			}
			else
			{
				*reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = 0;
			}
		}
		_s3 = chaos_locals[1];
		{
			const auto chaos_value = _s3;
			auto* chaos_boxed = CHAOS_IL2CPP_NEW_GC(chaos_boxed_type_CustomGenericsSubjects__0, {});
			chaos_boxed->header.type_info = chaos_mt_CustomGenericsSubjects__0.AsTypeInfoHot();
			chaos_boxed->value = chaos_value;
			_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_boxed);
		}
		{
			const auto chaos_arg_1 = _s3;
			const auto chaos_arg_0 = _s2;
			const auto chaos_result = chaos_object_equals(chaos_arg_0, chaos_arg_1);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return _s2;
	}
}

// Managed method: CustomGenericsSubjects/CustomGenerics.Box<!0>::Set(!0)
extern "C" void CustomGenericsSubjects_CustomGenerics_Box__0_Set_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	{
		auto chaos_value = _s1;
		if (chaos_is_string_id(chaos_value))
		{
			chaos_value = chaos_string_materialize(chaos_value);
		}
		auto* chaos_object = reinterpret_cast<chaos_type_CustomGenericsSubjects_CustomGenerics_Box__0_*>(_s0);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_object->field_CustomGenericsSubjects_CustomGenerics_Box__0____value));
		chaos_object->field_CustomGenericsSubjects_CustomGenerics_Box__0____value = chaos_value;
		chaos_gc_dirty_card(chaos_object);
	}
	return;
}

// Generic execution authority: definition=CustomGenericsSubjects/CustomGenerics.Box`1::.ctor:System.Void(!0); type=[System.Int32]; method=[]; support=Specialized; specialization=SharedBody; body=body:definition=CustomGenericsSubjects/CustomGenerics.Box`1::.ctor:System.Void(!0);type=[System.Int32];method=[]; stub=stub:definition=CustomGenericsSubjects/CustomGenerics.Box`1::.ctor:System.Void(!0);type=[System.Int32];method=[]
// Managed method: CustomGenericsSubjects/CustomGenerics.Box<System.Int32>::.ctor(System.Int32)
extern "C" void CustomGenericsSubjects_CustomGenerics_Box_System_Int32__ctor_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		ChaosObjectCtor(chaos_arg_0);
	}
	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	{
		auto chaos_value = _s1;
		if (chaos_is_string_id(chaos_value))
		{
			chaos_value = chaos_string_materialize(chaos_value);
		}
		auto* chaos_object = reinterpret_cast<chaos_type_CustomGenericsSubjects_CustomGenerics_Box_System_Int32_*>(_s0);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_object->field_CustomGenericsSubjects_CustomGenerics_Box_System_Int32____value));
		chaos_object->field_CustomGenericsSubjects_CustomGenerics_Box_System_Int32____value = chaos_value;
		chaos_gc_dirty_card(chaos_object);
	}
	return;
}

// Generic instantiation stub: CustomGenericsSubjects/CustomGenerics.Box<System.Int32>::.ctor(System.Int32)
// Generic execution authority: definition=CustomGenericsSubjects/CustomGenerics.Box`1::.ctor:System.Void(!0); type=[System.Int32]; method=[]; support=Specialized; specialization=SharedBody; body=body:definition=CustomGenericsSubjects/CustomGenerics.Box`1::.ctor:System.Void(!0);type=[System.Int32];method=[]; stub=stub:definition=CustomGenericsSubjects/CustomGenerics.Box`1::.ctor:System.Void(!0);type=[System.Int32];method=[]
extern "C" void chaos_stub_definition_CustomGenericsSubjects_CustomGenerics_Box_1___ctor_System_Void__0__type__System_Int32__method(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CustomGenericsSubjects_CustomGenerics_Box_System_Int32__ctor_System_Int32(chaos_fn_arg_0, chaos_fn_arg_1);
}

// Generic execution authority: definition=CustomGenericsSubjects/CustomGenerics.Box`1::Get:!0(); type=[System.Int32]; method=[]; support=Specialized; specialization=SharedBody; body=body:definition=CustomGenericsSubjects/CustomGenerics.Box`1::Get:!0();type=[System.Int32];method=[]; stub=stub:definition=CustomGenericsSubjects/CustomGenerics.Box`1::Get:!0();type=[System.Int32];method=[]
// Managed method: CustomGenericsSubjects/CustomGenerics.Box<System.Int32>::Get()
extern "C" CHAOS_IL2CPP_INT32 CustomGenericsSubjects_CustomGenerics_Box_System_Int32_Get(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_CustomGenericsSubjects_CustomGenerics_Box_System_Int32_*>(_s0);
		_s0 = chaos_object->field_CustomGenericsSubjects_CustomGenerics_Box_System_Int32____value;
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Generic instantiation stub: CustomGenericsSubjects/CustomGenerics.Box<System.Int32>::Get()
// Generic execution authority: definition=CustomGenericsSubjects/CustomGenerics.Box`1::Get:!0(); type=[System.Int32]; method=[]; support=Specialized; specialization=SharedBody; body=body:definition=CustomGenericsSubjects/CustomGenerics.Box`1::Get:!0();type=[System.Int32];method=[]; stub=stub:definition=CustomGenericsSubjects/CustomGenerics.Box`1::Get:!0();type=[System.Int32];method=[]
extern "C" CHAOS_IL2CPP_INT32 chaos_stub_definition_CustomGenericsSubjects_CustomGenerics_Box_1__Get__0___type__System_Int32__method(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return CustomGenericsSubjects_CustomGenerics_Box_System_Int32_Get(chaos_fn_arg_0);
}

// Managed method: CustomGenericsSubjects/CustomGenerics.Box<System.Int32>::IsDefault()
extern "C" CHAOS_IL2CPP_INTPTR CustomGenericsSubjects_CustomGenerics_Box_System_Int32_IsDefault(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_CustomGenericsSubjects_CustomGenerics_Box_System_Int32_*>(_s0);
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_object->field_CustomGenericsSubjects_CustomGenerics_Box_System_Int32____value);
	}
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		const auto chaos_address = _s1;
		if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)
		{
			auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));
			*chaos_slot = 0;
		}
		else
		{
			*reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = 0;
		}
	}
	_s1 = chaos_locals[0];
	{
		const auto chaos_value = _s1;
		auto* chaos_boxed = CHAOS_IL2CPP_NEW_GC(chaos_boxed_type_CustomGenericsSubjects_System_Int32, {});
		chaos_boxed->header.type_info = chaos_mt_CustomGenericsSubjects_System_Int32.AsTypeInfoHot();
		chaos_boxed->value = chaos_value;
		_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_boxed);
	}
	{
		if (_s1 == 0)
		{
			{
				auto* chaos_source = chaos_resolve_managed_value_pointer<CHAOS_IL2CPP_INTPTR>(_s0);
				_s0 = *chaos_source;
			}
			chaos_locals[0] = _s0;
			_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
		}
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
		{
			const auto chaos_address = _s3;
			if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)
			{
				auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));
				*chaos_slot = 0;
			}
			else
			{
				*reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = 0;
			}
		}
		_s3 = chaos_locals[1];
		{
			const auto chaos_value = _s3;
			auto* chaos_boxed = CHAOS_IL2CPP_NEW_GC(chaos_boxed_type_CustomGenericsSubjects_System_Int32, {});
			chaos_boxed->header.type_info = chaos_mt_CustomGenericsSubjects_System_Int32.AsTypeInfoHot();
			chaos_boxed->value = chaos_value;
			_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_boxed);
		}
		{
			const auto chaos_arg_1 = _s3;
			const auto chaos_arg_0 = _s2;
			const auto chaos_result = chaos_object_equals(chaos_arg_0, chaos_arg_1);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return _s2;
	}
}

// Generic execution authority: definition=CustomGenericsSubjects/CustomGenerics.Box`1::Set:System.Void(!0); type=[System.Int32]; method=[]; support=Specialized; specialization=SharedBody; body=body:definition=CustomGenericsSubjects/CustomGenerics.Box`1::Set:System.Void(!0);type=[System.Int32];method=[]; stub=stub:definition=CustomGenericsSubjects/CustomGenerics.Box`1::Set:System.Void(!0);type=[System.Int32];method=[]
// Managed method: CustomGenericsSubjects/CustomGenerics.Box<System.Int32>::Set(System.Int32)
extern "C" void CustomGenericsSubjects_CustomGenerics_Box_System_Int32_Set_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	{
		auto chaos_value = _s1;
		if (chaos_is_string_id(chaos_value))
		{
			chaos_value = chaos_string_materialize(chaos_value);
		}
		auto* chaos_object = reinterpret_cast<chaos_type_CustomGenericsSubjects_CustomGenerics_Box_System_Int32_*>(_s0);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_object->field_CustomGenericsSubjects_CustomGenerics_Box_System_Int32____value));
		chaos_object->field_CustomGenericsSubjects_CustomGenerics_Box_System_Int32____value = chaos_value;
		chaos_gc_dirty_card(chaos_object);
	}
	return;
}

// Generic instantiation stub: CustomGenericsSubjects/CustomGenerics.Box<System.Int32>::Set(System.Int32)
// Generic execution authority: definition=CustomGenericsSubjects/CustomGenerics.Box`1::Set:System.Void(!0); type=[System.Int32]; method=[]; support=Specialized; specialization=SharedBody; body=body:definition=CustomGenericsSubjects/CustomGenerics.Box`1::Set:System.Void(!0);type=[System.Int32];method=[]; stub=stub:definition=CustomGenericsSubjects/CustomGenerics.Box`1::Set:System.Void(!0);type=[System.Int32];method=[]
extern "C" void chaos_stub_definition_CustomGenericsSubjects_CustomGenerics_Box_1__Set_System_Void__0__type__System_Int32__method(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CustomGenericsSubjects_CustomGenerics_Box_System_Int32_Set_System_Int32(chaos_fn_arg_0, chaos_fn_arg_1);
}

// Managed method: CustomGenericsSubjects/CustomGenerics.Pair`2::.ctor(!0,!1)
extern "C" void CustomGenericsSubjects_CustomGenerics_Pair_2__ctor_0_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);
	chaos_args[2] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_2);

	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	{
		auto chaos_value = _s1;
		if (chaos_is_string_id(chaos_value))
		{
			chaos_value = chaos_string_materialize(chaos_value);
		}
		auto* chaos_value_owner = chaos_resolve_managed_value_pointer<chaos_valuetype_CustomGenericsSubjects_CustomGenerics_Pair__0__1_>(_s0);
		chaos_value_owner->field_CustomGenericsSubjects_CustomGenerics_Pair__0__1___First = chaos_value;
		if (chaos_is_gc_pointer(chaos_value_owner))
		{
			chaos_gc_dirty_card(chaos_value_owner);
		}
	}
	_s0 = chaos_args[0];
	_s1 = chaos_args[2];
	{
		auto chaos_value = _s1;
		if (chaos_is_string_id(chaos_value))
		{
			chaos_value = chaos_string_materialize(chaos_value);
		}
		auto* chaos_value_owner = chaos_resolve_managed_value_pointer<chaos_valuetype_CustomGenericsSubjects_CustomGenerics_Pair__0__1_>(_s0);
		chaos_value_owner->field_CustomGenericsSubjects_CustomGenerics_Pair__0__1___Second = chaos_value;
		if (chaos_is_gc_pointer(chaos_value_owner))
		{
			chaos_gc_dirty_card(chaos_value_owner);
		}
	}
	return;
}

// Managed method: CustomGenericsSubjects/CustomGenerics.Pair`2::Deconstruct(!0&,!1&)
extern "C" void CustomGenericsSubjects_CustomGenerics_Pair_2_Deconstruct_0_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);
	chaos_args[2] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_2);

	_s0 = chaos_args[1];
	_s1 = chaos_args[0];
	{
		auto* chaos_value = chaos_resolve_managed_value_pointer<chaos_valuetype_CustomGenericsSubjects_CustomGenerics_Pair__0__1_>(_s1);
		_s1 = chaos_value->field_CustomGenericsSubjects_CustomGenerics_Pair__0__1___First;
	}
	{
		const auto chaos_value = _s1;
		auto* chaos_destination = chaos_resolve_managed_value_pointer<CHAOS_IL2CPP_INTPTR>(_s0);
		*chaos_destination = chaos_value;
	}
	_s0 = chaos_args[2];
	_s1 = chaos_args[0];
	{
		auto* chaos_value = chaos_resolve_managed_value_pointer<chaos_valuetype_CustomGenericsSubjects_CustomGenerics_Pair__0__1_>(_s1);
		_s1 = chaos_value->field_CustomGenericsSubjects_CustomGenerics_Pair__0__1___Second;
	}
	{
		const auto chaos_value = _s1;
		auto* chaos_destination = chaos_resolve_managed_value_pointer<CHAOS_IL2CPP_INTPTR>(_s0);
		*chaos_destination = chaos_value;
	}
	return;
}

// Managed method: CustomGenericsSubjects/CustomGenerics.Pair<!0,!1>::.ctor(!0,!1)
extern "C" void CustomGenericsSubjects_CustomGenerics_Pair__0__1__ctor_0_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);
	chaos_args[2] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_2);

	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	{
		auto chaos_value = _s1;
		if (chaos_is_string_id(chaos_value))
		{
			chaos_value = chaos_string_materialize(chaos_value);
		}
		auto* chaos_value_owner = chaos_resolve_managed_value_pointer<chaos_valuetype_CustomGenericsSubjects_CustomGenerics_Pair__0__1_>(_s0);
		chaos_value_owner->field_CustomGenericsSubjects_CustomGenerics_Pair__0__1___First = chaos_value;
		if (chaos_is_gc_pointer(chaos_value_owner))
		{
			chaos_gc_dirty_card(chaos_value_owner);
		}
	}
	_s0 = chaos_args[0];
	_s1 = chaos_args[2];
	{
		auto chaos_value = _s1;
		if (chaos_is_string_id(chaos_value))
		{
			chaos_value = chaos_string_materialize(chaos_value);
		}
		auto* chaos_value_owner = chaos_resolve_managed_value_pointer<chaos_valuetype_CustomGenericsSubjects_CustomGenerics_Pair__0__1_>(_s0);
		chaos_value_owner->field_CustomGenericsSubjects_CustomGenerics_Pair__0__1___Second = chaos_value;
		if (chaos_is_gc_pointer(chaos_value_owner))
		{
			chaos_gc_dirty_card(chaos_value_owner);
		}
	}
	return;
}

// Managed method: CustomGenericsSubjects/CustomGenerics.Pair<!0,!1>::Deconstruct(!0&,!1&)
extern "C" void CustomGenericsSubjects_CustomGenerics_Pair__0__1_Deconstruct_0_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);
	chaos_args[2] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_2);

	_s0 = chaos_args[1];
	_s1 = chaos_args[0];
	{
		auto* chaos_value = chaos_resolve_managed_value_pointer<chaos_valuetype_CustomGenericsSubjects_CustomGenerics_Pair__0__1_>(_s1);
		_s1 = chaos_value->field_CustomGenericsSubjects_CustomGenerics_Pair__0__1___First;
	}
	{
		const auto chaos_value = _s1;
		auto* chaos_destination = chaos_resolve_managed_value_pointer<CHAOS_IL2CPP_INTPTR>(_s0);
		*chaos_destination = chaos_value;
	}
	_s0 = chaos_args[2];
	_s1 = chaos_args[0];
	{
		auto* chaos_value = chaos_resolve_managed_value_pointer<chaos_valuetype_CustomGenericsSubjects_CustomGenerics_Pair__0__1_>(_s1);
		_s1 = chaos_value->field_CustomGenericsSubjects_CustomGenerics_Pair__0__1___Second;
	}
	{
		const auto chaos_value = _s1;
		auto* chaos_destination = chaos_resolve_managed_value_pointer<CHAOS_IL2CPP_INTPTR>(_s0);
		*chaos_destination = chaos_value;
	}
	return;
}

// Generic execution authority: definition=CustomGenericsSubjects/CustomGenerics.Pair`2::.ctor:System.Void(!0,!1); type=[System.Int32, System.String]; method=[]; support=Specialized; specialization=SharedBody; body=body:definition=CustomGenericsSubjects/CustomGenerics.Pair`2::.ctor:System.Void(!0,!1);type=[System.Int32,System.String];method=[]; stub=stub:definition=CustomGenericsSubjects/CustomGenerics.Pair`2::.ctor:System.Void(!0,!1);type=[System.Int32,System.String];method=[]
// Managed method: CustomGenericsSubjects/CustomGenerics.Pair<System.Int32,System.String>::.ctor(System.Int32,System.String)
extern "C" void CustomGenericsSubjects_CustomGenerics_Pair_System_Int32_System_String__ctor_System_Int32_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);
	chaos_args[2] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_2);

	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	{
		auto chaos_value = _s1;
		if (chaos_is_string_id(chaos_value))
		{
			chaos_value = chaos_string_materialize(chaos_value);
		}
		auto* chaos_value_owner = chaos_resolve_managed_value_pointer<chaos_valuetype_CustomGenericsSubjects_CustomGenerics_Pair_System_Int32_System_String_>(_s0);
		chaos_value_owner->field_CustomGenericsSubjects_CustomGenerics_Pair_System_Int32_System_String___First = chaos_value;
		if (chaos_is_gc_pointer(chaos_value_owner))
		{
			chaos_gc_dirty_card(chaos_value_owner);
		}
	}
	_s0 = chaos_args[0];
	_s1 = chaos_args[2];
	{
		auto chaos_value = _s1;
		if (chaos_is_string_id(chaos_value))
		{
			chaos_value = chaos_string_materialize(chaos_value);
		}
		auto* chaos_value_owner = chaos_resolve_managed_value_pointer<chaos_valuetype_CustomGenericsSubjects_CustomGenerics_Pair_System_Int32_System_String_>(_s0);
		chaos_value_owner->field_CustomGenericsSubjects_CustomGenerics_Pair_System_Int32_System_String___Second = chaos_value;
		if (chaos_is_gc_pointer(chaos_value_owner))
		{
			chaos_gc_dirty_card(chaos_value_owner);
		}
	}
	return;
}

// Generic instantiation stub: CustomGenericsSubjects/CustomGenerics.Pair<System.Int32,System.String>::.ctor(System.Int32,System.String)
// Generic execution authority: definition=CustomGenericsSubjects/CustomGenerics.Pair`2::.ctor:System.Void(!0,!1); type=[System.Int32, System.String]; method=[]; support=Specialized; specialization=SharedBody; body=body:definition=CustomGenericsSubjects/CustomGenerics.Pair`2::.ctor:System.Void(!0,!1);type=[System.Int32,System.String];method=[]; stub=stub:definition=CustomGenericsSubjects/CustomGenerics.Pair`2::.ctor:System.Void(!0,!1);type=[System.Int32,System.String];method=[]
extern "C" void chaos_stub_definition_CustomGenericsSubjects_CustomGenerics_Pair_2___ctor_System_Void__0__1__type__System_Int32_System_String__method(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2)
{
	CustomGenericsSubjects_CustomGenerics_Pair_System_Int32_System_String__ctor_System_Int32_System_String(chaos_fn_arg_0, chaos_fn_arg_1, chaos_fn_arg_2);
}

// Generic execution authority: definition=CustomGenericsSubjects/CustomGenerics.Pair`2::Deconstruct:System.Void(!0&,!1&); type=[System.Int32, System.String]; method=[]; support=Specialized; specialization=SharedBody; body=body:definition=CustomGenericsSubjects/CustomGenerics.Pair`2::Deconstruct:System.Void(!0&,!1&);type=[System.Int32,System.String];method=[]; stub=stub:definition=CustomGenericsSubjects/CustomGenerics.Pair`2::Deconstruct:System.Void(!0&,!1&);type=[System.Int32,System.String];method=[]
// Managed method: CustomGenericsSubjects/CustomGenerics.Pair<System.Int32,System.String>::Deconstruct(System.Int32&,System.String&)
extern "C" void CustomGenericsSubjects_CustomGenerics_Pair_System_Int32_System_String_Deconstruct_System_Int32_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);
	chaos_args[2] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_2);

	_s0 = chaos_args[1];
	_s1 = chaos_args[0];
	{
		auto* chaos_value = chaos_resolve_managed_value_pointer<chaos_valuetype_CustomGenericsSubjects_CustomGenerics_Pair_System_Int32_System_String_>(_s1);
		_s1 = chaos_value->field_CustomGenericsSubjects_CustomGenerics_Pair_System_Int32_System_String___First;
	}
	{
		const auto chaos_value = _s1;
		auto* chaos_destination = chaos_resolve_managed_value_pointer<CHAOS_IL2CPP_INTPTR>(_s0);
		*chaos_destination = chaos_value;
	}
	_s0 = chaos_args[2];
	_s1 = chaos_args[0];
	{
		auto* chaos_value = chaos_resolve_managed_value_pointer<chaos_valuetype_CustomGenericsSubjects_CustomGenerics_Pair_System_Int32_System_String_>(_s1);
		_s1 = chaos_value->field_CustomGenericsSubjects_CustomGenerics_Pair_System_Int32_System_String___Second;
	}
	{
		const auto chaos_value = _s1;
		auto* chaos_destination = chaos_resolve_managed_value_pointer<CHAOS_IL2CPP_INTPTR>(_s0);
		*chaos_destination = chaos_value;
	}
	return;
}

// Generic instantiation stub: CustomGenericsSubjects/CustomGenerics.Pair<System.Int32,System.String>::Deconstruct(System.Int32&,System.String&)
// Generic execution authority: definition=CustomGenericsSubjects/CustomGenerics.Pair`2::Deconstruct:System.Void(!0&,!1&); type=[System.Int32, System.String]; method=[]; support=Specialized; specialization=SharedBody; body=body:definition=CustomGenericsSubjects/CustomGenerics.Pair`2::Deconstruct:System.Void(!0&,!1&);type=[System.Int32,System.String];method=[]; stub=stub:definition=CustomGenericsSubjects/CustomGenerics.Pair`2::Deconstruct:System.Void(!0&,!1&);type=[System.Int32,System.String];method=[]
extern "C" void chaos_stub_definition_CustomGenericsSubjects_CustomGenerics_Pair_2__Deconstruct_System_Void__0___1___type__System_Int32_System_String__method(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2)
{
	CustomGenericsSubjects_CustomGenerics_Pair_System_Int32_System_String_Deconstruct_System_Int32_System_String(chaos_fn_arg_0, chaos_fn_arg_1, chaos_fn_arg_2);
}

// Managed method: CustomGenericsSubjects/CustomGenerics.Util::.ctor()
extern "C" void CustomGenericsSubjects_CustomGenerics_Util__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CustomGenericsSubjects/CustomGenerics.Util::Echo`1(!!0)
extern "C" CHAOS_IL2CPP_INTPTR CustomGenericsSubjects_CustomGenerics_Util_Echo_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	return _s0;
}

// Generic execution authority: definition=CustomGenericsSubjects/CustomGenerics.Util::Echo`1:!!0(!!0); type=[]; method=[System.Int32]; support=Specialized; specialization=SharedBody; body=body:definition=CustomGenericsSubjects/CustomGenerics.Util::Echo`1:!!0(!!0);type=[];method=[System.Int32]; stub=stub:definition=CustomGenericsSubjects/CustomGenerics.Util::Echo`1:!!0(!!0);type=[];method=[System.Int32]
// Managed method: CustomGenericsSubjects/CustomGenerics.Util::Echo<System.Int32>(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 CustomGenericsSubjects_CustomGenerics_Util_Echo_System_Int32_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Generic instantiation stub: CustomGenericsSubjects/CustomGenerics.Util::Echo<System.Int32>(System.Int32)
// Generic execution authority: definition=CustomGenericsSubjects/CustomGenerics.Util::Echo`1:!!0(!!0); type=[]; method=[System.Int32]; support=Specialized; specialization=SharedBody; body=body:definition=CustomGenericsSubjects/CustomGenerics.Util::Echo`1:!!0(!!0);type=[];method=[System.Int32]; stub=stub:definition=CustomGenericsSubjects/CustomGenerics.Util::Echo`1:!!0(!!0);type=[];method=[System.Int32]
extern "C" CHAOS_IL2CPP_INT32 chaos_stub_definition_CustomGenericsSubjects_CustomGenerics_Util__Echo_1___0___0__type____method__System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
{
	return CustomGenericsSubjects_CustomGenerics_Util_Echo_System_Int32_System_Int32(chaos_fn_arg_0);
}

// Managed method: CustomGenericsSubjects/CustomGenerics.Util::Equal`1(!!0,!!0)
extern "C" CHAOS_IL2CPP_INTPTR CustomGenericsSubjects_CustomGenerics_Util_Equal_0_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_args[0]);
	_s1 = chaos_args[1];
	{
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[18])(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return _s1;
}

// Generic execution authority: definition=CustomGenericsSubjects/CustomGenerics.Util::Equal`1:System.Boolean(!!0,!!0); type=[]; method=[System.Int32]; support=Specialized; specialization=SharedBody; body=body:definition=CustomGenericsSubjects/CustomGenerics.Util::Equal`1:System.Boolean(!!0,!!0);type=[];method=[System.Int32]; stub=stub:definition=CustomGenericsSubjects/CustomGenerics.Util::Equal`1:System.Boolean(!!0,!!0);type=[];method=[System.Int32]
// Managed method: CustomGenericsSubjects/CustomGenerics.Util::Equal<System.Int32>(System.Int32,System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR CustomGenericsSubjects_CustomGenerics_Util_Equal_System_Int32_System_Int32_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_args[0]);
	_s1 = chaos_args[1];
	{
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[19])(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return _s1;
}

// Generic instantiation stub: CustomGenericsSubjects/CustomGenerics.Util::Equal<System.Int32>(System.Int32,System.Int32)
// Generic execution authority: definition=CustomGenericsSubjects/CustomGenerics.Util::Equal`1:System.Boolean(!!0,!!0); type=[]; method=[System.Int32]; support=Specialized; specialization=SharedBody; body=body:definition=CustomGenericsSubjects/CustomGenerics.Util::Equal`1:System.Boolean(!!0,!!0);type=[];method=[System.Int32]; stub=stub:definition=CustomGenericsSubjects/CustomGenerics.Util::Equal`1:System.Boolean(!!0,!!0);type=[];method=[System.Int32]
extern "C" CHAOS_IL2CPP_INTPTR chaos_stub_definition_CustomGenericsSubjects_CustomGenerics_Util__Equal_1_System_Boolean___0___0__type____method__System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	return CustomGenericsSubjects_CustomGenerics_Util_Equal_System_Int32_System_Int32_System_Int32(chaos_fn_arg_0, chaos_fn_arg_1);
}

// Managed method: CustomGenericsSubjects/CustomGenerics.Wrapper`1::.ctor(!0)
extern "C" void CustomGenericsSubjects_CustomGenerics_Wrapper_1__ctor_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		ChaosObjectCtor(chaos_arg_0);
	}
	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	{
		auto chaos_value = _s1;
		if (chaos_is_string_id(chaos_value))
		{
			chaos_value = chaos_string_materialize(chaos_value);
		}
		auto* chaos_object = reinterpret_cast<chaos_type_CustomGenericsSubjects_CustomGenerics_Wrapper__0_*>(_s0);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_object->field_CustomGenericsSubjects_CustomGenerics_Wrapper__0___Ref));
		chaos_object->field_CustomGenericsSubjects_CustomGenerics_Wrapper__0___Ref = chaos_value;
		chaos_gc_dirty_card(chaos_object);
	}
	return;
}

// Managed method: CustomGenericsSubjects/CustomGenerics.Wrapper`1::GetRefType()
extern "C" CHAOS_IL2CPP_INTPTR CustomGenericsSubjects_CustomGenerics_Wrapper_1_GetRefType(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_CustomGenericsSubjects_CustomGenerics_Wrapper__0_*>(_s0);
		_s0 = chaos_object->field_CustomGenericsSubjects_CustomGenerics_Wrapper__0___Ref;
	}
	{
		const auto chaos_value = _s0;
		auto* chaos_boxed = CHAOS_IL2CPP_NEW_GC(chaos_boxed_type_CustomGenericsSubjects__0, {});
		chaos_boxed->header.type_info = chaos_mt_CustomGenericsSubjects__0.AsTypeInfoHot();
		chaos_boxed->value = chaos_value;
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_boxed);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[21])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return _s1;
}

// Managed method: CustomGenericsSubjects/CustomGenerics.Wrapper<!0>::.ctor(!0)
extern "C" void CustomGenericsSubjects_CustomGenerics_Wrapper__0__ctor_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		ChaosObjectCtor(chaos_arg_0);
	}
	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	{
		auto chaos_value = _s1;
		if (chaos_is_string_id(chaos_value))
		{
			chaos_value = chaos_string_materialize(chaos_value);
		}
		auto* chaos_object = reinterpret_cast<chaos_type_CustomGenericsSubjects_CustomGenerics_Wrapper__0_*>(_s0);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_object->field_CustomGenericsSubjects_CustomGenerics_Wrapper__0___Ref));
		chaos_object->field_CustomGenericsSubjects_CustomGenerics_Wrapper__0___Ref = chaos_value;
		chaos_gc_dirty_card(chaos_object);
	}
	return;
}

// Managed method: CustomGenericsSubjects/CustomGenerics.Wrapper<!0>::GetRefType()
extern "C" CHAOS_IL2CPP_INTPTR CustomGenericsSubjects_CustomGenerics_Wrapper__0_GetRefType(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_CustomGenericsSubjects_CustomGenerics_Wrapper__0_*>(_s0);
		_s0 = chaos_object->field_CustomGenericsSubjects_CustomGenerics_Wrapper__0___Ref;
	}
	{
		const auto chaos_value = _s0;
		auto* chaos_boxed = CHAOS_IL2CPP_NEW_GC(chaos_boxed_type_CustomGenericsSubjects__0, {});
		chaos_boxed->header.type_info = chaos_mt_CustomGenericsSubjects__0.AsTypeInfoHot();
		chaos_boxed->value = chaos_value;
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_boxed);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[21])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return _s1;
}

// Generic execution authority: definition=CustomGenericsSubjects/CustomGenerics.Wrapper`1::.ctor:System.Void(!0); type=[System.String]; method=[]; support=Specialized; specialization=SharedBody; body=body:definition=CustomGenericsSubjects/CustomGenerics.Wrapper`1::.ctor:System.Void(!0);type=[System.String];method=[]; stub=stub:definition=CustomGenericsSubjects/CustomGenerics.Wrapper`1::.ctor:System.Void(!0);type=[System.String];method=[]
// Managed method: CustomGenericsSubjects/CustomGenerics.Wrapper<System.String>::.ctor(System.String)
extern "C" void CustomGenericsSubjects_CustomGenerics_Wrapper_System_String__ctor_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		ChaosObjectCtor(chaos_arg_0);
	}
	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	{
		auto chaos_value = _s1;
		if (chaos_is_string_id(chaos_value))
		{
			chaos_value = chaos_string_materialize(chaos_value);
		}
		auto* chaos_object = reinterpret_cast<chaos_type_CustomGenericsSubjects_CustomGenerics_Wrapper_System_String_*>(_s0);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_object->field_CustomGenericsSubjects_CustomGenerics_Wrapper_System_String___Ref));
		chaos_object->field_CustomGenericsSubjects_CustomGenerics_Wrapper_System_String___Ref = chaos_value;
		chaos_gc_dirty_card(chaos_object);
	}
	return;
}

// Generic instantiation stub: CustomGenericsSubjects/CustomGenerics.Wrapper<System.String>::.ctor(System.String)
// Generic execution authority: definition=CustomGenericsSubjects/CustomGenerics.Wrapper`1::.ctor:System.Void(!0); type=[System.String]; method=[]; support=Specialized; specialization=SharedBody; body=body:definition=CustomGenericsSubjects/CustomGenerics.Wrapper`1::.ctor:System.Void(!0);type=[System.String];method=[]; stub=stub:definition=CustomGenericsSubjects/CustomGenerics.Wrapper`1::.ctor:System.Void(!0);type=[System.String];method=[]
extern "C" void chaos_stub_definition_CustomGenericsSubjects_CustomGenerics_Wrapper_1___ctor_System_Void__0__type__System_String__method(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	CustomGenericsSubjects_CustomGenerics_Wrapper_System_String__ctor_System_String(chaos_fn_arg_0, chaos_fn_arg_1);
}

// Generic execution authority: definition=CustomGenericsSubjects/CustomGenerics.Wrapper`1::GetRefType:System.Type(); type=[System.String]; method=[]; support=Specialized; specialization=SharedBody; body=body:definition=CustomGenericsSubjects/CustomGenerics.Wrapper`1::GetRefType:System.Type();type=[System.String];method=[]; stub=stub:definition=CustomGenericsSubjects/CustomGenerics.Wrapper`1::GetRefType:System.Type();type=[System.String];method=[]
// Managed method: CustomGenericsSubjects/CustomGenerics.Wrapper<System.String>::GetRefType()
extern "C" CHAOS_IL2CPP_INTPTR CustomGenericsSubjects_CustomGenerics_Wrapper_System_String_GetRefType(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_CustomGenericsSubjects_CustomGenerics_Wrapper_System_String_*>(_s0);
		_s0 = chaos_object->field_CustomGenericsSubjects_CustomGenerics_Wrapper_System_String___Ref;
	}
	{
		const auto chaos_value = _s0;
		auto* chaos_boxed = CHAOS_IL2CPP_NEW_GC(chaos_boxed_type_CustomGenericsSubjects_System_String, {});
		chaos_boxed->header.type_info = chaos_mt_CustomGenericsSubjects_System_String.AsTypeInfoHot();
		chaos_boxed->value = chaos_value;
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_boxed);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[21])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return _s1;
}

// Generic instantiation stub: CustomGenericsSubjects/CustomGenerics.Wrapper<System.String>::GetRefType()
// Generic execution authority: definition=CustomGenericsSubjects/CustomGenerics.Wrapper`1::GetRefType:System.Type(); type=[System.String]; method=[]; support=Specialized; specialization=SharedBody; body=body:definition=CustomGenericsSubjects/CustomGenerics.Wrapper`1::GetRefType:System.Type();type=[System.String];method=[]; stub=stub:definition=CustomGenericsSubjects/CustomGenerics.Wrapper`1::GetRefType:System.Type();type=[System.String];method=[]
extern "C" CHAOS_IL2CPP_INTPTR chaos_stub_definition_CustomGenericsSubjects_CustomGenerics_Wrapper_1__GetRefType_System_Type___type__System_String__method(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return CustomGenericsSubjects_CustomGenerics_Wrapper_System_String_GetRefType(chaos_fn_arg_0);
}



}  // namespace chaos::il2cpp::codegen::CustomGenericsSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 35;

extern "C" void ChaosJitRegisterAll() {}