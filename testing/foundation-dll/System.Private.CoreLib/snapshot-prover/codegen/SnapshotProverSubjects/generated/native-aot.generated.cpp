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
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_CustomEntrySubject_8(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_0(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_1(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_2(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_3(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_4(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_5(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_6(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_7(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_9(void);

// Forward declarations (pipeline fix: used before extern "C" decl)
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_CustomEntrySubject_8(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_0(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_1(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_2(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_3(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_4(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_5(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_6(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_7(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_9(void);

// Forward declarations (pipeline fix: used before extern "C" decl)
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_CustomEntrySubject_8(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_0(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_1(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_2(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_3(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_4(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_5(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_6(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_7(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_9(void);

// Forward declarations (pipeline fix: used before extern "C" decl)
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_CustomEntrySubject_8(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_0(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_1(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_2(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_3(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_4(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_5(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_6(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_7(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_9(void);

// Forward declarations (pipeline fix: used before extern "C" decl)
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_CustomEntrySubject_8(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_0(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_1(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_2(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_3(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_4(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_5(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_6(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_7(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_9(void);

// Forward declarations (pipeline fix: used before extern "C" decl)
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_CustomEntrySubject_8(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_0(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_1(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_2(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_3(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_4(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_5(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_6(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_7(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_9(void);

// Forward declarations (pipeline fix: used before extern "C" decl)
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_CustomEntrySubject_8(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_0(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_1(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_2(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_3(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_4(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_5(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_6(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_7(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_9(void);

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
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_SnapshotProverSubjects_SnapshotProverSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(17700440604878766952ULL);
inline TypeInfoV0 chaos_mt_SnapshotTestFixtures_BaseClass = {{nullptr, chaos_vtable_SnapshotTestFixtures_BaseClass, 16934624807659080947ULL, 2u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_SnapshotTestFixtures_BaseClass = static_cast<CHAOS_IL2CPP_INTPTR>(16934624807659080947ULL);
inline TypeInfoV0 chaos_mt_SnapshotTestFixtures_DerivedClass = {{&chaos_mt_SnapshotTestFixtures_BaseClass.hot, chaos_vtable_SnapshotTestFixtures_DerivedClass, 5213792135327273735ULL, 2u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_SnapshotTestFixtures_DerivedClass = static_cast<CHAOS_IL2CPP_INTPTR>(5213792135327273735ULL);
inline TypeInfoV0 chaos_mt_SnapshotTestFixtures_FieldHelper = {{nullptr, nullptr, 14105055411266533488ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_SnapshotTestFixtures_FieldHelper = static_cast<CHAOS_IL2CPP_INTPTR>(14105055411266533488ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Object = {{nullptr, nullptr, 15228727185366376748ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_INTPTR>(15228727185366376748ULL);
inline TypeInfoV0 chaos_mt_SnapshotTestFixtures_HasFields = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_SnapshotTestFixtures_HasFields, 6075661983971539625ULL, 1u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_SnapshotTestFixtures_HasFields = static_cast<CHAOS_IL2CPP_INTPTR>(6075661983971539625ULL);
inline TypeInfoV0 chaos_mt_SnapshotTestFixtures_HasInstanceFields = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_SnapshotTestFixtures_HasInstanceFields, 17812473439196761566ULL, 3u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_SnapshotTestFixtures_HasInstanceFields = static_cast<CHAOS_IL2CPP_INTPTR>(17812473439196761566ULL);
static constexpr InterfaceMapEntry chaos_iface_map_SnapshotTestFixtures_ImplHelper[] = {
	{ chaos_type_id_SnapshotTestFixtures_IMyInterface, 3, 1 }
};
inline TypeInfoV0 chaos_mt_SnapshotTestFixtures_ImplHelper = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_SnapshotTestFixtures_ImplHelper, 2682382514493266270ULL, 4u, 32, 1, 1}, {chaos_iface_map_SnapshotTestFixtures_ImplHelper, nullptr, 1, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_SnapshotTestFixtures_ImplHelper = static_cast<CHAOS_IL2CPP_INTPTR>(2682382514493266270ULL);
inline TypeInfoV0 chaos_mt_SnapshotTestFixtures_InstanceHelper = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_SnapshotTestFixtures_InstanceHelper, 7539435265304875683ULL, 4u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_SnapshotTestFixtures_InstanceHelper = static_cast<CHAOS_IL2CPP_INTPTR>(7539435265304875683ULL);
inline TypeInfoV0 chaos_mt_SnapshotTestFixtures_LdftnHelper___O = {{nullptr, nullptr, 9616880548282383388ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_SnapshotTestFixtures_LdftnHelper___O = static_cast<CHAOS_IL2CPP_INTPTR>(9616880548282383388ULL);
static constexpr InterfaceMapEntry chaos_iface_map_SnapshotTestFixtures_MarkedClass[] = {
	{ chaos_type_id_SnapshotTestFixtures_IMarker, 0, 0 }
};
inline TypeInfoV0 chaos_mt_SnapshotTestFixtures_MarkedClass = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_SnapshotTestFixtures_MarkedClass, 6112983511997078372ULL, 1u, 32, 1, 1}, {chaos_iface_map_SnapshotTestFixtures_MarkedClass, nullptr, 1, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_SnapshotTestFixtures_MarkedClass = static_cast<CHAOS_IL2CPP_INTPTR>(6112983511997078372ULL);
inline TypeInfoV0 chaos_mt_SnapshotTestFixtures_MyClass = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_SnapshotTestFixtures_MyClass, 17017755788800598734ULL, 4u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_SnapshotTestFixtures_MyClass = static_cast<CHAOS_IL2CPP_INTPTR>(17017755788800598734ULL);
inline TypeInfoV0 chaos_mt_SnapshotTestFixtures_SealedHelper = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_SnapshotTestFixtures_SealedHelper, 8545221821474343780ULL, 5u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_SnapshotTestFixtures_SealedHelper = static_cast<CHAOS_IL2CPP_INTPTR>(8545221821474343780ULL);
inline TypeInfoV0 chaos_mt_SnapshotTestFixtures_StaticFieldWriteHelper = {{nullptr, nullptr, 7187250757378180401ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_SnapshotTestFixtures_StaticFieldWriteHelper = static_cast<CHAOS_IL2CPP_INTPTR>(7187250757378180401ULL);
inline TypeInfoV0 chaos_mt_SnapshotTestFixtures_Wrapper__0_ = {{nullptr, chaos_vtable_SnapshotTestFixtures_Wrapper__0_, 15764982902837746946ULL, 7u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_SnapshotTestFixtures_Wrapper__0_ = static_cast<CHAOS_IL2CPP_INTPTR>(15764982902837746946ULL);
inline TypeInfoV0 chaos_mt_SnapshotTestFixtures_Wrapper_System_Int32_ = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_SnapshotTestFixtures_Wrapper_System_Int32_, 1845517801504066080ULL, 4u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_SnapshotTestFixtures_Wrapper_System_Int32_ = static_cast<CHAOS_IL2CPP_INTPTR>(1845517801504066080ULL);
inline TypeInfoV0 chaos_mt_System_Collections_System_Collections_Generic_Dictionary_System_Int32_System_Int32_ = {{nullptr, nullptr, 9130424216747936531ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Collections_System_Collections_Generic_Dictionary_System_Int32_System_Int32_ = static_cast<CHAOS_IL2CPP_INTPTR>(9130424216747936531ULL);
inline TypeInfoV0 chaos_mt_System_Collections_System_Collections_Generic_HashSet_System_Int32_ = {{nullptr, nullptr, 11554060213542038524ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Collections_System_Collections_Generic_HashSet_System_Int32_ = static_cast<CHAOS_IL2CPP_INTPTR>(11554060213542038524ULL);
inline TypeInfoV0 chaos_mt_System_Collections_System_Collections_Generic_List_System_Int32_ = {{nullptr, nullptr, 8124737553590710378ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Collections_System_Collections_Generic_List_System_Int32_ = static_cast<CHAOS_IL2CPP_INTPTR>(8124737553590710378ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Byte = {{nullptr, nullptr, 3217969471912086765ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Byte = static_cast<CHAOS_IL2CPP_INTPTR>(3217969471912086765ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Delegate = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, nullptr, 7451128447593600616ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Delegate = static_cast<CHAOS_IL2CPP_INTPTR>(7451128447593600616ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_DivideByZeroException = {{nullptr, nullptr, 14499175579798713746ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_DivideByZeroException = static_cast<CHAOS_IL2CPP_INTPTR>(14499175579798713746ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Double = {{nullptr, nullptr, 1099026126681926114ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Double = static_cast<CHAOS_IL2CPP_INTPTR>(1099026126681926114ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Exception = {{nullptr, nullptr, 10972282733316558392ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Exception = static_cast<CHAOS_IL2CPP_INTPTR>(10972282733316558392ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_MulticastDelegate = {{&chaos_mt_System_Private_CoreLib_System_Delegate.hot, nullptr, 6681393039041505440ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_MulticastDelegate = static_cast<CHAOS_IL2CPP_INTPTR>(6681393039041505440ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Int32_ = {{&chaos_mt_System_Private_CoreLib_System_MulticastDelegate.hot, nullptr, 143744549824129343ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Func_System_Int32_System_Int32_ = static_cast<CHAOS_IL2CPP_INTPTR>(143744549824129343ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Func_System_Int32_ = {{&chaos_mt_System_Private_CoreLib_System_MulticastDelegate.hot, nullptr, 8866194404114377402ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Func_System_Int32_ = static_cast<CHAOS_IL2CPP_INTPTR>(8866194404114377402ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Int16 = {{nullptr, nullptr, 11007710000311077999ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Int16 = static_cast<CHAOS_IL2CPP_INTPTR>(11007710000311077999ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Int64 = {{nullptr, nullptr, 11012495074916106146ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Int64 = static_cast<CHAOS_IL2CPP_INTPTR>(11012495074916106146ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_OverflowException = {{nullptr, nullptr, 531363262308024406ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_OverflowException = static_cast<CHAOS_IL2CPP_INTPTR>(531363262308024406ULL);
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
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_SByte = {{nullptr, nullptr, 13054279835592181566ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_SByte = static_cast<CHAOS_IL2CPP_INTPTR>(13054279835592181566ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Single = {{nullptr, nullptr, 4813876735449938439ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Single = static_cast<CHAOS_IL2CPP_INTPTR>(4813876735449938439ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_String = {{nullptr, nullptr, 1782325859292956794ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_String = static_cast<CHAOS_IL2CPP_INTPTR>(1782325859292956794ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Type = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, nullptr, 3222174513575444759ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Type = static_cast<CHAOS_IL2CPP_INTPTR>(3222174513575444759ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_UInt16 = {{nullptr, nullptr, 4060647147365029652ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_UInt16 = static_cast<CHAOS_IL2CPP_INTPTR>(4060647147365029652ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_UInt32 = {{nullptr, nullptr, 4058804365876526466ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_UInt32 = static_cast<CHAOS_IL2CPP_INTPTR>(4058804365876526466ULL);
inline TypeInfoV0 chaos_mt_SnapshotTestFixtures_IMarker = {{nullptr, nullptr, 13604930402668384013ULL, 0u, 32, 3, 0}, {nullptr, nullptr, 0, 0, 0, 0}};
inline TypeInfoV0 chaos_mt_SnapshotTestFixtures_IMyInterface = {{nullptr, nullptr, 3080637198406087848ULL, 0u, 32, 3, 0}, {nullptr, nullptr, 0, 0, 0, 0}};
inline TypeInfoV0 chaos_mt_SnapshotTestFixtures_MyValueType = {{nullptr, nullptr, 3606843832057080951ULL, 0u, 32, 2, 0}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_SnapshotTestFixtures_MyValueType = static_cast<CHAOS_IL2CPP_INTPTR>(3606843832057080951ULL);
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
	{ 0x000000A8u, reinterpret_cast<void*>(&SnapshotTestFixtures_BaseClass__ctor) },
	{ 0x000000A7u, reinterpret_cast<void*>(&SnapshotTestFixtures_BaseClass_Compute) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_SnapshotTestFixtures_DerivedClass[] =
{
	{ 0x000000AAu, reinterpret_cast<void*>(&SnapshotTestFixtures_DerivedClass__ctor) },
	{ 0x000000A9u, reinterpret_cast<void*>(&SnapshotTestFixtures_DerivedClass_Compute) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_SnapshotTestFixtures_HasFields[] =
{
	{ 0x000000C4u, reinterpret_cast<void*>(&SnapshotTestFixtures_HasFields__ctor) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_SnapshotTestFixtures_HasInstanceFields[] =
{
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0x000000B2u, reinterpret_cast<void*>(&SnapshotTestFixtures_HasInstanceFields__ctor_System_Int32) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_SnapshotTestFixtures_ImplHelper[] =
{
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0x0000011Bu, reinterpret_cast<void*>(&SnapshotTestFixtures_ImplHelper__ctor_System_Int32) },
	{ 0x0000011Cu, reinterpret_cast<void*>(&SnapshotTestFixtures_ImplHelper_GetValue) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_SnapshotTestFixtures_InstanceHelper[] =
{
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0x0000008Fu, reinterpret_cast<void*>(&SnapshotTestFixtures_InstanceHelper__ctor_System_Int32) },
	{ 0x00000091u, reinterpret_cast<void*>(&SnapshotTestFixtures_InstanceHelper_GetValue) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_SnapshotTestFixtures_MarkedClass[] =
{
	{ 0x000000ACu, reinterpret_cast<void*>(&SnapshotTestFixtures_MarkedClass__ctor) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_SnapshotTestFixtures_MyClass[] =
{
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0x000000DFu, reinterpret_cast<void*>(&SnapshotTestFixtures_MyClass__ctor_System_Int32) },
	{ 0x000000E0u, reinterpret_cast<void*>(&SnapshotTestFixtures_MyClass_GetValue) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_SnapshotTestFixtures_SealedHelper[] =
{
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0x00000117u, reinterpret_cast<void*>(&SnapshotTestFixtures_SealedHelper__ctor_System_Int32) },
	{ 0u, nullptr },
	{ 0x00000118u, reinterpret_cast<void*>(&SnapshotTestFixtures_SealedHelper_GetValueVirtual) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_SnapshotTestFixtures_Wrapper__0_[] =
{
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0x00000128u, reinterpret_cast<void*>(&SnapshotTestFixtures_Wrapper__0__ctor_0) },
	{ 0x00000129u, reinterpret_cast<void*>(&SnapshotTestFixtures_Wrapper__0_GetValue) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_SnapshotTestFixtures_Wrapper_System_Int32_[] =
{
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0x00000126u, reinterpret_cast<void*>(&chaos_stub_definition_SnapshotTestFixtures_Wrapper_1___ctor_System_Void__0__type__System_Int32__method) },
	{ 0x00000127u, reinterpret_cast<void*>(&chaos_stub_definition_SnapshotTestFixtures_Wrapper_1__GetValue__0___type__System_Int32__method) },
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
	CHAOS_IL2CPP_INT32 field_SnapshotTestFixtures_HasFields__instanceVal = 0;
};

struct chaos_type_SnapshotTestFixtures_HasInstanceFields : public chaos_type_System_Private_CoreLib_System_Object
{
	CHAOS_IL2CPP_INT32 field_SnapshotTestFixtures_HasInstanceFields__Value = 0;
};

struct chaos_type_SnapshotTestFixtures_ImplHelper : public chaos_type_System_Private_CoreLib_System_Object
{
	CHAOS_IL2CPP_INT32 field_SnapshotTestFixtures_ImplHelper___val = 0;
};

struct chaos_type_SnapshotTestFixtures_InstanceHelper : public chaos_type_System_Private_CoreLib_System_Object
{
	CHAOS_IL2CPP_INT32 field_SnapshotTestFixtures_InstanceHelper___val = 0;
};

struct chaos_type_SnapshotTestFixtures_LdftnHelper___O
{
	ThinLockableHeader header{};
};

struct chaos_type_SnapshotTestFixtures_MarkedClass : public chaos_type_System_Private_CoreLib_System_Object
{
	CHAOS_IL2CPP_INT32 field_SnapshotTestFixtures_MarkedClass__Value = 0;
};

struct chaos_type_SnapshotTestFixtures_MyClass : public chaos_type_System_Private_CoreLib_System_Object
{
	CHAOS_IL2CPP_INT32 field_SnapshotTestFixtures_MyClass___val = 0;
};

struct chaos_type_SnapshotTestFixtures_SealedHelper : public chaos_type_System_Private_CoreLib_System_Object
{
	CHAOS_IL2CPP_INT32 field_SnapshotTestFixtures_SealedHelper___val = 0;
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
	CHAOS_IL2CPP_INT32 field_SnapshotTestFixtures_Wrapper_System_Int32____val = 0;
};

struct chaos_type_System_Collections_System_Collections_Generic_Dictionary_System_Int32_System_Int32_
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR chaos_native_storage = 0;  // native runtime storage ptr
};

struct chaos_type_System_Collections_System_Collections_Generic_HashSet_System_Int32_
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR chaos_native_storage = 0;  // native runtime storage ptr
};

struct chaos_type_System_Collections_System_Collections_Generic_List_System_Int32_
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR items_array = 0;  // GC array reference
	CHAOS_IL2CPP_INT32 size = 0;           // element count
	CHAOS_IL2CPP_INT32 version = 0;        // modification counter
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

struct chaos_type_System_Private_CoreLib_System_DivideByZeroException
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR _message = 0;
	CHAOS_IL2CPP_INTPTR _innerException = 0;
	CHAOS_IL2CPP_INTPTR _stackTrace = 0;
	CHAOS_IL2CPP_INT32 _HResult = 0;
};

struct chaos_type_System_Private_CoreLib_System_Double
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Private_CoreLib_System_Exception
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR _message = 0;
	CHAOS_IL2CPP_INTPTR _innerException = 0;
	CHAOS_IL2CPP_INTPTR _stackTrace = 0;
	CHAOS_IL2CPP_INT32 _HResult = 0;
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

struct chaos_valuetype_SnapshotTestFixtures_MyValueType
{
	CHAOS_IL2CPP_INT32 field_SnapshotTestFixtures_MyValueType__X = 0;
	CHAOS_IL2CPP_INT32 field_SnapshotTestFixtures_MyValueType__Y = 0;
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
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "CustomEntrySubject_8") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(104742814u);
			}

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
		case static_cast<CHAOS_IL2CPP_INTPTR>(48988052u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "CatchFinallyNested") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(102059296u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(34527326u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "FaultFinally") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(106346736u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(41189871u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "TryFault") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(102349707u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(41599390u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "FilterFinally") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(102256970u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(46154031u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "TryFilter") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101633017u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(47440284u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "TryFinally_Only") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107765658u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(35893753u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "SafeDivide") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(117304437u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(45955492u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "MultipleCatch") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(113855714u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(49264046u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "NestedTryCatch") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(108583560u);
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


// ── GcTypeLayout pointer offset arrays (computed via offsetof) ──
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Delegate[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Delegate, chaos_delegate_target)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Delegate, chaos_delegate_invocation_list))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_DivideByZeroException[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_DivideByZeroException, _message)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_DivideByZeroException, _innerException)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_DivideByZeroException, _stackTrace))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Exception[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Exception, _message)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Exception, _innerException)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Exception, _stackTrace))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_MulticastDelegate[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_MulticastDelegate, chaos_delegate_target)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_MulticastDelegate, chaos_delegate_invocation_list))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_OverflowException[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_OverflowException, _message)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_OverflowException, _innerException)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_OverflowException, _stackTrace))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_Assembly[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_Assembly, runtime_assembly_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, declaring_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, runtime_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, generic_definition_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, generic_argument_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, declaring_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, declaring_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, runtime_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, generic_definition_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, generic_argument_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Type[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Type, runtime_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Type, runtime_name_value))};

extern "C" void ChaosRegisterGcLayouts() {
	auto& registry = chaos::il2cpp::runtime_core::GcLayoutRegistry::Instance();

	registry.Register(17700440604878766952ULL, sizeof(chaos_type_SnapshotProverSubjects_SnapshotProverSubjects), nullptr, 0);
	registry.Register(16934624807659080947ULL, sizeof(chaos_type_SnapshotTestFixtures_BaseClass), nullptr, 0);
	registry.Register(5213792135327273735ULL, sizeof(chaos_type_SnapshotTestFixtures_DerivedClass), nullptr, 0);
	registry.Register(14105055411266533488ULL, sizeof(chaos_type_SnapshotTestFixtures_FieldHelper), nullptr, 0);
	registry.Register(15228727185366376748ULL, sizeof(chaos_type_System_Private_CoreLib_System_Object), nullptr, 0);
	registry.Register(6075661983971539625ULL, sizeof(chaos_type_SnapshotTestFixtures_HasFields), nullptr, 0);
	registry.Register(17812473439196761566ULL, sizeof(chaos_type_SnapshotTestFixtures_HasInstanceFields), nullptr, 0);
	registry.Register(2682382514493266270ULL, sizeof(chaos_type_SnapshotTestFixtures_ImplHelper), nullptr, 0);
	registry.Register(7539435265304875683ULL, sizeof(chaos_type_SnapshotTestFixtures_InstanceHelper), nullptr, 0);
	registry.Register(9616880548282383388ULL, sizeof(chaos_type_SnapshotTestFixtures_LdftnHelper___O), nullptr, 0);
	registry.Register(6112983511997078372ULL, sizeof(chaos_type_SnapshotTestFixtures_MarkedClass), nullptr, 0);
	registry.Register(17017755788800598734ULL, sizeof(chaos_type_SnapshotTestFixtures_MyClass), nullptr, 0);
	registry.Register(8545221821474343780ULL, sizeof(chaos_type_SnapshotTestFixtures_SealedHelper), nullptr, 0);
	registry.Register(7187250757378180401ULL, sizeof(chaos_type_SnapshotTestFixtures_StaticFieldWriteHelper), nullptr, 0);
	registry.Register(15764982902837746946ULL, sizeof(chaos_type_SnapshotTestFixtures_Wrapper__0_), nullptr, 0);
	registry.Register(1845517801504066080ULL, sizeof(chaos_type_SnapshotTestFixtures_Wrapper_System_Int32_), nullptr, 0);
	registry.Register(9130424216747936531ULL, sizeof(chaos_type_System_Collections_System_Collections_Generic_Dictionary_System_Int32_System_Int32_), nullptr, 0);
	registry.Register(11554060213542038524ULL, sizeof(chaos_type_System_Collections_System_Collections_Generic_HashSet_System_Int32_), nullptr, 0);
	registry.Register(8124737553590710378ULL, sizeof(chaos_type_System_Collections_System_Collections_Generic_List_System_Int32_), nullptr, 0);
	registry.Register(3217969471912086765ULL, sizeof(chaos_type_System_Private_CoreLib_System_Byte), nullptr, 0);
	registry.Register(7451128447593600616ULL, sizeof(chaos_type_System_Private_CoreLib_System_Delegate), kGcOffsets_chaos_type_System_Private_CoreLib_System_Delegate, 2);
	registry.Register(14499175579798713746ULL, sizeof(chaos_type_System_Private_CoreLib_System_DivideByZeroException), kGcOffsets_chaos_type_System_Private_CoreLib_System_DivideByZeroException, 3);
	registry.Register(1099026126681926114ULL, sizeof(chaos_type_System_Private_CoreLib_System_Double), nullptr, 0);
	registry.Register(10972282733316558392ULL, sizeof(chaos_type_System_Private_CoreLib_System_Exception), kGcOffsets_chaos_type_System_Private_CoreLib_System_Exception, 3);
	registry.Register(6681393039041505440ULL, sizeof(chaos_type_System_Private_CoreLib_System_MulticastDelegate), kGcOffsets_chaos_type_System_Private_CoreLib_System_MulticastDelegate, 2);
	registry.Register(143744549824129343ULL, sizeof(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Int32_), nullptr, 0);
	registry.Register(8866194404114377402ULL, sizeof(chaos_type_System_Private_CoreLib_System_Func_System_Int32_), nullptr, 0);
	registry.Register(11007710000311077999ULL, sizeof(chaos_type_System_Private_CoreLib_System_Int16), nullptr, 0);
	registry.Register(11009693519287992193ULL, sizeof(chaos_type_System_Private_CoreLib_System_Int32), nullptr, 0);
	registry.Register(11012495074916106146ULL, sizeof(chaos_type_System_Private_CoreLib_System_Int64), nullptr, 0);
	registry.Register(531363262308024406ULL, sizeof(chaos_type_System_Private_CoreLib_System_OverflowException), kGcOffsets_chaos_type_System_Private_CoreLib_System_OverflowException, 3);
	registry.Register(5474029880995115448ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_Assembly), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_Assembly, 1);
	registry.Register(17082367815459723707ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName, 1);
	registry.Register(4137207361503509124ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, 5);
	registry.Register(17040031516751226236ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, 2);
	registry.Register(10748947813473285525ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, 5);
	registry.Register(13054279835592181566ULL, sizeof(chaos_type_System_Private_CoreLib_System_SByte), nullptr, 0);
	registry.Register(4813876735449938439ULL, sizeof(chaos_type_System_Private_CoreLib_System_Single), nullptr, 0);
	registry.Register(1782325859292956794ULL, sizeof(chaos_type_System_Private_CoreLib_System_String), nullptr, 0);
	registry.Register(3222174513575444759ULL, sizeof(chaos_type_System_Private_CoreLib_System_Type), kGcOffsets_chaos_type_System_Private_CoreLib_System_Type, 2);
	registry.Register(4060647147365029652ULL, sizeof(chaos_type_System_Private_CoreLib_System_UInt16), nullptr, 0);
	registry.Register(4058804365876526466ULL, sizeof(chaos_type_System_Private_CoreLib_System_UInt32), nullptr, 0);

	// Register TypeInfoV0 address ranges for IsValidTypeInfoPointer.
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_SnapshotProverSubjects_SnapshotProverSubjects), reinterpret_cast<uintptr_t>(&chaos_mt_SnapshotProverSubjects_SnapshotProverSubjects) + sizeof(chaos_mt_SnapshotProverSubjects_SnapshotProverSubjects));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_SnapshotTestFixtures_BaseClass), reinterpret_cast<uintptr_t>(&chaos_mt_SnapshotTestFixtures_BaseClass) + sizeof(chaos_mt_SnapshotTestFixtures_BaseClass));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_SnapshotTestFixtures_DerivedClass), reinterpret_cast<uintptr_t>(&chaos_mt_SnapshotTestFixtures_DerivedClass) + sizeof(chaos_mt_SnapshotTestFixtures_DerivedClass));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_SnapshotTestFixtures_FieldHelper), reinterpret_cast<uintptr_t>(&chaos_mt_SnapshotTestFixtures_FieldHelper) + sizeof(chaos_mt_SnapshotTestFixtures_FieldHelper));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Object), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Object) + sizeof(chaos_mt_System_Private_CoreLib_System_Object));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_SnapshotTestFixtures_HasFields), reinterpret_cast<uintptr_t>(&chaos_mt_SnapshotTestFixtures_HasFields) + sizeof(chaos_mt_SnapshotTestFixtures_HasFields));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_SnapshotTestFixtures_HasInstanceFields), reinterpret_cast<uintptr_t>(&chaos_mt_SnapshotTestFixtures_HasInstanceFields) + sizeof(chaos_mt_SnapshotTestFixtures_HasInstanceFields));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_SnapshotTestFixtures_ImplHelper), reinterpret_cast<uintptr_t>(&chaos_mt_SnapshotTestFixtures_ImplHelper) + sizeof(chaos_mt_SnapshotTestFixtures_ImplHelper));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_SnapshotTestFixtures_InstanceHelper), reinterpret_cast<uintptr_t>(&chaos_mt_SnapshotTestFixtures_InstanceHelper) + sizeof(chaos_mt_SnapshotTestFixtures_InstanceHelper));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_SnapshotTestFixtures_LdftnHelper___O), reinterpret_cast<uintptr_t>(&chaos_mt_SnapshotTestFixtures_LdftnHelper___O) + sizeof(chaos_mt_SnapshotTestFixtures_LdftnHelper___O));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_SnapshotTestFixtures_MarkedClass), reinterpret_cast<uintptr_t>(&chaos_mt_SnapshotTestFixtures_MarkedClass) + sizeof(chaos_mt_SnapshotTestFixtures_MarkedClass));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_SnapshotTestFixtures_MyClass), reinterpret_cast<uintptr_t>(&chaos_mt_SnapshotTestFixtures_MyClass) + sizeof(chaos_mt_SnapshotTestFixtures_MyClass));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_SnapshotTestFixtures_SealedHelper), reinterpret_cast<uintptr_t>(&chaos_mt_SnapshotTestFixtures_SealedHelper) + sizeof(chaos_mt_SnapshotTestFixtures_SealedHelper));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_SnapshotTestFixtures_StaticFieldWriteHelper), reinterpret_cast<uintptr_t>(&chaos_mt_SnapshotTestFixtures_StaticFieldWriteHelper) + sizeof(chaos_mt_SnapshotTestFixtures_StaticFieldWriteHelper));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_SnapshotTestFixtures_Wrapper__0_), reinterpret_cast<uintptr_t>(&chaos_mt_SnapshotTestFixtures_Wrapper__0_) + sizeof(chaos_mt_SnapshotTestFixtures_Wrapper__0_));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_SnapshotTestFixtures_Wrapper_System_Int32_), reinterpret_cast<uintptr_t>(&chaos_mt_SnapshotTestFixtures_Wrapper_System_Int32_) + sizeof(chaos_mt_SnapshotTestFixtures_Wrapper_System_Int32_));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Collections_System_Collections_Generic_Dictionary_System_Int32_System_Int32_), reinterpret_cast<uintptr_t>(&chaos_mt_System_Collections_System_Collections_Generic_Dictionary_System_Int32_System_Int32_) + sizeof(chaos_mt_System_Collections_System_Collections_Generic_Dictionary_System_Int32_System_Int32_));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Collections_System_Collections_Generic_HashSet_System_Int32_), reinterpret_cast<uintptr_t>(&chaos_mt_System_Collections_System_Collections_Generic_HashSet_System_Int32_) + sizeof(chaos_mt_System_Collections_System_Collections_Generic_HashSet_System_Int32_));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Collections_System_Collections_Generic_List_System_Int32_), reinterpret_cast<uintptr_t>(&chaos_mt_System_Collections_System_Collections_Generic_List_System_Int32_) + sizeof(chaos_mt_System_Collections_System_Collections_Generic_List_System_Int32_));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Byte), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Byte) + sizeof(chaos_mt_System_Private_CoreLib_System_Byte));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Delegate), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Delegate) + sizeof(chaos_mt_System_Private_CoreLib_System_Delegate));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_DivideByZeroException), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_DivideByZeroException) + sizeof(chaos_mt_System_Private_CoreLib_System_DivideByZeroException));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Double), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Double) + sizeof(chaos_mt_System_Private_CoreLib_System_Double));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Exception), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Exception) + sizeof(chaos_mt_System_Private_CoreLib_System_Exception));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_MulticastDelegate), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_MulticastDelegate) + sizeof(chaos_mt_System_Private_CoreLib_System_MulticastDelegate));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Int32_), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Int32_) + sizeof(chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Int32_));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Func_System_Int32_), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Func_System_Int32_) + sizeof(chaos_mt_System_Private_CoreLib_System_Func_System_Int32_));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Int16), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Int16) + sizeof(chaos_mt_System_Private_CoreLib_System_Int16));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Int32), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Int32) + sizeof(chaos_mt_System_Private_CoreLib_System_Int32));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Int64), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Int64) + sizeof(chaos_mt_System_Private_CoreLib_System_Int64));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_OverflowException), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_OverflowException) + sizeof(chaos_mt_System_Private_CoreLib_System_OverflowException));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_Assembly), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_Assembly) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_Assembly));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_SByte), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_SByte) + sizeof(chaos_mt_System_Private_CoreLib_System_SByte));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Single), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Single) + sizeof(chaos_mt_System_Private_CoreLib_System_Single));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_String), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_String) + sizeof(chaos_mt_System_Private_CoreLib_System_String));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Type), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Type) + sizeof(chaos_mt_System_Private_CoreLib_System_Type));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_UInt16), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_UInt16) + sizeof(chaos_mt_System_Private_CoreLib_System_UInt16));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_UInt32), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_UInt32) + sizeof(chaos_mt_System_Private_CoreLib_System_UInt32));
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
	auto* _list = reinterpret_cast<chaos_list_fields*>(reinterpret_cast<char*>(chaos_arg_0) + 8);
	auto* hdr = reinterpret_cast<chaos_list_array_header*>(_list->items_array);
	if (hdr == nullptr || _list->size >= hdr->capacity) {
		auto old_cap = (hdr != nullptr) ? hdr->capacity : 0;
		auto new_cap = (old_cap == 0) ? 4 : old_cap * 2;
		auto* new_buf = static_cast<CHAOS_IL2CPP_INTPTR*>(CHAOS_IL2CPP_MALLOC(sizeof(CHAOS_IL2CPP_INT32) + static_cast<CHAOS_IL2CPP_SIZE>(new_cap) * sizeof(CHAOS_IL2CPP_INTPTR)));
		auto* new_hdr = reinterpret_cast<chaos_list_array_header*>(new_buf);
		new_hdr->capacity = new_cap;
		auto* new_elems = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(new_hdr + 1);
		if (hdr != nullptr && _list->size > 0) {
			auto* old_elems = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(hdr + 1);
			std::memcpy(new_elems, old_elems, static_cast<CHAOS_IL2CPP_SIZE>(_list->size) * sizeof(CHAOS_IL2CPP_INTPTR));
			CHAOS_IL2CPP_FREE(hdr);
		}
		_list->items_array = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(new_hdr);
		hdr = new_hdr;
	}
	auto* elems = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(hdr + 1);
	elems[_list->size] = chaos_arg_1;
	_list->size++;
	_list->version++;
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


// ── Bridge/import thunks ──
extern "C" void* kChaosExternalRuntimeFnTable[];

// Bridge/import thunk for: SnapshotProverSubjects/SnapshotProverSubjects::_exitCode
extern "C" void chaos_bridge_thunk_0(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[0])();
}

// Bridge/import thunk for: SnapshotTestFixtures/HasFields::instanceVal
extern "C" void chaos_bridge_thunk_1(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[1])();
}

// Bridge/import thunk for: System.Private.CoreLib/System.Func<System.Int32>::Invoke:System.Int32()
extern "C" CHAOS_IL2CPP_INT32 chaos_bridge_thunk_10(void)
{
	auto result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)()>(kChaosExternalRuntimeFnTable[19])();
	return result;
}

// Bridge/import thunk for: System.Private.CoreLib/System.Exception
extern "C" void chaos_bridge_thunk_11(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[20])();
}

// Bridge/import thunk for: SnapshotTestFixtures/FieldHelper::_counter
extern "C" void chaos_bridge_thunk_12(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[21])();
}

// Bridge/import thunk for: SnapshotTestFixtures/HasInstanceFields::Value
extern "C" void chaos_bridge_thunk_13(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[22])();
}

// Bridge/import thunk for: SnapshotTestFixtures/ImplHelper::_val
extern "C" void chaos_bridge_thunk_14(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[23])();
}

// Bridge/import thunk for: SnapshotTestFixtures/InstanceHelper::_val
extern "C" void chaos_bridge_thunk_15(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[24])();
}

// Bridge/import thunk for: SnapshotTestFixtures/LdftnHelper+<>O::<0>__GetValue
extern "C" void chaos_bridge_thunk_16(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[25])();
}

// Bridge/import thunk for: System.Private.CoreLib/System.SByte
extern "C" void chaos_bridge_thunk_17(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[26])();
}

// Bridge/import thunk for: System.Private.CoreLib/System.Int64
extern "C" void chaos_bridge_thunk_18(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[27])();
}

// Bridge/import thunk for: System.Private.CoreLib/System.Byte
extern "C" void chaos_bridge_thunk_19(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[28])();
}

// Bridge/import thunk for: System.Private.CoreLib/System.Int32
extern "C" void chaos_bridge_thunk_2(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[2])();
}

// Bridge/import thunk for: System.Private.CoreLib/System.UInt16
extern "C" void chaos_bridge_thunk_20(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[29])();
}

// Bridge/import thunk for: System.Private.CoreLib/System.UInt32
extern "C" void chaos_bridge_thunk_21(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[30])();
}

// Bridge/import thunk for: System.Private.CoreLib/System.RuntimeTypeHandle::get_Value:System.IntPtr()
extern "C" CHAOS_IL2CPP_INTPTR chaos_bridge_thunk_22(void)
{
	auto result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)()>(kChaosExternalRuntimeFnTable[33])();
	return result;
}

// Bridge/import thunk for: System.Private.CoreLib/System.IntPtr::ToInt32:System.Int32()
extern "C" CHAOS_IL2CPP_INT32 chaos_bridge_thunk_23(void)
{
	auto result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)()>(kChaosExternalRuntimeFnTable[34])();
	return result;
}

// Bridge/import thunk for: SnapshotTestFixtures/MyClass::_val
extern "C" void chaos_bridge_thunk_24(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[35])();
}

// Bridge/import thunk for: SnapshotTestFixtures/SealedHelper::_val
extern "C" void chaos_bridge_thunk_25(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[36])();
}

// Bridge/import thunk for: SnapshotTestFixtures/SmallStruct
extern "C" void chaos_bridge_thunk_26(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[37])();
}

// Bridge/import thunk for: SnapshotTestFixtures/StaticFieldWriteHelper::_writeTarget
extern "C" void chaos_bridge_thunk_27(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[38])();
}

// Bridge/import thunk for: SnapshotTestFixtures/StaticFieldWriteHelper::_readTarget
extern "C" void chaos_bridge_thunk_28(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[39])();
}

// Bridge/import thunk for: System.Private.CoreLib/System.Int16
extern "C" void chaos_bridge_thunk_29(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[40])();
}

// Bridge/import thunk for: System.Private.CoreLib/System.Func<System.Int32,System.Int32>::.ctor:System.Void(System.Object,System.IntPtr)
extern "C" void chaos_bridge_thunk_3(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[4])(chaos_fn_arg_0, chaos_fn_arg_1);
}

// Bridge/import thunk for: System.Private.CoreLib/System.Single
extern "C" void chaos_bridge_thunk_30(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[41])();
}

// Bridge/import thunk for: System.Private.CoreLib/System.Double
extern "C" void chaos_bridge_thunk_31(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[42])();
}

// Bridge/import thunk for: System.Private.CoreLib/System.Object
extern "C" void chaos_bridge_thunk_32(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[43])();
}

// Bridge/import thunk for: SnapshotTestFixtures/MarkedClass
extern "C" void chaos_bridge_thunk_33(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[45])();
}

// Bridge/import thunk for: SnapshotTestFixtures/MarkedClass::Value
extern "C" void chaos_bridge_thunk_34(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[46])();
}

// Bridge/import thunk for: System.Private.CoreLib/System.Double::IsFinite:System.Boolean(System.Double)
extern "C" CHAOS_IL2CPP_INTPTR chaos_bridge_thunk_35(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	auto result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[47])(chaos_fn_arg_0);
	return result;
}

// Bridge/import thunk for: SnapshotTestFixtures/MyValueType
extern "C" void chaos_bridge_thunk_36(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[48])();
}

// Bridge/import thunk for: SnapshotTestFixtures/MyValueType::X
extern "C" void chaos_bridge_thunk_37(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[49])();
}

// Bridge/import thunk for: SnapshotTestFixtures/MyValueType::Y
extern "C" void chaos_bridge_thunk_38(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[50])();
}

// Bridge/import thunk for: SnapshotTestFixtures/Wrapper<!0>::_val
extern "C" void chaos_bridge_thunk_39(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[51])();
}

// Bridge/import thunk for: System.Private.CoreLib/System.Func<System.Int32,System.Int32>::Invoke:System.Int32(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 chaos_bridge_thunk_4(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	auto result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[5])(chaos_fn_arg_0);
	return result;
}

// Bridge/import thunk for: SnapshotTestFixtures/Wrapper<System.Int32>::_val
extern "C" void chaos_bridge_thunk_40(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[52])();
}

// Bridge/import thunk for: System.Collections/System.Collections.Generic.Dictionary<System.Int32,System.Int32>::.ctor:System.Void()
extern "C" void chaos_bridge_thunk_5(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[6])();
}

// Bridge/import thunk for: System.Collections/System.Collections.Generic.List<System.Int32>::.ctor:System.Void()
extern "C" void chaos_bridge_thunk_6(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[11])();
}

// Bridge/import thunk for: System.Collections/System.Collections.Generic.List<System.Int32>::get_Count:System.Int32()
extern "C" CHAOS_IL2CPP_INT32 chaos_bridge_thunk_7(void)
{
	auto result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)()>(kChaosExternalRuntimeFnTable[13])();
	return result;
}

// Bridge/import thunk for: System.Collections/System.Collections.Generic.HashSet<System.Int32>::.ctor:System.Void()
extern "C" void chaos_bridge_thunk_8(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[14])();
}

// Bridge/import thunk for: System.Private.CoreLib/System.Func<System.Int32>::.ctor:System.Void(System.Object,System.IntPtr)
extern "C" void chaos_bridge_thunk_9(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[18])(chaos_fn_arg_0, chaos_fn_arg_1);
}

extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_0(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_1(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_2(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_3(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_4(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_5(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_6(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_Subject_7(void);
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_CustomEntrySubject_8(void);
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
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_EhCatchFinallyNestedHelper_CatchFinallyNested_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_EhFaultFinallyHelper_FaultFinally_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_EhFaultHelper_TryFault_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_EhFilterFinallyHelper_FilterFinally_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_EhFilterHelper_TryFilter_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_EhFinallyHelper_TryFinally_Only_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_EhHelper_SafeDivide(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_EhMultipleCatchHelper_MultipleCatch_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_EhNestedTryCatchHelper_NestedTryCatch_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
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
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[166] = {
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
	2u,
	3u,
	4u,
	5u,
	6u,
	7u,
	7u,
	8u,
	9u,
	9u,
	9u,
	9u,
	9u,
	9u,
	9u,
	9u,
	10u,
	11u,
	11u,
	11u,
	12u,
	12u,
	12u,
	13u,
	13u,
	13u,
	13u,
	14u,
	14u,
	14u,
	15u,
	16u,
	16u,
	16u,
	16u,
	16u,
	16u,
	16u,
	16u,
	16u,
	16u,
	16u,
	16u,
	16u,
	16u,
	16u,
	16u,
	16u,
	16u,
	16u,
	16u,
	16u,
	16u,
	16u,
	16u,
	16u,
	17u,
	17u,
	17u,
	18u,
	19u,
	20u,
	21u,
	22u,
	23u,
	24u,
	25u,
	26u,
	27u,
	28u,
	29u,
	29u,
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
	35u,
	36u,
	37u,
	37u,
	37u,
	38u,
	38u,
	38u,
	38u,
	39u,
	40u,
	41u,
	42u,
	42u,
	42u,
	42u,
	42u,
	42u,
	42u,
	42u,
	42u,
	42u,
	42u,
	42u,
	42u,
	43u,
	43u,
	43u,
	44u,
	44u,
	44u,
	44u,
	44u,
	44u,
	44u,
	44u,
	45u,
	45u,
	46u,
	46u,
	47u,
	47u,
};

static constexpr struct {
	::ChaosAbiManifestV0 header;
	::ChaosAbiMethodEntryV0 entries[165];
	CHAOS_IL2CPP_UINT8 params[47];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		165u,
		47u,
		2514420624u,  // FNV-1a over entries+params
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
		{ 0u, 0u },  // SnapshotProverSubjects_SnapshotProverSubjects_CustomEntrySubject_8
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
		{ 1u, 1u },  // SnapshotTestFixtures_EhCatchFinallyNestedHelper_CatchFinallyNested_System_Int32
		{ 1u, 1u },  // SnapshotTestFixtures_EhFaultFinallyHelper_FaultFinally_System_Int32
		{ 1u, 1u },  // SnapshotTestFixtures_EhFaultHelper_TryFault_System_Int32
		{ 1u, 1u },  // SnapshotTestFixtures_EhFilterFinallyHelper_FilterFinally_System_Int32
		{ 1u, 1u },  // SnapshotTestFixtures_EhFilterHelper_TryFilter_System_Int32
		{ 1u, 1u },  // SnapshotTestFixtures_EhFinallyHelper_TryFinally_Only_System_Int32
		{ 1u, 0u },  // SnapshotTestFixtures_EhHelper_SafeDivide
		{ 1u, 1u },  // SnapshotTestFixtures_EhMultipleCatchHelper_MultipleCatch_System_Int32
		{ 1u, 1u },  // SnapshotTestFixtures_EhNestedTryCatchHelper_NestedTryCatch_System_Int32
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
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
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
		12u,
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
		::chaos::il2cpp::runtime_core::RegisterModule("SnapshotProverSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[165] = {
	{ "RunAddress", 0x000000C5u, 0u },  // AddressHelper
	{ "RunCompare", 0x000000B0u, 0u },  // ArithmeticCompareHelper
	{ "RunAdd", 0x00000085u, 0u },  // ArithmeticOps
	{ "MakeAndFill", 0x000000A1u, 0u },  // ArrayHelper
	{ "RunLength", 0x000000B8u, 0u },  // ArrayLengthHelper
	{ "RunArrayRef", 0x000000D2u, 0u },  // ArrayRefHelper
	{ ".ctor", 0x000000A8u, 0u },  // BaseClass
	{ "Compute", 0x000000A7u, 0u },  // BaseClass
	{ "RunBitwise", 0x000000B6u, 0u },  // BitwiseHelper
	{ "DemoBoxStore", 0x00000114u, 0u },  // BoxInterfaceArrayDemo
	{ "BoxAndUnbox", 0x00000095u, 0u },  // BoxingHelper
	{ "RunBranchCompareB", 0x000000C3u, 0u },  // BranchCompareBHelper
	{ "RunBranchCompare", 0x000000BCu, 0u },  // BranchCompareHelper
	{ "RunBranchDup", 0x000000B1u, 0u },  // BranchDupHelper
	{ "RunBranchUnsignedB", 0x000000EDu, 0u },  // BranchUnsignedBHelper
	{ "RunBranchUnsigned", 0x000000ECu, 0u },  // BranchUnsignedHelper
	{ "CreateAndUse", 0x00000093u, 0u },  // CallVirtHelper
	{ "RunCalli", 0x000000DEu, 1u },  // CalliHelper
	{ "RunCallvirt", 0x000000CBu, 0u },  // CallvirtHelper
	{ "TestDict", 0x0000010Du, 0u },  // CollectionsHelper
	{ "TestList", 0x0000010Cu, 0u },  // CollectionsHelper
	{ "TestSet", 0x0000010Eu, 0u },  // CollectionsHelper
	{ "RunConvSmall", 0x000000C1u, 0u },  // ConvSmallIntHelper
	{ "RunConvWide", 0x000000BEu, 0u },  // ConvWideHelper
	{ "ConvertToInt", 0x00000099u, 0u },  // ConversionHelper
	{ "RunCpblk", 0x000000CDu, 0u },  // CpblkHelper
	{ "RunCpobj", 0x000000C9u, 0u },  // CpobjHelper
	{ "RunDelegate", 0x000000A6u, 0u },  // DelegateHelper
	{ ".ctor", 0x000000AAu, 0u },  // DerivedClass
	{ "Compute", 0x000000A9u, 0u },  // DerivedClass
	{ "CatchFinallyNested", 0x00000122u, 1u },  // EhCatchFinallyNestedHelper
	{ "FaultFinally", 0x00000125u, 1u },  // EhFaultFinallyHelper
	{ "TryFault", 0x0000011Fu, 1u },  // EhFaultHelper
	{ "FilterFinally", 0x00000123u, 1u },  // EhFilterFinallyHelper
	{ "TryFilter", 0x00000120u, 1u },  // EhFilterHelper
	{ "TryFinally_Only", 0x0000011Eu, 1u },  // EhFinallyHelper
	{ "SafeDivide", 0x000000A2u, 0u },  // EhHelper
	{ "MultipleCatch", 0x00000121u, 1u },  // EhMultipleCatchHelper
	{ "NestedTryCatch", 0x00000124u, 1u },  // EhNestedTryCatchHelper
	{ "CallExternal", 0x0000010Fu, 0u },  // ExternalCaller
	{ "GetAndIncrement", 0x00000097u, 0u },  // FieldHelper
	{ "RunFloatOps", 0x000000BDu, 0u },  // FloatOpsHelper
	{ "IsPositive", 0x0000008Du, 0u },  // FlowControl
	{ "UseGeneric", 0x000000A3u, 0u },  // GenericHelper
	{ "DemoCombine", 0x00000113u, 0u },  // GenericsVirtEhDemo
	{ ".ctor", 0x000000C4u, 0u },  // HasFields
	{ ".ctor", 0x000000B2u, 1u },  // HasInstanceFields
	{ "ConsumeString", 0x0000009Cu, 1u },  // Helper
	{ "GetValue", 0x0000009Du, 0u },  // Helper
	{ "Nop", 0x0000009Au, 0u },  // Helper
	{ "Square", 0x0000009Bu, 1u },  // Helper
	{ "Run", 0x00000116u, 0u },  // HotUpdateWithGenericsDemo
	{ "Run", 0x00000115u, 0u },  // HotUpdateWithTypesDemo
	{ ".ctor", 0x0000011Bu, 1u },  // ImplHelper
	{ "GetValue", 0x0000011Cu, 0u },  // ImplHelper
	{ "ReadWriteRef", 0x000000AFu, 0u },  // IndirectHelper
	{ "CreateAndUseFields", 0x000000B3u, 0u },  // InstanceFieldHelper
	{ ".ctor", 0x0000008Fu, 1u },  // InstanceHelper
	{ "GetValue", 0x00000091u, 0u },  // InstanceHelper
	{ "RunInterfaceTest", 0x0000011Du, 0u },  // InterfaceDevirtHelper
	{ "Double", 0x00000111u, 1u },  // InternalHelper
	{ "RunLdVirtftn", 0x000000E1u, 0u },  // LdVirtftnHelper
	{ "RunLdarga", 0x000000C8u, 1u },  // LdargaHelper
	{ "RunLdcI8", 0x000000C0u, 0u },  // LdcI8Helper
	{ "TestAllElems", 0x000000E2u, 0u },  // LdelemAllHelper
	{ "RunLdelema", 0x000000C7u, 0u },  // LdelemaHelper
	{ "GetFnPtr", 0x0000009Fu, 0u },  // LdftnHelper
	{ "RunLdindI1", 0x000000C2u, 0u },  // LdindI1Helper
	{ "RunLdindI8", 0x000000CFu, 0u },  // LdindI8Helper
	{ "RunLdindI", 0x0000010Au, 0u },  // LdindStindGapsHelper
	{ "RunLdindI4", 0x00000109u, 0u },  // LdindStindGapsHelper
	{ "RunStindI", 0x0000010Bu, 0u },  // LdindStindGapsHelper
	{ "RunLdindUnsigned", 0x000000CEu, 0u },  // LdindUnsignedHelper
	{ "RunLdindWide", 0x000000D0u, 0u },  // LdindWideHelper
	{ "RunLdtoken", 0x000000C6u, 0u },  // LdtokenHelper
	{ "RunAlloc", 0x000000BAu, 0u },  // LocalAllocHelper
	{ "SumToFive", 0x00000098u, 0u },  // LoopHelper
	{ ".ctor", 0x000000ACu, 0u },  // MarkedClass
	{ "RunSquare", 0x0000008Bu, 0u },  // MathHelper
	{ "RunArglist", 0x000000F2u, 0u },  // MoreRareOpsHelper
	{ "RunJmp", 0x000000F6u, 0u },  // MoreRareOpsHelper
	{ "RunMkrefany", 0x000000F3u, 0u },  // MoreRareOpsHelper
	{ "RunRefanytype", 0x000000F5u, 0u },  // MoreRareOpsHelper
	{ "RunRefanyval", 0x000000F4u, 0u },  // MoreRareOpsHelper
	{ "RunMulDiv", 0x000000BBu, 0u },  // MulDivHelper
	{ "DispatchBoth", 0x00000110u, 0u },  // MultiDispatcher
	{ ".ctor", 0x000000DFu, 1u },  // MyClass
	{ "GetValue", 0x000000E0u, 0u },  // MyClass
	{ "RunNegShift", 0x000000BFu, 0u },  // NegShiftHelper
	{ "ConvOvfI1", 0x000000F7u, 1u },  // OverflowConvExtHelper
	{ "ConvOvfI1Un", 0x000000F8u, 1u },  // OverflowConvExtHelper
	{ "ConvOvfI2Un", 0x000000FAu, 1u },  // OverflowConvExtHelper
	{ "ConvOvfI4Un", 0x000000FCu, 1u },  // OverflowConvExtHelper
	{ "ConvOvfI8Un", 0x000000FEu, 1u },  // OverflowConvExtHelper
	{ "ConvOvfIUn", 0x00000100u, 1u },  // OverflowConvExtHelper
	{ "ConvOvfU1Un", 0x000000F9u, 1u },  // OverflowConvExtHelper
	{ "ConvOvfU2Un", 0x000000FBu, 1u },  // OverflowConvExtHelper
	{ "ConvOvfU4Un", 0x000000FDu, 1u },  // OverflowConvExtHelper
	{ "ConvOvfU8Un", 0x000000FFu, 1u },  // OverflowConvExtHelper
	{ "ConvOvfUUn", 0x00000101u, 1u },  // OverflowConvExtHelper
	{ "ConvOvfI2", 0x000000D9u, 1u },  // OverflowConvHelper
	{ "ConvOvfI4", 0x000000D7u, 0u },  // OverflowConvHelper
	{ "ConvOvfI8", 0x000000DCu, 1u },  // OverflowConvHelper
	{ "ConvOvfU1", 0x000000D8u, 1u },  // OverflowConvHelper
	{ "ConvOvfU2", 0x000000DAu, 1u },  // OverflowConvHelper
	{ "ConvOvfU4", 0x000000DBu, 1u },  // OverflowConvHelper
	{ "ConvOvfU8", 0x000000DDu, 1u },  // OverflowConvHelper
	{ "RunOverflowAdd", 0x000000D4u, 0u },  // OverflowHelper
	{ "RunOverflowMul", 0x000000D6u, 0u },  // OverflowHelper
	{ "RunOverflowSub", 0x000000D5u, 0u },  // OverflowHelper
	{ "RunOverflowAddUn", 0x000000EFu, 0u },  // OverflowUnHelper
	{ "RunOverflowMulUn", 0x000000F1u, 0u },  // OverflowUnHelper
	{ "RunOverflowSubUn", 0x000000F0u, 0u },  // OverflowUnHelper
	{ "TestConvRUn", 0x000000EAu, 1u },  // RareOpsHelper
	{ "TestInitBlk", 0x000000E8u, 1u },  // RareOpsHelper
	{ "TestStarg", 0x000000E9u, 1u },  // RareOpsHelper
	{ "RunRethrow", 0x000000EEu, 0u },  // RethrowHelper
	{ "RunSealedVirtual", 0x00000119u, 0u },  // SealedClassVirtualHelper
	{ ".ctor", 0x00000117u, 1u },  // SealedHelper
	{ "GetValueVirtual", 0x00000118u, 0u },  // SealedHelper
	{ "RunShift", 0x000000B7u, 0u },  // ShiftHelper
	{ "RunConvI", 0x00000104u, 0u },  // SimpleGapsHelper
	{ "RunConvOvfI", 0x00000105u, 1u },  // SimpleGapsHelper
	{ "RunConvOvfIUn", 0x00000107u, 1u },  // SimpleGapsHelper
	{ "RunConvOvfU", 0x00000106u, 1u },  // SimpleGapsHelper
	{ "RunConvOvfUUn", 0x00000108u, 1u },  // SimpleGapsHelper
	{ "RunConvU4", 0x00000103u, 0u },  // SimpleGapsHelper
	{ "RunLdnull", 0x00000102u, 0u },  // SimpleGapsHelper
	{ "GetSize", 0x000000AEu, 0u },  // SizeOfHelper
	{ "Subject_0", 0x00000084u, 0u },  // SnapshotProverSubjects
	{ "Subject_1", 0x00000086u, 0u },  // SnapshotProverSubjects
	{ "Subject_2", 0x00000088u, 0u },  // SnapshotProverSubjects
	{ "Subject_3", 0x0000008Au, 0u },  // SnapshotProverSubjects
	{ "Subject_4", 0x0000008Cu, 0u },  // SnapshotProverSubjects
	{ "Subject_5", 0x0000008Eu, 0u },  // SnapshotProverSubjects
	{ "Subject_6", 0x00000090u, 0u },  // SnapshotProverSubjects
	{ "Subject_7", 0x00000092u, 0u },  // SnapshotProverSubjects
	{ "CustomEntrySubject_8", 0x00000094u, 0u },  // SnapshotProverSubjects
	{ "Subject_9", 0x00000096u, 0u },  // SnapshotProverSubjects
	{ "WriteAndRead", 0x000000B4u, 0u },  // StaticFieldWriteHelper
	{ "TestAllElems", 0x000000E3u, 0u },  // StelemAllHelper
	{ "RunStindNarrow", 0x000000CAu, 0u },  // StindNarrowHelper
	{ "RunStindWide", 0x000000D1u, 0u },  // StindWideHelper
	{ "TestConcat", 0x00000112u, 0u },  // StringConcatHelper
	{ "TestFormatOne", 0x000000EBu, 0u },  // StringFormatHelper
	{ "UseString", 0x00000087u, 0u },  // StringOps
	{ "Classify", 0x0000009Eu, 0u },  // SwitchHelper
	{ "GetValue", 0x000000A0u, 0u },  // TargetHelper
	{ "CheckPositive", 0x000000D3u, 1u },  // ThrowHelper
	{ "CheckAndCast", 0x000000ADu, 0u },  // TypeCheckHelper
	{ "RunUnbox", 0x000000CCu, 0u },  // UnboxHelper
	{ "TestCkfinite", 0x000000E7u, 1u },  // UnsignedOpsHelper
	{ "TestCltUn", 0x000000E4u, 0u },  // UnsignedOpsHelper
	{ "TestDivUn", 0x000000E5u, 0u },  // UnsignedOpsHelper
	{ "TestRemUn", 0x000000E6u, 0u },  // UnsignedOpsHelper
	{ "RunValueType", 0x000000B9u, 0u },  // ValueTypeHelper
	{ "UseVirtualDispatch", 0x000000B5u, 0u },  // VirtualDispatchHelper
	{ "UseVirtual", 0x000000ABu, 0u },  // VirtualHelper
	{ "DoNothing", 0x00000089u, 0u },  // VoidCaller
	{ ".ctor", 0x00000128u, 1u },  // Wrapper<!0>
	{ "GetValue", 0x00000129u, 0u },  // Wrapper<!0>
	{ ".ctor", 0x000000A4u, 1u },  // Wrapper`1
	{ "GetValue", 0x000000A5u, 0u },  // Wrapper`1
	{ ".ctor", 0x00000126u, 1u },  // Wrapper<System.Int32>
	{ "GetValue", 0x00000127u, 0u },  // Wrapper<System.Int32>
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[105] = {
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
	{ "EhCatchFinallyNestedHelper", "", 30u, 1u },
	{ "EhFaultFinallyHelper", "", 31u, 1u },
	{ "EhFaultHelper", "", 32u, 1u },
	{ "EhFilterFinallyHelper", "", 33u, 1u },
	{ "EhFilterHelper", "", 34u, 1u },
	{ "EhFinallyHelper", "", 35u, 1u },
	{ "EhHelper", "", 36u, 1u },
	{ "EhMultipleCatchHelper", "", 37u, 1u },
	{ "EhNestedTryCatchHelper", "", 38u, 1u },
	{ "ExternalCaller", "", 39u, 1u },
	{ "FieldHelper", "", 40u, 1u },
	{ "FloatOpsHelper", "", 41u, 1u },
	{ "FlowControl", "", 42u, 1u },
	{ "GenericHelper", "", 43u, 1u },
	{ "GenericsVirtEhDemo", "", 44u, 1u },
	{ "HasFields", "", 45u, 1u },
	{ "HasInstanceFields", "", 46u, 1u },
	{ "Helper", "", 47u, 4u },
	{ "HotUpdateWithGenericsDemo", "", 51u, 1u },
	{ "HotUpdateWithTypesDemo", "", 52u, 1u },
	{ "ImplHelper", "", 53u, 2u },
	{ "IndirectHelper", "", 55u, 1u },
	{ "InstanceFieldHelper", "", 56u, 1u },
	{ "InstanceHelper", "", 57u, 2u },
	{ "InterfaceDevirtHelper", "", 59u, 1u },
	{ "InternalHelper", "", 60u, 1u },
	{ "LdVirtftnHelper", "", 61u, 1u },
	{ "LdargaHelper", "", 62u, 1u },
	{ "LdcI8Helper", "", 63u, 1u },
	{ "LdelemAllHelper", "", 64u, 1u },
	{ "LdelemaHelper", "", 65u, 1u },
	{ "LdftnHelper", "", 66u, 1u },
	{ "LdindI1Helper", "", 67u, 1u },
	{ "LdindI8Helper", "", 68u, 1u },
	{ "LdindStindGapsHelper", "", 69u, 3u },
	{ "LdindUnsignedHelper", "", 72u, 1u },
	{ "LdindWideHelper", "", 73u, 1u },
	{ "LdtokenHelper", "", 74u, 1u },
	{ "LocalAllocHelper", "", 75u, 1u },
	{ "LoopHelper", "", 76u, 1u },
	{ "MarkedClass", "", 77u, 1u },
	{ "MathHelper", "", 78u, 1u },
	{ "MoreRareOpsHelper", "", 79u, 5u },
	{ "MulDivHelper", "", 84u, 1u },
	{ "MultiDispatcher", "", 85u, 1u },
	{ "MyClass", "", 86u, 2u },
	{ "NegShiftHelper", "", 88u, 1u },
	{ "OverflowConvExtHelper", "", 89u, 11u },
	{ "OverflowConvHelper", "", 100u, 7u },
	{ "OverflowHelper", "", 107u, 3u },
	{ "OverflowUnHelper", "", 110u, 3u },
	{ "RareOpsHelper", "", 113u, 3u },
	{ "RethrowHelper", "", 116u, 1u },
	{ "SealedClassVirtualHelper", "", 117u, 1u },
	{ "SealedHelper", "", 118u, 2u },
	{ "ShiftHelper", "", 120u, 1u },
	{ "SimpleGapsHelper", "", 121u, 7u },
	{ "SizeOfHelper", "", 128u, 1u },
	{ "SnapshotProverSubjects", "", 129u, 10u },
	{ "StaticFieldWriteHelper", "", 139u, 1u },
	{ "StelemAllHelper", "", 140u, 1u },
	{ "StindNarrowHelper", "", 141u, 1u },
	{ "StindWideHelper", "", 142u, 1u },
	{ "StringConcatHelper", "", 143u, 1u },
	{ "StringFormatHelper", "", 144u, 1u },
	{ "StringOps", "", 145u, 1u },
	{ "SwitchHelper", "", 146u, 1u },
	{ "TargetHelper", "", 147u, 1u },
	{ "ThrowHelper", "", 148u, 1u },
	{ "TypeCheckHelper", "", 149u, 1u },
	{ "UnboxHelper", "", 150u, 1u },
	{ "UnsignedOpsHelper", "", 151u, 4u },
	{ "ValueTypeHelper", "", 155u, 1u },
	{ "VirtualDispatchHelper", "", 156u, 1u },
	{ "VirtualHelper", "", 157u, 1u },
	{ "VoidCaller", "", 158u, 1u },
	{ "Wrapper<!0>", "", 159u, 2u },
	{ "Wrapper`1", "", 161u, 2u },
	{ "Wrapper<System.Int32>", "Wrapper<System", 163u, 2u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[165] = {
	{ 0x00000084u, 0u },
	{ 0x00000085u, 12u },
	{ 0x00000086u, 1u },
	{ 0x00000087u, 145u },
	{ 0x00000088u, 2u },
	{ 0x00000089u, 158u },
	{ 0x0000008Au, 3u },
	{ 0x0000008Bu, 88u },
	{ 0x0000008Cu, 4u },
	{ 0x0000008Du, 52u },
	{ 0x0000008Eu, 5u },
	{ 0x0000008Fu, 67u },
	{ 0x00000090u, 6u },
	{ 0x00000091u, 68u },
	{ 0x00000092u, 7u },
	{ 0x00000093u, 27u },
	{ 0x00000094u, 8u },
	{ 0x00000095u, 19u },
	{ 0x00000096u, 9u },
	{ 0x00000097u, 50u },
	{ 0x00000098u, 86u },
	{ 0x00000099u, 32u },
	{ 0x0000009Au, 59u },
	{ 0x0000009Bu, 60u },
	{ 0x0000009Cu, 57u },
	{ 0x0000009Du, 58u },
	{ 0x0000009Eu, 146u },
	{ 0x0000009Fu, 75u },
	{ 0x000000A0u, 147u },
	{ 0x000000A1u, 13u },
	{ 0x000000A2u, 46u },
	{ 0x000000A3u, 53u },
	{ 0x000000A4u, 159u },
	{ 0x000000A5u, 160u },
	{ 0x000000A6u, 37u },
	{ 0x000000A7u, 17u },
	{ 0x000000A8u, 16u },
	{ 0x000000A9u, 39u },
	{ 0x000000AAu, 38u },
	{ 0x000000ABu, 157u },
	{ 0x000000ACu, 87u },
	{ 0x000000ADu, 149u },
	{ 0x000000AEu, 138u },
	{ 0x000000AFu, 65u },
	{ 0x000000B0u, 11u },
	{ 0x000000B1u, 23u },
	{ 0x000000B2u, 56u },
	{ 0x000000B3u, 66u },
	{ 0x000000B4u, 139u },
	{ 0x000000B5u, 156u },
	{ 0x000000B6u, 18u },
	{ 0x000000B7u, 130u },
	{ 0x000000B8u, 14u },
	{ 0x000000B9u, 155u },
	{ 0x000000BAu, 85u },
	{ 0x000000BBu, 94u },
	{ 0x000000BCu, 22u },
	{ 0x000000BDu, 51u },
	{ 0x000000BEu, 34u },
	{ 0x000000BFu, 98u },
	{ 0x000000C0u, 72u },
	{ 0x000000C1u, 33u },
	{ 0x000000C2u, 76u },
	{ 0x000000C3u, 21u },
	{ 0x000000C4u, 55u },
	{ 0x000000C5u, 10u },
	{ 0x000000C6u, 83u },
	{ 0x000000C7u, 73u },
	{ 0x000000C8u, 71u },
	{ 0x000000C9u, 36u },
	{ 0x000000CAu, 141u },
	{ 0x000000CBu, 28u },
	{ 0x000000CCu, 150u },
	{ 0x000000CDu, 35u },
	{ 0x000000CEu, 81u },
	{ 0x000000CFu, 77u },
	{ 0x000000D0u, 82u },
	{ 0x000000D1u, 142u },
	{ 0x000000D2u, 15u },
	{ 0x000000D3u, 148u },
	{ 0x000000D4u, 117u },
	{ 0x000000D5u, 119u },
	{ 0x000000D6u, 118u },
	{ 0x000000D7u, 111u },
	{ 0x000000D8u, 113u },
	{ 0x000000D9u, 110u },
	{ 0x000000DAu, 114u },
	{ 0x000000DBu, 115u },
	{ 0x000000DCu, 112u },
	{ 0x000000DDu, 116u },
	{ 0x000000DEu, 26u },
	{ 0x000000DFu, 96u },
	{ 0x000000E0u, 97u },
	{ 0x000000E1u, 84u },
	{ 0x000000E2u, 74u },
	{ 0x000000E3u, 140u },
	{ 0x000000E4u, 152u },
	{ 0x000000E5u, 153u },
	{ 0x000000E6u, 154u },
	{ 0x000000E7u, 151u },
	{ 0x000000E8u, 124u },
	{ 0x000000E9u, 125u },
	{ 0x000000EAu, 123u },
	{ 0x000000EBu, 144u },
	{ 0x000000ECu, 25u },
	{ 0x000000EDu, 24u },
	{ 0x000000EEu, 126u },
	{ 0x000000EFu, 120u },
	{ 0x000000F0u, 122u },
	{ 0x000000F1u, 121u },
	{ 0x000000F2u, 89u },
	{ 0x000000F3u, 91u },
	{ 0x000000F4u, 93u },
	{ 0x000000F5u, 92u },
	{ 0x000000F6u, 90u },
	{ 0x000000F7u, 99u },
	{ 0x000000F8u, 100u },
	{ 0x000000F9u, 105u },
	{ 0x000000FAu, 101u },
	{ 0x000000FBu, 106u },
	{ 0x000000FCu, 102u },
	{ 0x000000FDu, 107u },
	{ 0x000000FEu, 103u },
	{ 0x000000FFu, 108u },
	{ 0x00000100u, 104u },
	{ 0x00000101u, 109u },
	{ 0x00000102u, 137u },
	{ 0x00000103u, 136u },
	{ 0x00000104u, 131u },
	{ 0x00000105u, 132u },
	{ 0x00000106u, 134u },
	{ 0x00000107u, 133u },
	{ 0x00000108u, 135u },
	{ 0x00000109u, 79u },
	{ 0x0000010Au, 78u },
	{ 0x0000010Bu, 80u },
	{ 0x0000010Cu, 30u },
	{ 0x0000010Du, 29u },
	{ 0x0000010Eu, 31u },
	{ 0x0000010Fu, 49u },
	{ 0x00000110u, 95u },
	{ 0x00000111u, 70u },
	{ 0x00000112u, 143u },
	{ 0x00000113u, 54u },
	{ 0x00000114u, 20u },
	{ 0x00000115u, 62u },
	{ 0x00000116u, 61u },
	{ 0x00000117u, 128u },
	{ 0x00000118u, 129u },
	{ 0x00000119u, 127u },
	{ 0x0000011Bu, 63u },
	{ 0x0000011Cu, 64u },
	{ 0x0000011Du, 69u },
	{ 0x0000011Eu, 45u },
	{ 0x0000011Fu, 42u },
	{ 0x00000120u, 44u },
	{ 0x00000121u, 47u },
	{ 0x00000122u, 40u },
	{ 0x00000123u, 43u },
	{ 0x00000124u, 48u },
	{ 0x00000125u, 41u },
	{ 0x00000126u, 163u },
	{ 0x00000127u, 164u },
	{ 0x00000128u, 161u },
	{ 0x00000129u, 162u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[165] = {
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // SnapshotProverSubjects::Subject_0
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // SnapshotProverSubjects::Subject_1
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // SnapshotProverSubjects::Subject_2
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // SnapshotProverSubjects::Subject_3
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // SnapshotProverSubjects::Subject_4
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // SnapshotProverSubjects::Subject_5
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // SnapshotProverSubjects::Subject_6
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // SnapshotProverSubjects::Subject_7
	{ reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_CustomEntrySubject_8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // SnapshotProverSubjects::CustomEntrySubject_8
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
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_EhCatchFinallyNestedHelper_CatchFinallyNested_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // EhCatchFinallyNestedHelper::CatchFinallyNested
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_EhFaultFinallyHelper_FaultFinally_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // EhFaultFinallyHelper::FaultFinally
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_EhFaultHelper_TryFault_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // EhFaultHelper::TryFault
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_EhFilterFinallyHelper_FilterFinally_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // EhFilterFinallyHelper::FilterFinally
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_EhFilterHelper_TryFilter_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // EhFilterHelper::TryFilter
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_EhFinallyHelper_TryFinally_Only_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // EhFinallyHelper::TryFinally_Only
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_EhHelper_SafeDivide), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // EhHelper::SafeDivide
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_EhMultipleCatchHelper_MultipleCatch_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // EhMultipleCatchHelper::MultipleCatch
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_EhNestedTryCatchHelper_NestedTryCatch_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // EhNestedTryCatchHelper::NestedTryCatch
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
	105u,
	s_hotpatch_methods,
	165u,
	s_hotpatch_slots,
	165u,
	s_hotpatch_entries,
	165u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[53] = {
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
	"System.Private.CoreLib/System.Exception",
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
	"SnapshotTestFixtures/MyValueType",
	"SnapshotTestFixtures/MyValueType::X",
	"SnapshotTestFixtures/MyValueType::Y",
	"SnapshotTestFixtures/Wrapper<!0>::_val",
	"SnapshotTestFixtures/Wrapper<System.Int32>::_val",
};

extern "C" void* kChaosExternalRuntimeFnTable[53] = {
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
	nullptr,
	nullptr,
	nullptr,
};

extern "C" int32_t kChaosExternalRuntimeCount = 53;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[165])() = {
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_0),
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_1),
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_2),
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_3),
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_4),
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_5),
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_6),
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_7),
	reinterpret_cast<void(*)()>(&SnapshotProverSubjects_SnapshotProverSubjects_CustomEntrySubject_8),
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
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_EhCatchFinallyNestedHelper_CatchFinallyNested_System_Int32),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_EhFaultFinallyHelper_FaultFinally_System_Int32),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_EhFaultHelper_TryFault_System_Int32),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_EhFilterFinallyHelper_FilterFinally_System_Int32),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_EhFilterHelper_TryFilter_System_Int32),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_EhFinallyHelper_TryFinally_Only_System_Int32),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_EhHelper_SafeDivide),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_EhMultipleCatchHelper_MultipleCatch_System_Int32),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_EhNestedTryCatchHelper_NestedTryCatch_System_Int32),
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
extern "C" void (*kBenchmarkWrappers[165])() = {
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
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[40])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[41])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[42])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[43])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[44])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[45])(0);},
	[]() {kAotMethods[46]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[47])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[48])(0);},
	[]() {kAotMethods[49]();},
	[]() {kAotMethods[50]();},
	[]() {kAotMethods[51]();},
	[]() {kAotMethods[52]();},
	[]() {kAotMethods[53]();},
	[]() {kAotMethods[54]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[55])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[56])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[57])(chaos_make_string_id_value(628919584683901915ULL));},
	[]() {kAotMethods[58]();},
	[]() {kAotMethods[59]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[60])(0);},
	[]() {kAotMethods[61]();},
	[]() {kAotMethods[62]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[63])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[64])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {kAotMethods[65]();},
	[]() {kAotMethods[66]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[67])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[68])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {kAotMethods[69]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[70])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[71])(0);},
	[]() {kAotMethods[72]();},
	[]() {kAotMethods[73]();},
	[]() {kAotMethods[74]();},
	[]() {kAotMethods[75]();},
	[]() {kAotMethods[76]();},
	[]() {kAotMethods[77]();},
	[]() {kAotMethods[78]();},
	[]() {kAotMethods[79]();},
	[]() {kAotMethods[80]();},
	[]() {kAotMethods[81]();},
	[]() {kAotMethods[82]();},
	[]() {kAotMethods[83]();},
	[]() {kAotMethods[84]();},
	[]() {kAotMethods[85]();},
	[]() {kAotMethods[86]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[87])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {kAotMethods[88]();},
	[]() {kAotMethods[89]();},
	[]() {kAotMethods[90]();},
	[]() {kAotMethods[91]();},
	[]() {kAotMethods[92]();},
	[]() {kAotMethods[93]();},
	[]() {kAotMethods[94]();},
	[]() {kAotMethods[95]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[96])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[97])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {kAotMethods[98]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[99])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[100])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[101])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[102])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[103])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[104])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[105])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[106])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[107])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[108])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[109])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[110])(0);},
	[]() {kAotMethods[111]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[112])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[113])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[114])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[115])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[116])(0);},
	[]() {kAotMethods[117]();},
	[]() {kAotMethods[118]();},
	[]() {kAotMethods[119]();},
	[]() {kAotMethods[120]();},
	[]() {kAotMethods[121]();},
	[]() {kAotMethods[122]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[123])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[124])(([](){ static CHAOS_IL2CPP_UINT8 __b[sizeof(CHAOS_IL2CPP_INTPTR)] = {}; return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(__b); })());},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[125])(0);},
	[]() {kAotMethods[126]();},
	[]() {kAotMethods[127]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[128])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[129])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {kAotMethods[130]();},
	[]() {kAotMethods[131]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[132])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[133])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[134])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[135])(0);},
	[]() {kAotMethods[136]();},
	[]() {kAotMethods[137]();},
	[]() {kAotMethods[138]();},
	[]() {kAotMethods[139]();},
	[]() {kAotMethods[140]();},
	[]() {kAotMethods[141]();},
	[]() {kAotMethods[142]();},
	[]() {kAotMethods[143]();},
	[]() {kAotMethods[144]();},
	[]() {kAotMethods[145]();},
	[]() {kAotMethods[146]();},
	[]() {kAotMethods[147]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[148])(0);},
	[]() {kAotMethods[149]();},
	[]() {kAotMethods[150]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[151])(0);},
	[]() {kAotMethods[152]();},
	[]() {kAotMethods[153]();},
	[]() {kAotMethods[154]();},
	[]() {kAotMethods[155]();},
	[]() {kAotMethods[156]();},
	[]() {kAotMethods[157]();},
	[]() {kAotMethods[158]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[159])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[160])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[161])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[162])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[163])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[164])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
};

// ── Subject entry index mapping ─────────────────────────────────
// Maps subject index (0-based sequential) to kAotMethod index.
// Used by runtime-entry.cpp to route --benchmark N to the correct
// AOT method slot, since kAotMethods[] includes lambdas/closures
// that shift subject methods to non-contiguous indices.
extern "C" const int kSubjectEntryCount = 10;
extern "C" const int kSubjectEntryIndices[10] = {
	0,
	1,
	2,
	3,
	4,
	5,
	6,
	7,
	8,
	9,
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

	/* ── Entry 0: SnapshotProverSubjects_SnapshotProverSubjects_Subject_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry0;
	/* ── Entry 1: SnapshotProverSubjects_SnapshotProverSubjects_Subject_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry1;
	/* ── Entry 2: SnapshotProverSubjects_SnapshotProverSubjects_Subject_2 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry2;
	/* ── Entry 3: SnapshotProverSubjects_SnapshotProverSubjects_Subject_3 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry3;
	/* ── Entry 4: SnapshotProverSubjects_SnapshotProverSubjects_Subject_4 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry4;
	/* ── Entry 5: SnapshotProverSubjects_SnapshotProverSubjects_Subject_5 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry5;
	/* ── Entry 6: SnapshotProverSubjects_SnapshotProverSubjects_Subject_6 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry6;
	/* ── Entry 7: SnapshotProverSubjects_SnapshotProverSubjects_Subject_7 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry7;
	/* ── Entry 8: SnapshotProverSubjects_SnapshotProverSubjects_Subject_9 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry8;
	/* ── Entry 9: SnapshotTestFixtures_AddressHelper_RunAddress ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry9;
	/* ── Entry 10: SnapshotTestFixtures_ArithmeticCompareHelper_RunCompare ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[11];
	} entry10;
	/* ── Entry 11: SnapshotTestFixtures_ArithmeticOps_RunAdd ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry11;
	/* ── Entry 12: SnapshotTestFixtures_ArrayHelper_MakeAndFill ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry12;
	/* ── Entry 13: SnapshotTestFixtures_ArrayLengthHelper_RunLength ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry13;
	/* ── Entry 14: SnapshotTestFixtures_ArrayRefHelper_RunArrayRef ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry14;
	/* ── Entry 15: SnapshotTestFixtures_BaseClass__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry15;
	/* ── Entry 16: SnapshotTestFixtures_BaseClass_Compute ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry16;
	/* ── Entry 17: SnapshotTestFixtures_BitwiseHelper_RunBitwise ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[10];
	} entry17;
	/* ── Entry 18: SnapshotTestFixtures_BoxingHelper_BoxAndUnbox ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry18;
	/* ── Entry 19: SnapshotTestFixtures_BoxInterfaceArrayDemo_DemoBoxStore ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry19;
	/* ── Entry 20: SnapshotTestFixtures_BranchCompareBHelper_RunBranchCompareB ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[12];
	} entry20;
	/* ── Entry 21: SnapshotTestFixtures_BranchCompareHelper_RunBranchCompare ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[11];
	} entry21;
	/* ── Entry 22: SnapshotTestFixtures_BranchDupHelper_RunBranchDup ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[9];
	} entry22;
	/* ── Entry 23: SnapshotTestFixtures_BranchUnsignedBHelper_RunBranchUnsignedB ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry23;
	/* ── Entry 24: SnapshotTestFixtures_BranchUnsignedHelper_RunBranchUnsigned ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry24;
	/* ── Entry 25: SnapshotTestFixtures_CalliHelper_RunCalli_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry25;
	/* ── Entry 26: SnapshotTestFixtures_CallVirtHelper_CreateAndUse ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry26;
	/* ── Entry 27: SnapshotTestFixtures_CallvirtHelper_RunCallvirt ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry27;
	/* ── Entry 28: SnapshotTestFixtures_CollectionsHelper_TestDict ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[8];
	} entry28;
	/* ── Entry 29: SnapshotTestFixtures_CollectionsHelper_TestList ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry29;
	/* ── Entry 30: SnapshotTestFixtures_CollectionsHelper_TestSet ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry30;
	/* ── Entry 31: SnapshotTestFixtures_ConversionHelper_ConvertToInt ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry31;
	/* ── Entry 32: SnapshotTestFixtures_ConvSmallIntHelper_RunConvSmall ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[12];
	} entry32;
	/* ── Entry 33: SnapshotTestFixtures_ConvWideHelper_RunConvWide ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[9];
	} entry33;
	/* ── Entry 34: SnapshotTestFixtures_CpblkHelper_RunCpblk ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry34;
	/* ── Entry 35: SnapshotTestFixtures_CpobjHelper_RunCpobj ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry35;
	/* ── Entry 36: SnapshotTestFixtures_DelegateHelper_RunDelegate ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry36;
	/* ── Entry 37: SnapshotTestFixtures_DerivedClass__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry37;
	/* ── Entry 38: SnapshotTestFixtures_DerivedClass_Compute ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry38;
	/* ── Entry 39: SnapshotTestFixtures_EhCatchFinallyNestedHelper_CatchFinallyNested_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry39;
	/* ── Entry 40: SnapshotTestFixtures_EhFaultFinallyHelper_FaultFinally_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry40;
	/* ── Entry 41: SnapshotTestFixtures_EhFaultHelper_TryFault_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry41;
	/* ── Entry 42: SnapshotTestFixtures_EhFilterFinallyHelper_FilterFinally_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry42;
	/* ── Entry 43: SnapshotTestFixtures_EhFilterHelper_TryFilter_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry43;
	/* ── Entry 44: SnapshotTestFixtures_EhFinallyHelper_TryFinally_Only_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry44;
	/* ── Entry 45: SnapshotTestFixtures_EhHelper_SafeDivide ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry45;
	/* ── Entry 46: SnapshotTestFixtures_EhMultipleCatchHelper_MultipleCatch_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry46;
	/* ── Entry 47: SnapshotTestFixtures_EhNestedTryCatchHelper_NestedTryCatch_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry47;
	/* ── Entry 48: SnapshotTestFixtures_ExternalCaller_CallExternal ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry48;
	/* ── Entry 49: SnapshotTestFixtures_FieldHelper_GetAndIncrement ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry49;
	/* ── Entry 50: SnapshotTestFixtures_FloatOpsHelper_RunFloatOps ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[9];
	} entry50;
	/* ── Entry 51: SnapshotTestFixtures_FlowControl_IsPositive ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry51;
	/* ── Entry 52: SnapshotTestFixtures_GenericHelper_UseGeneric ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry52;
	/* ── Entry 53: SnapshotTestFixtures_GenericsVirtEhDemo_DemoCombine ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry53;
	/* ── Entry 54: SnapshotTestFixtures_HasFields__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry54;
	/* ── Entry 55: SnapshotTestFixtures_HasInstanceFields__ctor_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry55;
	/* ── Entry 56: SnapshotTestFixtures_Helper_ConsumeString_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry56;
	/* ── Entry 57: SnapshotTestFixtures_Helper_GetValue ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry57;
	/* ── Entry 58: SnapshotTestFixtures_Helper_Square_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry58;
	/* ── Entry 59: SnapshotTestFixtures_HotUpdateWithGenericsDemo_Run ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry59;
	/* ── Entry 60: SnapshotTestFixtures_HotUpdateWithTypesDemo_Run ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry60;
	/* ── Entry 61: SnapshotTestFixtures_ImplHelper__ctor_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry61;
	/* ── Entry 62: SnapshotTestFixtures_ImplHelper_GetValue ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry62;
	/* ── Entry 63: SnapshotTestFixtures_IndirectHelper_ReadWriteRef ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry63;
	/* ── Entry 64: SnapshotTestFixtures_InstanceFieldHelper_CreateAndUseFields ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry64;
	/* ── Entry 65: SnapshotTestFixtures_InstanceHelper__ctor_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry65;
	/* ── Entry 66: SnapshotTestFixtures_InstanceHelper_GetValue ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry66;
	/* ── Entry 67: SnapshotTestFixtures_InterfaceDevirtHelper_RunInterfaceTest ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry67;
	/* ── Entry 68: SnapshotTestFixtures_InternalHelper_Double_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry68;
	/* ── Entry 69: SnapshotTestFixtures_LdargaHelper_RunLdarga_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry69;
	/* ── Entry 70: SnapshotTestFixtures_LdcI8Helper_RunLdcI8 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry70;
	/* ── Entry 71: SnapshotTestFixtures_LdelemaHelper_RunLdelema ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry71;
	/* ── Entry 72: SnapshotTestFixtures_LdelemAllHelper_TestAllElems ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry72;
	/* ── Entry 73: SnapshotTestFixtures_LdftnHelper_GetFnPtr ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry73;
	/* ── Entry 74: SnapshotTestFixtures_LdindI1Helper_RunLdindI1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry74;
	/* ── Entry 75: SnapshotTestFixtures_LdindI8Helper_RunLdindI8 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry75;
	/* ── Entry 76: SnapshotTestFixtures_LdindStindGapsHelper_RunLdindI ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry76;
	/* ── Entry 77: SnapshotTestFixtures_LdindStindGapsHelper_RunLdindI4 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry77;
	/* ── Entry 78: SnapshotTestFixtures_LdindStindGapsHelper_RunStindI ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry78;
	/* ── Entry 79: SnapshotTestFixtures_LdindUnsignedHelper_RunLdindUnsigned ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[11];
	} entry79;
	/* ── Entry 80: SnapshotTestFixtures_LdindWideHelper_RunLdindWide ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry80;
	/* ── Entry 81: SnapshotTestFixtures_LdtokenHelper_RunLdtoken ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry81;
	/* ── Entry 82: SnapshotTestFixtures_LdVirtftnHelper_RunLdVirtftn ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry82;
	/* ── Entry 83: SnapshotTestFixtures_LocalAllocHelper_RunAlloc ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry83;
	/* ── Entry 84: SnapshotTestFixtures_LoopHelper_SumToFive ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry84;
	/* ── Entry 85: SnapshotTestFixtures_MarkedClass__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry85;
	/* ── Entry 86: SnapshotTestFixtures_MathHelper_RunSquare ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry86;
	/* ── Entry 87: SnapshotTestFixtures_MoreRareOpsHelper_RunArglist ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry87;
	/* ── Entry 88: SnapshotTestFixtures_MoreRareOpsHelper_RunJmp ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry88;
	/* ── Entry 89: SnapshotTestFixtures_MoreRareOpsHelper_RunMkrefany ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry89;
	/* ── Entry 90: SnapshotTestFixtures_MoreRareOpsHelper_RunRefanytype ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry90;
	/* ── Entry 91: SnapshotTestFixtures_MoreRareOpsHelper_RunRefanyval ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry91;
	/* ── Entry 92: SnapshotTestFixtures_MulDivHelper_RunMulDiv ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry92;
	/* ── Entry 93: SnapshotTestFixtures_MultiDispatcher_DispatchBoth ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry93;
	/* ── Entry 94: SnapshotTestFixtures_MyClass__ctor_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry94;
	/* ── Entry 95: SnapshotTestFixtures_MyClass_GetValue ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry95;
	/* ── Entry 96: SnapshotTestFixtures_NegShiftHelper_RunNegShift ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[8];
	} entry96;
	/* ── Entry 97: SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfI1_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry97;
	/* ── Entry 98: SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfI1Un_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry98;
	/* ── Entry 99: SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfI2Un_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry99;
	/* ── Entry 100: SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfI4Un_System_Int64 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry100;
	/* ── Entry 101: SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfI8Un_System_Single ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry101;
	/* ── Entry 102: SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfIUn_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry102;
	/* ── Entry 103: SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfU1Un_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry103;
	/* ── Entry 104: SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfU2Un_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry104;
	/* ── Entry 105: SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfU4Un_System_Int64 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry105;
	/* ── Entry 106: SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfU8Un_System_Double ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry106;
	/* ── Entry 107: SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfUUn_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry107;
	/* ── Entry 108: SnapshotTestFixtures_OverflowConvHelper_ConvOvfI2_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry108;
	/* ── Entry 109: SnapshotTestFixtures_OverflowConvHelper_ConvOvfI4 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry109;
	/* ── Entry 110: SnapshotTestFixtures_OverflowConvHelper_ConvOvfI8_System_Single ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry110;
	/* ── Entry 111: SnapshotTestFixtures_OverflowConvHelper_ConvOvfU1_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry111;
	/* ── Entry 112: SnapshotTestFixtures_OverflowConvHelper_ConvOvfU2_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry112;
	/* ── Entry 113: SnapshotTestFixtures_OverflowConvHelper_ConvOvfU4_System_Int64 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry113;
	/* ── Entry 114: SnapshotTestFixtures_OverflowConvHelper_ConvOvfU8_System_Double ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry114;
	/* ── Entry 115: SnapshotTestFixtures_OverflowHelper_RunOverflowAdd ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry115;
	/* ── Entry 116: SnapshotTestFixtures_OverflowHelper_RunOverflowMul ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry116;
	/* ── Entry 117: SnapshotTestFixtures_OverflowHelper_RunOverflowSub ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry117;
	/* ── Entry 118: SnapshotTestFixtures_OverflowUnHelper_RunOverflowAddUn ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry118;
	/* ── Entry 119: SnapshotTestFixtures_OverflowUnHelper_RunOverflowMulUn ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry119;
	/* ── Entry 120: SnapshotTestFixtures_OverflowUnHelper_RunOverflowSubUn ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry120;
	/* ── Entry 121: SnapshotTestFixtures_RareOpsHelper_TestConvRUn_System_Single ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[8];
	} entry121;
	/* ── Entry 122: SnapshotTestFixtures_RareOpsHelper_TestStarg_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry122;
	/* ── Entry 123: SnapshotTestFixtures_RethrowHelper_RunRethrow ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry123;
	/* ── Entry 124: SnapshotTestFixtures_SealedClassVirtualHelper_RunSealedVirtual ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry124;
	/* ── Entry 125: SnapshotTestFixtures_SealedHelper__ctor_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry125;
	/* ── Entry 126: SnapshotTestFixtures_SealedHelper_GetValueVirtual ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry126;
	/* ── Entry 127: SnapshotTestFixtures_ShiftHelper_RunShift ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry127;
	/* ── Entry 128: SnapshotTestFixtures_SimpleGapsHelper_RunConvI ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry128;
	/* ── Entry 129: SnapshotTestFixtures_SimpleGapsHelper_RunConvOvfI_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry129;
	/* ── Entry 130: SnapshotTestFixtures_SimpleGapsHelper_RunConvOvfIUn_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry130;
	/* ── Entry 131: SnapshotTestFixtures_SimpleGapsHelper_RunConvOvfU_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry131;
	/* ── Entry 132: SnapshotTestFixtures_SimpleGapsHelper_RunConvOvfUUn_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry132;
	/* ── Entry 133: SnapshotTestFixtures_SimpleGapsHelper_RunConvU4 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry133;
	/* ── Entry 134: SnapshotTestFixtures_SimpleGapsHelper_RunLdnull ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry134;
	/* ── Entry 135: SnapshotTestFixtures_SizeOfHelper_GetSize ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry135;
	/* ── Entry 136: SnapshotTestFixtures_StaticFieldWriteHelper_WriteAndRead ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry136;
	/* ── Entry 137: SnapshotTestFixtures_StelemAllHelper_TestAllElems ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry137;
	/* ── Entry 138: SnapshotTestFixtures_StindNarrowHelper_RunStindNarrow ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[8];
	} entry138;
	/* ── Entry 139: SnapshotTestFixtures_StindWideHelper_RunStindWide ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[12];
	} entry139;
	/* ── Entry 140: SnapshotTestFixtures_StringConcatHelper_TestConcat ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry140;
	/* ── Entry 141: SnapshotTestFixtures_StringFormatHelper_TestFormatOne ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry141;
	/* ── Entry 142: SnapshotTestFixtures_StringOps_UseString ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry142;
	/* ── Entry 143: SnapshotTestFixtures_SwitchHelper_Classify ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry143;
	/* ── Entry 144: SnapshotTestFixtures_TargetHelper_GetValue ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry144;
	/* ── Entry 145: SnapshotTestFixtures_ThrowHelper_CheckPositive_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry145;
	/* ── Entry 146: SnapshotTestFixtures_TypeCheckHelper_CheckAndCast ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry146;
	/* ── Entry 147: SnapshotTestFixtures_UnboxHelper_RunUnbox ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry147;
	/* ── Entry 148: SnapshotTestFixtures_UnsignedOpsHelper_TestCkfinite_System_Double ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry148;
	/* ── Entry 149: SnapshotTestFixtures_UnsignedOpsHelper_TestCltUn ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry149;
	/* ── Entry 150: SnapshotTestFixtures_UnsignedOpsHelper_TestDivUn ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry150;
	/* ── Entry 151: SnapshotTestFixtures_UnsignedOpsHelper_TestRemUn ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry151;
	/* ── Entry 152: SnapshotTestFixtures_ValueTypeHelper_RunValueType ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry152;
	/* ── Entry 153: SnapshotTestFixtures_VirtualDispatchHelper_UseVirtualDispatch ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry153;
	/* ── Entry 154: SnapshotTestFixtures_VirtualHelper_UseVirtual ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry154;
	/* ── Entry 155: SnapshotTestFixtures_VoidCaller_DoNothing ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry155;
	/* ── Entry 156: SnapshotTestFixtures_Wrapper_1__ctor_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry156;
	/* ── Entry 157: SnapshotTestFixtures_Wrapper_1_GetValue ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry157;
	/* ── Entry 158: SnapshotTestFixtures_Wrapper__0__ctor_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry158;
	/* ── Entry 159: SnapshotTestFixtures_Wrapper__0_GetValue ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry159;
	/* ── Entry 160: SnapshotTestFixtures_Wrapper_System_Int32__ctor_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry160;
	/* ── Entry 161: SnapshotTestFixtures_Wrapper_System_Int32_GetValue ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry161;
} kChaosGcSlotMapsSection = {
	/* entry0 = SnapshotProverSubjects_SnapshotProverSubjects_Subject_0 */
	.entry0 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_0),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry1 = SnapshotProverSubjects_SnapshotProverSubjects_Subject_1 */
	.entry1 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_1),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry2 = SnapshotProverSubjects_SnapshotProverSubjects_Subject_2 */
	.entry2 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_2),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry3 = SnapshotProverSubjects_SnapshotProverSubjects_Subject_3 */
	.entry3 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_3),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry4 = SnapshotProverSubjects_SnapshotProverSubjects_Subject_4 */
	.entry4 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_4),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry5 = SnapshotProverSubjects_SnapshotProverSubjects_Subject_5 */
	.entry5 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_5),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry6 = SnapshotProverSubjects_SnapshotProverSubjects_Subject_6 */
	.entry6 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_6),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry7 = SnapshotProverSubjects_SnapshotProverSubjects_Subject_7 */
	.entry7 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_7),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry8 = SnapshotProverSubjects_SnapshotProverSubjects_Subject_9 */
	.entry8 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_9),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry9 = SnapshotTestFixtures_AddressHelper_RunAddress */
	.entry9 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_AddressHelper_RunAddress),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry10 = SnapshotTestFixtures_ArithmeticCompareHelper_RunCompare */
	.entry10 = {
		/* entry_total_size = 64 */ 64u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_ArithmeticCompareHelper_RunCompare),
		/* frame_size = 88 */ 88u,
		/* num_gc_slots = 11 */ 11u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u, 72u, 80u }
	},
	/* entry11 = SnapshotTestFixtures_ArithmeticOps_RunAdd */
	.entry11 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_ArithmeticOps_RunAdd),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry12 = SnapshotTestFixtures_ArrayHelper_MakeAndFill */
	.entry12 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_ArrayHelper_MakeAndFill),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry13 = SnapshotTestFixtures_ArrayLengthHelper_RunLength */
	.entry13 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_ArrayLengthHelper_RunLength),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry14 = SnapshotTestFixtures_ArrayRefHelper_RunArrayRef */
	.entry14 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_ArrayRefHelper_RunArrayRef),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry15 = SnapshotTestFixtures_BaseClass__ctor */
	.entry15 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_BaseClass__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry16 = SnapshotTestFixtures_BaseClass_Compute */
	.entry16 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_BaseClass_Compute),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry17 = SnapshotTestFixtures_BitwiseHelper_RunBitwise */
	.entry17 = {
		/* entry_total_size = 60 */ 60u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_BitwiseHelper_RunBitwise),
		/* frame_size = 80 */ 80u,
		/* num_gc_slots = 10 */ 10u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u, 72u }
	},
	/* entry18 = SnapshotTestFixtures_BoxingHelper_BoxAndUnbox */
	.entry18 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_BoxingHelper_BoxAndUnbox),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry19 = SnapshotTestFixtures_BoxInterfaceArrayDemo_DemoBoxStore */
	.entry19 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_BoxInterfaceArrayDemo_DemoBoxStore),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry20 = SnapshotTestFixtures_BranchCompareBHelper_RunBranchCompareB */
	.entry20 = {
		/* entry_total_size = 68 */ 68u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_BranchCompareBHelper_RunBranchCompareB),
		/* frame_size = 96 */ 96u,
		/* num_gc_slots = 12 */ 12u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u, 72u, 80u, 88u }
	},
	/* entry21 = SnapshotTestFixtures_BranchCompareHelper_RunBranchCompare */
	.entry21 = {
		/* entry_total_size = 64 */ 64u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_BranchCompareHelper_RunBranchCompare),
		/* frame_size = 88 */ 88u,
		/* num_gc_slots = 11 */ 11u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u, 72u, 80u }
	},
	/* entry22 = SnapshotTestFixtures_BranchDupHelper_RunBranchDup */
	.entry22 = {
		/* entry_total_size = 56 */ 56u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_BranchDupHelper_RunBranchDup),
		/* frame_size = 72 */ 72u,
		/* num_gc_slots = 9 */ 9u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u }
	},
	/* entry23 = SnapshotTestFixtures_BranchUnsignedBHelper_RunBranchUnsignedB */
	.entry23 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_BranchUnsignedBHelper_RunBranchUnsignedB),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry24 = SnapshotTestFixtures_BranchUnsignedHelper_RunBranchUnsigned */
	.entry24 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_BranchUnsignedHelper_RunBranchUnsigned),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry25 = SnapshotTestFixtures_CalliHelper_RunCalli_System_Int32 */
	.entry25 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_CalliHelper_RunCalli_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 8u, 16u, 24u, 32u }
	},
	/* entry26 = SnapshotTestFixtures_CallVirtHelper_CreateAndUse */
	.entry26 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_CallVirtHelper_CreateAndUse),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry27 = SnapshotTestFixtures_CallvirtHelper_RunCallvirt */
	.entry27 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_CallvirtHelper_RunCallvirt),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry28 = SnapshotTestFixtures_CollectionsHelper_TestDict */
	.entry28 = {
		/* entry_total_size = 52 */ 52u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_CollectionsHelper_TestDict),
		/* frame_size = 64 */ 64u,
		/* num_gc_slots = 8 */ 8u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u }
	},
	/* entry29 = SnapshotTestFixtures_CollectionsHelper_TestList */
	.entry29 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_CollectionsHelper_TestList),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry30 = SnapshotTestFixtures_CollectionsHelper_TestSet */
	.entry30 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_CollectionsHelper_TestSet),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry31 = SnapshotTestFixtures_ConversionHelper_ConvertToInt */
	.entry31 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_ConversionHelper_ConvertToInt),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry32 = SnapshotTestFixtures_ConvSmallIntHelper_RunConvSmall */
	.entry32 = {
		/* entry_total_size = 68 */ 68u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_ConvSmallIntHelper_RunConvSmall),
		/* frame_size = 96 */ 96u,
		/* num_gc_slots = 12 */ 12u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u, 72u, 80u, 88u }
	},
	/* entry33 = SnapshotTestFixtures_ConvWideHelper_RunConvWide */
	.entry33 = {
		/* entry_total_size = 56 */ 56u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_ConvWideHelper_RunConvWide),
		/* frame_size = 72 */ 72u,
		/* num_gc_slots = 9 */ 9u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u }
	},
	/* entry34 = SnapshotTestFixtures_CpblkHelper_RunCpblk */
	.entry34 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_CpblkHelper_RunCpblk),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry35 = SnapshotTestFixtures_CpobjHelper_RunCpobj */
	.entry35 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_CpobjHelper_RunCpobj),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry36 = SnapshotTestFixtures_DelegateHelper_RunDelegate */
	.entry36 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_DelegateHelper_RunDelegate),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry37 = SnapshotTestFixtures_DerivedClass__ctor */
	.entry37 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_DerivedClass__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry38 = SnapshotTestFixtures_DerivedClass_Compute */
	.entry38 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_DerivedClass_Compute),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry39 = SnapshotTestFixtures_EhCatchFinallyNestedHelper_CatchFinallyNested_System_Int32 */
	.entry39 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_EhCatchFinallyNestedHelper_CatchFinallyNested_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 8u, 16u, 24u, 32u }
	},
	/* entry40 = SnapshotTestFixtures_EhFaultFinallyHelper_FaultFinally_System_Int32 */
	.entry40 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_EhFaultFinallyHelper_FaultFinally_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 8u, 16u, 24u, 32u }
	},
	/* entry41 = SnapshotTestFixtures_EhFaultHelper_TryFault_System_Int32 */
	.entry41 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_EhFaultHelper_TryFault_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 8u, 16u, 24u, 32u }
	},
	/* entry42 = SnapshotTestFixtures_EhFilterFinallyHelper_FilterFinally_System_Int32 */
	.entry42 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_EhFilterFinallyHelper_FilterFinally_System_Int32),
		/* frame_size = 64 */ 64u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 8u, 16u, 24u, 32u, 40u, 48u, 56u }
	},
	/* entry43 = SnapshotTestFixtures_EhFilterHelper_TryFilter_System_Int32 */
	.entry43 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_EhFilterHelper_TryFilter_System_Int32),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry44 = SnapshotTestFixtures_EhFinallyHelper_TryFinally_Only_System_Int32 */
	.entry44 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_EhFinallyHelper_TryFinally_Only_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 8u, 16u, 24u, 32u }
	},
	/* entry45 = SnapshotTestFixtures_EhHelper_SafeDivide */
	.entry45 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_EhHelper_SafeDivide),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry46 = SnapshotTestFixtures_EhMultipleCatchHelper_MultipleCatch_System_Int32 */
	.entry46 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_EhMultipleCatchHelper_MultipleCatch_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 8u, 16u, 24u }
	},
	/* entry47 = SnapshotTestFixtures_EhNestedTryCatchHelper_NestedTryCatch_System_Int32 */
	.entry47 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_EhNestedTryCatchHelper_NestedTryCatch_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 8u, 16u, 24u }
	},
	/* entry48 = SnapshotTestFixtures_ExternalCaller_CallExternal */
	.entry48 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_ExternalCaller_CallExternal),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry49 = SnapshotTestFixtures_FieldHelper_GetAndIncrement */
	.entry49 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_FieldHelper_GetAndIncrement),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry50 = SnapshotTestFixtures_FloatOpsHelper_RunFloatOps */
	.entry50 = {
		/* entry_total_size = 56 */ 56u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_FloatOpsHelper_RunFloatOps),
		/* frame_size = 72 */ 72u,
		/* num_gc_slots = 9 */ 9u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u }
	},
	/* entry51 = SnapshotTestFixtures_FlowControl_IsPositive */
	.entry51 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_FlowControl_IsPositive),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry52 = SnapshotTestFixtures_GenericHelper_UseGeneric */
	.entry52 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_GenericHelper_UseGeneric),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry53 = SnapshotTestFixtures_GenericsVirtEhDemo_DemoCombine */
	.entry53 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_GenericsVirtEhDemo_DemoCombine),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry54 = SnapshotTestFixtures_HasFields__ctor */
	.entry54 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_HasFields__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry55 = SnapshotTestFixtures_HasInstanceFields__ctor_System_Int32 */
	.entry55 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_HasInstanceFields__ctor_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 16u, 24u }
	},
	/* entry56 = SnapshotTestFixtures_Helper_ConsumeString_System_String */
	.entry56 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_Helper_ConsumeString_System_String),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry57 = SnapshotTestFixtures_Helper_GetValue */
	.entry57 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_Helper_GetValue),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry58 = SnapshotTestFixtures_Helper_Square_System_Int32 */
	.entry58 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_Helper_Square_System_Int32),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 8u, 16u }
	},
	/* entry59 = SnapshotTestFixtures_HotUpdateWithGenericsDemo_Run */
	.entry59 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_HotUpdateWithGenericsDemo_Run),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry60 = SnapshotTestFixtures_HotUpdateWithTypesDemo_Run */
	.entry60 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_HotUpdateWithTypesDemo_Run),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry61 = SnapshotTestFixtures_ImplHelper__ctor_System_Int32 */
	.entry61 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_ImplHelper__ctor_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 16u, 24u }
	},
	/* entry62 = SnapshotTestFixtures_ImplHelper_GetValue */
	.entry62 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_ImplHelper_GetValue),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry63 = SnapshotTestFixtures_IndirectHelper_ReadWriteRef */
	.entry63 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_IndirectHelper_ReadWriteRef),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry64 = SnapshotTestFixtures_InstanceFieldHelper_CreateAndUseFields */
	.entry64 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_InstanceFieldHelper_CreateAndUseFields),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry65 = SnapshotTestFixtures_InstanceHelper__ctor_System_Int32 */
	.entry65 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_InstanceHelper__ctor_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 16u, 24u }
	},
	/* entry66 = SnapshotTestFixtures_InstanceHelper_GetValue */
	.entry66 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_InstanceHelper_GetValue),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry67 = SnapshotTestFixtures_InterfaceDevirtHelper_RunInterfaceTest */
	.entry67 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_InterfaceDevirtHelper_RunInterfaceTest),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry68 = SnapshotTestFixtures_InternalHelper_Double_System_Int32 */
	.entry68 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_InternalHelper_Double_System_Int32),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 8u, 16u }
	},
	/* entry69 = SnapshotTestFixtures_LdargaHelper_RunLdarga_System_Int32 */
	.entry69 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_LdargaHelper_RunLdarga_System_Int32),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 8u }
	},
	/* entry70 = SnapshotTestFixtures_LdcI8Helper_RunLdcI8 */
	.entry70 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_LdcI8Helper_RunLdcI8),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry71 = SnapshotTestFixtures_LdelemaHelper_RunLdelema */
	.entry71 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_LdelemaHelper_RunLdelema),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry72 = SnapshotTestFixtures_LdelemAllHelper_TestAllElems */
	.entry72 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_LdelemAllHelper_TestAllElems),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry73 = SnapshotTestFixtures_LdftnHelper_GetFnPtr */
	.entry73 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_LdftnHelper_GetFnPtr),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry74 = SnapshotTestFixtures_LdindI1Helper_RunLdindI1 */
	.entry74 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_LdindI1Helper_RunLdindI1),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry75 = SnapshotTestFixtures_LdindI8Helper_RunLdindI8 */
	.entry75 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_LdindI8Helper_RunLdindI8),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry76 = SnapshotTestFixtures_LdindStindGapsHelper_RunLdindI */
	.entry76 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_LdindStindGapsHelper_RunLdindI),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry77 = SnapshotTestFixtures_LdindStindGapsHelper_RunLdindI4 */
	.entry77 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_LdindStindGapsHelper_RunLdindI4),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry78 = SnapshotTestFixtures_LdindStindGapsHelper_RunStindI */
	.entry78 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_LdindStindGapsHelper_RunStindI),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry79 = SnapshotTestFixtures_LdindUnsignedHelper_RunLdindUnsigned */
	.entry79 = {
		/* entry_total_size = 64 */ 64u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_LdindUnsignedHelper_RunLdindUnsigned),
		/* frame_size = 88 */ 88u,
		/* num_gc_slots = 11 */ 11u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u, 72u, 80u }
	},
	/* entry80 = SnapshotTestFixtures_LdindWideHelper_RunLdindWide */
	.entry80 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_LdindWideHelper_RunLdindWide),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry81 = SnapshotTestFixtures_LdtokenHelper_RunLdtoken */
	.entry81 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_LdtokenHelper_RunLdtoken),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry82 = SnapshotTestFixtures_LdVirtftnHelper_RunLdVirtftn */
	.entry82 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_LdVirtftnHelper_RunLdVirtftn),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry83 = SnapshotTestFixtures_LocalAllocHelper_RunAlloc */
	.entry83 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_LocalAllocHelper_RunAlloc),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry84 = SnapshotTestFixtures_LoopHelper_SumToFive */
	.entry84 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_LoopHelper_SumToFive),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry85 = SnapshotTestFixtures_MarkedClass__ctor */
	.entry85 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_MarkedClass__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry86 = SnapshotTestFixtures_MathHelper_RunSquare */
	.entry86 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_MathHelper_RunSquare),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry87 = SnapshotTestFixtures_MoreRareOpsHelper_RunArglist */
	.entry87 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_MoreRareOpsHelper_RunArglist),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry88 = SnapshotTestFixtures_MoreRareOpsHelper_RunJmp */
	.entry88 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_MoreRareOpsHelper_RunJmp),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry89 = SnapshotTestFixtures_MoreRareOpsHelper_RunMkrefany */
	.entry89 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_MoreRareOpsHelper_RunMkrefany),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry90 = SnapshotTestFixtures_MoreRareOpsHelper_RunRefanytype */
	.entry90 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_MoreRareOpsHelper_RunRefanytype),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry91 = SnapshotTestFixtures_MoreRareOpsHelper_RunRefanyval */
	.entry91 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_MoreRareOpsHelper_RunRefanyval),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry92 = SnapshotTestFixtures_MulDivHelper_RunMulDiv */
	.entry92 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_MulDivHelper_RunMulDiv),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry93 = SnapshotTestFixtures_MultiDispatcher_DispatchBoth */
	.entry93 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_MultiDispatcher_DispatchBoth),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry94 = SnapshotTestFixtures_MyClass__ctor_System_Int32 */
	.entry94 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_MyClass__ctor_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 16u, 24u }
	},
	/* entry95 = SnapshotTestFixtures_MyClass_GetValue */
	.entry95 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_MyClass_GetValue),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry96 = SnapshotTestFixtures_NegShiftHelper_RunNegShift */
	.entry96 = {
		/* entry_total_size = 52 */ 52u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_NegShiftHelper_RunNegShift),
		/* frame_size = 64 */ 64u,
		/* num_gc_slots = 8 */ 8u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u }
	},
	/* entry97 = SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfI1_System_Int32 */
	.entry97 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfI1_System_Int32),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 8u, 16u }
	},
	/* entry98 = SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfI1Un_System_Int32 */
	.entry98 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfI1Un_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 8u, 16u, 24u }
	},
	/* entry99 = SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfI2Un_System_Int32 */
	.entry99 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfI2Un_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 8u, 16u, 24u }
	},
	/* entry100 = SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfI4Un_System_Int64 */
	.entry100 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfI4Un_System_Int64),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 8u, 16u, 24u }
	},
	/* entry101 = SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfI8Un_System_Single */
	.entry101 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfI8Un_System_Single),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 8u, 16u, 24u }
	},
	/* entry102 = SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfIUn_System_Int32 */
	.entry102 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfIUn_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 8u, 16u, 24u, 32u }
	},
	/* entry103 = SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfU1Un_System_Int32 */
	.entry103 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfU1Un_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 8u, 16u, 24u }
	},
	/* entry104 = SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfU2Un_System_Int32 */
	.entry104 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfU2Un_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 8u, 16u, 24u }
	},
	/* entry105 = SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfU4Un_System_Int64 */
	.entry105 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfU4Un_System_Int64),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 8u, 16u, 24u }
	},
	/* entry106 = SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfU8Un_System_Double */
	.entry106 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfU8Un_System_Double),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 8u, 16u, 24u }
	},
	/* entry107 = SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfUUn_System_Int32 */
	.entry107 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_OverflowConvExtHelper_ConvOvfUUn_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 8u, 16u, 24u, 32u }
	},
	/* entry108 = SnapshotTestFixtures_OverflowConvHelper_ConvOvfI2_System_Int32 */
	.entry108 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_OverflowConvHelper_ConvOvfI2_System_Int32),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 8u, 16u }
	},
	/* entry109 = SnapshotTestFixtures_OverflowConvHelper_ConvOvfI4 */
	.entry109 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_OverflowConvHelper_ConvOvfI4),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry110 = SnapshotTestFixtures_OverflowConvHelper_ConvOvfI8_System_Single */
	.entry110 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_OverflowConvHelper_ConvOvfI8_System_Single),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 8u, 16u }
	},
	/* entry111 = SnapshotTestFixtures_OverflowConvHelper_ConvOvfU1_System_Int32 */
	.entry111 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_OverflowConvHelper_ConvOvfU1_System_Int32),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 8u, 16u }
	},
	/* entry112 = SnapshotTestFixtures_OverflowConvHelper_ConvOvfU2_System_Int32 */
	.entry112 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_OverflowConvHelper_ConvOvfU2_System_Int32),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 8u, 16u }
	},
	/* entry113 = SnapshotTestFixtures_OverflowConvHelper_ConvOvfU4_System_Int64 */
	.entry113 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_OverflowConvHelper_ConvOvfU4_System_Int64),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 8u, 16u }
	},
	/* entry114 = SnapshotTestFixtures_OverflowConvHelper_ConvOvfU8_System_Double */
	.entry114 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_OverflowConvHelper_ConvOvfU8_System_Double),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 8u, 16u, 24u }
	},
	/* entry115 = SnapshotTestFixtures_OverflowHelper_RunOverflowAdd */
	.entry115 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_OverflowHelper_RunOverflowAdd),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry116 = SnapshotTestFixtures_OverflowHelper_RunOverflowMul */
	.entry116 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_OverflowHelper_RunOverflowMul),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry117 = SnapshotTestFixtures_OverflowHelper_RunOverflowSub */
	.entry117 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_OverflowHelper_RunOverflowSub),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry118 = SnapshotTestFixtures_OverflowUnHelper_RunOverflowAddUn */
	.entry118 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_OverflowUnHelper_RunOverflowAddUn),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry119 = SnapshotTestFixtures_OverflowUnHelper_RunOverflowMulUn */
	.entry119 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_OverflowUnHelper_RunOverflowMulUn),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry120 = SnapshotTestFixtures_OverflowUnHelper_RunOverflowSubUn */
	.entry120 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_OverflowUnHelper_RunOverflowSubUn),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry121 = SnapshotTestFixtures_RareOpsHelper_TestConvRUn_System_Single */
	.entry121 = {
		/* entry_total_size = 52 */ 52u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_RareOpsHelper_TestConvRUn_System_Single),
		/* frame_size = 72 */ 72u,
		/* num_gc_slots = 8 */ 8u,
		/* slots */ { 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u }
	},
	/* entry122 = SnapshotTestFixtures_RareOpsHelper_TestStarg_System_Int32 */
	.entry122 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_RareOpsHelper_TestStarg_System_Int32),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 8u }
	},
	/* entry123 = SnapshotTestFixtures_RethrowHelper_RunRethrow */
	.entry123 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_RethrowHelper_RunRethrow),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry124 = SnapshotTestFixtures_SealedClassVirtualHelper_RunSealedVirtual */
	.entry124 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_SealedClassVirtualHelper_RunSealedVirtual),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry125 = SnapshotTestFixtures_SealedHelper__ctor_System_Int32 */
	.entry125 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_SealedHelper__ctor_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 16u, 24u }
	},
	/* entry126 = SnapshotTestFixtures_SealedHelper_GetValueVirtual */
	.entry126 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_SealedHelper_GetValueVirtual),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry127 = SnapshotTestFixtures_ShiftHelper_RunShift */
	.entry127 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_ShiftHelper_RunShift),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry128 = SnapshotTestFixtures_SimpleGapsHelper_RunConvI */
	.entry128 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_SimpleGapsHelper_RunConvI),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry129 = SnapshotTestFixtures_SimpleGapsHelper_RunConvOvfI_System_Int32 */
	.entry129 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_SimpleGapsHelper_RunConvOvfI_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 8u, 16u, 24u }
	},
	/* entry130 = SnapshotTestFixtures_SimpleGapsHelper_RunConvOvfIUn_System_Int32 */
	.entry130 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_SimpleGapsHelper_RunConvOvfIUn_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 8u, 16u, 24u, 32u }
	},
	/* entry131 = SnapshotTestFixtures_SimpleGapsHelper_RunConvOvfU_System_Int32 */
	.entry131 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_SimpleGapsHelper_RunConvOvfU_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 8u, 16u, 24u }
	},
	/* entry132 = SnapshotTestFixtures_SimpleGapsHelper_RunConvOvfUUn_System_Int32 */
	.entry132 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_SimpleGapsHelper_RunConvOvfUUn_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 8u, 16u, 24u, 32u }
	},
	/* entry133 = SnapshotTestFixtures_SimpleGapsHelper_RunConvU4 */
	.entry133 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_SimpleGapsHelper_RunConvU4),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry134 = SnapshotTestFixtures_SimpleGapsHelper_RunLdnull */
	.entry134 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_SimpleGapsHelper_RunLdnull),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry135 = SnapshotTestFixtures_SizeOfHelper_GetSize */
	.entry135 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_SizeOfHelper_GetSize),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry136 = SnapshotTestFixtures_StaticFieldWriteHelper_WriteAndRead */
	.entry136 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_StaticFieldWriteHelper_WriteAndRead),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry137 = SnapshotTestFixtures_StelemAllHelper_TestAllElems */
	.entry137 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_StelemAllHelper_TestAllElems),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry138 = SnapshotTestFixtures_StindNarrowHelper_RunStindNarrow */
	.entry138 = {
		/* entry_total_size = 52 */ 52u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_StindNarrowHelper_RunStindNarrow),
		/* frame_size = 64 */ 64u,
		/* num_gc_slots = 8 */ 8u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u }
	},
	/* entry139 = SnapshotTestFixtures_StindWideHelper_RunStindWide */
	.entry139 = {
		/* entry_total_size = 68 */ 68u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_StindWideHelper_RunStindWide),
		/* frame_size = 96 */ 96u,
		/* num_gc_slots = 12 */ 12u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u, 72u, 80u, 88u }
	},
	/* entry140 = SnapshotTestFixtures_StringConcatHelper_TestConcat */
	.entry140 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_StringConcatHelper_TestConcat),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry141 = SnapshotTestFixtures_StringFormatHelper_TestFormatOne */
	.entry141 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_StringFormatHelper_TestFormatOne),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry142 = SnapshotTestFixtures_StringOps_UseString */
	.entry142 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_StringOps_UseString),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry143 = SnapshotTestFixtures_SwitchHelper_Classify */
	.entry143 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_SwitchHelper_Classify),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry144 = SnapshotTestFixtures_TargetHelper_GetValue */
	.entry144 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_TargetHelper_GetValue),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry145 = SnapshotTestFixtures_ThrowHelper_CheckPositive_System_Int32 */
	.entry145 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_ThrowHelper_CheckPositive_System_Int32),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 8u, 16u }
	},
	/* entry146 = SnapshotTestFixtures_TypeCheckHelper_CheckAndCast */
	.entry146 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_TypeCheckHelper_CheckAndCast),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry147 = SnapshotTestFixtures_UnboxHelper_RunUnbox */
	.entry147 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_UnboxHelper_RunUnbox),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry148 = SnapshotTestFixtures_UnsignedOpsHelper_TestCkfinite_System_Double */
	.entry148 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_UnsignedOpsHelper_TestCkfinite_System_Double),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 8u }
	},
	/* entry149 = SnapshotTestFixtures_UnsignedOpsHelper_TestCltUn */
	.entry149 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_UnsignedOpsHelper_TestCltUn),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry150 = SnapshotTestFixtures_UnsignedOpsHelper_TestDivUn */
	.entry150 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_UnsignedOpsHelper_TestDivUn),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry151 = SnapshotTestFixtures_UnsignedOpsHelper_TestRemUn */
	.entry151 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_UnsignedOpsHelper_TestRemUn),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry152 = SnapshotTestFixtures_ValueTypeHelper_RunValueType */
	.entry152 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_ValueTypeHelper_RunValueType),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry153 = SnapshotTestFixtures_VirtualDispatchHelper_UseVirtualDispatch */
	.entry153 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_VirtualDispatchHelper_UseVirtualDispatch),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry154 = SnapshotTestFixtures_VirtualHelper_UseVirtual */
	.entry154 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_VirtualHelper_UseVirtual),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry155 = SnapshotTestFixtures_VoidCaller_DoNothing */
	.entry155 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_VoidCaller_DoNothing),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry156 = SnapshotTestFixtures_Wrapper_1__ctor_0 */
	.entry156 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_Wrapper_1__ctor_0),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry157 = SnapshotTestFixtures_Wrapper_1_GetValue */
	.entry157 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_Wrapper_1_GetValue),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry158 = SnapshotTestFixtures_Wrapper__0__ctor_0 */
	.entry158 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_Wrapper__0__ctor_0),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry159 = SnapshotTestFixtures_Wrapper__0_GetValue */
	.entry159 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_Wrapper__0_GetValue),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry160 = SnapshotTestFixtures_Wrapper_System_Int32__ctor_System_Int32 */
	.entry160 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_Wrapper_System_Int32__ctor_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 16u, 24u }
	},
	/* entry161 = SnapshotTestFixtures_Wrapper_System_Int32_GetValue */
	.entry161 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_Wrapper_System_Int32_GetValue),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 5648u;

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[165] = {
	reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_0),
	reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_1),
	reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_2),
	reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_3),
	reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_4),
	reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_5),
	reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_6),
	reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_Subject_7),
	reinterpret_cast<void*>(&SnapshotProverSubjects_SnapshotProverSubjects_CustomEntrySubject_8),
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
	reinterpret_cast<void*>(&SnapshotTestFixtures_EhCatchFinallyNestedHelper_CatchFinallyNested_System_Int32),
	reinterpret_cast<void*>(&SnapshotTestFixtures_EhFaultFinallyHelper_FaultFinally_System_Int32),
	reinterpret_cast<void*>(&SnapshotTestFixtures_EhFaultHelper_TryFault_System_Int32),
	reinterpret_cast<void*>(&SnapshotTestFixtures_EhFilterFinallyHelper_FilterFinally_System_Int32),
	reinterpret_cast<void*>(&SnapshotTestFixtures_EhFilterHelper_TryFilter_System_Int32),
	reinterpret_cast<void*>(&SnapshotTestFixtures_EhFinallyHelper_TryFinally_Only_System_Int32),
	reinterpret_cast<void*>(&SnapshotTestFixtures_EhHelper_SafeDivide),
	reinterpret_cast<void*>(&SnapshotTestFixtures_EhMultipleCatchHelper_MultipleCatch_System_Int32),
	reinterpret_cast<void*>(&SnapshotTestFixtures_EhNestedTryCatchHelper_NestedTryCatch_System_Int32),
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
		0x0000006Bu,
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
		0x0000006Au,
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
	.method_pointer_count      = 165u,
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
	.image_name_utf8    = "SnapshotProverSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_SnapshotProverSubjects_SnapshotProverSubjects[1] = {
	{ 0x04000001u, "SnapshotProverSubjects/SnapshotProverSubjects::_exitCode", "_exitCode", "System.Int32", 0LL, 3u },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotProverSubjects_SnapshotProverSubjects[10] = {
	{ 0x00000084u, "SnapshotProverSubjects/SnapshotProverSubjects::Subject_0:System.Void()", "Subject_0", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000086u, "SnapshotProverSubjects/SnapshotProverSubjects::Subject_1:System.Void()", "Subject_1", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000088u, "SnapshotProverSubjects/SnapshotProverSubjects::Subject_2:System.Void()", "Subject_2", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000008Au, "SnapshotProverSubjects/SnapshotProverSubjects::Subject_3:System.Void()", "Subject_3", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000008Cu, "SnapshotProverSubjects/SnapshotProverSubjects::Subject_4:System.Void()", "Subject_4", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000008Eu, "SnapshotProverSubjects/SnapshotProverSubjects::Subject_5:System.Void()", "Subject_5", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000090u, "SnapshotProverSubjects/SnapshotProverSubjects::Subject_6:System.Void()", "Subject_6", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000092u, "SnapshotProverSubjects/SnapshotProverSubjects::Subject_7:System.Void()", "Subject_7", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000094u, "SnapshotProverSubjects/SnapshotProverSubjects::CustomEntrySubject_8:System.Void()", "CustomEntrySubject_8", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000096u, "SnapshotProverSubjects/SnapshotProverSubjects::Subject_9:System.Void()", "Subject_9", "System.Void", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_AddressHelper[1] = {
	{ 0x000000C5u, "SnapshotTestFixtures/AddressHelper::RunAddress:System.Int32()", "RunAddress", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_ArithmeticCompareHelper[1] = {
	{ 0x000000B0u, "SnapshotTestFixtures/ArithmeticCompareHelper::RunCompare:System.Int32()", "RunCompare", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_ArithmeticOps[1] = {
	{ 0x00000085u, "SnapshotTestFixtures/ArithmeticOps::RunAdd:System.Int32()", "RunAdd", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_ArrayHelper[1] = {
	{ 0x000000A1u, "SnapshotTestFixtures/ArrayHelper::MakeAndFill:System.Int32()", "MakeAndFill", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_ArrayLengthHelper[1] = {
	{ 0x000000B8u, "SnapshotTestFixtures/ArrayLengthHelper::RunLength:System.Int32()", "RunLength", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_ArrayRefHelper[1] = {
	{ 0x000000D2u, "SnapshotTestFixtures/ArrayRefHelper::RunArrayRef:System.Int32()", "RunArrayRef", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_BaseClass[2] = {
	{ 0x000000A8u, "SnapshotTestFixtures/BaseClass::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000A7u, "SnapshotTestFixtures/BaseClass::Compute:System.Int32()", "Compute", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_BitwiseHelper[1] = {
	{ 0x000000B6u, "SnapshotTestFixtures/BitwiseHelper::RunBitwise:System.Int32()", "RunBitwise", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_BoxingHelper[1] = {
	{ 0x00000095u, "SnapshotTestFixtures/BoxingHelper::BoxAndUnbox:System.Int32()", "BoxAndUnbox", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_BoxInterfaceArrayDemo[1] = {
	{ 0x00000114u, "SnapshotTestFixtures/BoxInterfaceArrayDemo::DemoBoxStore:System.Int32()", "DemoBoxStore", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_BranchCompareBHelper[1] = {
	{ 0x000000C3u, "SnapshotTestFixtures/BranchCompareBHelper::RunBranchCompareB:System.Int32()", "RunBranchCompareB", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_BranchCompareHelper[1] = {
	{ 0x000000BCu, "SnapshotTestFixtures/BranchCompareHelper::RunBranchCompare:System.Int32()", "RunBranchCompare", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_BranchDupHelper[1] = {
	{ 0x000000B1u, "SnapshotTestFixtures/BranchDupHelper::RunBranchDup:System.Int32()", "RunBranchDup", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_BranchUnsignedBHelper[1] = {
	{ 0x000000EDu, "SnapshotTestFixtures/BranchUnsignedBHelper::RunBranchUnsignedB:System.Int32()", "RunBranchUnsignedB", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_BranchUnsignedHelper[1] = {
	{ 0x000000ECu, "SnapshotTestFixtures/BranchUnsignedHelper::RunBranchUnsigned:System.Int32()", "RunBranchUnsigned", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_CalliHelper[1] = {
	{ 0x000000DEu, "SnapshotTestFixtures/CalliHelper::RunCalli:System.Int32(System.Int32)", "RunCalli", "System.Int32", 1, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_CallVirtHelper[1] = {
	{ 0x00000093u, "SnapshotTestFixtures/CallVirtHelper::CreateAndUse:System.Int32()", "CreateAndUse", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_CallvirtHelper[1] = {
	{ 0x000000CBu, "SnapshotTestFixtures/CallvirtHelper::RunCallvirt:System.Int32()", "RunCallvirt", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_CollectionsHelper[3] = {
	{ 0x0000010Du, "SnapshotTestFixtures/CollectionsHelper::TestDict:System.Int32()", "TestDict", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000010Cu, "SnapshotTestFixtures/CollectionsHelper::TestList:System.Int32()", "TestList", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000010Eu, "SnapshotTestFixtures/CollectionsHelper::TestSet:System.Int32()", "TestSet", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_ConversionHelper[1] = {
	{ 0x00000099u, "SnapshotTestFixtures/ConversionHelper::ConvertToInt:System.Int32()", "ConvertToInt", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_ConvSmallIntHelper[1] = {
	{ 0x000000C1u, "SnapshotTestFixtures/ConvSmallIntHelper::RunConvSmall:System.Int32()", "RunConvSmall", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_ConvWideHelper[1] = {
	{ 0x000000BEu, "SnapshotTestFixtures/ConvWideHelper::RunConvWide:System.Int32()", "RunConvWide", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_CpblkHelper[1] = {
	{ 0x000000CDu, "SnapshotTestFixtures/CpblkHelper::RunCpblk:System.Int32()", "RunCpblk", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_CpobjHelper[1] = {
	{ 0x000000C9u, "SnapshotTestFixtures/CpobjHelper::RunCpobj:System.Int32()", "RunCpobj", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_DelegateHelper[1] = {
	{ 0x000000A6u, "SnapshotTestFixtures/DelegateHelper::RunDelegate:System.Int32()", "RunDelegate", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_DerivedClass[2] = {
	{ 0x000000AAu, "SnapshotTestFixtures/DerivedClass::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000A9u, "SnapshotTestFixtures/DerivedClass::Compute:System.Int32()", "Compute", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_EhCatchFinallyNestedHelper[1] = {
	{ 0x00000122u, "SnapshotTestFixtures/EhCatchFinallyNestedHelper::CatchFinallyNested:System.Int32(System.Int32)", "CatchFinallyNested", "System.Int32", 1, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_EhFaultFinallyHelper[1] = {
	{ 0x00000125u, "SnapshotTestFixtures/EhFaultFinallyHelper::FaultFinally:System.Int32(System.Int32)", "FaultFinally", "System.Int32", 1, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_EhFaultHelper[1] = {
	{ 0x0000011Fu, "SnapshotTestFixtures/EhFaultHelper::TryFault:System.Int32(System.Int32)", "TryFault", "System.Int32", 1, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_EhFilterFinallyHelper[1] = {
	{ 0x00000123u, "SnapshotTestFixtures/EhFilterFinallyHelper::FilterFinally:System.Int32(System.Int32)", "FilterFinally", "System.Int32", 1, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_EhFilterHelper[1] = {
	{ 0x00000120u, "SnapshotTestFixtures/EhFilterHelper::TryFilter:System.Int32(System.Int32)", "TryFilter", "System.Int32", 1, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_EhFinallyHelper[1] = {
	{ 0x0000011Eu, "SnapshotTestFixtures/EhFinallyHelper::TryFinally_Only:System.Int32(System.Int32)", "TryFinally_Only", "System.Int32", 1, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_EhHelper[1] = {
	{ 0x000000A2u, "SnapshotTestFixtures/EhHelper::SafeDivide:System.Int32()", "SafeDivide", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_EhMultipleCatchHelper[1] = {
	{ 0x00000121u, "SnapshotTestFixtures/EhMultipleCatchHelper::MultipleCatch:System.Int32(System.Int32)", "MultipleCatch", "System.Int32", 1, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_EhNestedTryCatchHelper[1] = {
	{ 0x00000124u, "SnapshotTestFixtures/EhNestedTryCatchHelper::NestedTryCatch:System.Int32(System.Int32)", "NestedTryCatch", "System.Int32", 1, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_ExternalCaller[1] = {
	{ 0x0000010Fu, "SnapshotTestFixtures/ExternalCaller::CallExternal:System.Int32()", "CallExternal", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_FieldHelper[1] = {
	{ 0x00000097u, "SnapshotTestFixtures/FieldHelper::GetAndIncrement:System.Int32()", "GetAndIncrement", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_FloatOpsHelper[1] = {
	{ 0x000000BDu, "SnapshotTestFixtures/FloatOpsHelper::RunFloatOps:System.Int32()", "RunFloatOps", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_FlowControl[1] = {
	{ 0x0000008Du, "SnapshotTestFixtures/FlowControl::IsPositive:System.Int32()", "IsPositive", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_GenericHelper[1] = {
	{ 0x000000A3u, "SnapshotTestFixtures/GenericHelper::UseGeneric:System.Int32()", "UseGeneric", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_GenericsVirtEhDemo[1] = {
	{ 0x00000113u, "SnapshotTestFixtures/GenericsVirtEhDemo::DemoCombine:System.Int32()", "DemoCombine", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_HasFields[1] = {
	{ 0x000000C4u, "SnapshotTestFixtures/HasFields::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_HasInstanceFields[1] = {
	{ 0x000000B2u, "SnapshotTestFixtures/HasInstanceFields::.ctor:System.Void(System.Int32)", ".ctor", "System.Void", 1, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_Helper[4] = {
	{ 0x0000009Cu, "SnapshotTestFixtures/Helper::ConsumeString:System.Void(System.String)", "ConsumeString", "System.Void", 1, nullptr, 0u, nullptr, 0u },
	{ 0x0000009Du, "SnapshotTestFixtures/Helper::GetValue:System.Int32()", "GetValue", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000009Au, "SnapshotTestFixtures/Helper::Nop:System.Void()", "Nop", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000009Bu, "SnapshotTestFixtures/Helper::Square:System.Int32(System.Int32)", "Square", "System.Int32", 1, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_HotUpdateWithGenericsDemo[1] = {
	{ 0x00000116u, "SnapshotTestFixtures/HotUpdateWithGenericsDemo::Run:System.Int32()", "Run", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_HotUpdateWithTypesDemo[1] = {
	{ 0x00000115u, "SnapshotTestFixtures/HotUpdateWithTypesDemo::Run:System.Int32()", "Run", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_ImplHelper[2] = {
	{ 0x0000011Bu, "SnapshotTestFixtures/ImplHelper::.ctor:System.Void(System.Int32)", ".ctor", "System.Void", 1, nullptr, 0u, nullptr, 0u },
	{ 0x0000011Cu, "SnapshotTestFixtures/ImplHelper::GetValue:System.Int32()", "GetValue", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_IndirectHelper[1] = {
	{ 0x000000AFu, "SnapshotTestFixtures/IndirectHelper::ReadWriteRef:System.Int32()", "ReadWriteRef", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_InstanceFieldHelper[1] = {
	{ 0x000000B3u, "SnapshotTestFixtures/InstanceFieldHelper::CreateAndUseFields:System.Int32()", "CreateAndUseFields", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_InstanceHelper[2] = {
	{ 0x0000008Fu, "SnapshotTestFixtures/InstanceHelper::.ctor:System.Void(System.Int32)", ".ctor", "System.Void", 1, nullptr, 0u, nullptr, 0u },
	{ 0x00000091u, "SnapshotTestFixtures/InstanceHelper::GetValue:System.Int32()", "GetValue", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_InterfaceDevirtHelper[1] = {
	{ 0x0000011Du, "SnapshotTestFixtures/InterfaceDevirtHelper::RunInterfaceTest:System.Int32()", "RunInterfaceTest", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_InternalHelper[1] = {
	{ 0x00000111u, "SnapshotTestFixtures/InternalHelper::Double:System.Int32(System.Int32)", "Double", "System.Int32", 1, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_LdargaHelper[1] = {
	{ 0x000000C8u, "SnapshotTestFixtures/LdargaHelper::RunLdarga:System.Int32(System.Int32)", "RunLdarga", "System.Int32", 1, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_LdcI8Helper[1] = {
	{ 0x000000C0u, "SnapshotTestFixtures/LdcI8Helper::RunLdcI8:System.Int64()", "RunLdcI8", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_LdelemaHelper[1] = {
	{ 0x000000C7u, "SnapshotTestFixtures/LdelemaHelper::RunLdelema:System.Int32()", "RunLdelema", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_LdelemAllHelper[1] = {
	{ 0x000000E2u, "SnapshotTestFixtures/LdelemAllHelper::TestAllElems:System.Int32()", "TestAllElems", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_LdftnHelper[1] = {
	{ 0x0000009Fu, "SnapshotTestFixtures/LdftnHelper::GetFnPtr:System.Int32()", "GetFnPtr", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_LdindI1Helper[1] = {
	{ 0x000000C2u, "SnapshotTestFixtures/LdindI1Helper::RunLdindI1:System.Int32()", "RunLdindI1", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_LdindI8Helper[1] = {
	{ 0x000000CFu, "SnapshotTestFixtures/LdindI8Helper::RunLdindI8:System.Int32()", "RunLdindI8", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_LdindStindGapsHelper[3] = {
	{ 0x0000010Au, "SnapshotTestFixtures/LdindStindGapsHelper::RunLdindI:System.Int32()", "RunLdindI", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000109u, "SnapshotTestFixtures/LdindStindGapsHelper::RunLdindI4:System.Int32()", "RunLdindI4", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000010Bu, "SnapshotTestFixtures/LdindStindGapsHelper::RunStindI:System.Int32()", "RunStindI", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_LdindUnsignedHelper[1] = {
	{ 0x000000CEu, "SnapshotTestFixtures/LdindUnsignedHelper::RunLdindUnsigned:System.Int32()", "RunLdindUnsigned", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_LdindWideHelper[1] = {
	{ 0x000000D0u, "SnapshotTestFixtures/LdindWideHelper::RunLdindWide:System.Int32()", "RunLdindWide", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_LdtokenHelper[1] = {
	{ 0x000000C6u, "SnapshotTestFixtures/LdtokenHelper::RunLdtoken:System.Int32()", "RunLdtoken", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_LdVirtftnHelper[1] = {
	{ 0x000000E1u, "SnapshotTestFixtures/LdVirtftnHelper::RunLdVirtftn:System.Int32()", "RunLdVirtftn", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_LocalAllocHelper[1] = {
	{ 0x000000BAu, "SnapshotTestFixtures/LocalAllocHelper::RunAlloc:System.Int32()", "RunAlloc", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_LoopHelper[1] = {
	{ 0x00000098u, "SnapshotTestFixtures/LoopHelper::SumToFive:System.Int32()", "SumToFive", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_MarkedClass[1] = {
	{ 0x000000ACu, "SnapshotTestFixtures/MarkedClass::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_MathHelper[1] = {
	{ 0x0000008Bu, "SnapshotTestFixtures/MathHelper::RunSquare:System.Int32()", "RunSquare", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_MoreRareOpsHelper[5] = {
	{ 0x000000F2u, "SnapshotTestFixtures/MoreRareOpsHelper::RunArglist:System.Int32()", "RunArglist", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000F6u, "SnapshotTestFixtures/MoreRareOpsHelper::RunJmp:System.Int32()", "RunJmp", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000F3u, "SnapshotTestFixtures/MoreRareOpsHelper::RunMkrefany:System.Int32()", "RunMkrefany", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000F5u, "SnapshotTestFixtures/MoreRareOpsHelper::RunRefanytype:System.Int32()", "RunRefanytype", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000F4u, "SnapshotTestFixtures/MoreRareOpsHelper::RunRefanyval:System.Int32()", "RunRefanyval", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_MulDivHelper[1] = {
	{ 0x000000BBu, "SnapshotTestFixtures/MulDivHelper::RunMulDiv:System.Int32()", "RunMulDiv", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_MultiDispatcher[1] = {
	{ 0x00000110u, "SnapshotTestFixtures/MultiDispatcher::DispatchBoth:System.Int32()", "DispatchBoth", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_MyClass[2] = {
	{ 0x000000DFu, "SnapshotTestFixtures/MyClass::.ctor:System.Void(System.Int32)", ".ctor", "System.Void", 1, nullptr, 0u, nullptr, 0u },
	{ 0x000000E0u, "SnapshotTestFixtures/MyClass::GetValue:System.Int32()", "GetValue", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_NegShiftHelper[1] = {
	{ 0x000000BFu, "SnapshotTestFixtures/NegShiftHelper::RunNegShift:System.Int32()", "RunNegShift", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_OverflowConvExtHelper[11] = {
	{ 0x000000F7u, "SnapshotTestFixtures/OverflowConvExtHelper::ConvOvfI1:System.Int32(System.Int32)", "ConvOvfI1", "System.Int32", 1, nullptr, 0u, nullptr, 0u },
	{ 0x000000F8u, "SnapshotTestFixtures/OverflowConvExtHelper::ConvOvfI1Un:System.Int32(System.Int32)", "ConvOvfI1Un", "System.Int32", 1, nullptr, 0u, nullptr, 0u },
	{ 0x000000FAu, "SnapshotTestFixtures/OverflowConvExtHelper::ConvOvfI2Un:System.Int32(System.Int32)", "ConvOvfI2Un", "System.Int32", 1, nullptr, 0u, nullptr, 0u },
	{ 0x000000FCu, "SnapshotTestFixtures/OverflowConvExtHelper::ConvOvfI4Un:System.Int32(System.Int64)", "ConvOvfI4Un", "System.Int32", 1, nullptr, 0u, nullptr, 0u },
	{ 0x000000FEu, "SnapshotTestFixtures/OverflowConvExtHelper::ConvOvfI8Un:System.Int64(System.Single)", "ConvOvfI8Un", "System.Int64", 1, nullptr, 0u, nullptr, 0u },
	{ 0x00000100u, "SnapshotTestFixtures/OverflowConvExtHelper::ConvOvfIUn:System.Int32(System.Int32)", "ConvOvfIUn", "System.Int32", 1, nullptr, 0u, nullptr, 0u },
	{ 0x000000F9u, "SnapshotTestFixtures/OverflowConvExtHelper::ConvOvfU1Un:System.Int32(System.Int32)", "ConvOvfU1Un", "System.Int32", 1, nullptr, 0u, nullptr, 0u },
	{ 0x000000FBu, "SnapshotTestFixtures/OverflowConvExtHelper::ConvOvfU2Un:System.Int32(System.Int32)", "ConvOvfU2Un", "System.Int32", 1, nullptr, 0u, nullptr, 0u },
	{ 0x000000FDu, "SnapshotTestFixtures/OverflowConvExtHelper::ConvOvfU4Un:System.Int32(System.Int64)", "ConvOvfU4Un", "System.Int32", 1, nullptr, 0u, nullptr, 0u },
	{ 0x000000FFu, "SnapshotTestFixtures/OverflowConvExtHelper::ConvOvfU8Un:System.Int64(System.Double)", "ConvOvfU8Un", "System.Int64", 1, nullptr, 0u, nullptr, 0u },
	{ 0x00000101u, "SnapshotTestFixtures/OverflowConvExtHelper::ConvOvfUUn:System.Int32(System.Int32)", "ConvOvfUUn", "System.Int32", 1, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_OverflowConvHelper[7] = {
	{ 0x000000D9u, "SnapshotTestFixtures/OverflowConvHelper::ConvOvfI2:System.Int32(System.Int32)", "ConvOvfI2", "System.Int32", 1, nullptr, 0u, nullptr, 0u },
	{ 0x000000D7u, "SnapshotTestFixtures/OverflowConvHelper::ConvOvfI4:System.Int32()", "ConvOvfI4", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000DCu, "SnapshotTestFixtures/OverflowConvHelper::ConvOvfI8:System.Int64(System.Single)", "ConvOvfI8", "System.Int64", 1, nullptr, 0u, nullptr, 0u },
	{ 0x000000D8u, "SnapshotTestFixtures/OverflowConvHelper::ConvOvfU1:System.Int32(System.Int32)", "ConvOvfU1", "System.Int32", 1, nullptr, 0u, nullptr, 0u },
	{ 0x000000DAu, "SnapshotTestFixtures/OverflowConvHelper::ConvOvfU2:System.Int32(System.Int32)", "ConvOvfU2", "System.Int32", 1, nullptr, 0u, nullptr, 0u },
	{ 0x000000DBu, "SnapshotTestFixtures/OverflowConvHelper::ConvOvfU4:System.Int32(System.Int64)", "ConvOvfU4", "System.Int32", 1, nullptr, 0u, nullptr, 0u },
	{ 0x000000DDu, "SnapshotTestFixtures/OverflowConvHelper::ConvOvfU8:System.Int64(System.Double)", "ConvOvfU8", "System.Int64", 1, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_OverflowHelper[3] = {
	{ 0x000000D4u, "SnapshotTestFixtures/OverflowHelper::RunOverflowAdd:System.Int32()", "RunOverflowAdd", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000D6u, "SnapshotTestFixtures/OverflowHelper::RunOverflowMul:System.Int32()", "RunOverflowMul", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000D5u, "SnapshotTestFixtures/OverflowHelper::RunOverflowSub:System.Int32()", "RunOverflowSub", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_OverflowUnHelper[3] = {
	{ 0x000000EFu, "SnapshotTestFixtures/OverflowUnHelper::RunOverflowAddUn:System.Int32()", "RunOverflowAddUn", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000F1u, "SnapshotTestFixtures/OverflowUnHelper::RunOverflowMulUn:System.Int32()", "RunOverflowMulUn", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000F0u, "SnapshotTestFixtures/OverflowUnHelper::RunOverflowSubUn:System.Int32()", "RunOverflowSubUn", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_RareOpsHelper[3] = {
	{ 0x000000EAu, "SnapshotTestFixtures/RareOpsHelper::TestConvRUn:System.Int32(System.Single)", "TestConvRUn", "System.Int32", 1, nullptr, 0u, nullptr, 0u },
	{ 0x000000E8u, "SnapshotTestFixtures/RareOpsHelper::TestInitBlk:System.Void(System.Int32&)", "TestInitBlk", "System.Void", 1, nullptr, 0u, nullptr, 0u },
	{ 0x000000E9u, "SnapshotTestFixtures/RareOpsHelper::TestStarg:System.Int32(System.Int32)", "TestStarg", "System.Int32", 1, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_RethrowHelper[1] = {
	{ 0x000000EEu, "SnapshotTestFixtures/RethrowHelper::RunRethrow:System.Int32()", "RunRethrow", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_SealedClassVirtualHelper[1] = {
	{ 0x00000119u, "SnapshotTestFixtures/SealedClassVirtualHelper::RunSealedVirtual:System.Int32()", "RunSealedVirtual", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_SealedHelper[2] = {
	{ 0x00000117u, "SnapshotTestFixtures/SealedHelper::.ctor:System.Void(System.Int32)", ".ctor", "System.Void", 1, nullptr, 0u, nullptr, 0u },
	{ 0x00000118u, "SnapshotTestFixtures/SealedHelper::GetValueVirtual:System.Int32()", "GetValueVirtual", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_ShiftHelper[1] = {
	{ 0x000000B7u, "SnapshotTestFixtures/ShiftHelper::RunShift:System.Int32()", "RunShift", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_SimpleGapsHelper[7] = {
	{ 0x00000104u, "SnapshotTestFixtures/SimpleGapsHelper::RunConvI:System.Int32()", "RunConvI", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000105u, "SnapshotTestFixtures/SimpleGapsHelper::RunConvOvfI:System.Int32(System.Int32)", "RunConvOvfI", "System.Int32", 1, nullptr, 0u, nullptr, 0u },
	{ 0x00000107u, "SnapshotTestFixtures/SimpleGapsHelper::RunConvOvfIUn:System.Int32(System.Int32)", "RunConvOvfIUn", "System.Int32", 1, nullptr, 0u, nullptr, 0u },
	{ 0x00000106u, "SnapshotTestFixtures/SimpleGapsHelper::RunConvOvfU:System.Int32(System.Int32)", "RunConvOvfU", "System.Int32", 1, nullptr, 0u, nullptr, 0u },
	{ 0x00000108u, "SnapshotTestFixtures/SimpleGapsHelper::RunConvOvfUUn:System.Int32(System.Int32)", "RunConvOvfUUn", "System.Int32", 1, nullptr, 0u, nullptr, 0u },
	{ 0x00000103u, "SnapshotTestFixtures/SimpleGapsHelper::RunConvU4:System.Int32()", "RunConvU4", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000102u, "SnapshotTestFixtures/SimpleGapsHelper::RunLdnull:System.Int32()", "RunLdnull", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_SizeOfHelper[1] = {
	{ 0x000000AEu, "SnapshotTestFixtures/SizeOfHelper::GetSize:System.Int32()", "GetSize", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_StaticFieldWriteHelper[1] = {
	{ 0x000000B4u, "SnapshotTestFixtures/StaticFieldWriteHelper::WriteAndRead:System.Int32()", "WriteAndRead", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_StelemAllHelper[1] = {
	{ 0x000000E3u, "SnapshotTestFixtures/StelemAllHelper::TestAllElems:System.Int32()", "TestAllElems", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_StindNarrowHelper[1] = {
	{ 0x000000CAu, "SnapshotTestFixtures/StindNarrowHelper::RunStindNarrow:System.Int32()", "RunStindNarrow", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_StindWideHelper[1] = {
	{ 0x000000D1u, "SnapshotTestFixtures/StindWideHelper::RunStindWide:System.Int32()", "RunStindWide", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_StringConcatHelper[1] = {
	{ 0x00000112u, "SnapshotTestFixtures/StringConcatHelper::TestConcat:System.Int32()", "TestConcat", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_StringFormatHelper[1] = {
	{ 0x000000EBu, "SnapshotTestFixtures/StringFormatHelper::TestFormatOne:System.Int32()", "TestFormatOne", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_StringOps[1] = {
	{ 0x00000087u, "SnapshotTestFixtures/StringOps::UseString:System.Int32()", "UseString", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_SwitchHelper[1] = {
	{ 0x0000009Eu, "SnapshotTestFixtures/SwitchHelper::Classify:System.Int32()", "Classify", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_TargetHelper[1] = {
	{ 0x000000A0u, "SnapshotTestFixtures/TargetHelper::GetValue:System.Int32()", "GetValue", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_ThrowHelper[1] = {
	{ 0x000000D3u, "SnapshotTestFixtures/ThrowHelper::CheckPositive:System.Int32(System.Int32)", "CheckPositive", "System.Int32", 1, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_TypeCheckHelper[1] = {
	{ 0x000000ADu, "SnapshotTestFixtures/TypeCheckHelper::CheckAndCast:System.Int32()", "CheckAndCast", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_UnboxHelper[1] = {
	{ 0x000000CCu, "SnapshotTestFixtures/UnboxHelper::RunUnbox:System.Int32()", "RunUnbox", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_UnsignedOpsHelper[4] = {
	{ 0x000000E7u, "SnapshotTestFixtures/UnsignedOpsHelper::TestCkfinite:System.Double(System.Double)", "TestCkfinite", "System.Double", 1, nullptr, 0u, nullptr, 0u },
	{ 0x000000E4u, "SnapshotTestFixtures/UnsignedOpsHelper::TestCltUn:System.Int32()", "TestCltUn", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000E5u, "SnapshotTestFixtures/UnsignedOpsHelper::TestDivUn:System.Int32()", "TestDivUn", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000E6u, "SnapshotTestFixtures/UnsignedOpsHelper::TestRemUn:System.Int32()", "TestRemUn", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_ValueTypeHelper[1] = {
	{ 0x000000B9u, "SnapshotTestFixtures/ValueTypeHelper::RunValueType:System.Int32()", "RunValueType", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_VirtualDispatchHelper[1] = {
	{ 0x000000B5u, "SnapshotTestFixtures/VirtualDispatchHelper::UseVirtualDispatch:System.Int32()", "UseVirtualDispatch", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_VirtualHelper[1] = {
	{ 0x000000ABu, "SnapshotTestFixtures/VirtualHelper::UseVirtual:System.Int32()", "UseVirtual", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_VoidCaller[1] = {
	{ 0x00000089u, "SnapshotTestFixtures/VoidCaller::DoNothing:System.Int32()", "DoNothing", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_Wrapper_1[2] = {
	{ 0x000000A4u, "SnapshotTestFixtures/Wrapper`1::.ctor:System.Void(!0)", ".ctor", "System.Void", 1, nullptr, 0u, nullptr, 0u },
	{ 0x000000A5u, "SnapshotTestFixtures/Wrapper`1::GetValue:!0()", "GetValue", "!0", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_Wrapper__0_[2] = {
	{ 0x00000128u, "SnapshotTestFixtures/Wrapper<!0>::.ctor:System.Void(!0)", ".ctor", "System.Void", 1, nullptr, 0u, nullptr, 0u },
	{ 0x00000129u, "SnapshotTestFixtures/Wrapper<!0>::GetValue:!0()", "GetValue", "!0", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_Wrapper_System_Int32_[2] = {
	{ 0x00000126u, "SnapshotTestFixtures/Wrapper<System.Int32>::.ctor:System.Void(System.Int32)", ".ctor", "System.Void", 1, nullptr, 0u, nullptr, 0u },
	{ 0x00000127u, "SnapshotTestFixtures/Wrapper<System.Int32>::GetValue:System.Int32()", "GetValue", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[105] = {
	{ 0x00000002u, "SnapshotProverSubjects/SnapshotProverSubjects", "SnapshotProverSubjects/SnapshotProverSubjects", "", "SnapshotProverSubjects", "SnapshotProverSubjects", nullptr, kReflFields_SnapshotProverSubjects_SnapshotProverSubjects, 1u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotProverSubjects_SnapshotProverSubjects, 10u, nullptr, 0u, 0u },
	{ 0x00000033u, "SnapshotTestFixtures/AddressHelper", "SnapshotTestFixtures/AddressHelper", "", "AddressHelper", "AddressHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_AddressHelper, 1u, nullptr, 0u, 0u },
	{ 0x0000001Eu, "SnapshotTestFixtures/ArithmeticCompareHelper", "SnapshotTestFixtures/ArithmeticCompareHelper", "", "ArithmeticCompareHelper", "ArithmeticCompareHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_ArithmeticCompareHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000003u, "SnapshotTestFixtures/ArithmeticOps", "SnapshotTestFixtures/ArithmeticOps", "", "ArithmeticOps", "ArithmeticOps", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_ArithmeticOps, 1u, nullptr, 0u, 0u },
	{ 0x00000012u, "SnapshotTestFixtures/ArrayHelper", "SnapshotTestFixtures/ArrayHelper", "", "ArrayHelper", "ArrayHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_ArrayHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000026u, "SnapshotTestFixtures/ArrayLengthHelper", "SnapshotTestFixtures/ArrayLengthHelper", "", "ArrayLengthHelper", "ArrayLengthHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_ArrayLengthHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000040u, "SnapshotTestFixtures/ArrayRefHelper", "SnapshotTestFixtures/ArrayRefHelper", "", "ArrayRefHelper", "ArrayRefHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_ArrayRefHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000017u, "SnapshotTestFixtures/BaseClass", "SnapshotTestFixtures/BaseClass", "", "BaseClass", "BaseClass", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_BaseClass, 2u, nullptr, 0u, 0u },
	{ 0x00000024u, "SnapshotTestFixtures/BitwiseHelper", "SnapshotTestFixtures/BitwiseHelper", "", "BitwiseHelper", "BitwiseHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_BitwiseHelper, 1u, nullptr, 0u, 0u },
	{ 0x0000000Au, "SnapshotTestFixtures/BoxingHelper", "SnapshotTestFixtures/BoxingHelper", "", "BoxingHelper", "BoxingHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_BoxingHelper, 1u, nullptr, 0u, 0u },
	{ 0x0000005Au, "SnapshotTestFixtures/BoxInterfaceArrayDemo", "SnapshotTestFixtures/BoxInterfaceArrayDemo", "", "BoxInterfaceArrayDemo", "BoxInterfaceArrayDemo", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_BoxInterfaceArrayDemo, 1u, nullptr, 0u, 0u },
	{ 0x00000031u, "SnapshotTestFixtures/BranchCompareBHelper", "SnapshotTestFixtures/BranchCompareBHelper", "", "BranchCompareBHelper", "BranchCompareBHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_BranchCompareBHelper, 1u, nullptr, 0u, 0u },
	{ 0x0000002Au, "SnapshotTestFixtures/BranchCompareHelper", "SnapshotTestFixtures/BranchCompareHelper", "", "BranchCompareHelper", "BranchCompareHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_BranchCompareHelper, 1u, nullptr, 0u, 0u },
	{ 0x0000001Fu, "SnapshotTestFixtures/BranchDupHelper", "SnapshotTestFixtures/BranchDupHelper", "", "BranchDupHelper", "BranchDupHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_BranchDupHelper, 1u, nullptr, 0u, 0u },
	{ 0x0000004Du, "SnapshotTestFixtures/BranchUnsignedBHelper", "SnapshotTestFixtures/BranchUnsignedBHelper", "", "BranchUnsignedBHelper", "BranchUnsignedBHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_BranchUnsignedBHelper, 1u, nullptr, 0u, 0u },
	{ 0x0000004Cu, "SnapshotTestFixtures/BranchUnsignedHelper", "SnapshotTestFixtures/BranchUnsignedHelper", "", "BranchUnsignedHelper", "BranchUnsignedHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_BranchUnsignedHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000044u, "SnapshotTestFixtures/CalliHelper", "SnapshotTestFixtures/CalliHelper", "", "CalliHelper", "CalliHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_CalliHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000009u, "SnapshotTestFixtures/CallVirtHelper", "SnapshotTestFixtures/CallVirtHelper", "", "CallVirtHelper", "CallVirtHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_CallVirtHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000039u, "SnapshotTestFixtures/CallvirtHelper", "SnapshotTestFixtures/CallvirtHelper", "", "CallvirtHelper", "CallvirtHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_CallvirtHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000054u, "SnapshotTestFixtures/CollectionsHelper", "SnapshotTestFixtures/CollectionsHelper", "", "CollectionsHelper", "CollectionsHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_CollectionsHelper, 3u, nullptr, 0u, 0u },
	{ 0x0000000Du, "SnapshotTestFixtures/ConversionHelper", "SnapshotTestFixtures/ConversionHelper", "", "ConversionHelper", "ConversionHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_ConversionHelper, 1u, nullptr, 0u, 0u },
	{ 0x0000002Fu, "SnapshotTestFixtures/ConvSmallIntHelper", "SnapshotTestFixtures/ConvSmallIntHelper", "", "ConvSmallIntHelper", "ConvSmallIntHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_ConvSmallIntHelper, 1u, nullptr, 0u, 0u },
	{ 0x0000002Cu, "SnapshotTestFixtures/ConvWideHelper", "SnapshotTestFixtures/ConvWideHelper", "", "ConvWideHelper", "ConvWideHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_ConvWideHelper, 1u, nullptr, 0u, 0u },
	{ 0x0000003Bu, "SnapshotTestFixtures/CpblkHelper", "SnapshotTestFixtures/CpblkHelper", "", "CpblkHelper", "CpblkHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_CpblkHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000037u, "SnapshotTestFixtures/CpobjHelper", "SnapshotTestFixtures/CpobjHelper", "", "CpobjHelper", "CpobjHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_CpobjHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000016u, "SnapshotTestFixtures/DelegateHelper", "SnapshotTestFixtures/DelegateHelper", "", "DelegateHelper", "DelegateHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_DelegateHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000018u, "SnapshotTestFixtures/DerivedClass", "SnapshotTestFixtures/DerivedClass", "", "DerivedClass", "DerivedClass", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_DerivedClass, 2u, nullptr, 0u, 0u },
	{ 0x00000066u, "SnapshotTestFixtures/EhCatchFinallyNestedHelper", "SnapshotTestFixtures/EhCatchFinallyNestedHelper", "", "EhCatchFinallyNestedHelper", "EhCatchFinallyNestedHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_EhCatchFinallyNestedHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000069u, "SnapshotTestFixtures/EhFaultFinallyHelper", "SnapshotTestFixtures/EhFaultFinallyHelper", "", "EhFaultFinallyHelper", "EhFaultFinallyHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_EhFaultFinallyHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000063u, "SnapshotTestFixtures/EhFaultHelper", "SnapshotTestFixtures/EhFaultHelper", "", "EhFaultHelper", "EhFaultHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_EhFaultHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000067u, "SnapshotTestFixtures/EhFilterFinallyHelper", "SnapshotTestFixtures/EhFilterFinallyHelper", "", "EhFilterFinallyHelper", "EhFilterFinallyHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_EhFilterFinallyHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000064u, "SnapshotTestFixtures/EhFilterHelper", "SnapshotTestFixtures/EhFilterHelper", "", "EhFilterHelper", "EhFilterHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_EhFilterHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000062u, "SnapshotTestFixtures/EhFinallyHelper", "SnapshotTestFixtures/EhFinallyHelper", "", "EhFinallyHelper", "EhFinallyHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_EhFinallyHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000013u, "SnapshotTestFixtures/EhHelper", "SnapshotTestFixtures/EhHelper", "", "EhHelper", "EhHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_EhHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000065u, "SnapshotTestFixtures/EhMultipleCatchHelper", "SnapshotTestFixtures/EhMultipleCatchHelper", "", "EhMultipleCatchHelper", "EhMultipleCatchHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_EhMultipleCatchHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000068u, "SnapshotTestFixtures/EhNestedTryCatchHelper", "SnapshotTestFixtures/EhNestedTryCatchHelper", "", "EhNestedTryCatchHelper", "EhNestedTryCatchHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_EhNestedTryCatchHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000055u, "SnapshotTestFixtures/ExternalCaller", "SnapshotTestFixtures/ExternalCaller", "", "ExternalCaller", "ExternalCaller", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_ExternalCaller, 1u, nullptr, 0u, 0u },
	{ 0x0000000Bu, "SnapshotTestFixtures/FieldHelper", "SnapshotTestFixtures/FieldHelper", "", "FieldHelper", "FieldHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_FieldHelper, 1u, nullptr, 0u, 0u },
	{ 0x0000002Bu, "SnapshotTestFixtures/FloatOpsHelper", "SnapshotTestFixtures/FloatOpsHelper", "", "FloatOpsHelper", "FloatOpsHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_FloatOpsHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000007u, "SnapshotTestFixtures/FlowControl", "SnapshotTestFixtures/FlowControl", "", "FlowControl", "FlowControl", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_FlowControl, 1u, nullptr, 0u, 0u },
	{ 0x00000014u, "SnapshotTestFixtures/GenericHelper", "SnapshotTestFixtures/GenericHelper", "", "GenericHelper", "GenericHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_GenericHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000059u, "SnapshotTestFixtures/GenericsVirtEhDemo", "SnapshotTestFixtures/GenericsVirtEhDemo", "", "GenericsVirtEhDemo", "GenericsVirtEhDemo", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_GenericsVirtEhDemo, 1u, nullptr, 0u, 0u },
	{ 0x00000032u, "SnapshotTestFixtures/HasFields", "SnapshotTestFixtures/HasFields", "", "HasFields", "HasFields", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_HasFields, 1u, nullptr, 0u, 0u },
	{ 0x00000020u, "SnapshotTestFixtures/HasInstanceFields", "SnapshotTestFixtures/HasInstanceFields", "", "HasInstanceFields", "HasInstanceFields", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_HasInstanceFields, 1u, nullptr, 0u, 0u },
	{ 0x0000000Eu, "SnapshotTestFixtures/Helper", "SnapshotTestFixtures/Helper", "", "Helper", "Helper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_Helper, 4u, nullptr, 0u, 0u },
	{ 0x0000005Cu, "SnapshotTestFixtures/HotUpdateWithGenericsDemo", "SnapshotTestFixtures/HotUpdateWithGenericsDemo", "", "HotUpdateWithGenericsDemo", "HotUpdateWithGenericsDemo", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_HotUpdateWithGenericsDemo, 1u, nullptr, 0u, 0u },
	{ 0x0000005Bu, "SnapshotTestFixtures/HotUpdateWithTypesDemo", "SnapshotTestFixtures/HotUpdateWithTypesDemo", "", "HotUpdateWithTypesDemo", "HotUpdateWithTypesDemo", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_HotUpdateWithTypesDemo, 1u, nullptr, 0u, 0u },
	{ 0x00000060u, "SnapshotTestFixtures/ImplHelper", "SnapshotTestFixtures/ImplHelper", "", "ImplHelper", "ImplHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_ImplHelper, 2u, nullptr, 0u, 0u },
	{ 0x0000001Du, "SnapshotTestFixtures/IndirectHelper", "SnapshotTestFixtures/IndirectHelper", "", "IndirectHelper", "IndirectHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_IndirectHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000021u, "SnapshotTestFixtures/InstanceFieldHelper", "SnapshotTestFixtures/InstanceFieldHelper", "", "InstanceFieldHelper", "InstanceFieldHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_InstanceFieldHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000008u, "SnapshotTestFixtures/InstanceHelper", "SnapshotTestFixtures/InstanceHelper", "", "InstanceHelper", "InstanceHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_InstanceHelper, 2u, nullptr, 0u, 0u },
	{ 0x00000061u, "SnapshotTestFixtures/InterfaceDevirtHelper", "SnapshotTestFixtures/InterfaceDevirtHelper", "", "InterfaceDevirtHelper", "InterfaceDevirtHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_InterfaceDevirtHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000057u, "SnapshotTestFixtures/InternalHelper", "SnapshotTestFixtures/InternalHelper", "", "InternalHelper", "InternalHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_InternalHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000036u, "SnapshotTestFixtures/LdargaHelper", "SnapshotTestFixtures/LdargaHelper", "", "LdargaHelper", "LdargaHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_LdargaHelper, 1u, nullptr, 0u, 0u },
	{ 0x0000002Eu, "SnapshotTestFixtures/LdcI8Helper", "SnapshotTestFixtures/LdcI8Helper", "", "LdcI8Helper", "LdcI8Helper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_LdcI8Helper, 1u, nullptr, 0u, 0u },
	{ 0x00000035u, "SnapshotTestFixtures/LdelemaHelper", "SnapshotTestFixtures/LdelemaHelper", "", "LdelemaHelper", "LdelemaHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_LdelemaHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000047u, "SnapshotTestFixtures/LdelemAllHelper", "SnapshotTestFixtures/LdelemAllHelper", "", "LdelemAllHelper", "LdelemAllHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_LdelemAllHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000010u, "SnapshotTestFixtures/LdftnHelper", "SnapshotTestFixtures/LdftnHelper", "", "LdftnHelper", "LdftnHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_LdftnHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000030u, "SnapshotTestFixtures/LdindI1Helper", "SnapshotTestFixtures/LdindI1Helper", "", "LdindI1Helper", "LdindI1Helper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_LdindI1Helper, 1u, nullptr, 0u, 0u },
	{ 0x0000003Du, "SnapshotTestFixtures/LdindI8Helper", "SnapshotTestFixtures/LdindI8Helper", "", "LdindI8Helper", "LdindI8Helper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_LdindI8Helper, 1u, nullptr, 0u, 0u },
	{ 0x00000053u, "SnapshotTestFixtures/LdindStindGapsHelper", "SnapshotTestFixtures/LdindStindGapsHelper", "", "LdindStindGapsHelper", "LdindStindGapsHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_LdindStindGapsHelper, 3u, nullptr, 0u, 0u },
	{ 0x0000003Cu, "SnapshotTestFixtures/LdindUnsignedHelper", "SnapshotTestFixtures/LdindUnsignedHelper", "", "LdindUnsignedHelper", "LdindUnsignedHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_LdindUnsignedHelper, 1u, nullptr, 0u, 0u },
	{ 0x0000003Eu, "SnapshotTestFixtures/LdindWideHelper", "SnapshotTestFixtures/LdindWideHelper", "", "LdindWideHelper", "LdindWideHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_LdindWideHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000034u, "SnapshotTestFixtures/LdtokenHelper", "SnapshotTestFixtures/LdtokenHelper", "", "LdtokenHelper", "LdtokenHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_LdtokenHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000046u, "SnapshotTestFixtures/LdVirtftnHelper", "SnapshotTestFixtures/LdVirtftnHelper", "", "LdVirtftnHelper", "LdVirtftnHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_LdVirtftnHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000028u, "SnapshotTestFixtures/LocalAllocHelper", "SnapshotTestFixtures/LocalAllocHelper", "", "LocalAllocHelper", "LocalAllocHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_LocalAllocHelper, 1u, nullptr, 0u, 0u },
	{ 0x0000000Cu, "SnapshotTestFixtures/LoopHelper", "SnapshotTestFixtures/LoopHelper", "", "LoopHelper", "LoopHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_LoopHelper, 1u, nullptr, 0u, 0u },
	{ 0x0000001Au, "SnapshotTestFixtures/MarkedClass", "SnapshotTestFixtures/MarkedClass", "", "MarkedClass", "MarkedClass", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_MarkedClass, 1u, nullptr, 0u, 0u },
	{ 0x00000006u, "SnapshotTestFixtures/MathHelper", "SnapshotTestFixtures/MathHelper", "", "MathHelper", "MathHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_MathHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000050u, "SnapshotTestFixtures/MoreRareOpsHelper", "SnapshotTestFixtures/MoreRareOpsHelper", "", "MoreRareOpsHelper", "MoreRareOpsHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_MoreRareOpsHelper, 5u, nullptr, 0u, 0u },
	{ 0x00000029u, "SnapshotTestFixtures/MulDivHelper", "SnapshotTestFixtures/MulDivHelper", "", "MulDivHelper", "MulDivHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_MulDivHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000056u, "SnapshotTestFixtures/MultiDispatcher", "SnapshotTestFixtures/MultiDispatcher", "", "MultiDispatcher", "MultiDispatcher", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_MultiDispatcher, 1u, nullptr, 0u, 0u },
	{ 0x00000045u, "SnapshotTestFixtures/MyClass", "SnapshotTestFixtures/MyClass", "", "MyClass", "MyClass", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_MyClass, 2u, nullptr, 0u, 0u },
	{ 0x0000002Du, "SnapshotTestFixtures/NegShiftHelper", "SnapshotTestFixtures/NegShiftHelper", "", "NegShiftHelper", "NegShiftHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_NegShiftHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000051u, "SnapshotTestFixtures/OverflowConvExtHelper", "SnapshotTestFixtures/OverflowConvExtHelper", "", "OverflowConvExtHelper", "OverflowConvExtHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_OverflowConvExtHelper, 11u, nullptr, 0u, 0u },
	{ 0x00000043u, "SnapshotTestFixtures/OverflowConvHelper", "SnapshotTestFixtures/OverflowConvHelper", "", "OverflowConvHelper", "OverflowConvHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_OverflowConvHelper, 7u, nullptr, 0u, 0u },
	{ 0x00000042u, "SnapshotTestFixtures/OverflowHelper", "SnapshotTestFixtures/OverflowHelper", "", "OverflowHelper", "OverflowHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_OverflowHelper, 3u, nullptr, 0u, 0u },
	{ 0x0000004Fu, "SnapshotTestFixtures/OverflowUnHelper", "SnapshotTestFixtures/OverflowUnHelper", "", "OverflowUnHelper", "OverflowUnHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_OverflowUnHelper, 3u, nullptr, 0u, 0u },
	{ 0x0000004Au, "SnapshotTestFixtures/RareOpsHelper", "SnapshotTestFixtures/RareOpsHelper", "", "RareOpsHelper", "RareOpsHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_RareOpsHelper, 3u, nullptr, 0u, 0u },
	{ 0x0000004Eu, "SnapshotTestFixtures/RethrowHelper", "SnapshotTestFixtures/RethrowHelper", "", "RethrowHelper", "RethrowHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_RethrowHelper, 1u, nullptr, 0u, 0u },
	{ 0x0000005Eu, "SnapshotTestFixtures/SealedClassVirtualHelper", "SnapshotTestFixtures/SealedClassVirtualHelper", "", "SealedClassVirtualHelper", "SealedClassVirtualHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_SealedClassVirtualHelper, 1u, nullptr, 0u, 0u },
	{ 0x0000005Du, "SnapshotTestFixtures/SealedHelper", "SnapshotTestFixtures/SealedHelper", "", "SealedHelper", "SealedHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_SealedHelper, 2u, nullptr, 0u, 0u },
	{ 0x00000025u, "SnapshotTestFixtures/ShiftHelper", "SnapshotTestFixtures/ShiftHelper", "", "ShiftHelper", "ShiftHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_ShiftHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000052u, "SnapshotTestFixtures/SimpleGapsHelper", "SnapshotTestFixtures/SimpleGapsHelper", "", "SimpleGapsHelper", "SimpleGapsHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_SimpleGapsHelper, 7u, nullptr, 0u, 0u },
	{ 0x0000001Cu, "SnapshotTestFixtures/SizeOfHelper", "SnapshotTestFixtures/SizeOfHelper", "", "SizeOfHelper", "SizeOfHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_SizeOfHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000022u, "SnapshotTestFixtures/StaticFieldWriteHelper", "SnapshotTestFixtures/StaticFieldWriteHelper", "", "StaticFieldWriteHelper", "StaticFieldWriteHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_StaticFieldWriteHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000048u, "SnapshotTestFixtures/StelemAllHelper", "SnapshotTestFixtures/StelemAllHelper", "", "StelemAllHelper", "StelemAllHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_StelemAllHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000038u, "SnapshotTestFixtures/StindNarrowHelper", "SnapshotTestFixtures/StindNarrowHelper", "", "StindNarrowHelper", "StindNarrowHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_StindNarrowHelper, 1u, nullptr, 0u, 0u },
	{ 0x0000003Fu, "SnapshotTestFixtures/StindWideHelper", "SnapshotTestFixtures/StindWideHelper", "", "StindWideHelper", "StindWideHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_StindWideHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000058u, "SnapshotTestFixtures/StringConcatHelper", "SnapshotTestFixtures/StringConcatHelper", "", "StringConcatHelper", "StringConcatHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_StringConcatHelper, 1u, nullptr, 0u, 0u },
	{ 0x0000004Bu, "SnapshotTestFixtures/StringFormatHelper", "SnapshotTestFixtures/StringFormatHelper", "", "StringFormatHelper", "StringFormatHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_StringFormatHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000004u, "SnapshotTestFixtures/StringOps", "SnapshotTestFixtures/StringOps", "", "StringOps", "StringOps", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_StringOps, 1u, nullptr, 0u, 0u },
	{ 0x0000000Fu, "SnapshotTestFixtures/SwitchHelper", "SnapshotTestFixtures/SwitchHelper", "", "SwitchHelper", "SwitchHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_SwitchHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000011u, "SnapshotTestFixtures/TargetHelper", "SnapshotTestFixtures/TargetHelper", "", "TargetHelper", "TargetHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_TargetHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000041u, "SnapshotTestFixtures/ThrowHelper", "SnapshotTestFixtures/ThrowHelper", "", "ThrowHelper", "ThrowHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_ThrowHelper, 1u, nullptr, 0u, 0u },
	{ 0x0000001Bu, "SnapshotTestFixtures/TypeCheckHelper", "SnapshotTestFixtures/TypeCheckHelper", "", "TypeCheckHelper", "TypeCheckHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_TypeCheckHelper, 1u, nullptr, 0u, 0u },
	{ 0x0000003Au, "SnapshotTestFixtures/UnboxHelper", "SnapshotTestFixtures/UnboxHelper", "", "UnboxHelper", "UnboxHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_UnboxHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000049u, "SnapshotTestFixtures/UnsignedOpsHelper", "SnapshotTestFixtures/UnsignedOpsHelper", "", "UnsignedOpsHelper", "UnsignedOpsHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_UnsignedOpsHelper, 4u, nullptr, 0u, 0u },
	{ 0x00000027u, "SnapshotTestFixtures/ValueTypeHelper", "SnapshotTestFixtures/ValueTypeHelper", "", "ValueTypeHelper", "ValueTypeHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_ValueTypeHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000023u, "SnapshotTestFixtures/VirtualDispatchHelper", "SnapshotTestFixtures/VirtualDispatchHelper", "", "VirtualDispatchHelper", "VirtualDispatchHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_VirtualDispatchHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000019u, "SnapshotTestFixtures/VirtualHelper", "SnapshotTestFixtures/VirtualHelper", "", "VirtualHelper", "VirtualHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_VirtualHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000005u, "SnapshotTestFixtures/VoidCaller", "SnapshotTestFixtures/VoidCaller", "", "VoidCaller", "VoidCaller", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_VoidCaller, 1u, nullptr, 0u, 0u },
	{ 0x00000015u, "SnapshotTestFixtures/Wrapper`1", "SnapshotTestFixtures/Wrapper`1", "", "Wrapper`1", "Wrapper`1", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_Wrapper_1, 2u, nullptr, 0u, 0u },
	{ 0x0000006Bu, "SnapshotTestFixtures/Wrapper<!0>", "SnapshotTestFixtures/Wrapper<!0>", "", "Wrapper<!0>", "Wrapper<!0>", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_Wrapper__0_, 2u, nullptr, 0u, 0u },
	{ 0x0000006Au, "SnapshotTestFixtures/Wrapper<System.Int32>", "SnapshotTestFixtures/Wrapper<System.Int32>", "Wrapper<System", "Wrapper<System.Int32>", "Wrapper<System.Int32>", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_Wrapper_System_Int32_, 2u, nullptr, 0u, 0u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[105] = {
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
	&kReflTypes[97],
	&kReflTypes[98],
	&kReflTypes[99],
	&kReflTypes[100],
	&kReflTypes[101],
	&kReflTypes[102],
	&kReflTypes[103],
	&kReflTypes[104],
};

static const ReflectionQueryImageDescriptor kReflImage = { "SnapshotProverSubjects", kReflTypePtrs, 105u, 1, 0, 0, 0 };

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
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};


	CHAOS_EH_TRY
			{
				auto& _d12 = s_hotpatch_entries[12];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d12)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d12))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d12.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = SnapshotTestFixtures_ArithmeticOps_RunAdd();
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
			{
				auto& _d12 = s_hotpatch_entries[12];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d12)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d12))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d12.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = SnapshotTestFixtures_ArithmeticOps_RunAdd();
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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
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
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};


	CHAOS_EH_TRY
			{
				auto& _d158 = s_hotpatch_entries[158];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d158)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d158))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d158.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = SnapshotTestFixtures_VoidCaller_DoNothing();
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
			{
				auto& _d158 = s_hotpatch_entries[158];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d158)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d158))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d158.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = SnapshotTestFixtures_VoidCaller_DoNothing();
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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
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
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};


	CHAOS_EH_TRY
			{
				auto& _d88 = s_hotpatch_entries[88];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d88)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d88))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d88.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = SnapshotTestFixtures_MathHelper_RunSquare();
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
			{
				auto& _d88 = s_hotpatch_entries[88];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d88)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d88))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d88.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = SnapshotTestFixtures_MathHelper_RunSquare();
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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
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
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};


	CHAOS_EH_TRY
			{
				auto& _d52 = s_hotpatch_entries[52];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d52)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d52))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d52.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = SnapshotTestFixtures_FlowControl_IsPositive();
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
			{
				auto& _d52 = s_hotpatch_entries[52];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d52)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d52))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d52.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = SnapshotTestFixtures_FlowControl_IsPositive();
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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
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
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};


	CHAOS_EH_TRY
			{
				auto& _d27 = s_hotpatch_entries[27];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d27)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d27))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d27.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = SnapshotTestFixtures_CallVirtHelper_CreateAndUse();
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
			{
				auto& _d27 = s_hotpatch_entries[27];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d27)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d27))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d27.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = SnapshotTestFixtures_CallVirtHelper_CreateAndUse();
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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
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
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};


	CHAOS_EH_TRY
			{
				auto& _d19 = s_hotpatch_entries[19];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d19)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d19))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d19.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = SnapshotTestFixtures_BoxingHelper_BoxAndUnbox();
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
			{
				auto& _d19 = s_hotpatch_entries[19];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d19)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d19))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d19.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = SnapshotTestFixtures_BoxingHelper_BoxAndUnbox();
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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
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
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};


	CHAOS_EH_TRY
			{
				auto& _d32 = s_hotpatch_entries[32];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d32)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d32))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d32.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = SnapshotTestFixtures_ConversionHelper_ConvertToInt();
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
			{
				auto& _d32 = s_hotpatch_entries[32];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d32)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d32))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d32.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = SnapshotTestFixtures_ConversionHelper_ConvertToInt();
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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
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
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};


	CHAOS_EH_TRY
			{
				auto& _d86 = s_hotpatch_entries[86];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d86)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d86))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d86.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = SnapshotTestFixtures_LoopHelper_SumToFive();
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
			{
				auto& _d86 = s_hotpatch_entries[86];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d86)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d86))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d86.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = SnapshotTestFixtures_LoopHelper_SumToFive();
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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SnapshotProverSubjects/SnapshotProverSubjects::CustomEntrySubject_8()
extern "C" void SnapshotProverSubjects_SnapshotProverSubjects_CustomEntrySubject_8(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


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
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};


	CHAOS_EH_TRY
			{
				auto& _d54 = s_hotpatch_entries[54];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d54)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d54))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d54.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = SnapshotTestFixtures_GenericsVirtEhDemo_DemoCombine();
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
			{
				auto& _d54 = s_hotpatch_entries[54];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d54)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d54))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d54.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = SnapshotTestFixtures_GenericsVirtEhDemo_DemoCombine();
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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
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
				chaos_static_SnapshotProverSubjects_SnapshotProverSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SnapshotTestFixtures/AddressHelper::RunAddress()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_AddressHelper_RunAddress(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_SnapshotTestFixtures_HasFields, {});
		chaos_object->header.type_info = &chaos_mt_SnapshotTestFixtures_HasFields.hot;
		SnapshotTestFixtures_HasFields__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_SnapshotTestFixtures_HasFields*>(_s0);
		_s0 = chaos_object->field_SnapshotTestFixtures_HasFields__instanceVal;
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/ArithmeticCompareHelper::RunCompare()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_ArithmeticCompareHelper_RunCompare(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 9) chaos_locals{};
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
	CHAOS_IL2CPP_INTPTR _s34{};
	CHAOS_IL2CPP_INTPTR _s35{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
	chaos_locals[0] = _s0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[0];
	_s1 = chaos_locals[1];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapSub(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	chaos_locals[2] = _s0;
	_s0 = chaos_locals[0];
	_s1 = chaos_locals[1];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosRem(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	chaos_locals[3] = _s0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[4] = _s0;
	_s0 = chaos_locals[0];
	_s1 = chaos_locals[1];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
		{
			_s0 = chaos_locals[4];
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
			chaos_locals[4] = _s0;
		}
		_s6 = chaos_locals[0];
		_s7 = chaos_locals[1];
		_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s6) < static_cast<CHAOS_IL2CPP_INT32>(_s7) ? 1 : 0);
		{
			if (_s6 != 0)
			{
				_s6 = chaos_locals[4];
				_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
				_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s6), static_cast<CHAOS_IL2CPP_INT32>(_s7)));
				chaos_locals[4] = _s6;
			}
			_s2 = chaos_locals[0];
			_s3 = chaos_locals[1];
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s2) > static_cast<CHAOS_IL2CPP_INT32>(_s3) ? 1 : 0);
			{
				if (_s2 != 0)
				{
					_s2 = chaos_locals[4];
					_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
					_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s2), static_cast<CHAOS_IL2CPP_INT32>(_s3)));
					chaos_locals[4] = _s2;
				}
				_s4 = chaos_locals[2];
				_s5 = chaos_locals[3];
				_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s4), static_cast<CHAOS_IL2CPP_INT32>(_s5)));
				_s5 = chaos_locals[4];
				_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s4), static_cast<CHAOS_IL2CPP_INT32>(_s5)));
				chaos_locals[8] = _s4;
				_s4 = chaos_locals[8];
				return static_cast<CHAOS_IL2CPP_INT32>(_s4);
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
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
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
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s1);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s0);
		const auto chaos_element = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];
		_s0 = chaos_element;
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/ArrayLengthHelper::RunLength()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_ArrayLengthHelper_RunLength(void)
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
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s0 = [&](){ auto* _c_arr = reinterpret_cast<chaos_managed_array*>(_s0); return _c_arr ? static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_c_arr->length)) : CHAOS_IL2CPP_INTPTR{}; }();
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/ArrayRefHelper::RunArrayRef()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_ArrayRefHelper_RunArrayRef(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(10);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
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
		ChaosObjectCtor(chaos_arg_0);
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
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 7) chaos_locals{};
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(165);
	chaos_locals[0] = _s0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(90);
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[0];
	_s1 = chaos_locals[1];
	_s0 = chaos_store_uint64(chaos_load_uint64(_s0) & chaos_load_uint64(_s1));
	chaos_locals[2] = _s0;
	_s0 = chaos_locals[0];
	_s1 = chaos_locals[1];
	_s0 = chaos_store_uint64(chaos_load_uint64(_s0) | chaos_load_uint64(_s1));
	chaos_locals[3] = _s0;
	_s0 = chaos_locals[0];
	_s1 = chaos_locals[1];
	_s0 = chaos_store_uint64(chaos_load_uint64(_s0) ^ chaos_load_uint64(_s1));
	chaos_locals[4] = _s0;
	_s0 = chaos_locals[0];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(~static_cast<CHAOS_IL2CPP_INT32>(_s0));
	chaos_locals[5] = _s0;
	_s0 = chaos_locals[2];
	_s1 = chaos_locals[3];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	_s1 = chaos_locals[4];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	_s1 = chaos_locals[5];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	chaos_locals[6] = _s0;
	_s0 = chaos_locals[6];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/BoxingHelper::BoxAndUnbox()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_BoxingHelper_BoxAndUnbox(void)
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
	{
		const auto chaos_value = _s0;
		auto* chaos_boxed = CHAOS_IL2CPP_NEW_GC(chaos_boxed_type_System_Private_CoreLib_System_Int32, {});
		chaos_boxed->header.type_info = &chaos_mt_System_Private_CoreLib_System_Int32.hot;
		chaos_boxed->value = chaos_value;
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_boxed);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		auto* chaos_boxed = reinterpret_cast<chaos_boxed_type_System_Private_CoreLib_System_Int32*>(_s0);
		if (chaos_boxed == nullptr)
		{
			CHAOS_IL2CPP_FAIL();
		}
		_s0 = chaos_boxed->value;
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/BoxInterfaceArrayDemo::DemoBoxStore()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_BoxInterfaceArrayDemo_DemoBoxStore(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(47);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/BranchCompareBHelper::RunBranchCompareB()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_BranchCompareBHelper_RunBranchCompareB(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 8) chaos_locals{};
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
	CHAOS_IL2CPP_INTPTR _s34{};
	CHAOS_IL2CPP_INTPTR _s35{};
	CHAOS_IL2CPP_INTPTR _s36{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
	chaos_locals[0] = _s0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
	chaos_locals[1] = _s0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
	chaos_locals[2] = _s0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[3] = _s0;
	_s0 = chaos_locals[0];
	_s1 = chaos_locals[1];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0) > static_cast<CHAOS_IL2CPP_INT32>(_s1) ? 1 : 0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
		{
			_s0 = chaos_locals[3];
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
			chaos_locals[3] = _s0;
		}
		_s4 = chaos_locals[2];
		_s5 = chaos_locals[0];
		_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s4) < static_cast<CHAOS_IL2CPP_INT32>(_s5) ? 1 : 0);
		_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s4) == static_cast<CHAOS_IL2CPP_INTPTR>(_s5) ? 1 : 0);
		{
			if (_s4 != 0)
			{
				_s4 = chaos_locals[3];
				_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
				_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s4), static_cast<CHAOS_IL2CPP_INT32>(_s5)));
				chaos_locals[3] = _s4;
			}
			_s4 = chaos_locals[1];
			_s4 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s4));
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s5 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s5));
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s4) < static_cast<CHAOS_IL2CPP_INT32>(_s5) ? 1 : 0);
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s4) == static_cast<CHAOS_IL2CPP_INTPTR>(_s5) ? 1 : 0);
			{
				if (_s4 != 0)
				{
					_s4 = chaos_locals[3];
					_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
					_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s4), static_cast<CHAOS_IL2CPP_INT32>(_s5)));
					chaos_locals[3] = _s4;
				}
				_s10 = chaos_locals[3];
				chaos_locals[7] = _s10;
				_s10 = chaos_locals[7];
				return static_cast<CHAOS_IL2CPP_INT32>(_s10);
			}
		}
	}
}

// Managed method: SnapshotTestFixtures/BranchCompareHelper::RunBranchCompare()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_BranchCompareHelper_RunBranchCompare(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 9) chaos_locals{};
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
	CHAOS_IL2CPP_INTPTR _s34{};
	CHAOS_IL2CPP_INTPTR _s35{};


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
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
		{
			_s0 = chaos_locals[3];
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
			chaos_locals[3] = _s0;
		}
		_s2 = chaos_locals[0];
		_s3 = chaos_locals[1];
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s2) > static_cast<CHAOS_IL2CPP_INT32>(_s3) ? 1 : 0);
		{
			if (_s2 != 0)
			{
				_s2 = chaos_locals[3];
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
				_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s2), static_cast<CHAOS_IL2CPP_INT32>(_s3)));
				chaos_locals[3] = _s2;
			}
			_s2 = chaos_locals[1];
			_s3 = chaos_locals[0];
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s2) < static_cast<CHAOS_IL2CPP_INT32>(_s3) ? 1 : 0);
			{
				if (_s2 != 0)
				{
					_s2 = chaos_locals[3];
					_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
					_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s2), static_cast<CHAOS_IL2CPP_INT32>(_s3)));
					chaos_locals[3] = _s2;
				}
				_s2 = chaos_locals[0];
				_s3 = chaos_locals[1];
				_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
				_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
				{
					if (_s2 != 0)
					{
						_s2 = chaos_locals[3];
						_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(8);
						_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s2), static_cast<CHAOS_IL2CPP_INT32>(_s3)));
						chaos_locals[3] = _s2;
					}
					_s6 = chaos_locals[3];
					chaos_locals[8] = _s6;
					_s6 = chaos_locals[8];
					return static_cast<CHAOS_IL2CPP_INT32>(_s6);
				}
			}
		}
	}
}

// Managed method: SnapshotTestFixtures/BranchDupHelper::RunBranchDup()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_BranchDupHelper_RunBranchDup(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 6) chaos_locals{};
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	chaos_locals[0] = _s0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			chaos_locals[1] = _s0;
		}
		_s2 = chaos_locals[1];
		_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		{
			const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s3);
			const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s2);
			if (chaos_left == chaos_right)
			{
				_s2 = chaos_locals[1];
				{
					const auto chaos_value = _s2;
					auto* chaos_boxed = CHAOS_IL2CPP_NEW_GC(chaos_boxed_type_System_Private_CoreLib_System_Int32, {});
					chaos_boxed->header.type_info = &chaos_mt_System_Private_CoreLib_System_Int32.hot;
					chaos_boxed->value = chaos_value;
					_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_boxed);
				}
			}
			else
			{
				_s2 = 0;
			}
			_s2 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s2) ? 1 : 0);
			{
				if (_s1 != 0)
				{
					_s1 = chaos_locals[1];
					_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
					_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s1), static_cast<CHAOS_IL2CPP_INT32>(_s2)));
					chaos_locals[1] = _s1;
				}
				_s3 = chaos_locals[1];
				chaos_locals[5] = _s3;
				_s3 = chaos_locals[5];
				return static_cast<CHAOS_IL2CPP_INT32>(_s3);
			}
		}
	}
}

// Managed method: SnapshotTestFixtures/BranchUnsignedBHelper::RunBranchUnsignedB()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_BranchUnsignedBHelper_RunBranchUnsignedB(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
	chaos_locals[0] = _s0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[0];
	_s1 = chaos_locals[1];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapMul(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	chaos_locals[2] = _s0;
	_s0 = chaos_locals[2];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/BranchUnsignedHelper::RunBranchUnsigned()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_BranchUnsignedHelper_RunBranchUnsigned(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	chaos_locals[0] = _s0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[0];
	_s1 = chaos_locals[1];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	chaos_locals[2] = _s0;
	_s0 = chaos_locals[2];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/CalliHelper::RunCalli(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_CalliHelper_RunCalli_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
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
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = 0;
	{
		// Hotpatch-aware ldftn wrapper (slot 60)
		static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INT32 chaos_fn_arg_0) -> CHAOS_IL2CPP_INT32 {
			auto& _d_entry = s_hotpatch_entries[60];
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
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
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
			CHAOS_IL2CPP_INT32 __chaos_hotpatch_result{};
			bool __chaos_hotpatch_taken = false;
			uint64_t __chaos_args_buf[1] = { (uint64_t)chaos_arg_0 };
			uint64_t __chaos_ret_buf[2] = {};
			if (chaos_delegate->chaos_delegate_method_token != 0)
			{
				__chaos_hotpatch_taken = ::chaos::il2cpp::runtime_core::DelegateHotpatchCheckpoint(
					chaos_delegate->chaos_delegate_method_token,
					__chaos_args_buf, __chaos_ret_buf, 1);
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
					const auto chaos_open_function = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)>(chaos_delegate->chaos_delegate_method_ptr);
					chaos_result = chaos_open_function(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
				}
				else
				{
					const auto chaos_closed_function = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR chaos_delegate_target, CHAOS_IL2CPP_INT32 chaos_fn_arg_0)>(chaos_delegate->chaos_delegate_method_ptr);
					chaos_result = chaos_closed_function(chaos_delegate->chaos_delegate_target, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
				}
				_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
		}
	}
	chaos_locals[1] = _s2;
	_s2 = chaos_locals[1];
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
	CHAOS_IL2CPP_INTPTR _s3{};


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
		auto& _d68 = s_hotpatch_entries[68];
		CHAOS_IL2CPP_INT32 _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d68)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d68))
		{
			alignas(16) uint8_t _d_ab[8];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d68.method_key, _d_ab, &_d_hpresult);
		}
		else
		{
			_d_hpresult = SnapshotTestFixtures_InstanceHelper_GetValue(chaos_arg_0);
		}
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
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
		auto& _d68 = s_hotpatch_entries[68];
		CHAOS_IL2CPP_INT32 _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d68)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d68))
		{
			alignas(16) uint8_t _d_ab[8];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d68.method_key, _d_ab, &_d_hpresult);
		}
		else
		{
			_d_hpresult = SnapshotTestFixtures_InstanceHelper_GetValue(chaos_arg_0);
		}
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// AOT-unreachable stub: SnapshotTestFixtures/CollectionsHelper::TestDict:System.Int32()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_CollectionsHelper_TestDict(void)
{
	return {};
}

// Managed method: SnapshotTestFixtures/CollectionsHelper::TestList()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_CollectionsHelper_TestList(void)
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
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Collections_System_Collections_Generic_List_System_Int32_, {});
		chaos_object->header.type_info = &chaos_mt_System_Collections_System_Collections_Generic_List_System_Int32_.hot;
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		chaos_external_runtime_System_Private_CoreLib_System_Collections_Generic_List_System_Int32___Add_System_Void_System_Int32_(chaos_arg_0, chaos_arg_1);
	}
	_s0 = chaos_locals[0];
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[13])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[1] = _s1;
	_s1 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s1);
}

// Managed method: SnapshotTestFixtures/CollectionsHelper::TestSet()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_CollectionsHelper_TestSet(void)
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


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Collections_System_Collections_Generic_HashSet_System_Int32_, {});
		chaos_object->header.type_info = &chaos_mt_System_Collections_System_Collections_Generic_HashSet_System_Int32_.hot;
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(10);
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Collections_Generic_HashSet_System_Int32___Add_System_Boolean_System_Int32_(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s0 = chaos_locals[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(10);
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Collections_Generic_HashSet_System_Int32___Contains_System_Boolean_System_Int32_(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[0];
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
	_s0 = chaos_locals[1];
	{
		if (_s0 != 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		chaos_locals[2] = _s5;
		_s5 = chaos_locals[2];
		return static_cast<CHAOS_IL2CPP_INT32>(_s5);
	}
}

// Managed method: SnapshotTestFixtures/ConversionHelper::ConvertToInt()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_ConversionHelper_ConvertToInt(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(100);
	_s0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	chaos_locals[2] = _s0;
	_s0 = chaos_locals[2];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/ConvSmallIntHelper::RunConvSmall()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_ConvSmallIntHelper_RunConvSmall(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 6) chaos_locals{};
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(4660);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT8>(_s0));
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[0];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT16>(_s0));
	chaos_locals[2] = _s0;
	_s0 = chaos_locals[0];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT8>(_s0));
	chaos_locals[3] = _s0;
	_s0 = chaos_locals[0];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT16>(_s0));
	chaos_locals[4] = _s0;
	_s0 = chaos_locals[1];
	_s1 = chaos_locals[2];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	_s1 = chaos_locals[3];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	_s1 = chaos_locals[4];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	chaos_locals[5] = _s0;
	_s0 = chaos_locals[5];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/ConvWideHelper::RunConvWide()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_ConvWideHelper_RunConvWide(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[1] = _s0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(100);
	_s0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[2] = _s0;
	_s0 = chaos_locals[2];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	chaos_locals[3] = _s0;
	_s0 = chaos_locals[3];
	chaos_locals[4] = _s0;
	_s0 = chaos_locals[4];
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
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};


	_s0 = 0;
	{
		// Hotpatch-aware ldftn wrapper (slot 58)
		static auto* chaos_ftn_thunk = +[](void) -> CHAOS_IL2CPP_INT32 {
			auto& _d_entry = s_hotpatch_entries[58];
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
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
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
	chaos_locals[1] = _s2;
	_s2 = chaos_locals[1];
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
		auto& _d16 = s_hotpatch_entries[16];
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d16)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d16))
		{
			alignas(16) uint8_t _d_ab[8];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d16.method_key, _d_ab, nullptr);
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

// Managed method: SnapshotTestFixtures/EhCatchFinallyNestedHelper::CatchFinallyNested(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_EhCatchFinallyNestedHelper_CatchFinallyNested_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
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

	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[0] = _s0;
		auto _chaos_finally = [&]()
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-2);
			chaos_locals[0] = _s0;
		};
	CHAOS_EH_TRY_FINALLY
			CHAOS_EH_TRY
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(100);
					_s1 = chaos_args[0];
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosDiv(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
					chaos_locals[0] = _s0;
			CHAOS_EH_CATCH_BEGIN
				if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
				auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
				if (chaos_header != nullptr)
				{
					if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_DivideByZeroException.hot))
					{ CHAOS_EH_RETHROW; }
				}
				_s0 = CHAOS_EH_EXCEPTION_OBJ;
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1);
					chaos_locals[0] = _s0;
			CHAOS_EH_END
	CHAOS_EH_FINALLY_END
	_s0 = chaos_locals[0];
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/EhFaultFinallyHelper::FaultFinally(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_EhFaultFinallyHelper_FaultFinally_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
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

	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[0] = _s0;
		auto _chaos_finally = [&]()
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-2);
			chaos_locals[0] = _s0;
		};
	CHAOS_EH_TRY_FINALLY
				auto _chaos_finally = [&]()
				{
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-3);
					chaos_locals[0] = _s0;
				};
			CHAOS_EH_TRY_FINALLY
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(100);
					_s1 = chaos_args[0];
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosDiv(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
					chaos_locals[0] = _s0;
			CHAOS_EH_FINALLY_END
	CHAOS_EH_FINALLY_END
	_s0 = chaos_locals[0];
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/EhFaultHelper::TryFault(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_EhFaultHelper_TryFault_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
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
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[0] = _s0;
		auto _chaos_finally = [&]()
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1);
			chaos_locals[0] = _s0;
		};
	CHAOS_EH_TRY_FINALLY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(100);
			_s1 = chaos_args[0];
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosDiv(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
			chaos_locals[0] = _s0;
	CHAOS_EH_FINALLY_END
	_s0 = chaos_locals[0];
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/EhFilterFinallyHelper::FilterFinally(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_EhFilterFinallyHelper_FilterFinally_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
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
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[0] = _s0;
		auto _chaos_finally = [&]()
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-2);
			chaos_locals[0] = _s0;
		};
	CHAOS_EH_TRY_FINALLY
			CHAOS_EH_TRY
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(100);
					_s1 = chaos_args[0];
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosDiv(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
					chaos_locals[1] = _s0;
			CHAOS_EH_CATCH_BEGIN
				_s0 = CHAOS_EH_EXCEPTION_OBJ;
				{
					const auto chaos_value = _s0;
					auto chaos_matches = false;
					if (chaos_value != 0)
					{
						auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_value);
						chaos_matches = chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Exception.hot);
					}
					_s0 = chaos_matches ? chaos_value : 0;
				}
				_s1 = _s0;
				{
					if (_s1 != 0)
					{
						chaos_locals[2] = _s0;
						_s0 = chaos_locals[2];
						_s1 = 0;
						_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s0) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) ? 1 : 0);
						chaos_locals[3] = _s0;
						_s0 = chaos_locals[3];
						_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
						_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s0) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) ? 1 : 0);
					}
					else
					{
						_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
					}
				}
				if (_s1 == 0)
				{
					CHAOS_EH_RETHROW;
				}
				_s1 = CHAOS_EH_EXCEPTION_OBJ;
					_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(-1);
					chaos_locals[0] = _s1;
			CHAOS_EH_END
	CHAOS_EH_FINALLY_END
	_s1 = chaos_locals[0];
	chaos_locals[1] = _s1;
	_s1 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s1);
}

// Managed method: SnapshotTestFixtures/EhFilterHelper::TryFilter(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_EhFilterHelper_TryFilter_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
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
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(100);
			_s1 = chaos_args[0];
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosDiv(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
			chaos_locals[0] = _s0;
	CHAOS_EH_CATCH_BEGIN
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
		{
			const auto chaos_value = _s0;
			auto chaos_matches = false;
			if (chaos_value != 0)
			{
				auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_value);
				chaos_matches = chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Exception.hot);
			}
			_s0 = chaos_matches ? chaos_value : 0;
		}
		_s1 = _s0;
		{
			if (_s1 != 0)
			{
				chaos_locals[1] = _s0;
				_s0 = chaos_locals[1];
				_s1 = 0;
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s0) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) ? 1 : 0);
				chaos_locals[2] = _s0;
				_s0 = chaos_locals[2];
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s0) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) ? 1 : 0);
			}
			else
			{
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			}
		}
		if (_s1 == 0)
		{
			CHAOS_EH_RETHROW;
		}
		_s1 = CHAOS_EH_EXCEPTION_OBJ;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(-1);
			chaos_locals[0] = _s1;
	CHAOS_EH_END
	_s1 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s1);
}

// Managed method: SnapshotTestFixtures/EhFinallyHelper::TryFinally_Only(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_EhFinallyHelper_TryFinally_Only_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
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
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[0] = _s0;
		auto _chaos_finally = [&]()
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1);
			chaos_locals[0] = _s0;
		};
	CHAOS_EH_TRY_FINALLY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(100);
			_s1 = chaos_args[0];
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosDiv(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
			chaos_locals[0] = _s0;
	CHAOS_EH_FINALLY_END
	_s0 = chaos_locals[0];
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/EhHelper::SafeDivide()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_EhHelper_SafeDivide(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	chaos_locals[0] = _s0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[1] = _s0;
	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(100);
			_s1 = chaos_locals[1];
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosDiv(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
			chaos_locals[0] = _s0;
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1);
			chaos_locals[0] = _s0;
	CHAOS_EH_END
	_s0 = chaos_locals[0];
	chaos_locals[2] = _s0;
	_s0 = chaos_locals[2];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/EhMultipleCatchHelper::MultipleCatch(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_EhMultipleCatchHelper_MultipleCatch_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
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

	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(100);
			_s1 = chaos_args[0];
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosDiv(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
			chaos_locals[0] = _s0;
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_DivideByZeroException.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1);
			chaos_locals[0] = _s0;
	CHAOS_EH_END
	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(100);
			_s1 = chaos_args[0];
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosDiv(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
			chaos_locals[0] = _s0;
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Exception.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-2);
			chaos_locals[0] = _s0;
	CHAOS_EH_END
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/EhNestedTryCatchHelper::NestedTryCatch(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_EhNestedTryCatchHelper_NestedTryCatch_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(100);
			_s1 = chaos_args[0];
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosDiv(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
			chaos_locals[0] = _s0;
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_DivideByZeroException.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1);
			chaos_locals[0] = _s0;
	CHAOS_EH_END
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
	CHAOS_IL2CPP_INTPTR _s4{};


	_s0 = chaos_static_SnapshotTestFixtures_FieldHelper___counter;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	{
		auto chaos_value = _s0;
		chaos_static_SnapshotTestFixtures_FieldHelper___counter = chaos_value;
	}
	_s0 = chaos_static_SnapshotTestFixtures_FieldHelper___counter;
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/FloatOpsHelper::RunFloatOps()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_FloatOpsHelper_RunFloatOps(void)
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


	_s0 = ChaosStoreFloat32(3.5f);
	chaos_locals[0] = _s0;
	_s0 = ChaosStoreFloat64(7.2);
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	_s0 = ChaosStoreFloat32(static_cast<float>(_s0));
	chaos_locals[2] = _s0;
	_s0 = chaos_locals[0];
	_s0 = ChaosStoreFloat64(static_cast<double>(_s0));
	chaos_locals[3] = _s0;
	_s0 = chaos_locals[0];
	_s1 = chaos_locals[2];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(ChaosLoadFloat64(_s0), ChaosLoadFloat64(_s1)));
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	chaos_locals[4] = _s0;
	_s0 = chaos_locals[4];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/FlowControl::IsPositive()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_FlowControl_IsPositive(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		if (_s0 != 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		chaos_locals[1] = _s0;
		_s0 = chaos_locals[1];
		return static_cast<CHAOS_IL2CPP_INT32>(_s0);
	}
}

// Managed method: SnapshotTestFixtures/GenericHelper::UseGeneric()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_GenericHelper_UseGeneric(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		const auto chaos_arg_1 = _s0;
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_SnapshotTestFixtures_Wrapper_System_Int32_, {});
		chaos_object->header.type_info = &chaos_mt_SnapshotTestFixtures_Wrapper_System_Int32_.hot;
		chaos_stub_definition_SnapshotTestFixtures_Wrapper_1___ctor_System_Void__0__type__System_Int32__method(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
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
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/GenericsVirtEhDemo::DemoCombine()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_GenericsVirtEhDemo_DemoCombine(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};


	CHAOS_EH_TRY
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Collections_System_Collections_Generic_List_System_Int32_, {});
				chaos_object->header.type_info = &chaos_mt_System_Collections_System_Collections_Generic_List_System_Int32_.hot;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[13])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[1] = _s1;
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1);
			chaos_locals[1] = _s0;
	CHAOS_EH_END
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
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
		ChaosObjectCtor(chaos_arg_0);
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
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	chaos_locals[0] = _s0;
	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(100);
	{
		auto chaos_value_raw = _s1;
		const auto chaos_value = static_cast<CHAOS_IL2CPP_INT32>(chaos_value_raw);
		const auto chaos_address = _s0;
		chaos_store_indirect<CHAOS_IL2CPP_INT32>(chaos_address, chaos_value);
	}
	_s0 = chaos_locals[0];
	chaos_locals[2] = _s0;
	_s0 = chaos_locals[2];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/InstanceFieldHelper::CreateAndUseFields()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_InstanceFieldHelper_CreateAndUseFields(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		const auto chaos_arg_1 = _s0;
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_SnapshotTestFixtures_HasInstanceFields, {});
		chaos_object->header.type_info = &chaos_mt_SnapshotTestFixtures_HasInstanceFields.hot;
		SnapshotTestFixtures_HasInstanceFields__ctor_System_Int32(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(100);
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
	_s0 = chaos_locals[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_SnapshotTestFixtures_HasInstanceFields*>(_s0);
		_s0 = chaos_object->field_SnapshotTestFixtures_HasInstanceFields__Value;
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
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
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
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
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)]);
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		auto chaos_value_raw = _s1;
		const auto chaos_value = static_cast<CHAOS_IL2CPP_INT32>(chaos_value_raw);
		const auto chaos_address = _s0;
		chaos_store_indirect<CHAOS_IL2CPP_INT32>(chaos_address, chaos_value);
	}
	_s0 = chaos_locals[0];
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
	chaos_locals[2] = _s0;
	_s0 = chaos_locals[2];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/LdelemAllHelper::TestAllElems()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_LdelemAllHelper_TestAllElems(void)
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
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/LdftnHelper::GetFnPtr()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_LdftnHelper_GetFnPtr(void)
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


	_s0 = chaos_static_SnapshotTestFixtures_LdftnHelper___O___0___GetValue;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			_s0 = 0;
			{
				// Hotpatch-aware ldftn wrapper (slot 58)
				static auto* chaos_ftn_thunk = +[](void) -> CHAOS_IL2CPP_INT32 {
					auto& _d_entry = s_hotpatch_entries[58];
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
		chaos_locals[0] = _s0;
		_s0 = chaos_locals[0];
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
		chaos_locals[1] = _s2;
		_s2 = chaos_locals[1];
		return static_cast<CHAOS_IL2CPP_INT32>(_s2);
	}
}

// Managed method: SnapshotTestFixtures/LdindI1Helper::RunLdindI1()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_LdindI1Helper_RunLdindI1(void)
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
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/LdindI8Helper::RunLdindI8()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_LdindI8Helper_RunLdindI8(void)
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
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s2 = ChaosStoreInt64(4294967296LL);
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
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s1);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s0);
		const auto chaos_element = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];
		_s0 = chaos_element;
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	chaos_locals[2] = _s0;
	_s0 = chaos_locals[2];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/LdindStindGapsHelper::RunLdindI()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_LdindStindGapsHelper_RunLdindI(void)
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
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
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
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s1);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s0);
		const auto chaos_element = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];
		_s0 = chaos_element;
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/LdindStindGapsHelper::RunLdindI4()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_LdindStindGapsHelper_RunLdindI4(void)
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
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
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
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s1);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s0);
		const auto chaos_element = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];
		_s0 = chaos_element;
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/LdindStindGapsHelper::RunStindI()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_LdindStindGapsHelper_RunStindI(void)
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
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(100);
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
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s1);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s0);
		const auto chaos_element = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];
		_s0 = chaos_element;
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/LdindUnsignedHelper::RunLdindUnsigned()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_LdindUnsignedHelper_RunLdindUnsigned(void)
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
	CHAOS_IL2CPP_INTPTR _s28{};
	CHAOS_IL2CPP_INTPTR _s29{};
	CHAOS_IL2CPP_INTPTR _s30{};
	CHAOS_IL2CPP_INTPTR _s31{};
	CHAOS_IL2CPP_INTPTR _s32{};


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
	chaos_locals[3] = _s0;
	_s0 = chaos_locals[3];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/LdindWideHelper::RunLdindWide()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_LdindWideHelper_RunLdindWide(void)
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
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
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
		const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = ChaosReflectionGetTypeHandle(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[0] = _s0;
	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[33])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[1] = _s1;
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[34])();
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s2);
}

// Managed method: SnapshotTestFixtures/LdVirtftnHelper::RunLdVirtftn()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_LdVirtftnHelper_RunLdVirtftn(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		const auto chaos_arg_1 = _s0;
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_SnapshotTestFixtures_MyClass, {});
		chaos_object->header.type_info = &chaos_mt_SnapshotTestFixtures_MyClass.hot;
		SnapshotTestFixtures_MyClass__ctor_System_Int32(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
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
		chaos_callvirt_result = (*reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)>(chaos_vtable_resolve(chaos_object_get_type_info(reinterpret_cast<void*>(chaos_arg_0))->vtable_array, 3u)))(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_callvirt_result);
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/LocalAllocHelper::RunAlloc()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_LocalAllocHelper_RunAlloc(void)
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
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		auto chaos_value_raw = _s1;
		const auto chaos_value = static_cast<CHAOS_IL2CPP_INT8>(chaos_value_raw);
		const auto chaos_address = _s0;
		chaos_store_indirect<CHAOS_IL2CPP_INT8>(chaos_address, chaos_value);
	}
	_s0 = chaos_locals[0];
	{
		const auto chaos_address = _s0;
		const auto chaos_value = chaos_load_indirect<CHAOS_IL2CPP_UINT8>(chaos_address);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_value);
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/LoopHelper::SumToFive()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_LoopHelper_SumToFive(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 4) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	// pc-dispatch state machine for irreducible CFG
	int32_t chaos_pc = 0;
	while (chaos_pc >= 0)
	{
		switch (chaos_pc)
		{
		case 0:
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			chaos_locals[0] = _s0;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			chaos_locals[1] = _s0;
			chaos_pc = 2;
			break;
		}
		case 1:
		{
			_s0 = chaos_locals[0];
			_s1 = chaos_locals[1];
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[1];
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
			chaos_locals[1] = _s0;
			chaos_pc = -1;
			break;
		}
		case 2:
		{
			_s0 = chaos_locals[1];
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0) < static_cast<CHAOS_IL2CPP_INT32>(_s1) ? 1 : 0);
			chaos_locals[2] = _s0;
			_s0 = chaos_locals[2];
			chaos_pc = 1;
			break;
		}
		case 3:
		{
			_s1 = chaos_locals[0];
			chaos_locals[3] = _s1;
			chaos_pc = 4;
			break;
		}
		case 4:
		{
			_s1 = chaos_locals[3];
			return static_cast<CHAOS_IL2CPP_INT32>(_s1);
			chaos_pc = -1;
			break;
		}
		default:
			CHAOS_IL2CPP_FAIL("invalid pc-dispatch value");
			chaos_pc = -1;
			break;
		}
	}
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
		ChaosObjectCtor(chaos_arg_0);
	}
	return;
}

// Managed method: SnapshotTestFixtures/MathHelper::RunSquare()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_MathHelper_RunSquare(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		const auto chaos_arg_0 = _s0;
		auto& _d60 = s_hotpatch_entries[60];
		CHAOS_IL2CPP_INT32 _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d60)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d60))
		{
			alignas(16) uint8_t _d_ab[4];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d60.method_key, _d_ab, &_d_hpresult);
		}
		else
		{
			_d_hpresult = SnapshotTestFixtures_Helper_Square_System_Int32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
		}
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-static_cast<CHAOS_IL2CPP_INT32>(_s0));
	chaos_locals[1] = _s0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(CHAOS_IL2CPP_NUMERIC_LIMITS_MIN(CHAOS_IL2CPP_INT32));
	chaos_locals[2] = _s0;
	_s0 = chaos_locals[2];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_shift_right_int32(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	chaos_locals[3] = _s0;
	_s0 = chaos_locals[1];
	_s1 = chaos_locals[3];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	chaos_locals[4] = _s0;
	_s0 = chaos_locals[4];
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
	chaos_args[0] = ChaosStoreFloat32(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINT32>(_s0));
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s0 = ChaosStoreFloat32(static_cast<float>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s0)));
	_s0 = ChaosStoreFloat64(static_cast<double>(_s0));
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(chaos_load_float32(_s0)));
	chaos_locals[2] = _s0;
	_s0 = chaos_locals[2];
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
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			chaos_locals[0] = _s0;
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			CHAOS_EH_RETHROW;
	CHAOS_EH_END
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(8);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_shift_left_int32(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_shift_right_int32(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	chaos_locals[2] = _s0;
	_s0 = chaos_locals[1];
	_s1 = chaos_locals[2];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	chaos_locals[3] = _s0;
	_s0 = chaos_locals[3];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/SimpleGapsHelper::RunConvI()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_SimpleGapsHelper_RunConvI(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
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
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/SimpleGapsHelper::RunLdnull()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_SimpleGapsHelper_RunLdnull(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};


	_s0 = 0;
	{
		if (_s0 == 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		chaos_locals[1] = _s0;
		_s0 = chaos_locals[1];
		return static_cast<CHAOS_IL2CPP_INT32>(_s0);
	}
}

// Managed method: SnapshotTestFixtures/SizeOfHelper::GetSize()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_SizeOfHelper_GetSize(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/StaticFieldWriteHelper::WriteAndRead()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_StaticFieldWriteHelper_WriteAndRead(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		auto chaos_value = _s0;
		chaos_static_SnapshotTestFixtures_StaticFieldWriteHelper___writeTarget = chaos_value;
	}
	_s0 = chaos_static_SnapshotTestFixtures_StaticFieldWriteHelper___readTarget;
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/StelemAllHelper::TestAllElems()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_StelemAllHelper_TestAllElems(void)
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
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/StindNarrowHelper::RunStindNarrow()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_StindNarrowHelper_RunStindNarrow(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Int16.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
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
	_s0 = chaos_locals[1];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(100);
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
	chaos_locals[2] = _s0;
	_s0 = chaos_locals[2];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/StindWideHelper::RunStindWide()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_StindWideHelper_RunStindWide(void)
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
	CHAOS_IL2CPP_INTPTR _s28{};
	CHAOS_IL2CPP_INTPTR _s29{};
	CHAOS_IL2CPP_INTPTR _s30{};
	CHAOS_IL2CPP_INTPTR _s31{};
	CHAOS_IL2CPP_INTPTR _s32{};


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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Double.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Object.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	chaos_locals[2] = _s0;
	_s0 = chaos_locals[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s2 = ChaosStoreFloat32(3.5f);
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
	_s0 = chaos_locals[1];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s2 = ChaosStoreFloat64(7.2);
	{
		auto chaos_value_raw = _s2;
		const auto chaos_value = chaos_value_raw;
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s1);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s0);
		chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)] = chaos_value;
	}
	_s0 = chaos_locals[2];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{{
		_s2 = CHAOS_IL2CPP_STRING_ID("hi");
	}}
	{
		auto chaos_value_raw = _s2;
		if (chaos_is_string_id(chaos_value_raw))
		{
			chaos_value_raw = chaos_string_materialize(chaos_value_raw);
		}
		auto chaos_value = chaos_value_raw;
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s1);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s0);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)]));
		chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)] = chaos_value;
		GC_END_STUBBORN_CHANGE(chaos_array);
		chaos_gc_dirty_card(chaos_array);
	}
	_s0 = chaos_locals[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s1);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s0);
		const auto chaos_element = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];
		_s0 = chaos_element;
	}
	_s0 = ChaosStoreFloat64(static_cast<double>(_s0));
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
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(ChaosLoadFloat64(_s0), ChaosLoadFloat64(_s1)));
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s1 = chaos_locals[2];
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s2);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s1);
		const auto chaos_element = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];
		_s1 = chaos_element;
	}
	{
		if (_s1 != 0)
		{
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s17 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		_s15 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s15), static_cast<CHAOS_IL2CPP_INT32>(_s16)));
		chaos_locals[3] = _s15;
		_s15 = chaos_locals[3];
		return static_cast<CHAOS_IL2CPP_INT32>(_s15);
	}
}

// Managed method: SnapshotTestFixtures/StringConcatHelper::TestConcat()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_StringConcatHelper_TestConcat(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	{{
		_s0 = CHAOS_IL2CPP_STRING_ID("Hello");
	}}
	{
		const auto chaos_arg_0 = _s0;
		auto& _d57 = s_hotpatch_entries[57];
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d57)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d57))
		{
			alignas(16) uint8_t _d_ab[8];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d57.method_key, _d_ab, nullptr);
		}
		else
		{
			SnapshotTestFixtures_Helper_ConsumeString_System_String(chaos_arg_0);
		}
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
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
	CHAOS_IL2CPP_INTPTR _s6{};


	{{
		_s0 = CHAOS_IL2CPP_STRING_ID("Value: {0}");
	}}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		const auto chaos_value = _s1;
		auto* chaos_boxed = CHAOS_IL2CPP_NEW_GC(chaos_boxed_type_System_Private_CoreLib_System_Int32, {});
		chaos_boxed->header.type_info = &chaos_mt_System_Private_CoreLib_System_Int32.hot;
		chaos_boxed->value = chaos_value;
		_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_boxed);
	}
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__Format_System_String_System_String_System_Object_(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/StringOps::UseString()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_StringOps_UseString(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	{{
		_s0 = CHAOS_IL2CPP_STRING_ID("Hello");
	}}
	{
		const auto chaos_arg_0 = _s0;
		auto& _d57 = s_hotpatch_entries[57];
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d57)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d57))
		{
			alignas(16) uint8_t _d_ab[8];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d57.method_key, _d_ab, nullptr);
		}
		else
		{
			SnapshotTestFixtures_Helper_ConsumeString_System_String(chaos_arg_0);
		}
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/SwitchHelper::Classify()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_SwitchHelper_Classify(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	chaos_locals[0] = _s0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		if (_s0 != 0)
		{
		}
		_s0 = chaos_locals[0];
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		{
			const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
			const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
			if (chaos_left == chaos_right)
			{
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(10);
				chaos_locals[1] = _s0;
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(20);
				chaos_locals[1] = _s0;
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
				chaos_locals[1] = _s0;
			}
			else
			{
				_s0 = chaos_locals[0];
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
				{
					const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
					const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
					if (chaos_left == chaos_right)
					{
						_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(20);
						chaos_locals[1] = _s0;
						_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
						chaos_locals[1] = _s0;
					}
					else
					{
						_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(10);
						chaos_locals[1] = _s0;
					}
				}
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				if (_s0 != 0)
				{
				}
				_s0 = chaos_locals[1];
				chaos_locals[2] = _s0;
				_s0 = chaos_locals[2];
				return static_cast<CHAOS_IL2CPP_INT32>(_s0);
			}
		}
	}
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

// AOT-unreachable stub: SnapshotTestFixtures/ThrowHelper::CheckPositive:System.Int32(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_ThrowHelper_CheckPositive_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
{
	return {};
}

// Managed method: SnapshotTestFixtures/TypeCheckHelper::CheckAndCast()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_TypeCheckHelper_CheckAndCast(void)
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
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	_s1 = 0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s0) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 == 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1);
			chaos_locals[3] = _s0;
		}
		else
		{
			_s3 = chaos_locals[1];
			{
				auto* chaos_object = reinterpret_cast<chaos_type_SnapshotTestFixtures_MarkedClass*>(_s3);
				_s3 = chaos_object->field_SnapshotTestFixtures_MarkedClass__Value;
			}
			chaos_locals[3] = _s3;
		}
		_s3 = chaos_locals[3];
		return static_cast<CHAOS_IL2CPP_INT32>(_s3);
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
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[47])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (_s0 != 0)
		{
			_s0 = chaos_args[0];
		}
		else
		{
			_s1 = ChaosStoreFloat64(0);
		}
		return ChaosLoadFloat64(_s0);
	}
}

// Managed method: SnapshotTestFixtures/UnsignedOpsHelper::TestCltUn()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_UnsignedOpsHelper_TestCltUn(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	chaos_locals[0] = _s0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[0];
	_s1 = chaos_locals[1];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	chaos_locals[2] = _s0;
	_s0 = chaos_locals[2];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/UnsignedOpsHelper::TestDivUn()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_UnsignedOpsHelper_TestDivUn(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(10);
	chaos_locals[0] = _s0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[0];
	_s1 = chaos_locals[1];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosDiv(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	chaos_locals[2] = _s0;
	_s0 = chaos_locals[2];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/UnsignedOpsHelper::TestRemUn()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_UnsignedOpsHelper_TestRemUn(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(10);
	chaos_locals[0] = _s0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[0];
	_s1 = chaos_locals[1];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosRem(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	chaos_locals[2] = _s0;
	_s0 = chaos_locals[2];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/ValueTypeHelper::RunValueType()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_ValueTypeHelper_RunValueType(void)
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


	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		auto* chaos_value = chaos_resolve_managed_value_pointer<chaos_valuetype_SnapshotTestFixtures_MyValueType>(_s0);
		*chaos_value = chaos_valuetype_SnapshotTestFixtures_MyValueType{};
	}
	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		auto chaos_value = _s1;
		if (chaos_is_string_id(chaos_value))
		{
			chaos_value = chaos_string_materialize(chaos_value);
		}
		auto* chaos_value_owner = chaos_resolve_managed_value_pointer<chaos_valuetype_SnapshotTestFixtures_MyValueType>(_s0);
		chaos_value_owner->field_SnapshotTestFixtures_MyValueType__X = chaos_value;
		if (chaos_is_gc_pointer(chaos_value_owner))
		{
			chaos_gc_dirty_card(chaos_value_owner);
		}
	}
	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(10);
	{
		auto chaos_value = _s1;
		if (chaos_is_string_id(chaos_value))
		{
			chaos_value = chaos_string_materialize(chaos_value);
		}
		auto* chaos_value_owner = chaos_resolve_managed_value_pointer<chaos_valuetype_SnapshotTestFixtures_MyValueType>(_s0);
		chaos_value_owner->field_SnapshotTestFixtures_MyValueType__Y = chaos_value;
		if (chaos_is_gc_pointer(chaos_value_owner))
		{
			chaos_gc_dirty_card(chaos_value_owner);
		}
	}
	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		auto* chaos_value = chaos_resolve_managed_value_pointer<chaos_valuetype_SnapshotTestFixtures_MyValueType>(_s0);
		_s0 = chaos_value->field_SnapshotTestFixtures_MyValueType__X;
	}
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		auto* chaos_value = chaos_resolve_managed_value_pointer<chaos_valuetype_SnapshotTestFixtures_MyValueType>(_s1);
		_s1 = chaos_value->field_SnapshotTestFixtures_MyValueType__Y;
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: SnapshotTestFixtures/VirtualDispatchHelper::UseVirtualDispatch()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_VirtualDispatchHelper_UseVirtualDispatch(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_SnapshotTestFixtures_DerivedClass, {});
		chaos_object->header.type_info = &chaos_mt_SnapshotTestFixtures_DerivedClass.hot;
		SnapshotTestFixtures_DerivedClass__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
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

// Managed method: SnapshotTestFixtures/VirtualHelper::UseVirtual()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_VirtualHelper_UseVirtual(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_SnapshotTestFixtures_DerivedClass, {});
		chaos_object->header.type_info = &chaos_mt_SnapshotTestFixtures_DerivedClass.hot;
		SnapshotTestFixtures_DerivedClass__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
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

// Managed method: SnapshotTestFixtures/VoidCaller::DoNothing()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_VoidCaller_DoNothing(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	{
		auto& _d59 = s_hotpatch_entries[59];
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d59)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d59))
		{
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d59.method_key, nullptr, nullptr);
		}
		else
		{
			SnapshotTestFixtures_Helper_Nop();
		}
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
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

// Generic execution authority: definition=SnapshotTestFixtures/Wrapper`1::.ctor:System.Void(!0); type=[System.Int32]; method=[]; support=Specialized; specialization=SharedBody; body=body:definition=SnapshotTestFixtures/Wrapper`1::.ctor:System.Void(!0);type=[System.Int32];method=[]; stub=stub:definition=SnapshotTestFixtures/Wrapper`1::.ctor:System.Void(!0);type=[System.Int32];method=[]
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
		auto* chaos_object = reinterpret_cast<chaos_type_SnapshotTestFixtures_Wrapper_System_Int32_*>(_s0);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_object->field_SnapshotTestFixtures_Wrapper_System_Int32____val));
		chaos_object->field_SnapshotTestFixtures_Wrapper_System_Int32____val = chaos_value;
		chaos_gc_dirty_card(chaos_object);
	}
	return;
}

// Generic instantiation stub: SnapshotTestFixtures/Wrapper<System.Int32>::.ctor(System.Int32)
// Generic execution authority: definition=SnapshotTestFixtures/Wrapper`1::.ctor:System.Void(!0); type=[System.Int32]; method=[]; support=Specialized; specialization=SharedBody; body=body:definition=SnapshotTestFixtures/Wrapper`1::.ctor:System.Void(!0);type=[System.Int32];method=[]; stub=stub:definition=SnapshotTestFixtures/Wrapper`1::.ctor:System.Void(!0);type=[System.Int32];method=[]
extern "C" void chaos_stub_definition_SnapshotTestFixtures_Wrapper_1___ctor_System_Void__0__type__System_Int32__method(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	SnapshotTestFixtures_Wrapper_System_Int32__ctor_System_Int32(chaos_fn_arg_0, chaos_fn_arg_1);
}

// Generic execution authority: definition=SnapshotTestFixtures/Wrapper`1::GetValue:!0(); type=[System.Int32]; method=[]; support=Specialized; specialization=SharedBody; body=body:definition=SnapshotTestFixtures/Wrapper`1::GetValue:!0();type=[System.Int32];method=[]; stub=stub:definition=SnapshotTestFixtures/Wrapper`1::GetValue:!0();type=[System.Int32];method=[]
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
// Generic execution authority: definition=SnapshotTestFixtures/Wrapper`1::GetValue:!0(); type=[System.Int32]; method=[]; support=Specialized; specialization=SharedBody; body=body:definition=SnapshotTestFixtures/Wrapper`1::GetValue:!0();type=[System.Int32];method=[]; stub=stub:definition=SnapshotTestFixtures/Wrapper`1::GetValue:!0();type=[System.Int32];method=[]
extern "C" CHAOS_IL2CPP_INT32 chaos_stub_definition_SnapshotTestFixtures_Wrapper_1__GetValue__0___type__System_Int32__method(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return SnapshotTestFixtures_Wrapper_System_Int32_GetValue(chaos_fn_arg_0);
}



}  // namespace chaos::il2cpp::codegen::SnapshotProverSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 165;

extern "C" void ChaosJitRegisterAll() {}