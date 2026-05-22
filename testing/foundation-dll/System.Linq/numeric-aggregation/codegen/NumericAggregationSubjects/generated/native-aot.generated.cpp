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

namespace chaos::il2cpp::codegen::NumericAggregationSubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


const void* chaos_vtable_NumericAggregationSubjects_NumericAggregationSubjects___c[];
inline TypeInfoV0 chaos_mt_NumericAggregationSubjects__PrivateImplementationDetails_ = {{nullptr, nullptr, 10811871657461465681ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_NumericAggregationSubjects__PrivateImplementationDetails_ = static_cast<CHAOS_IL2CPP_INTPTR>(10811871657461465681ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Object = {{nullptr, nullptr, 15228727185366376748ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_INTPTR>(15228727185366376748ULL);
inline TypeInfoV0 chaos_mt_NumericAggregationSubjects_NumericAggregationSubjects___c = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_NumericAggregationSubjects_NumericAggregationSubjects___c, 12238133729049377710ULL, 47u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_NumericAggregationSubjects_NumericAggregationSubjects___c = static_cast<CHAOS_IL2CPP_INTPTR>(12238133729049377710ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Decimal = {{nullptr, nullptr, 1126810584598443822ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Decimal = static_cast<CHAOS_IL2CPP_INTPTR>(1126810584598443822ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Delegate = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, nullptr, 7451128447593600616ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Delegate = static_cast<CHAOS_IL2CPP_INTPTR>(7451128447593600616ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Double = {{nullptr, nullptr, 1099026126681926114ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Double = static_cast<CHAOS_IL2CPP_INTPTR>(1099026126681926114ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_MulticastDelegate = {{&chaos_mt_System_Private_CoreLib_System_Delegate.hot, nullptr, 6681393039041505440ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_MulticastDelegate = static_cast<CHAOS_IL2CPP_INTPTR>(6681393039041505440ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Decimal_ = {{&chaos_mt_System_Private_CoreLib_System_MulticastDelegate.hot, nullptr, 3287648597379204658ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Func_System_Int32_System_Decimal_ = static_cast<CHAOS_IL2CPP_INTPTR>(3287648597379204658ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Double_ = {{&chaos_mt_System_Private_CoreLib_System_MulticastDelegate.hot, nullptr, 4864595939477400398ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Func_System_Int32_System_Double_ = static_cast<CHAOS_IL2CPP_INTPTR>(4864595939477400398ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Int32_ = {{&chaos_mt_System_Private_CoreLib_System_MulticastDelegate.hot, nullptr, 143744549824129343ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Func_System_Int32_System_Int32_ = static_cast<CHAOS_IL2CPP_INTPTR>(143744549824129343ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Int64_ = {{&chaos_mt_System_Private_CoreLib_System_MulticastDelegate.hot, nullptr, 2075640356155029482ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Func_System_Int32_System_Int64_ = static_cast<CHAOS_IL2CPP_INTPTR>(2075640356155029482ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Decimal__ = {{&chaos_mt_System_Private_CoreLib_System_MulticastDelegate.hot, nullptr, 10180256040415175700ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Decimal__ = static_cast<CHAOS_IL2CPP_INTPTR>(10180256040415175700ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Double__ = {{&chaos_mt_System_Private_CoreLib_System_MulticastDelegate.hot, nullptr, 7681611236666947264ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Double__ = static_cast<CHAOS_IL2CPP_INTPTR>(7681611236666947264ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Int32__ = {{&chaos_mt_System_Private_CoreLib_System_MulticastDelegate.hot, nullptr, 6985111991745502659ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Int32__ = static_cast<CHAOS_IL2CPP_INTPTR>(6985111991745502659ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Int64__ = {{&chaos_mt_System_Private_CoreLib_System_MulticastDelegate.hot, nullptr, 6437342008370602060ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Int64__ = static_cast<CHAOS_IL2CPP_INTPTR>(6437342008370602060ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Single__ = {{&chaos_mt_System_Private_CoreLib_System_MulticastDelegate.hot, nullptr, 6656331294692990133ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Single__ = static_cast<CHAOS_IL2CPP_INTPTR>(6656331294692990133ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Single_ = {{&chaos_mt_System_Private_CoreLib_System_MulticastDelegate.hot, nullptr, 15185598848183690713ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Func_System_Int32_System_Single_ = static_cast<CHAOS_IL2CPP_INTPTR>(15185598848183690713ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Func_System_String_System_Int32_ = {{&chaos_mt_System_Private_CoreLib_System_MulticastDelegate.hot, nullptr, 5721823073990243850ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Func_System_String_System_Int32_ = static_cast<CHAOS_IL2CPP_INTPTR>(5721823073990243850ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Func_System_String_System_Int64_ = {{&chaos_mt_System_Private_CoreLib_System_MulticastDelegate.hot, nullptr, 3916309532227451195ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Func_System_String_System_Int64_ = static_cast<CHAOS_IL2CPP_INTPTR>(3916309532227451195ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Func_System_String_System_Nullable_System_Int32__ = {{&chaos_mt_System_Private_CoreLib_System_MulticastDelegate.hot, nullptr, 4971438128144269160ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Func_System_String_System_Nullable_System_Int32__ = static_cast<CHAOS_IL2CPP_INTPTR>(4971438128144269160ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Func_System_String_System_Nullable_System_Int64__ = {{&chaos_mt_System_Private_CoreLib_System_MulticastDelegate.hot, nullptr, 6071123657633147003ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Func_System_String_System_Nullable_System_Int64__ = static_cast<CHAOS_IL2CPP_INTPTR>(6071123657633147003ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Func_System_String_System_String_ = {{&chaos_mt_System_Private_CoreLib_System_MulticastDelegate.hot, nullptr, 4000746079747307385ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Func_System_String_System_String_ = static_cast<CHAOS_IL2CPP_INTPTR>(4000746079747307385ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Int32 = {{nullptr, nullptr, 11009693519287992193ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Int32 = static_cast<CHAOS_IL2CPP_INTPTR>(11009693519287992193ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Int64 = {{nullptr, nullptr, 11012495074916106146ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Int64 = static_cast<CHAOS_IL2CPP_INTPTR>(11012495074916106146ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Nullable_System_Decimal_ = {{nullptr, nullptr, 333222278826921068ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Nullable_System_Decimal_ = static_cast<CHAOS_IL2CPP_INTPTR>(333222278826921068ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Nullable_System_Double_ = {{nullptr, nullptr, 840799680635684624ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Nullable_System_Double_ = static_cast<CHAOS_IL2CPP_INTPTR>(840799680635684624ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Nullable_System_Int32_ = {{nullptr, nullptr, 17301262477321293673ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Nullable_System_Int32_ = static_cast<CHAOS_IL2CPP_INTPTR>(17301262477321293673ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Nullable_System_Int64_ = {{nullptr, nullptr, 14174692311676386384ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Nullable_System_Int64_ = static_cast<CHAOS_IL2CPP_INTPTR>(14174692311676386384ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Nullable_System_Single_ = {{nullptr, nullptr, 14822909699815843743ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Nullable_System_Single_ = static_cast<CHAOS_IL2CPP_INTPTR>(14822909699815843743ULL);
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
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Single = {{nullptr, nullptr, 4813876735449938439ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Single = static_cast<CHAOS_IL2CPP_INTPTR>(4813876735449938439ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_String = {{nullptr, nullptr, 1782325859292956794ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_String = static_cast<CHAOS_IL2CPP_INTPTR>(1782325859292956794ULL);

// ── Virtual method table arrays ──
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects___c__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_10_b__11_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT64 NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_11_b__12_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" float NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_12_b__13_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" double NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_13_b__14_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_14_b__15_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_15_b__16_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_16_b__17_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_17_b__18_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_18_b__19_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_19_b__20_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_32_b__33_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_33_b__34_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT64 NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_34_b__35_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_35_b__36_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" float NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_36_b__37_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_37_b__38_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" double NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_38_b__39_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_39_b__40_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_40_b__41_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_41_b__42_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_42_b__43_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_43_b__44_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_44_b__45_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_57_b__58_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_58_b__59_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT64 NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_59_b__60_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_60_b__61_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" float NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_61_b__62_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_62_b__63_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" double NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_63_b__64_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_64_b__65_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_65_b__66_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_66_b__67_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_67_b__68_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_68_b__69_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_69_b__70_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_80_b__81_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT64 NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_81_b__82_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" float NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_82_b__83_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" double NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_83_b__84_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_84_b__85_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_85_b__86_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_86_b__87_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_87_b__88_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_88_b__89_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_89_b__90_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
const void* chaos_vtable_NumericAggregationSubjects_NumericAggregationSubjects___c[] =
{
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c__ctor),
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_10_b__11_0_System_Int32),
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_11_b__12_0_System_Int32),
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_12_b__13_0_System_Int32),
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_13_b__14_0_System_Int32),
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_14_b__15_0_System_Int32),
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_15_b__16_0_System_Int32),
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_16_b__17_0_System_Int32),
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_17_b__18_0_System_Int32),
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_18_b__19_0_System_Int32),
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_19_b__20_0_System_Int32),
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_32_b__33_0_System_String),
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_33_b__34_0_System_String),
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_34_b__35_0_System_String),
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_35_b__36_0_System_String),
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_36_b__37_0_System_Int32),
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_37_b__38_0_System_Int32),
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_38_b__39_0_System_Int32),
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_39_b__40_0_System_Int32),
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_40_b__41_0_System_Int32),
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_41_b__42_0_System_Int32),
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_42_b__43_0_System_String),
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_43_b__44_0_System_String),
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_44_b__45_0_System_String),
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_57_b__58_0_System_String),
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_58_b__59_0_System_String),
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_59_b__60_0_System_String),
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_60_b__61_0_System_String),
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_61_b__62_0_System_Int32),
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_62_b__63_0_System_Int32),
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_63_b__64_0_System_Int32),
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_64_b__65_0_System_Int32),
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_65_b__66_0_System_Int32),
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_66_b__67_0_System_Int32),
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_67_b__68_0_System_String),
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_68_b__69_0_System_String),
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_69_b__70_0_System_String),
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_80_b__81_0_System_Int32),
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_81_b__82_0_System_Int32),
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_82_b__83_0_System_Int32),
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_83_b__84_0_System_Int32),
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_84_b__85_0_System_Int32),
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_85_b__86_0_System_Int32),
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_86_b__87_0_System_Int32),
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_87_b__88_0_System_Int32),
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_88_b__89_0_System_Int32),
	reinterpret_cast<void*>(NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_89_b__90_0_System_Int32),
};
static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_NumericAggregationSubjects_NumericAggregationSubjects___c[] =
{
	{ 0x0000009Eu, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c__ctor) },
	{ 0x0000009Fu, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_10_b__11_0_System_Int32) },
	{ 0x000000A0u, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_11_b__12_0_System_Int32) },
	{ 0x000000A1u, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_12_b__13_0_System_Int32) },
	{ 0x000000A2u, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_13_b__14_0_System_Int32) },
	{ 0x000000A3u, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_14_b__15_0_System_Int32) },
	{ 0x000000A4u, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_15_b__16_0_System_Int32) },
	{ 0x000000A5u, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_16_b__17_0_System_Int32) },
	{ 0x000000A6u, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_17_b__18_0_System_Int32) },
	{ 0x000000A7u, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_18_b__19_0_System_Int32) },
	{ 0x000000A8u, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_19_b__20_0_System_Int32) },
	{ 0x000000A9u, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_32_b__33_0_System_String) },
	{ 0x000000AAu, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_33_b__34_0_System_String) },
	{ 0x000000ABu, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_34_b__35_0_System_String) },
	{ 0x000000ACu, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_35_b__36_0_System_String) },
	{ 0x000000ADu, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_36_b__37_0_System_Int32) },
	{ 0x000000AEu, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_37_b__38_0_System_Int32) },
	{ 0x000000AFu, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_38_b__39_0_System_Int32) },
	{ 0x000000B0u, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_39_b__40_0_System_Int32) },
	{ 0x000000B1u, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_40_b__41_0_System_Int32) },
	{ 0x000000B2u, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_41_b__42_0_System_Int32) },
	{ 0x000000B3u, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_42_b__43_0_System_String) },
	{ 0x000000B4u, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_43_b__44_0_System_String) },
	{ 0x000000B5u, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_44_b__45_0_System_String) },
	{ 0x000000B6u, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_57_b__58_0_System_String) },
	{ 0x000000B7u, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_58_b__59_0_System_String) },
	{ 0x000000B8u, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_59_b__60_0_System_String) },
	{ 0x000000B9u, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_60_b__61_0_System_String) },
	{ 0x000000BAu, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_61_b__62_0_System_Int32) },
	{ 0x000000BBu, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_62_b__63_0_System_Int32) },
	{ 0x000000BCu, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_63_b__64_0_System_Int32) },
	{ 0x000000BDu, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_64_b__65_0_System_Int32) },
	{ 0x000000BEu, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_65_b__66_0_System_Int32) },
	{ 0x000000BFu, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_66_b__67_0_System_Int32) },
	{ 0x000000C0u, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_67_b__68_0_System_String) },
	{ 0x000000C1u, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_68_b__69_0_System_String) },
	{ 0x000000C2u, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_69_b__70_0_System_String) },
	{ 0x000000C3u, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_80_b__81_0_System_Int32) },
	{ 0x000000C4u, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_81_b__82_0_System_Int32) },
	{ 0x000000C5u, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_82_b__83_0_System_Int32) },
	{ 0x000000C6u, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_83_b__84_0_System_Int32) },
	{ 0x000000C7u, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_84_b__85_0_System_Int32) },
	{ 0x000000C8u, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_85_b__86_0_System_Int32) },
	{ 0x000000C9u, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_86_b__87_0_System_Int32) },
	{ 0x000000CAu, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_87_b__88_0_System_Int32) },
	{ 0x000000CBu, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_88_b__89_0_System_Int32) },
	{ 0x000000CCu, reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_89_b__90_0_System_Int32) },
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

struct chaos_type_NumericAggregationSubjects__PrivateImplementationDetails_
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR field_NumericAggregationSubjects__PrivateImplementationDetails___182CD2C8E82B98EC53B1CA4C202076C607114AA7401B0767536843C1ABBAFFF9 = 0;
	CHAOS_IL2CPP_INTPTR field_NumericAggregationSubjects__PrivateImplementationDetails___4636993D3E1DA4E9D6B8F87B79E8F7C6D018580D52661950EABC3845C5897A4D = 0;
	CHAOS_IL2CPP_INTPTR field_NumericAggregationSubjects__PrivateImplementationDetails___4F6ADDC9659D6FB90FE94B6688A79F2A1FA8D36EC43F8F3E1D9B6528C448A384 = 0;
	CHAOS_IL2CPP_INTPTR field_NumericAggregationSubjects__PrivateImplementationDetails___58F0E9F70561D14C7C4E5D2FF8D28FF308BBCADACE0C72A56EC7ACD9F9227D19 = 0;
	CHAOS_IL2CPP_INTPTR field_NumericAggregationSubjects__PrivateImplementationDetails___5E8AACDD15ECEBAFF7B1FB3797E322DE0FBC1A26201A13F545818DC8961C5D36 = 0;
	CHAOS_IL2CPP_INTPTR field_NumericAggregationSubjects__PrivateImplementationDetails___6B99172F78B2670437EC662F13B85B0B2135FFB49E3952F469FAC00C9646399E = 0;
	CHAOS_IL2CPP_INTPTR field_NumericAggregationSubjects__PrivateImplementationDetails___9AA6B7E9A2041E57FF4210A87426C778833120D5C972E859E0711CE16D883EA3 = 0;
	CHAOS_IL2CPP_INTPTR field_NumericAggregationSubjects__PrivateImplementationDetails___AD1461355D664226DF0A9DD36082CE06AFA03D52A19BEB8E90853216EF6B221E = 0;
	CHAOS_IL2CPP_INTPTR field_NumericAggregationSubjects__PrivateImplementationDetails___BC7280150A400968EE578E3BB3A783D36E12EE252E41DD8E04486692B7D709D6 = 0;
	CHAOS_IL2CPP_INTPTR field_NumericAggregationSubjects__PrivateImplementationDetails___DF0B70CF4E7035ED4D043FA1B1C524FF2F098F8E1E0CED3A45962ED4A830D168 = 0;
	CHAOS_IL2CPP_INTPTR field_NumericAggregationSubjects__PrivateImplementationDetails___E24846410E56684CEDBB55CFA4A0BB715A05EFE983172488FE80BBAF1D014FC7 = 0;
	CHAOS_IL2CPP_INTPTR field_NumericAggregationSubjects__PrivateImplementationDetails___F0764F66820230734879BE00CD0504D53104DD9C68B3B78273DC9779573963EE = 0;
};

struct chaos_type_System_Private_CoreLib_System_Object
{
	ThinLockableHeader header{};
};

struct chaos_type_NumericAggregationSubjects_NumericAggregationSubjects___c : public chaos_type_System_Private_CoreLib_System_Object
{
};

struct chaos_type_System_Private_CoreLib_System_Decimal
{
	PureTypeHeader header{};
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

struct chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Decimal_ : public chaos_type_System_Private_CoreLib_System_MulticastDelegate
{
};

struct chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Double_ : public chaos_type_System_Private_CoreLib_System_MulticastDelegate
{
};

struct chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Int32_ : public chaos_type_System_Private_CoreLib_System_MulticastDelegate
{
};

struct chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Int64_ : public chaos_type_System_Private_CoreLib_System_MulticastDelegate
{
};

struct chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Decimal__ : public chaos_type_System_Private_CoreLib_System_MulticastDelegate
{
};

struct chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Double__ : public chaos_type_System_Private_CoreLib_System_MulticastDelegate
{
};

struct chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Int32__ : public chaos_type_System_Private_CoreLib_System_MulticastDelegate
{
};

struct chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Int64__ : public chaos_type_System_Private_CoreLib_System_MulticastDelegate
{
};

struct chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Single__ : public chaos_type_System_Private_CoreLib_System_MulticastDelegate
{
};

struct chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Single_ : public chaos_type_System_Private_CoreLib_System_MulticastDelegate
{
};

struct chaos_type_System_Private_CoreLib_System_Func_System_String_System_Int32_ : public chaos_type_System_Private_CoreLib_System_MulticastDelegate
{
};

struct chaos_type_System_Private_CoreLib_System_Func_System_String_System_Int64_ : public chaos_type_System_Private_CoreLib_System_MulticastDelegate
{
};

struct chaos_type_System_Private_CoreLib_System_Func_System_String_System_Nullable_System_Int32__ : public chaos_type_System_Private_CoreLib_System_MulticastDelegate
{
};

struct chaos_type_System_Private_CoreLib_System_Func_System_String_System_Nullable_System_Int64__ : public chaos_type_System_Private_CoreLib_System_MulticastDelegate
{
};

struct chaos_type_System_Private_CoreLib_System_Func_System_String_System_String_ : public chaos_type_System_Private_CoreLib_System_MulticastDelegate
{
};

struct chaos_type_System_Private_CoreLib_System_Int32
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Private_CoreLib_System_Int64
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Private_CoreLib_System_Nullable_System_Decimal_
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Private_CoreLib_System_Nullable_System_Double_
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Private_CoreLib_System_Nullable_System_Int32_
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Private_CoreLib_System_Nullable_System_Int64_
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Private_CoreLib_System_Nullable_System_Single_
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

CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__11_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__12_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__13_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__14_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__15_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__16_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__17_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__18_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__19_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__20_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__33_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__34_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__35_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__36_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__37_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__38_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__39_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__40_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__41_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__42_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__43_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__44_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__45_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__58_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__59_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__60_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__61_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__62_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__63_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__64_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__65_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__66_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__67_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__68_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__69_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__70_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__81_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__82_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__83_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__84_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__85_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__86_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__87_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__88_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__89_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__90_0 = 0;


	// AOT-baked string table: sorted by StringId for binary search at runtime.
	constexpr chaos::il2cpp::string_table::StringEntry chaos_aot_string_entries[] = {
		{ 623245005171840293U, "bb", 2u },
		{ 643252818266422135U, "yy", 2u },
		{ 3414815163700866189U, "a", 1u },
		{ 3414842651491571463U, "x", 1u },
		{ 3806586984662747281U, "banana", 6u },
		{ 5660749963347831709U, "zzz", 3u },
		{ 5968103895666186489U, "cherry", 6u },
		{ 8492632318578411777U, "ccc", 3u },
		{ 8595791296793083327U, "apple", 5u },
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

const CHAOS_IL2CPP_UINT8 chaos_static_field_data_NumericAggregationSubjects__PrivateImplementationDetails___182CD2C8E82B98EC53B1CA4C202076C607114AA7401B0767536843C1ABBAFFF9[] =
{
	0x00, 0x00, 0xC0, 0x3F, 0x00, 0x00, 0x60, 0x40, 0x00, 0x00, 0x20, 0x40,
};

const CHAOS_IL2CPP_UINT8 chaos_static_field_data_NumericAggregationSubjects__PrivateImplementationDetails___4636993D3E1DA4E9D6B8F87B79E8F7C6D018580D52661950EABC3845C5897A4D[] =
{
	0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
};

const CHAOS_IL2CPP_UINT8 chaos_static_field_data_NumericAggregationSubjects__PrivateImplementationDetails___4F6ADDC9659D6FB90FE94B6688A79F2A1FA8D36EC43F8F3E1D9B6528C448A384[] =
{
	0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
	0x05, 0x00, 0x00, 0x00,
};

const CHAOS_IL2CPP_UINT8 chaos_static_field_data_NumericAggregationSubjects__PrivateImplementationDetails___58F0E9F70561D14C7C4E5D2FF8D28FF308BBCADACE0C72A56EC7ACD9F9227D19[] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x40,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x40,
};

const CHAOS_IL2CPP_UINT8 chaos_static_field_data_NumericAggregationSubjects__PrivateImplementationDetails___5E8AACDD15ECEBAFF7B1FB3797E322DE0FBC1A26201A13F545818DC8961C5D36[] =
{
	0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const CHAOS_IL2CPP_UINT8 chaos_static_field_data_NumericAggregationSubjects__PrivateImplementationDetails___6B99172F78B2670437EC662F13B85B0B2135FFB49E3952F469FAC00C9646399E[] =
{
	0x00, 0x00, 0x60, 0x40, 0x00, 0x00, 0xC0, 0x3F, 0x00, 0x00, 0x20, 0x40,
};

const CHAOS_IL2CPP_UINT8 chaos_static_field_data_NumericAggregationSubjects__PrivateImplementationDetails___9AA6B7E9A2041E57FF4210A87426C778833120D5C972E859E0711CE16D883EA3[] =
{
	0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const CHAOS_IL2CPP_UINT8 chaos_static_field_data_NumericAggregationSubjects__PrivateImplementationDetails___AD1461355D664226DF0A9DD36082CE06AFA03D52A19BEB8E90853216EF6B221E[] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x40,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x40,
};

const CHAOS_IL2CPP_UINT8 chaos_static_field_data_NumericAggregationSubjects__PrivateImplementationDetails___BC7280150A400968EE578E3BB3A783D36E12EE252E41DD8E04486692B7D709D6[] =
{
	0x00, 0x00, 0xC0, 0x3F, 0x00, 0x00, 0x20, 0x40, 0x00, 0x00, 0x60, 0x40,
};

const CHAOS_IL2CPP_UINT8 chaos_static_field_data_NumericAggregationSubjects__PrivateImplementationDetails___DF0B70CF4E7035ED4D043FA1B1C524FF2F098F8E1E0CED3A45962ED4A830D168[] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x3F,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x40,
};

const CHAOS_IL2CPP_UINT8 chaos_static_field_data_NumericAggregationSubjects__PrivateImplementationDetails___E24846410E56684CEDBB55CFA4A0BB715A05EFE983172488FE80BBAF1D014FC7[] =
{
	0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const CHAOS_IL2CPP_UINT8 chaos_static_field_data_NumericAggregationSubjects__PrivateImplementationDetails___F0764F66820230734879BE00CD0504D53104DD9C68B3B78273DC9779573963EE[] =
{
	0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
	0x05, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00,
};

bool chaos_try_get_static_field_data(
	CHAOS_IL2CPP_INTPTR chaos_field_handle,
	const CHAOS_IL2CPP_UINT8*& chaos_data,
	CHAOS_IL2CPP_SIZE& chaos_size)
{
	if (chaos_field_handle == static_cast<CHAOS_IL2CPP_INTPTR>(75398811u))
	{
		chaos_data = chaos_static_field_data_NumericAggregationSubjects__PrivateImplementationDetails___182CD2C8E82B98EC53B1CA4C202076C607114AA7401B0767536843C1ABBAFFF9;
		chaos_size = sizeof(chaos_static_field_data_NumericAggregationSubjects__PrivateImplementationDetails___182CD2C8E82B98EC53B1CA4C202076C607114AA7401B0767536843C1ABBAFFF9);
		return true;
	}
	if (chaos_field_handle == static_cast<CHAOS_IL2CPP_INTPTR>(75365335u))
	{
		chaos_data = chaos_static_field_data_NumericAggregationSubjects__PrivateImplementationDetails___4636993D3E1DA4E9D6B8F87B79E8F7C6D018580D52661950EABC3845C5897A4D;
		chaos_size = sizeof(chaos_static_field_data_NumericAggregationSubjects__PrivateImplementationDetails___4636993D3E1DA4E9D6B8F87B79E8F7C6D018580D52661950EABC3845C5897A4D);
		return true;
	}
	if (chaos_field_handle == static_cast<CHAOS_IL2CPP_INTPTR>(83620299u))
	{
		chaos_data = chaos_static_field_data_NumericAggregationSubjects__PrivateImplementationDetails___4F6ADDC9659D6FB90FE94B6688A79F2A1FA8D36EC43F8F3E1D9B6528C448A384;
		chaos_size = sizeof(chaos_static_field_data_NumericAggregationSubjects__PrivateImplementationDetails___4F6ADDC9659D6FB90FE94B6688A79F2A1FA8D36EC43F8F3E1D9B6528C448A384);
		return true;
	}
	if (chaos_field_handle == static_cast<CHAOS_IL2CPP_INTPTR>(76745527u))
	{
		chaos_data = chaos_static_field_data_NumericAggregationSubjects__PrivateImplementationDetails___58F0E9F70561D14C7C4E5D2FF8D28FF308BBCADACE0C72A56EC7ACD9F9227D19;
		chaos_size = sizeof(chaos_static_field_data_NumericAggregationSubjects__PrivateImplementationDetails___58F0E9F70561D14C7C4E5D2FF8D28FF308BBCADACE0C72A56EC7ACD9F9227D19);
		return true;
	}
	if (chaos_field_handle == static_cast<CHAOS_IL2CPP_INTPTR>(74978047u))
	{
		chaos_data = chaos_static_field_data_NumericAggregationSubjects__PrivateImplementationDetails___5E8AACDD15ECEBAFF7B1FB3797E322DE0FBC1A26201A13F545818DC8961C5D36;
		chaos_size = sizeof(chaos_static_field_data_NumericAggregationSubjects__PrivateImplementationDetails___5E8AACDD15ECEBAFF7B1FB3797E322DE0FBC1A26201A13F545818DC8961C5D36);
		return true;
	}
	if (chaos_field_handle == static_cast<CHAOS_IL2CPP_INTPTR>(68103623u))
	{
		chaos_data = chaos_static_field_data_NumericAggregationSubjects__PrivateImplementationDetails___6B99172F78B2670437EC662F13B85B0B2135FFB49E3952F469FAC00C9646399E;
		chaos_size = sizeof(chaos_static_field_data_NumericAggregationSubjects__PrivateImplementationDetails___6B99172F78B2670437EC662F13B85B0B2135FFB49E3952F469FAC00C9646399E);
		return true;
	}
	if (chaos_field_handle == static_cast<CHAOS_IL2CPP_INTPTR>(79353099u))
	{
		chaos_data = chaos_static_field_data_NumericAggregationSubjects__PrivateImplementationDetails___9AA6B7E9A2041E57FF4210A87426C778833120D5C972E859E0711CE16D883EA3;
		chaos_size = sizeof(chaos_static_field_data_NumericAggregationSubjects__PrivateImplementationDetails___9AA6B7E9A2041E57FF4210A87426C778833120D5C972E859E0711CE16D883EA3);
		return true;
	}
	if (chaos_field_handle == static_cast<CHAOS_IL2CPP_INTPTR>(68641022u))
	{
		chaos_data = chaos_static_field_data_NumericAggregationSubjects__PrivateImplementationDetails___AD1461355D664226DF0A9DD36082CE06AFA03D52A19BEB8E90853216EF6B221E;
		chaos_size = sizeof(chaos_static_field_data_NumericAggregationSubjects__PrivateImplementationDetails___AD1461355D664226DF0A9DD36082CE06AFA03D52A19BEB8E90853216EF6B221E);
		return true;
	}
	if (chaos_field_handle == static_cast<CHAOS_IL2CPP_INTPTR>(77934256u))
	{
		chaos_data = chaos_static_field_data_NumericAggregationSubjects__PrivateImplementationDetails___BC7280150A400968EE578E3BB3A783D36E12EE252E41DD8E04486692B7D709D6;
		chaos_size = sizeof(chaos_static_field_data_NumericAggregationSubjects__PrivateImplementationDetails___BC7280150A400968EE578E3BB3A783D36E12EE252E41DD8E04486692B7D709D6);
		return true;
	}
	if (chaos_field_handle == static_cast<CHAOS_IL2CPP_INTPTR>(72489192u))
	{
		chaos_data = chaos_static_field_data_NumericAggregationSubjects__PrivateImplementationDetails___DF0B70CF4E7035ED4D043FA1B1C524FF2F098F8E1E0CED3A45962ED4A830D168;
		chaos_size = sizeof(chaos_static_field_data_NumericAggregationSubjects__PrivateImplementationDetails___DF0B70CF4E7035ED4D043FA1B1C524FF2F098F8E1E0CED3A45962ED4A830D168);
		return true;
	}
	if (chaos_field_handle == static_cast<CHAOS_IL2CPP_INTPTR>(67442793u))
	{
		chaos_data = chaos_static_field_data_NumericAggregationSubjects__PrivateImplementationDetails___E24846410E56684CEDBB55CFA4A0BB715A05EFE983172488FE80BBAF1D014FC7;
		chaos_size = sizeof(chaos_static_field_data_NumericAggregationSubjects__PrivateImplementationDetails___E24846410E56684CEDBB55CFA4A0BB715A05EFE983172488FE80BBAF1D014FC7);
		return true;
	}
	if (chaos_field_handle == static_cast<CHAOS_IL2CPP_INTPTR>(82134825u))
	{
		chaos_data = chaos_static_field_data_NumericAggregationSubjects__PrivateImplementationDetails___F0764F66820230734879BE00CD0504D53104DD9C68B3B78273DC9779573963EE;
		chaos_size = sizeof(chaos_static_field_data_NumericAggregationSubjects__PrivateImplementationDetails___F0764F66820230734879BE00CD0504D53104DD9C68B3B78273DC9779573963EE);
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

CHAOS_IL2CPP_ONCE_FLAG chaos_type_init_once_NumericAggregationSubjects_NumericAggregationSubjects___c;

void chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c()
{
	CHAOS_IL2CPP_CALL_ONCE(chaos_type_init_once_NumericAggregationSubjects_NumericAggregationSubjects___c, []()
	{
	if (chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9 == 0)
	{
	    auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_NumericAggregationSubjects_NumericAggregationSubjects___c);
	    chaos_object->header.type_info = &chaos_mt_NumericAggregationSubjects_NumericAggregationSubjects___c.hot;
		const auto chaos_arg_0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
		NumericAggregationSubjects_NumericAggregationSubjects___c__ctor(chaos_arg_0);
	    chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	});
}

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };

extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_0(void);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_1(void);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_2(void);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_3(void);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_4(void);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_5(void);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_6(void);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_7(void);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_8(void);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_9(void);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_10(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_10_b__11_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_11(void);
extern "C" CHAOS_IL2CPP_INT64 NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_11_b__12_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_12(void);
extern "C" float NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_12_b__13_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_13(void);
extern "C" double NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_13_b__14_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_14(void);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_14_b__15_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_15(void);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_15_b__16_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_16(void);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_16_b__17_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_17(void);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_17_b__18_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_18(void);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_18_b__19_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_19(void);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_19_b__20_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_20(void);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_21(void);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_22(void);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_23(void);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_24(void);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_25(void);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_26(void);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_27(void);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_28(void);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_29(void);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_30(void);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_31(void);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_32(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_32_b__33_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_33(void);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_33_b__34_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_34(void);
extern "C" CHAOS_IL2CPP_INT64 NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_34_b__35_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_35(void);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_35_b__36_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_36(void);
extern "C" float NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_36_b__37_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_37(void);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_37_b__38_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_38(void);
extern "C" double NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_38_b__39_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_39(void);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_39_b__40_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_40(void);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_40_b__41_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_41(void);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_41_b__42_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_42(void);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_42_b__43_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_43(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_43_b__44_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_44(void);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_44_b__45_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_45(void);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_46(void);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_47(void);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_48(void);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_49(void);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_50(void);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_51(void);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_52(void);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_53(void);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_54(void);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_55(void);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_56(void);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_57(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_57_b__58_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_58(void);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_58_b__59_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_59(void);
extern "C" CHAOS_IL2CPP_INT64 NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_59_b__60_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_60(void);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_60_b__61_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_61(void);
extern "C" float NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_61_b__62_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_62(void);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_62_b__63_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_63(void);
extern "C" double NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_63_b__64_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_64(void);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_64_b__65_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_65(void);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_65_b__66_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_66(void);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_66_b__67_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_67(void);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_67_b__68_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_68(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_68_b__69_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_69(void);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_69_b__70_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_70(void);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_71(void);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_72(void);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_73(void);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_74(void);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_75(void);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_76(void);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_77(void);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_78(void);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_79(void);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_80(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_80_b__81_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_81(void);
extern "C" CHAOS_IL2CPP_INT64 NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_81_b__82_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_82(void);
extern "C" float NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_82_b__83_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_83(void);
extern "C" double NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_83_b__84_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_84(void);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_84_b__85_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_85(void);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_85_b__86_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_86(void);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_86_b__87_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_87(void);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_87_b__88_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_88(void);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_88_b__89_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_89(void);
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_89_b__90_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects___c_cctor(void);
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects___c__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[139] = {
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
	2u,
	2u,
	3u,
	3u,
	4u,
	4u,
	5u,
	5u,
	6u,
	6u,
	7u,
	7u,
	8u,
	8u,
	9u,
	9u,
	10u,
	10u,
	10u,
	10u,
	10u,
	10u,
	10u,
	10u,
	10u,
	10u,
	10u,
	10u,
	10u,
	10u,
	11u,
	11u,
	12u,
	12u,
	13u,
	13u,
	14u,
	14u,
	15u,
	15u,
	16u,
	16u,
	17u,
	17u,
	18u,
	18u,
	19u,
	19u,
	20u,
	20u,
	21u,
	21u,
	22u,
	22u,
	23u,
	23u,
	23u,
	23u,
	23u,
	23u,
	23u,
	23u,
	23u,
	23u,
	23u,
	23u,
	23u,
	23u,
	24u,
	24u,
	25u,
	25u,
	26u,
	26u,
	27u,
	27u,
	28u,
	28u,
	29u,
	29u,
	30u,
	30u,
	31u,
	31u,
	32u,
	32u,
	33u,
	33u,
	34u,
	34u,
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
	40u,
	40u,
	41u,
	41u,
	42u,
	42u,
	43u,
	43u,
	44u,
	44u,
	45u,
	45u,
	46u,
	46u,
	46u,
};

static constexpr struct {
	::ChaosAbiManifestV0 header;
	::ChaosAbiMethodEntryV0 entries[138];
	CHAOS_IL2CPP_UINT8 params[46];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		138u,
		46u,
		3431181005u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_0
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_1
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_2
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_3
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_4
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_5
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_6
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_7
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_8
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_9
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_10
		{ 1u, 1u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_10_b__11_0_System_Int32
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_11
		{ 10u, 1u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_11_b__12_0_System_Int32
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_12
		{ 8u, 1u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_12_b__13_0_System_Int32
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_13
		{ 9u, 1u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_13_b__14_0_System_Int32
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_14
		{ 2u, 1u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_14_b__15_0_System_Int32
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_15
		{ 2u, 1u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_15_b__16_0_System_Int32
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_16
		{ 2u, 1u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_16_b__17_0_System_Int32
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_17
		{ 2u, 1u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_17_b__18_0_System_Int32
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_18
		{ 2u, 1u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_18_b__19_0_System_Int32
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_19
		{ 2u, 1u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_19_b__20_0_System_Int32
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_20
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_21
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_22
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_23
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_24
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_25
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_26
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_27
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_28
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_29
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_30
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_31
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_32
		{ 1u, 1u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_32_b__33_0_System_String
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_33
		{ 2u, 1u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_33_b__34_0_System_String
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_34
		{ 10u, 1u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_34_b__35_0_System_String
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_35
		{ 2u, 1u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_35_b__36_0_System_String
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_36
		{ 8u, 1u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_36_b__37_0_System_Int32
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_37
		{ 2u, 1u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_37_b__38_0_System_Int32
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_38
		{ 9u, 1u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_38_b__39_0_System_Int32
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_39
		{ 2u, 1u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_39_b__40_0_System_Int32
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_40
		{ 2u, 1u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_40_b__41_0_System_Int32
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_41
		{ 2u, 1u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_41_b__42_0_System_Int32
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_42
		{ 2u, 1u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_42_b__43_0_System_String
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_43
		{ 1u, 1u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_43_b__44_0_System_String
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_44
		{ 2u, 1u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_44_b__45_0_System_String
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_45
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_46
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_47
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_48
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_49
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_50
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_51
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_52
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_53
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_54
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_55
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_56
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_57
		{ 1u, 1u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_57_b__58_0_System_String
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_58
		{ 2u, 1u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_58_b__59_0_System_String
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_59
		{ 10u, 1u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_59_b__60_0_System_String
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_60
		{ 2u, 1u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_60_b__61_0_System_String
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_61
		{ 8u, 1u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_61_b__62_0_System_Int32
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_62
		{ 2u, 1u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_62_b__63_0_System_Int32
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_63
		{ 9u, 1u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_63_b__64_0_System_Int32
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_64
		{ 2u, 1u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_64_b__65_0_System_Int32
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_65
		{ 2u, 1u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_65_b__66_0_System_Int32
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_66
		{ 2u, 1u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_66_b__67_0_System_Int32
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_67
		{ 2u, 1u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_67_b__68_0_System_String
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_68
		{ 1u, 1u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_68_b__69_0_System_String
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_69
		{ 2u, 1u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_69_b__70_0_System_String
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_70
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_71
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_72
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_73
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_74
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_75
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_76
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_77
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_78
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_79
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_80
		{ 1u, 1u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_80_b__81_0_System_Int32
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_81
		{ 10u, 1u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_81_b__82_0_System_Int32
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_82
		{ 8u, 1u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_82_b__83_0_System_Int32
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_83
		{ 9u, 1u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_83_b__84_0_System_Int32
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_84
		{ 2u, 1u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_84_b__85_0_System_Int32
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_85
		{ 2u, 1u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_85_b__86_0_System_Int32
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_86
		{ 2u, 1u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_86_b__87_0_System_Int32
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_87
		{ 2u, 1u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_87_b__88_0_System_Int32
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_88
		{ 2u, 1u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_88_b__89_0_System_Int32
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_89
		{ 2u, 1u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_89_b__90_0_System_Int32
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects___c_cctor
		{ 0u, 0u },  // NumericAggregationSubjects_NumericAggregationSubjects___c__ctor
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
		2u,
		2u,
		2u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		2u,
		2u,
		2u,
		2u,
		2u,
		2u,
		2u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		2u,
		2u,
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
		"NumericAggregationSubjects",
		"<PrivateImplementationDetails>",
		"<>c",
		"__StaticArrayInitTypeSize=12",
		"__StaticArrayInitTypeSize=20",
		"__StaticArrayInitTypeSize=24",
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
		&chaos_mt_NumericAggregationSubjects__PrivateImplementationDetails_.hot,
		&chaos_mt_NumericAggregationSubjects_NumericAggregationSubjects___c.hot,
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
		/* .name_utf8         = */ "NumericAggregationSubjects",
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
		::chaos::il2cpp::runtime_core::RegisterModule("NumericAggregationSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[138] = {
	{ "CustomEntrySubject_0", 0x00000043u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_1", 0x00000044u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_2", 0x00000045u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_3", 0x00000046u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_4", 0x00000047u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_5", 0x00000048u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_6", 0x00000049u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_7", 0x0000004Au, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_8", 0x0000004Bu, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_9", 0x0000004Cu, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_10", 0x0000004Du, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_11", 0x0000004Eu, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_12", 0x0000004Fu, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_13", 0x00000050u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_14", 0x00000051u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_15", 0x00000052u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_16", 0x00000053u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_17", 0x00000054u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_18", 0x00000055u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_19", 0x00000056u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_20", 0x00000057u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_21", 0x00000058u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_22", 0x00000059u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_23", 0x0000005Au, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_24", 0x0000005Bu, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_25", 0x0000005Cu, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_26", 0x0000005Du, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_27", 0x0000005Eu, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_28", 0x0000005Fu, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_29", 0x00000060u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_30", 0x00000061u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_31", 0x00000062u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_32", 0x00000063u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_33", 0x00000064u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_34", 0x00000065u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_35", 0x00000066u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_36", 0x00000067u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_37", 0x00000068u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_38", 0x00000069u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_39", 0x0000006Au, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_40", 0x0000006Bu, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_41", 0x0000006Cu, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_42", 0x0000006Du, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_43", 0x0000006Eu, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_44", 0x0000006Fu, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_45", 0x00000070u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_46", 0x00000071u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_47", 0x00000072u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_48", 0x00000073u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_49", 0x00000074u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_50", 0x00000075u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_51", 0x00000076u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_52", 0x00000077u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_53", 0x00000078u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_54", 0x00000079u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_55", 0x0000007Au, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_56", 0x0000007Bu, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_57", 0x0000007Cu, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_58", 0x0000007Du, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_59", 0x0000007Eu, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_60", 0x0000007Fu, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_61", 0x00000080u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_62", 0x00000081u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_63", 0x00000082u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_64", 0x00000083u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_65", 0x00000084u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_66", 0x00000085u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_67", 0x00000086u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_68", 0x00000087u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_69", 0x00000088u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_70", 0x00000089u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_71", 0x0000008Au, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_72", 0x0000008Bu, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_73", 0x0000008Cu, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_74", 0x0000008Du, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_75", 0x0000008Eu, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_76", 0x0000008Fu, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_77", 0x00000090u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_78", 0x00000091u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_79", 0x00000092u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_80", 0x00000093u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_81", 0x00000094u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_82", 0x00000095u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_83", 0x00000096u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_84", 0x00000097u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_85", 0x00000098u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_86", 0x00000099u, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_87", 0x0000009Au, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_88", 0x0000009Bu, 0u },  // NumericAggregationSubjects
	{ "CustomEntrySubject_89", 0x0000009Cu, 0u },  // NumericAggregationSubjects
	{ "<CustomEntrySubject_10>b__11_0", 0x0000009Fu, 1u },  // NumericAggregationSubjects+<>c
	{ "<CustomEntrySubject_11>b__12_0", 0x000000A0u, 1u },  // NumericAggregationSubjects+<>c
	{ "<CustomEntrySubject_12>b__13_0", 0x000000A1u, 1u },  // NumericAggregationSubjects+<>c
	{ "<CustomEntrySubject_13>b__14_0", 0x000000A2u, 1u },  // NumericAggregationSubjects+<>c
	{ "<CustomEntrySubject_14>b__15_0", 0x000000A3u, 1u },  // NumericAggregationSubjects+<>c
	{ "<CustomEntrySubject_15>b__16_0", 0x000000A4u, 1u },  // NumericAggregationSubjects+<>c
	{ "<CustomEntrySubject_16>b__17_0", 0x000000A5u, 1u },  // NumericAggregationSubjects+<>c
	{ "<CustomEntrySubject_17>b__18_0", 0x000000A6u, 1u },  // NumericAggregationSubjects+<>c
	{ "<CustomEntrySubject_18>b__19_0", 0x000000A7u, 1u },  // NumericAggregationSubjects+<>c
	{ "<CustomEntrySubject_19>b__20_0", 0x000000A8u, 1u },  // NumericAggregationSubjects+<>c
	{ "<CustomEntrySubject_32>b__33_0", 0x000000A9u, 1u },  // NumericAggregationSubjects+<>c
	{ "<CustomEntrySubject_33>b__34_0", 0x000000AAu, 1u },  // NumericAggregationSubjects+<>c
	{ "<CustomEntrySubject_34>b__35_0", 0x000000ABu, 1u },  // NumericAggregationSubjects+<>c
	{ "<CustomEntrySubject_35>b__36_0", 0x000000ACu, 1u },  // NumericAggregationSubjects+<>c
	{ "<CustomEntrySubject_36>b__37_0", 0x000000ADu, 1u },  // NumericAggregationSubjects+<>c
	{ "<CustomEntrySubject_37>b__38_0", 0x000000AEu, 1u },  // NumericAggregationSubjects+<>c
	{ "<CustomEntrySubject_38>b__39_0", 0x000000AFu, 1u },  // NumericAggregationSubjects+<>c
	{ "<CustomEntrySubject_39>b__40_0", 0x000000B0u, 1u },  // NumericAggregationSubjects+<>c
	{ "<CustomEntrySubject_40>b__41_0", 0x000000B1u, 1u },  // NumericAggregationSubjects+<>c
	{ "<CustomEntrySubject_41>b__42_0", 0x000000B2u, 1u },  // NumericAggregationSubjects+<>c
	{ "<CustomEntrySubject_42>b__43_0", 0x000000B3u, 1u },  // NumericAggregationSubjects+<>c
	{ "<CustomEntrySubject_43>b__44_0", 0x000000B4u, 1u },  // NumericAggregationSubjects+<>c
	{ "<CustomEntrySubject_44>b__45_0", 0x000000B5u, 1u },  // NumericAggregationSubjects+<>c
	{ "<CustomEntrySubject_57>b__58_0", 0x000000B6u, 1u },  // NumericAggregationSubjects+<>c
	{ "<CustomEntrySubject_58>b__59_0", 0x000000B7u, 1u },  // NumericAggregationSubjects+<>c
	{ "<CustomEntrySubject_59>b__60_0", 0x000000B8u, 1u },  // NumericAggregationSubjects+<>c
	{ "<CustomEntrySubject_60>b__61_0", 0x000000B9u, 1u },  // NumericAggregationSubjects+<>c
	{ "<CustomEntrySubject_61>b__62_0", 0x000000BAu, 1u },  // NumericAggregationSubjects+<>c
	{ "<CustomEntrySubject_62>b__63_0", 0x000000BBu, 1u },  // NumericAggregationSubjects+<>c
	{ "<CustomEntrySubject_63>b__64_0", 0x000000BCu, 1u },  // NumericAggregationSubjects+<>c
	{ "<CustomEntrySubject_64>b__65_0", 0x000000BDu, 1u },  // NumericAggregationSubjects+<>c
	{ "<CustomEntrySubject_65>b__66_0", 0x000000BEu, 1u },  // NumericAggregationSubjects+<>c
	{ "<CustomEntrySubject_66>b__67_0", 0x000000BFu, 1u },  // NumericAggregationSubjects+<>c
	{ "<CustomEntrySubject_67>b__68_0", 0x000000C0u, 1u },  // NumericAggregationSubjects+<>c
	{ "<CustomEntrySubject_68>b__69_0", 0x000000C1u, 1u },  // NumericAggregationSubjects+<>c
	{ "<CustomEntrySubject_69>b__70_0", 0x000000C2u, 1u },  // NumericAggregationSubjects+<>c
	{ "<CustomEntrySubject_80>b__81_0", 0x000000C3u, 1u },  // NumericAggregationSubjects+<>c
	{ "<CustomEntrySubject_81>b__82_0", 0x000000C4u, 1u },  // NumericAggregationSubjects+<>c
	{ "<CustomEntrySubject_82>b__83_0", 0x000000C5u, 1u },  // NumericAggregationSubjects+<>c
	{ "<CustomEntrySubject_83>b__84_0", 0x000000C6u, 1u },  // NumericAggregationSubjects+<>c
	{ "<CustomEntrySubject_84>b__85_0", 0x000000C7u, 1u },  // NumericAggregationSubjects+<>c
	{ "<CustomEntrySubject_85>b__86_0", 0x000000C8u, 1u },  // NumericAggregationSubjects+<>c
	{ "<CustomEntrySubject_86>b__87_0", 0x000000C9u, 1u },  // NumericAggregationSubjects+<>c
	{ "<CustomEntrySubject_87>b__88_0", 0x000000CAu, 1u },  // NumericAggregationSubjects+<>c
	{ "<CustomEntrySubject_88>b__89_0", 0x000000CBu, 1u },  // NumericAggregationSubjects+<>c
	{ "<CustomEntrySubject_89>b__90_0", 0x000000CCu, 1u },  // NumericAggregationSubjects+<>c
	{ ".cctor", 0x0000009Du, 0u },  // NumericAggregationSubjects+<>c
	{ ".ctor", 0x0000009Eu, 0u },  // NumericAggregationSubjects+<>c
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[2] = {
	{ "NumericAggregationSubjects", "", 0u, 90u },
	{ "NumericAggregationSubjects+<>c", "", 90u, 48u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[138] = {
	{ 0x00000043u, 0u },
	{ 0x00000044u, 1u },
	{ 0x00000045u, 2u },
	{ 0x00000046u, 3u },
	{ 0x00000047u, 4u },
	{ 0x00000048u, 5u },
	{ 0x00000049u, 6u },
	{ 0x0000004Au, 7u },
	{ 0x0000004Bu, 8u },
	{ 0x0000004Cu, 9u },
	{ 0x0000004Du, 10u },
	{ 0x0000004Eu, 12u },
	{ 0x0000004Fu, 14u },
	{ 0x00000050u, 16u },
	{ 0x00000051u, 18u },
	{ 0x00000052u, 20u },
	{ 0x00000053u, 22u },
	{ 0x00000054u, 24u },
	{ 0x00000055u, 26u },
	{ 0x00000056u, 28u },
	{ 0x00000057u, 30u },
	{ 0x00000058u, 31u },
	{ 0x00000059u, 32u },
	{ 0x0000005Au, 33u },
	{ 0x0000005Bu, 34u },
	{ 0x0000005Cu, 35u },
	{ 0x0000005Du, 36u },
	{ 0x0000005Eu, 37u },
	{ 0x0000005Fu, 38u },
	{ 0x00000060u, 39u },
	{ 0x00000061u, 40u },
	{ 0x00000062u, 41u },
	{ 0x00000063u, 42u },
	{ 0x00000064u, 44u },
	{ 0x00000065u, 46u },
	{ 0x00000066u, 48u },
	{ 0x00000067u, 50u },
	{ 0x00000068u, 52u },
	{ 0x00000069u, 54u },
	{ 0x0000006Au, 56u },
	{ 0x0000006Bu, 58u },
	{ 0x0000006Cu, 60u },
	{ 0x0000006Du, 62u },
	{ 0x0000006Eu, 64u },
	{ 0x0000006Fu, 66u },
	{ 0x00000070u, 68u },
	{ 0x00000071u, 69u },
	{ 0x00000072u, 70u },
	{ 0x00000073u, 71u },
	{ 0x00000074u, 72u },
	{ 0x00000075u, 73u },
	{ 0x00000076u, 74u },
	{ 0x00000077u, 75u },
	{ 0x00000078u, 76u },
	{ 0x00000079u, 77u },
	{ 0x0000007Au, 78u },
	{ 0x0000007Bu, 79u },
	{ 0x0000007Cu, 80u },
	{ 0x0000007Du, 82u },
	{ 0x0000007Eu, 84u },
	{ 0x0000007Fu, 86u },
	{ 0x00000080u, 88u },
	{ 0x00000081u, 90u },
	{ 0x00000082u, 92u },
	{ 0x00000083u, 94u },
	{ 0x00000084u, 96u },
	{ 0x00000085u, 98u },
	{ 0x00000086u, 100u },
	{ 0x00000087u, 102u },
	{ 0x00000088u, 104u },
	{ 0x00000089u, 106u },
	{ 0x0000008Au, 107u },
	{ 0x0000008Bu, 108u },
	{ 0x0000008Cu, 109u },
	{ 0x0000008Du, 110u },
	{ 0x0000008Eu, 111u },
	{ 0x0000008Fu, 112u },
	{ 0x00000090u, 113u },
	{ 0x00000091u, 114u },
	{ 0x00000092u, 115u },
	{ 0x00000093u, 116u },
	{ 0x00000094u, 118u },
	{ 0x00000095u, 120u },
	{ 0x00000096u, 122u },
	{ 0x00000097u, 124u },
	{ 0x00000098u, 126u },
	{ 0x00000099u, 128u },
	{ 0x0000009Au, 130u },
	{ 0x0000009Bu, 132u },
	{ 0x0000009Cu, 134u },
	{ 0x0000009Du, 136u },
	{ 0x0000009Eu, 137u },
	{ 0x0000009Fu, 11u },
	{ 0x000000A0u, 13u },
	{ 0x000000A1u, 15u },
	{ 0x000000A2u, 17u },
	{ 0x000000A3u, 19u },
	{ 0x000000A4u, 21u },
	{ 0x000000A5u, 23u },
	{ 0x000000A6u, 25u },
	{ 0x000000A7u, 27u },
	{ 0x000000A8u, 29u },
	{ 0x000000A9u, 43u },
	{ 0x000000AAu, 45u },
	{ 0x000000ABu, 47u },
	{ 0x000000ACu, 49u },
	{ 0x000000ADu, 51u },
	{ 0x000000AEu, 53u },
	{ 0x000000AFu, 55u },
	{ 0x000000B0u, 57u },
	{ 0x000000B1u, 59u },
	{ 0x000000B2u, 61u },
	{ 0x000000B3u, 63u },
	{ 0x000000B4u, 65u },
	{ 0x000000B5u, 67u },
	{ 0x000000B6u, 81u },
	{ 0x000000B7u, 83u },
	{ 0x000000B8u, 85u },
	{ 0x000000B9u, 87u },
	{ 0x000000BAu, 89u },
	{ 0x000000BBu, 91u },
	{ 0x000000BCu, 93u },
	{ 0x000000BDu, 95u },
	{ 0x000000BEu, 97u },
	{ 0x000000BFu, 99u },
	{ 0x000000C0u, 101u },
	{ 0x000000C1u, 103u },
	{ 0x000000C2u, 105u },
	{ 0x000000C3u, 117u },
	{ 0x000000C4u, 119u },
	{ 0x000000C5u, 121u },
	{ 0x000000C6u, 123u },
	{ 0x000000C7u, 125u },
	{ 0x000000C8u, 127u },
	{ 0x000000C9u, 129u },
	{ 0x000000CAu, 131u },
	{ 0x000000CBu, 133u },
	{ 0x000000CCu, 135u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[138] = {
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_1
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_2
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_3
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_4
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_5
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_6
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_7
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_8
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_9), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_9
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_10), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_10
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_10_b__11_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NumericAggregationSubjects+<>c::<CustomEntrySubject_10>b__11_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_11), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_11
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_11_b__12_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NumericAggregationSubjects+<>c::<CustomEntrySubject_11>b__12_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_12), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_12
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_12_b__13_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NumericAggregationSubjects+<>c::<CustomEntrySubject_12>b__13_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_13), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_13
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_13_b__14_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NumericAggregationSubjects+<>c::<CustomEntrySubject_13>b__14_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_14), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_14
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_14_b__15_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects+<>c::<CustomEntrySubject_14>b__15_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_15), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_15
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_15_b__16_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects+<>c::<CustomEntrySubject_15>b__16_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_16), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_16
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_16_b__17_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects+<>c::<CustomEntrySubject_16>b__17_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_17), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_17
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_17_b__18_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects+<>c::<CustomEntrySubject_17>b__18_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_18), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_18
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_18_b__19_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects+<>c::<CustomEntrySubject_18>b__19_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_19), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_19
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_19_b__20_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects+<>c::<CustomEntrySubject_19>b__20_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_20), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_20
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_21), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_21
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_22), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_22
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_23), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_23
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_24), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_24
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_25), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_25
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_26), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_26
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_27), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_27
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_28), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_28
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_29), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_29
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_30), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_30
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_31), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_31
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_32
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_32_b__33_0_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects+<>c::<CustomEntrySubject_32>b__33_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_33), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_33
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_33_b__34_0_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects+<>c::<CustomEntrySubject_33>b__34_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_34), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_34
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_34_b__35_0_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects+<>c::<CustomEntrySubject_34>b__35_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_35), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_35
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_35_b__36_0_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects+<>c::<CustomEntrySubject_35>b__36_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_36), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_36
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_36_b__37_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NumericAggregationSubjects+<>c::<CustomEntrySubject_36>b__37_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_37), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_37
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_37_b__38_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects+<>c::<CustomEntrySubject_37>b__38_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_38), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_38
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_38_b__39_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NumericAggregationSubjects+<>c::<CustomEntrySubject_38>b__39_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_39), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_39
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_39_b__40_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects+<>c::<CustomEntrySubject_39>b__40_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_40), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_40
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_40_b__41_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects+<>c::<CustomEntrySubject_40>b__41_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_41), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_41
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_41_b__42_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects+<>c::<CustomEntrySubject_41>b__42_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_42), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_42
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_42_b__43_0_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NumericAggregationSubjects+<>c::<CustomEntrySubject_42>b__43_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_43), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_43
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_43_b__44_0_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects+<>c::<CustomEntrySubject_43>b__44_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_44), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_44
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_44_b__45_0_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NumericAggregationSubjects+<>c::<CustomEntrySubject_44>b__45_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_45), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_45
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_46), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_46
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_47), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_47
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_48), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_48
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_49), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_49
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_50), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_50
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_51), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_51
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_52), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_52
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_53), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_53
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_54), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_54
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_55), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_55
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_56), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_56
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_57), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_57
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_57_b__58_0_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects+<>c::<CustomEntrySubject_57>b__58_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_58), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_58
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_58_b__59_0_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects+<>c::<CustomEntrySubject_58>b__59_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_59), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_59
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_59_b__60_0_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects+<>c::<CustomEntrySubject_59>b__60_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_60), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_60
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_60_b__61_0_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects+<>c::<CustomEntrySubject_60>b__61_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_61), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_61
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_61_b__62_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NumericAggregationSubjects+<>c::<CustomEntrySubject_61>b__62_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_62), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_62
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_62_b__63_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects+<>c::<CustomEntrySubject_62>b__63_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_63), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_63
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_63_b__64_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NumericAggregationSubjects+<>c::<CustomEntrySubject_63>b__64_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_64), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_64
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_64_b__65_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects+<>c::<CustomEntrySubject_64>b__65_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_65), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_65
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_65_b__66_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects+<>c::<CustomEntrySubject_65>b__66_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_66), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_66
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_66_b__67_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects+<>c::<CustomEntrySubject_66>b__67_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_67), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_67
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_67_b__68_0_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NumericAggregationSubjects+<>c::<CustomEntrySubject_67>b__68_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_68), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_68
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_68_b__69_0_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects+<>c::<CustomEntrySubject_68>b__69_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_69), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_69
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_69_b__70_0_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NumericAggregationSubjects+<>c::<CustomEntrySubject_69>b__70_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_70), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_70
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_71), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_71
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_72), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_72
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_73), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_73
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_74), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_74
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_75), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_75
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_76), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_76
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_77), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_77
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_78), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_78
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_79), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_79
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_80), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_80
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_80_b__81_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NumericAggregationSubjects+<>c::<CustomEntrySubject_80>b__81_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_81), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_81
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_81_b__82_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NumericAggregationSubjects+<>c::<CustomEntrySubject_81>b__82_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_82), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_82
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_82_b__83_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NumericAggregationSubjects+<>c::<CustomEntrySubject_82>b__83_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_83), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_83
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_83_b__84_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NumericAggregationSubjects+<>c::<CustomEntrySubject_83>b__84_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_84), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_84
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_84_b__85_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects+<>c::<CustomEntrySubject_84>b__85_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_85), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_85
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_85_b__86_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects+<>c::<CustomEntrySubject_85>b__86_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_86), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_86
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_86_b__87_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects+<>c::<CustomEntrySubject_86>b__87_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_87), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_87
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_87_b__88_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects+<>c::<CustomEntrySubject_87>b__88_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_88), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_88
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_88_b__89_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects+<>c::<CustomEntrySubject_88>b__89_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_89), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects::CustomEntrySubject_89
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_89_b__90_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects+<>c::<CustomEntrySubject_89>b__90_0
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_cctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NumericAggregationSubjects+<>c::.cctor
	{ reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NumericAggregationSubjects+<>c::.ctor
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"NumericAggregationSubjects",
	s_hotpatch_types,
	2u,
	s_hotpatch_methods,
	138u,
	s_hotpatch_slots,
	138u,
	s_hotpatch_entries,
	138u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[186] = {
	"System.Private.CoreLib/System.Int32",
	"NumericAggregationSubjects/<PrivateImplementationDetails>::4F6ADDC9659D6FB90FE94B6688A79F2A1FA8D36EC43F8F3E1D9B6528C448A384",
	"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)",
	"System.Linq/Enumerable::Average:System.Double(System.Collections.Generic.IEnumerable<System.Int32>)",
	"System.Private.CoreLib/System.Int64",
	"NumericAggregationSubjects/<PrivateImplementationDetails>::E24846410E56684CEDBB55CFA4A0BB715A05EFE983172488FE80BBAF1D014FC7",
	"System.Linq/Enumerable::Average:System.Double(System.Collections.Generic.IEnumerable<System.Int64>)",
	"System.Private.CoreLib/System.Single",
	"NumericAggregationSubjects/<PrivateImplementationDetails>::BC7280150A400968EE578E3BB3A783D36E12EE252E41DD8E04486692B7D709D6",
	"System.Linq/Enumerable::Average:System.Single(System.Collections.Generic.IEnumerable<System.Single>)",
	"System.Private.CoreLib/System.Double",
	"NumericAggregationSubjects/<PrivateImplementationDetails>::AD1461355D664226DF0A9DD36082CE06AFA03D52A19BEB8E90853216EF6B221E",
	"System.Linq/Enumerable::Average:System.Double(System.Collections.Generic.IEnumerable<System.Double>)",
	"System.Private.CoreLib/System.Decimal",
	"System.Private.CoreLib/System.Decimal::.ctor:System.Void(System.Int32,System.Int32,System.Int32,System.Boolean,System.Byte)",
	"System.Linq/Enumerable::Average:System.Decimal(System.Collections.Generic.IEnumerable<System.Decimal>)",
	"System.Private.CoreLib/System.Nullable<System.Int32>",
	"System.Private.CoreLib/System.Nullable<System.Int32>::.ctor:System.Void(System.Int32)",
	"System.Linq/Enumerable::Average:System.Nullable<System.Double>(System.Collections.Generic.IEnumerable<System.Nullable<System.Int32>>)",
	"System.Private.CoreLib/System.Nullable<System.Int64>",
	"System.Private.CoreLib/System.Nullable<System.Int64>::.ctor:System.Void(System.Int64)",
	"System.Linq/Enumerable::Average:System.Nullable<System.Double>(System.Collections.Generic.IEnumerable<System.Nullable<System.Int64>>)",
	"System.Private.CoreLib/System.Nullable<System.Single>",
	"System.Private.CoreLib/System.Nullable<System.Single>::.ctor:System.Void(System.Single)",
	"System.Linq/Enumerable::Average:System.Nullable<System.Single>(System.Collections.Generic.IEnumerable<System.Nullable<System.Single>>)",
	"System.Private.CoreLib/System.Nullable<System.Double>",
	"System.Private.CoreLib/System.Nullable<System.Double>::.ctor:System.Void(System.Double)",
	"System.Linq/Enumerable::Average:System.Nullable<System.Double>(System.Collections.Generic.IEnumerable<System.Nullable<System.Double>>)",
	"System.Private.CoreLib/System.Nullable<System.Decimal>",
	"System.Private.CoreLib/System.Nullable<System.Decimal>::.ctor:System.Void(System.Decimal)",
	"System.Linq/Enumerable::Average:System.Nullable<System.Decimal>(System.Collections.Generic.IEnumerable<System.Nullable<System.Decimal>>)",
	"NumericAggregationSubjects/<PrivateImplementationDetails>::4636993D3E1DA4E9D6B8F87B79E8F7C6D018580D52661950EABC3845C5897A4D",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9__11_0",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9",
	"System.Private.CoreLib/System.Func<System.Int32,System.Int32>::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Linq/Enumerable::Average<System.Int32>:System.Double(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9__12_0",
	"System.Private.CoreLib/System.Func<System.Int32,System.Int64>::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Linq/Enumerable::Average<System.Int32>:System.Double(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int64>)",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9__13_0",
	"System.Private.CoreLib/System.Func<System.Int32,System.Single>::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Linq/Enumerable::Average<System.Int32>:System.Single(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Single>)",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9__14_0",
	"System.Private.CoreLib/System.Func<System.Int32,System.Double>::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Linq/Enumerable::Average<System.Int32>:System.Double(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Double>)",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9__15_0",
	"System.Private.CoreLib/System.Func<System.Int32,System.Decimal>::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Linq/Enumerable::Average<System.Int32>:System.Decimal(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Decimal>)",
	"System.Private.CoreLib/System.Decimal::op_Implicit:System.Decimal(System.Int32)",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9__16_0",
	"System.Private.CoreLib/System.Func<System.Int32,System.Nullable<System.Int32>>::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Linq/Enumerable::Average<System.Int32>:System.Nullable<System.Double>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Nullable<System.Int32>>)",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9__17_0",
	"System.Private.CoreLib/System.Func<System.Int32,System.Nullable<System.Int64>>::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Linq/Enumerable::Average<System.Int32>:System.Nullable<System.Double>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Nullable<System.Int64>>)",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9__18_0",
	"System.Private.CoreLib/System.Func<System.Int32,System.Nullable<System.Single>>::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Linq/Enumerable::Average<System.Int32>:System.Nullable<System.Single>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Nullable<System.Single>>)",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9__19_0",
	"System.Private.CoreLib/System.Func<System.Int32,System.Nullable<System.Double>>::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Linq/Enumerable::Average<System.Int32>:System.Nullable<System.Double>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Nullable<System.Double>>)",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9__20_0",
	"System.Private.CoreLib/System.Func<System.Int32,System.Nullable<System.Decimal>>::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Linq/Enumerable::Average<System.Int32>:System.Nullable<System.Decimal>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Nullable<System.Decimal>>)",
	"NumericAggregationSubjects/<PrivateImplementationDetails>::F0764F66820230734879BE00CD0504D53104DD9C68B3B78273DC9779573963EE",
	"System.Linq/Enumerable::Max:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>)",
	"NumericAggregationSubjects/<PrivateImplementationDetails>::5E8AACDD15ECEBAFF7B1FB3797E322DE0FBC1A26201A13F545818DC8961C5D36",
	"System.Linq/Enumerable::Max:System.Int64(System.Collections.Generic.IEnumerable<System.Int64>)",
	"System.Linq/Enumerable::Max:System.Nullable<System.Int32>(System.Collections.Generic.IEnumerable<System.Nullable<System.Int32>>)",
	"System.Linq/Enumerable::Max:System.Nullable<System.Int64>(System.Collections.Generic.IEnumerable<System.Nullable<System.Int64>>)",
	"NumericAggregationSubjects/<PrivateImplementationDetails>::58F0E9F70561D14C7C4E5D2FF8D28FF308BBCADACE0C72A56EC7ACD9F9227D19",
	"System.Linq/Enumerable::Max:System.Double(System.Collections.Generic.IEnumerable<System.Double>)",
	"System.Linq/Enumerable::Max:System.Nullable<System.Double>(System.Collections.Generic.IEnumerable<System.Nullable<System.Double>>)",
	"NumericAggregationSubjects/<PrivateImplementationDetails>::182CD2C8E82B98EC53B1CA4C202076C607114AA7401B0767536843C1ABBAFFF9",
	"System.Linq/Enumerable::Max:System.Single(System.Collections.Generic.IEnumerable<System.Single>)",
	"System.Linq/Enumerable::Max:System.Nullable<System.Single>(System.Collections.Generic.IEnumerable<System.Nullable<System.Single>>)",
	"System.Linq/Enumerable::Max:System.Decimal(System.Collections.Generic.IEnumerable<System.Decimal>)",
	"System.Linq/Enumerable::Max:System.Nullable<System.Decimal>(System.Collections.Generic.IEnumerable<System.Nullable<System.Decimal>>)",
	"System.Private.CoreLib/System.String",
	"System.Linq/Enumerable::Max<System.String>:System.String(System.Collections.Generic.IEnumerable<System.String>)",
	"System.Private.CoreLib/System.StringComparer::get_OrdinalIgnoreCase:System.StringComparer()",
	"System.Linq/Enumerable::Max<System.String>:System.String(System.Collections.Generic.IEnumerable<System.String>,System.Collections.Generic.IComparer<System.String>)",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9__33_0",
	"System.Private.CoreLib/System.Func<System.String,System.Int32>::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Linq/Enumerable::Max<System.String>:System.Int32(System.Collections.Generic.IEnumerable<System.String>,System.Func<System.String,System.Int32>)",
	"System.Private.CoreLib/System.String::get_Length:System.Int32()",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9__34_0",
	"System.Private.CoreLib/System.Func<System.String,System.Nullable<System.Int32>>::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Linq/Enumerable::Max<System.String>:System.Nullable<System.Int32>(System.Collections.Generic.IEnumerable<System.String>,System.Func<System.String,System.Nullable<System.Int32>>)",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9__35_0",
	"System.Private.CoreLib/System.Func<System.String,System.Int64>::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Linq/Enumerable::Max<System.String>:System.Int64(System.Collections.Generic.IEnumerable<System.String>,System.Func<System.String,System.Int64>)",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9__36_0",
	"System.Private.CoreLib/System.Func<System.String,System.Nullable<System.Int64>>::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Linq/Enumerable::Max<System.String>:System.Nullable<System.Int64>(System.Collections.Generic.IEnumerable<System.String>,System.Func<System.String,System.Nullable<System.Int64>>)",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9__37_0",
	"System.Linq/Enumerable::Max<System.Int32>:System.Single(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Single>)",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9__38_0",
	"System.Linq/Enumerable::Max<System.Int32>:System.Nullable<System.Single>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Nullable<System.Single>>)",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9__39_0",
	"System.Linq/Enumerable::Max<System.Int32>:System.Double(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Double>)",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9__40_0",
	"System.Linq/Enumerable::Max<System.Int32>:System.Nullable<System.Double>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Nullable<System.Double>>)",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9__41_0",
	"System.Linq/Enumerable::Max<System.Int32>:System.Decimal(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Decimal>)",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9__42_0",
	"System.Linq/Enumerable::Max<System.Int32>:System.Nullable<System.Decimal>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Nullable<System.Decimal>>)",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9__43_0",
	"System.Private.CoreLib/System.Func<System.String,System.String>::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Linq/Enumerable::Max<System.String,System.String>:System.String(System.Collections.Generic.IEnumerable<System.String>,System.Func<System.String,System.String>)",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9__44_0",
	"System.Linq/Enumerable::MaxBy<System.String,System.Int32>:System.String(System.Collections.Generic.IEnumerable<System.String>,System.Func<System.String,System.Int32>)",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9__45_0",
	"System.Linq/Enumerable::MaxBy<System.String,System.String>:System.String(System.Collections.Generic.IEnumerable<System.String>,System.Func<System.String,System.String>,System.Collections.Generic.IComparer<System.String>)",
	"System.Linq/Enumerable::Min:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>)",
	"NumericAggregationSubjects/<PrivateImplementationDetails>::9AA6B7E9A2041E57FF4210A87426C778833120D5C972E859E0711CE16D883EA3",
	"System.Linq/Enumerable::Min:System.Int64(System.Collections.Generic.IEnumerable<System.Int64>)",
	"System.Linq/Enumerable::Min:System.Nullable<System.Int32>(System.Collections.Generic.IEnumerable<System.Nullable<System.Int32>>)",
	"System.Linq/Enumerable::Min:System.Nullable<System.Int64>(System.Collections.Generic.IEnumerable<System.Nullable<System.Int64>>)",
	"NumericAggregationSubjects/<PrivateImplementationDetails>::6B99172F78B2670437EC662F13B85B0B2135FFB49E3952F469FAC00C9646399E",
	"System.Linq/Enumerable::Min:System.Single(System.Collections.Generic.IEnumerable<System.Single>)",
	"System.Linq/Enumerable::Min:System.Nullable<System.Single>(System.Collections.Generic.IEnumerable<System.Nullable<System.Single>>)",
	"NumericAggregationSubjects/<PrivateImplementationDetails>::DF0B70CF4E7035ED4D043FA1B1C524FF2F098F8E1E0CED3A45962ED4A830D168",
	"System.Linq/Enumerable::Min:System.Double(System.Collections.Generic.IEnumerable<System.Double>)",
	"System.Linq/Enumerable::Min:System.Nullable<System.Double>(System.Collections.Generic.IEnumerable<System.Nullable<System.Double>>)",
	"System.Linq/Enumerable::Min:System.Decimal(System.Collections.Generic.IEnumerable<System.Decimal>)",
	"System.Linq/Enumerable::Min:System.Nullable<System.Decimal>(System.Collections.Generic.IEnumerable<System.Nullable<System.Decimal>>)",
	"System.Linq/Enumerable::Min<System.String>:System.String(System.Collections.Generic.IEnumerable<System.String>)",
	"System.Linq/Enumerable::Min<System.String>:System.String(System.Collections.Generic.IEnumerable<System.String>,System.Collections.Generic.IComparer<System.String>)",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9__58_0",
	"System.Linq/Enumerable::Min<System.String>:System.Int32(System.Collections.Generic.IEnumerable<System.String>,System.Func<System.String,System.Int32>)",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9__59_0",
	"System.Linq/Enumerable::Min<System.String>:System.Nullable<System.Int32>(System.Collections.Generic.IEnumerable<System.String>,System.Func<System.String,System.Nullable<System.Int32>>)",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9__60_0",
	"System.Linq/Enumerable::Min<System.String>:System.Int64(System.Collections.Generic.IEnumerable<System.String>,System.Func<System.String,System.Int64>)",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9__61_0",
	"System.Linq/Enumerable::Min<System.String>:System.Nullable<System.Int64>(System.Collections.Generic.IEnumerable<System.String>,System.Func<System.String,System.Nullable<System.Int64>>)",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9__62_0",
	"System.Linq/Enumerable::Min<System.Int32>:System.Single(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Single>)",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9__63_0",
	"System.Linq/Enumerable::Min<System.Int32>:System.Nullable<System.Single>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Nullable<System.Single>>)",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9__64_0",
	"System.Linq/Enumerable::Min<System.Int32>:System.Double(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Double>)",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9__65_0",
	"System.Linq/Enumerable::Min<System.Int32>:System.Nullable<System.Double>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Nullable<System.Double>>)",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9__66_0",
	"System.Linq/Enumerable::Min<System.Int32>:System.Decimal(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Decimal>)",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9__67_0",
	"System.Linq/Enumerable::Min<System.Int32>:System.Nullable<System.Decimal>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Nullable<System.Decimal>>)",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9__68_0",
	"System.Linq/Enumerable::Min<System.String,System.String>:System.String(System.Collections.Generic.IEnumerable<System.String>,System.Func<System.String,System.String>)",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9__69_0",
	"System.Linq/Enumerable::MinBy<System.String,System.Int32>:System.String(System.Collections.Generic.IEnumerable<System.String>,System.Func<System.String,System.Int32>)",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9__70_0",
	"System.Linq/Enumerable::MinBy<System.String,System.String>:System.String(System.Collections.Generic.IEnumerable<System.String>,System.Func<System.String,System.String>,System.Collections.Generic.IComparer<System.String>)",
	"System.Linq/Enumerable::Sum:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>)",
	"System.Linq/Enumerable::Sum:System.Int64(System.Collections.Generic.IEnumerable<System.Int64>)",
	"System.Linq/Enumerable::Sum:System.Single(System.Collections.Generic.IEnumerable<System.Single>)",
	"System.Linq/Enumerable::Sum:System.Double(System.Collections.Generic.IEnumerable<System.Double>)",
	"System.Linq/Enumerable::Sum:System.Decimal(System.Collections.Generic.IEnumerable<System.Decimal>)",
	"System.Linq/Enumerable::Sum:System.Nullable<System.Int32>(System.Collections.Generic.IEnumerable<System.Nullable<System.Int32>>)",
	"System.Linq/Enumerable::Sum:System.Nullable<System.Int64>(System.Collections.Generic.IEnumerable<System.Nullable<System.Int64>>)",
	"System.Linq/Enumerable::Sum:System.Nullable<System.Single>(System.Collections.Generic.IEnumerable<System.Nullable<System.Single>>)",
	"System.Linq/Enumerable::Sum:System.Nullable<System.Double>(System.Collections.Generic.IEnumerable<System.Nullable<System.Double>>)",
	"System.Linq/Enumerable::Sum:System.Nullable<System.Decimal>(System.Collections.Generic.IEnumerable<System.Nullable<System.Decimal>>)",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9__81_0",
	"System.Linq/Enumerable::Sum<System.Int32>:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9__82_0",
	"System.Linq/Enumerable::Sum<System.Int32>:System.Int64(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int64>)",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9__83_0",
	"System.Linq/Enumerable::Sum<System.Int32>:System.Single(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Single>)",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9__84_0",
	"System.Linq/Enumerable::Sum<System.Int32>:System.Double(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Double>)",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9__85_0",
	"System.Linq/Enumerable::Sum<System.Int32>:System.Decimal(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Decimal>)",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9__86_0",
	"System.Linq/Enumerable::Sum<System.Int32>:System.Nullable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Nullable<System.Int32>>)",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9__87_0",
	"System.Linq/Enumerable::Sum<System.Int32>:System.Nullable<System.Int64>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Nullable<System.Int64>>)",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9__88_0",
	"System.Linq/Enumerable::Sum<System.Int32>:System.Nullable<System.Single>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Nullable<System.Single>>)",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9__89_0",
	"System.Linq/Enumerable::Sum<System.Int32>:System.Nullable<System.Double>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Nullable<System.Double>>)",
	"NumericAggregationSubjects/NumericAggregationSubjects+<>c::<>9__90_0",
	"System.Linq/Enumerable::Sum<System.Int32>:System.Nullable<System.Decimal>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Nullable<System.Decimal>>)",
	"System.Private.CoreLib/System.Object::.ctor:System.Void()",
};

extern "C" void* kChaosExternalRuntimeFnTable[186] = {
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
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__),
};

extern "C" int32_t kChaosExternalRuntimeCount = 186;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[138])() = {
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_0),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_1),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_2),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_3),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_4),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_5),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_6),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_7),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_8),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_9),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_10),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_10_b__11_0_System_Int32),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_11),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_11_b__12_0_System_Int32),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_12),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_12_b__13_0_System_Int32),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_13),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_13_b__14_0_System_Int32),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_14),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_14_b__15_0_System_Int32),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_15),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_15_b__16_0_System_Int32),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_16),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_16_b__17_0_System_Int32),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_17),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_17_b__18_0_System_Int32),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_18),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_18_b__19_0_System_Int32),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_19),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_19_b__20_0_System_Int32),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_20),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_21),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_22),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_23),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_24),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_25),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_26),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_27),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_28),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_29),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_30),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_31),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_32),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_32_b__33_0_System_String),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_33),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_33_b__34_0_System_String),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_34),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_34_b__35_0_System_String),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_35),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_35_b__36_0_System_String),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_36),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_36_b__37_0_System_Int32),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_37),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_37_b__38_0_System_Int32),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_38),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_38_b__39_0_System_Int32),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_39),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_39_b__40_0_System_Int32),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_40),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_40_b__41_0_System_Int32),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_41),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_41_b__42_0_System_Int32),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_42),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_42_b__43_0_System_String),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_43),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_43_b__44_0_System_String),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_44),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_44_b__45_0_System_String),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_45),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_46),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_47),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_48),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_49),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_50),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_51),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_52),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_53),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_54),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_55),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_56),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_57),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_57_b__58_0_System_String),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_58),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_58_b__59_0_System_String),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_59),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_59_b__60_0_System_String),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_60),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_60_b__61_0_System_String),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_61),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_61_b__62_0_System_Int32),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_62),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_62_b__63_0_System_Int32),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_63),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_63_b__64_0_System_Int32),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_64),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_64_b__65_0_System_Int32),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_65),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_65_b__66_0_System_Int32),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_66),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_66_b__67_0_System_Int32),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_67),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_67_b__68_0_System_String),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_68),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_68_b__69_0_System_String),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_69),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_69_b__70_0_System_String),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_70),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_71),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_72),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_73),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_74),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_75),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_76),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_77),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_78),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_79),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_80),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_80_b__81_0_System_Int32),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_81),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_81_b__82_0_System_Int32),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_82),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_82_b__83_0_System_Int32),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_83),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_83_b__84_0_System_Int32),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_84),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_84_b__85_0_System_Int32),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_85),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_85_b__86_0_System_Int32),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_86),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_86_b__87_0_System_Int32),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_87),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_87_b__88_0_System_Int32),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_88),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_88_b__89_0_System_Int32),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_89),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_89_b__90_0_System_Int32),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c_cctor),
	reinterpret_cast<void(*)()>(&NumericAggregationSubjects_NumericAggregationSubjects___c__ctor),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
extern "C" void (*kBenchmarkWrappers[138])() = {
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
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[11])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[12]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[13])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[14]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[15])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[16]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[17])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[18]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[19])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[20]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[21])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[22]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[23])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[24]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[25])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[26]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[27])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[28]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[29])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[30]();},
	[]() {kAotMethods[31]();},
	[]() {kAotMethods[32]();},
	[]() {kAotMethods[33]();},
	[]() {kAotMethods[34]();},
	[]() {kAotMethods[35]();},
	[]() {kAotMethods[36]();},
	[]() {kAotMethods[37]();},
	[]() {kAotMethods[38]();},
	[]() {kAotMethods[39]();},
	[]() {kAotMethods[40]();},
	[]() {kAotMethods[41]();},
	[]() {kAotMethods[42]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[43])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),chaos_make_string_id_value(8492632318578411777ULL));},
	[]() {kAotMethods[44]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[45])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),chaos_make_string_id_value(8492632318578411777ULL));},
	[]() {kAotMethods[46]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[47])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),chaos_make_string_id_value(8492632318578411777ULL));},
	[]() {kAotMethods[48]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[49])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),chaos_make_string_id_value(8492632318578411777ULL));},
	[]() {kAotMethods[50]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[51])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[52]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[53])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[54]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[55])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[56]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[57])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[58]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[59])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[60]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[61])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[62]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[63])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),chaos_make_string_id_value(8492632318578411777ULL));},
	[]() {kAotMethods[64]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[65])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),chaos_make_string_id_value(8492632318578411777ULL));},
	[]() {kAotMethods[66]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[67])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),chaos_make_string_id_value(8492632318578411777ULL));},
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
	[]() {kAotMethods[79]();},
	[]() {kAotMethods[80]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[81])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),chaos_make_string_id_value(8492632318578411777ULL));},
	[]() {kAotMethods[82]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[83])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),chaos_make_string_id_value(8492632318578411777ULL));},
	[]() {kAotMethods[84]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[85])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),chaos_make_string_id_value(8492632318578411777ULL));},
	[]() {kAotMethods[86]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[87])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),chaos_make_string_id_value(8492632318578411777ULL));},
	[]() {kAotMethods[88]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[89])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[90]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[91])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[92]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[93])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[94]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[95])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[96]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[97])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[98]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[99])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[100]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[101])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),chaos_make_string_id_value(8492632318578411777ULL));},
	[]() {kAotMethods[102]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[103])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),chaos_make_string_id_value(8492632318578411777ULL));},
	[]() {kAotMethods[104]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[105])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),chaos_make_string_id_value(8492632318578411777ULL));},
	[]() {kAotMethods[106]();},
	[]() {kAotMethods[107]();},
	[]() {kAotMethods[108]();},
	[]() {kAotMethods[109]();},
	[]() {kAotMethods[110]();},
	[]() {kAotMethods[111]();},
	[]() {kAotMethods[112]();},
	[]() {kAotMethods[113]();},
	[]() {kAotMethods[114]();},
	[]() {kAotMethods[115]();},
	[]() {kAotMethods[116]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[117])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[118]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[119])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[120]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[121])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[122]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[123])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[124]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[125])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[126]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[127])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[128]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[129])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[130]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[131])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[132]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[133])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[134]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[135])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[136]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[137])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
};

// ── Subject entry index mapping ─────────────────────────────────
// Maps subject index (0-based sequential) to kAotMethod index.
// Used by runtime-entry.cpp to route --benchmark N to the correct
// AOT method slot, since kAotMethods[] includes lambdas/closures
// that shift subject methods to non-contiguous indices.
extern "C" const int kSubjectEntryCount = 90;
extern "C" const int kSubjectEntryIndices[90] = {
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
	10,
	12,
	14,
	16,
	18,
	20,
	22,
	24,
	26,
	28,
	30,
	31,
	32,
	33,
	34,
	35,
	36,
	37,
	38,
	39,
	40,
	41,
	42,
	44,
	46,
	48,
	50,
	52,
	54,
	56,
	58,
	60,
	62,
	64,
	66,
	68,
	69,
	70,
	71,
	72,
	73,
	74,
	75,
	76,
	77,
	78,
	79,
	80,
	82,
	84,
	86,
	88,
	90,
	92,
	94,
	96,
	98,
	100,
	102,
	104,
	106,
	107,
	108,
	109,
	110,
	111,
	112,
	113,
	114,
	115,
	116,
	118,
	120,
	122,
	124,
	126,
	128,
	130,
	132,
	134,
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

	/* ── Entry 0: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry0;
	/* ── Entry 1: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry1;
	/* ── Entry 2: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_2 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry2;
	/* ── Entry 3: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_3 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry3;
	/* ── Entry 4: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_4 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[9];
	} entry4;
	/* ── Entry 5: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_5 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry5;
	/* ── Entry 6: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_6 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry6;
	/* ── Entry 7: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_7 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry7;
	/* ── Entry 8: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_8 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry8;
	/* ── Entry 9: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_9 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[9];
	} entry9;
	/* ── Entry 10: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_10 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry10;
	/* ── Entry 11: NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_10_b__11_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry11;
	/* ── Entry 12: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_11 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry12;
	/* ── Entry 13: NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_11_b__12_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry13;
	/* ── Entry 14: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_12 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry14;
	/* ── Entry 15: NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_12_b__13_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry15;
	/* ── Entry 16: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_13 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry16;
	/* ── Entry 17: NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_13_b__14_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry17;
	/* ── Entry 18: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_14 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry18;
	/* ── Entry 19: NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_14_b__15_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry19;
	/* ── Entry 20: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_15 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry20;
	/* ── Entry 21: NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_15_b__16_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry21;
	/* ── Entry 22: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_16 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry22;
	/* ── Entry 23: NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_16_b__17_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry23;
	/* ── Entry 24: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_17 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry24;
	/* ── Entry 25: NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_17_b__18_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry25;
	/* ── Entry 26: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_18 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry26;
	/* ── Entry 27: NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_18_b__19_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry27;
	/* ── Entry 28: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_19 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry28;
	/* ── Entry 29: NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_19_b__20_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry29;
	/* ── Entry 30: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_20 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry30;
	/* ── Entry 31: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_21 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry31;
	/* ── Entry 32: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_22 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry32;
	/* ── Entry 33: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_23 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry33;
	/* ── Entry 34: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_24 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry34;
	/* ── Entry 35: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_25 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry35;
	/* ── Entry 36: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_26 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry36;
	/* ── Entry 37: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_27 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry37;
	/* ── Entry 38: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_28 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[9];
	} entry38;
	/* ── Entry 39: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_29 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[9];
	} entry39;
	/* ── Entry 40: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_30 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry40;
	/* ── Entry 41: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_31 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry41;
	/* ── Entry 42: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry42;
	/* ── Entry 43: NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_32_b__33_0_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry43;
	/* ── Entry 44: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_33 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry44;
	/* ── Entry 45: NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_33_b__34_0_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry45;
	/* ── Entry 46: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_34 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry46;
	/* ── Entry 47: NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_34_b__35_0_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry47;
	/* ── Entry 48: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_35 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry48;
	/* ── Entry 49: NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_35_b__36_0_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry49;
	/* ── Entry 50: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_36 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry50;
	/* ── Entry 51: NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_36_b__37_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry51;
	/* ── Entry 52: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_37 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry52;
	/* ── Entry 53: NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_37_b__38_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry53;
	/* ── Entry 54: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_38 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry54;
	/* ── Entry 55: NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_38_b__39_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry55;
	/* ── Entry 56: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_39 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry56;
	/* ── Entry 57: NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_39_b__40_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry57;
	/* ── Entry 58: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_40 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry58;
	/* ── Entry 59: NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_40_b__41_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry59;
	/* ── Entry 60: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_41 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry60;
	/* ── Entry 61: NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_41_b__42_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry61;
	/* ── Entry 62: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_42 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry62;
	/* ── Entry 63: NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_42_b__43_0_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry63;
	/* ── Entry 64: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_43 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry64;
	/* ── Entry 65: NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_43_b__44_0_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry65;
	/* ── Entry 66: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_44 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry66;
	/* ── Entry 67: NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_44_b__45_0_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry67;
	/* ── Entry 68: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_45 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry68;
	/* ── Entry 69: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_46 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry69;
	/* ── Entry 70: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_47 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry70;
	/* ── Entry 71: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_48 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry71;
	/* ── Entry 72: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_49 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry72;
	/* ── Entry 73: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_50 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry73;
	/* ── Entry 74: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_51 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry74;
	/* ── Entry 75: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_52 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry75;
	/* ── Entry 76: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_53 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[9];
	} entry76;
	/* ── Entry 77: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_54 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[9];
	} entry77;
	/* ── Entry 78: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_55 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry78;
	/* ── Entry 79: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_56 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry79;
	/* ── Entry 80: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_57 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry80;
	/* ── Entry 81: NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_57_b__58_0_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry81;
	/* ── Entry 82: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_58 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry82;
	/* ── Entry 83: NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_58_b__59_0_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry83;
	/* ── Entry 84: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_59 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry84;
	/* ── Entry 85: NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_59_b__60_0_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry85;
	/* ── Entry 86: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_60 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry86;
	/* ── Entry 87: NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_60_b__61_0_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry87;
	/* ── Entry 88: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_61 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry88;
	/* ── Entry 89: NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_61_b__62_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry89;
	/* ── Entry 90: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_62 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry90;
	/* ── Entry 91: NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_62_b__63_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry91;
	/* ── Entry 92: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_63 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry92;
	/* ── Entry 93: NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_63_b__64_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry93;
	/* ── Entry 94: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_64 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry94;
	/* ── Entry 95: NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_64_b__65_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry95;
	/* ── Entry 96: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_65 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry96;
	/* ── Entry 97: NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_65_b__66_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry97;
	/* ── Entry 98: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_66 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry98;
	/* ── Entry 99: NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_66_b__67_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry99;
	/* ── Entry 100: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_67 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry100;
	/* ── Entry 101: NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_67_b__68_0_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry101;
	/* ── Entry 102: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_68 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry102;
	/* ── Entry 103: NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_68_b__69_0_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry103;
	/* ── Entry 104: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_69 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry104;
	/* ── Entry 105: NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_69_b__70_0_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry105;
	/* ── Entry 106: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_70 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry106;
	/* ── Entry 107: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_71 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry107;
	/* ── Entry 108: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_72 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry108;
	/* ── Entry 109: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_73 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry109;
	/* ── Entry 110: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_74 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[9];
	} entry110;
	/* ── Entry 111: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_75 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry111;
	/* ── Entry 112: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_76 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry112;
	/* ── Entry 113: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_77 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry113;
	/* ── Entry 114: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_78 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry114;
	/* ── Entry 115: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_79 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[9];
	} entry115;
	/* ── Entry 116: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_80 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry116;
	/* ── Entry 117: NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_80_b__81_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry117;
	/* ── Entry 118: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_81 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry118;
	/* ── Entry 119: NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_81_b__82_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry119;
	/* ── Entry 120: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_82 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry120;
	/* ── Entry 121: NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_82_b__83_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry121;
	/* ── Entry 122: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_83 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry122;
	/* ── Entry 123: NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_83_b__84_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry123;
	/* ── Entry 124: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_84 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry124;
	/* ── Entry 125: NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_84_b__85_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry125;
	/* ── Entry 126: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_85 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry126;
	/* ── Entry 127: NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_85_b__86_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry127;
	/* ── Entry 128: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_86 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry128;
	/* ── Entry 129: NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_86_b__87_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry129;
	/* ── Entry 130: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_87 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry130;
	/* ── Entry 131: NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_87_b__88_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry131;
	/* ── Entry 132: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_88 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry132;
	/* ── Entry 133: NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_88_b__89_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry133;
	/* ── Entry 134: NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_89 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry134;
	/* ── Entry 135: NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_89_b__90_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry135;
	/* ── Entry 136: NumericAggregationSubjects_NumericAggregationSubjects___c_cctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry136;
	/* ── Entry 137: NumericAggregationSubjects_NumericAggregationSubjects___c__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry137;
} kChaosGcSlotMapsSection = {
	/* entry0 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_0 */
	.entry0 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_0),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry1 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_1 */
	.entry1 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_1),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry2 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_2 */
	.entry2 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_2),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry3 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_3 */
	.entry3 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_3),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry4 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_4 */
	.entry4 = {
		/* entry_total_size = 56 */ 56u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_4),
		/* frame_size = 72 */ 72u,
		/* num_gc_slots = 9 */ 9u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u }
	},
	/* entry5 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_5 */
	.entry5 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_5),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry6 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_6 */
	.entry6 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_6),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry7 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_7 */
	.entry7 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_7),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry8 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_8 */
	.entry8 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_8),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry9 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_9 */
	.entry9 = {
		/* entry_total_size = 56 */ 56u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_9),
		/* frame_size = 72 */ 72u,
		/* num_gc_slots = 9 */ 9u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u }
	},
	/* entry10 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_10 */
	.entry10 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_10),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry11 = NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_10_b__11_0_System_Int32 */
	.entry11 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_10_b__11_0_System_Int32),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 16u }
	},
	/* entry12 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_11 */
	.entry12 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_11),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry13 = NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_11_b__12_0_System_Int32 */
	.entry13 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_11_b__12_0_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 16u, 24u }
	},
	/* entry14 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_12 */
	.entry14 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_12),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry15 = NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_12_b__13_0_System_Int32 */
	.entry15 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_12_b__13_0_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 16u, 24u }
	},
	/* entry16 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_13 */
	.entry16 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_13),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry17 = NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_13_b__14_0_System_Int32 */
	.entry17 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_13_b__14_0_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 16u, 24u }
	},
	/* entry18 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_14 */
	.entry18 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_14),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry19 = NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_14_b__15_0_System_Int32 */
	.entry19 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_14_b__15_0_System_Int32),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 16u }
	},
	/* entry20 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_15 */
	.entry20 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_15),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry21 = NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_15_b__16_0_System_Int32 */
	.entry21 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_15_b__16_0_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 16u, 24u, 32u }
	},
	/* entry22 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_16 */
	.entry22 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_16),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry23 = NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_16_b__17_0_System_Int32 */
	.entry23 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_16_b__17_0_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 16u, 24u, 32u }
	},
	/* entry24 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_17 */
	.entry24 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_17),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry25 = NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_17_b__18_0_System_Int32 */
	.entry25 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_17_b__18_0_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 16u, 24u, 32u }
	},
	/* entry26 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_18 */
	.entry26 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_18),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry27 = NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_18_b__19_0_System_Int32 */
	.entry27 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_18_b__19_0_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 16u, 24u, 32u }
	},
	/* entry28 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_19 */
	.entry28 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_19),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry29 = NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_19_b__20_0_System_Int32 */
	.entry29 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_19_b__20_0_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 16u, 24u, 32u }
	},
	/* entry30 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_20 */
	.entry30 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_20),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry31 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_21 */
	.entry31 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_21),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry32 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_22 */
	.entry32 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_22),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry33 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_23 */
	.entry33 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_23),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry34 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_24 */
	.entry34 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_24),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry35 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_25 */
	.entry35 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_25),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry36 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_26 */
	.entry36 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_26),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry37 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_27 */
	.entry37 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_27),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry38 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_28 */
	.entry38 = {
		/* entry_total_size = 56 */ 56u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_28),
		/* frame_size = 72 */ 72u,
		/* num_gc_slots = 9 */ 9u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u }
	},
	/* entry39 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_29 */
	.entry39 = {
		/* entry_total_size = 56 */ 56u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_29),
		/* frame_size = 72 */ 72u,
		/* num_gc_slots = 9 */ 9u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u }
	},
	/* entry40 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_30 */
	.entry40 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_30),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry41 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_31 */
	.entry41 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_31),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry42 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_32 */
	.entry42 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry43 = NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_32_b__33_0_System_String */
	.entry43 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_32_b__33_0_System_String),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry44 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_33 */
	.entry44 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_33),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry45 = NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_33_b__34_0_System_String */
	.entry45 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_33_b__34_0_System_String),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry46 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_34 */
	.entry46 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_34),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry47 = NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_34_b__35_0_System_String */
	.entry47 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_34_b__35_0_System_String),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry48 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_35 */
	.entry48 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_35),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry49 = NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_35_b__36_0_System_String */
	.entry49 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_35_b__36_0_System_String),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry50 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_36 */
	.entry50 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_36),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry51 = NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_36_b__37_0_System_Int32 */
	.entry51 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_36_b__37_0_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 16u, 24u }
	},
	/* entry52 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_37 */
	.entry52 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_37),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry53 = NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_37_b__38_0_System_Int32 */
	.entry53 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_37_b__38_0_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 16u, 24u, 32u }
	},
	/* entry54 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_38 */
	.entry54 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_38),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry55 = NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_38_b__39_0_System_Int32 */
	.entry55 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_38_b__39_0_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 16u, 24u }
	},
	/* entry56 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_39 */
	.entry56 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_39),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry57 = NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_39_b__40_0_System_Int32 */
	.entry57 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_39_b__40_0_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 16u, 24u, 32u }
	},
	/* entry58 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_40 */
	.entry58 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_40),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry59 = NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_40_b__41_0_System_Int32 */
	.entry59 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_40_b__41_0_System_Int32),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 16u }
	},
	/* entry60 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_41 */
	.entry60 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_41),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry61 = NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_41_b__42_0_System_Int32 */
	.entry61 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_41_b__42_0_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 16u, 24u, 32u }
	},
	/* entry62 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_42 */
	.entry62 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_42),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry63 = NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_42_b__43_0_System_String */
	.entry63 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_42_b__43_0_System_String),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry64 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_43 */
	.entry64 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_43),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry65 = NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_43_b__44_0_System_String */
	.entry65 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_43_b__44_0_System_String),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry66 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_44 */
	.entry66 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_44),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry67 = NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_44_b__45_0_System_String */
	.entry67 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_44_b__45_0_System_String),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry68 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_45 */
	.entry68 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_45),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry69 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_46 */
	.entry69 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_46),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry70 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_47 */
	.entry70 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_47),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry71 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_48 */
	.entry71 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_48),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry72 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_49 */
	.entry72 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_49),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry73 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_50 */
	.entry73 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_50),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry74 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_51 */
	.entry74 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_51),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry75 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_52 */
	.entry75 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_52),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry76 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_53 */
	.entry76 = {
		/* entry_total_size = 56 */ 56u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_53),
		/* frame_size = 72 */ 72u,
		/* num_gc_slots = 9 */ 9u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u }
	},
	/* entry77 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_54 */
	.entry77 = {
		/* entry_total_size = 56 */ 56u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_54),
		/* frame_size = 72 */ 72u,
		/* num_gc_slots = 9 */ 9u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u }
	},
	/* entry78 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_55 */
	.entry78 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_55),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry79 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_56 */
	.entry79 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_56),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry80 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_57 */
	.entry80 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_57),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry81 = NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_57_b__58_0_System_String */
	.entry81 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_57_b__58_0_System_String),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry82 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_58 */
	.entry82 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_58),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry83 = NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_58_b__59_0_System_String */
	.entry83 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_58_b__59_0_System_String),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry84 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_59 */
	.entry84 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_59),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry85 = NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_59_b__60_0_System_String */
	.entry85 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_59_b__60_0_System_String),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry86 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_60 */
	.entry86 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_60),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry87 = NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_60_b__61_0_System_String */
	.entry87 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_60_b__61_0_System_String),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry88 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_61 */
	.entry88 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_61),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry89 = NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_61_b__62_0_System_Int32 */
	.entry89 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_61_b__62_0_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 16u, 24u }
	},
	/* entry90 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_62 */
	.entry90 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_62),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry91 = NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_62_b__63_0_System_Int32 */
	.entry91 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_62_b__63_0_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 16u, 24u, 32u }
	},
	/* entry92 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_63 */
	.entry92 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_63),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry93 = NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_63_b__64_0_System_Int32 */
	.entry93 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_63_b__64_0_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 16u, 24u }
	},
	/* entry94 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_64 */
	.entry94 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_64),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry95 = NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_64_b__65_0_System_Int32 */
	.entry95 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_64_b__65_0_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 16u, 24u, 32u }
	},
	/* entry96 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_65 */
	.entry96 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_65),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry97 = NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_65_b__66_0_System_Int32 */
	.entry97 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_65_b__66_0_System_Int32),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 16u }
	},
	/* entry98 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_66 */
	.entry98 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_66),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry99 = NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_66_b__67_0_System_Int32 */
	.entry99 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_66_b__67_0_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 16u, 24u, 32u }
	},
	/* entry100 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_67 */
	.entry100 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_67),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry101 = NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_67_b__68_0_System_String */
	.entry101 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_67_b__68_0_System_String),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry102 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_68 */
	.entry102 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_68),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry103 = NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_68_b__69_0_System_String */
	.entry103 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_68_b__69_0_System_String),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry104 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_69 */
	.entry104 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_69),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry105 = NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_69_b__70_0_System_String */
	.entry105 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_69_b__70_0_System_String),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry106 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_70 */
	.entry106 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_70),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry107 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_71 */
	.entry107 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_71),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry108 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_72 */
	.entry108 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_72),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry109 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_73 */
	.entry109 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_73),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry110 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_74 */
	.entry110 = {
		/* entry_total_size = 56 */ 56u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_74),
		/* frame_size = 72 */ 72u,
		/* num_gc_slots = 9 */ 9u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u }
	},
	/* entry111 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_75 */
	.entry111 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_75),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry112 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_76 */
	.entry112 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_76),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry113 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_77 */
	.entry113 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_77),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry114 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_78 */
	.entry114 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_78),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry115 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_79 */
	.entry115 = {
		/* entry_total_size = 56 */ 56u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_79),
		/* frame_size = 72 */ 72u,
		/* num_gc_slots = 9 */ 9u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u }
	},
	/* entry116 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_80 */
	.entry116 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_80),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry117 = NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_80_b__81_0_System_Int32 */
	.entry117 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_80_b__81_0_System_Int32),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 16u }
	},
	/* entry118 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_81 */
	.entry118 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_81),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry119 = NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_81_b__82_0_System_Int32 */
	.entry119 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_81_b__82_0_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 16u, 24u }
	},
	/* entry120 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_82 */
	.entry120 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_82),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry121 = NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_82_b__83_0_System_Int32 */
	.entry121 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_82_b__83_0_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 16u, 24u }
	},
	/* entry122 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_83 */
	.entry122 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_83),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry123 = NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_83_b__84_0_System_Int32 */
	.entry123 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_83_b__84_0_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 16u, 24u }
	},
	/* entry124 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_84 */
	.entry124 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_84),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry125 = NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_84_b__85_0_System_Int32 */
	.entry125 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_84_b__85_0_System_Int32),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 16u }
	},
	/* entry126 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_85 */
	.entry126 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_85),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry127 = NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_85_b__86_0_System_Int32 */
	.entry127 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_85_b__86_0_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 16u, 24u, 32u }
	},
	/* entry128 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_86 */
	.entry128 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_86),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry129 = NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_86_b__87_0_System_Int32 */
	.entry129 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_86_b__87_0_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 16u, 24u, 32u }
	},
	/* entry130 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_87 */
	.entry130 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_87),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry131 = NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_87_b__88_0_System_Int32 */
	.entry131 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_87_b__88_0_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 16u, 24u, 32u }
	},
	/* entry132 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_88 */
	.entry132 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_88),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry133 = NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_88_b__89_0_System_Int32 */
	.entry133 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_88_b__89_0_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 16u, 24u, 32u }
	},
	/* entry134 = NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_89 */
	.entry134 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_89),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry135 = NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_89_b__90_0_System_Int32 */
	.entry135 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_89_b__90_0_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 16u, 24u, 32u }
	},
	/* entry136 = NumericAggregationSubjects_NumericAggregationSubjects___c_cctor */
	.entry136 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_cctor),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry137 = NumericAggregationSubjects_NumericAggregationSubjects___c__ctor */
	.entry137 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 5172u;

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[138] = {
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_0),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_1),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_2),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_3),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_4),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_5),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_6),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_7),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_8),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_9),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_10),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_10_b__11_0_System_Int32),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_11),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_11_b__12_0_System_Int32),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_12),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_12_b__13_0_System_Int32),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_13),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_13_b__14_0_System_Int32),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_14),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_14_b__15_0_System_Int32),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_15),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_15_b__16_0_System_Int32),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_16),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_16_b__17_0_System_Int32),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_17),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_17_b__18_0_System_Int32),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_18),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_18_b__19_0_System_Int32),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_19),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_19_b__20_0_System_Int32),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_20),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_21),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_22),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_23),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_24),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_25),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_26),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_27),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_28),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_29),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_30),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_31),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_32),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_32_b__33_0_System_String),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_33),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_33_b__34_0_System_String),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_34),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_34_b__35_0_System_String),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_35),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_35_b__36_0_System_String),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_36),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_36_b__37_0_System_Int32),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_37),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_37_b__38_0_System_Int32),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_38),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_38_b__39_0_System_Int32),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_39),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_39_b__40_0_System_Int32),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_40),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_40_b__41_0_System_Int32),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_41),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_41_b__42_0_System_Int32),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_42),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_42_b__43_0_System_String),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_43),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_43_b__44_0_System_String),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_44),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_44_b__45_0_System_String),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_45),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_46),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_47),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_48),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_49),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_50),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_51),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_52),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_53),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_54),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_55),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_56),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_57),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_57_b__58_0_System_String),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_58),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_58_b__59_0_System_String),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_59),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_59_b__60_0_System_String),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_60),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_60_b__61_0_System_String),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_61),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_61_b__62_0_System_Int32),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_62),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_62_b__63_0_System_Int32),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_63),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_63_b__64_0_System_Int32),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_64),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_64_b__65_0_System_Int32),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_65),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_65_b__66_0_System_Int32),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_66),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_66_b__67_0_System_Int32),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_67),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_67_b__68_0_System_String),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_68),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_68_b__69_0_System_String),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_69),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_69_b__70_0_System_String),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_70),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_71),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_72),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_73),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_74),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_75),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_76),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_77),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_78),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_79),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_80),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_80_b__81_0_System_Int32),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_81),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_81_b__82_0_System_Int32),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_82),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_82_b__83_0_System_Int32),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_83),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_83_b__84_0_System_Int32),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_84),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_84_b__85_0_System_Int32),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_85),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_85_b__86_0_System_Int32),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_86),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_86_b__87_0_System_Int32),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_87),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_87_b__88_0_System_Int32),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_88),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_88_b__89_0_System_Int32),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_89),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_89_b__90_0_System_Int32),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c_cctor),
	reinterpret_cast<void*>(&NumericAggregationSubjects_NumericAggregationSubjects___c__ctor),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
// ── VTable descriptors (for BootstrapRuntime TypeVTable registration) ──
static const VTableDescriptorV0 kChaosVTableDescriptors[] = {
	{
		CHAOS_IL2CPP_UINT64_C(0xA9D695C941B6FBAE),
		0x00000002u,
		0u,
		47u,
		reinterpret_cast<const void*>(kSlots_NumericAggregationSubjects_NumericAggregationSubjects___c),
		reinterpret_cast<const void**>(chaos_vtable_NumericAggregationSubjects_NumericAggregationSubjects___c),
		47u,
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
	.method_pointer_count      = 138u,
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
	.image_name_utf8    = "NumericAggregationSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_NumericAggregationSubjects_NumericAggregationSubjects[1] = {
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::_exitCode", "_exitCode", "System.Int32", 0LL },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_NumericAggregationSubjects_NumericAggregationSubjects[90] = {
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_0:System.Void()", "CustomEntrySubject_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_1:System.Void()", "CustomEntrySubject_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_2:System.Void()", "CustomEntrySubject_2", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_3:System.Void()", "CustomEntrySubject_3", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_4:System.Void()", "CustomEntrySubject_4", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_5:System.Void()", "CustomEntrySubject_5", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_6:System.Void()", "CustomEntrySubject_6", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_7:System.Void()", "CustomEntrySubject_7", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_8:System.Void()", "CustomEntrySubject_8", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_9:System.Void()", "CustomEntrySubject_9", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_10:System.Void()", "CustomEntrySubject_10", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_11:System.Void()", "CustomEntrySubject_11", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_12:System.Void()", "CustomEntrySubject_12", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_13:System.Void()", "CustomEntrySubject_13", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_14:System.Void()", "CustomEntrySubject_14", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_15:System.Void()", "CustomEntrySubject_15", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_16:System.Void()", "CustomEntrySubject_16", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_17:System.Void()", "CustomEntrySubject_17", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_18:System.Void()", "CustomEntrySubject_18", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_19:System.Void()", "CustomEntrySubject_19", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_20:System.Void()", "CustomEntrySubject_20", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_21:System.Void()", "CustomEntrySubject_21", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_22:System.Void()", "CustomEntrySubject_22", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_23:System.Void()", "CustomEntrySubject_23", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_24:System.Void()", "CustomEntrySubject_24", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_25:System.Void()", "CustomEntrySubject_25", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_26:System.Void()", "CustomEntrySubject_26", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_27:System.Void()", "CustomEntrySubject_27", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_28:System.Void()", "CustomEntrySubject_28", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_29:System.Void()", "CustomEntrySubject_29", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_30:System.Void()", "CustomEntrySubject_30", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_31:System.Void()", "CustomEntrySubject_31", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_32:System.Void()", "CustomEntrySubject_32", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_33:System.Void()", "CustomEntrySubject_33", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_34:System.Void()", "CustomEntrySubject_34", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_35:System.Void()", "CustomEntrySubject_35", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_36:System.Void()", "CustomEntrySubject_36", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_37:System.Void()", "CustomEntrySubject_37", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_38:System.Void()", "CustomEntrySubject_38", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_39:System.Void()", "CustomEntrySubject_39", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_40:System.Void()", "CustomEntrySubject_40", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_41:System.Void()", "CustomEntrySubject_41", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_42:System.Void()", "CustomEntrySubject_42", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_43:System.Void()", "CustomEntrySubject_43", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_44:System.Void()", "CustomEntrySubject_44", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_45:System.Void()", "CustomEntrySubject_45", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_46:System.Void()", "CustomEntrySubject_46", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_47:System.Void()", "CustomEntrySubject_47", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_48:System.Void()", "CustomEntrySubject_48", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_49:System.Void()", "CustomEntrySubject_49", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_50:System.Void()", "CustomEntrySubject_50", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_51:System.Void()", "CustomEntrySubject_51", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_52:System.Void()", "CustomEntrySubject_52", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_53:System.Void()", "CustomEntrySubject_53", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_54:System.Void()", "CustomEntrySubject_54", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_55:System.Void()", "CustomEntrySubject_55", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_56:System.Void()", "CustomEntrySubject_56", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_57:System.Void()", "CustomEntrySubject_57", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_58:System.Void()", "CustomEntrySubject_58", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_59:System.Void()", "CustomEntrySubject_59", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_60:System.Void()", "CustomEntrySubject_60", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_61:System.Void()", "CustomEntrySubject_61", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_62:System.Void()", "CustomEntrySubject_62", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_63:System.Void()", "CustomEntrySubject_63", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_64:System.Void()", "CustomEntrySubject_64", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_65:System.Void()", "CustomEntrySubject_65", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_66:System.Void()", "CustomEntrySubject_66", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_67:System.Void()", "CustomEntrySubject_67", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_68:System.Void()", "CustomEntrySubject_68", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_69:System.Void()", "CustomEntrySubject_69", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_70:System.Void()", "CustomEntrySubject_70", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_71:System.Void()", "CustomEntrySubject_71", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_72:System.Void()", "CustomEntrySubject_72", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_73:System.Void()", "CustomEntrySubject_73", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_74:System.Void()", "CustomEntrySubject_74", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_75:System.Void()", "CustomEntrySubject_75", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_76:System.Void()", "CustomEntrySubject_76", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_77:System.Void()", "CustomEntrySubject_77", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_78:System.Void()", "CustomEntrySubject_78", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_79:System.Void()", "CustomEntrySubject_79", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_80:System.Void()", "CustomEntrySubject_80", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_81:System.Void()", "CustomEntrySubject_81", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_82:System.Void()", "CustomEntrySubject_82", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_83:System.Void()", "CustomEntrySubject_83", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_84:System.Void()", "CustomEntrySubject_84", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_85:System.Void()", "CustomEntrySubject_85", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_86:System.Void()", "CustomEntrySubject_86", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_87:System.Void()", "CustomEntrySubject_87", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_88:System.Void()", "CustomEntrySubject_88", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_89:System.Void()", "CustomEntrySubject_89", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_NumericAggregationSubjects_NumericAggregationSubjects___c[48] = {
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_10>b__11_0:System.Int32(System.Int32)", "<CustomEntrySubject_10>b__11_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_11>b__12_0:System.Int64(System.Int32)", "<CustomEntrySubject_11>b__12_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_12>b__13_0:System.Single(System.Int32)", "<CustomEntrySubject_12>b__13_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_13>b__14_0:System.Double(System.Int32)", "<CustomEntrySubject_13>b__14_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_14>b__15_0:System.Decimal(System.Int32)", "<CustomEntrySubject_14>b__15_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_15>b__16_0:System.Nullable<System.Int32>(System.Int32)", "<CustomEntrySubject_15>b__16_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_16>b__17_0:System.Nullable<System.Int64>(System.Int32)", "<CustomEntrySubject_16>b__17_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_17>b__18_0:System.Nullable<System.Single>(System.Int32)", "<CustomEntrySubject_17>b__18_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_18>b__19_0:System.Nullable<System.Double>(System.Int32)", "<CustomEntrySubject_18>b__19_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_19>b__20_0:System.Nullable<System.Decimal>(System.Int32)", "<CustomEntrySubject_19>b__20_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_32>b__33_0:System.Int32(System.String)", "<CustomEntrySubject_32>b__33_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_33>b__34_0:System.Nullable<System.Int32>(System.String)", "<CustomEntrySubject_33>b__34_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_34>b__35_0:System.Int64(System.String)", "<CustomEntrySubject_34>b__35_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_35>b__36_0:System.Nullable<System.Int64>(System.String)", "<CustomEntrySubject_35>b__36_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_36>b__37_0:System.Single(System.Int32)", "<CustomEntrySubject_36>b__37_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_37>b__38_0:System.Nullable<System.Single>(System.Int32)", "<CustomEntrySubject_37>b__38_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_38>b__39_0:System.Double(System.Int32)", "<CustomEntrySubject_38>b__39_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_39>b__40_0:System.Nullable<System.Double>(System.Int32)", "<CustomEntrySubject_39>b__40_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_40>b__41_0:System.Decimal(System.Int32)", "<CustomEntrySubject_40>b__41_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_41>b__42_0:System.Nullable<System.Decimal>(System.Int32)", "<CustomEntrySubject_41>b__42_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_42>b__43_0:System.String(System.String)", "<CustomEntrySubject_42>b__43_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_43>b__44_0:System.Int32(System.String)", "<CustomEntrySubject_43>b__44_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_44>b__45_0:System.String(System.String)", "<CustomEntrySubject_44>b__45_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_57>b__58_0:System.Int32(System.String)", "<CustomEntrySubject_57>b__58_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_58>b__59_0:System.Nullable<System.Int32>(System.String)", "<CustomEntrySubject_58>b__59_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_59>b__60_0:System.Int64(System.String)", "<CustomEntrySubject_59>b__60_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_60>b__61_0:System.Nullable<System.Int64>(System.String)", "<CustomEntrySubject_60>b__61_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_61>b__62_0:System.Single(System.Int32)", "<CustomEntrySubject_61>b__62_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_62>b__63_0:System.Nullable<System.Single>(System.Int32)", "<CustomEntrySubject_62>b__63_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_63>b__64_0:System.Double(System.Int32)", "<CustomEntrySubject_63>b__64_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_64>b__65_0:System.Nullable<System.Double>(System.Int32)", "<CustomEntrySubject_64>b__65_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_65>b__66_0:System.Decimal(System.Int32)", "<CustomEntrySubject_65>b__66_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_66>b__67_0:System.Nullable<System.Decimal>(System.Int32)", "<CustomEntrySubject_66>b__67_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_67>b__68_0:System.String(System.String)", "<CustomEntrySubject_67>b__68_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_68>b__69_0:System.Int32(System.String)", "<CustomEntrySubject_68>b__69_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_69>b__70_0:System.String(System.String)", "<CustomEntrySubject_69>b__70_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_80>b__81_0:System.Int32(System.Int32)", "<CustomEntrySubject_80>b__81_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_81>b__82_0:System.Int64(System.Int32)", "<CustomEntrySubject_81>b__82_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_82>b__83_0:System.Single(System.Int32)", "<CustomEntrySubject_82>b__83_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_83>b__84_0:System.Double(System.Int32)", "<CustomEntrySubject_83>b__84_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_84>b__85_0:System.Decimal(System.Int32)", "<CustomEntrySubject_84>b__85_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_85>b__86_0:System.Nullable<System.Int32>(System.Int32)", "<CustomEntrySubject_85>b__86_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_86>b__87_0:System.Nullable<System.Int64>(System.Int32)", "<CustomEntrySubject_86>b__87_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_87>b__88_0:System.Nullable<System.Single>(System.Int32)", "<CustomEntrySubject_87>b__88_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_88>b__89_0:System.Nullable<System.Double>(System.Int32)", "<CustomEntrySubject_88>b__89_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_89>b__90_0:System.Nullable<System.Decimal>(System.Int32)", "<CustomEntrySubject_89>b__90_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::.cctor:System.Void()", ".cctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[2] = {
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects", "NumericAggregationSubjects/NumericAggregationSubjects", "", "NumericAggregationSubjects", "NumericAggregationSubjects", nullptr, kReflFields_NumericAggregationSubjects_NumericAggregationSubjects, 1u, nullptr, 0u,
	kReflMethods_NumericAggregationSubjects_NumericAggregationSubjects, 90u },
	{ 0u, "NumericAggregationSubjects/NumericAggregationSubjects+<>c", "NumericAggregationSubjects/NumericAggregationSubjects+<>c", "", "NumericAggregationSubjects+<>c", "NumericAggregationSubjects+<>c", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_NumericAggregationSubjects_NumericAggregationSubjects___c, 48u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[2] = {
	&kReflTypes[0],
	&kReflTypes[1],
};

static const ReflectionQueryImageDescriptor kReflImage = { "NumericAggregationSubjects", kReflTypePtrs, 2u };

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
// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_0()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_0(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(83620299u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<double(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[3])(chaos_arg_0);
		_s0 = ChaosStoreFloat64(chaos_result);
	}
	return;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_1()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_1(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Int64.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(67442793u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<double(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[6])(chaos_arg_0);
		_s0 = ChaosStoreFloat64(chaos_result);
	}
	return;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_2()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_2(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Single.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(77934256u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<float(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[9])(chaos_arg_0);
		_s0 = ChaosStoreFloat32(chaos_result);
	}
	return;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_3()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_3(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Double.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(68641022u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<double(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[12])(chaos_arg_0);
		_s0 = ChaosStoreFloat64(chaos_result);
	}
	return;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_4()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_4(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Decimal.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(15);
	_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Decimal, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Decimal.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(25);
	_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Decimal, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Decimal.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(35);
	_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Decimal, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Decimal.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[15])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_5()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_5(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Int32_.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Int32_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Int32_.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Int32_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Int32_.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Int32_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Int32_.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[18])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_6()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_6(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Int64_.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(10);
	_s3 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s3));
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Int64_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Int64_.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(30);
	_s3 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s3));
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Int64_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Int64_.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[21])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_7()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_7(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Single_.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = ChaosStoreFloat32(1.5f);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Single_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Single_.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	_s3 = ChaosStoreFloat32(3.5f);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Single_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Single_.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[24])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_8()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_8(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Double_.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = ChaosStoreFloat64(1.5);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Double_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Double_.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	_s3 = ChaosStoreFloat64(3.5);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Double_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Double_.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[27])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_9()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_9(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Decimal_.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(15);
	_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Decimal, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Decimal.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Decimal_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Decimal_.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(35);
	_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Decimal, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Decimal.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Decimal_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Decimal_.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[30])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_10()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_10(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(75365335u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
	_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__11_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
			_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 11)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INT32 {
					auto& _d_entry = s_hotpatch_entries[11];
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
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
				chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__11_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<double(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[35])(chaos_arg_0, chaos_arg_1);
			_s4 = ChaosStoreFloat64(chaos_result);
		}
		return;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_10>b__11_0(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_10_b__11_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_11()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_11(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(75365335u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
	_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__12_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
			_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 13)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INT64 {
					auto& _d_entry = s_hotpatch_entries[13];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[12];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
						_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_fn_arg_1));
						CHAOS_IL2CPP_INT64 _d_ret{};
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, &_d_ret);
						return _d_ret;
					}
					return reinterpret_cast<CHAOS_IL2CPP_INT64(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1);
				};
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s2;
				const auto chaos_target = _s1;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Int64_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Int64_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
				chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__12_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<double(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[38])(chaos_arg_0, chaos_arg_1);
			_s4 = ChaosStoreFloat64(chaos_result);
		}
		return;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_11>b__12_0(System.Int32)
extern "C" CHAOS_IL2CPP_INT64 NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_11_b__12_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	_s0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	return ChaosLoadInt64(_s0);
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_12()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_12(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(75365335u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
	_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__13_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
			_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 15)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> float {
					auto& _d_entry = s_hotpatch_entries[15];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[12];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
						_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_fn_arg_1));
						float _d_ret{};
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, &_d_ret);
						return _d_ret;
					}
					return reinterpret_cast<float(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1);
				};
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s2;
				const auto chaos_target = _s1;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Single_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Single_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
				chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__13_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<float(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[41])(chaos_arg_0, chaos_arg_1);
			_s4 = ChaosStoreFloat32(chaos_result);
		}
		return;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_12>b__13_0(System.Int32)
extern "C" float NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_12_b__13_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	_s0 = ChaosStoreFloat32(static_cast<float>(_s0));
	return chaos_load_float32(_s0);
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_13()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_13(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(75365335u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
	_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__14_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
			_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 17)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> double {
					auto& _d_entry = s_hotpatch_entries[17];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[12];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
						_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_fn_arg_1));
						double _d_ret{};
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, &_d_ret);
						return _d_ret;
					}
					return reinterpret_cast<double(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1);
				};
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s2;
				const auto chaos_target = _s1;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Double_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Double_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
				chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__14_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<double(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[44])(chaos_arg_0, chaos_arg_1);
			_s4 = ChaosStoreFloat64(chaos_result);
		}
		return;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_13>b__14_0(System.Int32)
extern "C" double NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_13_b__14_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	_s0 = ChaosStoreFloat64(static_cast<double>(_s0));
	return ChaosLoadFloat64(_s0);
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_14()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_14(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(75365335u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
	_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__15_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
			_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 19)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
					auto& _d_entry = s_hotpatch_entries[19];
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
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Decimal_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Decimal_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
				chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__15_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[47])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_14>b__15_0(System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_14_b__15_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[48])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return _s0;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_15()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_15(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(75365335u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
	_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__16_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
			_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9;
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
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Int32__, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Int32__.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
				chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__16_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[51])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_15>b__16_0(System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_15_b__16_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
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
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosRem(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	{
		if (_s0 == 0)
		{
			_s0 = chaos_args[1];
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Int32_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Int32_.hot;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
		}
		else
		{
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
			{
				const auto chaos_address = _s2;
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
			_s2 = chaos_locals[0];
		}
		return _s1;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_16()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_16(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(75365335u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
	_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__17_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
			_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9;
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
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Int64__, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Int64__.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
				chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__17_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[54])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_16>b__17_0(System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_16_b__17_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
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
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosRem(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	{
		if (_s0 == 0)
		{
			_s0 = chaos_args[1];
			_s0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Int64_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Int64_.hot;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
		}
		else
		{
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
			{
				const auto chaos_address = _s2;
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
			_s2 = chaos_locals[0];
		}
		return _s1;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_17()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_17(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(75365335u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
	_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__18_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
			_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 25)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
					auto& _d_entry = s_hotpatch_entries[25];
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
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Single__, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Single__.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
				chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__18_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[57])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_17>b__18_0(System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_17_b__18_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
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
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosRem(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	{
		if (_s0 == 0)
		{
			_s0 = chaos_args[1];
			_s0 = ChaosStoreFloat32(static_cast<float>(_s0));
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Single_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Single_.hot;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
		}
		else
		{
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
			{
				const auto chaos_address = _s2;
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
			_s2 = chaos_locals[0];
		}
		return _s1;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_18()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_18(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(75365335u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
	_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__19_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
			_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 27)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
					auto& _d_entry = s_hotpatch_entries[27];
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
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Double__, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Double__.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
				chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__19_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[60])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_18>b__19_0(System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_18_b__19_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
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
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosRem(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	{
		if (_s0 == 0)
		{
			_s0 = chaos_args[1];
			_s0 = ChaosStoreFloat64(static_cast<double>(_s0));
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Double_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Double_.hot;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
		}
		else
		{
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
			{
				const auto chaos_address = _s2;
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
			_s2 = chaos_locals[0];
		}
		return _s1;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_19()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_19(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(75365335u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
	_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__20_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
			_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 29)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
					auto& _d_entry = s_hotpatch_entries[29];
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
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Decimal__, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Decimal__.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
				chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__20_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[63])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_19>b__20_0(System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_19_b__20_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
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
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosRem(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	{
		if (_s0 == 0)
		{
			_s0 = chaos_args[1];
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[48])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Decimal_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Decimal_.hot;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
		}
		else
		{
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
			{
				const auto chaos_address = _s2;
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
			_s2 = chaos_locals[0];
		}
		return _s1;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_20()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_20(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};


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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(82134825u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[65])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_21()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_21(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Int64.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(74978047u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT64(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[67])(chaos_arg_0);
		_s0 = ChaosStoreInt64(chaos_result);
	}
	return;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_22()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_22(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Int32_.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Int32_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Int32_.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(9);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Int32_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Int32_.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Int32_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Int32_.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[68])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_23()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_23(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Int64_.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(10);
	_s3 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s3));
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Int64_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Int64_.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(30);
	_s3 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s3));
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Int64_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Int64_.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[69])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_24()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_24(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Double.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(76745527u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<double(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[71])(chaos_arg_0);
		_s0 = ChaosStoreFloat64(chaos_result);
	}
	return;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_25()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_25(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Double_.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = ChaosStoreFloat64(1.5);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Double_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Double_.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	_s3 = ChaosStoreFloat64(3.5);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Double_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Double_.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[72])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_26()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_26(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Single.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(75398811u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<float(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[74])(chaos_arg_0);
		_s0 = ChaosStoreFloat32(chaos_result);
	}
	return;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_27()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_27(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Single_.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = ChaosStoreFloat32(1.5f);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Single_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Single_.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	_s3 = ChaosStoreFloat32(3.5f);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Single_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Single_.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[75])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_28()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_28(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Decimal.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(15);
	_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Decimal, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Decimal.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(35);
	_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Decimal, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Decimal.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(25);
	_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Decimal, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Decimal.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[76])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_29()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_29(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Decimal_.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(15);
	_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Decimal, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Decimal.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Decimal_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Decimal_.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(35);
	_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Decimal, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Decimal.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Decimal_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Decimal_.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[77])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_30()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_30(void)
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
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[79])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_31()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_31(void)
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
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[80])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[81])(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_32()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_32(void)
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
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
	_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__33_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
			_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 43)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1) -> CHAOS_IL2CPP_INT32 {
					auto& _d_entry = s_hotpatch_entries[43];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[16];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_1));
						CHAOS_IL2CPP_INT32 _d_ret{};
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, &_d_ret);
						return _d_ret;
					}
					return reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1);
				};
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s2;
				const auto chaos_target = _s1;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_String_System_Int32_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_String_System_Int32_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
				chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__33_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[84])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_32>b__33_0(System.String)
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_32_b__33_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	{
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_33()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_33(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_String.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{{
		_s3 = CHAOS_IL2CPP_STRING_ID("a");
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
		_s3 = CHAOS_IL2CPP_STRING_ID("bb");
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
		_s3 = CHAOS_IL2CPP_STRING_ID("ccc");
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
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
	_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__34_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
			_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 45)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
					auto& _d_entry = s_hotpatch_entries[45];
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
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_String_System_Nullable_System_Int32__, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_String_System_Nullable_System_Int32__.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
				chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__34_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[88])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_33>b__34_0(System.String)
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_33_b__34_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
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
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	{
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INT32>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>(_s0);
		if (chaos_left > chaos_right)
		{
			_s0 = chaos_args[1];
			{
				const auto chaos_arg_0 = _s0;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Int32_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Int32_.hot;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
		}
		else
		{
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
		}
		return _s0;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_34()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_34(void)
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
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
	_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__35_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
			_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 47)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1) -> CHAOS_IL2CPP_INT64 {
					auto& _d_entry = s_hotpatch_entries[47];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[16];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_1));
						CHAOS_IL2CPP_INT64 _d_ret{};
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, &_d_ret);
						return _d_ret;
					}
					return reinterpret_cast<CHAOS_IL2CPP_INT64(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1);
				};
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s2;
				const auto chaos_target = _s1;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_String_System_Int64_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_String_System_Int64_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
				chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__35_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT64(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[91])(chaos_arg_0, chaos_arg_1);
			_s4 = ChaosStoreInt64(chaos_result);
		}
		return;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_34>b__35_0(System.String)
extern "C" CHAOS_IL2CPP_INT64 NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_34_b__35_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	{
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	return ChaosLoadInt64(_s0);
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_35()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_35(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_String.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{{
		_s3 = CHAOS_IL2CPP_STRING_ID("a");
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
		_s3 = CHAOS_IL2CPP_STRING_ID("bb");
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
		_s3 = CHAOS_IL2CPP_STRING_ID("ccc");
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
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
	_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__36_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
			_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 49)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
					auto& _d_entry = s_hotpatch_entries[49];
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
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_String_System_Nullable_System_Int64__, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_String_System_Nullable_System_Int64__.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
				chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__36_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[94])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_35>b__36_0(System.String)
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_35_b__36_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
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

	_s0 = chaos_args[1];
	{
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INT32>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>(_s0);
		if (chaos_left > chaos_right)
		{
			_s0 = chaos_args[1];
			{
				const auto chaos_arg_0 = _s0;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Int64_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Int64_.hot;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
		}
		else
		{
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
		}
		return _s0;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_36()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_36(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(75365335u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
	_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__37_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
			_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 51)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> float {
					auto& _d_entry = s_hotpatch_entries[51];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[12];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
						_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_fn_arg_1));
						float _d_ret{};
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, &_d_ret);
						return _d_ret;
					}
					return reinterpret_cast<float(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1);
				};
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s2;
				const auto chaos_target = _s1;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Single_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Single_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
				chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__37_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<float(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[96])(chaos_arg_0, chaos_arg_1);
			_s4 = ChaosStoreFloat32(chaos_result);
		}
		return;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_36>b__37_0(System.Int32)
extern "C" float NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_36_b__37_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	_s0 = ChaosStoreFloat32(static_cast<float>(_s0));
	return chaos_load_float32(_s0);
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_37()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_37(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(75365335u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
	_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__38_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
			_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 53)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
					auto& _d_entry = s_hotpatch_entries[53];
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
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Single__, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Single__.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
				chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__38_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[98])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_37>b__38_0(System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_37_b__38_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
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
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosRem(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	{
		if (_s0 == 0)
		{
			_s0 = chaos_args[1];
			_s0 = ChaosStoreFloat32(static_cast<float>(_s0));
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Single_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Single_.hot;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
		}
		else
		{
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
			{
				const auto chaos_address = _s2;
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
			_s2 = chaos_locals[0];
		}
		return _s1;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_38()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_38(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(75365335u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
	_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__39_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
			_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 55)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> double {
					auto& _d_entry = s_hotpatch_entries[55];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[12];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
						_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_fn_arg_1));
						double _d_ret{};
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, &_d_ret);
						return _d_ret;
					}
					return reinterpret_cast<double(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1);
				};
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s2;
				const auto chaos_target = _s1;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Double_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Double_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
				chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__39_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<double(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[100])(chaos_arg_0, chaos_arg_1);
			_s4 = ChaosStoreFloat64(chaos_result);
		}
		return;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_38>b__39_0(System.Int32)
extern "C" double NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_38_b__39_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	_s0 = ChaosStoreFloat64(static_cast<double>(_s0));
	return ChaosLoadFloat64(_s0);
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_39()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_39(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(75365335u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
	_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__40_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
			_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 57)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
					auto& _d_entry = s_hotpatch_entries[57];
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
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Double__, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Double__.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
				chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__40_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[102])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_39>b__40_0(System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_39_b__40_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
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
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosRem(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	{
		if (_s0 == 0)
		{
			_s0 = chaos_args[1];
			_s0 = ChaosStoreFloat64(static_cast<double>(_s0));
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Double_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Double_.hot;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
		}
		else
		{
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
			{
				const auto chaos_address = _s2;
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
			_s2 = chaos_locals[0];
		}
		return _s1;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_40()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_40(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(75365335u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
	_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__41_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
			_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 59)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
					auto& _d_entry = s_hotpatch_entries[59];
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
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Decimal_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Decimal_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
				chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__41_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[104])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_40>b__41_0(System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_40_b__41_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[48])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return _s0;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_41()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_41(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(75365335u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
	_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__42_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
			_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 61)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
					auto& _d_entry = s_hotpatch_entries[61];
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
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Decimal__, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Decimal__.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
				chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__42_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[106])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_41>b__42_0(System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_41_b__42_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
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
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosRem(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	{
		if (_s0 == 0)
		{
			_s0 = chaos_args[1];
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[48])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Decimal_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Decimal_.hot;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
		}
		else
		{
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
			{
				const auto chaos_address = _s2;
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
			_s2 = chaos_locals[0];
		}
		return _s1;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_42()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_42(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_String.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{{
		_s3 = CHAOS_IL2CPP_STRING_ID("x");
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
		_s3 = CHAOS_IL2CPP_STRING_ID("yy");
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
		_s3 = CHAOS_IL2CPP_STRING_ID("zzz");
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
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
	_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__43_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
			_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 63)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
					auto& _d_entry = s_hotpatch_entries[63];
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
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_String_System_String_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_String_System_String_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
				chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__43_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[109])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_42>b__43_0(System.String)
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_42_b__43_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	return _s0;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_43()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_43(void)
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
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
	_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__44_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
			_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 65)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1) -> CHAOS_IL2CPP_INT32 {
					auto& _d_entry = s_hotpatch_entries[65];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[16];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_1));
						CHAOS_IL2CPP_INT32 _d_ret{};
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, &_d_ret);
						return _d_ret;
					}
					return reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1);
				};
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s2;
				const auto chaos_target = _s1;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_String_System_Int32_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_String_System_Int32_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
				chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__44_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[111])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_43>b__44_0(System.String)
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_43_b__44_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	{
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_44()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_44(void)
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
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
	_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__45_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
			_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 67)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
					auto& _d_entry = s_hotpatch_entries[67];
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
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_String_System_String_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_String_System_String_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
				chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__45_0 = chaos_value;
			}
		}
		{
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[80])();
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			const auto chaos_arg_2 = _s6;
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[113])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_44>b__45_0(System.String)
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_44_b__45_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	return _s0;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_45()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_45(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};


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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(82134825u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[114])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_46()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_46(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Int64.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(79353099u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT64(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[116])(chaos_arg_0);
		_s0 = ChaosStoreInt64(chaos_result);
	}
	return;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_47()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_47(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Int32_.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Int32_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Int32_.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Int32_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Int32_.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(9);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Int32_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Int32_.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[117])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_48()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_48(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Int64_.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(30);
	_s3 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s3));
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Int64_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Int64_.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(10);
	_s3 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s3));
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Int64_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Int64_.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[118])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_49()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_49(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Single.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(68103623u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<float(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[120])(chaos_arg_0);
		_s0 = ChaosStoreFloat32(chaos_result);
	}
	return;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_50()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_50(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Single_.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = ChaosStoreFloat32(3.5f);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Single_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Single_.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	_s3 = ChaosStoreFloat32(1.5f);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Single_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Single_.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[121])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_51()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_51(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Double.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(72489192u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<double(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[123])(chaos_arg_0);
		_s0 = ChaosStoreFloat64(chaos_result);
	}
	return;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_52()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_52(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Double_.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = ChaosStoreFloat64(3.5);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Double_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Double_.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	_s3 = ChaosStoreFloat64(1.5);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Double_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Double_.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[124])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_53()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_53(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Decimal.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(35);
	_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Decimal, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Decimal.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(15);
	_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Decimal, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Decimal.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(25);
	_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Decimal, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Decimal.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[125])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_54()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_54(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Decimal_.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(35);
	_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Decimal, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Decimal.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Decimal_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Decimal_.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(15);
	_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Decimal, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Decimal.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Decimal_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Decimal_.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[126])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_55()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_55(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_String.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
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
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
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
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[127])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_56()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_56(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_String.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
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
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
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
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[80])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[128])(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_57()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_57(void)
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
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
	_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__58_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
			_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 81)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1) -> CHAOS_IL2CPP_INT32 {
					auto& _d_entry = s_hotpatch_entries[81];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[16];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_1));
						CHAOS_IL2CPP_INT32 _d_ret{};
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, &_d_ret);
						return _d_ret;
					}
					return reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1);
				};
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s2;
				const auto chaos_target = _s1;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_String_System_Int32_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_String_System_Int32_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
				chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__58_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[130])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_57>b__58_0(System.String)
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_57_b__58_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	{
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_58()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_58(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_String.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{{
		_s3 = CHAOS_IL2CPP_STRING_ID("a");
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
		_s3 = CHAOS_IL2CPP_STRING_ID("bb");
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
		_s3 = CHAOS_IL2CPP_STRING_ID("ccc");
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
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
	_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__59_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
			_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 83)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
					auto& _d_entry = s_hotpatch_entries[83];
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
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_String_System_Nullable_System_Int32__, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_String_System_Nullable_System_Int32__.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
				chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__59_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[132])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_58>b__59_0(System.String)
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_58_b__59_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
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
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	{
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INT32>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>(_s0);
		if (chaos_left > chaos_right)
		{
			_s0 = chaos_args[1];
			{
				const auto chaos_arg_0 = _s0;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Int32_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Int32_.hot;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
		}
		else
		{
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
		}
		return _s0;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_59()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_59(void)
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
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
	_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__60_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
			_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 85)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1) -> CHAOS_IL2CPP_INT64 {
					auto& _d_entry = s_hotpatch_entries[85];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[16];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_1));
						CHAOS_IL2CPP_INT64 _d_ret{};
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, &_d_ret);
						return _d_ret;
					}
					return reinterpret_cast<CHAOS_IL2CPP_INT64(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1);
				};
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s2;
				const auto chaos_target = _s1;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_String_System_Int64_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_String_System_Int64_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
				chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__60_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT64(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[134])(chaos_arg_0, chaos_arg_1);
			_s4 = ChaosStoreInt64(chaos_result);
		}
		return;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_59>b__60_0(System.String)
extern "C" CHAOS_IL2CPP_INT64 NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_59_b__60_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	{
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	return ChaosLoadInt64(_s0);
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_60()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_60(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_String.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{{
		_s3 = CHAOS_IL2CPP_STRING_ID("a");
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
		_s3 = CHAOS_IL2CPP_STRING_ID("bb");
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
		_s3 = CHAOS_IL2CPP_STRING_ID("ccc");
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
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
	_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__61_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
			_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 87)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
					auto& _d_entry = s_hotpatch_entries[87];
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
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_String_System_Nullable_System_Int64__, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_String_System_Nullable_System_Int64__.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
				chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__61_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[136])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_60>b__61_0(System.String)
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_60_b__61_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
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

	_s0 = chaos_args[1];
	{
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INT32>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>(_s0);
		if (chaos_left > chaos_right)
		{
			_s0 = chaos_args[1];
			{
				const auto chaos_arg_0 = _s0;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Int64_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Int64_.hot;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
		}
		else
		{
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
		}
		return _s0;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_61()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_61(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(75365335u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
	_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__62_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
			_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 89)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> float {
					auto& _d_entry = s_hotpatch_entries[89];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[12];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
						_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_fn_arg_1));
						float _d_ret{};
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, &_d_ret);
						return _d_ret;
					}
					return reinterpret_cast<float(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1);
				};
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s2;
				const auto chaos_target = _s1;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Single_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Single_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
				chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__62_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<float(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[138])(chaos_arg_0, chaos_arg_1);
			_s4 = ChaosStoreFloat32(chaos_result);
		}
		return;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_61>b__62_0(System.Int32)
extern "C" float NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_61_b__62_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	_s0 = ChaosStoreFloat32(static_cast<float>(_s0));
	return chaos_load_float32(_s0);
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_62()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_62(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(75365335u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
	_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__63_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
			_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 91)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
					auto& _d_entry = s_hotpatch_entries[91];
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
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Single__, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Single__.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
				chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__63_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[140])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_62>b__63_0(System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_62_b__63_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
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
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosRem(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	{
		if (_s0 == 0)
		{
			_s0 = chaos_args[1];
			_s0 = ChaosStoreFloat32(static_cast<float>(_s0));
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Single_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Single_.hot;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
		}
		else
		{
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
			{
				const auto chaos_address = _s2;
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
			_s2 = chaos_locals[0];
		}
		return _s1;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_63()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_63(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(75365335u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
	_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__64_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
			_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 93)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> double {
					auto& _d_entry = s_hotpatch_entries[93];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[12];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
						_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_fn_arg_1));
						double _d_ret{};
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, &_d_ret);
						return _d_ret;
					}
					return reinterpret_cast<double(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1);
				};
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s2;
				const auto chaos_target = _s1;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Double_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Double_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
				chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__64_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<double(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[142])(chaos_arg_0, chaos_arg_1);
			_s4 = ChaosStoreFloat64(chaos_result);
		}
		return;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_63>b__64_0(System.Int32)
extern "C" double NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_63_b__64_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	_s0 = ChaosStoreFloat64(static_cast<double>(_s0));
	return ChaosLoadFloat64(_s0);
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_64()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_64(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(75365335u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
	_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__65_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
			_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 95)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
					auto& _d_entry = s_hotpatch_entries[95];
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
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Double__, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Double__.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
				chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__65_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[144])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_64>b__65_0(System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_64_b__65_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
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
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosRem(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	{
		if (_s0 == 0)
		{
			_s0 = chaos_args[1];
			_s0 = ChaosStoreFloat64(static_cast<double>(_s0));
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Double_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Double_.hot;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
		}
		else
		{
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
			{
				const auto chaos_address = _s2;
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
			_s2 = chaos_locals[0];
		}
		return _s1;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_65()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_65(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(75365335u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
	_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__66_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
			_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 97)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
					auto& _d_entry = s_hotpatch_entries[97];
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
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Decimal_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Decimal_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
				chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__66_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[146])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_65>b__66_0(System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_65_b__66_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[48])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return _s0;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_66()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_66(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(75365335u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
	_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__67_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
			_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 99)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
					auto& _d_entry = s_hotpatch_entries[99];
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
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Decimal__, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Decimal__.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
				chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__67_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[148])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_66>b__67_0(System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_66_b__67_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
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
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosRem(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	{
		if (_s0 == 0)
		{
			_s0 = chaos_args[1];
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[48])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Decimal_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Decimal_.hot;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
		}
		else
		{
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
			{
				const auto chaos_address = _s2;
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
			_s2 = chaos_locals[0];
		}
		return _s1;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_67()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_67(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_String.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{{
		_s3 = CHAOS_IL2CPP_STRING_ID("zzz");
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
		_s3 = CHAOS_IL2CPP_STRING_ID("yy");
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
		_s3 = CHAOS_IL2CPP_STRING_ID("x");
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
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
	_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__68_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
			_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 101)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
					auto& _d_entry = s_hotpatch_entries[101];
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
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_String_System_String_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_String_System_String_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
				chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__68_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[150])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_67>b__68_0(System.String)
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_67_b__68_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	return _s0;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_68()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_68(void)
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
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
	_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__69_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
			_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 103)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1) -> CHAOS_IL2CPP_INT32 {
					auto& _d_entry = s_hotpatch_entries[103];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[16];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_1));
						CHAOS_IL2CPP_INT32 _d_ret{};
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, &_d_ret);
						return _d_ret;
					}
					return reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1);
				};
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s2;
				const auto chaos_target = _s1;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_String_System_Int32_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_String_System_Int32_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
				chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__69_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[152])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_68>b__69_0(System.String)
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_68_b__69_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	{
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_69()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_69(void)
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
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
	_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__70_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
			_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 105)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
					auto& _d_entry = s_hotpatch_entries[105];
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
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_String_System_String_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_String_System_String_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
				chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__70_0 = chaos_value;
			}
		}
		{
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[80])();
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			const auto chaos_arg_2 = _s6;
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[154])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_69>b__70_0(System.String)
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_69_b__70_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	return _s0;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_70()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_70(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(83620299u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[155])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_71()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_71(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Int64.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(67442793u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT64(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[156])(chaos_arg_0);
		_s0 = ChaosStoreInt64(chaos_result);
	}
	return;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_72()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_72(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Single.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(77934256u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<float(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[157])(chaos_arg_0);
		_s0 = ChaosStoreFloat32(chaos_result);
	}
	return;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_73()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_73(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Double.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(68641022u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<double(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[158])(chaos_arg_0);
		_s0 = ChaosStoreFloat64(chaos_result);
	}
	return;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_74()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_74(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Decimal.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(15);
	_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Decimal, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Decimal.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(25);
	_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Decimal, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Decimal.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(35);
	_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Decimal, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Decimal.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[159])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_75()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_75(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Int32_.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Int32_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Int32_.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Int32_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Int32_.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Int32_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Int32_.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[160])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_76()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_76(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Int64_.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(10);
	_s3 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s3));
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Int64_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Int64_.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(30);
	_s3 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s3));
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Int64_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Int64_.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[161])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_77()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_77(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Single_.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = ChaosStoreFloat32(1.5f);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Single_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Single_.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	_s3 = ChaosStoreFloat32(3.5f);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Single_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Single_.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[162])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_78()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_78(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Double_.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = ChaosStoreFloat64(1.5);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Double_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Double_.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	_s3 = ChaosStoreFloat64(3.5);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Double_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Double_.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[163])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_79()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_79(void)
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Decimal_.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(15);
	_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Decimal, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Decimal.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Decimal_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Decimal_.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(35);
	_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Decimal, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Decimal.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Decimal_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Decimal_.hot;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
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
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[164])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_80()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_80(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(75365335u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
	_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__81_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
			_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 117)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INT32 {
					auto& _d_entry = s_hotpatch_entries[117];
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
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
				chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__81_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[166])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_80>b__81_0(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_80_b__81_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_81()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_81(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(75365335u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
	_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__82_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
			_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 119)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INT64 {
					auto& _d_entry = s_hotpatch_entries[119];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[12];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
						_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_fn_arg_1));
						CHAOS_IL2CPP_INT64 _d_ret{};
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, &_d_ret);
						return _d_ret;
					}
					return reinterpret_cast<CHAOS_IL2CPP_INT64(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1);
				};
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s2;
				const auto chaos_target = _s1;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Int64_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Int64_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
				chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__82_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT64(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[168])(chaos_arg_0, chaos_arg_1);
			_s4 = ChaosStoreInt64(chaos_result);
		}
		return;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_81>b__82_0(System.Int32)
extern "C" CHAOS_IL2CPP_INT64 NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_81_b__82_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	_s0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	return ChaosLoadInt64(_s0);
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_82()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_82(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(75365335u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
	_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__83_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
			_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 121)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> float {
					auto& _d_entry = s_hotpatch_entries[121];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[12];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
						_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_fn_arg_1));
						float _d_ret{};
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, &_d_ret);
						return _d_ret;
					}
					return reinterpret_cast<float(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1);
				};
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s2;
				const auto chaos_target = _s1;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Single_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Single_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
				chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__83_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<float(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[170])(chaos_arg_0, chaos_arg_1);
			_s4 = ChaosStoreFloat32(chaos_result);
		}
		return;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_82>b__83_0(System.Int32)
extern "C" float NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_82_b__83_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	_s0 = ChaosStoreFloat32(static_cast<float>(_s0));
	return chaos_load_float32(_s0);
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_83()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_83(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(75365335u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
	_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__84_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
			_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 123)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> double {
					auto& _d_entry = s_hotpatch_entries[123];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[12];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
						_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_fn_arg_1));
						double _d_ret{};
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, &_d_ret);
						return _d_ret;
					}
					return reinterpret_cast<double(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1);
				};
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s2;
				const auto chaos_target = _s1;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Double_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Double_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
				chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__84_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<double(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[172])(chaos_arg_0, chaos_arg_1);
			_s4 = ChaosStoreFloat64(chaos_result);
		}
		return;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_83>b__84_0(System.Int32)
extern "C" double NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_83_b__84_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	_s0 = ChaosStoreFloat64(static_cast<double>(_s0));
	return ChaosLoadFloat64(_s0);
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_84()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_84(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(75365335u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
	_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__85_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
			_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 125)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
					auto& _d_entry = s_hotpatch_entries[125];
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
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Decimal_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Decimal_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
				chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__85_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[174])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_84>b__85_0(System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_84_b__85_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[48])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return _s0;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_85()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_85(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(75365335u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
	_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__86_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
			_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 127)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
					auto& _d_entry = s_hotpatch_entries[127];
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
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Int32__, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Int32__.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
				chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__86_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[176])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_85>b__86_0(System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_85_b__86_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
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
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosRem(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	{
		if (_s0 == 0)
		{
			_s0 = chaos_args[1];
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Int32_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Int32_.hot;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
		}
		else
		{
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
			{
				const auto chaos_address = _s2;
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
			_s2 = chaos_locals[0];
		}
		return _s1;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_86()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_86(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(75365335u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
	_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__87_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
			_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 129)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
					auto& _d_entry = s_hotpatch_entries[129];
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
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Int64__, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Int64__.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
				chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__87_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[178])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_86>b__87_0(System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_86_b__87_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
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
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosRem(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	{
		if (_s0 == 0)
		{
			_s0 = chaos_args[1];
			_s0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Int64_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Int64_.hot;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
		}
		else
		{
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
			{
				const auto chaos_address = _s2;
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
			_s2 = chaos_locals[0];
		}
		return _s1;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_87()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_87(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(75365335u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
	_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__88_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
			_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 131)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
					auto& _d_entry = s_hotpatch_entries[131];
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
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Single__, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Single__.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
				chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__88_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[180])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_87>b__88_0(System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_87_b__88_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
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
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosRem(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	{
		if (_s0 == 0)
		{
			_s0 = chaos_args[1];
			_s0 = ChaosStoreFloat32(static_cast<float>(_s0));
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Single_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Single_.hot;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
		}
		else
		{
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
			{
				const auto chaos_address = _s2;
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
			_s2 = chaos_locals[0];
		}
		return _s1;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_88()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_88(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(75365335u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
	_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__89_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
			_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 133)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
					auto& _d_entry = s_hotpatch_entries[133];
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
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Double__, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Double__.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
				chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__89_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[182])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_88>b__89_0(System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_88_b__89_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
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
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosRem(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	{
		if (_s0 == 0)
		{
			_s0 = chaos_args[1];
			_s0 = ChaosStoreFloat64(static_cast<double>(_s0));
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Double_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Double_.hot;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
		}
		else
		{
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
			{
				const auto chaos_address = _s2;
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
			_s2 = chaos_locals[0];
		}
		return _s1;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects::CustomEntrySubject_89()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects_CustomEntrySubject_89(void)
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(75365335u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
	_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__90_0;
	_s2 = _s1;
	{
		if (_s2 == 0)
		{
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
			_s1 = chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 135)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
					auto& _d_entry = s_hotpatch_entries[135];
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
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Decimal__, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_System_Nullable_System_Decimal__.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
				chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9__90_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s5;
			const auto chaos_arg_0 = _s4;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[184])(chaos_arg_0, chaos_arg_1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::<CustomEntrySubject_89>b__90_0(System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR NumericAggregationSubjects_NumericAggregationSubjects___c_CustomEntrySubject_89_b__90_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
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
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosRem(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	{
		if (_s0 == 0)
		{
			_s0 = chaos_args[1];
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[48])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Nullable_System_Decimal_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Nullable_System_Decimal_.hot;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
		}
		else
		{
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
			{
				const auto chaos_address = _s2;
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
			_s2 = chaos_locals[0];
		}
		return _s1;
	}
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::.cctor()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects___c_cctor(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};

	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();

	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_NumericAggregationSubjects_NumericAggregationSubjects___c, {});
		chaos_object->header.type_info = &chaos_mt_NumericAggregationSubjects_NumericAggregationSubjects___c.hot;
		NumericAggregationSubjects_NumericAggregationSubjects___c__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	{
		auto chaos_value = _s0;
	chaos_ensure_type_initialized_NumericAggregationSubjects_NumericAggregationSubjects___c();
		chaos_static_NumericAggregationSubjects_NumericAggregationSubjects___c____9 = chaos_value;
	}
	return;
}

// Managed method: NumericAggregationSubjects/NumericAggregationSubjects+<>c::.ctor()
extern "C" void NumericAggregationSubjects_NumericAggregationSubjects___c__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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



}  // namespace chaos::il2cpp::codegen::NumericAggregationSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 138;

extern "C" void ChaosJitRegisterAll() {}