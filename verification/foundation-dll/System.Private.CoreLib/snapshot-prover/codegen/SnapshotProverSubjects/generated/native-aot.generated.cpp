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

namespace chaos::il2cpp::codegen::SnapshotProverSubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_SnapshotTestFixtures_IMarker = static_cast<CHAOS_IL2CPP_INTPTR>(13604930402668384013ULL);
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_SnapshotTestFixtures_IMyInterface = static_cast<CHAOS_IL2CPP_INTPTR>(3080637198406087848ULL);
const void* chaos_vtable_SnapshotTestFixtures_BaseClass[];
const void* chaos_vtable_SnapshotTestFixtures_DerivedClass[];
const void* chaos_vtable_SnapshotTestFixtures_HasFields[];
const void* chaos_vtable_SnapshotTestFixtures_HasInstanceFields[];
const void* chaos_vtable_SnapshotTestFixtures_ImplHelper[];
const void* chaos_vtable_SnapshotTestFixtures_InstanceHelper[];
const void* chaos_vtable_SnapshotTestFixtures_MarkedClass[];
const void* chaos_vtable_SnapshotTestFixtures_MyClass[];
const void* chaos_vtable_SnapshotTestFixtures_SealedHelper[];
const void* chaos_vtable_SnapshotTestFixtures_Wrapper__0_[];
const void* chaos_vtable_SnapshotTestFixtures_Wrapper_System_Int32_[];
inline TypeInfoV0 chaos_mt_SnapshotProverSubjects_SnapshotProverSubjects = {{nullptr, nullptr, 17700440604878766952ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_SnapshotProverSubjects_SnapshotProverSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(17700440604878766952ULL);inline TypeInfoV0 chaos_mt_SnapshotTestFixtures_BaseClass = {{nullptr, chaos_vtable_SnapshotTestFixtures_BaseClass, 16934624807659080947ULL, 2u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_SnapshotTestFixtures_BaseClass = static_cast<CHAOS_IL2CPP_INTPTR>(16934624807659080947ULL);inline TypeInfoV0 chaos_mt_SnapshotTestFixtures_DerivedClass = {{&chaos_mt_SnapshotTestFixtures_BaseClass.hot, chaos_vtable_SnapshotTestFixtures_DerivedClass, 5213792135327273735ULL, 2u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_SnapshotTestFixtures_DerivedClass = static_cast<CHAOS_IL2CPP_INTPTR>(5213792135327273735ULL);inline TypeInfoV0 chaos_mt_SnapshotTestFixtures_FieldHelper = {{nullptr, nullptr, 14105055411266533488ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_SnapshotTestFixtures_FieldHelper = static_cast<CHAOS_IL2CPP_INTPTR>(14105055411266533488ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Object = {{nullptr, nullptr, 15228727185366376748ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_INTPTR>(15228727185366376748ULL);inline TypeInfoV0 chaos_mt_SnapshotTestFixtures_HasFields = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_SnapshotTestFixtures_HasFields, 6075661983971539625ULL, 1u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_SnapshotTestFixtures_HasFields = static_cast<CHAOS_IL2CPP_INTPTR>(6075661983971539625ULL);inline TypeInfoV0 chaos_mt_SnapshotTestFixtures_HasInstanceFields = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_SnapshotTestFixtures_HasInstanceFields, 17812473439196761566ULL, 3u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_SnapshotTestFixtures_HasInstanceFields = static_cast<CHAOS_IL2CPP_INTPTR>(17812473439196761566ULL);static constexpr InterfaceMapEntry chaos_iface_map_SnapshotTestFixtures_ImplHelper[] = {
	{ chaos_type_id_SnapshotTestFixtures_IMyInterface, 3, 1 }
};
inline TypeInfoV0 chaos_mt_SnapshotTestFixtures_ImplHelper = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_SnapshotTestFixtures_ImplHelper, 2682382514493266270ULL, 4u, 32, 1, 1}, {chaos_iface_map_SnapshotTestFixtures_ImplHelper, nullptr, 1, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_SnapshotTestFixtures_ImplHelper = static_cast<CHAOS_IL2CPP_INTPTR>(2682382514493266270ULL);inline TypeInfoV0 chaos_mt_SnapshotTestFixtures_InstanceHelper = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_SnapshotTestFixtures_InstanceHelper, 7539435265304875683ULL, 4u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_SnapshotTestFixtures_InstanceHelper = static_cast<CHAOS_IL2CPP_INTPTR>(7539435265304875683ULL);inline TypeInfoV0 chaos_mt_SnapshotTestFixtures_LdftnHelper___O = {{nullptr, nullptr, 9616880548282383388ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_SnapshotTestFixtures_LdftnHelper___O = static_cast<CHAOS_IL2CPP_INTPTR>(9616880548282383388ULL);static constexpr InterfaceMapEntry chaos_iface_map_SnapshotTestFixtures_MarkedClass[] = {
	{ chaos_type_id_SnapshotTestFixtures_IMarker, 0, 0 }
};
inline TypeInfoV0 chaos_mt_SnapshotTestFixtures_MarkedClass = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_SnapshotTestFixtures_MarkedClass, 6112983511997078372ULL, 1u, 32, 1, 1}, {chaos_iface_map_SnapshotTestFixtures_MarkedClass, nullptr, 1, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_SnapshotTestFixtures_MarkedClass = static_cast<CHAOS_IL2CPP_INTPTR>(6112983511997078372ULL);inline TypeInfoV0 chaos_mt_SnapshotTestFixtures_MyClass = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_SnapshotTestFixtures_MyClass, 17017755788800598734ULL, 4u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_SnapshotTestFixtures_MyClass = static_cast<CHAOS_IL2CPP_INTPTR>(17017755788800598734ULL);inline TypeInfoV0 chaos_mt_SnapshotTestFixtures_SealedHelper = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_SnapshotTestFixtures_SealedHelper, 8545221821474343780ULL, 5u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_SnapshotTestFixtures_SealedHelper = static_cast<CHAOS_IL2CPP_INTPTR>(8545221821474343780ULL);inline TypeInfoV0 chaos_mt_SnapshotTestFixtures_StaticFieldWriteHelper = {{nullptr, nullptr, 7187250757378180401ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_SnapshotTestFixtures_StaticFieldWriteHelper = static_cast<CHAOS_IL2CPP_INTPTR>(7187250757378180401ULL);inline TypeInfoV0 chaos_mt_SnapshotTestFixtures_Wrapper__0_ = {{nullptr, chaos_vtable_SnapshotTestFixtures_Wrapper__0_, 15764982902837746946ULL, 7u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_SnapshotTestFixtures_Wrapper__0_ = static_cast<CHAOS_IL2CPP_INTPTR>(15764982902837746946ULL);inline TypeInfoV0 chaos_mt_SnapshotTestFixtures_Wrapper_System_Int32_ = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_SnapshotTestFixtures_Wrapper_System_Int32_, 1845517801504066080ULL, 4u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_SnapshotTestFixtures_Wrapper_System_Int32_ = static_cast<CHAOS_IL2CPP_INTPTR>(1845517801504066080ULL);inline TypeInfoV0 chaos_mt_System_Collections_System_Collections_Generic_Dictionary_System_Int32_System_Int32_ = {{nullptr, nullptr, 9130424216747936531ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Collections_System_Collections_Generic_Dictionary_System_Int32_System_Int32_ = static_cast<CHAOS_IL2CPP_INTPTR>(9130424216747936531ULL);inline TypeInfoV0 chaos_mt_System_Collections_System_Collections_Generic_HashSet_System_Int32_ = {{nullptr, nullptr, 11554060213542038524ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Collections_System_Collections_Generic_HashSet_System_Int32_ = static_cast<CHAOS_IL2CPP_INTPTR>(11554060213542038524ULL);inline TypeInfoV0 chaos_mt_System_Collections_System_Collections_Generic_List_System_Int32_ = {{nullptr, nullptr, 8124737553590710378ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Collections_System_Collections_Generic_List_System_Int32_ = static_cast<CHAOS_IL2CPP_INTPTR>(8124737553590710378ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Byte = {{nullptr, nullptr, 3217969471912086765ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Byte = static_cast<CHAOS_IL2CPP_INTPTR>(3217969471912086765ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Delegate = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, nullptr, 7451128447593600616ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Delegate = static_cast<CHAOS_IL2CPP_INTPTR>(7451128447593600616ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Double = {{nullptr, nullptr, 1099026126681926114ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Double = static_cast<CHAOS_IL2CPP_INTPTR>(1099026126681926114ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_MulticastDelegate = {{&chaos_mt_System_Private_CoreLib_System_Delegate.hot, nullptr, 6681393039041505440ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_MulticastDelegate = static_cast<CHAOS_IL2CPP_INTPTR>(6681393039041505440ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Int32_ = {{&chaos_mt_System_Private_CoreLib_System_MulticastDelegate.hot, nullptr, 143744549824129343ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Func_System_Int32_System_Int32_ = static_cast<CHAOS_IL2CPP_INTPTR>(143744549824129343ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Func_System_Int32_ = {{&chaos_mt_System_Private_CoreLib_System_MulticastDelegate.hot, nullptr, 8866194404114377402ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Func_System_Int32_ = static_cast<CHAOS_IL2CPP_INTPTR>(8866194404114377402ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Int16 = {{nullptr, nullptr, 11007710000311077999ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Int16 = static_cast<CHAOS_IL2CPP_INTPTR>(11007710000311077999ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Int64 = {{nullptr, nullptr, 11012495074916106146ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Int64 = static_cast<CHAOS_IL2CPP_INTPTR>(11012495074916106146ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_OverflowException = {{nullptr, nullptr, 531363262308024406ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_OverflowException = static_cast<CHAOS_IL2CPP_INTPTR>(531363262308024406ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_Assembly = {{nullptr, nullptr, 5474029880995115448ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_Assembly = static_cast<CHAOS_IL2CPP_INTPTR>(5474029880995115448ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName = {{nullptr, nullptr, 17082367815459723707ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_AssemblyName = static_cast<CHAOS_IL2CPP_INTPTR>(17082367815459723707ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo = {{nullptr, nullptr, 4137207361503509124ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_ConstructorInfo = static_cast<CHAOS_IL2CPP_INTPTR>(4137207361503509124ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo = {{nullptr, nullptr, 17040031516751226236ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_FieldInfo = static_cast<CHAOS_IL2CPP_INTPTR>(17040031516751226236ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo = {{nullptr, nullptr, 10748947813473285525ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_MethodInfo = static_cast<CHAOS_IL2CPP_INTPTR>(10748947813473285525ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_SByte = {{nullptr, nullptr, 13054279835592181566ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_SByte = static_cast<CHAOS_IL2CPP_INTPTR>(13054279835592181566ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Single = {{nullptr, nullptr, 4813876735449938439ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Single = static_cast<CHAOS_IL2CPP_INTPTR>(4813876735449938439ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_String = {{nullptr, nullptr, 1782325859292956794ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_String = static_cast<CHAOS_IL2CPP_INTPTR>(1782325859292956794ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Type = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, nullptr, 3222174513575444759ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Type = static_cast<CHAOS_IL2CPP_INTPTR>(3222174513575444759ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_UInt16 = {{nullptr, nullptr, 4060647147365029652ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_UInt16 = static_cast<CHAOS_IL2CPP_INTPTR>(4060647147365029652ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_UInt32 = {{nullptr, nullptr, 4058804365876526466ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_UInt32 = static_cast<CHAOS_IL2CPP_INTPTR>(4058804365876526466ULL);inline TypeInfoV0 chaos_mt_SnapshotTestFixtures_IMarker = {{nullptr, nullptr, 13604930402668384013ULL, 0u, 32, 3, 0}, {nullptr, nullptr, 0, 0, 0, 0}};
inline TypeInfoV0 chaos_mt_SnapshotTestFixtures_IMyInterface = {{nullptr, nullptr, 3080637198406087848ULL, 0u, 32, 3, 0}, {nullptr, nullptr, 0, 0, 0, 0}};
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Int32 = {{nullptr, nullptr, 11009693519287992193ULL, 0u, 32, 2, 0}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_boxed_type_id_System_Private_CoreLib_System_Int32 = static_cast<CHAOS_IL2CPP_INTPTR>(11009693519287992193ULL);

// ── Virtual method table arrays ──
extern "C" void SnapshotTestFixtures_BaseClass__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_BaseClass_Compute(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
const void* chaos_vtable_SnapshotTestFixtures_BaseClass[] =
{
	reinterpret_cast<void*>(SnapshotTestFixtures_BaseClass__ctor),
	reinterpret_cast<void*>(SnapshotTestFixtures_BaseClass_Compute),
};
extern "C" void SnapshotTestFixtures_DerivedClass__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_DerivedClass_Compute(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
const void* chaos_vtable_SnapshotTestFixtures_DerivedClass[] =
{
	reinterpret_cast<void*>(SnapshotTestFixtures_DerivedClass__ctor),
	reinterpret_cast<void*>(SnapshotTestFixtures_DerivedClass_Compute),
};
extern "C" void SnapshotTestFixtures_HasFields__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
const void* chaos_vtable_SnapshotTestFixtures_HasFields[] =
{
	reinterpret_cast<void*>(SnapshotTestFixtures_HasFields__ctor),
};
extern "C" void SnapshotTestFixtures_HasInstanceFields__ctor_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
const void* chaos_vtable_SnapshotTestFixtures_HasInstanceFields[] =
{
	nullptr,
	nullptr,
	reinterpret_cast<void*>(SnapshotTestFixtures_HasInstanceFields__ctor_System_Int32),
};
extern "C" void SnapshotTestFixtures_ImplHelper__ctor_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_ImplHelper_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
const void* chaos_vtable_SnapshotTestFixtures_ImplHelper[] =
{
	nullptr,
	nullptr,
	reinterpret_cast<void*>(SnapshotTestFixtures_ImplHelper__ctor_System_Int32),
	reinterpret_cast<void*>(SnapshotTestFixtures_ImplHelper_GetValue),
};
extern "C" void SnapshotTestFixtures_InstanceHelper__ctor_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_InstanceHelper_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
const void* chaos_vtable_SnapshotTestFixtures_InstanceHelper[] =
{
	nullptr,
	nullptr,
	reinterpret_cast<void*>(SnapshotTestFixtures_InstanceHelper__ctor_System_Int32),
	reinterpret_cast<void*>(SnapshotTestFixtures_InstanceHelper_GetValue),
};
extern "C" void SnapshotTestFixtures_MarkedClass__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
const void* chaos_vtable_SnapshotTestFixtures_MarkedClass[] =
{
	reinterpret_cast<void*>(SnapshotTestFixtures_MarkedClass__ctor),
};
extern "C" void SnapshotTestFixtures_MyClass__ctor_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_MyClass_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
const void* chaos_vtable_SnapshotTestFixtures_MyClass[] =
{
	nullptr,
	nullptr,
	reinterpret_cast<void*>(SnapshotTestFixtures_MyClass__ctor_System_Int32),
	reinterpret_cast<void*>(SnapshotTestFixtures_MyClass_GetValue),
};
extern "C" void SnapshotTestFixtures_SealedHelper__ctor_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_SealedHelper_GetValueVirtual(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
const void* chaos_vtable_SnapshotTestFixtures_SealedHelper[] =
{
	nullptr,
	nullptr,
	reinterpret_cast<void*>(SnapshotTestFixtures_SealedHelper__ctor_System_Int32),
	nullptr,
	reinterpret_cast<void*>(SnapshotTestFixtures_SealedHelper_GetValueVirtual),
};
extern "C" void SnapshotTestFixtures_Wrapper__0__ctor_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR SnapshotTestFixtures_Wrapper__0_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
const void* chaos_vtable_SnapshotTestFixtures_Wrapper__0_[] =
{
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	reinterpret_cast<void*>(SnapshotTestFixtures_Wrapper__0__ctor_0),
	reinterpret_cast<void*>(SnapshotTestFixtures_Wrapper__0_GetValue),
};
extern "C" void SnapshotTestFixtures_Wrapper_System_Int32__ctor_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void chaos_stub_definition_SnapshotTestFixtures_Wrapper_1___ctor_System_Void__0__type__System_Int32__method(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_Wrapper_System_Int32_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 chaos_stub_definition_SnapshotTestFixtures_Wrapper_1__GetValue__0___type__System_Int32__method(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
const void* chaos_vtable_SnapshotTestFixtures_Wrapper_System_Int32_[] =
{
	nullptr,
	nullptr,
	reinterpret_cast<void*>(chaos_stub_definition_SnapshotTestFixtures_Wrapper_1___ctor_System_Void__0__type__System_Int32__method),
	reinterpret_cast<void*>(chaos_stub_definition_SnapshotTestFixtures_Wrapper_1__GetValue__0___type__System_Int32__method),
};
static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_SnapshotTestFixtures_BaseClass[] =
{
	{ 0x0000009Du, reinterpret_cast<void*>(&SnapshotTestFixtures_BaseClass__ctor) },
	{ 0x0000009Cu, reinterpret_cast<void*>(&SnapshotTestFixtures_BaseClass_Compute) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_SnapshotTestFixtures_DerivedClass[] =
{
	{ 0x0000009Fu, reinterpret_cast<void*>(&SnapshotTestFixtures_DerivedClass__ctor) },
	{ 0x0000009Eu, reinterpret_cast<void*>(&SnapshotTestFixtures_DerivedClass_Compute) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_SnapshotTestFixtures_HasFields[] =
{
	{ 0x000000B9u, reinterpret_cast<void*>(&SnapshotTestFixtures_HasFields__ctor) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_SnapshotTestFixtures_HasInstanceFields[] =
{
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0x000000A7u, reinterpret_cast<void*>(&SnapshotTestFixtures_HasInstanceFields__ctor_System_Int32) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_SnapshotTestFixtures_ImplHelper[] =
{
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0x00000110u, reinterpret_cast<void*>(&SnapshotTestFixtures_ImplHelper__ctor_System_Int32) },
	{ 0x00000111u, reinterpret_cast<void*>(&SnapshotTestFixtures_ImplHelper_GetValue) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_SnapshotTestFixtures_InstanceHelper[] =
{
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0x00000084u, reinterpret_cast<void*>(&SnapshotTestFixtures_InstanceHelper__ctor_System_Int32) },
	{ 0x00000086u, reinterpret_cast<void*>(&SnapshotTestFixtures_InstanceHelper_GetValue) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_SnapshotTestFixtures_MarkedClass[] =
{
	{ 0x000000A1u, reinterpret_cast<void*>(&SnapshotTestFixtures_MarkedClass__ctor) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_SnapshotTestFixtures_MyClass[] =
{
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0x000000D4u, reinterpret_cast<void*>(&SnapshotTestFixtures_MyClass__ctor_System_Int32) },
	{ 0x000000D5u, reinterpret_cast<void*>(&SnapshotTestFixtures_MyClass_GetValue) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_SnapshotTestFixtures_SealedHelper[] =
{
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0x0000010Cu, reinterpret_cast<void*>(&SnapshotTestFixtures_SealedHelper__ctor_System_Int32) },
	{ 0u, nullptr },
	{ 0x0000010Du, reinterpret_cast<void*>(&SnapshotTestFixtures_SealedHelper_GetValueVirtual) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_SnapshotTestFixtures_Wrapper__0_[] =
{
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0x00000115u, reinterpret_cast<void*>(&SnapshotTestFixtures_Wrapper__0__ctor_0) },
	{ 0x00000116u, reinterpret_cast<void*>(&SnapshotTestFixtures_Wrapper__0_GetValue) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_SnapshotTestFixtures_Wrapper_System_Int32_[] =
{
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0x00000113u, reinterpret_cast<void*>(&chaos_stub_definition_SnapshotTestFixtures_Wrapper_1___ctor_System_Void__0__type__System_Int32__method) },
	{ 0x00000114u, reinterpret_cast<void*>(&chaos_stub_definition_SnapshotTestFixtures_Wrapper_1__GetValue__0___type__System_Int32__method) },
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

struct chaos_type_SnapshotProverSubjects_SnapshotProverSubjects
{
	ThinLockableHeader header{};
};

struct chaos_type_SnapshotTestFixtures_BaseClass
{
	ThinLockableHeader header{};
};

struct chaos_type_SnapshotTestFixtures_DerivedClass : public chaos_type_SnapshotTestFixtures_BaseClass
{
};

struct chaos_type_SnapshotTestFixtures_FieldHelper
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Private_CoreLib_System_Object
{
	ThinLockableHeader header{};
};

struct chaos_type_SnapshotTestFixtures_HasFields : public chaos_type_System_Private_CoreLib_System_Object
{
	CHAOS_IL2CPP_INTPTR field_SnapshotTestFixtures_HasFields__instanceVal = 0;
};

struct chaos_type_SnapshotTestFixtures_HasInstanceFields : public chaos_type_System_Private_CoreLib_System_Object
{
	CHAOS_IL2CPP_INTPTR field_SnapshotTestFixtures_HasInstanceFields__Value = 0;
};

struct chaos_type_SnapshotTestFixtures_ImplHelper : public chaos_type_System_Private_CoreLib_System_Object
{
	CHAOS_IL2CPP_INTPTR field_SnapshotTestFixtures_ImplHelper___val = 0;
};

struct chaos_type_SnapshotTestFixtures_InstanceHelper : public chaos_type_System_Private_CoreLib_System_Object
{
	CHAOS_IL2CPP_INTPTR field_SnapshotTestFixtures_InstanceHelper___val = 0;
};

struct chaos_type_SnapshotTestFixtures_LdftnHelper___O
{
	ThinLockableHeader header{};
};

struct chaos_type_SnapshotTestFixtures_MarkedClass : public chaos_type_System_Private_CoreLib_System_Object
{
	CHAOS_IL2CPP_INTPTR field_SnapshotTestFixtures_MarkedClass__Value = 0;
};

struct chaos_type_SnapshotTestFixtures_MyClass : public chaos_type_System_Private_CoreLib_System_Object
{
	CHAOS_IL2CPP_INTPTR field_SnapshotTestFixtures_MyClass___val = 0;
};

struct chaos_type_SnapshotTestFixtures_SealedHelper : public chaos_type_System_Private_CoreLib_System_Object
{
	CHAOS_IL2CPP_INTPTR field_SnapshotTestFixtures_SealedHelper___val = 0;
};

struct chaos_type_SnapshotTestFixtures_StaticFieldWriteHelper
{
	ThinLockableHeader header{};
};

struct chaos_type_SnapshotTestFixtures_Wrapper__0_
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR field_SnapshotTestFixtures_Wrapper__0____val = 0;
};

struct chaos_type_SnapshotTestFixtures_Wrapper_System_Int32_ : public chaos_type_System_Private_CoreLib_System_Object
{
	CHAOS_IL2CPP_INTPTR field_SnapshotTestFixtures_Wrapper_System_Int32____val = 0;
};

struct chaos_type_System_Collections_System_Collections_Generic_Dictionary_System_Int32_System_Int32_
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Collections_System_Collections_Generic_HashSet_System_Int32_
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Collections_System_Collections_Generic_List_System_Int32_
{
	ThinLockableHeader header{};
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

struct chaos_type_System_Private_CoreLib_System_Double
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Private_CoreLib_System_MulticastDelegate : public chaos_type_System_Private_CoreLib_System_Delegate
{
};

struct chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Int32_ : public chaos_type_System_Private_CoreLib_System_MulticastDelegate
{
};

struct chaos_type_System_Private_CoreLib_System_Func_System_Int32_ : public chaos_type_System_Private_CoreLib_System_MulticastDelegate
{
};

struct chaos_type_System_Private_CoreLib_System_Int16
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Private_CoreLib_System_Int32
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Private_CoreLib_System_Int64
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Private_CoreLib_System_OverflowException
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR _message = 0;
	CHAOS_IL2CPP_INTPTR _innerException = 0;
	CHAOS_IL2CPP_INTPTR _stackTrace = 0;
	CHAOS_IL2CPP_INT32 _HResult = 0;
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

struct chaos_type_System_Private_CoreLib_System_SByte
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Private_CoreLib_System_Single
{
	ThinLockableHeader header{};
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

struct chaos_type_System_Private_CoreLib_System_UInt16
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Private_CoreLib_System_UInt32
{
	ThinLockableHeader header{};
};

struct chaos_boxed_type_System_Private_CoreLib_System_Int32
{
	PureTypeHeader header{};
	CHAOS_IL2CPP_INTPTR value = 0;
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
	chaos_type->header.type_info = &chaos_mt_System_Private_CoreLib_System_Type.hot;
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
		case static_cast<CHAOS_IL2CPP_INTPTR>(47319400u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_0") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(111373729u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_1") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(113672426u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_2") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(115853767u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_3") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(115115800u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_4") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(114246245u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_5") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(112398158u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_6") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(110896379u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_7") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(102620620u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_8") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116627369u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_9") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(106783026u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(49409092u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunAddress") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109816299u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(37548307u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunCompare") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(110347403u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(36446084u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunAdd") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101156254u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(45533953u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "MakeAndFill") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101319541u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(49305945u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunLength") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(104475972u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(39576434u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunArrayRef") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(102333289u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(36086899u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, ".ctor") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(110758931u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Compute") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(105066326u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(36450275u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunBitwise") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(114481423u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(35794926u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "DemoBoxStore") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(108865643u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(39839167u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "BoxAndUnbox") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(112139539u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(36852263u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunBranchCompareB") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(111030583u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(37051095u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunBranchCompare") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107335663u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(35550869u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunBranchDup") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(117131967u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(38017159u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunBranchUnsignedB") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(106612967u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(47496439u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunBranchUnsigned") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(105522271u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(50330935u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "CreateAndUse") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(117060723u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(42338923u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunCalli") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(111773118u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(36724567u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunCallvirt") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116521911u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(49873977u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "TestDict") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(105785589u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "TestList") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(112861293u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "TestSet") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109591175u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(50144478u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunConvSmall") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(115385262u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(49526757u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunConvWide") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101405359u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(43576188u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "ConvertToInt") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109538579u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(41492034u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunCpblk") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101723267u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(38596644u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunCpobj") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(115809963u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(50163999u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunDelegate") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(114556063u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(37280839u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, ".ctor") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(113885887u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Compute") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(103412770u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(35893753u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "SafeDivide") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(117304437u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(35103672u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "CallExternal") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101158779u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(39962160u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "GetAndIncrement") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(112968756u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(35638090u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunFloatOps") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(108831613u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(47863529u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "IsPositive") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(110974616u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(36311393u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "UseGeneric") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(100688869u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(48836823u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "DemoCombine") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(110874365u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(49883188u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "ConsumeString") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(112982909u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "GetValue") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(102894917u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Nop") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(102149083u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Square") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101423646u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(47349103u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Run") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(106762602u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(41759528u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Run") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(117215689u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(38030504u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "GetValue") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(111621689u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(47412574u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, ".ctor") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(108154201u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "GetValue") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(100994607u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(36269270u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "ReadWriteRef") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(104869148u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(49851465u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "CreateAndUseFields") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(108375670u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(37525155u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, ".ctor") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(111550354u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "GetValue") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(110120276u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(42394183u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunInterfaceTest") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(110353895u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(35527789u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Double") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(111478191u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(48450805u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunLdVirtftn") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107201247u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(45178287u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunLdarga") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109863150u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(37812404u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunLdcI8") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(115951764u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(50228854u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "TestAllElems") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109414911u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(34459302u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunLdelema") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116536107u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(46310704u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "GetFnPtr") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(104440880u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(49897275u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunLdindI1") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(105059215u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(34069500u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunLdindI8") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(103220199u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(48560878u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunLdindI4") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116708385u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunLdindI") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116199895u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunStindI") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(110857806u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(41161170u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunLdindUnsigned") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(100860135u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(43083406u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunLdindWide") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101367635u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(45348393u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunLdtoken") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116673347u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(49793300u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunAlloc") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107613056u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(38752278u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "SumToFive") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(114206742u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(37874478u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunSquare") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(114870726u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(48669607u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunArglist") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(108197278u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunJmp") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(114654739u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunMkrefany") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(111988681u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunRefanytype") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(114897901u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunRefanyval") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101685302u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(48911845u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunMulDiv") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(110685591u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(48961364u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "DispatchBoth") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101354081u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(39897870u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, ".ctor") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109620041u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "GetValue") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101796767u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(43036006u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunNegShift") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(112505297u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(36291623u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "ConvOvfI1") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(106677315u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "ConvOvfI1Un") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(110673166u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "ConvOvfI2Un") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(105949409u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "ConvOvfI4Un") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(108184346u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "ConvOvfI8Un") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107311650u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "ConvOvfIUn") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109204337u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "ConvOvfU1Un") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(111821354u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "ConvOvfU2Un") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101279549u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "ConvOvfU4Un") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116588670u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "ConvOvfU8Un") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109032213u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "ConvOvfUUn") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(110237445u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(40652224u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "ConvOvfI2") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(114191911u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "ConvOvfI4") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(114372492u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "ConvOvfI8") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(110413360u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "ConvOvfU1") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(113169484u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "ConvOvfU2") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101598115u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "ConvOvfU4") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(108490208u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "ConvOvfU8") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107256895u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(39143358u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunOverflowAdd") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101430842u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunOverflowMul") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(108578957u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunOverflowSub") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(115656137u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(42641737u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunOverflowAddUn") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(103071682u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunOverflowMulUn") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(111179913u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunOverflowSubUn") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109135141u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(42828960u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "TestConvRUn") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107623132u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "TestInitBlk") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(102275110u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "TestStarg") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(106581188u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(36384403u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunRethrow") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109133903u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(44908915u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunSealedVirtual") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(103771063u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(34120164u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, ".ctor") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(113746803u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "GetValueVirtual") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(113172662u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(36014504u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunShift") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(113797379u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(42914831u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunConvI") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(108105205u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunConvOvfI") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(103629077u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunConvOvfIUn") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101489020u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunConvOvfU") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(102171633u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunConvOvfUUn") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(108256824u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunConvU4") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(102197759u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunLdnull") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(114812949u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(47768488u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "GetSize") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(115174073u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(34731889u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "WriteAndRead") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(114484249u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(36408351u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "TestAllElems") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(106904464u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(43410375u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunStindNarrow") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109630671u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(50273967u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunStindWide") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(115703615u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(48626301u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "TestConcat") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116100901u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(34964670u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "TestFormatOne") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(111532355u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(44876563u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "UseString") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(103468675u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(39914904u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Classify") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116986528u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(42481549u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "GetValue") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101533326u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(34153228u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "CheckPositive") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(106211162u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(34318004u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "CheckAndCast") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(106415284u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(42669808u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunUnbox") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(114489347u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(35259661u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "TestCkfinite") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(105119869u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "TestCltUn") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(110542409u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "TestDivUn") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109526267u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "TestRemUn") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(103788546u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(41490949u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "RunValueType") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(106763891u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(43764333u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "UseVirtualDispatch") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(112384933u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(42461291u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "UseVirtual") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(115398185u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(41456133u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "DoNothing") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(111433593u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(40238434u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, ".ctor") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(115351955u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "GetValue") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(108787077u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(49885664u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, ".ctor") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(115073687u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "GetValue") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(108162289u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(47145076u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, ".ctor") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(102567961u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "GetValue") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(104724879u);
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
	chaos_method->header.type_info = &chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo.hot;
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
	chaos_closed_method->header.type_info = &chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo.hot;
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

	return 0;
}

CHAOS_IL2CPP_INT32 chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = 0;
CHAOS_IL2CPP_INT32 chaos_static_SnapshotTestFixtures_FieldHelper___counter = 0;
CHAOS_IL2CPP_INTPTR chaos_static_SnapshotTestFixtures_LdftnHelper___O___0___GetValue = 0;
CHAOS_IL2CPP_INT32 chaos_static_SnapshotTestFixtures_StaticFieldWriteHelper___readTarget = 0;
CHAOS_IL2CPP_INT32 chaos_static_SnapshotTestFixtures_StaticFieldWriteHelper___writeTarget = 0;


	// AOT-baked string table: sorted by StringId for binary search at runtime.
	constexpr chaos::il2cpp::string_table::StringEntry chaos_aot_string_entries[] = {
		{ 628919584683901915U, "hi", 2u },
		{ 803950926144638187U, "Value: {0}", 10u },
		{ 7201466553693376363U, "Hello", 5u },
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

static void chaos_external_runtime_System_Private_CoreLib_System_Collections_Generic_Dictionary_System_Int32_System_Int32___Add_System_Void_System_Int32_System_Int32_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2)
{
	CollectionDictionaryAdd(chaos_arg_0, chaos_arg_1, chaos_arg_2);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Collections_Generic_Dictionary_System_Int32_System_Int32___ContainsKey_System_Boolean_System_Int32_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
	return CollectionDictionaryContainsKey(chaos_arg_0, chaos_arg_1);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Collections_Generic_Dictionary_System_Int32_System_Int32___Remove_System_Boolean_System_Int32_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
	return CollectionDictionaryRemove(chaos_arg_0, chaos_arg_1);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Collections_Generic_Dictionary_System_Int32_System_Int32___TryGetValue_System_Boolean_System_Int32_System_Int32__(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2)
{
	return CollectionDictionaryTryGetValue(chaos_arg_0, chaos_arg_1, reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_arg_2));
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Collections_Generic_HashSet_System_Int32___Add_System_Boolean_System_Int32_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
	return CollectionHashSetAdd(chaos_arg_0, chaos_arg_1);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Collections_Generic_HashSet_System_Int32___Contains_System_Boolean_System_Int32_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
	return CollectionHashSetContains(chaos_arg_0, chaos_arg_1);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Collections_Generic_HashSet_System_Int32___Remove_System_Boolean_System_Int32_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
	return CollectionHashSetRemove(chaos_arg_0, chaos_arg_1);
}

static void chaos_external_runtime_System_Private_CoreLib_System_Collections_Generic_List_System_Int32___Add_System_Void_System_Int32_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
	CollectionListAdd(chaos_arg_0, chaos_arg_1);
}

static void chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	ChaosObjectCtor(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_String__Format_System_String_System_String_System_Object_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
	return ChaosStringFormat1(chaos_arg_0, chaos_arg_1);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetTypeFromHandle(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_TypeHandle_System_RuntimeTypeHandle__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetTypeHandle(chaos_fn_arg_0);
}

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };

extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_0(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_1(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_2(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_3(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_4(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_5(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_6(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_7(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_8(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_9(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_AddressHelper_RunAddress(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_ArithmeticCompareHelper_RunCompare(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_ArithmeticOps_RunAdd(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_ArrayHelper_MakeAndFill(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_ArrayLengthHelper_RunLength(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_ArrayRefHelper_RunArrayRef(void);
extern "C" void SnapshotTestFixtures_BaseClass__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_BaseClass_Compute(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_BitwiseHelper_RunBitwise(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_BoxingHelper_BoxAndUnbox(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_BoxInterfaceArrayDemo_DemoBoxStore(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_BranchCompareBHelper_RunBranchCompareB(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_BranchCompareHelper_RunBranchCompare(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_BranchDupHelper_RunBranchDup(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_BranchUnsignedBHelper_RunBranchUnsignedB(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_BranchUnsignedHelper_RunBranchUnsigned(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_CalliHelper_RunCalli_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_CallVirtHelper_CreateAndUse(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_CallvirtHelper_RunCallvirt(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_CollectionsHelper_TestDict(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_CollectionsHelper_TestList(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_CollectionsHelper_TestSet(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_ConversionHelper_ConvertToInt(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_ConvSmallIntHelper_RunConvSmall(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_ConvWideHelper_RunConvWide(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_CpblkHelper_RunCpblk(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_CpobjHelper_RunCpobj(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_DelegateHelper_RunDelegate(void);
extern "C" void SnapshotTestFixtures_DerivedClass__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_DerivedClass_Compute(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_EhHelper_SafeDivide(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_ExternalCaller_CallExternal(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_FieldHelper_GetAndIncrement(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_FloatOpsHelper_RunFloatOps(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_FlowControl_IsPositive(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_GenericHelper_UseGeneric(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_GenericsVirtEhDemo_DemoCombine(void);
extern "C" void SnapshotTestFixtures_HasFields__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void SnapshotTestFixtures_HasInstanceFields__ctor_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void SnapshotTestFixtures_Helper_ConsumeString_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_Helper_GetValue(void);
extern "C" void SnapshotTestFixtures_Helper_Nop(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_Helper_Square_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_HotUpdateWithGenericsDemo_Run(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_HotUpdateWithTypesDemo_Run(void);
extern "C" void SnapshotTestFixtures_ImplHelper__ctor_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_ImplHelper_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_IndirectHelper_ReadWriteRef(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_InstanceFieldHelper_CreateAndUseFields(void);
extern "C" void SnapshotTestFixtures_InstanceHelper__ctor_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_InstanceHelper_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_InterfaceDevirtHelper_RunInterfaceTest(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_InternalHelper_Double_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_LdargaHelper_RunLdarga_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 SnapshotTestFixtures_LdcI8Helper_RunLdcI8(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_LdelemaHelper_RunLdelema(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_LdelemAllHelper_TestAllElems(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_LdftnHelper_GetFnPtr(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_LdindI1Helper_RunLdindI1(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_LdindI8Helper_RunLdindI8(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_LdindStindGapsHelper_RunLdindI(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_LdindStindGapsHelper_RunLdindI4(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_LdindStindGapsHelper_RunStindI(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_LdindUnsignedHelper_RunLdindUnsigned(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_LdindWideHelper_RunLdindWide(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_LdtokenHelper_RunLdtoken(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_LdVirtftnHelper_RunLdVirtftn(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_LocalAllocHelper_RunAlloc(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_LoopHelper_SumToFive(void);
extern "C" void SnapshotTestFixtures_MarkedClass__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_MathHelper_RunSquare(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_MoreRareOpsHelper_RunArglist(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_MoreRareOpsHelper_RunJmp(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_MoreRareOpsHelper_RunMkrefany(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_MoreRareOpsHelper_RunRefanytype(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_MoreRareOpsHelper_RunRefanyval(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_MulDivHelper_RunMulDiv(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_MultiDispatcher_DispatchBoth(void);
extern "C" void SnapshotTestFixtures_MyClass__ctor_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_MyClass_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_NegShiftHelper_RunNegShift(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfI1_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfI1Un_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfI2Un_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfI4Un_System_Int64(CHAOS_IL2CPP_INT64 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfI8Un_System_Single(float chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfIUn_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfU1Un_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfU2Un_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfU4Un_System_Int64(CHAOS_IL2CPP_INT64 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfU8Un_System_Double(double chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfUUn_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_OverflowConvHelper_ConvOvfI2_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_OverflowConvHelper_ConvOvfI4(void);
extern "C" CHAOS_IL2CPP_INT64 SnapshotTestFixtures_OverflowConvHelper_ConvOvfI8_System_Single(float chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_OverflowConvHelper_ConvOvfU1_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_OverflowConvHelper_ConvOvfU2_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_OverflowConvHelper_ConvOvfU4_System_Int64(CHAOS_IL2CPP_INT64 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 SnapshotTestFixtures_OverflowConvHelper_ConvOvfU8_System_Double(double chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_OverflowHelper_RunOverflowAdd(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_OverflowHelper_RunOverflowMul(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_OverflowHelper_RunOverflowSub(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_OverflowUnHelper_RunOverflowAddUn(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_OverflowUnHelper_RunOverflowMulUn(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_OverflowUnHelper_RunOverflowSubUn(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_RareOpsHelper_TestConvRUn_System_Single(float chaos_fn_arg_0);
extern "C" void SnapshotTestFixtures_RareOpsHelper_TestInitBlk_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_RareOpsHelper_TestStarg_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_RethrowHelper_RunRethrow(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_SealedClassVirtualHelper_RunSealedVirtual(void);
extern "C" void SnapshotTestFixtures_SealedHelper__ctor_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_SealedHelper_GetValueVirtual(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_ShiftHelper_RunShift(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_SimpleGapsHelper_RunConvI(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_SimpleGapsHelper_RunConvOvfI_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_SimpleGapsHelper_RunConvOvfIUn_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_SimpleGapsHelper_RunConvOvfU_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_SimpleGapsHelper_RunConvOvfUUn_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_SimpleGapsHelper_RunConvU4(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_SimpleGapsHelper_RunLdnull(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_SizeOfHelper_GetSize(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_StaticFieldWriteHelper_WriteAndRead(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_StelemAllHelper_TestAllElems(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_StindNarrowHelper_RunStindNarrow(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_StindWideHelper_RunStindWide(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_StringConcatHelper_TestConcat(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_StringFormatHelper_TestFormatOne(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_StringOps_UseString(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_SwitchHelper_Classify(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_TargetHelper_GetValue(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_ThrowHelper_CheckPositive_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_TypeCheckHelper_CheckAndCast(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_UnboxHelper_RunUnbox(void);
extern "C" double SnapshotTestFixtures_UnsignedOpsHelper_TestCkfinite_System_Double(double chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_UnsignedOpsHelper_TestCltUn(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_UnsignedOpsHelper_TestDivUn(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_UnsignedOpsHelper_TestRemUn(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_ValueTypeHelper_RunValueType(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_VirtualDispatchHelper_UseVirtualDispatch(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_VirtualHelper_UseVirtual(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_VoidCaller_DoNothing(void);
extern "C" void SnapshotTestFixtures_Wrapper_1__ctor_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR SnapshotTestFixtures_Wrapper_1_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void SnapshotTestFixtures_Wrapper__0__ctor_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR SnapshotTestFixtures_Wrapper__0_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void SnapshotTestFixtures_Wrapper_System_Int32__ctor_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void chaos_stub_definition_SnapshotTestFixtures_Wrapper_1___ctor_System_Void__0__type__System_Int32__method(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_Wrapper_System_Int32_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 chaos_stub_definition_SnapshotTestFixtures_Wrapper_1__GetValue__0___type__System_Int32__method(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[158] = {
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
	1u,
	1u,
	1u,
	1u,
	1u,
	1u,
	1u,
	2u,
	3u,
	3u,
	3u,
	4u,
	4u,
	4u,
	5u,
	5u,
	5u,
	5u,
	6u,
	6u,
	6u,
	7u,
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
	8u,
	8u,
	8u,
	8u,
	8u,
	9u,
	9u,
	9u,
	10u,
	11u,
	12u,
	13u,
	14u,
	15u,
	16u,
	17u,
	18u,
	19u,
	20u,
	21u,
	21u,
	22u,
	23u,
	24u,
	25u,
	26u,
	26u,
	26u,
	26u,
	26u,
	26u,
	26u,
	27u,
	28u,
	29u,
	29u,
	29u,
	30u,
	30u,
	30u,
	30u,
	31u,
	32u,
	33u,
	34u,
	34u,
	34u,
	34u,
	34u,
	34u,
	34u,
	34u,
	34u,
	34u,
	34u,
	34u,
	34u,
	35u,
	35u,
	35u,
	36u,
	36u,
	36u,
	36u,
	36u,
	36u,
	36u,
	36u,
	37u,
	37u,
	38u,
	38u,
	39u,
	39u,
};

static constexpr struct {
	::ChaosAbiManifestV0 header;
	::ChaosAbiMethodEntryV0 entries[157];
	CHAOS_IL2CPP_UINT8 params[39];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		157u,
		39u,
		1229870042u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // SnapshotProverSubjects_SnapshotProverSubjects_Subject_0
		{ 0u, 0u },  // SnapshotProverSubjects_SnapshotProverSubjects_Subject_1
		{ 0u, 0u },  // SnapshotProverSubjects_SnapshotProverSubjects_Subject_2
		{ 0u, 0u },  // SnapshotProverSubjects_SnapshotProverSubjects_Subject_3
		{ 0u, 0u },  // SnapshotProverSubjects_SnapshotProverSubjects_Subject_4
		{ 0u, 0u },  // SnapshotProverSubjects_SnapshotProverSubjects_Subject_5
		{ 0u, 0u },  // SnapshotProverSubjects_SnapshotProverSubjects_Subject_6
		{ 0u, 0u },  // SnapshotProverSubjects_SnapshotProverSubjects_Subject_7
		{ 0u, 0u },  // SnapshotProverSubjects_SnapshotProverSubjects_Subject_8
		{ 0u, 0u },  // SnapshotProverSubjects_SnapshotProverSubjects_Subject_9
		{ 1u, 0u },  // SnapshotTestFixtures_AddressHelper_RunAddress
		{ 1u, 0u },  // SnapshotTestFixtures_ArithmeticCompareHelper_RunCompare
		{ 1u, 0u },  // SnapshotTestFixtures_ArithmeticOps_RunAdd
		{ 1u, 0u },  // SnapshotTestFixtures_ArrayHelper_MakeAndFill
		{ 1u, 0u },  // SnapshotTestFixtures_ArrayLengthHelper_RunLength
		{ 1u, 0u },  // SnapshotTestFixtures_ArrayRefHelper_RunArrayRef
		{ 0u, 0u },  // SnapshotTestFixtures_BaseClass__ctor
		{ 1u, 0u },  // SnapshotTestFixtures_BaseClass_Compute
		{ 1u, 0u },  // SnapshotTestFixtures_BitwiseHelper_RunBitwise
		{ 1u, 0u },  // SnapshotTestFixtures_BoxingHelper_BoxAndUnbox
		{ 1u, 0u },  // SnapshotTestFixtures_BoxInterfaceArrayDemo_DemoBoxStore
		{ 1u, 0u },  // SnapshotTestFixtures_BranchCompareBHelper_RunBranchCompareB
		{ 1u, 0u },  // SnapshotTestFixtures_BranchCompareHelper_RunBranchCompare
		{ 1u, 0u },  // SnapshotTestFixtures_BranchDupHelper_RunBranchDup
		{ 1u, 0u },  // SnapshotTestFixtures_BranchUnsignedBHelper_RunBranchUnsignedB
		{ 1u, 0u },  // SnapshotTestFixtures_BranchUnsignedHelper_RunBranchUnsigned
		{ 1u, 1u },  // SnapshotTestFixtures_CalliHelper_RunCalli_System_Int32
		{ 1u, 0u },  // SnapshotTestFixtures_CallVirtHelper_CreateAndUse
		{ 1u, 0u },  // SnapshotTestFixtures_CallvirtHelper_RunCallvirt
		{ 1u, 0u },  // SnapshotTestFixtures_CollectionsHelper_TestDict
		{ 1u, 0u },  // SnapshotTestFixtures_CollectionsHelper_TestList
		{ 1u, 0u },  // SnapshotTestFixtures_CollectionsHelper_TestSet
		{ 1u, 0u },  // SnapshotTestFixtures_ConversionHelper_ConvertToInt
		{ 1u, 0u },  // SnapshotTestFixtures_ConvSmallIntHelper_RunConvSmall
		{ 1u, 0u },  // SnapshotTestFixtures_ConvWideHelper_RunConvWide
		{ 1u, 0u },  // SnapshotTestFixtures_CpblkHelper_RunCpblk
		{ 1u, 0u },  // SnapshotTestFixtures_CpobjHelper_RunCpobj
		{ 1u, 0u },  // SnapshotTestFixtures_DelegateHelper_RunDelegate
		{ 0u, 0u },  // SnapshotTestFixtures_DerivedClass__ctor
		{ 1u, 0u },  // SnapshotTestFixtures_DerivedClass_Compute
		{ 1u, 0u },  // SnapshotTestFixtures_EhHelper_SafeDivide
		{ 1u, 0u },  // SnapshotTestFixtures_ExternalCaller_CallExternal
		{ 1u, 0u },  // SnapshotTestFixtures_FieldHelper_GetAndIncrement
		{ 1u, 0u },  // SnapshotTestFixtures_FloatOpsHelper_RunFloatOps
		{ 1u, 0u },  // SnapshotTestFixtures_FlowControl_IsPositive
		{ 1u, 0u },  // SnapshotTestFixtures_GenericHelper_UseGeneric
		{ 1u, 0u },  // SnapshotTestFixtures_GenericsVirtEhDemo_DemoCombine
		{ 0u, 0u },  // SnapshotTestFixtures_HasFields__ctor
		{ 0u, 1u },  // SnapshotTestFixtures_HasInstanceFields__ctor_System_Int32
		{ 0u, 1u },  // SnapshotTestFixtures_Helper_ConsumeString_System_String
		{ 1u, 0u },  // SnapshotTestFixtures_Helper_GetValue
		{ 0u, 0u },  // SnapshotTestFixtures_Helper_Nop
		{ 1u, 1u },  // SnapshotTestFixtures_Helper_Square_System_Int32
		{ 1u, 0u },  // SnapshotTestFixtures_HotUpdateWithGenericsDemo_Run
		{ 1u, 0u },  // SnapshotTestFixtures_HotUpdateWithTypesDemo_Run
		{ 0u, 1u },  // SnapshotTestFixtures_ImplHelper__ctor_System_Int32
		{ 1u, 0u },  // SnapshotTestFixtures_ImplHelper_GetValue
		{ 1u, 0u },  // SnapshotTestFixtures_IndirectHelper_ReadWriteRef
		{ 1u, 0u },  // SnapshotTestFixtures_InstanceFieldHelper_CreateAndUseFields
		{ 0u, 1u },  // SnapshotTestFixtures_InstanceHelper__ctor_System_Int32
		{ 1u, 0u },  // SnapshotTestFixtures_InstanceHelper_GetValue
		{ 1u, 0u },  // SnapshotTestFixtures_InterfaceDevirtHelper_RunInterfaceTest
		{ 1u, 1u },  // SnapshotTestFixtures_InternalHelper_Double_System_Int32
		{ 1u, 1u },  // SnapshotTestFixtures_LdargaHelper_RunLdarga_System_Int32
		{ 10u, 0u },  // SnapshotTestFixtures_LdcI8Helper_RunLdcI8
		{ 1u, 0u },  // SnapshotTestFixtures_LdelemaHelper_RunLdelema
		{ 1u, 0u },  // SnapshotTestFixtures_LdelemAllHelper_TestAllElems
		{ 1u, 0u },  // SnapshotTestFixtures_LdftnHelper_GetFnPtr
		{ 1u, 0u },  // SnapshotTestFixtures_LdindI1Helper_RunLdindI1
		{ 1u, 0u },  // SnapshotTestFixtures_LdindI8Helper_RunLdindI8
		{ 1u, 0u },  // SnapshotTestFixtures_LdindStindGapsHelper_RunLdindI
		{ 1u, 0u },  // SnapshotTestFixtures_LdindStindGapsHelper_RunLdindI4
		{ 1u, 0u },  // SnapshotTestFixtures_LdindStindGapsHelper_RunStindI
		{ 1u, 0u },  // SnapshotTestFixtures_LdindUnsignedHelper_RunLdindUnsigned
		{ 1u, 0u },  // SnapshotTestFixtures_LdindWideHelper_RunLdindWide
		{ 1u, 0u },  // SnapshotTestFixtures_LdtokenHelper_RunLdtoken
		{ 1u, 0u },  // SnapshotTestFixtures_LdVirtftnHelper_RunLdVirtftn
		{ 1u, 0u },  // SnapshotTestFixtures_LocalAllocHelper_RunAlloc
		{ 1u, 0u },  // SnapshotTestFixtures_LoopHelper_SumToFive
		{ 0u, 0u },  // SnapshotTestFixtures_MarkedClass__ctor
		{ 1u, 0u },  // SnapshotTestFixtures_MathHelper_RunSquare
		{ 1u, 0u },  // SnapshotTestFixtures_MoreRareOpsHelper_RunArglist
		{ 1u, 0u },  // SnapshotTestFixtures_MoreRareOpsHelper_RunJmp
		{ 1u, 0u },  // SnapshotTestFixtures_MoreRareOpsHelper_RunMkrefany
		{ 1u, 0u },  // SnapshotTestFixtures_MoreRareOpsHelper_RunRefanytype
		{ 1u, 0u },  // SnapshotTestFixtures_MoreRareOpsHelper_RunRefanyval
		{ 1u, 0u },  // SnapshotTestFixtures_MulDivHelper_RunMulDiv
		{ 1u, 0u },  // SnapshotTestFixtures_MultiDispatcher_DispatchBoth
		{ 0u, 1u },  // SnapshotTestFixtures_MyClass__ctor_System_Int32
		{ 1u, 0u },  // SnapshotTestFixtures_MyClass_GetValue
		{ 1u, 0u },  // SnapshotTestFixtures_NegShiftHelper_RunNegShift
		{ 1u, 1u },  // SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfI1_System_Int32
		{ 1u, 1u },  // SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfI1Un_System_Int32
		{ 1u, 1u },  // SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfI2Un_System_Int32
		{ 1u, 1u },  // SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfI4Un_System_Int64
		{ 10u, 1u },  // SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfI8Un_System_Single
		{ 1u, 1u },  // SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfIUn_System_Int32
		{ 1u, 1u },  // SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfU1Un_System_Int32
		{ 1u, 1u },  // SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfU2Un_System_Int32
		{ 1u, 1u },  // SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfU4Un_System_Int64
		{ 10u, 1u },  // SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfU8Un_System_Double
		{ 1u, 1u },  // SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfUUn_System_Int32
		{ 1u, 1u },  // SnapshotTestFixtures_OverflowConvHelper_ConvOvfI2_System_Int32
		{ 1u, 0u },  // SnapshotTestFixtures_OverflowConvHelper_ConvOvfI4
		{ 10u, 1u },  // SnapshotTestFixtures_OverflowConvHelper_ConvOvfI8_System_Single
		{ 1u, 1u },  // SnapshotTestFixtures_OverflowConvHelper_ConvOvfU1_System_Int32
		{ 1u, 1u },  // SnapshotTestFixtures_OverflowConvHelper_ConvOvfU2_System_Int32
		{ 1u, 1u },  // SnapshotTestFixtures_OverflowConvHelper_ConvOvfU4_System_Int64
		{ 10u, 1u },  // SnapshotTestFixtures_OverflowConvHelper_ConvOvfU8_System_Double
		{ 1u, 0u },  // SnapshotTestFixtures_OverflowHelper_RunOverflowAdd
		{ 1u, 0u },  // SnapshotTestFixtures_OverflowHelper_RunOverflowMul
		{ 1u, 0u },  // SnapshotTestFixtures_OverflowHelper_RunOverflowSub
		{ 1u, 0u },  // SnapshotTestFixtures_OverflowUnHelper_RunOverflowAddUn
		{ 1u, 0u },  // SnapshotTestFixtures_OverflowUnHelper_RunOverflowMulUn
		{ 1u, 0u },  // SnapshotTestFixtures_OverflowUnHelper_RunOverflowSubUn
		{ 1u, 1u },  // SnapshotTestFixtures_RareOpsHelper_TestConvRUn_System_Single
		{ 0u, 1u },  // SnapshotTestFixtures_RareOpsHelper_TestInitBlk_System_Int32
		{ 1u, 1u },  // SnapshotTestFixtures_RareOpsHelper_TestStarg_System_Int32
		{ 1u, 0u },  // SnapshotTestFixtures_RethrowHelper_RunRethrow
		{ 1u, 0u },  // SnapshotTestFixtures_SealedClassVirtualHelper_RunSealedVirtual
		{ 0u, 1u },  // SnapshotTestFixtures_SealedHelper__ctor_System_Int32
		{ 1u, 0u },  // SnapshotTestFixtures_SealedHelper_GetValueVirtual
		{ 1u, 0u },  // SnapshotTestFixtures_ShiftHelper_RunShift
		{ 1u, 0u },  // SnapshotTestFixtures_SimpleGapsHelper_RunConvI
		{ 1u, 1u },  // SnapshotTestFixtures_SimpleGapsHelper_RunConvOvfI_System_Int32
		{ 1u, 1u },  // SnapshotTestFixtures_SimpleGapsHelper_RunConvOvfIUn_System_Int32
		{ 1u, 1u },  // SnapshotTestFixtures_SimpleGapsHelper_RunConvOvfU_System_Int32
		{ 1u, 1u },  // SnapshotTestFixtures_SimpleGapsHelper_RunConvOvfUUn_System_Int32
		{ 1u, 0u },  // SnapshotTestFixtures_SimpleGapsHelper_RunConvU4
		{ 1u, 0u },  // SnapshotTestFixtures_SimpleGapsHelper_RunLdnull
		{ 1u, 0u },  // SnapshotTestFixtures_SizeOfHelper_GetSize
		{ 1u, 0u },  // SnapshotTestFixtures_StaticFieldWriteHelper_WriteAndRead
		{ 1u, 0u },  // SnapshotTestFixtures_StelemAllHelper_TestAllElems
		{ 1u, 0u },  // SnapshotTestFixtures_StindNarrowHelper_RunStindNarrow
		{ 1u, 0u },  // SnapshotTestFixtures_StindWideHelper_RunStindWide
		{ 1u, 0u },  // SnapshotTestFixtures_StringConcatHelper_TestConcat
		{ 1u, 0u },  // SnapshotTestFixtures_StringFormatHelper_TestFormatOne
		{ 1u, 0u },  // SnapshotTestFixtures_StringOps_UseString
		{ 1u, 0u },  // SnapshotTestFixtures_SwitchHelper_Classify
		{ 1u, 0u },  // SnapshotTestFixtures_TargetHelper_GetValue
		{ 1u, 1u },  // SnapshotTestFixtures_ThrowHelper_CheckPositive_System_Int32
		{ 1u, 0u },  // SnapshotTestFixtures_TypeCheckHelper_CheckAndCast
		{ 1u, 0u },  // SnapshotTestFixtures_UnboxHelper_RunUnbox
		{ 9u, 1u },  // SnapshotTestFixtures_UnsignedOpsHelper_TestCkfinite_System_Double
		{ 1u, 0u },  // SnapshotTestFixtures_UnsignedOpsHelper_TestCltUn
		{ 1u, 0u },  // SnapshotTestFixtures_UnsignedOpsHelper_TestDivUn
		{ 1u, 0u },  // SnapshotTestFixtures_UnsignedOpsHelper_TestRemUn
		{ 1u, 0u },  // SnapshotTestFixtures_ValueTypeHelper_RunValueType
		{ 1u, 0u },  // SnapshotTestFixtures_VirtualDispatchHelper_UseVirtualDispatch
		{ 1u, 0u },  // SnapshotTestFixtures_VirtualHelper_UseVirtual
		{ 1u, 0u },  // SnapshotTestFixtures_VoidCaller_DoNothing
		{ 0u, 1u },  // SnapshotTestFixtures_Wrapper_1__ctor_0
		{ 2u, 0u },  // SnapshotTestFixtures_Wrapper_1_GetValue
		{ 0u, 1u },  // SnapshotTestFixtures_Wrapper__0__ctor_0
		{ 2u, 0u },  // SnapshotTestFixtures_Wrapper__0_GetValue
		{ 0u, 1u },  // SnapshotTestFixtures_Wrapper_System_Int32__ctor_System_Int32
		{ 1u, 0u },  // SnapshotTestFixtures_Wrapper_System_Int32_GetValue
	},
	{
		1u,
		1u,
		2u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		10u,
		8u,
		1u,
		1u,
		1u,
		10u,
		9u,
		1u,
		1u,
		8u,
		1u,
		1u,
		10u,
		9u,
		8u,
		2u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		9u,
		2u,
		2u,
		1u,
	},
};
static const ::ChaosAbiManifestV0* const s_abi_manifest =
	reinterpret_cast<const ::ChaosAbiManifestV0*>(&s_abi_manifest_storage);
// ── Module registration ──

	static constexpr CHAOS_IL2CPP_UINT32 s_type_flags[2] = {
		0u,
		1548u,
	};

	static constexpr const char* s_type_names[2] = {
		"<Module>",
		"SnapshotProverSubjects",
	};

	static constexpr const char* s_type_namespaces[2] = {
		"",
		"",
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_type_parent_tokens[2] = {
		0u,
		0u,
	};

	static const TypeInfoHot* const s_type_info_ptrs[2] = {
		nullptr,
		&chaos_mt_SnapshotProverSubjects_SnapshotProverSubjects.hot,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_offset[3] = {
		0u,
		0u,
		0u,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_generic_param_constraint_offset[3] = {
		0u,
		0u,
		0u,
	};



	static const ::chaos::il2cpp::runtime_core::ModuleDescriptor s_native_aot_module = {
		/* .name_utf8         = */ "SnapshotProverSubjects",
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
		/* .type_count        = */ 2u,
	/* .custom_attribute_blob       = */ nullptr,  // Tier 1 — deferred
	/* .custom_attribute_offset     = */ nullptr,
	/* .custom_attribute_entity_count = */ 0u,
	/* .custom_attribute_materializer = */ nullptr,

		/* .abi_manifest      = */ s_abi_manifest,
	};
	static const CHAOS_IL2CPP_UINT32 s_native_aot_module_id =
		::chaos::il2cpp::runtime_core::RegisterModule("SnapshotProverSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[157] = {
	{ "RunAddress", 0x000000BAu, 0u },  // AddressHelper
	{ "RunCompare", 0x000000A5u, 0u },  // ArithmeticCompareHelper
	{ "RunAdd", 0x0000007Au, 0u },  // ArithmeticOps
	{ "MakeAndFill", 0x00000096u, 0u },  // ArrayHelper
	{ "RunLength", 0x000000ADu, 0u },  // ArrayLengthHelper
	{ "RunArrayRef", 0x000000C7u, 0u },  // ArrayRefHelper
	{ ".ctor", 0x0000009Du, 0u },  // BaseClass
	{ "Compute", 0x0000009Cu, 0u },  // BaseClass
	{ "RunBitwise", 0x000000ABu, 0u },  // BitwiseHelper
	{ "DemoBoxStore", 0x00000109u, 0u },  // BoxInterfaceArrayDemo
	{ "BoxAndUnbox", 0x0000008Au, 0u },  // BoxingHelper
	{ "RunBranchCompareB", 0x000000B8u, 0u },  // BranchCompareBHelper
	{ "RunBranchCompare", 0x000000B1u, 0u },  // BranchCompareHelper
	{ "RunBranchDup", 0x000000A6u, 0u },  // BranchDupHelper
	{ "RunBranchUnsignedB", 0x000000E2u, 0u },  // BranchUnsignedBHelper
	{ "RunBranchUnsigned", 0x000000E1u, 0u },  // BranchUnsignedHelper
	{ "CreateAndUse", 0x00000088u, 0u },  // CallVirtHelper
	{ "RunCalli", 0x000000D3u, 1u },  // CalliHelper
	{ "RunCallvirt", 0x000000C0u, 0u },  // CallvirtHelper
	{ "TestDict", 0x00000102u, 0u },  // CollectionsHelper
	{ "TestList", 0x00000101u, 0u },  // CollectionsHelper
	{ "TestSet", 0x00000103u, 0u },  // CollectionsHelper
	{ "RunConvSmall", 0x000000B6u, 0u },  // ConvSmallIntHelper
	{ "RunConvWide", 0x000000B3u, 0u },  // ConvWideHelper
	{ "ConvertToInt", 0x0000008Eu, 0u },  // ConversionHelper
	{ "RunCpblk", 0x000000C2u, 0u },  // CpblkHelper
	{ "RunCpobj", 0x000000BEu, 0u },  // CpobjHelper
	{ "RunDelegate", 0x0000009Bu, 0u },  // DelegateHelper
	{ ".ctor", 0x0000009Fu, 0u },  // DerivedClass
	{ "Compute", 0x0000009Eu, 0u },  // DerivedClass
	{ "SafeDivide", 0x00000097u, 0u },  // EhHelper
	{ "CallExternal", 0x00000104u, 0u },  // ExternalCaller
	{ "GetAndIncrement", 0x0000008Cu, 0u },  // FieldHelper
	{ "RunFloatOps", 0x000000B2u, 0u },  // FloatOpsHelper
	{ "IsPositive", 0x00000082u, 0u },  // FlowControl
	{ "UseGeneric", 0x00000098u, 0u },  // GenericHelper
	{ "DemoCombine", 0x00000108u, 0u },  // GenericsVirtEhDemo
	{ ".ctor", 0x000000B9u, 0u },  // HasFields
	{ ".ctor", 0x000000A7u, 1u },  // HasInstanceFields
	{ "ConsumeString", 0x00000091u, 1u },  // Helper
	{ "GetValue", 0x00000092u, 0u },  // Helper
	{ "Nop", 0x0000008Fu, 0u },  // Helper
	{ "Square", 0x00000090u, 1u },  // Helper
	{ "Run", 0x0000010Bu, 0u },  // HotUpdateWithGenericsDemo
	{ "Run", 0x0000010Au, 0u },  // HotUpdateWithTypesDemo
	{ ".ctor", 0x00000110u, 1u },  // ImplHelper
	{ "GetValue", 0x00000111u, 0u },  // ImplHelper
	{ "ReadWriteRef", 0x000000A4u, 0u },  // IndirectHelper
	{ "CreateAndUseFields", 0x000000A8u, 0u },  // InstanceFieldHelper
	{ ".ctor", 0x00000084u, 1u },  // InstanceHelper
	{ "GetValue", 0x00000086u, 0u },  // InstanceHelper
	{ "RunInterfaceTest", 0x00000112u, 0u },  // InterfaceDevirtHelper
	{ "Double", 0x00000106u, 1u },  // InternalHelper
	{ "RunLdVirtftn", 0x000000D6u, 0u },  // LdVirtftnHelper
	{ "RunLdarga", 0x000000BDu, 1u },  // LdargaHelper
	{ "RunLdcI8", 0x000000B5u, 0u },  // LdcI8Helper
	{ "TestAllElems", 0x000000D7u, 0u },  // LdelemAllHelper
	{ "RunLdelema", 0x000000BCu, 0u },  // LdelemaHelper
	{ "GetFnPtr", 0x00000094u, 0u },  // LdftnHelper
	{ "RunLdindI1", 0x000000B7u, 0u },  // LdindI1Helper
	{ "RunLdindI8", 0x000000C4u, 0u },  // LdindI8Helper
	{ "RunLdindI", 0x000000FFu, 0u },  // LdindStindGapsHelper
	{ "RunLdindI4", 0x000000FEu, 0u },  // LdindStindGapsHelper
	{ "RunStindI", 0x00000100u, 0u },  // LdindStindGapsHelper
	{ "RunLdindUnsigned", 0x000000C3u, 0u },  // LdindUnsignedHelper
	{ "RunLdindWide", 0x000000C5u, 0u },  // LdindWideHelper
	{ "RunLdtoken", 0x000000BBu, 0u },  // LdtokenHelper
	{ "RunAlloc", 0x000000AFu, 0u },  // LocalAllocHelper
	{ "SumToFive", 0x0000008Du, 0u },  // LoopHelper
	{ ".ctor", 0x000000A1u, 0u },  // MarkedClass
	{ "RunSquare", 0x00000080u, 0u },  // MathHelper
	{ "RunArglist", 0x000000E7u, 0u },  // MoreRareOpsHelper
	{ "RunJmp", 0x000000EBu, 0u },  // MoreRareOpsHelper
	{ "RunMkrefany", 0x000000E8u, 0u },  // MoreRareOpsHelper
	{ "RunRefanytype", 0x000000EAu, 0u },  // MoreRareOpsHelper
	{ "RunRefanyval", 0x000000E9u, 0u },  // MoreRareOpsHelper
	{ "RunMulDiv", 0x000000B0u, 0u },  // MulDivHelper
	{ "DispatchBoth", 0x00000105u, 0u },  // MultiDispatcher
	{ ".ctor", 0x000000D4u, 1u },  // MyClass
	{ "GetValue", 0x000000D5u, 0u },  // MyClass
	{ "RunNegShift", 0x000000B4u, 0u },  // NegShiftHelper
	{ "ConvOvfI1", 0x000000ECu, 1u },  // OverflowConvExtHelper
	{ "ConvOvfI1Un", 0x000000EDu, 1u },  // OverflowConvExtHelper
	{ "ConvOvfI2Un", 0x000000EFu, 1u },  // OverflowConvExtHelper
	{ "ConvOvfI4Un", 0x000000F1u, 1u },  // OverflowConvExtHelper
	{ "ConvOvfI8Un", 0x000000F3u, 1u },  // OverflowConvExtHelper
	{ "ConvOvfIUn", 0x000000F5u, 1u },  // OverflowConvExtHelper
	{ "ConvOvfU1Un", 0x000000EEu, 1u },  // OverflowConvExtHelper
	{ "ConvOvfU2Un", 0x000000F0u, 1u },  // OverflowConvExtHelper
	{ "ConvOvfU4Un", 0x000000F2u, 1u },  // OverflowConvExtHelper
	{ "ConvOvfU8Un", 0x000000F4u, 1u },  // OverflowConvExtHelper
	{ "ConvOvfUUn", 0x000000F6u, 1u },  // OverflowConvExtHelper
	{ "ConvOvfI2", 0x000000CEu, 1u },  // OverflowConvHelper
	{ "ConvOvfI4", 0x000000CCu, 0u },  // OverflowConvHelper
	{ "ConvOvfI8", 0x000000D1u, 1u },  // OverflowConvHelper
	{ "ConvOvfU1", 0x000000CDu, 1u },  // OverflowConvHelper
	{ "ConvOvfU2", 0x000000CFu, 1u },  // OverflowConvHelper
	{ "ConvOvfU4", 0x000000D0u, 1u },  // OverflowConvHelper
	{ "ConvOvfU8", 0x000000D2u, 1u },  // OverflowConvHelper
	{ "RunOverflowAdd", 0x000000C9u, 0u },  // OverflowHelper
	{ "RunOverflowMul", 0x000000CBu, 0u },  // OverflowHelper
	{ "RunOverflowSub", 0x000000CAu, 0u },  // OverflowHelper
	{ "RunOverflowAddUn", 0x000000E4u, 0u },  // OverflowUnHelper
	{ "RunOverflowMulUn", 0x000000E6u, 0u },  // OverflowUnHelper
	{ "RunOverflowSubUn", 0x000000E5u, 0u },  // OverflowUnHelper
	{ "TestConvRUn", 0x000000DFu, 1u },  // RareOpsHelper
	{ "TestInitBlk", 0x000000DDu, 1u },  // RareOpsHelper
	{ "TestStarg", 0x000000DEu, 1u },  // RareOpsHelper
	{ "RunRethrow", 0x000000E3u, 0u },  // RethrowHelper
	{ "RunSealedVirtual", 0x0000010Eu, 0u },  // SealedClassVirtualHelper
	{ ".ctor", 0x0000010Cu, 1u },  // SealedHelper
	{ "GetValueVirtual", 0x0000010Du, 0u },  // SealedHelper
	{ "RunShift", 0x000000ACu, 0u },  // ShiftHelper
	{ "RunConvI", 0x000000F9u, 0u },  // SimpleGapsHelper
	{ "RunConvOvfI", 0x000000FAu, 1u },  // SimpleGapsHelper
	{ "RunConvOvfIUn", 0x000000FCu, 1u },  // SimpleGapsHelper
	{ "RunConvOvfU", 0x000000FBu, 1u },  // SimpleGapsHelper
	{ "RunConvOvfUUn", 0x000000FDu, 1u },  // SimpleGapsHelper
	{ "RunConvU4", 0x000000F8u, 0u },  // SimpleGapsHelper
	{ "RunLdnull", 0x000000F7u, 0u },  // SimpleGapsHelper
	{ "GetSize", 0x000000A3u, 0u },  // SizeOfHelper
	{ "Subject_0", 0x00000079u, 0u },  // SnapshotProverSubjects
	{ "Subject_1", 0x0000007Bu, 0u },  // SnapshotProverSubjects
	{ "Subject_2", 0x0000007Du, 0u },  // SnapshotProverSubjects
	{ "Subject_3", 0x0000007Fu, 0u },  // SnapshotProverSubjects
	{ "Subject_4", 0x00000081u, 0u },  // SnapshotProverSubjects
	{ "Subject_5", 0x00000083u, 0u },  // SnapshotProverSubjects
	{ "Subject_6", 0x00000085u, 0u },  // SnapshotProverSubjects
	{ "Subject_7", 0x00000087u, 0u },  // SnapshotProverSubjects
	{ "Subject_8", 0x00000089u, 0u },  // SnapshotProverSubjects
	{ "Subject_9", 0x0000008Bu, 0u },  // SnapshotProverSubjects
	{ "WriteAndRead", 0x000000A9u, 0u },  // StaticFieldWriteHelper
	{ "TestAllElems", 0x000000D8u, 0u },  // StelemAllHelper
	{ "RunStindNarrow", 0x000000BFu, 0u },  // StindNarrowHelper
	{ "RunStindWide", 0x000000C6u, 0u },  // StindWideHelper
	{ "TestConcat", 0x00000107u, 0u },  // StringConcatHelper
	{ "TestFormatOne", 0x000000E0u, 0u },  // StringFormatHelper
	{ "UseString", 0x0000007Cu, 0u },  // StringOps
	{ "Classify", 0x00000093u, 0u },  // SwitchHelper
	{ "GetValue", 0x00000095u, 0u },  // TargetHelper
	{ "CheckPositive", 0x000000C8u, 1u },  // ThrowHelper
	{ "CheckAndCast", 0x000000A2u, 0u },  // TypeCheckHelper
	{ "RunUnbox", 0x000000C1u, 0u },  // UnboxHelper
	{ "TestCkfinite", 0x000000DCu, 1u },  // UnsignedOpsHelper
	{ "TestCltUn", 0x000000D9u, 0u },  // UnsignedOpsHelper
	{ "TestDivUn", 0x000000DAu, 0u },  // UnsignedOpsHelper
	{ "TestRemUn", 0x000000DBu, 0u },  // UnsignedOpsHelper
	{ "RunValueType", 0x000000AEu, 0u },  // ValueTypeHelper
	{ "UseVirtualDispatch", 0x000000AAu, 0u },  // VirtualDispatchHelper
	{ "UseVirtual", 0x000000A0u, 0u },  // VirtualHelper
	{ "DoNothing", 0x0000007Eu, 0u },  // VoidCaller
	{ ".ctor", 0x00000115u, 1u },  // Wrapper<!0>
	{ "GetValue", 0x00000116u, 0u },  // Wrapper<!0>
	{ ".ctor", 0x00000099u, 1u },  // Wrapper`1
	{ "GetValue", 0x0000009Au, 0u },  // Wrapper`1
	{ ".ctor", 0x00000113u, 1u },  // Wrapper<System.Int32>
	{ "GetValue", 0x00000114u, 0u },  // Wrapper<System.Int32>
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[97] = {
	{ "AddressHelper", "", 0u, 1u },
	{ "ArithmeticCompareHelper", "", 1u, 1u },
	{ "ArithmeticOps", "", 2u, 1u },
	{ "ArrayHelper", "", 3u, 1u },
	{ "ArrayLengthHelper", "", 4u, 1u },
	{ "ArrayRefHelper", "", 5u, 1u },
	{ "BaseClass", "", 6u, 2u },
	{ "BitwiseHelper", "", 8u, 1u },
	{ "BoxInterfaceArrayDemo", "", 9u, 1u },
	{ "BoxingHelper", "", 10u, 1u },
	{ "BranchCompareBHelper", "", 11u, 1u },
	{ "BranchCompareHelper", "", 12u, 1u },
	{ "BranchDupHelper", "", 13u, 1u },
	{ "BranchUnsignedBHelper", "", 14u, 1u },
	{ "BranchUnsignedHelper", "", 15u, 1u },
	{ "CallVirtHelper", "", 16u, 1u },
	{ "CalliHelper", "", 17u, 1u },
	{ "CallvirtHelper", "", 18u, 1u },
	{ "CollectionsHelper", "", 19u, 3u },
	{ "ConvSmallIntHelper", "", 22u, 1u },
	{ "ConvWideHelper", "", 23u, 1u },
	{ "ConversionHelper", "", 24u, 1u },
	{ "CpblkHelper", "", 25u, 1u },
	{ "CpobjHelper", "", 26u, 1u },
	{ "DelegateHelper", "", 27u, 1u },
	{ "DerivedClass", "", 28u, 2u },
	{ "EhHelper", "", 30u, 1u },
	{ "ExternalCaller", "", 31u, 1u },
	{ "FieldHelper", "", 32u, 1u },
	{ "FloatOpsHelper", "", 33u, 1u },
	{ "FlowControl", "", 34u, 1u },
	{ "GenericHelper", "", 35u, 1u },
	{ "GenericsVirtEhDemo", "", 36u, 1u },
	{ "HasFields", "", 37u, 1u },
	{ "HasInstanceFields", "", 38u, 1u },
	{ "Helper", "", 39u, 4u },
	{ "HotUpdateWithGenericsDemo", "", 43u, 1u },
	{ "HotUpdateWithTypesDemo", "", 44u, 1u },
	{ "ImplHelper", "", 45u, 2u },
	{ "IndirectHelper", "", 47u, 1u },
	{ "InstanceFieldHelper", "", 48u, 1u },
	{ "InstanceHelper", "", 49u, 2u },
	{ "InterfaceDevirtHelper", "", 51u, 1u },
	{ "InternalHelper", "", 52u, 1u },
	{ "LdVirtftnHelper", "", 53u, 1u },
	{ "LdargaHelper", "", 54u, 1u },
	{ "LdcI8Helper", "", 55u, 1u },
	{ "LdelemAllHelper", "", 56u, 1u },
	{ "LdelemaHelper", "", 57u, 1u },
	{ "LdftnHelper", "", 58u, 1u },
	{ "LdindI1Helper", "", 59u, 1u },
	{ "LdindI8Helper", "", 60u, 1u },
	{ "LdindStindGapsHelper", "", 61u, 3u },
	{ "LdindUnsignedHelper", "", 64u, 1u },
	{ "LdindWideHelper", "", 65u, 1u },
	{ "LdtokenHelper", "", 66u, 1u },
	{ "LocalAllocHelper", "", 67u, 1u },
	{ "LoopHelper", "", 68u, 1u },
	{ "MarkedClass", "", 69u, 1u },
	{ "MathHelper", "", 70u, 1u },
	{ "MoreRareOpsHelper", "", 71u, 5u },
	{ "MulDivHelper", "", 76u, 1u },
	{ "MultiDispatcher", "", 77u, 1u },
	{ "MyClass", "", 78u, 2u },
	{ "NegShiftHelper", "", 80u, 1u },
	{ "OverflowConvExtHelper", "", 81u, 11u },
	{ "OverflowConvHelper", "", 92u, 7u },
	{ "OverflowHelper", "", 99u, 3u },
	{ "OverflowUnHelper", "", 102u, 3u },
	{ "RareOpsHelper", "", 105u, 3u },
	{ "RethrowHelper", "", 108u, 1u },
	{ "SealedClassVirtualHelper", "", 109u, 1u },
	{ "SealedHelper", "", 110u, 2u },
	{ "ShiftHelper", "", 112u, 1u },
	{ "SimpleGapsHelper", "", 113u, 7u },
	{ "SizeOfHelper", "", 120u, 1u },
	{ "SnapshotProverSubjects", "", 121u, 10u },
	{ "StaticFieldWriteHelper", "", 131u, 1u },
	{ "StelemAllHelper", "", 132u, 1u },
	{ "StindNarrowHelper", "", 133u, 1u },
	{ "StindWideHelper", "", 134u, 1u },
	{ "StringConcatHelper", "", 135u, 1u },
	{ "StringFormatHelper", "", 136u, 1u },
	{ "StringOps", "", 137u, 1u },
	{ "SwitchHelper", "", 138u, 1u },
	{ "TargetHelper", "", 139u, 1u },
	{ "ThrowHelper", "", 140u, 1u },
	{ "TypeCheckHelper", "", 141u, 1u },
	{ "UnboxHelper", "", 142u, 1u },
	{ "UnsignedOpsHelper", "", 143u, 4u },
	{ "ValueTypeHelper", "", 147u, 1u },
	{ "VirtualDispatchHelper", "", 148u, 1u },
	{ "VirtualHelper", "", 149u, 1u },
	{ "VoidCaller", "", 150u, 1u },
	{ "Wrapper<!0>", "", 151u, 2u },
	{ "Wrapper`1", "", 153u, 2u },
	{ "Wrapper<System.Int32>", "Wrapper<System", 155u, 2u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[157] = {
	{ 0x00000079u, 0u },
	{ 0x0000007Au, 12u },
	{ 0x0000007Bu, 1u },
	{ 0x0000007Cu, 137u },
	{ 0x0000007Du, 2u },
	{ 0x0000007Eu, 150u },
	{ 0x0000007Fu, 3u },
	{ 0x00000080u, 80u },
	{ 0x00000081u, 4u },
	{ 0x00000082u, 44u },
	{ 0x00000083u, 5u },
	{ 0x00000084u, 59u },
	{ 0x00000085u, 6u },
	{ 0x00000086u, 60u },
	{ 0x00000087u, 7u },
	{ 0x00000088u, 27u },
	{ 0x00000089u, 8u },
	{ 0x0000008Au, 19u },
	{ 0x0000008Bu, 9u },
	{ 0x0000008Cu, 42u },
	{ 0x0000008Du, 78u },
	{ 0x0000008Eu, 32u },
	{ 0x0000008Fu, 51u },
	{ 0x00000090u, 52u },
	{ 0x00000091u, 49u },
	{ 0x00000092u, 50u },
	{ 0x00000093u, 138u },
	{ 0x00000094u, 67u },
	{ 0x00000095u, 139u },
	{ 0x00000096u, 13u },
	{ 0x00000097u, 40u },
	{ 0x00000098u, 45u },
	{ 0x00000099u, 151u },
	{ 0x0000009Au, 152u },
	{ 0x0000009Bu, 37u },
	{ 0x0000009Cu, 17u },
	{ 0x0000009Du, 16u },
	{ 0x0000009Eu, 39u },
	{ 0x0000009Fu, 38u },
	{ 0x000000A0u, 149u },
	{ 0x000000A1u, 79u },
	{ 0x000000A2u, 141u },
	{ 0x000000A3u, 130u },
	{ 0x000000A4u, 57u },
	{ 0x000000A5u, 11u },
	{ 0x000000A6u, 23u },
	{ 0x000000A7u, 48u },
	{ 0x000000A8u, 58u },
	{ 0x000000A9u, 131u },
	{ 0x000000AAu, 148u },
	{ 0x000000ABu, 18u },
	{ 0x000000ACu, 122u },
	{ 0x000000ADu, 14u },
	{ 0x000000AEu, 147u },
	{ 0x000000AFu, 77u },
	{ 0x000000B0u, 86u },
	{ 0x000000B1u, 22u },
	{ 0x000000B2u, 43u },
	{ 0x000000B3u, 34u },
	{ 0x000000B4u, 90u },
	{ 0x000000B5u, 64u },
	{ 0x000000B6u, 33u },
	{ 0x000000B7u, 68u },
	{ 0x000000B8u, 21u },
	{ 0x000000B9u, 47u },
	{ 0x000000BAu, 10u },
	{ 0x000000BBu, 75u },
	{ 0x000000BCu, 65u },
	{ 0x000000BDu, 63u },
	{ 0x000000BEu, 36u },
	{ 0x000000BFu, 133u },
	{ 0x000000C0u, 28u },
	{ 0x000000C1u, 142u },
	{ 0x000000C2u, 35u },
	{ 0x000000C3u, 73u },
	{ 0x000000C4u, 69u },
	{ 0x000000C5u, 74u },
	{ 0x000000C6u, 134u },
	{ 0x000000C7u, 15u },
	{ 0x000000C8u, 140u },
	{ 0x000000C9u, 109u },
	{ 0x000000CAu, 111u },
	{ 0x000000CBu, 110u },
	{ 0x000000CCu, 103u },
	{ 0x000000CDu, 105u },
	{ 0x000000CEu, 102u },
	{ 0x000000CFu, 106u },
	{ 0x000000D0u, 107u },
	{ 0x000000D1u, 104u },
	{ 0x000000D2u, 108u },
	{ 0x000000D3u, 26u },
	{ 0x000000D4u, 88u },
	{ 0x000000D5u, 89u },
	{ 0x000000D6u, 76u },
	{ 0x000000D7u, 66u },
	{ 0x000000D8u, 132u },
	{ 0x000000D9u, 144u },
	{ 0x000000DAu, 145u },
	{ 0x000000DBu, 146u },
	{ 0x000000DCu, 143u },
	{ 0x000000DDu, 116u },
	{ 0x000000DEu, 117u },
	{ 0x000000DFu, 115u },
	{ 0x000000E0u, 136u },
	{ 0x000000E1u, 25u },
	{ 0x000000E2u, 24u },
	{ 0x000000E3u, 118u },
	{ 0x000000E4u, 112u },
	{ 0x000000E5u, 114u },
	{ 0x000000E6u, 113u },
	{ 0x000000E7u, 81u },
	{ 0x000000E8u, 83u },
	{ 0x000000E9u, 85u },
	{ 0x000000EAu, 84u },
	{ 0x000000EBu, 82u },
	{ 0x000000ECu, 91u },
	{ 0x000000EDu, 92u },
	{ 0x000000EEu, 97u },
	{ 0x000000EFu, 93u },
	{ 0x000000F0u, 98u },
	{ 0x000000F1u, 94u },
	{ 0x000000F2u, 99u },
	{ 0x000000F3u, 95u },
	{ 0x000000F4u, 100u },
	{ 0x000000F5u, 96u },
	{ 0x000000F6u, 101u },
	{ 0x000000F7u, 129u },
	{ 0x000000F8u, 128u },
	{ 0x000000F9u, 123u },
	{ 0x000000FAu, 124u },
	{ 0x000000FBu, 126u },
	{ 0x000000FCu, 125u },
	{ 0x000000FDu, 127u },
	{ 0x000000FEu, 71u },
	{ 0x000000FFu, 70u },
	{ 0x00000100u, 72u },
	{ 0x00000101u, 30u },
	{ 0x00000102u, 29u },
	{ 0x00000103u, 31u },
	{ 0x00000104u, 41u },
	{ 0x00000105u, 87u },
	{ 0x00000106u, 62u },
	{ 0x00000107u, 135u },
	{ 0x00000108u, 46u },
	{ 0x00000109u, 20u },
	{ 0x0000010Au, 54u },
	{ 0x0000010Bu, 53u },
	{ 0x0000010Cu, 120u },
	{ 0x0000010Du, 121u },
	{ 0x0000010Eu, 119u },
	{ 0x00000110u, 55u },
	{ 0x00000111u, 56u },
	{ 0x00000112u, 61u },
	{ 0x00000113u, 155u },
	{ 0x00000114u, 156u },
	{ 0x00000115u, 153u },
	{ 0x00000116u, 154u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[157] = {
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // SnapshotProverSubjects::Subject_0
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // SnapshotProverSubjects::Subject_1
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // SnapshotProverSubjects::Subject_2
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // SnapshotProverSubjects::Subject_3
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // SnapshotProverSubjects::Subject_4
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // SnapshotProverSubjects::Subject_5
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // SnapshotProverSubjects::Subject_6
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // SnapshotProverSubjects::Subject_7
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // SnapshotProverSubjects::Subject_8
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_9), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // SnapshotProverSubjects::Subject_9
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_AddressHelper_RunAddress), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AddressHelper::RunAddress
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_ArithmeticCompareHelper_RunCompare), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ArithmeticCompareHelper::RunCompare
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_ArithmeticOps_RunAdd), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ArithmeticOps::RunAdd
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_ArrayHelper_MakeAndFill), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ArrayHelper::MakeAndFill
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_ArrayLengthHelper_RunLength), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ArrayLengthHelper::RunLength
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_ArrayRefHelper_RunArrayRef), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ArrayRefHelper::RunArrayRef
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_BaseClass__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // BaseClass::.ctor
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_BaseClass_Compute), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // BaseClass::Compute
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_BitwiseHelper_RunBitwise), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // BitwiseHelper::RunBitwise
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_BoxingHelper_BoxAndUnbox), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // BoxingHelper::BoxAndUnbox
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_BoxInterfaceArrayDemo_DemoBoxStore), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // BoxInterfaceArrayDemo::DemoBoxStore
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_BranchCompareBHelper_RunBranchCompareB), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // BranchCompareBHelper::RunBranchCompareB
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_BranchCompareHelper_RunBranchCompare), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // BranchCompareHelper::RunBranchCompare
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_BranchDupHelper_RunBranchDup), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // BranchDupHelper::RunBranchDup
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_BranchUnsignedBHelper_RunBranchUnsignedB), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // BranchUnsignedBHelper::RunBranchUnsignedB
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_BranchUnsignedHelper_RunBranchUnsigned), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // BranchUnsignedHelper::RunBranchUnsigned
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_CalliHelper_RunCalli_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // CalliHelper::RunCalli
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_CallVirtHelper_CreateAndUse), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // CallVirtHelper::CreateAndUse
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_CallvirtHelper_RunCallvirt), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // CallvirtHelper::RunCallvirt
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_CollectionsHelper_TestDict), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // CollectionsHelper::TestDict
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_CollectionsHelper_TestList), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // CollectionsHelper::TestList
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_CollectionsHelper_TestSet), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // CollectionsHelper::TestSet
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_ConversionHelper_ConvertToInt), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ConversionHelper::ConvertToInt
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_ConvSmallIntHelper_RunConvSmall), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ConvSmallIntHelper::RunConvSmall
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_ConvWideHelper_RunConvWide), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ConvWideHelper::RunConvWide
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_CpblkHelper_RunCpblk), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // CpblkHelper::RunCpblk
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_CpobjHelper_RunCpobj), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // CpobjHelper::RunCpobj
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_DelegateHelper_RunDelegate), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // DelegateHelper::RunDelegate
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_DerivedClass__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // DerivedClass::.ctor
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_DerivedClass_Compute), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // DerivedClass::Compute
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_EhHelper_SafeDivide), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // EhHelper::SafeDivide
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_ExternalCaller_CallExternal), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ExternalCaller::CallExternal
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_FieldHelper_GetAndIncrement), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // FieldHelper::GetAndIncrement
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_FloatOpsHelper_RunFloatOps), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // FloatOpsHelper::RunFloatOps
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_FlowControl_IsPositive), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // FlowControl::IsPositive
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_GenericHelper_UseGeneric), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GenericHelper::UseGeneric
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_GenericsVirtEhDemo_DemoCombine), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // GenericsVirtEhDemo::DemoCombine
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_HasFields__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // HasFields::.ctor
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_HasInstanceFields__ctor_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // HasInstanceFields::.ctor
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_Helper_ConsumeString_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // Helper::ConsumeString
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_Helper_GetValue), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // Helper::GetValue
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_Helper_Nop), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // Helper::Nop
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_Helper_Square_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // Helper::Square
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_HotUpdateWithGenericsDemo_Run), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // HotUpdateWithGenericsDemo::Run
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_HotUpdateWithTypesDemo_Run), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // HotUpdateWithTypesDemo::Run
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_ImplHelper__ctor_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ImplHelper::.ctor
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_ImplHelper_GetValue), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ImplHelper::GetValue
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_IndirectHelper_ReadWriteRef), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // IndirectHelper::ReadWriteRef
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_InstanceFieldHelper_CreateAndUseFields), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // InstanceFieldHelper::CreateAndUseFields
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_InstanceHelper__ctor_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // InstanceHelper::.ctor
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_InstanceHelper_GetValue), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // InstanceHelper::GetValue
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_InterfaceDevirtHelper_RunInterfaceTest), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // InterfaceDevirtHelper::RunInterfaceTest
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_InternalHelper_Double_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // InternalHelper::Double
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_LdargaHelper_RunLdarga_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // LdargaHelper::RunLdarga
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_LdcI8Helper_RunLdcI8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // LdcI8Helper::RunLdcI8
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_LdelemaHelper_RunLdelema), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // LdelemaHelper::RunLdelema
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_LdelemAllHelper_TestAllElems), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // LdelemAllHelper::TestAllElems
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_LdftnHelper_GetFnPtr), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // LdftnHelper::GetFnPtr
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_LdindI1Helper_RunLdindI1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // LdindI1Helper::RunLdindI1
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_LdindI8Helper_RunLdindI8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // LdindI8Helper::RunLdindI8
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_LdindStindGapsHelper_RunLdindI), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // LdindStindGapsHelper::RunLdindI
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_LdindStindGapsHelper_RunLdindI4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // LdindStindGapsHelper::RunLdindI4
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_LdindStindGapsHelper_RunStindI), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // LdindStindGapsHelper::RunStindI
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_LdindUnsignedHelper_RunLdindUnsigned), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // LdindUnsignedHelper::RunLdindUnsigned
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_LdindWideHelper_RunLdindWide), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // LdindWideHelper::RunLdindWide
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_LdtokenHelper_RunLdtoken), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // LdtokenHelper::RunLdtoken
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_LdVirtftnHelper_RunLdVirtftn), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // LdVirtftnHelper::RunLdVirtftn
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_LocalAllocHelper_RunAlloc), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // LocalAllocHelper::RunAlloc
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_LoopHelper_SumToFive), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // LoopHelper::SumToFive
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_MarkedClass__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // MarkedClass::.ctor
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_MathHelper_RunSquare), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // MathHelper::RunSquare
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_MoreRareOpsHelper_RunArglist), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // MoreRareOpsHelper::RunArglist
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_MoreRareOpsHelper_RunJmp), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // MoreRareOpsHelper::RunJmp
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_MoreRareOpsHelper_RunMkrefany), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // MoreRareOpsHelper::RunMkrefany
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_MoreRareOpsHelper_RunRefanytype), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // MoreRareOpsHelper::RunRefanytype
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_MoreRareOpsHelper_RunRefanyval), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // MoreRareOpsHelper::RunRefanyval
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_MulDivHelper_RunMulDiv), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // MulDivHelper::RunMulDiv
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_MultiDispatcher_DispatchBoth), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // MultiDispatcher::DispatchBoth
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_MyClass__ctor_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // MyClass::.ctor
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_MyClass_GetValue), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // MyClass::GetValue
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_NegShiftHelper_RunNegShift), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NegShiftHelper::RunNegShift
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfI1_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // OverflowConvExtHelper::ConvOvfI1
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfI1Un_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // OverflowConvExtHelper::ConvOvfI1Un
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfI2Un_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // OverflowConvExtHelper::ConvOvfI2Un
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfI4Un_System_Int64), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // OverflowConvExtHelper::ConvOvfI4Un
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfI8Un_System_Single), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // OverflowConvExtHelper::ConvOvfI8Un
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfIUn_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // OverflowConvExtHelper::ConvOvfIUn
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfU1Un_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // OverflowConvExtHelper::ConvOvfU1Un
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfU2Un_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // OverflowConvExtHelper::ConvOvfU2Un
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfU4Un_System_Int64), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // OverflowConvExtHelper::ConvOvfU4Un
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfU8Un_System_Double), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // OverflowConvExtHelper::ConvOvfU8Un
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfUUn_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // OverflowConvExtHelper::ConvOvfUUn
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowConvHelper_ConvOvfI2_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // OverflowConvHelper::ConvOvfI2
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowConvHelper_ConvOvfI4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // OverflowConvHelper::ConvOvfI4
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowConvHelper_ConvOvfI8_System_Single), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // OverflowConvHelper::ConvOvfI8
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowConvHelper_ConvOvfU1_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // OverflowConvHelper::ConvOvfU1
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowConvHelper_ConvOvfU2_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // OverflowConvHelper::ConvOvfU2
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowConvHelper_ConvOvfU4_System_Int64), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // OverflowConvHelper::ConvOvfU4
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowConvHelper_ConvOvfU8_System_Double), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // OverflowConvHelper::ConvOvfU8
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowHelper_RunOverflowAdd), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // OverflowHelper::RunOverflowAdd
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowHelper_RunOverflowMul), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // OverflowHelper::RunOverflowMul
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowHelper_RunOverflowSub), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // OverflowHelper::RunOverflowSub
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowUnHelper_RunOverflowAddUn), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // OverflowUnHelper::RunOverflowAddUn
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowUnHelper_RunOverflowMulUn), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // OverflowUnHelper::RunOverflowMulUn
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowUnHelper_RunOverflowSubUn), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // OverflowUnHelper::RunOverflowSubUn
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_RareOpsHelper_TestConvRUn_System_Single), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // RareOpsHelper::TestConvRUn
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_RareOpsHelper_TestInitBlk_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // RareOpsHelper::TestInitBlk
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_RareOpsHelper_TestStarg_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // RareOpsHelper::TestStarg
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_RethrowHelper_RunRethrow), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // RethrowHelper::RunRethrow
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_SealedClassVirtualHelper_RunSealedVirtual), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // SealedClassVirtualHelper::RunSealedVirtual
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_SealedHelper__ctor_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // SealedHelper::.ctor
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_SealedHelper_GetValueVirtual), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // SealedHelper::GetValueVirtual
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_ShiftHelper_RunShift), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ShiftHelper::RunShift
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_SimpleGapsHelper_RunConvI), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // SimpleGapsHelper::RunConvI
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_SimpleGapsHelper_RunConvOvfI_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // SimpleGapsHelper::RunConvOvfI
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_SimpleGapsHelper_RunConvOvfIUn_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // SimpleGapsHelper::RunConvOvfIUn
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_SimpleGapsHelper_RunConvOvfU_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // SimpleGapsHelper::RunConvOvfU
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_SimpleGapsHelper_RunConvOvfUUn_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // SimpleGapsHelper::RunConvOvfUUn
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_SimpleGapsHelper_RunConvU4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // SimpleGapsHelper::RunConvU4
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_SimpleGapsHelper_RunLdnull), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // SimpleGapsHelper::RunLdnull
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_SizeOfHelper_GetSize), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // SizeOfHelper::GetSize
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_StaticFieldWriteHelper_WriteAndRead), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // StaticFieldWriteHelper::WriteAndRead
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_StelemAllHelper_TestAllElems), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // StelemAllHelper::TestAllElems
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_StindNarrowHelper_RunStindNarrow), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // StindNarrowHelper::RunStindNarrow
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_StindWideHelper_RunStindWide), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // StindWideHelper::RunStindWide
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_StringConcatHelper_TestConcat), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // StringConcatHelper::TestConcat
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_StringFormatHelper_TestFormatOne), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // StringFormatHelper::TestFormatOne
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_StringOps_UseString), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // StringOps::UseString
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_SwitchHelper_Classify), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // SwitchHelper::Classify
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_TargetHelper_GetValue), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // TargetHelper::GetValue
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_ThrowHelper_CheckPositive_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ThrowHelper::CheckPositive
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_TypeCheckHelper_CheckAndCast), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // TypeCheckHelper::CheckAndCast
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_UnboxHelper_RunUnbox), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // UnboxHelper::RunUnbox
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_UnsignedOpsHelper_TestCkfinite_System_Double), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // UnsignedOpsHelper::TestCkfinite
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_UnsignedOpsHelper_TestCltUn), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // UnsignedOpsHelper::TestCltUn
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_UnsignedOpsHelper_TestDivUn), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // UnsignedOpsHelper::TestDivUn
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_UnsignedOpsHelper_TestRemUn), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // UnsignedOpsHelper::TestRemUn
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_ValueTypeHelper_RunValueType), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ValueTypeHelper::RunValueType
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_VirtualDispatchHelper_UseVirtualDispatch), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // VirtualDispatchHelper::UseVirtualDispatch
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_VirtualHelper_UseVirtual), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // VirtualHelper::UseVirtual
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_VoidCaller_DoNothing), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // VoidCaller::DoNothing
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_Wrapper_1__ctor_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // Wrapper`1::.ctor
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_Wrapper_1_GetValue), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // Wrapper`1::GetValue
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_Wrapper__0__ctor_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // Wrapper<!0>::.ctor
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_Wrapper__0_GetValue), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // Wrapper<!0>::GetValue
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_Wrapper_System_Int32__ctor_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // Wrapper<System.Int32>::.ctor
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_Wrapper_System_Int32_GetValue), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // Wrapper<System.Int32>::GetValue
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"SnapshotProverSubjects",
	s_hotpatch_types,
	97u,
	s_hotpatch_methods,
	157u,
	s_hotpatch_slots,
	157u,
	s_hotpatch_entries,
	157u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[49] = {
	"SnapshotProverSubjects/SnapshotProverSubjects::_exitCode",
	"SnapshotTestFixtures/HasFields::instanceVal",
	"System.Private.CoreLib/System.Int32",
	"System.Private.CoreLib/System.Object::.ctor:System.Void()",
	"System.Private.CoreLib/System.Func<System.Int32,System.Int32>::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Private.CoreLib/System.Func<System.Int32,System.Int32>::Invoke:System.Int32(System.Int32)",
	"System.Collections/System.Collections.Generic.Dictionary<System.Int32,System.Int32>::.ctor:System.Void()",
	"System.Collections/System.Collections.Generic.Dictionary<System.Int32,System.Int32>::Add:System.Void(System.Int32,System.Int32)",
	"System.Collections/System.Collections.Generic.Dictionary<System.Int32,System.Int32>::TryGetValue:System.Boolean(System.Int32,System.Int32&)",
	"System.Collections/System.Collections.Generic.Dictionary<System.Int32,System.Int32>::ContainsKey:System.Boolean(System.Int32)",
	"System.Collections/System.Collections.Generic.Dictionary<System.Int32,System.Int32>::Remove:System.Boolean(System.Int32)",
	"System.Collections/System.Collections.Generic.List<System.Int32>::.ctor:System.Void()",
	"System.Collections/System.Collections.Generic.List<System.Int32>::Add:System.Void(System.Int32)",
	"System.Collections/System.Collections.Generic.List<System.Int32>::get_Count:System.Int32()",
	"System.Collections/System.Collections.Generic.HashSet<System.Int32>::.ctor:System.Void()",
	"System.Collections/System.Collections.Generic.HashSet<System.Int32>::Add:System.Boolean(System.Int32)",
	"System.Collections/System.Collections.Generic.HashSet<System.Int32>::Contains:System.Boolean(System.Int32)",
	"System.Collections/System.Collections.Generic.HashSet<System.Int32>::Remove:System.Boolean(System.Int32)",
	"System.Private.CoreLib/System.Func<System.Int32>::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Private.CoreLib/System.Func<System.Int32>::Invoke:System.Int32()",
	"SnapshotTestFixtures/FieldHelper::_counter",
	"SnapshotTestFixtures/HasInstanceFields::Value",
	"SnapshotTestFixtures/ImplHelper::_val",
	"SnapshotTestFixtures/InstanceHelper::_val",
	"SnapshotTestFixtures/LdftnHelper+<>O::<0>__GetValue",
	"System.Private.CoreLib/System.SByte",
	"System.Private.CoreLib/System.Int64",
	"System.Private.CoreLib/System.Byte",
	"System.Private.CoreLib/System.UInt16",
	"System.Private.CoreLib/System.UInt32",
	"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)",
	"System.Private.CoreLib/System.Type::get_TypeHandle:System.RuntimeTypeHandle()",
	"System.Private.CoreLib/System.RuntimeTypeHandle::get_Value:System.IntPtr()",
	"System.Private.CoreLib/System.IntPtr::ToInt32:System.Int32()",
	"SnapshotTestFixtures/MyClass::_val",
	"SnapshotTestFixtures/SealedHelper::_val",
	"SnapshotTestFixtures/SmallStruct",
	"SnapshotTestFixtures/StaticFieldWriteHelper::_writeTarget",
	"SnapshotTestFixtures/StaticFieldWriteHelper::_readTarget",
	"System.Private.CoreLib/System.Int16",
	"System.Private.CoreLib/System.Single",
	"System.Private.CoreLib/System.Double",
	"System.Private.CoreLib/System.Object",
	"System.Private.CoreLib/System.String::Format:System.String(System.String,System.Object)",
	"SnapshotTestFixtures/MarkedClass",
	"SnapshotTestFixtures/MarkedClass::Value",
	"System.Private.CoreLib/System.Double::IsFinite:System.Boolean(System.Double)",
	"SnapshotTestFixtures/Wrapper<!0>::_val",
	"SnapshotTestFixtures/Wrapper<System.Int32>::_val",
};

extern "C" void* kChaosExternalRuntimeFnTable[49] = {
	nullptr,
	nullptr,
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__),
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
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_TypeHandle_System_RuntimeTypeHandle__),
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
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_String__Format_System_String_System_String_System_Object_),
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
};

extern "C" int32_t kChaosExternalRuntimeCount = 49;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[157])() = {
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_0),
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_1),
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_2),
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_3),
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_4),
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_5),
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_6),
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_7),
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_8),
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_9),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_AddressHelper_RunAddress),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_ArithmeticCompareHelper_RunCompare),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_ArithmeticOps_RunAdd),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_ArrayHelper_MakeAndFill),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_ArrayLengthHelper_RunLength),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_ArrayRefHelper_RunArrayRef),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_BaseClass__ctor),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_BaseClass_Compute),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_BitwiseHelper_RunBitwise),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_BoxingHelper_BoxAndUnbox),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_BoxInterfaceArrayDemo_DemoBoxStore),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_BranchCompareBHelper_RunBranchCompareB),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_BranchCompareHelper_RunBranchCompare),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_BranchDupHelper_RunBranchDup),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_BranchUnsignedBHelper_RunBranchUnsignedB),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_BranchUnsignedHelper_RunBranchUnsigned),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_CalliHelper_RunCalli_System_Int32),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_CallVirtHelper_CreateAndUse),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_CallvirtHelper_RunCallvirt),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_CollectionsHelper_TestDict),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_CollectionsHelper_TestList),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_CollectionsHelper_TestSet),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_ConversionHelper_ConvertToInt),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_ConvSmallIntHelper_RunConvSmall),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_ConvWideHelper_RunConvWide),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_CpblkHelper_RunCpblk),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_CpobjHelper_RunCpobj),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_DelegateHelper_RunDelegate),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_DerivedClass__ctor),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_DerivedClass_Compute),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_EhHelper_SafeDivide),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_ExternalCaller_CallExternal),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_FieldHelper_GetAndIncrement),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_FloatOpsHelper_RunFloatOps),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_FlowControl_IsPositive),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_GenericHelper_UseGeneric),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_GenericsVirtEhDemo_DemoCombine),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_HasFields__ctor),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_HasInstanceFields__ctor_System_Int32),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_Helper_ConsumeString_System_String),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_Helper_GetValue),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_Helper_Nop),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_Helper_Square_System_Int32),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_HotUpdateWithGenericsDemo_Run),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_HotUpdateWithTypesDemo_Run),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_ImplHelper__ctor_System_Int32),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_ImplHelper_GetValue),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_IndirectHelper_ReadWriteRef),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_InstanceFieldHelper_CreateAndUseFields),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_InstanceHelper__ctor_System_Int32),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_InstanceHelper_GetValue),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_InterfaceDevirtHelper_RunInterfaceTest),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_InternalHelper_Double_System_Int32),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_LdargaHelper_RunLdarga_System_Int32),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_LdcI8Helper_RunLdcI8),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_LdelemaHelper_RunLdelema),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_LdelemAllHelper_TestAllElems),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_LdftnHelper_GetFnPtr),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_LdindI1Helper_RunLdindI1),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_LdindI8Helper_RunLdindI8),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_LdindStindGapsHelper_RunLdindI),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_LdindStindGapsHelper_RunLdindI4),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_LdindStindGapsHelper_RunStindI),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_LdindUnsignedHelper_RunLdindUnsigned),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_LdindWideHelper_RunLdindWide),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_LdtokenHelper_RunLdtoken),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_LdVirtftnHelper_RunLdVirtftn),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_LocalAllocHelper_RunAlloc),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_LoopHelper_SumToFive),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_MarkedClass__ctor),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_MathHelper_RunSquare),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_MoreRareOpsHelper_RunArglist),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_MoreRareOpsHelper_RunJmp),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_MoreRareOpsHelper_RunMkrefany),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_MoreRareOpsHelper_RunRefanytype),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_MoreRareOpsHelper_RunRefanyval),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_MulDivHelper_RunMulDiv),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_MultiDispatcher_DispatchBoth),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_MyClass__ctor_System_Int32),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_MyClass_GetValue),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_NegShiftHelper_RunNegShift),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfI1_System_Int32),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfI1Un_System_Int32),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfI2Un_System_Int32),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfI4Un_System_Int64),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfI8Un_System_Single),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfIUn_System_Int32),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfU1Un_System_Int32),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfU2Un_System_Int32),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfU4Un_System_Int64),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfU8Un_System_Double),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfUUn_System_Int32),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_OverflowConvHelper_ConvOvfI2_System_Int32),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_OverflowConvHelper_ConvOvfI4),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_OverflowConvHelper_ConvOvfI8_System_Single),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_OverflowConvHelper_ConvOvfU1_System_Int32),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_OverflowConvHelper_ConvOvfU2_System_Int32),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_OverflowConvHelper_ConvOvfU4_System_Int64),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_OverflowConvHelper_ConvOvfU8_System_Double),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_OverflowHelper_RunOverflowAdd),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_OverflowHelper_RunOverflowMul),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_OverflowHelper_RunOverflowSub),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_OverflowUnHelper_RunOverflowAddUn),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_OverflowUnHelper_RunOverflowMulUn),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_OverflowUnHelper_RunOverflowSubUn),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_RareOpsHelper_TestConvRUn_System_Single),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_RareOpsHelper_TestInitBlk_System_Int32),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_RareOpsHelper_TestStarg_System_Int32),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_RethrowHelper_RunRethrow),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_SealedClassVirtualHelper_RunSealedVirtual),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_SealedHelper__ctor_System_Int32),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_SealedHelper_GetValueVirtual),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_ShiftHelper_RunShift),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_SimpleGapsHelper_RunConvI),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_SimpleGapsHelper_RunConvOvfI_System_Int32),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_SimpleGapsHelper_RunConvOvfIUn_System_Int32),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_SimpleGapsHelper_RunConvOvfU_System_Int32),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_SimpleGapsHelper_RunConvOvfUUn_System_Int32),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_SimpleGapsHelper_RunConvU4),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_SimpleGapsHelper_RunLdnull),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_SizeOfHelper_GetSize),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_StaticFieldWriteHelper_WriteAndRead),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_StelemAllHelper_TestAllElems),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_StindNarrowHelper_RunStindNarrow),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_StindWideHelper_RunStindWide),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_StringConcatHelper_TestConcat),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_StringFormatHelper_TestFormatOne),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_StringOps_UseString),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_SwitchHelper_Classify),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_TargetHelper_GetValue),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_ThrowHelper_CheckPositive_System_Int32),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_TypeCheckHelper_CheckAndCast),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_UnboxHelper_RunUnbox),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_UnsignedOpsHelper_TestCkfinite_System_Double),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_UnsignedOpsHelper_TestCltUn),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_UnsignedOpsHelper_TestDivUn),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_UnsignedOpsHelper_TestRemUn),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_ValueTypeHelper_RunValueType),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_VirtualDispatchHelper_UseVirtualDispatch),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_VirtualHelper_UseVirtual),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_VoidCaller_DoNothing),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_Wrapper_1__ctor_0),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_Wrapper_1_GetValue),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_Wrapper__0__ctor_0),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_Wrapper__0_GetValue),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_Wrapper_System_Int32__ctor_System_Int32),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_Wrapper_System_Int32_GetValue),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
static void (*kBenchmarkWrappers[157])() = {
	[]() {kAotMethods[0]();},
	[]() {kAotMethods[1]();},
	[]() {kAotMethods[2]();},
	[]() {kAotMethods[3]();},
	[]() {kAotMethods[4]();},
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
	[]() {kAotMethods[15]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[16])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[17])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {kAotMethods[18]();},
	[]() {kAotMethods[19]();},
	[]() {kAotMethods[20]();},
	[]() {kAotMethods[21]();},
	[]() {kAotMethods[22]();},
	[]() {kAotMethods[23]();},
	[]() {kAotMethods[24]();},
	[]() {kAotMethods[25]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[26])(0);},
	[]() {kAotMethods[27]();},
	[]() {kAotMethods[28]();},
	[]() {kAotMethods[29]();},
	[]() {kAotMethods[30]();},
	[]() {kAotMethods[31]();},
	[]() {kAotMethods[32]();},
	[]() {kAotMethods[33]();},
	[]() {kAotMethods[34]();},
	[]() {kAotMethods[35]();},
	[]() {kAotMethods[36]();},
	[]() {kAotMethods[37]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[38])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[39])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {kAotMethods[40]();},
	[]() {kAotMethods[41]();},
	[]() {kAotMethods[42]();},
	[]() {kAotMethods[43]();},
	[]() {kAotMethods[44]();},
	[]() {kAotMethods[45]();},
	[]() {kAotMethods[46]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[47])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[48])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[49])(chaos_make_string_id_value(803950926144638187ULL));},
	[]() {kAotMethods[50]();},
	[]() {kAotMethods[51]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[52])(0);},
	[]() {kAotMethods[53]();},
	[]() {kAotMethods[54]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[55])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[56])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {kAotMethods[57]();},
	[]() {kAotMethods[58]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[59])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[60])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {kAotMethods[61]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[62])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[63])(0);},
	[]() {kAotMethods[64]();},
	[]() {kAotMethods[65]();},
	[]() {kAotMethods[66]();},
	[]() {kAotMethods[67]();},
	[]() {kAotMethods[68]();},
	[]() {kAotMethods[69]();},
	[]() {kAotMethods[70]();},
	[]() {kAotMethods[71]();},
	[]() {kAotMethods[72]();},
	[]() {kAotMethods[73]();},
	[]() {kAotMethods[74]();},
	[]() {kAotMethods[75]();},
	[]() {kAotMethods[76]();},
	[]() {kAotMethods[77]();},
	[]() {kAotMethods[78]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[79])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {kAotMethods[80]();},
	[]() {kAotMethods[81]();},
	[]() {kAotMethods[82]();},
	[]() {kAotMethods[83]();},
	[]() {kAotMethods[84]();},
	[]() {kAotMethods[85]();},
	[]() {kAotMethods[86]();},
	[]() {kAotMethods[87]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[88])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[89])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {kAotMethods[90]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[91])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[92])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[93])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[94])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[95])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[96])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[97])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[98])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[99])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[100])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[101])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[102])(0);},
	[]() {kAotMethods[103]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[104])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[105])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[106])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[107])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[108])(0);},
	[]() {kAotMethods[109]();},
	[]() {kAotMethods[110]();},
	[]() {kAotMethods[111]();},
	[]() {kAotMethods[112]();},
	[]() {kAotMethods[113]();},
	[]() {kAotMethods[114]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[115])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[116])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[117])(0);},
	[]() {kAotMethods[118]();},
	[]() {kAotMethods[119]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[120])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[121])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {kAotMethods[122]();},
	[]() {kAotMethods[123]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[124])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[125])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[126])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[127])(0);},
	[]() {kAotMethods[128]();},
	[]() {kAotMethods[129]();},
	[]() {kAotMethods[130]();},
	[]() {kAotMethods[131]();},
	[]() {kAotMethods[132]();},
	[]() {kAotMethods[133]();},
	[]() {kAotMethods[134]();},
	[]() {kAotMethods[135]();},
	[]() {kAotMethods[136]();},
	[]() {kAotMethods[137]();},
	[]() {kAotMethods[138]();},
	[]() {kAotMethods[139]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[140])(0);},
	[]() {kAotMethods[141]();},
	[]() {kAotMethods[142]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[143])(0);},
	[]() {kAotMethods[144]();},
	[]() {kAotMethods[145]();},
	[]() {kAotMethods[146]();},
	[]() {kAotMethods[147]();},
	[]() {kAotMethods[148]();},
	[]() {kAotMethods[149]();},
	[]() {kAotMethods[150]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[151])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[152])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[153])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[154])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[155])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[156])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
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
static void* const kMethodPointers[157] = {
	reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_0),
	reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_1),
	reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_2),
	reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_3),
	reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_4),
	reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_5),
	reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_6),
	reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_7),
	reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_8),
	reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_9),
	reinterpret_cast<void*>(&SnapshotTestFixtures_AddressHelper_RunAddress),
	reinterpret_cast<void*>(&SnapshotTestFixtures_ArithmeticCompareHelper_RunCompare),
	reinterpret_cast<void*>(&SnapshotTestFixtures_ArithmeticOps_RunAdd),
	reinterpret_cast<void*>(&SnapshotTestFixtures_ArrayHelper_MakeAndFill),
	reinterpret_cast<void*>(&SnapshotTestFixtures_ArrayLengthHelper_RunLength),
	reinterpret_cast<void*>(&SnapshotTestFixtures_ArrayRefHelper_RunArrayRef),
	reinterpret_cast<void*>(&SnapshotTestFixtures_BaseClass__ctor),
	reinterpret_cast<void*>(&SnapshotTestFixtures_BaseClass_Compute),
	reinterpret_cast<void*>(&SnapshotTestFixtures_BitwiseHelper_RunBitwise),
	reinterpret_cast<void*>(&SnapshotTestFixtures_BoxingHelper_BoxAndUnbox),
	reinterpret_cast<void*>(&SnapshotTestFixtures_BoxInterfaceArrayDemo_DemoBoxStore),
	reinterpret_cast<void*>(&SnapshotTestFixtures_BranchCompareBHelper_RunBranchCompareB),
	reinterpret_cast<void*>(&SnapshotTestFixtures_BranchCompareHelper_RunBranchCompare),
	reinterpret_cast<void*>(&SnapshotTestFixtures_BranchDupHelper_RunBranchDup),
	reinterpret_cast<void*>(&SnapshotTestFixtures_BranchUnsignedBHelper_RunBranchUnsignedB),
	reinterpret_cast<void*>(&SnapshotTestFixtures_BranchUnsignedHelper_RunBranchUnsigned),
	reinterpret_cast<void*>(&SnapshotTestFixtures_CalliHelper_RunCalli_System_Int32),
	reinterpret_cast<void*>(&SnapshotTestFixtures_CallVirtHelper_CreateAndUse),
	reinterpret_cast<void*>(&SnapshotTestFixtures_CallvirtHelper_RunCallvirt),
	reinterpret_cast<void*>(&SnapshotTestFixtures_CollectionsHelper_TestDict),
	reinterpret_cast<void*>(&SnapshotTestFixtures_CollectionsHelper_TestList),
	reinterpret_cast<void*>(&SnapshotTestFixtures_CollectionsHelper_TestSet),
	reinterpret_cast<void*>(&SnapshotTestFixtures_ConversionHelper_ConvertToInt),
	reinterpret_cast<void*>(&SnapshotTestFixtures_ConvSmallIntHelper_RunConvSmall),
	reinterpret_cast<void*>(&SnapshotTestFixtures_ConvWideHelper_RunConvWide),
	reinterpret_cast<void*>(&SnapshotTestFixtures_CpblkHelper_RunCpblk),
	reinterpret_cast<void*>(&SnapshotTestFixtures_CpobjHelper_RunCpobj),
	reinterpret_cast<void*>(&SnapshotTestFixtures_DelegateHelper_RunDelegate),
	reinterpret_cast<void*>(&SnapshotTestFixtures_DerivedClass__ctor),
	reinterpret_cast<void*>(&SnapshotTestFixtures_DerivedClass_Compute),
	reinterpret_cast<void*>(&SnapshotTestFixtures_EhHelper_SafeDivide),
	reinterpret_cast<void*>(&SnapshotTestFixtures_ExternalCaller_CallExternal),
	reinterpret_cast<void*>(&SnapshotTestFixtures_FieldHelper_GetAndIncrement),
	reinterpret_cast<void*>(&SnapshotTestFixtures_FloatOpsHelper_RunFloatOps),
	reinterpret_cast<void*>(&SnapshotTestFixtures_FlowControl_IsPositive),
	reinterpret_cast<void*>(&SnapshotTestFixtures_GenericHelper_UseGeneric),
	reinterpret_cast<void*>(&SnapshotTestFixtures_GenericsVirtEhDemo_DemoCombine),
	reinterpret_cast<void*>(&SnapshotTestFixtures_HasFields__ctor),
	reinterpret_cast<void*>(&SnapshotTestFixtures_HasInstanceFields__ctor_System_Int32),
	reinterpret_cast<void*>(&SnapshotTestFixtures_Helper_ConsumeString_System_String),
	reinterpret_cast<void*>(&SnapshotTestFixtures_Helper_GetValue),
	reinterpret_cast<void*>(&SnapshotTestFixtures_Helper_Nop),
	reinterpret_cast<void*>(&SnapshotTestFixtures_Helper_Square_System_Int32),
	reinterpret_cast<void*>(&SnapshotTestFixtures_HotUpdateWithGenericsDemo_Run),
	reinterpret_cast<void*>(&SnapshotTestFixtures_HotUpdateWithTypesDemo_Run),
	reinterpret_cast<void*>(&SnapshotTestFixtures_ImplHelper__ctor_System_Int32),
	reinterpret_cast<void*>(&SnapshotTestFixtures_ImplHelper_GetValue),
	reinterpret_cast<void*>(&SnapshotTestFixtures_IndirectHelper_ReadWriteRef),
	reinterpret_cast<void*>(&SnapshotTestFixtures_InstanceFieldHelper_CreateAndUseFields),
	reinterpret_cast<void*>(&SnapshotTestFixtures_InstanceHelper__ctor_System_Int32),
	reinterpret_cast<void*>(&SnapshotTestFixtures_InstanceHelper_GetValue),
	reinterpret_cast<void*>(&SnapshotTestFixtures_InterfaceDevirtHelper_RunInterfaceTest),
	reinterpret_cast<void*>(&SnapshotTestFixtures_InternalHelper_Double_System_Int32),
	reinterpret_cast<void*>(&SnapshotTestFixtures_LdargaHelper_RunLdarga_System_Int32),
	reinterpret_cast<void*>(&SnapshotTestFixtures_LdcI8Helper_RunLdcI8),
	reinterpret_cast<void*>(&SnapshotTestFixtures_LdelemaHelper_RunLdelema),
	reinterpret_cast<void*>(&SnapshotTestFixtures_LdelemAllHelper_TestAllElems),
	reinterpret_cast<void*>(&SnapshotTestFixtures_LdftnHelper_GetFnPtr),
	reinterpret_cast<void*>(&SnapshotTestFixtures_LdindI1Helper_RunLdindI1),
	reinterpret_cast<void*>(&SnapshotTestFixtures_LdindI8Helper_RunLdindI8),
	reinterpret_cast<void*>(&SnapshotTestFixtures_LdindStindGapsHelper_RunLdindI),
	reinterpret_cast<void*>(&SnapshotTestFixtures_LdindStindGapsHelper_RunLdindI4),
	reinterpret_cast<void*>(&SnapshotTestFixtures_LdindStindGapsHelper_RunStindI),
	reinterpret_cast<void*>(&SnapshotTestFixtures_LdindUnsignedHelper_RunLdindUnsigned),
	reinterpret_cast<void*>(&SnapshotTestFixtures_LdindWideHelper_RunLdindWide),
	reinterpret_cast<void*>(&SnapshotTestFixtures_LdtokenHelper_RunLdtoken),
	reinterpret_cast<void*>(&SnapshotTestFixtures_LdVirtftnHelper_RunLdVirtftn),
	reinterpret_cast<void*>(&SnapshotTestFixtures_LocalAllocHelper_RunAlloc),
	reinterpret_cast<void*>(&SnapshotTestFixtures_LoopHelper_SumToFive),
	reinterpret_cast<void*>(&SnapshotTestFixtures_MarkedClass__ctor),
	reinterpret_cast<void*>(&SnapshotTestFixtures_MathHelper_RunSquare),
	reinterpret_cast<void*>(&SnapshotTestFixtures_MoreRareOpsHelper_RunArglist),
	reinterpret_cast<void*>(&SnapshotTestFixtures_MoreRareOpsHelper_RunJmp),
	reinterpret_cast<void*>(&SnapshotTestFixtures_MoreRareOpsHelper_RunMkrefany),
	reinterpret_cast<void*>(&SnapshotTestFixtures_MoreRareOpsHelper_RunRefanytype),
	reinterpret_cast<void*>(&SnapshotTestFixtures_MoreRareOpsHelper_RunRefanyval),
	reinterpret_cast<void*>(&SnapshotTestFixtures_MulDivHelper_RunMulDiv),
	reinterpret_cast<void*>(&SnapshotTestFixtures_MultiDispatcher_DispatchBoth),
	reinterpret_cast<void*>(&SnapshotTestFixtures_MyClass__ctor_System_Int32),
	reinterpret_cast<void*>(&SnapshotTestFixtures_MyClass_GetValue),
	reinterpret_cast<void*>(&SnapshotTestFixtures_NegShiftHelper_RunNegShift),
	reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfI1_System_Int32),
	reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfI1Un_System_Int32),
	reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfI2Un_System_Int32),
	reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfI4Un_System_Int64),
	reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfI8Un_System_Single),
	reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfIUn_System_Int32),
	reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfU1Un_System_Int32),
	reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfU2Un_System_Int32),
	reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfU4Un_System_Int64),
	reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfU8Un_System_Double),
	reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfUUn_System_Int32),
	reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowConvHelper_ConvOvfI2_System_Int32),
	reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowConvHelper_ConvOvfI4),
	reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowConvHelper_ConvOvfI8_System_Single),
	reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowConvHelper_ConvOvfU1_System_Int32),
	reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowConvHelper_ConvOvfU2_System_Int32),
	reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowConvHelper_ConvOvfU4_System_Int64),
	reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowConvHelper_ConvOvfU8_System_Double),
	reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowHelper_RunOverflowAdd),
	reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowHelper_RunOverflowMul),
	reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowHelper_RunOverflowSub),
	reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowUnHelper_RunOverflowAddUn),
	reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowUnHelper_RunOverflowMulUn),
	reinterpret_cast<void*>(&SnapshotTestFixtures_OverflowUnHelper_RunOverflowSubUn),
	reinterpret_cast<void*>(&SnapshotTestFixtures_RareOpsHelper_TestConvRUn_System_Single),
	reinterpret_cast<void*>(&SnapshotTestFixtures_RareOpsHelper_TestInitBlk_System_Int32),
	reinterpret_cast<void*>(&SnapshotTestFixtures_RareOpsHelper_TestStarg_System_Int32),
	reinterpret_cast<void*>(&SnapshotTestFixtures_RethrowHelper_RunRethrow),
	reinterpret_cast<void*>(&SnapshotTestFixtures_SealedClassVirtualHelper_RunSealedVirtual),
	reinterpret_cast<void*>(&SnapshotTestFixtures_SealedHelper__ctor_System_Int32),
	reinterpret_cast<void*>(&SnapshotTestFixtures_SealedHelper_GetValueVirtual),
	reinterpret_cast<void*>(&SnapshotTestFixtures_ShiftHelper_RunShift),
	reinterpret_cast<void*>(&SnapshotTestFixtures_SimpleGapsHelper_RunConvI),
	reinterpret_cast<void*>(&SnapshotTestFixtures_SimpleGapsHelper_RunConvOvfI_System_Int32),
	reinterpret_cast<void*>(&SnapshotTestFixtures_SimpleGapsHelper_RunConvOvfIUn_System_Int32),
	reinterpret_cast<void*>(&SnapshotTestFixtures_SimpleGapsHelper_RunConvOvfU_System_Int32),
	reinterpret_cast<void*>(&SnapshotTestFixtures_SimpleGapsHelper_RunConvOvfUUn_System_Int32),
	reinterpret_cast<void*>(&SnapshotTestFixtures_SimpleGapsHelper_RunConvU4),
	reinterpret_cast<void*>(&SnapshotTestFixtures_SimpleGapsHelper_RunLdnull),
	reinterpret_cast<void*>(&SnapshotTestFixtures_SizeOfHelper_GetSize),
	reinterpret_cast<void*>(&SnapshotTestFixtures_StaticFieldWriteHelper_WriteAndRead),
	reinterpret_cast<void*>(&SnapshotTestFixtures_StelemAllHelper_TestAllElems),
	reinterpret_cast<void*>(&SnapshotTestFixtures_StindNarrowHelper_RunStindNarrow),
	reinterpret_cast<void*>(&SnapshotTestFixtures_StindWideHelper_RunStindWide),
	reinterpret_cast<void*>(&SnapshotTestFixtures_StringConcatHelper_TestConcat),
	reinterpret_cast<void*>(&SnapshotTestFixtures_StringFormatHelper_TestFormatOne),
	reinterpret_cast<void*>(&SnapshotTestFixtures_StringOps_UseString),
	reinterpret_cast<void*>(&SnapshotTestFixtures_SwitchHelper_Classify),
	reinterpret_cast<void*>(&SnapshotTestFixtures_TargetHelper_GetValue),
	reinterpret_cast<void*>(&SnapshotTestFixtures_ThrowHelper_CheckPositive_System_Int32),
	reinterpret_cast<void*>(&SnapshotTestFixtures_TypeCheckHelper_CheckAndCast),
	reinterpret_cast<void*>(&SnapshotTestFixtures_UnboxHelper_RunUnbox),
	reinterpret_cast<void*>(&SnapshotTestFixtures_UnsignedOpsHelper_TestCkfinite_System_Double),
	reinterpret_cast<void*>(&SnapshotTestFixtures_UnsignedOpsHelper_TestCltUn),
	reinterpret_cast<void*>(&SnapshotTestFixtures_UnsignedOpsHelper_TestDivUn),
	reinterpret_cast<void*>(&SnapshotTestFixtures_UnsignedOpsHelper_TestRemUn),
	reinterpret_cast<void*>(&SnapshotTestFixtures_ValueTypeHelper_RunValueType),
	reinterpret_cast<void*>(&SnapshotTestFixtures_VirtualDispatchHelper_UseVirtualDispatch),
	reinterpret_cast<void*>(&SnapshotTestFixtures_VirtualHelper_UseVirtual),
	reinterpret_cast<void*>(&SnapshotTestFixtures_VoidCaller_DoNothing),
	reinterpret_cast<void*>(&SnapshotTestFixtures_Wrapper_1__ctor_0),
	reinterpret_cast<void*>(&SnapshotTestFixtures_Wrapper_1_GetValue),
	reinterpret_cast<void*>(&SnapshotTestFixtures_Wrapper__0__ctor_0),
	reinterpret_cast<void*>(&SnapshotTestFixtures_Wrapper__0_GetValue),
	reinterpret_cast<void*>(&SnapshotTestFixtures_Wrapper_System_Int32__ctor_System_Int32),
	reinterpret_cast<void*>(&SnapshotTestFixtures_Wrapper_System_Int32_GetValue),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
// ── VTable descriptors (for BootstrapRuntime TypeVTable registration) ──
static const VTableDescriptorV0 kChaosVTableDescriptors[] = {
	{
		CHAOS_IL2CPP_UINT64_C(0xEB03DF8A01098CF3),
		0x00000017u,
		0u,
		2u,
		reinterpret_cast<const void*>(kSlots_SnapshotTestFixtures_BaseClass),
		reinterpret_cast<const void**>(chaos_vtable_SnapshotTestFixtures_BaseClass),
		2u,
		1,
		{0, 0, 0},
		nullptr,
		0u,
	},
	{
		CHAOS_IL2CPP_UINT64_C(0x485B1C52BEE85307),
		0x00000018u,
		0x00000017u,
		2u,
		reinterpret_cast<const void*>(kSlots_SnapshotTestFixtures_DerivedClass),
		reinterpret_cast<const void**>(chaos_vtable_SnapshotTestFixtures_DerivedClass),
		2u,
		1,
		{0, 0, 0},
		nullptr,
		0u,
	},
	{
		CHAOS_IL2CPP_UINT64_C(0x5451166259F7FAA9),
		0x00000032u,
		0u,
		1u,
		reinterpret_cast<const void*>(kSlots_SnapshotTestFixtures_HasFields),
		reinterpret_cast<const void**>(chaos_vtable_SnapshotTestFixtures_HasFields),
		1u,
		1,
		{0, 0, 0},
		nullptr,
		0u,
	},
	{
		CHAOS_IL2CPP_UINT64_C(0xF7329E37EA674DDE),
		0x00000020u,
		0u,
		3u,
		reinterpret_cast<const void*>(kSlots_SnapshotTestFixtures_HasInstanceFields),
		reinterpret_cast<const void**>(chaos_vtable_SnapshotTestFixtures_HasInstanceFields),
		3u,
		1,
		{0, 0, 0},
		nullptr,
		0u,
	},
	{
		CHAOS_IL2CPP_UINT64_C(0x2539BCAF6017195E),
		0x00000060u,
		0u,
		4u,
		reinterpret_cast<const void*>(kSlots_SnapshotTestFixtures_ImplHelper),
		reinterpret_cast<const void**>(chaos_vtable_SnapshotTestFixtures_ImplHelper),
		4u,
		1,
		{0, 0, 0},
		chaos_iface_map_SnapshotTestFixtures_ImplHelper,
		1u,
	},
	{
		CHAOS_IL2CPP_UINT64_C(0x68A1746D9B8CD2A3),
		0x00000008u,
		0u,
		4u,
		reinterpret_cast<const void*>(kSlots_SnapshotTestFixtures_InstanceHelper),
		reinterpret_cast<const void**>(chaos_vtable_SnapshotTestFixtures_InstanceHelper),
		4u,
		1,
		{0, 0, 0},
		nullptr,
		0u,
	},
	{
		CHAOS_IL2CPP_UINT64_C(0x54D5AE1DBE67DB64),
		0x0000001Au,
		0u,
		1u,
		reinterpret_cast<const void*>(kSlots_SnapshotTestFixtures_MarkedClass),
		reinterpret_cast<const void**>(chaos_vtable_SnapshotTestFixtures_MarkedClass),
		1u,
		1,
		{0, 0, 0},
		chaos_iface_map_SnapshotTestFixtures_MarkedClass,
		1u,
	},
	{
		CHAOS_IL2CPP_UINT64_C(0xEC2B36B9D9CEA2CE),
		0x00000045u,
		0u,
		4u,
		reinterpret_cast<const void*>(kSlots_SnapshotTestFixtures_MyClass),
		reinterpret_cast<const void**>(chaos_vtable_SnapshotTestFixtures_MyClass),
		4u,
		1,
		{0, 0, 0},
		nullptr,
		0u,
	},
	{
		CHAOS_IL2CPP_UINT64_C(0x7696B9F8DBE50B64),
		0x0000005Du,
		0u,
		5u,
		reinterpret_cast<const void*>(kSlots_SnapshotTestFixtures_SealedHelper),
		reinterpret_cast<const void**>(chaos_vtable_SnapshotTestFixtures_SealedHelper),
		5u,
		1,
		{0, 0, 0},
		nullptr,
		0u,
	},
	{
		CHAOS_IL2CPP_UINT64_C(0xDAC8786C75C01D02),
		0x00000063u,
		0u,
		7u,
		reinterpret_cast<const void*>(kSlots_SnapshotTestFixtures_Wrapper__0_),
		reinterpret_cast<const void**>(chaos_vtable_SnapshotTestFixtures_Wrapper__0_),
		7u,
		1,
		{0, 0, 0},
		nullptr,
		0u,
	},
	{
		CHAOS_IL2CPP_UINT64_C(0x199C98A9994F4A20),
		0x00000062u,
		0u,
		4u,
		reinterpret_cast<const void*>(kSlots_SnapshotTestFixtures_Wrapper_System_Int32_),
		reinterpret_cast<const void**>(chaos_vtable_SnapshotTestFixtures_Wrapper_System_Int32_),
		4u,
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
	.method_pointer_count      = 157u,
	.reverse_pinvoke_wrappers  = nullptr,
	.reverse_pinvoke_wrapper_count = 0u,
	.invoker_pointers          = nullptr,
	.invoker_pointer_count     = 0u,
	.unresolved_virtual_calls = nullptr,
	.unresolved_virtual_call_count = 0u,
	.type_capabilities       = nullptr,
	.type_capability_count   = 0u,
	.vtable_descriptors = kChaosVTableDescriptors,
	.vtable_descriptor_count = 11u,
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
	.image_name_utf8    = "SnapshotProverSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_SnapshotProverSubjects_SnapshotProverSubjects[1] = {
	{ 0u, "SnapshotProverSubjects/SnapshotProverSubjects::_exitCode", "_exitCode", "System.Int32", 0LL },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotProverSubjects_SnapshotProverSubjects[10] = {
	{ 0u, "SnapshotProverSubjects/SnapshotProverSubjects::Subject_0:System.Void()", "Subject_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotProverSubjects/SnapshotProverSubjects::Subject_1:System.Void()", "Subject_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotProverSubjects/SnapshotProverSubjects::Subject_2:System.Void()", "Subject_2", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotProverSubjects/SnapshotProverSubjects::Subject_3:System.Void()", "Subject_3", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotProverSubjects/SnapshotProverSubjects::Subject_4:System.Void()", "Subject_4", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotProverSubjects/SnapshotProverSubjects::Subject_5:System.Void()", "Subject_5", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotProverSubjects/SnapshotProverSubjects::Subject_6:System.Void()", "Subject_6", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotProverSubjects/SnapshotProverSubjects::Subject_7:System.Void()", "Subject_7", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotProverSubjects/SnapshotProverSubjects::Subject_8:System.Void()", "Subject_8", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotProverSubjects/SnapshotProverSubjects::Subject_9:System.Void()", "Subject_9", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_AddressHelper[1] = {
	{ 0u, "SnapshotTestFixtures/AddressHelper::RunAddress:System.Int32()", "RunAddress", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_ArithmeticCompareHelper[1] = {
	{ 0u, "SnapshotTestFixtures/ArithmeticCompareHelper::RunCompare:System.Int32()", "RunCompare", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_ArithmeticOps[1] = {
	{ 0u, "SnapshotTestFixtures/ArithmeticOps::RunAdd:System.Int32()", "RunAdd", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_ArrayHelper[1] = {
	{ 0u, "SnapshotTestFixtures/ArrayHelper::MakeAndFill:System.Int32()", "MakeAndFill", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_ArrayLengthHelper[1] = {
	{ 0u, "SnapshotTestFixtures/ArrayLengthHelper::RunLength:System.Int32()", "RunLength", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_ArrayRefHelper[1] = {
	{ 0u, "SnapshotTestFixtures/ArrayRefHelper::RunArrayRef:System.Int32()", "RunArrayRef", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_BaseClass[2] = {
	{ 0u, "SnapshotTestFixtures/BaseClass::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/BaseClass::Compute:System.Int32()", "Compute", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_BitwiseHelper[1] = {
	{ 0u, "SnapshotTestFixtures/BitwiseHelper::RunBitwise:System.Int32()", "RunBitwise", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_BoxingHelper[1] = {
	{ 0u, "SnapshotTestFixtures/BoxingHelper::BoxAndUnbox:System.Int32()", "BoxAndUnbox", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_BoxInterfaceArrayDemo[1] = {
	{ 0u, "SnapshotTestFixtures/BoxInterfaceArrayDemo::DemoBoxStore:System.Int32()", "DemoBoxStore", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_BranchCompareBHelper[1] = {
	{ 0u, "SnapshotTestFixtures/BranchCompareBHelper::RunBranchCompareB:System.Int32()", "RunBranchCompareB", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_BranchCompareHelper[1] = {
	{ 0u, "SnapshotTestFixtures/BranchCompareHelper::RunBranchCompare:System.Int32()", "RunBranchCompare", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_BranchDupHelper[1] = {
	{ 0u, "SnapshotTestFixtures/BranchDupHelper::RunBranchDup:System.Int32()", "RunBranchDup", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_BranchUnsignedBHelper[1] = {
	{ 0u, "SnapshotTestFixtures/BranchUnsignedBHelper::RunBranchUnsignedB:System.Int32()", "RunBranchUnsignedB", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_BranchUnsignedHelper[1] = {
	{ 0u, "SnapshotTestFixtures/BranchUnsignedHelper::RunBranchUnsigned:System.Int32()", "RunBranchUnsigned", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_CalliHelper[1] = {
	{ 0u, "SnapshotTestFixtures/CalliHelper::RunCalli:System.Int32(System.Int32)", "RunCalli", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_CallVirtHelper[1] = {
	{ 0u, "SnapshotTestFixtures/CallVirtHelper::CreateAndUse:System.Int32()", "CreateAndUse", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_CallvirtHelper[1] = {
	{ 0u, "SnapshotTestFixtures/CallvirtHelper::RunCallvirt:System.Int32()", "RunCallvirt", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_CollectionsHelper[3] = {
	{ 0u, "SnapshotTestFixtures/CollectionsHelper::TestDict:System.Int32()", "TestDict", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/CollectionsHelper::TestList:System.Int32()", "TestList", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/CollectionsHelper::TestSet:System.Int32()", "TestSet", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_ConversionHelper[1] = {
	{ 0u, "SnapshotTestFixtures/ConversionHelper::ConvertToInt:System.Int32()", "ConvertToInt", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_ConvSmallIntHelper[1] = {
	{ 0u, "SnapshotTestFixtures/ConvSmallIntHelper::RunConvSmall:System.Int32()", "RunConvSmall", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_ConvWideHelper[1] = {
	{ 0u, "SnapshotTestFixtures/ConvWideHelper::RunConvWide:System.Int32()", "RunConvWide", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_CpblkHelper[1] = {
	{ 0u, "SnapshotTestFixtures/CpblkHelper::RunCpblk:System.Int32()", "RunCpblk", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_CpobjHelper[1] = {
	{ 0u, "SnapshotTestFixtures/CpobjHelper::RunCpobj:System.Int32()", "RunCpobj", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_DelegateHelper[1] = {
	{ 0u, "SnapshotTestFixtures/DelegateHelper::RunDelegate:System.Int32()", "RunDelegate", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_DerivedClass[2] = {
	{ 0u, "SnapshotTestFixtures/DerivedClass::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/DerivedClass::Compute:System.Int32()", "Compute", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_EhHelper[1] = {
	{ 0u, "SnapshotTestFixtures/EhHelper::SafeDivide:System.Int32()", "SafeDivide", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_ExternalCaller[1] = {
	{ 0u, "SnapshotTestFixtures/ExternalCaller::CallExternal:System.Int32()", "CallExternal", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_FieldHelper[1] = {
	{ 0u, "SnapshotTestFixtures/FieldHelper::GetAndIncrement:System.Int32()", "GetAndIncrement", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_FloatOpsHelper[1] = {
	{ 0u, "SnapshotTestFixtures/FloatOpsHelper::RunFloatOps:System.Int32()", "RunFloatOps", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_FlowControl[1] = {
	{ 0u, "SnapshotTestFixtures/FlowControl::IsPositive:System.Int32()", "IsPositive", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_GenericHelper[1] = {
	{ 0u, "SnapshotTestFixtures/GenericHelper::UseGeneric:System.Int32()", "UseGeneric", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_GenericsVirtEhDemo[1] = {
	{ 0u, "SnapshotTestFixtures/GenericsVirtEhDemo::DemoCombine:System.Int32()", "DemoCombine", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_HasFields[1] = {
	{ 0u, "SnapshotTestFixtures/HasFields::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_HasInstanceFields[1] = {
	{ 0u, "SnapshotTestFixtures/HasInstanceFields::.ctor:System.Void(System.Int32)", ".ctor", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_Helper[4] = {
	{ 0u, "SnapshotTestFixtures/Helper::ConsumeString:System.Void(System.String)", "ConsumeString", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/Helper::GetValue:System.Int32()", "GetValue", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/Helper::Nop:System.Void()", "Nop", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/Helper::Square:System.Int32(System.Int32)", "Square", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_HotUpdateWithGenericsDemo[1] = {
	{ 0u, "SnapshotTestFixtures/HotUpdateWithGenericsDemo::Run:System.Int32()", "Run", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_HotUpdateWithTypesDemo[1] = {
	{ 0u, "SnapshotTestFixtures/HotUpdateWithTypesDemo::Run:System.Int32()", "Run", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_ImplHelper[2] = {
	{ 0u, "SnapshotTestFixtures/ImplHelper::.ctor:System.Void(System.Int32)", ".ctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/ImplHelper::GetValue:System.Int32()", "GetValue", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_IndirectHelper[1] = {
	{ 0u, "SnapshotTestFixtures/IndirectHelper::ReadWriteRef:System.Int32()", "ReadWriteRef", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_InstanceFieldHelper[1] = {
	{ 0u, "SnapshotTestFixtures/InstanceFieldHelper::CreateAndUseFields:System.Int32()", "CreateAndUseFields", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_InstanceHelper[2] = {
	{ 0u, "SnapshotTestFixtures/InstanceHelper::.ctor:System.Void(System.Int32)", ".ctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/InstanceHelper::GetValue:System.Int32()", "GetValue", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_InterfaceDevirtHelper[1] = {
	{ 0u, "SnapshotTestFixtures/InterfaceDevirtHelper::RunInterfaceTest:System.Int32()", "RunInterfaceTest", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_InternalHelper[1] = {
	{ 0u, "SnapshotTestFixtures/InternalHelper::Double:System.Int32(System.Int32)", "Double", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_LdargaHelper[1] = {
	{ 0u, "SnapshotTestFixtures/LdargaHelper::RunLdarga:System.Int32(System.Int32)", "RunLdarga", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_LdcI8Helper[1] = {
	{ 0u, "SnapshotTestFixtures/LdcI8Helper::RunLdcI8:System.Int64()", "RunLdcI8", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_LdelemaHelper[1] = {
	{ 0u, "SnapshotTestFixtures/LdelemaHelper::RunLdelema:System.Int32()", "RunLdelema", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_LdelemAllHelper[1] = {
	{ 0u, "SnapshotTestFixtures/LdelemAllHelper::TestAllElems:System.Int32()", "TestAllElems", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_LdftnHelper[1] = {
	{ 0u, "SnapshotTestFixtures/LdftnHelper::GetFnPtr:System.Int32()", "GetFnPtr", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_LdindI1Helper[1] = {
	{ 0u, "SnapshotTestFixtures/LdindI1Helper::RunLdindI1:System.Int32()", "RunLdindI1", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_LdindI8Helper[1] = {
	{ 0u, "SnapshotTestFixtures/LdindI8Helper::RunLdindI8:System.Int32()", "RunLdindI8", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_LdindStindGapsHelper[3] = {
	{ 0u, "SnapshotTestFixtures/LdindStindGapsHelper::RunLdindI:System.Int32()", "RunLdindI", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/LdindStindGapsHelper::RunLdindI4:System.Int32()", "RunLdindI4", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/LdindStindGapsHelper::RunStindI:System.Int32()", "RunStindI", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_LdindUnsignedHelper[1] = {
	{ 0u, "SnapshotTestFixtures/LdindUnsignedHelper::RunLdindUnsigned:System.Int32()", "RunLdindUnsigned", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_LdindWideHelper[1] = {
	{ 0u, "SnapshotTestFixtures/LdindWideHelper::RunLdindWide:System.Int32()", "RunLdindWide", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_LdtokenHelper[1] = {
	{ 0u, "SnapshotTestFixtures/LdtokenHelper::RunLdtoken:System.Int32()", "RunLdtoken", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_LdVirtftnHelper[1] = {
	{ 0u, "SnapshotTestFixtures/LdVirtftnHelper::RunLdVirtftn:System.Int32()", "RunLdVirtftn", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_LocalAllocHelper[1] = {
	{ 0u, "SnapshotTestFixtures/LocalAllocHelper::RunAlloc:System.Int32()", "RunAlloc", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_LoopHelper[1] = {
	{ 0u, "SnapshotTestFixtures/LoopHelper::SumToFive:System.Int32()", "SumToFive", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_MarkedClass[1] = {
	{ 0u, "SnapshotTestFixtures/MarkedClass::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_MathHelper[1] = {
	{ 0u, "SnapshotTestFixtures/MathHelper::RunSquare:System.Int32()", "RunSquare", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_MoreRareOpsHelper[5] = {
	{ 0u, "SnapshotTestFixtures/MoreRareOpsHelper::RunArglist:System.Int32()", "RunArglist", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/MoreRareOpsHelper::RunJmp:System.Int32()", "RunJmp", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/MoreRareOpsHelper::RunMkrefany:System.Int32()", "RunMkrefany", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/MoreRareOpsHelper::RunRefanytype:System.Int32()", "RunRefanytype", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/MoreRareOpsHelper::RunRefanyval:System.Int32()", "RunRefanyval", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_MulDivHelper[1] = {
	{ 0u, "SnapshotTestFixtures/MulDivHelper::RunMulDiv:System.Int32()", "RunMulDiv", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_MultiDispatcher[1] = {
	{ 0u, "SnapshotTestFixtures/MultiDispatcher::DispatchBoth:System.Int32()", "DispatchBoth", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_MyClass[2] = {
	{ 0u, "SnapshotTestFixtures/MyClass::.ctor:System.Void(System.Int32)", ".ctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/MyClass::GetValue:System.Int32()", "GetValue", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_NegShiftHelper[1] = {
	{ 0u, "SnapshotTestFixtures/NegShiftHelper::RunNegShift:System.Int32()", "RunNegShift", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_OverflowConvExtHelper[11] = {
	{ 0u, "SnapshotTestFixtures/OverflowConvExtHelper::ConvOvfI1:System.Int32(System.Int32)", "ConvOvfI1", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/OverflowConvExtHelper::ConvOvfI1Un:System.Int32(System.Int32)", "ConvOvfI1Un", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/OverflowConvExtHelper::ConvOvfI2Un:System.Int32(System.Int32)", "ConvOvfI2Un", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/OverflowConvExtHelper::ConvOvfI4Un:System.Int32(System.Int64)", "ConvOvfI4Un", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/OverflowConvExtHelper::ConvOvfI8Un:System.Int64(System.Single)", "ConvOvfI8Un", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/OverflowConvExtHelper::ConvOvfIUn:System.Int32(System.Int32)", "ConvOvfIUn", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/OverflowConvExtHelper::ConvOvfU1Un:System.Int32(System.Int32)", "ConvOvfU1Un", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/OverflowConvExtHelper::ConvOvfU2Un:System.Int32(System.Int32)", "ConvOvfU2Un", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/OverflowConvExtHelper::ConvOvfU4Un:System.Int32(System.Int64)", "ConvOvfU4Un", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/OverflowConvExtHelper::ConvOvfU8Un:System.Int64(System.Double)", "ConvOvfU8Un", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/OverflowConvExtHelper::ConvOvfUUn:System.Int32(System.Int32)", "ConvOvfUUn", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_OverflowConvHelper[7] = {
	{ 0u, "SnapshotTestFixtures/OverflowConvHelper::ConvOvfI2:System.Int32(System.Int32)", "ConvOvfI2", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/OverflowConvHelper::ConvOvfI4:System.Int32()", "ConvOvfI4", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/OverflowConvHelper::ConvOvfI8:System.Int64(System.Single)", "ConvOvfI8", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/OverflowConvHelper::ConvOvfU1:System.Int32(System.Int32)", "ConvOvfU1", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/OverflowConvHelper::ConvOvfU2:System.Int32(System.Int32)", "ConvOvfU2", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/OverflowConvHelper::ConvOvfU4:System.Int32(System.Int64)", "ConvOvfU4", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/OverflowConvHelper::ConvOvfU8:System.Int64(System.Double)", "ConvOvfU8", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_OverflowHelper[3] = {
	{ 0u, "SnapshotTestFixtures/OverflowHelper::RunOverflowAdd:System.Int32()", "RunOverflowAdd", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/OverflowHelper::RunOverflowMul:System.Int32()", "RunOverflowMul", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/OverflowHelper::RunOverflowSub:System.Int32()", "RunOverflowSub", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_OverflowUnHelper[3] = {
	{ 0u, "SnapshotTestFixtures/OverflowUnHelper::RunOverflowAddUn:System.Int32()", "RunOverflowAddUn", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/OverflowUnHelper::RunOverflowMulUn:System.Int32()", "RunOverflowMulUn", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/OverflowUnHelper::RunOverflowSubUn:System.Int32()", "RunOverflowSubUn", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_RareOpsHelper[3] = {
	{ 0u, "SnapshotTestFixtures/RareOpsHelper::TestConvRUn:System.Int32(System.Single)", "TestConvRUn", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/RareOpsHelper::TestInitBlk:System.Void(System.Int32&)", "TestInitBlk", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/RareOpsHelper::TestStarg:System.Int32(System.Int32)", "TestStarg", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_RethrowHelper[1] = {
	{ 0u, "SnapshotTestFixtures/RethrowHelper::RunRethrow:System.Int32()", "RunRethrow", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_SealedClassVirtualHelper[1] = {
	{ 0u, "SnapshotTestFixtures/SealedClassVirtualHelper::RunSealedVirtual:System.Int32()", "RunSealedVirtual", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_SealedHelper[2] = {
	{ 0u, "SnapshotTestFixtures/SealedHelper::.ctor:System.Void(System.Int32)", ".ctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/SealedHelper::GetValueVirtual:System.Int32()", "GetValueVirtual", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_ShiftHelper[1] = {
	{ 0u, "SnapshotTestFixtures/ShiftHelper::RunShift:System.Int32()", "RunShift", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_SimpleGapsHelper[7] = {
	{ 0u, "SnapshotTestFixtures/SimpleGapsHelper::RunConvI:System.Int32()", "RunConvI", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/SimpleGapsHelper::RunConvOvfI:System.Int32(System.Int32)", "RunConvOvfI", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/SimpleGapsHelper::RunConvOvfIUn:System.Int32(System.Int32)", "RunConvOvfIUn", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/SimpleGapsHelper::RunConvOvfU:System.Int32(System.Int32)", "RunConvOvfU", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/SimpleGapsHelper::RunConvOvfUUn:System.Int32(System.Int32)", "RunConvOvfUUn", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/SimpleGapsHelper::RunConvU4:System.Int32()", "RunConvU4", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/SimpleGapsHelper::RunLdnull:System.Int32()", "RunLdnull", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_SizeOfHelper[1] = {
	{ 0u, "SnapshotTestFixtures/SizeOfHelper::GetSize:System.Int32()", "GetSize", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_StaticFieldWriteHelper[1] = {
	{ 0u, "SnapshotTestFixtures/StaticFieldWriteHelper::WriteAndRead:System.Int32()", "WriteAndRead", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_StelemAllHelper[1] = {
	{ 0u, "SnapshotTestFixtures/StelemAllHelper::TestAllElems:System.Int32()", "TestAllElems", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_StindNarrowHelper[1] = {
	{ 0u, "SnapshotTestFixtures/StindNarrowHelper::RunStindNarrow:System.Int32()", "RunStindNarrow", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_StindWideHelper[1] = {
	{ 0u, "SnapshotTestFixtures/StindWideHelper::RunStindWide:System.Int32()", "RunStindWide", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_StringConcatHelper[1] = {
	{ 0u, "SnapshotTestFixtures/StringConcatHelper::TestConcat:System.Int32()", "TestConcat", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_StringFormatHelper[1] = {
	{ 0u, "SnapshotTestFixtures/StringFormatHelper::TestFormatOne:System.Int32()", "TestFormatOne", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_StringOps[1] = {
	{ 0u, "SnapshotTestFixtures/StringOps::UseString:System.Int32()", "UseString", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_SwitchHelper[1] = {
	{ 0u, "SnapshotTestFixtures/SwitchHelper::Classify:System.Int32()", "Classify", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_TargetHelper[1] = {
	{ 0u, "SnapshotTestFixtures/TargetHelper::GetValue:System.Int32()", "GetValue", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_ThrowHelper[1] = {
	{ 0u, "SnapshotTestFixtures/ThrowHelper::CheckPositive:System.Int32(System.Int32)", "CheckPositive", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_TypeCheckHelper[1] = {
	{ 0u, "SnapshotTestFixtures/TypeCheckHelper::CheckAndCast:System.Int32()", "CheckAndCast", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_UnboxHelper[1] = {
	{ 0u, "SnapshotTestFixtures/UnboxHelper::RunUnbox:System.Int32()", "RunUnbox", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_UnsignedOpsHelper[4] = {
	{ 0u, "SnapshotTestFixtures/UnsignedOpsHelper::TestCkfinite:System.Double(System.Double)", "TestCkfinite", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/UnsignedOpsHelper::TestCltUn:System.Int32()", "TestCltUn", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/UnsignedOpsHelper::TestDivUn:System.Int32()", "TestDivUn", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/UnsignedOpsHelper::TestRemUn:System.Int32()", "TestRemUn", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_ValueTypeHelper[1] = {
	{ 0u, "SnapshotTestFixtures/ValueTypeHelper::RunValueType:System.Int32()", "RunValueType", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_VirtualDispatchHelper[1] = {
	{ 0u, "SnapshotTestFixtures/VirtualDispatchHelper::UseVirtualDispatch:System.Int32()", "UseVirtualDispatch", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_VirtualHelper[1] = {
	{ 0u, "SnapshotTestFixtures/VirtualHelper::UseVirtual:System.Int32()", "UseVirtual", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_VoidCaller[1] = {
	{ 0u, "SnapshotTestFixtures/VoidCaller::DoNothing:System.Int32()", "DoNothing", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_Wrapper_1[2] = {
	{ 0u, "SnapshotTestFixtures/Wrapper`1::.ctor:System.Void(!0)", ".ctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/Wrapper`1::GetValue:!0()", "GetValue", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_Wrapper__0_[2] = {
	{ 0u, "SnapshotTestFixtures/Wrapper<!0>::.ctor:System.Void(!0)", ".ctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/Wrapper<!0>::GetValue:!0()", "GetValue", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_Wrapper_System_Int32_[2] = {
	{ 0u, "SnapshotTestFixtures/Wrapper<System.Int32>::.ctor:System.Void(System.Int32)", ".ctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "SnapshotTestFixtures/Wrapper<System.Int32>::GetValue:System.Int32()", "GetValue", "System.Void", 0, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[97] = {
	{ 0u, "SnapshotProverSubjects/SnapshotProverSubjects", "SnapshotProverSubjects/SnapshotProverSubjects", "", "SnapshotProverSubjects", "SnapshotProverSubjects", nullptr, kReflFields_SnapshotProverSubjects_SnapshotProverSubjects, 1u, nullptr, 0u,
	kReflMethods_SnapshotProverSubjects_SnapshotProverSubjects, 10u },
	{ 0u, "SnapshotTestFixtures/AddressHelper", "SnapshotTestFixtures/AddressHelper", "", "AddressHelper", "AddressHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_AddressHelper, 1u },
	{ 0u, "SnapshotTestFixtures/ArithmeticCompareHelper", "SnapshotTestFixtures/ArithmeticCompareHelper", "", "ArithmeticCompareHelper", "ArithmeticCompareHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_ArithmeticCompareHelper, 1u },
	{ 0u, "SnapshotTestFixtures/ArithmeticOps", "SnapshotTestFixtures/ArithmeticOps", "", "ArithmeticOps", "ArithmeticOps", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_ArithmeticOps, 1u },
	{ 0u, "SnapshotTestFixtures/ArrayHelper", "SnapshotTestFixtures/ArrayHelper", "", "ArrayHelper", "ArrayHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_ArrayHelper, 1u },
	{ 0u, "SnapshotTestFixtures/ArrayLengthHelper", "SnapshotTestFixtures/ArrayLengthHelper", "", "ArrayLengthHelper", "ArrayLengthHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_ArrayLengthHelper, 1u },
	{ 0u, "SnapshotTestFixtures/ArrayRefHelper", "SnapshotTestFixtures/ArrayRefHelper", "", "ArrayRefHelper", "ArrayRefHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_ArrayRefHelper, 1u },
	{ 0u, "SnapshotTestFixtures/BaseClass", "SnapshotTestFixtures/BaseClass", "", "BaseClass", "BaseClass", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_BaseClass, 2u },
	{ 0u, "SnapshotTestFixtures/BitwiseHelper", "SnapshotTestFixtures/BitwiseHelper", "", "BitwiseHelper", "BitwiseHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_BitwiseHelper, 1u },
	{ 0u, "SnapshotTestFixtures/BoxingHelper", "SnapshotTestFixtures/BoxingHelper", "", "BoxingHelper", "BoxingHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_BoxingHelper, 1u },
	{ 0u, "SnapshotTestFixtures/BoxInterfaceArrayDemo", "SnapshotTestFixtures/BoxInterfaceArrayDemo", "", "BoxInterfaceArrayDemo", "BoxInterfaceArrayDemo", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_BoxInterfaceArrayDemo, 1u },
	{ 0u, "SnapshotTestFixtures/BranchCompareBHelper", "SnapshotTestFixtures/BranchCompareBHelper", "", "BranchCompareBHelper", "BranchCompareBHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_BranchCompareBHelper, 1u },
	{ 0u, "SnapshotTestFixtures/BranchCompareHelper", "SnapshotTestFixtures/BranchCompareHelper", "", "BranchCompareHelper", "BranchCompareHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_BranchCompareHelper, 1u },
	{ 0u, "SnapshotTestFixtures/BranchDupHelper", "SnapshotTestFixtures/BranchDupHelper", "", "BranchDupHelper", "BranchDupHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_BranchDupHelper, 1u },
	{ 0u, "SnapshotTestFixtures/BranchUnsignedBHelper", "SnapshotTestFixtures/BranchUnsignedBHelper", "", "BranchUnsignedBHelper", "BranchUnsignedBHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_BranchUnsignedBHelper, 1u },
	{ 0u, "SnapshotTestFixtures/BranchUnsignedHelper", "SnapshotTestFixtures/BranchUnsignedHelper", "", "BranchUnsignedHelper", "BranchUnsignedHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_BranchUnsignedHelper, 1u },
	{ 0u, "SnapshotTestFixtures/CalliHelper", "SnapshotTestFixtures/CalliHelper", "", "CalliHelper", "CalliHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_CalliHelper, 1u },
	{ 0u, "SnapshotTestFixtures/CallVirtHelper", "SnapshotTestFixtures/CallVirtHelper", "", "CallVirtHelper", "CallVirtHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_CallVirtHelper, 1u },
	{ 0u, "SnapshotTestFixtures/CallvirtHelper", "SnapshotTestFixtures/CallvirtHelper", "", "CallvirtHelper", "CallvirtHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_CallvirtHelper, 1u },
	{ 0u, "SnapshotTestFixtures/CollectionsHelper", "SnapshotTestFixtures/CollectionsHelper", "", "CollectionsHelper", "CollectionsHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_CollectionsHelper, 3u },
	{ 0u, "SnapshotTestFixtures/ConversionHelper", "SnapshotTestFixtures/ConversionHelper", "", "ConversionHelper", "ConversionHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_ConversionHelper, 1u },
	{ 0u, "SnapshotTestFixtures/ConvSmallIntHelper", "SnapshotTestFixtures/ConvSmallIntHelper", "", "ConvSmallIntHelper", "ConvSmallIntHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_ConvSmallIntHelper, 1u },
	{ 0u, "SnapshotTestFixtures/ConvWideHelper", "SnapshotTestFixtures/ConvWideHelper", "", "ConvWideHelper", "ConvWideHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_ConvWideHelper, 1u },
	{ 0u, "SnapshotTestFixtures/CpblkHelper", "SnapshotTestFixtures/CpblkHelper", "", "CpblkHelper", "CpblkHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_CpblkHelper, 1u },
	{ 0u, "SnapshotTestFixtures/CpobjHelper", "SnapshotTestFixtures/CpobjHelper", "", "CpobjHelper", "CpobjHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_CpobjHelper, 1u },
	{ 0u, "SnapshotTestFixtures/DelegateHelper", "SnapshotTestFixtures/DelegateHelper", "", "DelegateHelper", "DelegateHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_DelegateHelper, 1u },
	{ 0u, "SnapshotTestFixtures/DerivedClass", "SnapshotTestFixtures/DerivedClass", "", "DerivedClass", "DerivedClass", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_DerivedClass, 2u },
	{ 0u, "SnapshotTestFixtures/EhHelper", "SnapshotTestFixtures/EhHelper", "", "EhHelper", "EhHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_EhHelper, 1u },
	{ 0u, "SnapshotTestFixtures/ExternalCaller", "SnapshotTestFixtures/ExternalCaller", "", "ExternalCaller", "ExternalCaller", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_ExternalCaller, 1u },
	{ 0u, "SnapshotTestFixtures/FieldHelper", "SnapshotTestFixtures/FieldHelper", "", "FieldHelper", "FieldHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_FieldHelper, 1u },
	{ 0u, "SnapshotTestFixtures/FloatOpsHelper", "SnapshotTestFixtures/FloatOpsHelper", "", "FloatOpsHelper", "FloatOpsHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_FloatOpsHelper, 1u },
	{ 0u, "SnapshotTestFixtures/FlowControl", "SnapshotTestFixtures/FlowControl", "", "FlowControl", "FlowControl", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_FlowControl, 1u },
	{ 0u, "SnapshotTestFixtures/GenericHelper", "SnapshotTestFixtures/GenericHelper", "", "GenericHelper", "GenericHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_GenericHelper, 1u },
	{ 0u, "SnapshotTestFixtures/GenericsVirtEhDemo", "SnapshotTestFixtures/GenericsVirtEhDemo", "", "GenericsVirtEhDemo", "GenericsVirtEhDemo", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_GenericsVirtEhDemo, 1u },
	{ 0u, "SnapshotTestFixtures/HasFields", "SnapshotTestFixtures/HasFields", "", "HasFields", "HasFields", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_HasFields, 1u },
	{ 0u, "SnapshotTestFixtures/HasInstanceFields", "SnapshotTestFixtures/HasInstanceFields", "", "HasInstanceFields", "HasInstanceFields", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_HasInstanceFields, 1u },
	{ 0u, "SnapshotTestFixtures/Helper", "SnapshotTestFixtures/Helper", "", "Helper", "Helper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_Helper, 4u },
	{ 0u, "SnapshotTestFixtures/HotUpdateWithGenericsDemo", "SnapshotTestFixtures/HotUpdateWithGenericsDemo", "", "HotUpdateWithGenericsDemo", "HotUpdateWithGenericsDemo", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_HotUpdateWithGenericsDemo, 1u },
	{ 0u, "SnapshotTestFixtures/HotUpdateWithTypesDemo", "SnapshotTestFixtures/HotUpdateWithTypesDemo", "", "HotUpdateWithTypesDemo", "HotUpdateWithTypesDemo", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_HotUpdateWithTypesDemo, 1u },
	{ 0u, "SnapshotTestFixtures/ImplHelper", "SnapshotTestFixtures/ImplHelper", "", "ImplHelper", "ImplHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_ImplHelper, 2u },
	{ 0u, "SnapshotTestFixtures/IndirectHelper", "SnapshotTestFixtures/IndirectHelper", "", "IndirectHelper", "IndirectHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_IndirectHelper, 1u },
	{ 0u, "SnapshotTestFixtures/InstanceFieldHelper", "SnapshotTestFixtures/InstanceFieldHelper", "", "InstanceFieldHelper", "InstanceFieldHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_InstanceFieldHelper, 1u },
	{ 0u, "SnapshotTestFixtures/InstanceHelper", "SnapshotTestFixtures/InstanceHelper", "", "InstanceHelper", "InstanceHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_InstanceHelper, 2u },
	{ 0u, "SnapshotTestFixtures/InterfaceDevirtHelper", "SnapshotTestFixtures/InterfaceDevirtHelper", "", "InterfaceDevirtHelper", "InterfaceDevirtHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_InterfaceDevirtHelper, 1u },
	{ 0u, "SnapshotTestFixtures/InternalHelper", "SnapshotTestFixtures/InternalHelper", "", "InternalHelper", "InternalHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_InternalHelper, 1u },
	{ 0u, "SnapshotTestFixtures/LdargaHelper", "SnapshotTestFixtures/LdargaHelper", "", "LdargaHelper", "LdargaHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_LdargaHelper, 1u },
	{ 0u, "SnapshotTestFixtures/LdcI8Helper", "SnapshotTestFixtures/LdcI8Helper", "", "LdcI8Helper", "LdcI8Helper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_LdcI8Helper, 1u },
	{ 0u, "SnapshotTestFixtures/LdelemaHelper", "SnapshotTestFixtures/LdelemaHelper", "", "LdelemaHelper", "LdelemaHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_LdelemaHelper, 1u },
	{ 0u, "SnapshotTestFixtures/LdelemAllHelper", "SnapshotTestFixtures/LdelemAllHelper", "", "LdelemAllHelper", "LdelemAllHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_LdelemAllHelper, 1u },
	{ 0u, "SnapshotTestFixtures/LdftnHelper", "SnapshotTestFixtures/LdftnHelper", "", "LdftnHelper", "LdftnHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_LdftnHelper, 1u },
	{ 0u, "SnapshotTestFixtures/LdindI1Helper", "SnapshotTestFixtures/LdindI1Helper", "", "LdindI1Helper", "LdindI1Helper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_LdindI1Helper, 1u },
	{ 0u, "SnapshotTestFixtures/LdindI8Helper", "SnapshotTestFixtures/LdindI8Helper", "", "LdindI8Helper", "LdindI8Helper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_LdindI8Helper, 1u },
	{ 0u, "SnapshotTestFixtures/LdindStindGapsHelper", "SnapshotTestFixtures/LdindStindGapsHelper", "", "LdindStindGapsHelper", "LdindStindGapsHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_LdindStindGapsHelper, 3u },
	{ 0u, "SnapshotTestFixtures/LdindUnsignedHelper", "SnapshotTestFixtures/LdindUnsignedHelper", "", "LdindUnsignedHelper", "LdindUnsignedHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_LdindUnsignedHelper, 1u },
	{ 0u, "SnapshotTestFixtures/LdindWideHelper", "SnapshotTestFixtures/LdindWideHelper", "", "LdindWideHelper", "LdindWideHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_LdindWideHelper, 1u },
	{ 0u, "SnapshotTestFixtures/LdtokenHelper", "SnapshotTestFixtures/LdtokenHelper", "", "LdtokenHelper", "LdtokenHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_LdtokenHelper, 1u },
	{ 0u, "SnapshotTestFixtures/LdVirtftnHelper", "SnapshotTestFixtures/LdVirtftnHelper", "", "LdVirtftnHelper", "LdVirtftnHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_LdVirtftnHelper, 1u },
	{ 0u, "SnapshotTestFixtures/LocalAllocHelper", "SnapshotTestFixtures/LocalAllocHelper", "", "LocalAllocHelper", "LocalAllocHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_LocalAllocHelper, 1u },
	{ 0u, "SnapshotTestFixtures/LoopHelper", "SnapshotTestFixtures/LoopHelper", "", "LoopHelper", "LoopHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_LoopHelper, 1u },
	{ 0u, "SnapshotTestFixtures/MarkedClass", "SnapshotTestFixtures/MarkedClass", "", "MarkedClass", "MarkedClass", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_MarkedClass, 1u },
	{ 0u, "SnapshotTestFixtures/MathHelper", "SnapshotTestFixtures/MathHelper", "", "MathHelper", "MathHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_MathHelper, 1u },
	{ 0u, "SnapshotTestFixtures/MoreRareOpsHelper", "SnapshotTestFixtures/MoreRareOpsHelper", "", "MoreRareOpsHelper", "MoreRareOpsHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_MoreRareOpsHelper, 5u },
	{ 0u, "SnapshotTestFixtures/MulDivHelper", "SnapshotTestFixtures/MulDivHelper", "", "MulDivHelper", "MulDivHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_MulDivHelper, 1u },
	{ 0u, "SnapshotTestFixtures/MultiDispatcher", "SnapshotTestFixtures/MultiDispatcher", "", "MultiDispatcher", "MultiDispatcher", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_MultiDispatcher, 1u },
	{ 0u, "SnapshotTestFixtures/MyClass", "SnapshotTestFixtures/MyClass", "", "MyClass", "MyClass", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_MyClass, 2u },
	{ 0u, "SnapshotTestFixtures/NegShiftHelper", "SnapshotTestFixtures/NegShiftHelper", "", "NegShiftHelper", "NegShiftHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_NegShiftHelper, 1u },
	{ 0u, "SnapshotTestFixtures/OverflowConvExtHelper", "SnapshotTestFixtures/OverflowConvExtHelper", "", "OverflowConvExtHelper", "OverflowConvExtHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_OverflowConvExtHelper, 11u },
	{ 0u, "SnapshotTestFixtures/OverflowConvHelper", "SnapshotTestFixtures/OverflowConvHelper", "", "OverflowConvHelper", "OverflowConvHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_OverflowConvHelper, 7u },
	{ 0u, "SnapshotTestFixtures/OverflowHelper", "SnapshotTestFixtures/OverflowHelper", "", "OverflowHelper", "OverflowHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_OverflowHelper, 3u },
	{ 0u, "SnapshotTestFixtures/OverflowUnHelper", "SnapshotTestFixtures/OverflowUnHelper", "", "OverflowUnHelper", "OverflowUnHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_OverflowUnHelper, 3u },
	{ 0u, "SnapshotTestFixtures/RareOpsHelper", "SnapshotTestFixtures/RareOpsHelper", "", "RareOpsHelper", "RareOpsHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_RareOpsHelper, 3u },
	{ 0u, "SnapshotTestFixtures/RethrowHelper", "SnapshotTestFixtures/RethrowHelper", "", "RethrowHelper", "RethrowHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_RethrowHelper, 1u },
	{ 0u, "SnapshotTestFixtures/SealedClassVirtualHelper", "SnapshotTestFixtures/SealedClassVirtualHelper", "", "SealedClassVirtualHelper", "SealedClassVirtualHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_SealedClassVirtualHelper, 1u },
	{ 0u, "SnapshotTestFixtures/SealedHelper", "SnapshotTestFixtures/SealedHelper", "", "SealedHelper", "SealedHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_SealedHelper, 2u },
	{ 0u, "SnapshotTestFixtures/ShiftHelper", "SnapshotTestFixtures/ShiftHelper", "", "ShiftHelper", "ShiftHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_ShiftHelper, 1u },
	{ 0u, "SnapshotTestFixtures/SimpleGapsHelper", "SnapshotTestFixtures/SimpleGapsHelper", "", "SimpleGapsHelper", "SimpleGapsHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_SimpleGapsHelper, 7u },
	{ 0u, "SnapshotTestFixtures/SizeOfHelper", "SnapshotTestFixtures/SizeOfHelper", "", "SizeOfHelper", "SizeOfHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_SizeOfHelper, 1u },
	{ 0u, "SnapshotTestFixtures/StaticFieldWriteHelper", "SnapshotTestFixtures/StaticFieldWriteHelper", "", "StaticFieldWriteHelper", "StaticFieldWriteHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_StaticFieldWriteHelper, 1u },
	{ 0u, "SnapshotTestFixtures/StelemAllHelper", "SnapshotTestFixtures/StelemAllHelper", "", "StelemAllHelper", "StelemAllHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_StelemAllHelper, 1u },
	{ 0u, "SnapshotTestFixtures/StindNarrowHelper", "SnapshotTestFixtures/StindNarrowHelper", "", "StindNarrowHelper", "StindNarrowHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_StindNarrowHelper, 1u },
	{ 0u, "SnapshotTestFixtures/StindWideHelper", "SnapshotTestFixtures/StindWideHelper", "", "StindWideHelper", "StindWideHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_StindWideHelper, 1u },
	{ 0u, "SnapshotTestFixtures/StringConcatHelper", "SnapshotTestFixtures/StringConcatHelper", "", "StringConcatHelper", "StringConcatHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_StringConcatHelper, 1u },
	{ 0u, "SnapshotTestFixtures/StringFormatHelper", "SnapshotTestFixtures/StringFormatHelper", "", "StringFormatHelper", "StringFormatHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_StringFormatHelper, 1u },
	{ 0u, "SnapshotTestFixtures/StringOps", "SnapshotTestFixtures/StringOps", "", "StringOps", "StringOps", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_StringOps, 1u },
	{ 0u, "SnapshotTestFixtures/SwitchHelper", "SnapshotTestFixtures/SwitchHelper", "", "SwitchHelper", "SwitchHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_SwitchHelper, 1u },
	{ 0u, "SnapshotTestFixtures/TargetHelper", "SnapshotTestFixtures/TargetHelper", "", "TargetHelper", "TargetHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_TargetHelper, 1u },
	{ 0u, "SnapshotTestFixtures/ThrowHelper", "SnapshotTestFixtures/ThrowHelper", "", "ThrowHelper", "ThrowHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_ThrowHelper, 1u },
	{ 0u, "SnapshotTestFixtures/TypeCheckHelper", "SnapshotTestFixtures/TypeCheckHelper", "", "TypeCheckHelper", "TypeCheckHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_TypeCheckHelper, 1u },
	{ 0u, "SnapshotTestFixtures/UnboxHelper", "SnapshotTestFixtures/UnboxHelper", "", "UnboxHelper", "UnboxHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_UnboxHelper, 1u },
	{ 0u, "SnapshotTestFixtures/UnsignedOpsHelper", "SnapshotTestFixtures/UnsignedOpsHelper", "", "UnsignedOpsHelper", "UnsignedOpsHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_UnsignedOpsHelper, 4u },
	{ 0u, "SnapshotTestFixtures/ValueTypeHelper", "SnapshotTestFixtures/ValueTypeHelper", "", "ValueTypeHelper", "ValueTypeHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_ValueTypeHelper, 1u },
	{ 0u, "SnapshotTestFixtures/VirtualDispatchHelper", "SnapshotTestFixtures/VirtualDispatchHelper", "", "VirtualDispatchHelper", "VirtualDispatchHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_VirtualDispatchHelper, 1u },
	{ 0u, "SnapshotTestFixtures/VirtualHelper", "SnapshotTestFixtures/VirtualHelper", "", "VirtualHelper", "VirtualHelper", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_VirtualHelper, 1u },
	{ 0u, "SnapshotTestFixtures/VoidCaller", "SnapshotTestFixtures/VoidCaller", "", "VoidCaller", "VoidCaller", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_VoidCaller, 1u },
	{ 0u, "SnapshotTestFixtures/Wrapper`1", "SnapshotTestFixtures/Wrapper`1", "", "Wrapper`1", "Wrapper`1", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_Wrapper_1, 2u },
	{ 0u, "SnapshotTestFixtures/Wrapper<!0>", "SnapshotTestFixtures/Wrapper<!0>", "", "Wrapper<!0>", "Wrapper<!0>", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_Wrapper__0_, 2u },
	{ 0u, "SnapshotTestFixtures/Wrapper<System.Int32>", "SnapshotTestFixtures/Wrapper<System.Int32>", "Wrapper<System", "Wrapper<System.Int32>", "Wrapper<System.Int32>", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_SnapshotTestFixtures_Wrapper_System_Int32_, 2u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[97] = {
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
	&kReflTypes[11],
	&kReflTypes[12],
	&kReflTypes[13],
	&kReflTypes[14],
	&kReflTypes[15],
	&kReflTypes[16],
	&kReflTypes[17],
	&kReflTypes[18],
	&kReflTypes[19],
	&kReflTypes[20],
	&kReflTypes[21],
	&kReflTypes[22],
	&kReflTypes[23],
	&kReflTypes[24],
	&kReflTypes[25],
	&kReflTypes[26],
	&kReflTypes[27],
	&kReflTypes[28],
	&kReflTypes[29],
	&kReflTypes[30],
	&kReflTypes[31],
	&kReflTypes[32],
	&kReflTypes[33],
	&kReflTypes[34],
	&kReflTypes[35],
	&kReflTypes[36],
	&kReflTypes[37],
	&kReflTypes[38],
	&kReflTypes[39],
	&kReflTypes[40],
	&kReflTypes[41],
	&kReflTypes[42],
	&kReflTypes[43],
	&kReflTypes[44],
	&kReflTypes[45],
	&kReflTypes[46],
	&kReflTypes[47],
	&kReflTypes[48],
	&kReflTypes[49],
	&kReflTypes[50],
	&kReflTypes[51],
	&kReflTypes[52],
	&kReflTypes[53],
	&kReflTypes[54],
	&kReflTypes[55],
	&kReflTypes[56],
	&kReflTypes[57],
	&kReflTypes[58],
	&kReflTypes[59],
	&kReflTypes[60],
	&kReflTypes[61],
	&kReflTypes[62],
	&kReflTypes[63],
	&kReflTypes[64],
	&kReflTypes[65],
	&kReflTypes[66],
	&kReflTypes[67],
	&kReflTypes[68],
	&kReflTypes[69],
	&kReflTypes[70],
	&kReflTypes[71],
	&kReflTypes[72],
	&kReflTypes[73],
	&kReflTypes[74],
	&kReflTypes[75],
	&kReflTypes[76],
	&kReflTypes[77],
	&kReflTypes[78],
	&kReflTypes[79],
	&kReflTypes[80],
	&kReflTypes[81],
	&kReflTypes[82],
	&kReflTypes[83],
	&kReflTypes[84],
	&kReflTypes[85],
	&kReflTypes[86],
	&kReflTypes[87],
	&kReflTypes[88],
	&kReflTypes[89],
	&kReflTypes[90],
	&kReflTypes[91],
	&kReflTypes[92],
	&kReflTypes[93],
	&kReflTypes[94],
	&kReflTypes[95],
	&kReflTypes[96],
};

static const ReflectionQueryImageDescriptor kReflImage = { "SnapshotProverSubjects", kReflTypePtrs, 97u };

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
// Managed method: SnapshotProverSubjects/SnapshotProverSubjects::Subject_0()
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	return;
}

// Managed method: SnapshotProverSubjects/SnapshotProverSubjects::Subject_1()
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	return;
}

// Managed method: SnapshotProverSubjects/SnapshotProverSubjects::Subject_2()
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_2(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	return;
}

// Managed method: SnapshotProverSubjects/SnapshotProverSubjects::Subject_3()
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_3(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	return;
}

// Managed method: SnapshotProverSubjects/SnapshotProverSubjects::Subject_4()
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_4(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	return;
}

// Managed method: SnapshotProverSubjects/SnapshotProverSubjects::Subject_5()
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_5(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	return;
}

// Managed method: SnapshotProverSubjects/SnapshotProverSubjects::Subject_6()
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_6(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	return;
}

// Managed method: SnapshotProverSubjects/SnapshotProverSubjects::Subject_7()
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_7(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	return;
}

// Managed method: SnapshotProverSubjects/SnapshotProverSubjects::Subject_8()
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_8(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	return;
}

// Managed method: SnapshotProverSubjects/SnapshotProverSubjects::Subject_9()
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_9(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	return;
}

// Managed method: SnapshotTestFixtures/AddressHelper::RunAddress()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_AddressHelper_RunAddress(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_SnapshotTestFixtures_HasFields, {});
		chaos_object->header.type_info = &chaos_mt_SnapshotTestFixtures_HasFields.hot;
		SnapshotTestFixtures_HasFields__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	{
		auto* chaos_object = reinterpret_cast<chaos_type_SnapshotTestFixtures_HasFields*>(_s0);
		_s0 = chaos_object->field_SnapshotTestFixtures_HasFields__instanceVal;
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/ArithmeticCompareHelper::RunCompare()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_ArithmeticCompareHelper_RunCompare(void)
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
	CHAOS_IL2CPP_INTPTR _s21{};
	CHAOS_IL2CPP_INTPTR _s22{};
	CHAOS_IL2CPP_INTPTR _s23{};
	CHAOS_IL2CPP_INTPTR _s24{};
	CHAOS_IL2CPP_INTPTR _s25{};
	CHAOS_IL2CPP_INTPTR _s26{};
	CHAOS_IL2CPP_INTPTR _s27{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
	chaos_locals[0] = _s1;
	_s1 = _s0;
	_s2 = chaos_locals[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapSub(static_cast<CHAOS_IL2CPP_INT32>(_s1), static_cast<CHAOS_IL2CPP_INT32>(_s2)));
	chaos_locals[1] = _s1;
	_s1 = _s0;
	_s2 = chaos_locals[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosRem(static_cast<CHAOS_IL2CPP_INT32>(_s1), static_cast<CHAOS_IL2CPP_INT32>(_s2)));
	chaos_locals[2] = _s1;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[3] = _s1;
	_s1 = _s0;
	_s2 = chaos_locals[0];
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s2);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		if (chaos_left == chaos_right)
		{
			_s1 = chaos_locals[3];
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s1), static_cast<CHAOS_IL2CPP_INT32>(_s2)));
			chaos_locals[3] = _s1;
		}
		_s1 = _s0;
		_s2 = chaos_locals[0];
		{
			const auto chaos_right = static_cast<CHAOS_IL2CPP_INT32>(_s2);
			const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>(_s1);
			if (chaos_left < chaos_right)
			{
				_s1 = chaos_locals[3];
				_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s1), static_cast<CHAOS_IL2CPP_INT32>(_s2)));
				chaos_locals[3] = _s1;
			}
			_s1 = chaos_locals[0];
			{
				const auto chaos_right = static_cast<CHAOS_IL2CPP_INT32>(_s1);
				const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>(_s0);
				if (chaos_left > chaos_right)
				{
					_s0 = chaos_locals[3];
					_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
					chaos_locals[3] = _s0;
				}
				_s0 = chaos_locals[1];
				_s1 = chaos_locals[2];
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
				_s1 = chaos_locals[3];
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
				return static_cast<CHAOS_IL2CPP_INT32>(_s0);
			}
		}
	}
}

// Managed method: SnapshotTestFixtures/ArithmeticOps::RunAdd()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_ArithmeticOps_RunAdd(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(7);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/ArrayHelper::MakeAndFill()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_ArrayHelper_MakeAndFill(void)
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
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s1);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s0);
		if (chaos_array == nullptr)
		{
			CHAOS_IL2CPP_FAIL();
		}
		if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_element = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];
		_s0 = chaos_element;
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/ArrayLengthHelper::RunLength()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_ArrayLengthHelper_RunLength(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
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
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/ArrayRefHelper::RunArrayRef()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_ArrayRefHelper_RunArrayRef(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(10);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/BaseClass::.ctor()
extern "C" void SnapshotTestFixtures_BaseClass__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: SnapshotTestFixtures/BaseClass::Compute()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_BaseClass_Compute(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/BitwiseHelper::RunBitwise()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_BitwiseHelper_RunBitwise(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(165);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(90);
	chaos_locals[0] = _s1;
	_s1 = _s0;
	_s2 = chaos_locals[0];
	_s1 = chaos_store_uint64(chaos_load_uint64(_s1) & chaos_load_uint64(_s2));
	chaos_locals[1] = _s1;
	_s1 = _s0;
	_s2 = chaos_locals[0];
	_s1 = chaos_store_uint64(chaos_load_uint64(_s1) | chaos_load_uint64(_s2));
	chaos_locals[2] = _s1;
	_s1 = _s0;
	_s2 = chaos_locals[0];
	_s1 = chaos_store_uint64(chaos_load_uint64(_s1) ^ chaos_load_uint64(_s2));
	chaos_locals[3] = _s1;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(~static_cast<CHAOS_IL2CPP_INT32>(_s0));
	chaos_locals[4] = _s0;
	_s0 = chaos_locals[1];
	_s1 = chaos_locals[2];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	_s1 = chaos_locals[3];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	_s1 = chaos_locals[4];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/BoxingHelper::BoxAndUnbox()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_BoxingHelper_BoxAndUnbox(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	chaos_boxed_type_System_Private_CoreLib_System_Int32 chaos_box_storage_0{};
	{
		const auto chaos_value = _s0;
		chaos_box_storage_0.header.type_info = &chaos_mt_System_Private_CoreLib_System_Int32.hot;
		chaos_box_storage_0.value = chaos_value;
	}
	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_box_storage_0);
	{
		auto* chaos_boxed = reinterpret_cast<chaos_boxed_type_System_Private_CoreLib_System_Int32*>(_s0);
		if (chaos_boxed == nullptr)
		{
			CHAOS_IL2CPP_FAIL();
		}
		_s0 = chaos_boxed->value;
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/BoxInterfaceArrayDemo::DemoBoxStore()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_BoxInterfaceArrayDemo_DemoBoxStore(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(47);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/BranchCompareBHelper::RunBranchCompareB()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_BranchCompareBHelper_RunBranchCompareB(void)
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
	CHAOS_IL2CPP_INTPTR _s22{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
	chaos_locals[0] = _s0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
	chaos_locals[1] = _s0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[2] = _s1;
	_s1 = chaos_locals[0];
	_s2 = chaos_locals[1];
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INT32>(_s2);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>(_s1);
		if (chaos_left <= chaos_right)
		{
			_s1 = chaos_locals[2];
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s1), static_cast<CHAOS_IL2CPP_INT32>(_s2)));
			chaos_locals[2] = _s1;
		}
		_s1 = chaos_locals[0];
		{
			const auto chaos_right = static_cast<CHAOS_IL2CPP_INT32>(_s1);
			const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>(_s0);
			if (chaos_left >= chaos_right)
			{
				_s0 = chaos_locals[2];
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
				chaos_locals[2] = _s0;
			}
			_s0 = chaos_locals[1];
			_s0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s1 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s1));
			{
				const auto chaos_right = static_cast<CHAOS_IL2CPP_INT32>(_s1);
				const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>(_s0);
				if (chaos_left >= chaos_right)
				{
					_s0 = chaos_locals[2];
					_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
					chaos_locals[2] = _s0;
				}
				_s0 = chaos_locals[2];
				return static_cast<CHAOS_IL2CPP_INT32>(_s0);
			}
		}
	}
}

// Managed method: SnapshotTestFixtures/BranchCompareHelper::RunBranchCompare()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_BranchCompareHelper_RunBranchCompare(void)
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
	CHAOS_IL2CPP_INTPTR _s21{};
	CHAOS_IL2CPP_INTPTR _s22{};
	CHAOS_IL2CPP_INTPTR _s23{};
	CHAOS_IL2CPP_INTPTR _s24{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
	chaos_locals[0] = _s0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
	chaos_locals[1] = _s0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
	chaos_locals[2] = _s0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[3] = _s0;
	_s0 = chaos_locals[0];
	_s1 = chaos_locals[2];
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		if (chaos_left == chaos_right)
		{
			_s0 = chaos_locals[3];
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
			chaos_locals[3] = _s0;
		}
		_s0 = chaos_locals[0];
		_s1 = chaos_locals[1];
		{
			const auto chaos_right = static_cast<CHAOS_IL2CPP_INT32>(_s1);
			const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>(_s0);
			if (chaos_left > chaos_right)
			{
				_s0 = chaos_locals[3];
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
				chaos_locals[3] = _s0;
			}
			_s0 = chaos_locals[1];
			_s1 = chaos_locals[0];
			{
				const auto chaos_right = static_cast<CHAOS_IL2CPP_INT32>(_s1);
				const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>(_s0);
				if (chaos_left < chaos_right)
				{
					_s0 = chaos_locals[3];
					_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
					chaos_locals[3] = _s0;
				}
				_s0 = chaos_locals[0];
				_s1 = chaos_locals[1];
				{
					const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
					const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
					if (chaos_left != chaos_right)
					{
						_s0 = chaos_locals[3];
						_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(8);
						_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
						chaos_locals[3] = _s0;
					}
					_s0 = chaos_locals[3];
					return static_cast<CHAOS_IL2CPP_INT32>(_s0);
				}
			}
		}
	}
}

// Managed method: SnapshotTestFixtures/BranchDupHelper::RunBranchDup()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_BranchDupHelper_RunBranchDup(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[0] = _s1;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		if (chaos_left == chaos_right)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			chaos_locals[0] = _s0;
		}
		_s0 = chaos_locals[0];
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		{
			const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
			const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
			if (chaos_left == chaos_right)
			{
				_s0 = chaos_locals[0];
				chaos_boxed_type_System_Private_CoreLib_System_Int32 chaos_box_storage_0{};
				{
					const auto chaos_value = _s0;
					chaos_box_storage_0.header.type_info = &chaos_mt_System_Private_CoreLib_System_Int32.hot;
					chaos_box_storage_0.value = chaos_value;
				}
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_box_storage_0);
			}
			else
			{
				_s0 = 0;
			}
			{
				if (_s0 != 0)
				{
					_s0 = chaos_locals[0];
					_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
					chaos_locals[0] = _s0;
				}
				_s0 = chaos_locals[0];
				return static_cast<CHAOS_IL2CPP_INT32>(_s0);
			}
		}
	}
}

// Managed method: SnapshotTestFixtures/BranchUnsignedBHelper::RunBranchUnsignedB()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_BranchUnsignedBHelper_RunBranchUnsignedB(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
	chaos_locals[0] = _s1;
	_s1 = chaos_locals[0];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapMul(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/BranchUnsignedHelper::RunBranchUnsigned()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_BranchUnsignedHelper_RunBranchUnsigned(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
	chaos_locals[0] = _s1;
	_s1 = chaos_locals[0];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/CalliHelper::RunCalli(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_CalliHelper_RunCalli_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = 0;
	{
		// Hotpatch-aware ldftn wrapper (slot 42)
		static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INT32 chaos_fn_arg_0) -> CHAOS_IL2CPP_INT32 {
			auto& _d_entry = s_hotpatch_entries[42];
			if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
				&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
			{
				alignas(16) uint8_t _d_ab[4];
				ArgBuffer _d_bw(_d_ab);
				_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_fn_arg_0));
				CHAOS_IL2CPP_INT32 _d_ret{};
				::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
					_d_entry.method_key, _d_ab, &_d_ret);
				return _d_ret;
			}
			return reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INT32)>(_d_entry.direct_ptr)(chaos_fn_arg_0);
		};
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
	}
	{
		const auto chaos_method_ptr = _s1;
		const auto chaos_target = _s0;
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Int32_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Int32_.hot;
		chaos_object->chaos_delegate_target = chaos_target;
		chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	_s1 = chaos_args[0];
	{
		auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_delegate_value = _s0;
		if (chaos_delegate_value == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		auto* chaos_delegate = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Int32_*>(chaos_delegate_value);
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
				auto* chaos_invocation_delegate = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Int32_*>(chaos_invocation_delegate_value);
				if (chaos_invocation_delegate->chaos_delegate_method_ptr == 0)
				{
					CHAOS_IL2CPP_FAIL();
				}
				if (chaos_invocation_delegate->chaos_delegate_target == 0)
				{
					const auto chaos_open_function = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)>(chaos_invocation_delegate->chaos_delegate_method_ptr);
					chaos_result = chaos_open_function(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
				}
				else
				{
					const auto chaos_closed_function = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR chaos_delegate_target, CHAOS_IL2CPP_INT32 chaos_fn_arg_0)>(chaos_invocation_delegate->chaos_delegate_method_ptr);
					chaos_result = chaos_closed_function(chaos_invocation_delegate->chaos_delegate_target, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
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
			if (chaos_delegate->chaos_delegate_target == 0)
			{
				const auto chaos_open_function = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)>(chaos_delegate->chaos_delegate_method_ptr);
				const auto chaos_result = chaos_open_function(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			else
			{
				const auto chaos_closed_function = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR chaos_delegate_target, CHAOS_IL2CPP_INT32 chaos_fn_arg_0)>(chaos_delegate->chaos_delegate_method_ptr);
				const auto chaos_result = chaos_closed_function(chaos_delegate->chaos_delegate_target, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
				_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
		}
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s2);
}

// Managed method: SnapshotTestFixtures/CallVirtHelper::CreateAndUse()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_CallVirtHelper_CreateAndUse(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		const auto chaos_arg_1 = _s0;
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_SnapshotTestFixtures_InstanceHelper, {});
		chaos_object->header.type_info = &chaos_mt_SnapshotTestFixtures_InstanceHelper.hot;
		SnapshotTestFixtures_InstanceHelper__ctor_System_Int32(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s0);
		auto& _d50 = s_hotpatch_entries[50];
		CHAOS_IL2CPP_INT32 _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d50)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d50))
		{
			alignas(16) uint8_t _d_ab[8];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d50.method_key, _d_ab, &_d_hpresult);
		}
		else
		{
			_d_hpresult = SnapshotTestFixtures_InstanceHelper_GetValue(chaos_arg_0);
		}
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/CallvirtHelper::RunCallvirt()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_CallvirtHelper_RunCallvirt(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		const auto chaos_arg_1 = _s0;
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_SnapshotTestFixtures_InstanceHelper, {});
		chaos_object->header.type_info = &chaos_mt_SnapshotTestFixtures_InstanceHelper.hot;
		SnapshotTestFixtures_InstanceHelper__ctor_System_Int32(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s0);
		auto& _d50 = s_hotpatch_entries[50];
		CHAOS_IL2CPP_INT32 _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d50)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d50))
		{
			alignas(16) uint8_t _d_ab[8];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d50.method_key, _d_ab, &_d_hpresult);
		}
		else
		{
			_d_hpresult = SnapshotTestFixtures_InstanceHelper_GetValue(chaos_arg_0);
		}
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/CollectionsHelper::TestDict()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_CollectionsHelper_TestDict(void)
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


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Collections_System_Collections_Generic_Dictionary_System_Int32_System_Int32_, {});
		chaos_object->header.type_info = &chaos_mt_System_Collections_System_Collections_Generic_Dictionary_System_Int32_System_Int32_.hot;
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(100);
	{
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		chaos_external_runtime_System_Private_CoreLib_System_Collections_Generic_Dictionary_System_Int32_System_Int32___Add_System_Void_System_Int32_System_Int32_(chaos_arg_0, chaos_arg_1, chaos_arg_2);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Collections_Generic_Dictionary_System_Int32_System_Int32___TryGetValue_System_Boolean_System_Int32_System_Int32__(chaos_arg_0, chaos_arg_1, chaos_arg_2);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[1] = _s1;
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Collections_Generic_Dictionary_System_Int32_System_Int32___ContainsKey_System_Boolean_System_Int32_(chaos_arg_0, chaos_arg_1);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[2] = _s1;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Collections_Generic_Dictionary_System_Int32_System_Int32___Remove_System_Boolean_System_Int32_(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s0 = chaos_locals[1];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s0) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) ? 1 : 0);
	_s1 = chaos_locals[2];
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s2) ? 1 : 0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	_s1 = chaos_locals[0];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/CollectionsHelper::TestList()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_CollectionsHelper_TestList(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Collections_System_Collections_Generic_List_System_Int32_, {});
		chaos_object->header.type_info = &chaos_mt_System_Collections_System_Collections_Generic_List_System_Int32_.hot;
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		chaos_external_runtime_System_Private_CoreLib_System_Collections_Generic_List_System_Int32___Add_System_Void_System_Int32_(chaos_arg_0, chaos_arg_1);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[13])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s1);
}

// Managed method: SnapshotTestFixtures/CollectionsHelper::TestSet()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_CollectionsHelper_TestSet(void)
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


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Collections_System_Collections_Generic_HashSet_System_Int32_, {});
		chaos_object->header.type_info = &chaos_mt_System_Collections_System_Collections_Generic_HashSet_System_Int32_.hot;
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(10);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Collections_Generic_HashSet_System_Int32___Add_System_Boolean_System_Int32_(chaos_arg_0, chaos_arg_1);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(10);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Collections_Generic_HashSet_System_Int32___Contains_System_Boolean_System_Int32_(chaos_arg_0, chaos_arg_1);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[0] = _s1;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(10);
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Collections_Generic_HashSet_System_Int32___Remove_System_Boolean_System_Int32_(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s0 = chaos_locals[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s0) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) ? 1 : 0);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/ConversionHelper::ConvertToInt()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_ConversionHelper_ConvertToInt(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(100);
	_s0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/ConvSmallIntHelper::RunConvSmall()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_ConvSmallIntHelper_RunConvSmall(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(4660);
	_s1 = _s0;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT8>(_s1));
	chaos_locals[0] = _s1;
	_s1 = _s0;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT16>(_s1));
	chaos_locals[1] = _s1;
	_s1 = _s0;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT8>(_s1));
	chaos_locals[2] = _s1;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT16>(_s0));
	chaos_locals[3] = _s0;
	_s0 = chaos_locals[0];
	_s1 = chaos_locals[1];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	_s1 = chaos_locals[2];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	_s1 = chaos_locals[3];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/ConvWideHelper::RunConvWide()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_ConvWideHelper_RunConvWide(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(100);
	_s0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/CpblkHelper::RunCpblk()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_CpblkHelper_RunCpblk(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/CpobjHelper::RunCpobj()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_CpobjHelper_RunCpobj(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/DelegateHelper::RunDelegate()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_DelegateHelper_RunDelegate(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	_s0 = 0;
	{
		// Hotpatch-aware ldftn wrapper (slot 40)
		static auto* chaos_ftn_thunk = +[](void) -> CHAOS_IL2CPP_INT32 {
			auto& _d_entry = s_hotpatch_entries[40];
			if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
				&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
			{
				CHAOS_IL2CPP_INT32 _d_ret{};
				::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
					_d_entry.method_key, nullptr, &_d_ret);
				return _d_ret;
			}
			return reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(_d_entry.direct_ptr)();
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
			if (chaos_delegate->chaos_delegate_target == 0)
			{
				const auto chaos_open_function = reinterpret_cast<CHAOS_IL2CPP_INT32(*)()>(chaos_delegate->chaos_delegate_method_ptr);
				const auto chaos_result = chaos_open_function();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			else
			{
				const auto chaos_closed_function = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR chaos_delegate_target)>(chaos_delegate->chaos_delegate_method_ptr);
				const auto chaos_result = chaos_closed_function(chaos_delegate->chaos_delegate_target);
				_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
		}
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s2);
}

// Managed method: SnapshotTestFixtures/DerivedClass::.ctor()
extern "C" void SnapshotTestFixtures_DerivedClass__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s0);
		auto& _d6 = s_hotpatch_entries[6];
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d6)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d6))
		{
			alignas(16) uint8_t _d_ab[8];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d6.method_key, _d_ab, nullptr);
		}
		else
		{
			SnapshotTestFixtures_BaseClass__ctor(chaos_arg_0);
		}
	}
	return;
}

// Managed method: SnapshotTestFixtures/DerivedClass::Compute()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_DerivedClass_Compute(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/EhHelper::SafeDivide()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_EhHelper_SafeDivide(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	chaos_locals[0] = _s0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[1] = _s0;
#if !defined(CHAOS_IL2CPP_EH_SETJMP) && !defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	try
	{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(100);
			_s1 = chaos_locals[1];
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosDiv(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
			chaos_locals[0] = _s0;
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
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1);
			chaos_locals[0] = _s0;
	}
#elif defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	__try
	{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(100);
			_s1 = chaos_locals[1];
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosDiv(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
			chaos_locals[0] = _s0;
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
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1);
			chaos_locals[0] = _s0;
	}
#else
	{
		auto* _chaos_jmp =
			chaos::il2cpp::runtime_core::push_exception_jmp_buf();
		if (setjmp(*_chaos_jmp) == 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(100);
			_s1 = chaos_locals[1];
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosDiv(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
			chaos_locals[0] = _s0;
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
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1);
			chaos_locals[0] = _s0;
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/ExternalCaller::CallExternal()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_ExternalCaller_CallExternal(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/FieldHelper::GetAndIncrement()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_FieldHelper_GetAndIncrement(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	_s0 = chaos_static_SnapshotTestFixtures_FieldHelper___counter;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	{
		auto chaos_value = _s0;
		chaos_static_SnapshotTestFixtures_FieldHelper___counter = chaos_value;
	}
	_s0 = chaos_static_SnapshotTestFixtures_FieldHelper___counter;
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/FloatOpsHelper::RunFloatOps()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_FloatOpsHelper_RunFloatOps(void)
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


	_s0 = ChaosStoreFloat32(3.5f);
	_s1 = ChaosStoreFloat64(7.2);
	_s1 = ChaosStoreFloat32(static_cast<float>(_s1));
	chaos_locals[0] = _s1;
	_s1 = chaos_locals[0];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(ChaosLoadFloat64(_s0), ChaosLoadFloat64(_s1)));
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/FlowControl::IsPositive()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_FlowControl_IsPositive(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s0) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) ? 1 : 0);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/GenericHelper::UseGeneric()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_GenericHelper_UseGeneric(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		const auto chaos_arg_1 = _s0;
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_SnapshotTestFixtures_Wrapper_System_Int32_, {});
		chaos_object->header.type_info = &chaos_mt_SnapshotTestFixtures_Wrapper_System_Int32_.hot;
		chaos_stub_definition_SnapshotTestFixtures_Wrapper_1___ctor_System_Void__0__type__System_Int32__method(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s0);
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_stub_definition_SnapshotTestFixtures_Wrapper_1__GetValue__0___type__System_Int32__method(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/GenericsVirtEhDemo::DemoCombine()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_GenericsVirtEhDemo_DemoCombine(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


#if !defined(CHAOS_IL2CPP_EH_SETJMP) && !defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	try
	{
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Collections_System_Collections_Generic_List_System_Int32_, {});
				chaos_object->header.type_info = &chaos_mt_System_Collections_System_Collections_Generic_List_System_Int32_.hot;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[13])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[0] = _s1;
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
		_s1 = chaos_exception.object_value;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(-1);
			chaos_locals[0] = _s1;
	}
#elif defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	__try
	{
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Collections_System_Collections_Generic_List_System_Int32_, {});
				chaos_object->header.type_info = &chaos_mt_System_Collections_System_Collections_Generic_List_System_Int32_.hot;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[13])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[0] = _s1;
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
		_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(-1);
			chaos_locals[0] = _s1;
	}
#else
	{
		auto* _chaos_jmp =
			chaos::il2cpp::runtime_core::push_exception_jmp_buf();
		if (setjmp(*_chaos_jmp) == 0)
		{
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Collections_System_Collections_Generic_List_System_Int32_, {});
				chaos_object->header.type_info = &chaos_mt_System_Collections_System_Collections_Generic_List_System_Int32_.hot;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[13])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[0] = _s1;
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
		_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(-1);
			chaos_locals[0] = _s1;
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	_s1 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s1);
}

// Managed method: SnapshotTestFixtures/HasFields::.ctor()
extern "C" void SnapshotTestFixtures_HasFields__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: SnapshotTestFixtures/HasInstanceFields::.ctor(System.Int32)
extern "C" void SnapshotTestFixtures_HasInstanceFields__ctor_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(chaos_arg_0);
	}
	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	{
		auto chaos_value = _s1;
		if (chaos_is_string_id(chaos_value))
		{
			chaos_value = chaos_string_materialize(chaos_value);
		}
		auto* chaos_object = reinterpret_cast<chaos_type_SnapshotTestFixtures_HasInstanceFields*>(_s0);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_object->field_SnapshotTestFixtures_HasInstanceFields__Value));
		chaos_object->field_SnapshotTestFixtures_HasInstanceFields__Value = chaos_value;
		chaos_gc_dirty_card(chaos_object);
	}
	return;
}

// Managed method: SnapshotTestFixtures/Helper::ConsumeString(System.String)
extern "C" void SnapshotTestFixtures_Helper_ConsumeString_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	return;
}

// Managed method: SnapshotTestFixtures/Helper::GetValue()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_Helper_GetValue(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/Helper::Nop()
extern "C" void SnapshotTestFixtures_Helper_Nop(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: SnapshotTestFixtures/Helper::Square(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_Helper_Square_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	_s1 = chaos_args[0];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapMul(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/HotUpdateWithGenericsDemo::Run()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_HotUpdateWithGenericsDemo_Run(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/HotUpdateWithTypesDemo::Run()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_HotUpdateWithTypesDemo_Run(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/ImplHelper::.ctor(System.Int32)
extern "C" void SnapshotTestFixtures_ImplHelper__ctor_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(chaos_arg_0);
	}
	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	{
		auto chaos_value = _s1;
		if (chaos_is_string_id(chaos_value))
		{
			chaos_value = chaos_string_materialize(chaos_value);
		}
		auto* chaos_object = reinterpret_cast<chaos_type_SnapshotTestFixtures_ImplHelper*>(_s0);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_object->field_SnapshotTestFixtures_ImplHelper___val));
		chaos_object->field_SnapshotTestFixtures_ImplHelper___val = chaos_value;
		chaos_gc_dirty_card(chaos_object);
	}
	return;
}

// Managed method: SnapshotTestFixtures/ImplHelper::GetValue()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_ImplHelper_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_SnapshotTestFixtures_ImplHelper*>(_s0);
		_s0 = chaos_object->field_SnapshotTestFixtures_ImplHelper___val;
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/IndirectHelper::ReadWriteRef()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_IndirectHelper_ReadWriteRef(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	chaos_locals[0] = _s0;
	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(100);
	{
		auto chaos_value_raw = _s1;
		const auto chaos_value = static_cast<CHAOS_IL2CPP_INT32>(chaos_value_raw);
		const auto chaos_address = _s0;
		chaos_store_indirect<CHAOS_IL2CPP_INT32>(chaos_address, chaos_value);
	}
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/InstanceFieldHelper::CreateAndUseFields()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_InstanceFieldHelper_CreateAndUseFields(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		const auto chaos_arg_1 = _s0;
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_SnapshotTestFixtures_HasInstanceFields, {});
		chaos_object->header.type_info = &chaos_mt_SnapshotTestFixtures_HasInstanceFields.hot;
		SnapshotTestFixtures_HasInstanceFields__ctor_System_Int32(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(100);
	{
		auto chaos_value = _s2;
		if (chaos_is_string_id(chaos_value))
		{
			chaos_value = chaos_string_materialize(chaos_value);
		}
		auto* chaos_object = reinterpret_cast<chaos_type_SnapshotTestFixtures_HasInstanceFields*>(_s1);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_object->field_SnapshotTestFixtures_HasInstanceFields__Value));
		chaos_object->field_SnapshotTestFixtures_HasInstanceFields__Value = chaos_value;
		chaos_gc_dirty_card(chaos_object);
	}
	{
		auto* chaos_object = reinterpret_cast<chaos_type_SnapshotTestFixtures_HasInstanceFields*>(_s0);
		_s0 = chaos_object->field_SnapshotTestFixtures_HasInstanceFields__Value;
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/InstanceHelper::.ctor(System.Int32)
extern "C" void SnapshotTestFixtures_InstanceHelper__ctor_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(chaos_arg_0);
	}
	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	{
		auto chaos_value = _s1;
		if (chaos_is_string_id(chaos_value))
		{
			chaos_value = chaos_string_materialize(chaos_value);
		}
		auto* chaos_object = reinterpret_cast<chaos_type_SnapshotTestFixtures_InstanceHelper*>(_s0);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_object->field_SnapshotTestFixtures_InstanceHelper___val));
		chaos_object->field_SnapshotTestFixtures_InstanceHelper___val = chaos_value;
		chaos_gc_dirty_card(chaos_object);
	}
	return;
}

// Managed method: SnapshotTestFixtures/InstanceHelper::GetValue()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_InstanceHelper_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_SnapshotTestFixtures_InstanceHelper*>(_s0);
		_s0 = chaos_object->field_SnapshotTestFixtures_InstanceHelper___val;
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/InterfaceDevirtHelper::RunInterfaceTest()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_InterfaceDevirtHelper_RunInterfaceTest(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		const auto chaos_arg_1 = _s0;
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_SnapshotTestFixtures_ImplHelper, {});
		chaos_object->header.type_info = &chaos_mt_SnapshotTestFixtures_ImplHelper.hot;
		SnapshotTestFixtures_ImplHelper__ctor_System_Int32(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/InternalHelper::Double(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_InternalHelper_Double_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapMul(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/LdargaHelper::RunLdarga(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_LdargaHelper_RunLdarga_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/LdcI8Helper::RunLdcI8()
extern "C" CHAOS_IL2CPP_INT64 SnapshotTestFixtures_LdcI8Helper_RunLdcI8(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};


	_s0 = ChaosStoreInt64(1000000000000LL);
	return ChaosLoadInt64(_s0);
}

// Managed method: SnapshotTestFixtures/LdelemaHelper::RunLdelema()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_LdelemaHelper_RunLdelema(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
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
		_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)]);
	}
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		auto chaos_value_raw = _s2;
		const auto chaos_value = static_cast<CHAOS_IL2CPP_INT32>(chaos_value_raw);
		const auto chaos_address = _s1;
		chaos_store_indirect<CHAOS_IL2CPP_INT32>(chaos_address, chaos_value);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s1);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s0);
		if (chaos_array == nullptr)
		{
			CHAOS_IL2CPP_FAIL();
		}
		if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_element = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];
		_s0 = chaos_element;
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/LdelemAllHelper::TestAllElems()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_LdelemAllHelper_TestAllElems(void)
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
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
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
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(-6);
	{
		auto chaos_value_raw = _s3;
		const auto chaos_value = chaos_value_raw;
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s2);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s1);
		chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)] = chaos_value;
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s1);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s0);
		if (chaos_array == nullptr)
		{
			CHAOS_IL2CPP_FAIL();
		}
		if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_element = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];
		_s0 = chaos_element;
	}
	_s1 = chaos_locals[0];
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
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
		const auto chaos_element = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];
		_s1 = chaos_element;
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/LdftnHelper::GetFnPtr()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_LdftnHelper_GetFnPtr(void)
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


	_s0 = chaos_static_SnapshotTestFixtures_LdftnHelper___O___0___GetValue;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			_s0 = 0;
			{
				// Hotpatch-aware ldftn wrapper (slot 40)
				static auto* chaos_ftn_thunk = +[](void) -> CHAOS_IL2CPP_INT32 {
					auto& _d_entry = s_hotpatch_entries[40];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						CHAOS_IL2CPP_INT32 _d_ret{};
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, nullptr, &_d_ret);
						return _d_ret;
					}
					return reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(_d_entry.direct_ptr)();
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
				chaos_static_SnapshotTestFixtures_LdftnHelper___O___0___GetValue = chaos_value;
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
				if (chaos_delegate->chaos_delegate_target == 0)
				{
					const auto chaos_open_function = reinterpret_cast<CHAOS_IL2CPP_INT32(*)()>(chaos_delegate->chaos_delegate_method_ptr);
					const auto chaos_result = chaos_open_function();
					_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
				}
				else
				{
					const auto chaos_closed_function = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR chaos_delegate_target)>(chaos_delegate->chaos_delegate_method_ptr);
					const auto chaos_result = chaos_closed_function(chaos_delegate->chaos_delegate_target);
					_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
				}
			}
		}
		return static_cast<CHAOS_IL2CPP_INT32>(_s2);
	}
}

// Managed method: SnapshotTestFixtures/LdindI1Helper::RunLdindI1()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_LdindI1Helper_RunLdindI1(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_SByte.hot;
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
		chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)] = chaos_value;
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s1);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s0);
		if (chaos_array == nullptr)
		{
			CHAOS_IL2CPP_FAIL();
		}
		if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_element = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];
		_s0 = chaos_element;
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/LdindI8Helper::RunLdindI8()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_LdindI8Helper_RunLdindI8(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Int64.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = ChaosStoreInt64(4294967296LL);
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
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s1);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s0);
		if (chaos_array == nullptr)
		{
			CHAOS_IL2CPP_FAIL();
		}
		if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_element = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];
		_s0 = chaos_element;
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/LdindStindGapsHelper::RunLdindI()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_LdindStindGapsHelper_RunLdindI(void)
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
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s1);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s0);
		if (chaos_array == nullptr)
		{
			CHAOS_IL2CPP_FAIL();
		}
		if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_element = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];
		_s0 = chaos_element;
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/LdindStindGapsHelper::RunLdindI4()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_LdindStindGapsHelper_RunLdindI4(void)
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
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s1);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s0);
		if (chaos_array == nullptr)
		{
			CHAOS_IL2CPP_FAIL();
		}
		if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_element = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];
		_s0 = chaos_element;
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/LdindStindGapsHelper::RunStindI()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_LdindStindGapsHelper_RunStindI(void)
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
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(100);
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
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s1);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s0);
		if (chaos_array == nullptr)
		{
			CHAOS_IL2CPP_FAIL();
		}
		if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_element = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];
		_s0 = chaos_element;
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/LdindUnsignedHelper::RunLdindUnsigned()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_LdindUnsignedHelper_RunLdindUnsigned(void)
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
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(200);
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_UInt16.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(40000);
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
	chaos_locals[1] = _s0;
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_UInt32.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(100000);
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
	chaos_locals[2] = _s0;
	_s0 = chaos_locals[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s1);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s0);
		if (chaos_array == nullptr)
		{
			CHAOS_IL2CPP_FAIL();
		}
		if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_element = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];
		_s0 = chaos_element;
	}
	_s1 = chaos_locals[1];
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
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
		const auto chaos_element = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];
		_s1 = chaos_element;
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	_s0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	_s1 = chaos_locals[2];
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s2);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s1);
		const auto chaos_element = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];
		_s1 = chaos_element;
	}
	_s1 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s1));
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/LdindWideHelper::RunLdindWide()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_LdindWideHelper_RunLdindWide(void)
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
		chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)] = chaos_value;
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s1);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s0);
		if (chaos_array == nullptr)
		{
			CHAOS_IL2CPP_FAIL();
		}
		if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_element = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];
		_s0 = chaos_element;
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/LdtokenHelper::RunLdtoken()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_LdtokenHelper_RunLdtoken(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(40745025u);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_TypeHandle_System_RuntimeTypeHandle__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[0] = _s0;
	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[32])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[1] = _s1;
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[33])();
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s2);
}

// Managed method: SnapshotTestFixtures/LdVirtftnHelper::RunLdVirtftn()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_LdVirtftnHelper_RunLdVirtftn(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		const auto chaos_arg_1 = _s0;
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_SnapshotTestFixtures_MyClass, {});
		chaos_object->header.type_info = &chaos_mt_SnapshotTestFixtures_MyClass.hot;
		SnapshotTestFixtures_MyClass__ctor_System_Int32(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/LocalAllocHelper::RunAlloc()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_LocalAllocHelper_RunAlloc(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(256);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s0));
	{
		const auto chaos_size = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		if (chaos_size < 0)
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_byte_count = static_cast<CHAOS_IL2CPP_SIZE>(chaos_size);
		void* chaos_block = CHAOS_IL2CPP_MALLOC(chaos_byte_count == static_cast<CHAOS_IL2CPP_SIZE>(0) ? static_cast<CHAOS_IL2CPP_SIZE>(1) : chaos_byte_count);
		if (chaos_block == nullptr)
		{
			CHAOS_IL2CPP_FAIL();
		}
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_block);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		auto chaos_value_raw = _s2;
		const auto chaos_value = static_cast<CHAOS_IL2CPP_INT8>(chaos_value_raw);
		const auto chaos_address = _s1;
		chaos_store_indirect<CHAOS_IL2CPP_INT8>(chaos_address, chaos_value);
	}
	{
		const auto chaos_address = _s0;
		const auto chaos_value = chaos_load_indirect<CHAOS_IL2CPP_UINT8>(chaos_address);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_value);
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/LoopHelper::SumToFive()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_LoopHelper_SumToFive(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_eval_stack{};
	CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


chaos_label_0:
	chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
chaos_label_1:
	chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
chaos_label_2:
	chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
chaos_label_3:
	chaos_locals[1] = chaos_eval_stack[--chaos_stack_top];
chaos_label_4:
	goto chaos_label_14;
chaos_label_6:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
chaos_label_7:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[1];
chaos_label_8:
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(chaos_left, chaos_right));
	}
chaos_label_9:
	chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
chaos_label_10:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[1];
chaos_label_11:
	chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
chaos_label_12:
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(chaos_left, chaos_right));
	}
chaos_label_13:
	chaos_locals[1] = chaos_eval_stack[--chaos_stack_top];
chaos_label_14:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[1];
chaos_label_15:
	chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(5);
chaos_label_16:
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
		if (chaos_left < chaos_right)
			goto chaos_label_6;
	}
chaos_label_18:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
chaos_label_19:
	return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
}

// Managed method: SnapshotTestFixtures/MarkedClass::.ctor()
extern "C" void SnapshotTestFixtures_MarkedClass__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: SnapshotTestFixtures/MathHelper::RunSquare()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_MathHelper_RunSquare(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
	{
		const auto chaos_arg_0 = _s0;
		auto& _d42 = s_hotpatch_entries[42];
		CHAOS_IL2CPP_INT32 _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d42)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d42))
		{
			alignas(16) uint8_t _d_ab[4];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d42.method_key, _d_ab, &_d_hpresult);
		}
		else
		{
			_d_hpresult = SnapshotTestFixtures_Helper_Square_System_Int32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
		}
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/MoreRareOpsHelper::RunArglist()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_MoreRareOpsHelper_RunArglist(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/MoreRareOpsHelper::RunJmp()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_MoreRareOpsHelper_RunJmp(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/MoreRareOpsHelper::RunMkrefany()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_MoreRareOpsHelper_RunMkrefany(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/MoreRareOpsHelper::RunRefanytype()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_MoreRareOpsHelper_RunRefanytype(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/MoreRareOpsHelper::RunRefanyval()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_MoreRareOpsHelper_RunRefanyval(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/MulDivHelper::RunMulDiv()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_MulDivHelper_RunMulDiv(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(14);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/MultiDispatcher::DispatchBoth()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_MultiDispatcher_DispatchBoth(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/MyClass::.ctor(System.Int32)
extern "C" void SnapshotTestFixtures_MyClass__ctor_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(chaos_arg_0);
	}
	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	{
		auto chaos_value = _s1;
		if (chaos_is_string_id(chaos_value))
		{
			chaos_value = chaos_string_materialize(chaos_value);
		}
		auto* chaos_object = reinterpret_cast<chaos_type_SnapshotTestFixtures_MyClass*>(_s0);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_object->field_SnapshotTestFixtures_MyClass___val));
		chaos_object->field_SnapshotTestFixtures_MyClass___val = chaos_value;
		chaos_gc_dirty_card(chaos_object);
	}
	return;
}

// Managed method: SnapshotTestFixtures/MyClass::GetValue()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_MyClass_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_SnapshotTestFixtures_MyClass*>(_s0);
		_s0 = chaos_object->field_SnapshotTestFixtures_MyClass___val;
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/NegShiftHelper::RunNegShift()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_NegShiftHelper_RunNegShift(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(CHAOS_IL2CPP_NUMERIC_LIMITS_MIN(CHAOS_IL2CPP_INT32));
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_shift_right_int32(static_cast<CHAOS_IL2CPP_INT32>(_s1), static_cast<CHAOS_IL2CPP_INT32>(_s2)));
	chaos_locals[0] = _s1;
	_s1 = chaos_locals[0];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/OverflowConvExtHelper::ConvOvfI1(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfI1_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		const auto chaos_val = static_cast<CHAOS_IL2CPP_INT64>(_s0);
		if (chaos_val < static_cast<CHAOS_IL2CPP_INT64>(CHAOS_IL2CPP_INT8_MIN) || chaos_val > static_cast<CHAOS_IL2CPP_INT64>(CHAOS_IL2CPP_INT8_MAX)) { CHAOS_IL2CPP_FAIL(); }
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT8>(chaos_val));
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/OverflowConvExtHelper::ConvOvfI1Un(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfI1Un_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		const auto chaos_val = static_cast<CHAOS_IL2CPP_UINT64>(_s0);
		if (chaos_val > static_cast<CHAOS_IL2CPP_UINT64>(CHAOS_IL2CPP_UINT32_MAX)) { CHAOS_IL2CPP_FAIL(); }
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINT32>(chaos_val));
	}
	{
		const auto chaos_val = static_cast<CHAOS_IL2CPP_UINTPTR>(_s0);
		if (chaos_val > static_cast<CHAOS_IL2CPP_UINTPTR>(CHAOS_IL2CPP_INT8_MAX)) { CHAOS_IL2CPP_FAIL(); }
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT8>(chaos_val));
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/OverflowConvExtHelper::ConvOvfI2Un(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfI2Un_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		const auto chaos_val = static_cast<CHAOS_IL2CPP_UINT64>(_s0);
		if (chaos_val > static_cast<CHAOS_IL2CPP_UINT64>(CHAOS_IL2CPP_UINT32_MAX)) { CHAOS_IL2CPP_FAIL(); }
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINT32>(chaos_val));
	}
	{
		const auto chaos_val = static_cast<CHAOS_IL2CPP_UINTPTR>(_s0);
		if (chaos_val > static_cast<CHAOS_IL2CPP_UINTPTR>(CHAOS_IL2CPP_INT16_MAX)) { CHAOS_IL2CPP_FAIL(); }
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT16>(chaos_val));
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/OverflowConvExtHelper::ConvOvfI4Un(System.Int64)
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfI4Un_System_Int64(CHAOS_IL2CPP_INT64 chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	chaos_args[0] = ChaosStoreInt64(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		const auto chaos_val = static_cast<CHAOS_IL2CPP_UINT64>(_s0);
	#if CHAOS_IL2CPP_SIZEOF_VOID_P == 4
		if (chaos_val > static_cast<CHAOS_IL2CPP_UINT64>(CHAOS_IL2CPP_UINT64_MAX)) { CHAOS_IL2CPP_FAIL(); }
	#endif
		_s0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(chaos_val));
	}
	{
		const auto chaos_val = static_cast<CHAOS_IL2CPP_UINTPTR>(_s0);
		if (chaos_val > static_cast<CHAOS_IL2CPP_UINTPTR>(CHAOS_IL2CPP_INT32_MAX)) { CHAOS_IL2CPP_FAIL(); }
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(chaos_val));
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/OverflowConvExtHelper::ConvOvfI8Un(System.Single)
extern "C" CHAOS_IL2CPP_INT64 SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfI8Un_System_Single(float chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	chaos_args[0] = ChaosStoreFloat32(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	_s0 = ChaosStoreFloat64(static_cast<double>(_s0));
	{
		const auto chaos_val = static_cast<CHAOS_IL2CPP_INT64>(_s0);
	#if CHAOS_IL2CPP_SIZEOF_VOID_P == 4
		if (static_cast<CHAOS_IL2CPP_INT64>(chaos_val) < static_cast<CHAOS_IL2CPP_INT64>(CHAOS_IL2CPP_INT64_MIN) || static_cast<CHAOS_IL2CPP_INT64>(chaos_val) > static_cast<CHAOS_IL2CPP_INT64>(CHAOS_IL2CPP_INT64_MAX)) { CHAOS_IL2CPP_FAIL(); }
	#endif
		_s0 = ChaosStoreInt64(chaos_val);
	}
	return ChaosLoadInt64(_s0);
}

// Managed method: SnapshotTestFixtures/OverflowConvExtHelper::ConvOvfIUn(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfIUn_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
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
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		const auto chaos_val = static_cast<CHAOS_IL2CPP_UINT64>(_s0);
		if (chaos_val > static_cast<CHAOS_IL2CPP_UINT64>(CHAOS_IL2CPP_UINT32_MAX)) { CHAOS_IL2CPP_FAIL(); }
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINT32>(chaos_val));
	}
	{
		const auto chaos_val = static_cast<CHAOS_IL2CPP_UINTPTR>(_s0);
		if (chaos_val > static_cast<CHAOS_IL2CPP_UINTPTR>(CHAOS_IL2CPP_INTPTR_MAX)) { CHAOS_IL2CPP_FAIL(); }
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_val);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/OverflowConvExtHelper::ConvOvfU1Un(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfU1Un_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		const auto chaos_val = static_cast<CHAOS_IL2CPP_UINT64>(_s0);
		if (chaos_val > static_cast<CHAOS_IL2CPP_UINT64>(CHAOS_IL2CPP_UINT32_MAX)) { CHAOS_IL2CPP_FAIL(); }
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINT32>(chaos_val));
	}
	{
		const auto chaos_val = static_cast<CHAOS_IL2CPP_UINTPTR>(_s0);
		if (chaos_val > static_cast<CHAOS_IL2CPP_UINTPTR>(CHAOS_IL2CPP_UINT8_MAX)) { CHAOS_IL2CPP_FAIL(); }
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINT8>(chaos_val));
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/OverflowConvExtHelper::ConvOvfU2Un(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfU2Un_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		const auto chaos_val = static_cast<CHAOS_IL2CPP_UINT64>(_s0);
		if (chaos_val > static_cast<CHAOS_IL2CPP_UINT64>(CHAOS_IL2CPP_UINT32_MAX)) { CHAOS_IL2CPP_FAIL(); }
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINT32>(chaos_val));
	}
	{
		const auto chaos_val = static_cast<CHAOS_IL2CPP_UINTPTR>(_s0);
		if (chaos_val > static_cast<CHAOS_IL2CPP_UINTPTR>(CHAOS_IL2CPP_UINT16_MAX)) { CHAOS_IL2CPP_FAIL(); }
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINT16>(chaos_val));
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/OverflowConvExtHelper::ConvOvfU4Un(System.Int64)
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfU4Un_System_Int64(CHAOS_IL2CPP_INT64 chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	chaos_args[0] = ChaosStoreInt64(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		const auto chaos_val = static_cast<CHAOS_IL2CPP_UINT64>(_s0);
	#if CHAOS_IL2CPP_SIZEOF_VOID_P == 4
		if (chaos_val > static_cast<CHAOS_IL2CPP_UINT64>(CHAOS_IL2CPP_UINT64_MAX)) { CHAOS_IL2CPP_FAIL(); }
	#endif
		_s0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(chaos_val));
	}
	{
		const auto chaos_val = static_cast<CHAOS_IL2CPP_UINTPTR>(_s0);
		if (chaos_val > static_cast<CHAOS_IL2CPP_UINTPTR>(CHAOS_IL2CPP_UINT32_MAX)) { CHAOS_IL2CPP_FAIL(); }
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINT32>(chaos_val));
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/OverflowConvExtHelper::ConvOvfU8Un(System.Double)
extern "C" CHAOS_IL2CPP_INT64 SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfU8Un_System_Double(double chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	chaos_args[0] = ChaosStoreFloat64(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		const auto chaos_val = static_cast<CHAOS_IL2CPP_UINT64>(_s0);
	#if CHAOS_IL2CPP_SIZEOF_VOID_P == 4
		if (chaos_val > static_cast<CHAOS_IL2CPP_UINT64>(CHAOS_IL2CPP_UINT64_MAX)) { CHAOS_IL2CPP_FAIL(); }
	#endif
		_s0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(chaos_val));
	}
	{
		const auto chaos_val = static_cast<CHAOS_IL2CPP_UINTPTR>(_s0);
		if (chaos_val > static_cast<CHAOS_IL2CPP_UINTPTR>(CHAOS_IL2CPP_INT64_MAX)) { CHAOS_IL2CPP_FAIL(); }
		_s0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(chaos_val));
	}
	return ChaosLoadInt64(_s0);
}

// Managed method: SnapshotTestFixtures/OverflowConvExtHelper::ConvOvfUUn(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfUUn_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
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
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		const auto chaos_val = static_cast<CHAOS_IL2CPP_UINT64>(_s0);
		if (chaos_val > static_cast<CHAOS_IL2CPP_UINT64>(CHAOS_IL2CPP_UINT32_MAX)) { CHAOS_IL2CPP_FAIL(); }
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINT32>(chaos_val));
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s0));
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/OverflowConvHelper::ConvOvfI2(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_OverflowConvHelper_ConvOvfI2_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		const auto chaos_val = static_cast<CHAOS_IL2CPP_INT64>(_s0);
		if (chaos_val < static_cast<CHAOS_IL2CPP_INT64>(CHAOS_IL2CPP_INT16_MIN) || chaos_val > static_cast<CHAOS_IL2CPP_INT64>(CHAOS_IL2CPP_INT16_MAX)) { CHAOS_IL2CPP_FAIL(); }
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT16>(chaos_val));
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/OverflowConvHelper::ConvOvfI4()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_OverflowConvHelper_ConvOvfI4(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(100);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/OverflowConvHelper::ConvOvfI8(System.Single)
extern "C" CHAOS_IL2CPP_INT64 SnapshotTestFixtures_OverflowConvHelper_ConvOvfI8_System_Single(float chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = ChaosStoreFloat32(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		const auto chaos_val = static_cast<CHAOS_IL2CPP_INT64>(_s0);
	#if CHAOS_IL2CPP_SIZEOF_VOID_P == 4
		if (static_cast<CHAOS_IL2CPP_INT64>(chaos_val) < static_cast<CHAOS_IL2CPP_INT64>(CHAOS_IL2CPP_INT64_MIN) || static_cast<CHAOS_IL2CPP_INT64>(chaos_val) > static_cast<CHAOS_IL2CPP_INT64>(CHAOS_IL2CPP_INT64_MAX)) { CHAOS_IL2CPP_FAIL(); }
	#endif
		_s0 = ChaosStoreInt64(chaos_val);
	}
	return ChaosLoadInt64(_s0);
}

// Managed method: SnapshotTestFixtures/OverflowConvHelper::ConvOvfU1(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_OverflowConvHelper_ConvOvfU1_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		const auto chaos_val = static_cast<CHAOS_IL2CPP_UINT64>(_s0);
		if (chaos_val > static_cast<CHAOS_IL2CPP_UINT64>(CHAOS_IL2CPP_UINT8_MAX)) { CHAOS_IL2CPP_FAIL(); }
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINT8>(chaos_val));
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/OverflowConvHelper::ConvOvfU2(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_OverflowConvHelper_ConvOvfU2_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		const auto chaos_val = static_cast<CHAOS_IL2CPP_UINT64>(_s0);
		if (chaos_val > static_cast<CHAOS_IL2CPP_UINT64>(CHAOS_IL2CPP_UINT16_MAX)) { CHAOS_IL2CPP_FAIL(); }
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINT16>(chaos_val));
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/OverflowConvHelper::ConvOvfU4(System.Int64)
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_OverflowConvHelper_ConvOvfU4_System_Int64(CHAOS_IL2CPP_INT64 chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = ChaosStoreInt64(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		const auto chaos_val = static_cast<CHAOS_IL2CPP_UINT64>(_s0);
		if (chaos_val > static_cast<CHAOS_IL2CPP_UINT64>(CHAOS_IL2CPP_UINT32_MAX)) { CHAOS_IL2CPP_FAIL(); }
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINT32>(chaos_val));
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/OverflowConvHelper::ConvOvfU8(System.Double)
extern "C" CHAOS_IL2CPP_INT64 SnapshotTestFixtures_OverflowConvHelper_ConvOvfU8_System_Double(double chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	chaos_args[0] = ChaosStoreFloat64(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		const auto chaos_val = static_cast<CHAOS_IL2CPP_UINT64>(_s0);
	#if CHAOS_IL2CPP_SIZEOF_VOID_P == 4
		if (chaos_val > static_cast<CHAOS_IL2CPP_UINT64>(CHAOS_IL2CPP_UINT64_MAX)) { CHAOS_IL2CPP_FAIL(); }
	#endif
		_s0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(chaos_val));
	}
	{
		const auto chaos_val = static_cast<CHAOS_IL2CPP_UINTPTR>(_s0);
		if (chaos_val > static_cast<CHAOS_IL2CPP_UINTPTR>(CHAOS_IL2CPP_INT64_MAX)) { CHAOS_IL2CPP_FAIL(); }
		_s0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(chaos_val));
	}
	return ChaosLoadInt64(_s0);
}

// Managed method: SnapshotTestFixtures/OverflowHelper::RunOverflowAdd()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_OverflowHelper_RunOverflowAdd(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(2147483647);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/OverflowHelper::RunOverflowMul()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_OverflowHelper_RunOverflowMul(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/OverflowHelper::RunOverflowSub()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_OverflowHelper_RunOverflowSub(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(50);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/OverflowUnHelper::RunOverflowAddUn()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_OverflowUnHelper_RunOverflowAddUn(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(150);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/OverflowUnHelper::RunOverflowMulUn()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_OverflowUnHelper_RunOverflowMulUn(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/OverflowUnHelper::RunOverflowSubUn()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_OverflowUnHelper_RunOverflowSubUn(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(70);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/RareOpsHelper::TestConvRUn(System.Single)
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_RareOpsHelper_TestConvRUn_System_Single(float chaos_fn_arg_0)
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
	chaos_args[0] = ChaosStoreFloat32(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINT32>(_s0));
	_s0 = ChaosStoreFloat32(static_cast<float>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s0)));
	_s0 = ChaosStoreFloat64(static_cast<double>(_s0));
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(chaos_load_float32(_s0)));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/RareOpsHelper::TestInitBlk(System.Int32&)
extern "C" void SnapshotTestFixtures_RareOpsHelper_TestInitBlk_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	return;
}

// Managed method: SnapshotTestFixtures/RareOpsHelper::TestStarg(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_RareOpsHelper_TestStarg_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/RethrowHelper::RunRethrow()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_RethrowHelper_RunRethrow(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


#if !defined(CHAOS_IL2CPP_EH_SETJMP) && !defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	try
	{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			chaos_locals[0] = _s0;
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
#if defined(CHAOS_IL2CPP_EH_SETJMP) || defined(CHAOS_IL2CPP_EH_WIN32_SEH)
			chaos::il2cpp::runtime_core::chaos_raise_exception(
				reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
					chaos::il2cpp::runtime_core::g_chaos_exception_obj));
#else
			throw;
#endif
	}
#elif defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	__try
	{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			chaos_locals[0] = _s0;
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
#if defined(CHAOS_IL2CPP_EH_SETJMP) || defined(CHAOS_IL2CPP_EH_WIN32_SEH)
			chaos::il2cpp::runtime_core::chaos_raise_exception(
				reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
					chaos::il2cpp::runtime_core::g_chaos_exception_obj));
#else
			throw;
#endif
	}
#else
	{
		auto* _chaos_jmp =
			chaos::il2cpp::runtime_core::push_exception_jmp_buf();
		if (setjmp(*_chaos_jmp) == 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			chaos_locals[0] = _s0;
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
#if defined(CHAOS_IL2CPP_EH_SETJMP) || defined(CHAOS_IL2CPP_EH_WIN32_SEH)
			chaos::il2cpp::runtime_core::chaos_raise_exception(
				reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
					chaos::il2cpp::runtime_core::g_chaos_exception_obj));
#else
			throw;
#endif
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/SealedClassVirtualHelper::RunSealedVirtual()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_SealedClassVirtualHelper_RunSealedVirtual(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		const auto chaos_arg_1 = _s0;
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_SnapshotTestFixtures_SealedHelper, {});
		chaos_object->header.type_info = &chaos_mt_SnapshotTestFixtures_SealedHelper.hot;
		SnapshotTestFixtures_SealedHelper__ctor_System_Int32(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/SealedHelper::.ctor(System.Int32)
extern "C" void SnapshotTestFixtures_SealedHelper__ctor_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(chaos_arg_0);
	}
	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	{
		auto chaos_value = _s1;
		if (chaos_is_string_id(chaos_value))
		{
			chaos_value = chaos_string_materialize(chaos_value);
		}
		auto* chaos_object = reinterpret_cast<chaos_type_SnapshotTestFixtures_SealedHelper*>(_s0);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_object->field_SnapshotTestFixtures_SealedHelper___val));
		chaos_object->field_SnapshotTestFixtures_SealedHelper___val = chaos_value;
		chaos_gc_dirty_card(chaos_object);
	}
	return;
}

// Managed method: SnapshotTestFixtures/SealedHelper::GetValueVirtual()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_SealedHelper_GetValueVirtual(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_SnapshotTestFixtures_SealedHelper*>(_s0);
		_s0 = chaos_object->field_SnapshotTestFixtures_SealedHelper___val;
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/ShiftHelper::RunShift()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_ShiftHelper_RunShift(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(8);
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_shift_left_int32(static_cast<CHAOS_IL2CPP_INT32>(_s1), static_cast<CHAOS_IL2CPP_INT32>(_s2)));
	chaos_locals[0] = _s1;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_shift_right_int32(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[0];
	_s1 = chaos_locals[1];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/SimpleGapsHelper::RunConvI()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_SimpleGapsHelper_RunConvI(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/SimpleGapsHelper::RunConvOvfI(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_SimpleGapsHelper_RunConvOvfI_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/SimpleGapsHelper::RunConvOvfIUn(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_SimpleGapsHelper_RunConvOvfIUn_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
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
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		const auto chaos_val = static_cast<CHAOS_IL2CPP_UINT64>(_s0);
		if (chaos_val > static_cast<CHAOS_IL2CPP_UINT64>(CHAOS_IL2CPP_UINT32_MAX)) { CHAOS_IL2CPP_FAIL(); }
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINT32>(chaos_val));
	}
	{
		const auto chaos_val = static_cast<CHAOS_IL2CPP_UINTPTR>(_s0);
		if (chaos_val > static_cast<CHAOS_IL2CPP_UINTPTR>(CHAOS_IL2CPP_INTPTR_MAX)) { CHAOS_IL2CPP_FAIL(); }
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_val);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/SimpleGapsHelper::RunConvOvfU(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_SimpleGapsHelper_RunConvOvfU_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		const auto chaos_val = static_cast<CHAOS_IL2CPP_UINT64>(_s0);
		if (chaos_val > static_cast<CHAOS_IL2CPP_UINT64>(CHAOS_IL2CPP_UINTPTR_MAX)) { CHAOS_IL2CPP_FAIL(); }
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_val));
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/SimpleGapsHelper::RunConvOvfUUn(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_SimpleGapsHelper_RunConvOvfUUn_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
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
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		const auto chaos_val = static_cast<CHAOS_IL2CPP_UINT64>(_s0);
		if (chaos_val > static_cast<CHAOS_IL2CPP_UINT64>(CHAOS_IL2CPP_UINT32_MAX)) { CHAOS_IL2CPP_FAIL(); }
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINT32>(chaos_val));
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s0));
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/SimpleGapsHelper::RunConvU4()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_SimpleGapsHelper_RunConvU4(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/SimpleGapsHelper::RunLdnull()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_SimpleGapsHelper_RunLdnull(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	_s0 = 0;
	_s1 = 0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s0) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) ? 1 : 0);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/SizeOfHelper::GetSize()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_SizeOfHelper_GetSize(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/StaticFieldWriteHelper::WriteAndRead()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_StaticFieldWriteHelper_WriteAndRead(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		auto chaos_value = _s0;
		chaos_static_SnapshotTestFixtures_StaticFieldWriteHelper___writeTarget = chaos_value;
	}
	_s0 = chaos_static_SnapshotTestFixtures_StaticFieldWriteHelper___readTarget;
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/StelemAllHelper::TestAllElems()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_StelemAllHelper_TestAllElems(void)
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
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
	{
		auto chaos_value_raw = _s2;
		const auto chaos_value = chaos_value_raw;
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s1);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s0);
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
	_s0 = chaos_locals[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(-6);
	{
		auto chaos_value_raw = _s2;
		const auto chaos_value = chaos_value_raw;
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s1);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s0);
		chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)] = chaos_value;
	}
	_s0 = chaos_locals[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s1);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s0);
		const auto chaos_element = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];
		_s0 = chaos_element;
	}
	_s1 = chaos_locals[0];
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
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
		const auto chaos_element = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];
		_s1 = chaos_element;
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/StindNarrowHelper::RunStindNarrow()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_StindNarrowHelper_RunStindNarrow(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Int16.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	chaos_locals[0] = _s1;
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
	_s1 = chaos_locals[0];
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(100);
	{
		auto chaos_value_raw = _s3;
		const auto chaos_value = chaos_value_raw;
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s2);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s1);
		chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)] = chaos_value;
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s1);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s0);
		if (chaos_array == nullptr)
		{
			CHAOS_IL2CPP_FAIL();
		}
		if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_element = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];
		_s0 = chaos_element;
	}
	_s1 = chaos_locals[0];
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
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
		const auto chaos_element = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];
		_s1 = chaos_element;
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/StindWideHelper::RunStindWide()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_StindWideHelper_RunStindWide(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Single.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Double.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	chaos_locals[0] = _s1;
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Object.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	chaos_locals[1] = _s1;
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = ChaosStoreFloat32(3.5f);
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
	_s1 = chaos_locals[0];
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = ChaosStoreFloat64(7.2);
	{
		auto chaos_value_raw = _s3;
		const auto chaos_value = chaos_value_raw;
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s2);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s1);
		chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)] = chaos_value;
	}
	_s1 = chaos_locals[1];
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{{
		_s3 = CHAOS_IL2CPP_STRING_ID("hi");
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
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s1);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s0);
		if (chaos_array == nullptr)
		{
			CHAOS_IL2CPP_FAIL();
		}
		if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_element = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];
		_s0 = chaos_element;
	}
	_s0 = ChaosStoreFloat64(static_cast<double>(_s0));
	_s1 = chaos_locals[0];
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
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
		const auto chaos_element = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];
		_s1 = chaos_element;
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(ChaosLoadFloat64(_s0), ChaosLoadFloat64(_s1)));
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s1 = chaos_locals[1];
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s2);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s1);
		const auto chaos_element = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];
		_s1 = chaos_element;
	}
	_s2 = 0;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s2) ? 1 : 0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/StringConcatHelper::TestConcat()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_StringConcatHelper_TestConcat(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	{{
		_s0 = CHAOS_IL2CPP_STRING_ID("Hello");
	}}
	{
		const auto chaos_arg_0 = _s0;
		auto& _d39 = s_hotpatch_entries[39];
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d39)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d39))
		{
			alignas(16) uint8_t _d_ab[8];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d39.method_key, _d_ab, nullptr);
		}
		else
		{
			SnapshotTestFixtures_Helper_ConsumeString_System_String(chaos_arg_0);
		}
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/StringFormatHelper::TestFormatOne()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_StringFormatHelper_TestFormatOne(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};


	{{
		_s0 = CHAOS_IL2CPP_STRING_ID("Value: {0}");
	}}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	chaos_boxed_type_System_Private_CoreLib_System_Int32 chaos_box_storage_0{};
	{
		const auto chaos_value = _s1;
		chaos_box_storage_0.header.type_info = &chaos_mt_System_Private_CoreLib_System_Int32.hot;
		chaos_box_storage_0.value = chaos_value;
	}
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_box_storage_0);
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__Format_System_String_System_String_System_Object_(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/StringOps::UseString()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_StringOps_UseString(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	{{
		_s0 = CHAOS_IL2CPP_STRING_ID("Hello");
	}}
	{
		const auto chaos_arg_0 = _s0;
		auto& _d39 = s_hotpatch_entries[39];
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d39)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d39))
		{
			alignas(16) uint8_t _d_ab[8];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d39.method_key, _d_ab, nullptr);
		}
		else
		{
			SnapshotTestFixtures_Helper_ConsumeString_System_String(chaos_arg_0);
		}
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/SwitchHelper::Classify()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_SwitchHelper_Classify(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_eval_stack{};
	CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


chaos_label_0:
	chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(2);
chaos_label_1:
	chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
chaos_label_2:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
chaos_label_3:
	chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
chaos_label_4:
	{
		const auto chaos_right = chaos_eval_stack[--chaos_stack_top];
		const auto chaos_left = chaos_eval_stack[--chaos_stack_top];
		if (chaos_left == chaos_right)
			goto chaos_label_12;
	}
chaos_label_6:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
chaos_label_7:
	chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(2);
chaos_label_8:
	{
		const auto chaos_right = chaos_eval_stack[--chaos_stack_top];
		const auto chaos_left = chaos_eval_stack[--chaos_stack_top];
		if (chaos_left == chaos_right)
			goto chaos_label_17;
	}
chaos_label_10:
	goto chaos_label_22;
chaos_label_12:
	chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(10);
chaos_label_14:
	chaos_locals[1] = chaos_eval_stack[--chaos_stack_top];
chaos_label_15:
	goto chaos_label_24;
chaos_label_17:
	chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(20);
chaos_label_19:
	chaos_locals[1] = chaos_eval_stack[--chaos_stack_top];
chaos_label_20:
	goto chaos_label_24;
chaos_label_22:
	chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
chaos_label_23:
	chaos_locals[1] = chaos_eval_stack[--chaos_stack_top];
chaos_label_24:
	chaos_eval_stack[chaos_stack_top++] = chaos_locals[1];
chaos_label_25:
	return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
}

// Managed method: SnapshotTestFixtures/TargetHelper::GetValue()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_TargetHelper_GetValue(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/ThrowHelper::CheckPositive(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_ThrowHelper_CheckPositive_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INT32>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>(_s0);
		if (chaos_left < chaos_right)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1);
			return static_cast<CHAOS_IL2CPP_INT32>(_s0);
		}
		else
		{
			_s0 = chaos_args[0];
			return static_cast<CHAOS_IL2CPP_INT32>(_s0);
		}
	}
}

// Managed method: SnapshotTestFixtures/TypeCheckHelper::CheckAndCast()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_TypeCheckHelper_CheckAndCast(void)
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


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_SnapshotTestFixtures_MarkedClass, {});
		chaos_object->header.type_info = &chaos_mt_SnapshotTestFixtures_MarkedClass.hot;
		SnapshotTestFixtures_MarkedClass__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	{
		const auto chaos_value = _s0;
		auto chaos_matches = false;
		if (chaos_value != 0)
		{
			auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_value);
			chaos_matches = chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_SnapshotTestFixtures_MarkedClass.hot);
		}
		_s0 = chaos_matches ? chaos_value : 0;
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		if (_s0 == 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1);
			return static_cast<CHAOS_IL2CPP_INT32>(_s0);
		}
		else
		{
			_s0 = chaos_locals[0];
			{
				auto* chaos_object = reinterpret_cast<chaos_type_SnapshotTestFixtures_MarkedClass*>(_s0);
				_s0 = chaos_object->field_SnapshotTestFixtures_MarkedClass__Value;
			}
			return static_cast<CHAOS_IL2CPP_INT32>(_s0);
		}
	}
}

// Managed method: SnapshotTestFixtures/UnboxHelper::RunUnbox()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_UnboxHelper_RunUnbox(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/UnsignedOpsHelper::TestCkfinite(System.Double)
extern "C" double SnapshotTestFixtures_UnsignedOpsHelper_TestCkfinite_System_Double(double chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	chaos_args[0] = ChaosStoreFloat64(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[46])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (_s0 != 0)
		{
			_s0 = chaos_args[0];
			return ChaosLoadFloat64(_s0);
		}
		else
		{
			_s0 = ChaosStoreFloat64(0);
			return ChaosLoadFloat64(_s0);
		}
	}
}

// Managed method: SnapshotTestFixtures/UnsignedOpsHelper::TestCltUn()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_UnsignedOpsHelper_TestCltUn(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
	chaos_locals[0] = _s1;
	_s1 = chaos_locals[0];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/UnsignedOpsHelper::TestDivUn()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_UnsignedOpsHelper_TestDivUn(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(10);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
	chaos_locals[0] = _s1;
	_s1 = chaos_locals[0];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosDiv(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/UnsignedOpsHelper::TestRemUn()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_UnsignedOpsHelper_TestRemUn(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(10);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
	chaos_locals[0] = _s1;
	_s1 = chaos_locals[0];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosRem(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/ValueTypeHelper::RunValueType()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_ValueTypeHelper_RunValueType(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(30);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/VirtualDispatchHelper::UseVirtualDispatch()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_VirtualDispatchHelper_UseVirtualDispatch(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_SnapshotTestFixtures_DerivedClass, {});
		chaos_object->header.type_info = &chaos_mt_SnapshotTestFixtures_DerivedClass.hot;
		SnapshotTestFixtures_DerivedClass__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/VirtualHelper::UseVirtual()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_VirtualHelper_UseVirtual(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_SnapshotTestFixtures_DerivedClass, {});
		chaos_object->header.type_info = &chaos_mt_SnapshotTestFixtures_DerivedClass.hot;
		SnapshotTestFixtures_DerivedClass__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/VoidCaller::DoNothing()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_VoidCaller_DoNothing(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};


	{
		auto& _d41 = s_hotpatch_entries[41];
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d41)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d41))
		{
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d41.method_key, nullptr, nullptr);
		}
		else
		{
			SnapshotTestFixtures_Helper_Nop();
		}
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/Wrapper`1::.ctor(!0)
extern "C" void SnapshotTestFixtures_Wrapper_1__ctor_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(chaos_arg_0);
	}
	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	{
		auto chaos_value = _s1;
		if (chaos_is_string_id(chaos_value))
		{
			chaos_value = chaos_string_materialize(chaos_value);
		}
		auto* chaos_object = reinterpret_cast<chaos_type_SnapshotTestFixtures_Wrapper__0_*>(_s0);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_object->field_SnapshotTestFixtures_Wrapper__0____val));
		chaos_object->field_SnapshotTestFixtures_Wrapper__0____val = chaos_value;
		chaos_gc_dirty_card(chaos_object);
	}
	return;
}

// Managed method: SnapshotTestFixtures/Wrapper`1::GetValue()
extern "C" CHAOS_IL2CPP_INTPTR SnapshotTestFixtures_Wrapper_1_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_SnapshotTestFixtures_Wrapper__0_*>(_s0);
		_s0 = chaos_object->field_SnapshotTestFixtures_Wrapper__0____val;
	}
	return _s0;
}

// Managed method: SnapshotTestFixtures/Wrapper<!0>::.ctor(!0)
extern "C" void SnapshotTestFixtures_Wrapper__0__ctor_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(chaos_arg_0);
	}
	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	{
		auto chaos_value = _s1;
		if (chaos_is_string_id(chaos_value))
		{
			chaos_value = chaos_string_materialize(chaos_value);
		}
		auto* chaos_object = reinterpret_cast<chaos_type_SnapshotTestFixtures_Wrapper__0_*>(_s0);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_object->field_SnapshotTestFixtures_Wrapper__0____val));
		chaos_object->field_SnapshotTestFixtures_Wrapper__0____val = chaos_value;
		chaos_gc_dirty_card(chaos_object);
	}
	return;
}

// Managed method: SnapshotTestFixtures/Wrapper<!0>::GetValue()
extern "C" CHAOS_IL2CPP_INTPTR SnapshotTestFixtures_Wrapper__0_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_SnapshotTestFixtures_Wrapper__0_*>(_s0);
		_s0 = chaos_object->field_SnapshotTestFixtures_Wrapper__0____val;
	}
	return _s0;
}

// Generic execution authority: definition=SnapshotTestFixtures/Wrapper`1::.ctor:System.Void(!0); type=[System.Int32]; method=[]; support=Specialized; specialization=SpecializedBody; body=body:definition=SnapshotTestFixtures/Wrapper`1::.ctor:System.Void(!0);type=[System.Int32];method=[]; stub=stub:definition=SnapshotTestFixtures/Wrapper`1::.ctor:System.Void(!0);type=[System.Int32];method=[]
// Managed method: SnapshotTestFixtures/Wrapper<System.Int32>::.ctor(System.Int32)
extern "C" void SnapshotTestFixtures_Wrapper_System_Int32__ctor_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(chaos_arg_0);
	}
	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	{
		auto chaos_value = _s1;
		if (chaos_is_string_id(chaos_value))
		{
			chaos_value = chaos_string_materialize(chaos_value);
		}
		auto* chaos_object = reinterpret_cast<chaos_type_SnapshotTestFixtures_Wrapper_System_Int32_*>(_s0);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_object->field_SnapshotTestFixtures_Wrapper_System_Int32____val));
		chaos_object->field_SnapshotTestFixtures_Wrapper_System_Int32____val = chaos_value;
		chaos_gc_dirty_card(chaos_object);
	}
	return;
}

// Generic instantiation stub: SnapshotTestFixtures/Wrapper<System.Int32>::.ctor(System.Int32)
// Generic execution authority: definition=SnapshotTestFixtures/Wrapper`1::.ctor:System.Void(!0); type=[System.Int32]; method=[]; support=Specialized; specialization=SpecializedBody; body=body:definition=SnapshotTestFixtures/Wrapper`1::.ctor:System.Void(!0);type=[System.Int32];method=[]; stub=stub:definition=SnapshotTestFixtures/Wrapper`1::.ctor:System.Void(!0);type=[System.Int32];method=[]
extern "C" void chaos_stub_definition_SnapshotTestFixtures_Wrapper_1___ctor_System_Void__0__type__System_Int32__method(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	SnapshotTestFixtures_Wrapper_System_Int32__ctor_System_Int32(chaos_fn_arg_0, chaos_fn_arg_1);
}

// Generic execution authority: definition=SnapshotTestFixtures/Wrapper`1::GetValue:!0(); type=[System.Int32]; method=[]; support=Specialized; specialization=SpecializedBody; body=body:definition=SnapshotTestFixtures/Wrapper`1::GetValue:!0();type=[System.Int32];method=[]; stub=stub:definition=SnapshotTestFixtures/Wrapper`1::GetValue:!0();type=[System.Int32];method=[]
// Managed method: SnapshotTestFixtures/Wrapper<System.Int32>::GetValue()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_Wrapper_System_Int32_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_SnapshotTestFixtures_Wrapper_System_Int32_*>(_s0);
		_s0 = chaos_object->field_SnapshotTestFixtures_Wrapper_System_Int32____val;
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Generic instantiation stub: SnapshotTestFixtures/Wrapper<System.Int32>::GetValue()
// Generic execution authority: definition=SnapshotTestFixtures/Wrapper`1::GetValue:!0(); type=[System.Int32]; method=[]; support=Specialized; specialization=SpecializedBody; body=body:definition=SnapshotTestFixtures/Wrapper`1::GetValue:!0();type=[System.Int32];method=[]; stub=stub:definition=SnapshotTestFixtures/Wrapper`1::GetValue:!0();type=[System.Int32];method=[]
extern "C" CHAOS_IL2CPP_INT32 chaos_stub_definition_SnapshotTestFixtures_Wrapper_1__GetValue__0___type__System_Int32__method(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return SnapshotTestFixtures_Wrapper_System_Int32_GetValue(chaos_fn_arg_0);
}



}  // namespace chaos::il2cpp::codegen::SnapshotProverSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 157;

extern "C" void ChaosJitRegisterAll() {}