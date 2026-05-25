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
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_CalcAdd_Add_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_CalcAdd_Multiply_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" void InterfaceDispatchSubjects_CalcAdd__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_CalcMul_Add_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_CalcMul_Multiply_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" void InterfaceDispatchSubjects_CalcMul__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_IWithDefault_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplDiamond2_BaseMethod(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplDiamond2_DerivedMethod(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void InterfaceDispatchSubjects_ImplDiamond2__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplDiamond_BaseMethod(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplDiamond_DerivedMethod(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void InterfaceDispatchSubjects_ImplDiamond__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplSimple_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void InterfaceDispatchSubjects_ImplSimple__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplWithDefault_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void InterfaceDispatchSubjects_ImplWithDefault__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_Run_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestAsCheck(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondBase(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondDerived(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondMulti(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestIsCheck(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestMultiImplCalc(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplDefault(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplSimple(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_0(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_1(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_2(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_3(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_4(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_5(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_6(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_7(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_8(void);

// Forward declarations (pipeline fix: used before extern "C" decl)
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_CalcAdd_Add_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_CalcAdd_Multiply_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" void InterfaceDispatchSubjects_CalcAdd__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_CalcMul_Add_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_CalcMul_Multiply_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" void InterfaceDispatchSubjects_CalcMul__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_IWithDefault_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplDiamond2_BaseMethod(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplDiamond2_DerivedMethod(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void InterfaceDispatchSubjects_ImplDiamond2__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplDiamond_BaseMethod(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplDiamond_DerivedMethod(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void InterfaceDispatchSubjects_ImplDiamond__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplSimple_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void InterfaceDispatchSubjects_ImplSimple__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplWithDefault_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void InterfaceDispatchSubjects_ImplWithDefault__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_Run_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestAsCheck(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondBase(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondDerived(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondMulti(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestIsCheck(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestMultiImplCalc(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplDefault(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplSimple(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_0(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_1(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_2(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_3(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_4(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_5(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_6(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_7(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_8(void);

// Forward declarations (pipeline fix: used before extern "C" decl)
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_CalcAdd_Add_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_CalcAdd_Multiply_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" void InterfaceDispatchSubjects_CalcAdd__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_CalcMul_Add_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_CalcMul_Multiply_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" void InterfaceDispatchSubjects_CalcMul__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_IWithDefault_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplDiamond2_BaseMethod(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplDiamond2_DerivedMethod(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void InterfaceDispatchSubjects_ImplDiamond2__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplDiamond_BaseMethod(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplDiamond_DerivedMethod(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void InterfaceDispatchSubjects_ImplDiamond__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplSimple_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void InterfaceDispatchSubjects_ImplSimple__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplWithDefault_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void InterfaceDispatchSubjects_ImplWithDefault__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_Run_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestAsCheck(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondBase(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondDerived(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondMulti(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestIsCheck(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestMultiImplCalc(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplDefault(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplSimple(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_0(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_1(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_2(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_3(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_4(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_5(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_6(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_7(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_8(void);

// Forward declarations (pipeline fix: used before extern "C" decl)
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_CalcAdd_Add_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_CalcAdd_Multiply_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" void InterfaceDispatchSubjects_CalcAdd__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_CalcMul_Add_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_CalcMul_Multiply_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" void InterfaceDispatchSubjects_CalcMul__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_IWithDefault_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplDiamond2_BaseMethod(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplDiamond2_DerivedMethod(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void InterfaceDispatchSubjects_ImplDiamond2__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplDiamond_BaseMethod(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplDiamond_DerivedMethod(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void InterfaceDispatchSubjects_ImplDiamond__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplSimple_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void InterfaceDispatchSubjects_ImplSimple__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplWithDefault_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void InterfaceDispatchSubjects_ImplWithDefault__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_Run_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestAsCheck(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondBase(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondDerived(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondMulti(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestIsCheck(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestMultiImplCalc(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplDefault(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplSimple(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_0(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_1(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_2(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_3(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_4(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_5(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_6(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_7(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_8(void);

// Forward declarations (pipeline fix: used before extern "C" decl)
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_CalcAdd_Add_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_CalcAdd_Multiply_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" void InterfaceDispatchSubjects_CalcAdd__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_CalcMul_Add_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_CalcMul_Multiply_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" void InterfaceDispatchSubjects_CalcMul__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_IWithDefault_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplDiamond2_BaseMethod(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplDiamond2_DerivedMethod(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void InterfaceDispatchSubjects_ImplDiamond2__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplDiamond_BaseMethod(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplDiamond_DerivedMethod(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void InterfaceDispatchSubjects_ImplDiamond__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplSimple_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void InterfaceDispatchSubjects_ImplSimple__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplWithDefault_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void InterfaceDispatchSubjects_ImplWithDefault__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_Run_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestAsCheck(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondBase(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondDerived(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondMulti(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestIsCheck(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestMultiImplCalc(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplDefault(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplSimple(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_0(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_1(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_2(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_3(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_4(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_5(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_6(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_7(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_8(void);

namespace chaos::il2cpp::codegen::InterfaceDispatchSubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_InterfaceDispatchSubjects_IBase = static_cast<CHAOS_IL2CPP_INTPTR>(15449752221026509750ULL);
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_InterfaceDispatchSubjects_ICalculator = static_cast<CHAOS_IL2CPP_INTPTR>(771964806139090035ULL);
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_InterfaceDispatchSubjects_IDerived = static_cast<CHAOS_IL2CPP_INTPTR>(3305311391869622176ULL);
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_InterfaceDispatchSubjects_ISimple = static_cast<CHAOS_IL2CPP_INTPTR>(14787273767725517097ULL);
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_InterfaceDispatchSubjects_IWithDefault = static_cast<CHAOS_IL2CPP_INTPTR>(8355604655153802020ULL);
const void* chaos_vtable_InterfaceDispatchSubjects_CalcAdd[];
const void* chaos_vtable_InterfaceDispatchSubjects_CalcMul[];
const void* chaos_vtable_InterfaceDispatchSubjects_ImplDiamond[];
const void* chaos_vtable_InterfaceDispatchSubjects_ImplDiamond2[];
const void* chaos_vtable_InterfaceDispatchSubjects_ImplSimple[];
const void* chaos_vtable_InterfaceDispatchSubjects_ImplWithDefault[];
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Object = {{nullptr, nullptr, 15228727185366376748ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_INTPTR>(15228727185366376748ULL);
static constexpr InterfaceMapEntry chaos_iface_map_InterfaceDispatchSubjects_CalcAdd[] = {
	{ chaos_type_id_InterfaceDispatchSubjects_ICalculator, 1, 2 }
};
inline TypeInfoV0 chaos_mt_InterfaceDispatchSubjects_CalcAdd = {{reinterpret_cast<const TypeInfoHot*>(&chaos_mt_System_Private_CoreLib_System_Object), chaos_vtable_InterfaceDispatchSubjects_CalcAdd, 2898228154134497916ULL, 3u, 32, 1, 1}, {chaos_iface_map_InterfaceDispatchSubjects_CalcAdd, nullptr, 1, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_InterfaceDispatchSubjects_CalcAdd = static_cast<CHAOS_IL2CPP_INTPTR>(2898228154134497916ULL);
static constexpr InterfaceMapEntry chaos_iface_map_InterfaceDispatchSubjects_CalcMul[] = {
	{ chaos_type_id_InterfaceDispatchSubjects_ICalculator, 1, 2 }
};
inline TypeInfoV0 chaos_mt_InterfaceDispatchSubjects_CalcMul = {{reinterpret_cast<const TypeInfoHot*>(&chaos_mt_System_Private_CoreLib_System_Object), chaos_vtable_InterfaceDispatchSubjects_CalcMul, 13787125010580401065ULL, 3u, 32, 1, 1}, {chaos_iface_map_InterfaceDispatchSubjects_CalcMul, nullptr, 1, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_InterfaceDispatchSubjects_CalcMul = static_cast<CHAOS_IL2CPP_INTPTR>(13787125010580401065ULL);
static constexpr InterfaceMapEntry chaos_iface_map_InterfaceDispatchSubjects_ImplDiamond[] = {
	{ chaos_type_id_InterfaceDispatchSubjects_IDerived, 4, 1 },
	{ chaos_type_id_InterfaceDispatchSubjects_IBase, 3, 1 }
};
inline TypeInfoV0 chaos_mt_InterfaceDispatchSubjects_ImplDiamond = {{reinterpret_cast<const TypeInfoHot*>(&chaos_mt_System_Private_CoreLib_System_Object), chaos_vtable_InterfaceDispatchSubjects_ImplDiamond, 5829620332136686238ULL, 5u, 32, 1, 1}, {chaos_iface_map_InterfaceDispatchSubjects_ImplDiamond, nullptr, 2, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_InterfaceDispatchSubjects_ImplDiamond = static_cast<CHAOS_IL2CPP_INTPTR>(5829620332136686238ULL);
static constexpr InterfaceMapEntry chaos_iface_map_InterfaceDispatchSubjects_ImplDiamond2[] = {
	{ chaos_type_id_InterfaceDispatchSubjects_IDerived, 4, 1 },
	{ chaos_type_id_InterfaceDispatchSubjects_IBase, 3, 1 }
};
inline TypeInfoV0 chaos_mt_InterfaceDispatchSubjects_ImplDiamond2 = {{reinterpret_cast<const TypeInfoHot*>(&chaos_mt_System_Private_CoreLib_System_Object), chaos_vtable_InterfaceDispatchSubjects_ImplDiamond2, 16094583433855183428ULL, 5u, 32, 1, 1}, {chaos_iface_map_InterfaceDispatchSubjects_ImplDiamond2, nullptr, 2, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_InterfaceDispatchSubjects_ImplDiamond2 = static_cast<CHAOS_IL2CPP_INTPTR>(16094583433855183428ULL);
static constexpr InterfaceMapEntry chaos_iface_map_InterfaceDispatchSubjects_ImplSimple[] = {
	{ chaos_type_id_InterfaceDispatchSubjects_ISimple, 5, 1 }
};
inline TypeInfoV0 chaos_mt_InterfaceDispatchSubjects_ImplSimple = {{reinterpret_cast<const TypeInfoHot*>(&chaos_mt_System_Private_CoreLib_System_Object), chaos_vtable_InterfaceDispatchSubjects_ImplSimple, 17332692813202302180ULL, 6u, 32, 1, 1}, {chaos_iface_map_InterfaceDispatchSubjects_ImplSimple, nullptr, 1, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_InterfaceDispatchSubjects_ImplSimple = static_cast<CHAOS_IL2CPP_INTPTR>(17332692813202302180ULL);
static constexpr InterfaceMapEntry chaos_iface_map_InterfaceDispatchSubjects_ImplWithDefault[] = {
	{ chaos_type_id_InterfaceDispatchSubjects_IWithDefault, 5, 1 }
};
inline TypeInfoV0 chaos_mt_InterfaceDispatchSubjects_ImplWithDefault = {{reinterpret_cast<const TypeInfoHot*>(&chaos_mt_System_Private_CoreLib_System_Object), chaos_vtable_InterfaceDispatchSubjects_ImplWithDefault, 10131636431187859815ULL, 6u, 32, 1, 1}, {chaos_iface_map_InterfaceDispatchSubjects_ImplWithDefault, nullptr, 1, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_InterfaceDispatchSubjects_ImplWithDefault = static_cast<CHAOS_IL2CPP_INTPTR>(10131636431187859815ULL);
inline TypeInfoV0 chaos_mt_InterfaceDispatchSubjects_InterfaceDispatchSubjects = {{nullptr, nullptr, 16224833485471026240ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_InterfaceDispatchSubjects_InterfaceDispatchSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(16224833485471026240ULL);
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
inline TypeInfoV0 chaos_mt_InterfaceDispatchSubjects_IBase = {{nullptr, nullptr, 15449752221026509750ULL, 0u, 32, 3, 0}, {nullptr, nullptr, 0, 0, 0, 0}};
inline TypeInfoV0 chaos_mt_InterfaceDispatchSubjects_ICalculator = {{nullptr, nullptr, 771964806139090035ULL, 0u, 32, 3, 0}, {nullptr, nullptr, 0, 0, 0, 0}};
inline TypeInfoV0 chaos_mt_InterfaceDispatchSubjects_IDerived = {{nullptr, nullptr, 3305311391869622176ULL, 0u, 32, 3, 0}, {nullptr, nullptr, 0, 0, 0, 0}};
inline TypeInfoV0 chaos_mt_InterfaceDispatchSubjects_ISimple = {{nullptr, nullptr, 14787273767725517097ULL, 0u, 32, 3, 0}, {nullptr, nullptr, 0, 0, 0, 0}};
inline TypeInfoV0 chaos_mt_InterfaceDispatchSubjects_IWithDefault = {{nullptr, nullptr, 8355604655153802020ULL, 0u, 32, 3, 0}, {nullptr, nullptr, 0, 0, 0, 0}};

// ── Virtual method table arrays ──
extern "C" void InterfaceDispatchSubjects_CalcAdd__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_CalcAdd_Add_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_CalcAdd_Multiply_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
const void* chaos_vtable_InterfaceDispatchSubjects_CalcAdd[] =
{
	reinterpret_cast<void*>(InterfaceDispatchSubjects_CalcAdd__ctor),
	reinterpret_cast<void*>(InterfaceDispatchSubjects_CalcAdd_Add_System_Int32_System_Int32),
	reinterpret_cast<void*>(InterfaceDispatchSubjects_CalcAdd_Multiply_System_Int32_System_Int32),
};
extern "C" void InterfaceDispatchSubjects_CalcMul__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_CalcMul_Add_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_CalcMul_Multiply_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
const void* chaos_vtable_InterfaceDispatchSubjects_CalcMul[] =
{
	reinterpret_cast<void*>(InterfaceDispatchSubjects_CalcMul__ctor),
	reinterpret_cast<void*>(InterfaceDispatchSubjects_CalcMul_Add_System_Int32_System_Int32),
	reinterpret_cast<void*>(InterfaceDispatchSubjects_CalcMul_Multiply_System_Int32_System_Int32),
};
extern "C" void InterfaceDispatchSubjects_ImplDiamond__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplDiamond_BaseMethod(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplDiamond_DerivedMethod(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
const void* chaos_vtable_InterfaceDispatchSubjects_ImplDiamond[] =
{
	reinterpret_cast<void*>(InterfaceDispatchSubjects_ImplDiamond__ctor),
	nullptr,
	nullptr,
	reinterpret_cast<void*>(InterfaceDispatchSubjects_ImplDiamond_BaseMethod),
	reinterpret_cast<void*>(InterfaceDispatchSubjects_ImplDiamond_DerivedMethod),
};
extern "C" void InterfaceDispatchSubjects_ImplDiamond2__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplDiamond2_BaseMethod(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplDiamond2_DerivedMethod(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
const void* chaos_vtable_InterfaceDispatchSubjects_ImplDiamond2[] =
{
	reinterpret_cast<void*>(InterfaceDispatchSubjects_ImplDiamond2__ctor),
	nullptr,
	nullptr,
	reinterpret_cast<void*>(InterfaceDispatchSubjects_ImplDiamond2_BaseMethod),
	reinterpret_cast<void*>(InterfaceDispatchSubjects_ImplDiamond2_DerivedMethod),
};
extern "C" void InterfaceDispatchSubjects_ImplSimple__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplSimple_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
const void* chaos_vtable_InterfaceDispatchSubjects_ImplSimple[] =
{
	reinterpret_cast<void*>(InterfaceDispatchSubjects_ImplSimple__ctor),
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	reinterpret_cast<void*>(InterfaceDispatchSubjects_ImplSimple_GetValue),
};
extern "C" void InterfaceDispatchSubjects_ImplWithDefault__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplWithDefault_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
const void* chaos_vtable_InterfaceDispatchSubjects_ImplWithDefault[] =
{
	reinterpret_cast<void*>(InterfaceDispatchSubjects_ImplWithDefault__ctor),
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	reinterpret_cast<void*>(InterfaceDispatchSubjects_ImplWithDefault_GetValue),
};
static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_InterfaceDispatchSubjects_CalcAdd[] =
{
	{ 0x00000024u, reinterpret_cast<void*>(&InterfaceDispatchSubjects_CalcAdd__ctor) },
	{ 0x00000022u, reinterpret_cast<void*>(&InterfaceDispatchSubjects_CalcAdd_Add_System_Int32_System_Int32) },
	{ 0x00000023u, reinterpret_cast<void*>(&InterfaceDispatchSubjects_CalcAdd_Multiply_System_Int32_System_Int32) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_InterfaceDispatchSubjects_CalcMul[] =
{
	{ 0x00000027u, reinterpret_cast<void*>(&InterfaceDispatchSubjects_CalcMul__ctor) },
	{ 0x00000025u, reinterpret_cast<void*>(&InterfaceDispatchSubjects_CalcMul_Add_System_Int32_System_Int32) },
	{ 0x00000026u, reinterpret_cast<void*>(&InterfaceDispatchSubjects_CalcMul_Multiply_System_Int32_System_Int32) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_InterfaceDispatchSubjects_ImplDiamond[] =
{
	{ 0x0000002Au, reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplDiamond__ctor) },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0x00000028u, reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplDiamond_BaseMethod) },
	{ 0x00000029u, reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplDiamond_DerivedMethod) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_InterfaceDispatchSubjects_ImplDiamond2[] =
{
	{ 0x0000002Du, reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplDiamond2__ctor) },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0x0000002Bu, reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplDiamond2_BaseMethod) },
	{ 0x0000002Cu, reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplDiamond2_DerivedMethod) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_InterfaceDispatchSubjects_ImplSimple[] =
{
	{ 0x0000001Fu, reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplSimple__ctor) },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0x0000001Eu, reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplSimple_GetValue) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_InterfaceDispatchSubjects_ImplWithDefault[] =
{
	{ 0x00000021u, reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplWithDefault__ctor) },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0x00000020u, reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplWithDefault_GetValue) },
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
		return chaos_array->element_type_shape == chaos_type_shape_reference            && chaos_is_type_compatible(reinterpret_cast<const TypeInfoHot*>(&chaos_mt_System_Private_CoreLib_System_String), chaos_array->element_type_info);
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

struct chaos_type_System_Private_CoreLib_System_Object
{
	ThinLockableHeader header{};
};

struct chaos_type_InterfaceDispatchSubjects_CalcAdd : public chaos_type_System_Private_CoreLib_System_Object
{
};

struct chaos_type_InterfaceDispatchSubjects_CalcMul : public chaos_type_System_Private_CoreLib_System_Object
{
};

struct chaos_type_InterfaceDispatchSubjects_ImplDiamond : public chaos_type_System_Private_CoreLib_System_Object
{
};

struct chaos_type_InterfaceDispatchSubjects_ImplDiamond2 : public chaos_type_System_Private_CoreLib_System_Object
{
};

struct chaos_type_InterfaceDispatchSubjects_ImplSimple : public chaos_type_System_Private_CoreLib_System_Object
{
};

struct chaos_type_InterfaceDispatchSubjects_ImplWithDefault : public chaos_type_System_Private_CoreLib_System_Object
{
};

struct chaos_type_InterfaceDispatchSubjects_InterfaceDispatchSubjects
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
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_Assembly[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_Assembly, runtime_assembly_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, declaring_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, runtime_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, generic_definition_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, generic_argument_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, declaring_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, declaring_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, runtime_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, generic_definition_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, generic_argument_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, runtime_name_value))};

extern "C" void ChaosRegisterGcLayouts() {
	auto& registry = chaos::il2cpp::runtime_core::GcLayoutRegistry::Instance();

	registry.Register(15228727185366376748ULL, sizeof(chaos_type_System_Private_CoreLib_System_Object), nullptr, 0);
	registry.Register(2898228154134497916ULL, sizeof(chaos_type_InterfaceDispatchSubjects_CalcAdd), nullptr, 0);
	registry.Register(13787125010580401065ULL, sizeof(chaos_type_InterfaceDispatchSubjects_CalcMul), nullptr, 0);
	registry.Register(5829620332136686238ULL, sizeof(chaos_type_InterfaceDispatchSubjects_ImplDiamond), nullptr, 0);
	registry.Register(16094583433855183428ULL, sizeof(chaos_type_InterfaceDispatchSubjects_ImplDiamond2), nullptr, 0);
	registry.Register(17332692813202302180ULL, sizeof(chaos_type_InterfaceDispatchSubjects_ImplSimple), nullptr, 0);
	registry.Register(10131636431187859815ULL, sizeof(chaos_type_InterfaceDispatchSubjects_ImplWithDefault), nullptr, 0);
	registry.Register(16224833485471026240ULL, sizeof(chaos_type_InterfaceDispatchSubjects_InterfaceDispatchSubjects), nullptr, 0);
	registry.Register(5474029880995115448ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_Assembly), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_Assembly, 1);
	registry.Register(17082367815459723707ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName, 1);
	registry.Register(4137207361503509124ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, 5);
	registry.Register(17040031516751226236ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, 2);
	registry.Register(10748947813473285525ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, 5);
	registry.Register(1782325859292956794ULL, sizeof(chaos_type_System_Private_CoreLib_System_String), nullptr, 0);

	// Register TypeInfoV0 address ranges for IsValidTypeInfoPointer.
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Object), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Object) + sizeof(chaos_mt_System_Private_CoreLib_System_Object));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_InterfaceDispatchSubjects_CalcAdd), reinterpret_cast<uintptr_t>(&chaos_mt_InterfaceDispatchSubjects_CalcAdd) + sizeof(chaos_mt_InterfaceDispatchSubjects_CalcAdd));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_InterfaceDispatchSubjects_CalcMul), reinterpret_cast<uintptr_t>(&chaos_mt_InterfaceDispatchSubjects_CalcMul) + sizeof(chaos_mt_InterfaceDispatchSubjects_CalcMul));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_InterfaceDispatchSubjects_ImplDiamond), reinterpret_cast<uintptr_t>(&chaos_mt_InterfaceDispatchSubjects_ImplDiamond) + sizeof(chaos_mt_InterfaceDispatchSubjects_ImplDiamond));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_InterfaceDispatchSubjects_ImplDiamond2), reinterpret_cast<uintptr_t>(&chaos_mt_InterfaceDispatchSubjects_ImplDiamond2) + sizeof(chaos_mt_InterfaceDispatchSubjects_ImplDiamond2));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_InterfaceDispatchSubjects_ImplSimple), reinterpret_cast<uintptr_t>(&chaos_mt_InterfaceDispatchSubjects_ImplSimple) + sizeof(chaos_mt_InterfaceDispatchSubjects_ImplSimple));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_InterfaceDispatchSubjects_ImplWithDefault), reinterpret_cast<uintptr_t>(&chaos_mt_InterfaceDispatchSubjects_ImplWithDefault) + sizeof(chaos_mt_InterfaceDispatchSubjects_ImplWithDefault));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_InterfaceDispatchSubjects_InterfaceDispatchSubjects), reinterpret_cast<uintptr_t>(&chaos_mt_InterfaceDispatchSubjects_InterfaceDispatchSubjects) + sizeof(chaos_mt_InterfaceDispatchSubjects_InterfaceDispatchSubjects));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_Assembly), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_Assembly) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_Assembly));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_String), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_String) + sizeof(chaos_mt_System_Private_CoreLib_System_String));
}

CHAOS_IL2CPP_INT32 chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = 0;

static void chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	ChaosObjectCtor(chaos_fn_arg_0);
}

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };


// ── Bridge/import thunks ──
extern "C" void* kChaosExternalRuntimeFnTable[];

// Bridge/import thunk for: InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode
extern "C" void chaos_bridge_thunk_0(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[0])();
}

// Bridge/import thunk for: InterfaceDispatchSubjects/ISimple
extern "C" void chaos_bridge_thunk_1(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[2])();
}

// Bridge/import thunk for: InterfaceDispatchSubjects/ICalculator
extern "C" void chaos_bridge_thunk_2(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[3])();
}

extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_0(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_1(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_2(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_3(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_4(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_5(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_6(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_7(void);
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_8(void);
extern "C" void InterfaceDispatchSubjects_CalcAdd__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_CalcAdd_Add_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_CalcAdd_Multiply_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" void InterfaceDispatchSubjects_CalcMul__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_CalcMul_Add_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_CalcMul_Multiply_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" void InterfaceDispatchSubjects_ImplDiamond__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplDiamond_BaseMethod(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplDiamond_DerivedMethod(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void InterfaceDispatchSubjects_ImplDiamond2__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplDiamond2_BaseMethod(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplDiamond2_DerivedMethod(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void InterfaceDispatchSubjects_ImplSimple__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplSimple_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void InterfaceDispatchSubjects_ImplWithDefault__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplWithDefault_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_Run_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestAsCheck(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondBase(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondDerived(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondMulti(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestIsCheck(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestMultiImplCalc(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplDefault(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplSimple(void);
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_IWithDefault_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);


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
	0u,
	0u,
	0u,
	0u,
	2u,
	4u,
	4u,
	6u,
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
	9u,
	9u,
	9u,
	9u,
	9u,
	9u,
	9u,
};

static constexpr struct {
	::ChaosAbiManifestV0 header;
	::ChaosAbiMethodEntryV0 entries[35];
	CHAOS_IL2CPP_UINT8 params[9];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		35u,
		9u,
		1723331133u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_0
		{ 0u, 0u },  // InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_1
		{ 0u, 0u },  // InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_2
		{ 0u, 0u },  // InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_3
		{ 0u, 0u },  // InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_4
		{ 0u, 0u },  // InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_5
		{ 0u, 0u },  // InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_6
		{ 0u, 0u },  // InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_7
		{ 0u, 0u },  // InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_8
		{ 0u, 0u },  // InterfaceDispatchSubjects_CalcAdd__ctor
		{ 1u, 2u },  // InterfaceDispatchSubjects_CalcAdd_Add_System_Int32_System_Int32
		{ 1u, 2u },  // InterfaceDispatchSubjects_CalcAdd_Multiply_System_Int32_System_Int32
		{ 0u, 0u },  // InterfaceDispatchSubjects_CalcMul__ctor
		{ 1u, 2u },  // InterfaceDispatchSubjects_CalcMul_Add_System_Int32_System_Int32
		{ 1u, 2u },  // InterfaceDispatchSubjects_CalcMul_Multiply_System_Int32_System_Int32
		{ 0u, 0u },  // InterfaceDispatchSubjects_ImplDiamond__ctor
		{ 1u, 0u },  // InterfaceDispatchSubjects_ImplDiamond_BaseMethod
		{ 1u, 0u },  // InterfaceDispatchSubjects_ImplDiamond_DerivedMethod
		{ 0u, 0u },  // InterfaceDispatchSubjects_ImplDiamond2__ctor
		{ 1u, 0u },  // InterfaceDispatchSubjects_ImplDiamond2_BaseMethod
		{ 1u, 0u },  // InterfaceDispatchSubjects_ImplDiamond2_DerivedMethod
		{ 0u, 0u },  // InterfaceDispatchSubjects_ImplSimple__ctor
		{ 1u, 0u },  // InterfaceDispatchSubjects_ImplSimple_GetValue
		{ 0u, 0u },  // InterfaceDispatchSubjects_ImplWithDefault__ctor
		{ 1u, 0u },  // InterfaceDispatchSubjects_ImplWithDefault_GetValue
		{ 1u, 1u },  // InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_Run_System_Int32
		{ 1u, 0u },  // InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestAsCheck
		{ 1u, 0u },  // InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondBase
		{ 1u, 0u },  // InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondDerived
		{ 1u, 0u },  // InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondMulti
		{ 1u, 0u },  // InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestIsCheck
		{ 1u, 0u },  // InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestMultiImplCalc
		{ 1u, 0u },  // InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplDefault
		{ 1u, 0u },  // InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplSimple
		{ 1u, 0u },  // InterfaceDispatchSubjects_IWithDefault_GetValue
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
	},
};
static const ::ChaosAbiManifestV0* const s_abi_manifest =
	reinterpret_cast<const ::ChaosAbiManifestV0*>(&s_abi_manifest_storage);
// ── Module registration ──

	static constexpr CHAOS_IL2CPP_UINT32 s_type_flags[14] = {
		0u,
		1548u,
		1556u,
		1556u,
		1556u,
		1556u,
		1556u,
		1536u,
		1536u,
		1536u,
		1536u,
		1536u,
		1536u,
		1548u,
	};

	static constexpr const char* s_type_names[14] = {
		"<Module>",
		"InterfaceDispatchSubjects",
		"ISimple",
		"IWithDefault",
		"ICalculator",
		"IBase",
		"IDerived",
		"ImplSimple",
		"ImplWithDefault",
		"CalcAdd",
		"CalcMul",
		"ImplDiamond",
		"ImplDiamond2",
		"InterfaceDispatchNativeEntry",
	};

	static constexpr const char* s_type_namespaces[14] = {
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
		"",
		"",
		"",
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_type_parent_tokens[14] = {
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

	static const TypeInfoHot* const s_type_info_ptrs[14] = {
		nullptr,
		reinterpret_cast<const TypeInfoHot*>(&chaos_mt_InterfaceDispatchSubjects_InterfaceDispatchSubjects),
		reinterpret_cast<const TypeInfoHot*>(&chaos_mt_InterfaceDispatchSubjects_ISimple),
		reinterpret_cast<const TypeInfoHot*>(&chaos_mt_InterfaceDispatchSubjects_IWithDefault),
		reinterpret_cast<const TypeInfoHot*>(&chaos_mt_InterfaceDispatchSubjects_ICalculator),
		reinterpret_cast<const TypeInfoHot*>(&chaos_mt_InterfaceDispatchSubjects_IBase),
		reinterpret_cast<const TypeInfoHot*>(&chaos_mt_InterfaceDispatchSubjects_IDerived),
		reinterpret_cast<const TypeInfoHot*>(&chaos_mt_InterfaceDispatchSubjects_ImplSimple),
		reinterpret_cast<const TypeInfoHot*>(&chaos_mt_InterfaceDispatchSubjects_ImplWithDefault),
		reinterpret_cast<const TypeInfoHot*>(&chaos_mt_InterfaceDispatchSubjects_CalcAdd),
		reinterpret_cast<const TypeInfoHot*>(&chaos_mt_InterfaceDispatchSubjects_CalcMul),
		reinterpret_cast<const TypeInfoHot*>(&chaos_mt_InterfaceDispatchSubjects_ImplDiamond),
		reinterpret_cast<const TypeInfoHot*>(&chaos_mt_InterfaceDispatchSubjects_ImplDiamond2),
		nullptr,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_offset[15] = {
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

	static constexpr CHAOS_IL2CPP_UINT32 s_generic_param_constraint_offset[15] = {
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



	static const ::chaos::il2cpp::runtime_core::ModuleDescriptor s_native_aot_module = {
		/* .name_utf8         = */ "InterfaceDispatchSubjects",
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
		/* .type_count        = */ 14u,
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
		::chaos::il2cpp::runtime_core::RegisterModule("InterfaceDispatchSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[35] = {
	{ ".ctor", 0x00000024u, 0u },  // CalcAdd
	{ "Add", 0x00000022u, 2u },  // CalcAdd
	{ "Multiply", 0x00000023u, 2u },  // CalcAdd
	{ ".ctor", 0x00000027u, 0u },  // CalcMul
	{ "Add", 0x00000025u, 2u },  // CalcMul
	{ "Multiply", 0x00000026u, 2u },  // CalcMul
	{ "GetValue", 0x00000019u, 0u },  // IWithDefault
	{ ".ctor", 0x0000002Au, 0u },  // ImplDiamond
	{ "BaseMethod", 0x00000028u, 0u },  // ImplDiamond
	{ "DerivedMethod", 0x00000029u, 0u },  // ImplDiamond
	{ ".ctor", 0x0000002Du, 0u },  // ImplDiamond2
	{ "BaseMethod", 0x0000002Bu, 0u },  // ImplDiamond2
	{ "DerivedMethod", 0x0000002Cu, 0u },  // ImplDiamond2
	{ ".ctor", 0x0000001Fu, 0u },  // ImplSimple
	{ "GetValue", 0x0000001Eu, 0u },  // ImplSimple
	{ ".ctor", 0x00000021u, 0u },  // ImplWithDefault
	{ "GetValue", 0x00000020u, 0u },  // ImplWithDefault
	{ "Run", 0x0000002Eu, 1u },  // InterfaceDispatchNativeEntry
	{ "TestAsCheck", 0x00000033u, 0u },  // InterfaceDispatchNativeEntry
	{ "TestDiamondBase", 0x00000034u, 0u },  // InterfaceDispatchNativeEntry
	{ "TestDiamondDerived", 0x00000035u, 0u },  // InterfaceDispatchNativeEntry
	{ "TestDiamondMulti", 0x00000036u, 0u },  // InterfaceDispatchNativeEntry
	{ "TestIsCheck", 0x00000032u, 0u },  // InterfaceDispatchNativeEntry
	{ "TestMultiImplCalc", 0x00000031u, 0u },  // InterfaceDispatchNativeEntry
	{ "TestSingleImplDefault", 0x00000030u, 0u },  // InterfaceDispatchNativeEntry
	{ "TestSingleImplSimple", 0x0000002Fu, 0u },  // InterfaceDispatchNativeEntry
	{ "Subject_0", 0x0000000Fu, 0u },  // InterfaceDispatchSubjects
	{ "Subject_1", 0x00000010u, 0u },  // InterfaceDispatchSubjects
	{ "Subject_2", 0x00000011u, 0u },  // InterfaceDispatchSubjects
	{ "Subject_3", 0x00000012u, 0u },  // InterfaceDispatchSubjects
	{ "Subject_4", 0x00000013u, 0u },  // InterfaceDispatchSubjects
	{ "Subject_5", 0x00000014u, 0u },  // InterfaceDispatchSubjects
	{ "Subject_6", 0x00000015u, 0u },  // InterfaceDispatchSubjects
	{ "Subject_7", 0x00000016u, 0u },  // InterfaceDispatchSubjects
	{ "Subject_8", 0x00000017u, 0u },  // InterfaceDispatchSubjects
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[9] = {
	{ "CalcAdd", "", 0u, 3u },
	{ "CalcMul", "", 3u, 3u },
	{ "IWithDefault", "", 6u, 1u },
	{ "ImplDiamond", "", 7u, 3u },
	{ "ImplDiamond2", "", 10u, 3u },
	{ "ImplSimple", "", 13u, 2u },
	{ "ImplWithDefault", "", 15u, 2u },
	{ "InterfaceDispatchNativeEntry", "", 17u, 9u },
	{ "InterfaceDispatchSubjects", "", 26u, 9u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[35] = {
	{ 0x0000000Fu, 0u },
	{ 0x00000010u, 1u },
	{ 0x00000011u, 2u },
	{ 0x00000012u, 3u },
	{ 0x00000013u, 4u },
	{ 0x00000014u, 5u },
	{ 0x00000015u, 6u },
	{ 0x00000016u, 7u },
	{ 0x00000017u, 8u },
	{ 0x00000019u, 34u },
	{ 0x0000001Eu, 22u },
	{ 0x0000001Fu, 21u },
	{ 0x00000020u, 24u },
	{ 0x00000021u, 23u },
	{ 0x00000022u, 10u },
	{ 0x00000023u, 11u },
	{ 0x00000024u, 9u },
	{ 0x00000025u, 13u },
	{ 0x00000026u, 14u },
	{ 0x00000027u, 12u },
	{ 0x00000028u, 16u },
	{ 0x00000029u, 17u },
	{ 0x0000002Au, 15u },
	{ 0x0000002Bu, 19u },
	{ 0x0000002Cu, 20u },
	{ 0x0000002Du, 18u },
	{ 0x0000002Eu, 25u },
	{ 0x0000002Fu, 33u },
	{ 0x00000030u, 32u },
	{ 0x00000031u, 31u },
	{ 0x00000032u, 30u },
	{ 0x00000033u, 26u },
	{ 0x00000034u, 27u },
	{ 0x00000035u, 28u },
	{ 0x00000036u, 29u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[35] = {
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // InterfaceDispatchSubjects::Subject_0
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // InterfaceDispatchSubjects::Subject_1
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // InterfaceDispatchSubjects::Subject_2
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // InterfaceDispatchSubjects::Subject_3
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // InterfaceDispatchSubjects::Subject_4
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // InterfaceDispatchSubjects::Subject_5
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // InterfaceDispatchSubjects::Subject_6
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // InterfaceDispatchSubjects::Subject_7
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // InterfaceDispatchSubjects::Subject_8
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_CalcAdd__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // CalcAdd::.ctor
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_CalcAdd_Add_System_Int32_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // CalcAdd::Add
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_CalcAdd_Multiply_System_Int32_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // CalcAdd::Multiply
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_CalcMul__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // CalcMul::.ctor
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_CalcMul_Add_System_Int32_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // CalcMul::Add
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_CalcMul_Multiply_System_Int32_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // CalcMul::Multiply
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplDiamond__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ImplDiamond::.ctor
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplDiamond_BaseMethod), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ImplDiamond::BaseMethod
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplDiamond_DerivedMethod), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ImplDiamond::DerivedMethod
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplDiamond2__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ImplDiamond2::.ctor
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplDiamond2_BaseMethod), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ImplDiamond2::BaseMethod
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplDiamond2_DerivedMethod), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ImplDiamond2::DerivedMethod
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplSimple__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ImplSimple::.ctor
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplSimple_GetValue), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ImplSimple::GetValue
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplWithDefault__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ImplWithDefault::.ctor
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplWithDefault_GetValue), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ImplWithDefault::GetValue
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_Run_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // InterfaceDispatchNativeEntry::Run
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestAsCheck), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // InterfaceDispatchNativeEntry::TestAsCheck
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondBase), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // InterfaceDispatchNativeEntry::TestDiamondBase
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondDerived), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // InterfaceDispatchNativeEntry::TestDiamondDerived
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondMulti), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // InterfaceDispatchNativeEntry::TestDiamondMulti
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestIsCheck), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // InterfaceDispatchNativeEntry::TestIsCheck
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestMultiImplCalc), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // InterfaceDispatchNativeEntry::TestMultiImplCalc
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplDefault), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // InterfaceDispatchNativeEntry::TestSingleImplDefault
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplSimple), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // InterfaceDispatchNativeEntry::TestSingleImplSimple
	{ reinterpret_cast<void*>(&InterfaceDispatchSubjects_IWithDefault_GetValue), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // IWithDefault::GetValue
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"InterfaceDispatchSubjects",
	s_hotpatch_types,
	9u,
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

extern "C" const char* kChaosExternalRuntimeSubjects[4] = {
	"InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode",
	"System.Private.CoreLib/System.Object::.ctor:System.Void()",
	"InterfaceDispatchSubjects/ISimple",
	"InterfaceDispatchSubjects/ICalculator",
};

extern "C" void* kChaosExternalRuntimeFnTable[4] = {
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__),
	nullptr,
	nullptr,
};

extern "C" int32_t kChaosExternalRuntimeCount = 4;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[35])() = {
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_0),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_1),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_2),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_3),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_4),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_5),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_6),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_7),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_8),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_CalcAdd__ctor),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_CalcAdd_Add_System_Int32_System_Int32),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_CalcAdd_Multiply_System_Int32_System_Int32),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_CalcMul__ctor),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_CalcMul_Add_System_Int32_System_Int32),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_CalcMul_Multiply_System_Int32_System_Int32),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_ImplDiamond__ctor),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_ImplDiamond_BaseMethod),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_ImplDiamond_DerivedMethod),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_ImplDiamond2__ctor),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_ImplDiamond2_BaseMethod),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_ImplDiamond2_DerivedMethod),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_ImplSimple__ctor),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_ImplSimple_GetValue),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_ImplWithDefault__ctor),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_ImplWithDefault_GetValue),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_Run_System_Int32),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestAsCheck),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondBase),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondDerived),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondMulti),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestIsCheck),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestMultiImplCalc),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplDefault),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplSimple),
	reinterpret_cast<void(*)()>(&InterfaceDispatchSubjects_IWithDefault_GetValue),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
extern "C" void (*kBenchmarkWrappers[35])() = {
	[]() {kAotMethods[0]();},
	[]() {kAotMethods[1]();},
	[]() {kAotMethods[2]();},
	[]() {kAotMethods[3]();},
	[]() {kAotMethods[4]();},
	[]() {kAotMethods[5]();},
	[]() {kAotMethods[6]();},
	[]() {kAotMethods[7]();},
	[]() {kAotMethods[8]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[9])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[10])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0,0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[11])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0,0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[12])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[13])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0,0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[14])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0,0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[15])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[16])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[17])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[18])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[19])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[20])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[21])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[22])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[23])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[24])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[25])(0);},
	[]() {kAotMethods[26]();},
	[]() {kAotMethods[27]();},
	[]() {kAotMethods[28]();},
	[]() {kAotMethods[29]();},
	[]() {kAotMethods[30]();},
	[]() {kAotMethods[31]();},
	[]() {kAotMethods[32]();},
	[]() {kAotMethods[33]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[34])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
};

// ── Subject entry index mapping ─────────────────────────────────
// Maps subject index (0-based sequential) to kAotMethod index.
// Used by runtime-entry.cpp to route --benchmark N to the correct
// AOT method slot, since kAotMethods[] includes lambdas/closures
// that shift subject methods to non-contiguous indices.
extern "C" const int kSubjectEntryCount = 9;
extern "C" const int kSubjectEntryIndices[9] = {
	0,
	1,
	2,
	3,
	4,
	5,
	6,
	7,
	8,
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

	/* ── Entry 0: InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry0;
	/* ── Entry 1: InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry1;
	/* ── Entry 2: InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_2 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry2;
	/* ── Entry 3: InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_3 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry3;
	/* ── Entry 4: InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_4 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry4;
	/* ── Entry 5: InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_5 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry5;
	/* ── Entry 6: InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_6 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry6;
	/* ── Entry 7: InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_7 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry7;
	/* ── Entry 8: InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_8 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry8;
	/* ── Entry 9: InterfaceDispatchSubjects_CalcAdd__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry9;
	/* ── Entry 10: InterfaceDispatchSubjects_CalcAdd_Add_System_Int32_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry10;
	/* ── Entry 11: InterfaceDispatchSubjects_CalcAdd_Multiply_System_Int32_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry11;
	/* ── Entry 12: InterfaceDispatchSubjects_CalcMul__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry12;
	/* ── Entry 13: InterfaceDispatchSubjects_CalcMul_Add_System_Int32_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry13;
	/* ── Entry 14: InterfaceDispatchSubjects_CalcMul_Multiply_System_Int32_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry14;
	/* ── Entry 15: InterfaceDispatchSubjects_ImplDiamond__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry15;
	/* ── Entry 16: InterfaceDispatchSubjects_ImplDiamond_BaseMethod ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry16;
	/* ── Entry 17: InterfaceDispatchSubjects_ImplDiamond_DerivedMethod ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry17;
	/* ── Entry 18: InterfaceDispatchSubjects_ImplDiamond2__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry18;
	/* ── Entry 19: InterfaceDispatchSubjects_ImplDiamond2_BaseMethod ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry19;
	/* ── Entry 20: InterfaceDispatchSubjects_ImplDiamond2_DerivedMethod ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry20;
	/* ── Entry 21: InterfaceDispatchSubjects_ImplSimple__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry21;
	/* ── Entry 22: InterfaceDispatchSubjects_ImplSimple_GetValue ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry22;
	/* ── Entry 23: InterfaceDispatchSubjects_ImplWithDefault__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry23;
	/* ── Entry 24: InterfaceDispatchSubjects_ImplWithDefault_GetValue ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry24;
	/* ── Entry 25: InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_Run_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[11];
	} entry25;
	/* ── Entry 26: InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestAsCheck ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry26;
	/* ── Entry 27: InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondBase ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry27;
	/* ── Entry 28: InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondDerived ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry28;
	/* ── Entry 29: InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondMulti ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry29;
	/* ── Entry 30: InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestIsCheck ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[12];
	} entry30;
	/* ── Entry 31: InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestMultiImplCalc ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry31;
	/* ── Entry 32: InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplDefault ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry32;
	/* ── Entry 33: InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplSimple ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry33;
	/* ── Entry 34: InterfaceDispatchSubjects_IWithDefault_GetValue ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry34;
} kChaosGcSlotMapsSection = {
	/* entry0 = InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_0 */
	.entry0 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_0),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry1 = InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_1 */
	.entry1 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_1),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry2 = InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_2 */
	.entry2 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_2),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry3 = InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_3 */
	.entry3 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_3),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry4 = InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_4 */
	.entry4 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_4),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry5 = InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_5 */
	.entry5 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_5),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry6 = InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_6 */
	.entry6 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_6),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry7 = InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_7 */
	.entry7 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_7),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry8 = InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_8 */
	.entry8 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_8),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry9 = InterfaceDispatchSubjects_CalcAdd__ctor */
	.entry9 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&InterfaceDispatchSubjects_CalcAdd__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry10 = InterfaceDispatchSubjects_CalcAdd_Add_System_Int32_System_Int32 */
	.entry10 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&InterfaceDispatchSubjects_CalcAdd_Add_System_Int32_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 24u, 32u }
	},
	/* entry11 = InterfaceDispatchSubjects_CalcAdd_Multiply_System_Int32_System_Int32 */
	.entry11 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&InterfaceDispatchSubjects_CalcAdd_Multiply_System_Int32_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 24u, 32u }
	},
	/* entry12 = InterfaceDispatchSubjects_CalcMul__ctor */
	.entry12 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&InterfaceDispatchSubjects_CalcMul__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry13 = InterfaceDispatchSubjects_CalcMul_Add_System_Int32_System_Int32 */
	.entry13 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&InterfaceDispatchSubjects_CalcMul_Add_System_Int32_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 24u, 32u }
	},
	/* entry14 = InterfaceDispatchSubjects_CalcMul_Multiply_System_Int32_System_Int32 */
	.entry14 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&InterfaceDispatchSubjects_CalcMul_Multiply_System_Int32_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 24u, 32u }
	},
	/* entry15 = InterfaceDispatchSubjects_ImplDiamond__ctor */
	.entry15 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&InterfaceDispatchSubjects_ImplDiamond__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry16 = InterfaceDispatchSubjects_ImplDiamond_BaseMethod */
	.entry16 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&InterfaceDispatchSubjects_ImplDiamond_BaseMethod),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry17 = InterfaceDispatchSubjects_ImplDiamond_DerivedMethod */
	.entry17 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&InterfaceDispatchSubjects_ImplDiamond_DerivedMethod),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry18 = InterfaceDispatchSubjects_ImplDiamond2__ctor */
	.entry18 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&InterfaceDispatchSubjects_ImplDiamond2__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry19 = InterfaceDispatchSubjects_ImplDiamond2_BaseMethod */
	.entry19 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&InterfaceDispatchSubjects_ImplDiamond2_BaseMethod),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry20 = InterfaceDispatchSubjects_ImplDiamond2_DerivedMethod */
	.entry20 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&InterfaceDispatchSubjects_ImplDiamond2_DerivedMethod),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry21 = InterfaceDispatchSubjects_ImplSimple__ctor */
	.entry21 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&InterfaceDispatchSubjects_ImplSimple__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry22 = InterfaceDispatchSubjects_ImplSimple_GetValue */
	.entry22 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&InterfaceDispatchSubjects_ImplSimple_GetValue),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry23 = InterfaceDispatchSubjects_ImplWithDefault__ctor */
	.entry23 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&InterfaceDispatchSubjects_ImplWithDefault__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry24 = InterfaceDispatchSubjects_ImplWithDefault_GetValue */
	.entry24 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&InterfaceDispatchSubjects_ImplWithDefault_GetValue),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry25 = InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_Run_System_Int32 */
	.entry25 = {
		/* entry_total_size = 64 */ 64u,
		/* code_address */ reinterpret_cast<const void*>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_Run_System_Int32),
		/* frame_size = 96 */ 96u,
		/* num_gc_slots = 11 */ 11u,
		/* slots */ { 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u, 72u, 80u, 88u }
	},
	/* entry26 = InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestAsCheck */
	.entry26 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestAsCheck),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry27 = InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondBase */
	.entry27 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondBase),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry28 = InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondDerived */
	.entry28 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondDerived),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry29 = InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondMulti */
	.entry29 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondMulti),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry30 = InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestIsCheck */
	.entry30 = {
		/* entry_total_size = 68 */ 68u,
		/* code_address */ reinterpret_cast<const void*>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestIsCheck),
		/* frame_size = 96 */ 96u,
		/* num_gc_slots = 12 */ 12u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u, 72u, 80u, 88u }
	},
	/* entry31 = InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestMultiImplCalc */
	.entry31 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestMultiImplCalc),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry32 = InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplDefault */
	.entry32 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplDefault),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry33 = InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplSimple */
	.entry33 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplSimple),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry34 = InterfaceDispatchSubjects_IWithDefault_GetValue */
	.entry34 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&InterfaceDispatchSubjects_IWithDefault_GetValue),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 1176u;

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[35] = {
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_0),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_1),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_2),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_3),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_4),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_5),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_6),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_7),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_8),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_CalcAdd__ctor),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_CalcAdd_Add_System_Int32_System_Int32),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_CalcAdd_Multiply_System_Int32_System_Int32),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_CalcMul__ctor),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_CalcMul_Add_System_Int32_System_Int32),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_CalcMul_Multiply_System_Int32_System_Int32),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplDiamond__ctor),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplDiamond_BaseMethod),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplDiamond_DerivedMethod),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplDiamond2__ctor),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplDiamond2_BaseMethod),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplDiamond2_DerivedMethod),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplSimple__ctor),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplSimple_GetValue),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplWithDefault__ctor),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_ImplWithDefault_GetValue),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_Run_System_Int32),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestAsCheck),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondBase),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondDerived),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondMulti),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestIsCheck),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestMultiImplCalc),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplDefault),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplSimple),
	reinterpret_cast<void*>(&InterfaceDispatchSubjects_IWithDefault_GetValue),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
// ── VTable descriptors (for BootstrapRuntime TypeVTable registration) ──
static const VTableDescriptorV0 kChaosVTableDescriptors[] = {
	{
		CHAOS_IL2CPP_UINT64_C(0x2838932695390A7C),
		0x00000009u,
		0u,
		3u,
		reinterpret_cast<const void*>(kSlots_InterfaceDispatchSubjects_CalcAdd),
		reinterpret_cast<const void**>(chaos_vtable_InterfaceDispatchSubjects_CalcAdd),
		3u,
		1,
		{0, 0, 0},
		chaos_iface_map_InterfaceDispatchSubjects_CalcAdd,
		1u,
	},
	{
		CHAOS_IL2CPP_UINT64_C(0xBF55B526598C6BA9),
		0x0000000Au,
		0u,
		3u,
		reinterpret_cast<const void*>(kSlots_InterfaceDispatchSubjects_CalcMul),
		reinterpret_cast<const void**>(chaos_vtable_InterfaceDispatchSubjects_CalcMul),
		3u,
		1,
		{0, 0, 0},
		chaos_iface_map_InterfaceDispatchSubjects_CalcMul,
		1u,
	},
	{
		CHAOS_IL2CPP_UINT64_C(0x50E6F8CE3066E29E),
		0x0000000Bu,
		0u,
		5u,
		reinterpret_cast<const void*>(kSlots_InterfaceDispatchSubjects_ImplDiamond),
		reinterpret_cast<const void**>(chaos_vtable_InterfaceDispatchSubjects_ImplDiamond),
		5u,
		1,
		{0, 0, 0},
		chaos_iface_map_InterfaceDispatchSubjects_ImplDiamond,
		2u,
	},
	{
		CHAOS_IL2CPP_UINT64_C(0xDF5B725C3ED32A44),
		0x0000000Cu,
		0u,
		5u,
		reinterpret_cast<const void*>(kSlots_InterfaceDispatchSubjects_ImplDiamond2),
		reinterpret_cast<const void**>(chaos_vtable_InterfaceDispatchSubjects_ImplDiamond2),
		5u,
		1,
		{0, 0, 0},
		chaos_iface_map_InterfaceDispatchSubjects_ImplDiamond2,
		2u,
	},
	{
		CHAOS_IL2CPP_UINT64_C(0xF08A1847F3E3A4E4),
		0x00000007u,
		0u,
		6u,
		reinterpret_cast<const void*>(kSlots_InterfaceDispatchSubjects_ImplSimple),
		reinterpret_cast<const void**>(chaos_vtable_InterfaceDispatchSubjects_ImplSimple),
		6u,
		1,
		{0, 0, 0},
		chaos_iface_map_InterfaceDispatchSubjects_ImplSimple,
		1u,
	},
	{
		CHAOS_IL2CPP_UINT64_C(0x8C9ACDA78A5BD967),
		0x00000008u,
		0u,
		6u,
		reinterpret_cast<const void*>(kSlots_InterfaceDispatchSubjects_ImplWithDefault),
		reinterpret_cast<const void**>(chaos_vtable_InterfaceDispatchSubjects_ImplWithDefault),
		6u,
		1,
		{0, 0, 0},
		chaos_iface_map_InterfaceDispatchSubjects_ImplWithDefault,
		1u,
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
	.vtable_descriptor_count = 6u,
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
	.image_name_utf8    = "InterfaceDispatchSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_InterfaceDispatchSubjects_InterfaceDispatchSubjects[1] = {
	{ 0x04000001u, "InterfaceDispatchSubjects/InterfaceDispatchSubjects::_exitCode", "_exitCode", "System.Int32", 0LL, 3u },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_InterfaceDispatchSubjects_InterfaceDispatchSubjects[9] = {
	{ 0x0000000Fu, "InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_0:System.Void()", "Subject_0", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000010u, "InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_1:System.Void()", "Subject_1", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000011u, "InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_2:System.Void()", "Subject_2", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000012u, "InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_3:System.Void()", "Subject_3", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000013u, "InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_4:System.Void()", "Subject_4", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000014u, "InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_5:System.Void()", "Subject_5", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000015u, "InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_6:System.Void()", "Subject_6", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000016u, "InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_7:System.Void()", "Subject_7", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000017u, "InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_8:System.Void()", "Subject_8", "System.Void", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_InterfaceDispatchSubjects_CalcAdd[3] = {
	{ 0x00000024u, "InterfaceDispatchSubjects/CalcAdd::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000022u, "InterfaceDispatchSubjects/CalcAdd::Add:System.Int32(System.Int32,System.Int32)", "Add", "System.Int32", 2, nullptr, 0u, nullptr, 0u },
	{ 0x00000023u, "InterfaceDispatchSubjects/CalcAdd::Multiply:System.Int32(System.Int32,System.Int32)", "Multiply", "System.Int32", 2, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_InterfaceDispatchSubjects_CalcMul[3] = {
	{ 0x00000027u, "InterfaceDispatchSubjects/CalcMul::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000025u, "InterfaceDispatchSubjects/CalcMul::Add:System.Int32(System.Int32,System.Int32)", "Add", "System.Int32", 2, nullptr, 0u, nullptr, 0u },
	{ 0x00000026u, "InterfaceDispatchSubjects/CalcMul::Multiply:System.Int32(System.Int32,System.Int32)", "Multiply", "System.Int32", 2, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_InterfaceDispatchSubjects_ImplDiamond[3] = {
	{ 0x0000002Au, "InterfaceDispatchSubjects/ImplDiamond::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000028u, "InterfaceDispatchSubjects/ImplDiamond::BaseMethod:System.Int32()", "BaseMethod", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000029u, "InterfaceDispatchSubjects/ImplDiamond::DerivedMethod:System.Int32()", "DerivedMethod", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_InterfaceDispatchSubjects_ImplDiamond2[3] = {
	{ 0x0000002Du, "InterfaceDispatchSubjects/ImplDiamond2::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000002Bu, "InterfaceDispatchSubjects/ImplDiamond2::BaseMethod:System.Int32()", "BaseMethod", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000002Cu, "InterfaceDispatchSubjects/ImplDiamond2::DerivedMethod:System.Int32()", "DerivedMethod", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_InterfaceDispatchSubjects_ImplSimple[2] = {
	{ 0x0000001Fu, "InterfaceDispatchSubjects/ImplSimple::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000001Eu, "InterfaceDispatchSubjects/ImplSimple::GetValue:System.Int32()", "GetValue", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_InterfaceDispatchSubjects_ImplWithDefault[2] = {
	{ 0x00000021u, "InterfaceDispatchSubjects/ImplWithDefault::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000020u, "InterfaceDispatchSubjects/ImplWithDefault::GetValue:System.Int32()", "GetValue", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_InterfaceDispatchSubjects_InterfaceDispatchNativeEntry[9] = {
	{ 0x0000002Eu, "InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::Run:System.Int32(System.Int32)", "Run", "System.Int32", 1, nullptr, 0u, nullptr, 0u },
	{ 0x00000033u, "InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestAsCheck:System.Int32()", "TestAsCheck", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000034u, "InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondBase:System.Int32()", "TestDiamondBase", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000035u, "InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondDerived:System.Int32()", "TestDiamondDerived", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000036u, "InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondMulti:System.Int32()", "TestDiamondMulti", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000032u, "InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestIsCheck:System.Int32()", "TestIsCheck", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000031u, "InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestMultiImplCalc:System.Int32()", "TestMultiImplCalc", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000030u, "InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestSingleImplDefault:System.Int32()", "TestSingleImplDefault", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000002Fu, "InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestSingleImplSimple:System.Int32()", "TestSingleImplSimple", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_InterfaceDispatchSubjects_IWithDefault[1] = {
	{ 0x00000019u, "InterfaceDispatchSubjects/IWithDefault::GetValue:System.Int32()", "GetValue", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[9] = {
	{ 0x00000001u, "InterfaceDispatchSubjects/InterfaceDispatchSubjects", "InterfaceDispatchSubjects/InterfaceDispatchSubjects", "", "InterfaceDispatchSubjects", "InterfaceDispatchSubjects", nullptr, kReflFields_InterfaceDispatchSubjects_InterfaceDispatchSubjects, 1u, nullptr, 0u,
nullptr, 0u,    kReflMethods_InterfaceDispatchSubjects_InterfaceDispatchSubjects, 9u, nullptr, 0u, 0u },
	{ 0x00000009u, "InterfaceDispatchSubjects/CalcAdd", "InterfaceDispatchSubjects/CalcAdd", "", "CalcAdd", "CalcAdd", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_InterfaceDispatchSubjects_CalcAdd, 3u, nullptr, 0u, 0u },
	{ 0x0000000Au, "InterfaceDispatchSubjects/CalcMul", "InterfaceDispatchSubjects/CalcMul", "", "CalcMul", "CalcMul", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_InterfaceDispatchSubjects_CalcMul, 3u, nullptr, 0u, 0u },
	{ 0x0000000Bu, "InterfaceDispatchSubjects/ImplDiamond", "InterfaceDispatchSubjects/ImplDiamond", "", "ImplDiamond", "ImplDiamond", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_InterfaceDispatchSubjects_ImplDiamond, 3u, nullptr, 0u, 0u },
	{ 0x0000000Cu, "InterfaceDispatchSubjects/ImplDiamond2", "InterfaceDispatchSubjects/ImplDiamond2", "", "ImplDiamond2", "ImplDiamond2", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_InterfaceDispatchSubjects_ImplDiamond2, 3u, nullptr, 0u, 0u },
	{ 0x00000007u, "InterfaceDispatchSubjects/ImplSimple", "InterfaceDispatchSubjects/ImplSimple", "", "ImplSimple", "ImplSimple", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_InterfaceDispatchSubjects_ImplSimple, 2u, nullptr, 0u, 0u },
	{ 0x00000008u, "InterfaceDispatchSubjects/ImplWithDefault", "InterfaceDispatchSubjects/ImplWithDefault", "", "ImplWithDefault", "ImplWithDefault", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_InterfaceDispatchSubjects_ImplWithDefault, 2u, nullptr, 0u, 0u },
	{ 0x0000000Du, "InterfaceDispatchSubjects/InterfaceDispatchNativeEntry", "InterfaceDispatchSubjects/InterfaceDispatchNativeEntry", "", "InterfaceDispatchNativeEntry", "InterfaceDispatchNativeEntry", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_InterfaceDispatchSubjects_InterfaceDispatchNativeEntry, 9u, nullptr, 0u, 0u },
	{ 0x00000003u, "InterfaceDispatchSubjects/IWithDefault", "InterfaceDispatchSubjects/IWithDefault", "", "IWithDefault", "IWithDefault", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_InterfaceDispatchSubjects_IWithDefault, 1u, nullptr, 0u, 0u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[9] = {
	&kReflTypes[0],
	&kReflTypes[1],
	&kReflTypes[2],
	&kReflTypes[3],
	&kReflTypes[4],
	&kReflTypes[5],
	&kReflTypes[6],
	&kReflTypes[7],
	&kReflTypes[8],
};

static const ReflectionQueryImageDescriptor kReflImage = { "InterfaceDispatchSubjects", kReflTypePtrs, 9u, 1, 0, 0, 0 };

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
// Managed method: InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_0()
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_0(void)
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


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_0 = _s0;
				auto& _d25 = s_hotpatch_entries[25];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d25)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d25))
				{
					alignas(16) uint8_t _d_ab[4];
					ArgBuffer _d_bw(_d_ab);
					_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d25.method_key, _d_ab, &_d_hpresult);
				}
				else
				{
					_d_hpresult = InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_Run_System_Int32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_0 = _s1;
				auto& _d25 = s_hotpatch_entries[25];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d25)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d25))
				{
					alignas(16) uint8_t _d_ab[4];
					ArgBuffer _d_bw(_d_ab);
					_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d25.method_key, _d_ab, &_d_hpresult);
				}
				else
				{
					_d_hpresult = InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_Run_System_Int32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
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
				chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), reinterpret_cast<const TypeInfoHot*>(&chaos_mt_System_Private_CoreLib_System_Object)))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_1()
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_1(void)
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
				auto& _d33 = s_hotpatch_entries[33];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d33)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d33))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d33.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplSimple();
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
			{
				auto& _d33 = s_hotpatch_entries[33];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d33)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d33))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d33.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplSimple();
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
				chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), reinterpret_cast<const TypeInfoHot*>(&chaos_mt_System_Private_CoreLib_System_Object)))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_2()
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_2(void)
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
					_d_hpresult = InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplDefault();
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
					_d_hpresult = InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplDefault();
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
				chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), reinterpret_cast<const TypeInfoHot*>(&chaos_mt_System_Private_CoreLib_System_Object)))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_3()
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_3(void)
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
				auto& _d31 = s_hotpatch_entries[31];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d31)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d31))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d31.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestMultiImplCalc();
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
			{
				auto& _d31 = s_hotpatch_entries[31];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d31)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d31))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d31.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestMultiImplCalc();
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
				chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), reinterpret_cast<const TypeInfoHot*>(&chaos_mt_System_Private_CoreLib_System_Object)))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_4()
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_4(void)
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
				auto& _d30 = s_hotpatch_entries[30];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d30)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d30))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d30.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestIsCheck();
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
			{
				auto& _d30 = s_hotpatch_entries[30];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d30)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d30))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d30.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestIsCheck();
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
				chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), reinterpret_cast<const TypeInfoHot*>(&chaos_mt_System_Private_CoreLib_System_Object)))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_5()
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_5(void)
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
				auto& _d26 = s_hotpatch_entries[26];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d26)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d26))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d26.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestAsCheck();
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
			{
				auto& _d26 = s_hotpatch_entries[26];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d26)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d26))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d26.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestAsCheck();
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
				chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), reinterpret_cast<const TypeInfoHot*>(&chaos_mt_System_Private_CoreLib_System_Object)))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_6()
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_6(void)
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
					_d_hpresult = InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondBase();
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
					_d_hpresult = InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondBase();
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
				chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), reinterpret_cast<const TypeInfoHot*>(&chaos_mt_System_Private_CoreLib_System_Object)))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_7()
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_7(void)
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
				auto& _d28 = s_hotpatch_entries[28];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d28)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d28))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d28.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondDerived();
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
			{
				auto& _d28 = s_hotpatch_entries[28];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d28)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d28))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d28.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondDerived();
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
				chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), reinterpret_cast<const TypeInfoHot*>(&chaos_mt_System_Private_CoreLib_System_Object)))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: InterfaceDispatchSubjects/InterfaceDispatchSubjects::Subject_8()
extern "C" void InterfaceDispatchSubjects_InterfaceDispatchSubjects_Subject_8(void)
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
				auto& _d29 = s_hotpatch_entries[29];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d29)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d29))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d29.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondMulti();
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
			{
				auto& _d29 = s_hotpatch_entries[29];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d29)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d29))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d29.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondMulti();
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
				chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), reinterpret_cast<const TypeInfoHot*>(&chaos_mt_System_Private_CoreLib_System_Object)))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_InterfaceDispatchSubjects_InterfaceDispatchSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: InterfaceDispatchSubjects/CalcAdd::.ctor()
extern "C" void InterfaceDispatchSubjects_CalcAdd__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: InterfaceDispatchSubjects/CalcAdd::Add(System.Int32,System.Int32)
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_CalcAdd_Add_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2)
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

// Managed method: InterfaceDispatchSubjects/CalcAdd::Multiply(System.Int32,System.Int32)
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_CalcAdd_Multiply_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2)
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
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapMul(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: InterfaceDispatchSubjects/CalcMul::.ctor()
extern "C" void InterfaceDispatchSubjects_CalcMul__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: InterfaceDispatchSubjects/CalcMul::Add(System.Int32,System.Int32)
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_CalcMul_Add_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);
	chaos_args[2] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_2);

	_s0 = chaos_args[1];
	_s1 = chaos_args[2];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: InterfaceDispatchSubjects/CalcMul::Multiply(System.Int32,System.Int32)
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_CalcMul_Multiply_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);
	chaos_args[2] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_2);

	_s0 = chaos_args[1];
	_s1 = chaos_args[2];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapMul(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapMul(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: InterfaceDispatchSubjects/ImplDiamond::.ctor()
extern "C" void InterfaceDispatchSubjects_ImplDiamond__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: InterfaceDispatchSubjects/ImplDiamond::BaseMethod()
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplDiamond_BaseMethod(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(100);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: InterfaceDispatchSubjects/ImplDiamond::DerivedMethod()
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplDiamond_DerivedMethod(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(200);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: InterfaceDispatchSubjects/ImplDiamond2::.ctor()
extern "C" void InterfaceDispatchSubjects_ImplDiamond2__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: InterfaceDispatchSubjects/ImplDiamond2::BaseMethod()
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplDiamond2_BaseMethod(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(300);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: InterfaceDispatchSubjects/ImplDiamond2::DerivedMethod()
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplDiamond2_DerivedMethod(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(400);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: InterfaceDispatchSubjects/ImplSimple::.ctor()
extern "C" void InterfaceDispatchSubjects_ImplSimple__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: InterfaceDispatchSubjects/ImplSimple::GetValue()
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplSimple_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: InterfaceDispatchSubjects/ImplWithDefault::.ctor()
extern "C" void InterfaceDispatchSubjects_ImplWithDefault__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: InterfaceDispatchSubjects/ImplWithDefault::GetValue()
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_ImplWithDefault_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::Run(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_Run_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
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
	CHAOS_IL2CPP_INTPTR _s36{};
	CHAOS_IL2CPP_INTPTR _s37{};
	CHAOS_IL2CPP_INTPTR _s38{};
	CHAOS_IL2CPP_INTPTR _s39{};
	CHAOS_IL2CPP_INTPTR _s40{};
	CHAOS_IL2CPP_INTPTR _s41{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 == 0)
		{
			_s2 = chaos_args[0];
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			{
				if (_s2 == 0)
				{
					_s2 = chaos_args[0];
					_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
					_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
					{
						if (_s2 == 0)
						{
							_s2 = chaos_args[0];
							_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
							_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
							{
								if (_s2 == 0)
								{
									_s2 = chaos_args[0];
									_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
									_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
									{
										if (_s2 == 0)
										{
											_s2 = chaos_args[0];
											_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
											_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
											{
												if (_s2 == 0)
												{
													_s2 = chaos_args[0];
													_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(6);
													_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
													{
														if (_s2 == 0)
														{
															_s2 = chaos_args[0];
															_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(7);
															_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
															{
																if (_s2 == 0)
																{
																	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(-1);
																	chaos_locals[1] = _s2;
																}
																else
																{
																	{
																		auto& _d29 = s_hotpatch_entries[29];
																		CHAOS_IL2CPP_INT32 _d_hpresult{};
																		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d29)
																			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d29))
																		{
																			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
																				_d29.method_key, nullptr, &_d_hpresult);
																		}
																		else
																		{
																			_d_hpresult = InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondMulti();
																		}
																		_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
																	}
																	chaos_locals[1] = _s4;
																}
															}
														}
														else
														{
															{
																auto& _d28 = s_hotpatch_entries[28];
																CHAOS_IL2CPP_INT32 _d_hpresult{};
																if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d28)
																	&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d28))
																{
																	::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
																		_d28.method_key, nullptr, &_d_hpresult);
																}
																else
																{
																	_d_hpresult = InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondDerived();
																}
																_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
															}
															chaos_locals[1] = _s4;
														}
													}
												}
												else
												{
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
															_d_hpresult = InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondBase();
														}
														_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
													}
													chaos_locals[1] = _s4;
												}
											}
										}
										else
										{
											{
												auto& _d26 = s_hotpatch_entries[26];
												CHAOS_IL2CPP_INT32 _d_hpresult{};
												if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d26)
													&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d26))
												{
													::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
														_d26.method_key, nullptr, &_d_hpresult);
												}
												else
												{
													_d_hpresult = InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestAsCheck();
												}
												_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
											}
											chaos_locals[1] = _s4;
										}
									}
								}
								else
								{
									{
										auto& _d30 = s_hotpatch_entries[30];
										CHAOS_IL2CPP_INT32 _d_hpresult{};
										if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d30)
											&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d30))
										{
											::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
												_d30.method_key, nullptr, &_d_hpresult);
										}
										else
										{
											_d_hpresult = InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestIsCheck();
										}
										_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
									}
									chaos_locals[1] = _s4;
								}
							}
						}
						else
						{
							{
								auto& _d31 = s_hotpatch_entries[31];
								CHAOS_IL2CPP_INT32 _d_hpresult{};
								if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d31)
									&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d31))
								{
									::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
										_d31.method_key, nullptr, &_d_hpresult);
								}
								else
								{
									_d_hpresult = InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestMultiImplCalc();
								}
								_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
							}
							chaos_locals[1] = _s4;
						}
					}
				}
				else
				{
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
							_d_hpresult = InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplDefault();
						}
						_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
					}
					chaos_locals[1] = _s4;
				}
			}
		}
		else
		{
			{
				auto& _d33 = s_hotpatch_entries[33];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d33)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d33))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d33.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplSimple();
				}
				_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
			chaos_locals[1] = _s2;
		}
		_s2 = chaos_locals[1];
		return static_cast<CHAOS_IL2CPP_INT32>(_s2);
	}
}

// Managed method: InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestAsCheck()
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestAsCheck(void)
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


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_InterfaceDispatchSubjects_ImplSimple, {});
		chaos_object->header.type_info = reinterpret_cast<const TypeInfoHot*>(&chaos_mt_InterfaceDispatchSubjects_ImplSimple);
		InterfaceDispatchSubjects_ImplSimple__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	{
		const auto chaos_value = _s0;
		auto chaos_matches = false;
		if (chaos_value != 0)
		{
			auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_value);
			chaos_matches = chaos_does_type_implement_interface(chaos_object_get_type_info(chaos_header), reinterpret_cast<const TypeInfoHot*>(&chaos_mt_InterfaceDispatchSubjects_ISimple));
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
				const auto chaos_raw_arg_0 = _s3;
				const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				CHAOS_IL2CPP_INT32 chaos_callvirt_result{};
				chaos_callvirt_result = (*reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)>(chaos_vtable_resolve(chaos_object_get_type_info(reinterpret_cast<void*>(chaos_arg_0))->vtable_array, 5u)))(chaos_arg_0);
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_callvirt_result);
			}
			chaos_locals[3] = _s3;
		}
		_s3 = chaos_locals[3];
		return static_cast<CHAOS_IL2CPP_INT32>(_s3);
	}
}

// Managed method: InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondBase()
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondBase(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_InterfaceDispatchSubjects_ImplDiamond, {});
		chaos_object->header.type_info = reinterpret_cast<const TypeInfoHot*>(&chaos_mt_InterfaceDispatchSubjects_ImplDiamond);
		InterfaceDispatchSubjects_ImplDiamond__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
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

// Managed method: InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondDerived()
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondDerived(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_InterfaceDispatchSubjects_ImplDiamond, {});
		chaos_object->header.type_info = reinterpret_cast<const TypeInfoHot*>(&chaos_mt_InterfaceDispatchSubjects_ImplDiamond);
		InterfaceDispatchSubjects_ImplDiamond__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
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
		chaos_callvirt_result = (*reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)>(chaos_vtable_resolve(chaos_object_get_type_info(reinterpret_cast<void*>(chaos_arg_0))->vtable_array, 4u)))(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_callvirt_result);
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestDiamondMulti()
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestDiamondMulti(void)
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
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_InterfaceDispatchSubjects_ImplDiamond, {});
		chaos_object->header.type_info = reinterpret_cast<const TypeInfoHot*>(&chaos_mt_InterfaceDispatchSubjects_ImplDiamond);
		InterfaceDispatchSubjects_ImplDiamond__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[0] = _s0;
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_InterfaceDispatchSubjects_ImplDiamond2, {});
		chaos_object->header.type_info = reinterpret_cast<const TypeInfoHot*>(&chaos_mt_InterfaceDispatchSubjects_ImplDiamond2);
		InterfaceDispatchSubjects_ImplDiamond2__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[1] = _s0;
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
	_s1 = chaos_locals[0];
	{
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		CHAOS_IL2CPP_INT32 chaos_callvirt_result{};
		chaos_callvirt_result = (*reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)>(chaos_vtable_resolve(chaos_object_get_type_info(reinterpret_cast<void*>(chaos_arg_0))->vtable_array, 4u)))(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_callvirt_result);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	_s1 = chaos_locals[1];
	{
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		CHAOS_IL2CPP_INT32 chaos_callvirt_result{};
		chaos_callvirt_result = (*reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)>(chaos_vtable_resolve(chaos_object_get_type_info(reinterpret_cast<void*>(chaos_arg_0))->vtable_array, 3u)))(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_callvirt_result);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	_s1 = chaos_locals[1];
	{
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		CHAOS_IL2CPP_INT32 chaos_callvirt_result{};
		chaos_callvirt_result = (*reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)>(chaos_vtable_resolve(chaos_object_get_type_info(reinterpret_cast<void*>(chaos_arg_0))->vtable_array, 4u)))(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_callvirt_result);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	chaos_locals[2] = _s0;
	_s0 = chaos_locals[2];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestIsCheck()
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestIsCheck(void)
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


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_InterfaceDispatchSubjects_ImplSimple, {});
		chaos_object->header.type_info = reinterpret_cast<const TypeInfoHot*>(&chaos_mt_InterfaceDispatchSubjects_ImplSimple);
		InterfaceDispatchSubjects_ImplSimple__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[0] = _s0;
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_InterfaceDispatchSubjects_CalcAdd, {});
		chaos_object->header.type_info = reinterpret_cast<const TypeInfoHot*>(&chaos_mt_InterfaceDispatchSubjects_CalcAdd);
		InterfaceDispatchSubjects_CalcAdd__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[1] = _s0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[2] = _s0;
	_s0 = chaos_locals[0];
	{
		const auto chaos_value = _s0;
		auto chaos_matches = false;
		if (chaos_value != 0)
		{
			auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_value);
			chaos_matches = chaos_does_type_implement_interface(chaos_object_get_type_info(chaos_header), reinterpret_cast<const TypeInfoHot*>(&chaos_mt_InterfaceDispatchSubjects_ISimple));
		}
		_s0 = chaos_matches ? chaos_value : 0;
	}
	_s1 = 0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s0) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
		{
			_s0 = chaos_locals[2];
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(10);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
			chaos_locals[2] = _s0;
		}
		_s3 = chaos_locals[1];
		{
			const auto chaos_value = _s3;
			auto chaos_matches = false;
			if (chaos_value != 0)
			{
				auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_value);
				chaos_matches = chaos_does_type_implement_interface(chaos_object_get_type_info(chaos_header), reinterpret_cast<const TypeInfoHot*>(&chaos_mt_InterfaceDispatchSubjects_ISimple));
			}
			_s3 = chaos_matches ? chaos_value : 0;
		}
		_s4 = 0;
		_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s3) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s4) ? 1 : 0);
		{
			if (_s3 != 0)
			{
				_s3 = chaos_locals[2];
				_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(100);
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s3), static_cast<CHAOS_IL2CPP_INT32>(_s4)));
				chaos_locals[2] = _s3;
			}
			_s3 = chaos_locals[1];
			{
				const auto chaos_value = _s3;
				auto chaos_matches = false;
				if (chaos_value != 0)
				{
					auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_value);
					chaos_matches = chaos_does_type_implement_interface(chaos_object_get_type_info(chaos_header), reinterpret_cast<const TypeInfoHot*>(&chaos_mt_InterfaceDispatchSubjects_ICalculator));
				}
				_s3 = chaos_matches ? chaos_value : 0;
			}
			_s4 = 0;
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s3) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s4) ? 1 : 0);
			{
				if (_s3 != 0)
				{
					_s3 = chaos_locals[2];
					_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(1000);
					_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s3), static_cast<CHAOS_IL2CPP_INT32>(_s4)));
					chaos_locals[2] = _s3;
				}
				_s6 = chaos_locals[2];
				chaos_locals[6] = _s6;
				_s6 = chaos_locals[6];
				return static_cast<CHAOS_IL2CPP_INT32>(_s6);
			}
		}
	}
}

// Managed method: InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestMultiImplCalc()
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestMultiImplCalc(void)
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


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_InterfaceDispatchSubjects_CalcAdd, {});
		chaos_object->header.type_info = reinterpret_cast<const TypeInfoHot*>(&chaos_mt_InterfaceDispatchSubjects_CalcAdd);
		InterfaceDispatchSubjects_CalcAdd__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[0] = _s0;
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_InterfaceDispatchSubjects_CalcMul, {});
		chaos_object->header.type_info = reinterpret_cast<const TypeInfoHot*>(&chaos_mt_InterfaceDispatchSubjects_CalcMul);
		InterfaceDispatchSubjects_CalcMul__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(10);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
	{
		const auto chaos_raw_arg_2 = _s2;
		const auto chaos_arg_2 = chaos_raw_arg_2;
		const auto chaos_raw_arg_1 = _s1;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		CHAOS_IL2CPP_INT32 chaos_callvirt_result{};
		chaos_callvirt_result = (*reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2)>(chaos_vtable_resolve(chaos_object_get_type_info(reinterpret_cast<void*>(chaos_arg_0))->vtable_array, 1u)))(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2));
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_callvirt_result);
	}
	_s1 = chaos_locals[1];
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
	{
		const auto chaos_raw_arg_2 = _s3;
		const auto chaos_arg_2 = chaos_raw_arg_2;
		const auto chaos_raw_arg_1 = _s2;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		CHAOS_IL2CPP_INT32 chaos_callvirt_result{};
		chaos_callvirt_result = (*reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2)>(chaos_vtable_resolve(chaos_object_get_type_info(reinterpret_cast<void*>(chaos_arg_0))->vtable_array, 2u)))(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2));
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_callvirt_result);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	chaos_locals[2] = _s0;
	_s0 = chaos_locals[2];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestSingleImplDefault()
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplDefault(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_InterfaceDispatchSubjects_ImplWithDefault, {});
		chaos_object->header.type_info = reinterpret_cast<const TypeInfoHot*>(&chaos_mt_InterfaceDispatchSubjects_ImplWithDefault);
		InterfaceDispatchSubjects_ImplWithDefault__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
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
		chaos_callvirt_result = (*reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)>(chaos_vtable_resolve(chaos_object_get_type_info(reinterpret_cast<void*>(chaos_arg_0))->vtable_array, 5u)))(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_callvirt_result);
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: InterfaceDispatchSubjects/InterfaceDispatchNativeEntry::TestSingleImplSimple()
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_InterfaceDispatchNativeEntry_TestSingleImplSimple(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_InterfaceDispatchSubjects_ImplSimple, {});
		chaos_object->header.type_info = reinterpret_cast<const TypeInfoHot*>(&chaos_mt_InterfaceDispatchSubjects_ImplSimple);
		InterfaceDispatchSubjects_ImplSimple__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
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
		chaos_callvirt_result = (*reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)>(chaos_vtable_resolve(chaos_object_get_type_info(reinterpret_cast<void*>(chaos_arg_0))->vtable_array, 5u)))(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_callvirt_result);
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: InterfaceDispatchSubjects/IWithDefault::GetValue()
extern "C" CHAOS_IL2CPP_INT32 InterfaceDispatchSubjects_IWithDefault_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}



}  // namespace chaos::il2cpp::codegen::InterfaceDispatchSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 35;

extern "C" void ChaosJitRegisterAll() {}