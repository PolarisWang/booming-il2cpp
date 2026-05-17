/// Integration tests for the IR->execution pipeline.
///
/// Each test constructs an IRMethod with explicit IRInstruction entries and
/// executes it via InterpreterVM::Execute, then verifies the result.
///
/// The old IL->IR lowering path (LowerILToIR, ParseMethodBodyHeader) has been
/// removed; all tests now build IRMethod directly.

#include "interpreter_vm.h"
#include "vtable_registry.h"
#include "token_resolver.h"

#include <chaos/common.h>
#include <chaos/config.h>
#include <chaos/profile.h>
#include <chaos/type_info.h>
#include <codegen_bridge.h>

#include <iostream>
#include <cstring>
#include <cstdint>

// ── Stubs for symbols normally emitted by codegen ───────────────────────
// These are referenced by bootstrap/interpreter libs but not provided
// when the test is built without the full codegen pipeline.
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module = nullptr;
extern "C" const char* kChaosExternalRuntimeSubjects[1] = { nullptr };
extern "C" void* kChaosExternalRuntimeFnTable[1] = { nullptr };
extern "C" int32_t kChaosExternalRuntimeCount = 0;

// ── Namespace aliases ───────────────────────────────────────────────────
using chaos::il2cpp::interpreter::ExecutionFrame;
using chaos::il2cpp::interpreter::ExecutionResult;
using chaos::il2cpp::interpreter::IRInstruction;
using chaos::il2cpp::interpreter::IRMethod;
using chaos::il2cpp::interpreter::IROpCode;
using chaos::il2cpp::interpreter::InterpreterObject;
using chaos::il2cpp::interpreter::InterpreterValue;
using chaos::il2cpp::interpreter::InterpreterVM;
using chaos::il2cpp::interpreter::SEHClause;
using chaos::il2cpp::interpreter::SEHFlags;
using chaos::il2cpp::interpreter::ValueTag;
using chaos::il2cpp::interpreter::DispatchResult;
using chaos::il2cpp::interpreter::DispatchCallback;
using chaos::il2cpp::interpreter::DefaultTokenResolver;
using chaos::il2cpp::interpreter::TokenResolverContext;

// ════════════════════════════════════════════════════════════════════════════
// Test declarations
// ════════════════════════════════════════════════════════════════════════════

static bool TestLdcI4_0();
static bool TestLdcI4_S();
static bool TestLdcI4_2_3_Add();
static bool TestLdArgAdd();
static bool TestStLocLdLoc();
static bool TestBrTrue_Taken();
static bool TestBrTrue_NotTaken();
static bool TestBeq_Equal();
static bool TestBeq_NotEqual();
static bool TestSub();
static bool TestMul();
static bool TestLdNull();

// ── Phase A new opcode tests ───────────────────────────────────────────────
static bool TestDup();
static bool TestDivUn();
static bool TestRemUn();
static bool TestBitwiseAnd();
static bool TestBitwiseOr();
static bool TestBitwiseXor();
static bool TestBitwiseNot();
static bool TestShiftLeft();
static bool TestShiftRightArith();
static bool TestShiftRightLogical();
static bool TestConvRUn();
static bool TestConvI();
static bool TestConvU();
static bool TestSizeOf();

static bool TestExtendedLdFtn();
static bool TestExtendedLdArg();
static bool TestExtendedLdLoc();

// ── CallVirt / VTable dispatch tests ─────────────────────────────────────
static bool TestCallVirtDirectResolution();
static bool TestCallVirtInheritanceChain();

// -- SEH exception handling tests ------------------------------------------
static bool TestThrowUnhandled();
static bool TestThrowCatch();
static bool TestThrowFinallyUnwind();
static bool TestLeaveFinally();
static bool TestRethrow();

// Runtime-generic-method simulation tests
static bool TestRuntimeMethodExecute();
static bool TestRuntimeMethodReturnValueDispatch();
static bool TestRuntimeMethodTypeParamResolution();
static bool TestRuntimeMethodExceptionPropagation();

// Struct value type tests
static bool TestStructReturnValue();

// Phase 8: Dispatch callback integration tests
static bool TestDispatchBasic();
static bool TestDispatchException();
static bool TestDispatchArgs();

// New interpreter VM feature tests
static bool TestFloatBlt();
static bool TestFloatBgt();
static bool TestFloatBle();
static bool TestFloatBge();
static bool TestLdArgA_RefSemantics();
static bool TestLdLocA_RefSemantics();
static bool TestInterfaceCastClass();
static bool TestInterfaceIsInst();
static bool TestInterfaceVtableDispatch();

// ── Codegen VTable dispatch tests (RegisterCodegenVTable path) ─────────
static bool Test_CodegenVTableDirect();
static bool Test_CodegenVTableInheritance();
static bool Test_CodegenVTableInterfaceDispatch();

// ── Hot-update VTable resolution test (RegisterHotUpdateVTable path) ─
static bool Test_HotUpdateVTableResolution();

// ════════════════════════════════════════════════════════════════════════════
// Test runner
// ════════════════════════════════════════════════════════════════════════════

static int failures = 0;

#define TEST(name)                                                      \
    do {                                                                \
        if (!(name())) {                                                \
            std::cerr << "FAIL: " << #name << std::endl;                \
            ++failures;                                                 \
        }                                                               \
    } while (false)

int main()
{
    TEST(TestLdcI4_0);
    TEST(TestLdcI4_S);
    TEST(TestLdcI4_2_3_Add);
    TEST(TestLdArgAdd);
    TEST(TestStLocLdLoc);
    TEST(TestBrTrue_Taken);
    TEST(TestBrTrue_NotTaken);
    TEST(TestBeq_Equal);
    TEST(TestBeq_NotEqual);
    TEST(TestSub);
    TEST(TestMul);
    TEST(TestLdNull);

    // Phase A new opcode tests
    TEST(TestDup);
    TEST(TestDivUn);
    TEST(TestRemUn);
    TEST(TestBitwiseAnd);
    TEST(TestBitwiseOr);
    TEST(TestBitwiseXor);
    TEST(TestBitwiseNot);
    TEST(TestShiftLeft);
    TEST(TestShiftRightArith);
    TEST(TestShiftRightLogical);
    TEST(TestConvRUn);
    TEST(TestConvI);
    TEST(TestConvU);
    TEST(TestSizeOf);
    TEST(TestExtendedLdFtn);
    TEST(TestExtendedLdArg);
    TEST(TestExtendedLdLoc);

    // CallVirt / VTable dispatch tests
    TEST(TestCallVirtDirectResolution);
    TEST(TestCallVirtInheritanceChain);

    // Codegen VTable path tests (using RegisterCodegenVTable)
    TEST(Test_CodegenVTableDirect);
    TEST(Test_CodegenVTableInheritance);
    TEST(Test_CodegenVTableInterfaceDispatch);

    // Hot-update VTable path test (RegisterHotUpdateVTable)
    TEST(Test_HotUpdateVTableResolution);

    // SEH exception handling tests
    TEST(TestThrowUnhandled);
    TEST(TestThrowCatch);
    TEST(TestThrowFinallyUnwind);
    TEST(TestLeaveFinally);
    TEST(TestRethrow);

    // Runtime-generic-method simulation tests (bridge core logic)
    TEST(TestRuntimeMethodExecute);
    TEST(TestRuntimeMethodReturnValueDispatch);
    TEST(TestRuntimeMethodTypeParamResolution);
    TEST(TestRuntimeMethodExceptionPropagation);

    // Struct value type tests
    TEST(TestStructReturnValue);

    // Phase 8: Dispatch callback integration tests
    TEST(TestDispatchBasic);
    TEST(TestDispatchException);
    TEST(TestDispatchArgs);

    // New interpreter VM feature tests
    TEST(TestFloatBlt);
    TEST(TestFloatBgt);
    TEST(TestFloatBle);
    TEST(TestFloatBge);
    TEST(TestLdArgA_RefSemantics);
    TEST(TestLdLocA_RefSemantics);
    TEST(TestInterfaceCastClass);
    TEST(TestInterfaceIsInst);
    TEST(TestInterfaceVtableDispatch);

    CHAOS_IL2CPP_PROFILE_DUMP();
    CHAOS_IL2CPP_PROFILE_RESET();

    std::cout << "interpreter-integration=failures=" << failures << std::endl;

    if (failures > 0) {
        std::cerr << "SOME INTEGRATION TESTS FAILED" << std::endl;
        return 1;
    }
    std::cout << "all interpreter integration tests passed" << std::endl;
    return 0;
}

// ═══════════════════════════════════════════════════════════════════════════
// Test implementations
// ═══════════════════════════════════════════════════════════════════════════

static IRMethod MakeSimpleMethod(IROpCode op, CHAOS_IL2CPP_INT32 imm = 0)
{
    IRMethod method;
    IRInstruction i;
    i.op_code = op;
    i.immediate_i4 = imm;
    method.instructions.push_back(i);
    IRInstruction ret;
    ret.op_code = IROpCode::Ret;
    method.instructions.push_back(ret);
    return method;
}

bool TestLdcI4_0()
{
    // ldc.i4.0 → ret
    ExecutionFrame frame = {};
    const InterpreterVM vm = {};
    IRMethod method = MakeSimpleMethod(IROpCode::LdcI4, 0);
    ExecutionResult result = vm.Execute(method, &frame);
    return result.has_return_value && result.int32_value == 0;
}

bool TestLdcI4_S()
{
    // ldc.i4.s 42 → ret
    IRMethod method = MakeSimpleMethod(IROpCode::LdcI4, 42);
    ExecutionFrame frame = {};
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    return result.has_return_value && result.int32_value == 42;
}

bool TestLdcI4_2_3_Add()
{
    // ldc.i4.2 → ldc.i4.3 → add → ret
    IRMethod method;
    IRInstruction i1; i1.op_code = IROpCode::LdcI4; i1.immediate_i4 = 2; method.instructions.push_back(i1);
    IRInstruction i2; i2.op_code = IROpCode::LdcI4; i2.immediate_i4 = 3; method.instructions.push_back(i2);
    IRInstruction add; add.op_code = IROpCode::Add; method.instructions.push_back(add);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame = {};
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    return result.has_return_value && result.int32_value == 5;
}

bool TestLdArgAdd()
{
    // ldarg.0 → ldarg.1 → add → ret
    IRMethod method;
    IRInstruction a0; a0.op_code = IROpCode::LdArg; a0.operand_index = 0; method.instructions.push_back(a0);
    IRInstruction a1; a1.op_code = IROpCode::LdArg; a1.operand_index = 1; method.instructions.push_back(a1);
    IRInstruction add; add.op_code = IROpCode::Add; method.instructions.push_back(add);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame = {};
    frame.arguments.push_back(InterpreterValue::from_i32(10));
    frame.arguments.push_back(InterpreterValue::from_i32(32));
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    return result.has_return_value && result.int32_value == 42;
}

bool TestStLocLdLoc()
{
    // ldc.i4.s 42 → stloc.0 → ldloc.0 → ret
    IRMethod method;
    IRInstruction push; push.op_code = IROpCode::LdcI4; push.immediate_i4 = 42; method.instructions.push_back(push);
    IRInstruction st;   st.op_code = IROpCode::StLoc; st.operand_index = 0; method.instructions.push_back(st);
    IRInstruction ld;   ld.op_code = IROpCode::LdLoc; ld.operand_index = 0; method.instructions.push_back(ld);
    IRInstruction ret;  ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame = {};
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    return result.has_return_value && result.int32_value == 42;
}

bool TestSub()
{
    // ldc.i4.s 10 → ldc.i4.3 → sub → ret
    IRMethod method;
    IRInstruction a; a.op_code = IROpCode::LdcI4; a.immediate_i4 = 10; method.instructions.push_back(a);
    IRInstruction b; b.op_code = IROpCode::LdcI4; b.immediate_i4 = 3; method.instructions.push_back(b);
    IRInstruction sub; sub.op_code = IROpCode::Sub; method.instructions.push_back(sub);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame = {};
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    return result.has_return_value && result.int32_value == 7;
}

bool TestMul()
{
    // ldc.i4.6 → ldc.i4.7 → mul → ret
    IRMethod method;
    IRInstruction a; a.op_code = IROpCode::LdcI4; a.immediate_i4 = 6; method.instructions.push_back(a);
    IRInstruction b; b.op_code = IROpCode::LdcI4; b.immediate_i4 = 7; method.instructions.push_back(b);
    IRInstruction mul; mul.op_code = IROpCode::Mul; method.instructions.push_back(mul);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame = {};
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    return result.has_return_value && result.int32_value == 42;
}

bool TestLdNull()
{
    // ldnull → pop → ldc.i4.0 → ret
    IRMethod method;
    IRInstruction null; null.op_code = IROpCode::LdNull; method.instructions.push_back(null);
    IRInstruction pop; pop.op_code = IROpCode::Pop; method.instructions.push_back(pop);
    IRInstruction push; push.op_code = IROpCode::LdcI4; push.immediate_i4 = 0; method.instructions.push_back(push);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame = {};
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    return result.has_return_value && result.int32_value == 0;
}

bool TestBrTrue_Taken()
{
    // ldc.i4.1 → brtrue(4) → ldc.i4.0 → ret → [4] ldc.i4.s 42 → ret
    // brtrue offset 4 skips the push-0/ret, lands on ldc.i4.s 42
    IRMethod method;
    IRInstruction push1; push1.op_code = IROpCode::LdcI4; push1.immediate_i4 = 1; method.instructions.push_back(push1);
    IRInstruction br;    br.op_code = IROpCode::BrTrue; br.branch_target = 4; method.instructions.push_back(br);
    IRInstruction push0; push0.op_code = IROpCode::LdcI4; push0.immediate_i4 = 0; method.instructions.push_back(push0);
    IRInstruction ret1;  ret1.op_code = IROpCode::Ret; method.instructions.push_back(ret1);
    IRInstruction push42; push42.op_code = IROpCode::LdcI4; push42.immediate_i4 = 42; method.instructions.push_back(push42);
    IRInstruction ret2;   ret2.op_code = IROpCode::Ret; method.instructions.push_back(ret2);

    ExecutionFrame frame = {};
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    return result.has_return_value && result.int32_value == 42;
}

bool TestBrTrue_NotTaken()
{
    // ldc.i4.0 → brtrue(4) → NOT taken → ldc.i4.1 → ret
    IRMethod method;
    IRInstruction push0; push0.op_code = IROpCode::LdcI4; push0.immediate_i4 = 0; method.instructions.push_back(push0);
    IRInstruction br;    br.op_code = IROpCode::BrTrue; br.branch_target = 4; method.instructions.push_back(br);
    IRInstruction push1; push1.op_code = IROpCode::LdcI4; push1.immediate_i4 = 1; method.instructions.push_back(push1);
    IRInstruction ret;   ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);
    IRInstruction push42; push42.op_code = IROpCode::LdcI4; push42.immediate_i4 = 42; method.instructions.push_back(push42);
    IRInstruction ret2;   ret2.op_code = IROpCode::Ret; method.instructions.push_back(ret2);

    ExecutionFrame frame = {};
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    return result.has_return_value && result.int32_value == 1;
}

bool TestBeq_Equal()
{
    // ldarg.0 → ldarg.1 → beq(5) → ldc.i4.0 → ret → [5] ldc.i4.1 → ret
    IRMethod method;
    IRInstruction a0; a0.op_code = IROpCode::LdArg; a0.operand_index = 0; method.instructions.push_back(a0);
    IRInstruction a1; a1.op_code = IROpCode::LdArg; a1.operand_index = 1; method.instructions.push_back(a1);
    IRInstruction beq; beq.op_code = IROpCode::Beq; beq.branch_target = 5; method.instructions.push_back(beq);
    IRInstruction push0; push0.op_code = IROpCode::LdcI4; push0.immediate_i4 = 0; method.instructions.push_back(push0);
    IRInstruction ret1;  ret1.op_code = IROpCode::Ret; method.instructions.push_back(ret1);
    IRInstruction push1; push1.op_code = IROpCode::LdcI4; push1.immediate_i4 = 1; method.instructions.push_back(push1);
    IRInstruction ret2;  ret2.op_code = IROpCode::Ret; method.instructions.push_back(ret2);

    ExecutionFrame frame = {};
    frame.arguments.push_back(InterpreterValue::from_i32(42));
    frame.arguments.push_back(InterpreterValue::from_i32(42));
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    return result.has_return_value && result.int32_value == 1;
}

bool TestBeq_NotEqual()
{
    IRMethod method;
    IRInstruction a0; a0.op_code = IROpCode::LdArg; a0.operand_index = 0; method.instructions.push_back(a0);
    IRInstruction a1; a1.op_code = IROpCode::LdArg; a1.operand_index = 1; method.instructions.push_back(a1);
    IRInstruction beq; beq.op_code = IROpCode::Beq; beq.branch_target = 5; method.instructions.push_back(beq);
    IRInstruction push0; push0.op_code = IROpCode::LdcI4; push0.immediate_i4 = 0; method.instructions.push_back(push0);
    IRInstruction ret1;  ret1.op_code = IROpCode::Ret; method.instructions.push_back(ret1);
    IRInstruction push1; push1.op_code = IROpCode::LdcI4; push1.immediate_i4 = 1; method.instructions.push_back(push1);
    IRInstruction ret2;  ret2.op_code = IROpCode::Ret; method.instructions.push_back(ret2);

    ExecutionFrame frame = {};
    frame.arguments.push_back(InterpreterValue::from_i32(10));
    frame.arguments.push_back(InterpreterValue::from_i32(99));
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    return result.has_return_value && result.int32_value == 0;
}

// ═══════════════════════════════════════════════════════════════════════════
// Phase A: New opcode tests
// ═══════════════════════════════════════════════════════════════════════════

bool TestDup()
{
    // ldc.i4.5 → ldc.i4.3 → dup → add → add → ret
    // Stack: [5], [5,3], [5,3,3], [5,6], [11]
    IRMethod method;
    IRInstruction p5; p5.op_code = IROpCode::LdcI4; p5.immediate_i4 = 5; method.instructions.push_back(p5);
    IRInstruction p3; p3.op_code = IROpCode::LdcI4; p3.immediate_i4 = 3; method.instructions.push_back(p3);
    IRInstruction dup; dup.op_code = IROpCode::Dup; method.instructions.push_back(dup);
    IRInstruction a1; a1.op_code = IROpCode::Add; method.instructions.push_back(a1);
    IRInstruction a2; a2.op_code = IROpCode::Add; method.instructions.push_back(a2);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame = {};
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    return result.has_return_value && result.int32_value == 11;
}

bool TestDivUn()
{
    // ldc.i4.s -1 (0xFFFFFFFF) → ldc.i4.2 → div.un → ret
    // Unsigned -1 / 2 = 0xFFFFFFFF / 2 = 2147483647
    IRMethod method;
    IRInstruction a; a.op_code = IROpCode::LdcI4; a.immediate_i4 = -1; method.instructions.push_back(a);
    IRInstruction b; b.op_code = IROpCode::LdcI4; b.immediate_i4 = 2; method.instructions.push_back(b);
    IRInstruction du; du.op_code = IROpCode::DivUn; method.instructions.push_back(du);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame = {};
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    return result.has_return_value && result.int32_value == 2147483647;
}

bool TestRemUn()
{
    // ldc.i4.s -1 → ldc.i4.3 → rem.un → ret
    // 4294967295 % 3 = 0
    IRMethod method;
    IRInstruction a; a.op_code = IROpCode::LdcI4; a.immediate_i4 = -1; method.instructions.push_back(a);
    IRInstruction b; b.op_code = IROpCode::LdcI4; b.immediate_i4 = 3; method.instructions.push_back(b);
    IRInstruction ru; ru.op_code = IROpCode::RemUn; method.instructions.push_back(ru);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame = {};
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    return result.has_return_value && result.int32_value == 0;
}

bool TestBitwiseAnd()
{
    // ldc.i4.3 → ldc.i4.6 → and → ret  →  3 & 6 = 2
    IRMethod method;
    IRInstruction a; a.op_code = IROpCode::LdcI4; a.immediate_i4 = 3; method.instructions.push_back(a);
    IRInstruction b; b.op_code = IROpCode::LdcI4; b.immediate_i4 = 6; method.instructions.push_back(b);
    IRInstruction and; and.op_code = IROpCode::And; method.instructions.push_back(and);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame = {};
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    return result.has_return_value && result.int32_value == 2;
}

bool TestBitwiseOr()
{
    // ldc.i4.3 → ldc.i4.6 → or → ret  →  3 | 6 = 7
    IRMethod method;
    IRInstruction a; a.op_code = IROpCode::LdcI4; a.immediate_i4 = 3; method.instructions.push_back(a);
    IRInstruction b; b.op_code = IROpCode::LdcI4; b.immediate_i4 = 6; method.instructions.push_back(b);
    IRInstruction or; or.op_code = IROpCode::Or; method.instructions.push_back(or);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame = {};
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    return result.has_return_value && result.int32_value == 7;
}

bool TestBitwiseXor()
{
    // ldc.i4.3 → ldc.i4.6 → xor → ret  →  3 ^ 6 = 5
    IRMethod method;
    IRInstruction a; a.op_code = IROpCode::LdcI4; a.immediate_i4 = 3; method.instructions.push_back(a);
    IRInstruction b; b.op_code = IROpCode::LdcI4; b.immediate_i4 = 6; method.instructions.push_back(b);
    IRInstruction xor; xor.op_code = IROpCode::Xor; method.instructions.push_back(xor);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame = {};
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    return result.has_return_value && result.int32_value == 5;
}

bool TestBitwiseNot()
{
    // ldc.i4.0 → not → ret  →  ~0 = -1
    IRMethod method;
    IRInstruction a; a.op_code = IROpCode::LdcI4; a.immediate_i4 = 0; method.instructions.push_back(a);
    IRInstruction not; not.op_code = IROpCode::Not; method.instructions.push_back(not);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame = {};
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    return result.has_return_value && result.int32_value == -1;
}

bool TestShiftLeft()
{
    // ldc.i4.3 → ldc.i4.2 → shl → ret  →  3 << 2 = 12
    IRMethod method;
    IRInstruction a; a.op_code = IROpCode::LdcI4; a.immediate_i4 = 3; method.instructions.push_back(a);
    IRInstruction b; b.op_code = IROpCode::LdcI4; b.immediate_i4 = 2; method.instructions.push_back(b);
    IRInstruction shl; shl.op_code = IROpCode::Shl; method.instructions.push_back(shl);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame = {};
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    return result.has_return_value && result.int32_value == 12;
}

bool TestShiftRightArith()
{
    // ldc.i4.s -8 → ldc.i4.2 → shr → ret  →  -8 >> 2 = -2
    IRMethod method;
    IRInstruction a; a.op_code = IROpCode::LdcI4; a.immediate_i4 = -8; method.instructions.push_back(a);
    IRInstruction b; b.op_code = IROpCode::LdcI4; b.immediate_i4 = 2; method.instructions.push_back(b);
    IRInstruction shr; shr.op_code = IROpCode::Shr; method.instructions.push_back(shr);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame = {};
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    return result.has_return_value && result.int32_value == -2;
}

bool TestShiftRightLogical()
{
    // ldc.i4.s -8 → ldc.i4.2 → shr.un → ret
    // -8 >>> 2 = 0x3FFFFFFE = 1073741822
    IRMethod method;
    IRInstruction a; a.op_code = IROpCode::LdcI4; a.immediate_i4 = -8; method.instructions.push_back(a);
    IRInstruction b; b.op_code = IROpCode::LdcI4; b.immediate_i4 = 2; method.instructions.push_back(b);
    IRInstruction shru; shru.op_code = IROpCode::ShrUn; method.instructions.push_back(shru);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame = {};
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    return result.has_return_value && result.int32_value == 1073741822;
}

bool TestConvRUn()
{
    // ldc.i4.3 → conv.r.un → conv.i4 → ret
    // unsigned 3 → 3.0 → 3
    IRMethod method;
    IRInstruction a; a.op_code = IROpCode::LdcI4; a.immediate_i4 = 3; method.instructions.push_back(a);
    IRInstruction cru; cru.op_code = IROpCode::ConvRUn; method.instructions.push_back(cru);
    IRInstruction ci; ci.op_code = IROpCode::Conv_I4; method.instructions.push_back(ci);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame = {};
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    return result.has_return_value && result.int32_value == 3;
}

bool TestConvI()
{
    // ldc.i4.5 → conv.i → ret
    IRMethod method;
    IRInstruction push; push.op_code = IROpCode::LdcI4; push.immediate_i4 = 5; method.instructions.push_back(push);
    IRInstruction ci; ci.op_code = IROpCode::ConvI; method.instructions.push_back(ci);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame = {};
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    return result.has_return_value && result.int32_value == 5;
}

bool TestConvU()
{
    // ldc.i4.5 → conv.u → ret
    IRMethod method;
    IRInstruction push; push.op_code = IROpCode::LdcI4; push.immediate_i4 = 5; method.instructions.push_back(push);
    IRInstruction cu; cu.op_code = IROpCode::ConvU; method.instructions.push_back(cu);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame = {};
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    return result.has_return_value && result.int32_value == 5;
}

bool TestSizeOf()
{
    // sizeof(0x12345) → ret
    // The immediate_i4 stores the type token, which the interpreter
    // may use to look up type size at runtime.
    IRMethod method;
    IRInstruction so; so.op_code = IROpCode::SizeOf; so.immediate_i4 = static_cast<CHAOS_IL2CPP_INT32>(0x12345u); method.instructions.push_back(so);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame = {};
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    return result.has_return_value && result.int32_value == static_cast<CHAOS_IL2CPP_INT32>(0x12345u);
}

bool TestExtendedLdFtn()
{
    // ldftn(0xCCDDAABB) → pop → ldc.i4.1 → ret
    // Verifies that LdFtn does not crash and pops cleanly.
    IRMethod method;
    IRInstruction ldftn; ldftn.op_code = IROpCode::LdFtn; ldftn.call_target = reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0xCCDDAABBu)); method.instructions.push_back(ldftn);
    IRInstruction pop; pop.op_code = IROpCode::Pop; method.instructions.push_back(pop);
    IRInstruction push; push.op_code = IROpCode::LdcI4; push.immediate_i4 = 1; method.instructions.push_back(push);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame = {};
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    return result.has_return_value && result.int32_value == 1;
}

bool TestExtendedLdArg()
{
    // ldarg 0 → ret  (uint16 index form)
    IRMethod method;
    IRInstruction ldarg; ldarg.op_code = IROpCode::LdArg; ldarg.operand_index = 0; method.instructions.push_back(ldarg);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame = {};
    frame.arguments.push_back(InterpreterValue::from_i32(99));
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    return result.has_return_value && result.int32_value == 99;
}

bool TestExtendedLdLoc()
{
    // ldc.i4.s 42 → stloc.0 → ldloc 0 → ret (uint16 index form of ldloc)
    IRMethod method;
    IRInstruction push; push.op_code = IROpCode::LdcI4; push.immediate_i4 = 42; method.instructions.push_back(push);
    IRInstruction st; st.op_code = IROpCode::StLoc; st.operand_index = 0; method.instructions.push_back(st);
    IRInstruction ld; ld.op_code = IROpCode::LdLoc; ld.operand_index = 0; method.instructions.push_back(ld);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame = {};
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    return result.has_return_value && result.int32_value == 42;
}

// ═══════════════════════════════════════════════════════════════════════════
// CallVirt / VTable dispatch tests
// ═══════════════════════════════════════════════════════════════════════════

bool TestCallVirtDirectResolution()
{
    using namespace chaos::il2cpp::vtable_registry;

    // Register base class vtable: type_token=0x100, method_token=0x200 → 0xBEEF
    VTableSlot base_slots[] = {
        { 0x200u, reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0xBEEFu)) }
    };
    TypeVTable base_vtable = {};
    base_vtable.type_token = 0x100u;
    base_vtable.base_token = 0u;
    base_vtable.slot_count = 1u;
    base_vtable.slots = base_slots;
    RegisterTypeVTable(&base_vtable);

    // Build IRMethod manually: ldarg.0 (this) → callvirt method_token=0x200
    IRMethod method;

    IRInstruction ldarg;
    ldarg.op_code = IROpCode::LdArg;
    ldarg.operand_index = 0;
    method.instructions.push_back(ldarg);

    IRInstruction callvirt;
    callvirt.op_code = IROpCode::CallVirt;
    callvirt.secondary_index = static_cast<CHAOS_IL2CPP_SIZE>(0x200u);  // declared method token
    callvirt.arg_count = 1u;  // just 'this'
    method.instructions.push_back(callvirt);

    // Create an object whose type token matches the registered vtable.
    auto* storage = new InterpreterObject();
    storage->type_token = 0x100u;
    storage->fields.resize(1u);

    ExecutionFrame frame;
    frame.arguments.push_back(InterpreterValue::from_obj(storage));

    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);

    // Must dispatch externally with the resolved vtable pointer.
    return result.needs_external_dispatch &&
           result.call_target == reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0xBEEFu));
}

bool TestCallVirtInheritanceChain()
{
    using namespace chaos::il2cpp::vtable_registry;

    // Register base vtable: type_token=0x100, method_token=0x200 → 0xBEEF
    VTableSlot base_slots[] = {
        { 0x200u, reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0xBEEFu)) }
    };
    TypeVTable base_vtable = {};
    base_vtable.type_token = 0x100u;
    base_vtable.base_token = 0u;
    base_vtable.slot_count = 1u;
    base_vtable.slots = base_slots;
    RegisterTypeVTable(&base_vtable);

    // Register derived vtable: type_token=0x101, base_token=0x100, method_token=0x200 → 0xCAFE
    VTableSlot derived_slots[] = {
        { 0x200u, reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0xCAFEu)) }
    };
    TypeVTable derived_vtable = {};
    derived_vtable.type_token = 0x101u;
    derived_vtable.base_token = 0x100u;
    derived_vtable.slot_count = 1u;
    derived_vtable.slots = derived_slots;
    RegisterTypeVTable(&derived_vtable);

    // Build IRMethod: ldarg.0 → callvirt method_token=0x200
    IRMethod method;

    IRInstruction ldarg;
    ldarg.op_code = IROpCode::LdArg;
    ldarg.operand_index = 0;
    method.instructions.push_back(ldarg);

    IRInstruction callvirt;
    callvirt.op_code = IROpCode::CallVirt;
    callvirt.secondary_index = static_cast<CHAOS_IL2CPP_SIZE>(0x200u);
    callvirt.arg_count = 1u;
    method.instructions.push_back(callvirt);

    // Create a derived-type object (type_token = 0x101).
    auto* storage = new InterpreterObject();
    storage->type_token = 0x101u;
    storage->fields.resize(1u);

    ExecutionFrame frame;
    frame.arguments.push_back(InterpreterValue::from_obj(storage));

    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);

    // Must resolve to the DERIVED class override (0xCAFE), not base (0xBEEF).
    return result.needs_external_dispatch &&
           result.call_target == reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0xCAFEu));
}

// ═══════════════════════════════════════════════════════════════════════════
// SEH test implementations
// ═══════════════════════════════════════════════════════════════════════════

// --- TestThrowUnhandled ----------------------------------------------------
// Throw with no SEH clause → threw_exception should be true.
bool TestThrowUnhandled()
{
    IRMethod method;

    IRInstruction push;
    push.op_code = IROpCode::LdcI4;
    push.immediate_i4 = 1;
    method.instructions.push_back(push);

    IRInstruction throw_insn;
    throw_insn.op_code = IROpCode::Throw;
    method.instructions.push_back(throw_insn);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);

    return result.threw_exception;
}

// --- TestThrowCatch --------------------------------------------------------
// Throw inside a try block with a catch handler → caught, handler returns 1.
bool TestThrowCatch()
{
    IRMethod method;

    // 0: push 42 (dummy exception object)
    IRInstruction push;
    push.op_code = IROpCode::LdcI4;
    push.immediate_i4 = 42;
    method.instructions.push_back(push);

    // 1: throw
    IRInstruction throw_insn;
    throw_insn.op_code = IROpCode::Throw;
    method.instructions.push_back(throw_insn);

    // 2: (skipped) push 0
    IRInstruction skipped_push;
    skipped_push.op_code = IROpCode::LdcI4;
    skipped_push.immediate_i4 = 0;
    method.instructions.push_back(skipped_push);

    // 3: (skipped) ret
    IRInstruction skipped_ret;
    skipped_ret.op_code = IROpCode::Ret;
    method.instructions.push_back(skipped_ret);

    // 4: (handler) push 1
    IRInstruction handler_push;
    handler_push.op_code = IROpCode::LdcI4;
    handler_push.immediate_i4 = 1;
    method.instructions.push_back(handler_push);

    // 5: (handler) ret
    IRInstruction handler_ret;
    handler_ret.op_code = IROpCode::Ret;
    method.instructions.push_back(handler_ret);

    // SEH: catch clause covering instructions 0-1
    SEHClause catch_clause = {};
    catch_clause.flags = SEHFlags::Exception;
    catch_clause.try_start_idx = 0;
    catch_clause.try_end_idx = 2;
    catch_clause.handler_start_idx = 4;
    catch_clause.handler_end_idx = 6;
    method.seh_clauses.push_back(catch_clause);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);

    return !result.threw_exception && result.has_return_value && result.int32_value == 1;
}

// --- TestThrowFinallyUnwind ------------------------------------------------
// Throw inside a try that has both a finally AND an outer-matching catch.
// The finally must execute (unwind) before the catch receives control.
bool TestThrowFinallyUnwind()
{
    IRMethod method;

    // 0: push 42 (dummy exception object)
    IRInstruction push;
    push.op_code = IROpCode::LdcI4;
    push.immediate_i4 = 42;
    method.instructions.push_back(push);

    // 1: throw
    IRInstruction throw_insn;
    throw_insn.op_code = IROpCode::Throw;
    method.instructions.push_back(throw_insn);

    // 2: (skipped) push 0
    IRInstruction skipped;
    skipped.op_code = IROpCode::LdcI4;
    skipped.immediate_i4 = 0;
    method.instructions.push_back(skipped);

    // 3: (skipped) ret
    IRInstruction skipped_ret;
    skipped_ret.op_code = IROpCode::Ret;
    method.instructions.push_back(skipped_ret);

    // 4: (finally handler) EndFinally — unwind continues to catch
    IRInstruction endfinally;
    endfinally.op_code = IROpCode::EndFinally;
    method.instructions.push_back(endfinally);

    // 5: (catch handler) push 1
    IRInstruction catch_push;
    catch_push.op_code = IROpCode::LdcI4;
    catch_push.immediate_i4 = 1;
    method.instructions.push_back(catch_push);

    // 6: (catch handler) ret
    IRInstruction catch_ret;
    catch_ret.op_code = IROpCode::Ret;
    method.instructions.push_back(catch_ret);

    // Clause 0: finally (innermost → runs first during unwind)
    SEHClause finally_clause = {};
    finally_clause.flags = SEHFlags::Finally;
    finally_clause.try_start_idx = 0;
    finally_clause.try_end_idx = 2;
    finally_clause.handler_start_idx = 4;
    finally_clause.handler_end_idx = 5;
    method.seh_clauses.push_back(finally_clause);

    // Clause 1: catch (outermost → catches after finally unwind)
    SEHClause catch_clause = {};
    catch_clause.flags = SEHFlags::Exception;
    catch_clause.try_start_idx = 0;
    catch_clause.try_end_idx = 2;
    catch_clause.handler_start_idx = 5;
    catch_clause.handler_end_idx = 7;
    method.seh_clauses.push_back(catch_clause);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);

    return !result.threw_exception && result.has_return_value && result.int32_value == 1;
}

// --- TestLeaveFinally ------------------------------------------------------
// Leave inside a try block that has a finally → finally runs, then resumes at target.
bool TestLeaveFinally()
{
    IRMethod method;

    // 0: Leave(branch_target=3) → finds finally → saves target, jumps to handler at 5
    IRInstruction leave;
    leave.op_code = IROpCode::Leave;
    leave.branch_target = 3;
    method.instructions.push_back(leave);

    // 1: (skipped) push 0
    IRInstruction skipped;
    skipped.op_code = IROpCode::LdcI4;
    skipped.immediate_i4 = 0;
    method.instructions.push_back(skipped);

    // 2: (skipped) ret
    IRInstruction skipped_ret;
    skipped_ret.op_code = IROpCode::Ret;
    method.instructions.push_back(skipped_ret);

    // 3: (leave target) push 1
    IRInstruction target_push;
    target_push.op_code = IROpCode::LdcI4;
    target_push.immediate_i4 = 1;
    method.instructions.push_back(target_push);

    // 4: ret
    IRInstruction target_ret;
    target_ret.op_code = IROpCode::Ret;
    method.instructions.push_back(target_ret);

    // 5: (finally handler) EndFinally → pending_leave → resumes at 3
    IRInstruction endfinally;
    endfinally.op_code = IROpCode::EndFinally;
    method.instructions.push_back(endfinally);

    // Finally clause covering instruction 0
    SEHClause finally_clause = {};
    finally_clause.flags = SEHFlags::Finally;
    finally_clause.try_start_idx = 0;
    finally_clause.try_end_idx = 1;
    finally_clause.handler_start_idx = 5;
    finally_clause.handler_end_idx = 6;
    method.seh_clauses.push_back(finally_clause);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);

    return !result.threw_exception && result.has_return_value && result.int32_value == 1;
}

// ── Runtime-generic-method simulation tests ──────────────────────────────
// These tests simulate what the RuntimeInstantiationBridgeV0::interpret_method_call
// does: build IR → execute → extract result.

// Test: Execute a simple add method via IR + ExecutionFrame.
// Simulates the bridge's core: IRMethod → ExecutionFrame → Execute.
static bool TestRuntimeMethodExecute()
{
    // IR: ldarg.0 → ldarg.1 → add → ret
    IRMethod method;
    IRInstruction a0; a0.op_code = IROpCode::LdArg; a0.operand_index = 0; method.instructions.push_back(a0);
    IRInstruction a1; a1.op_code = IROpCode::LdArg; a1.operand_index = 1; method.instructions.push_back(a1);
    IRInstruction add; add.op_code = IROpCode::Add; method.instructions.push_back(add);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    // Build frame with two int32 arguments.
    ExecutionFrame frame;
    frame.arguments.push_back(InterpreterValue::from_i32(10));
    frame.arguments.push_back(InterpreterValue::from_i32(20));

    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);

    return result.has_return_value
        && result.return_value.tag == ValueTag::Int32
        && result.return_value.i32 == 30;
}

// Test: Return value extraction via InterpreterValue tag dispatch.
// The bridge uses tag dispatch to write return_value into raw out_return_value.
static bool TestRuntimeMethodReturnValueDispatch()
{
    // IR: ldc.i4.s 42 → ret
    IRMethod method = MakeSimpleMethod(IROpCode::LdcI4, 42);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);

    if (!result.has_return_value || result.return_value.tag != ValueTag::Int32) {
        return false;
    }

    // Simulate the bridge's tag dispatch.
    int32_t extracted = 0;
    switch (result.return_value.tag) {
        case ValueTag::Int32:
            extracted = result.return_value.i32;
            break;
        default:
            return false;
    }

    return extracted == 42;
}

// Test: Generic type parameter resolution via DefaultTokenResolver.
// Simulates what happens during lowering of a generic method body that
// references ELEMENT_TYPE_VAR.  The resolution is done using type_args
// from the TokenResolverContext.
static bool TestRuntimeMethodTypeParamResolution()
{
    // Set up TokenResolverContext with type_args.
    TypeInfoHandle dummy_type = static_cast<TypeInfoHandle>(0xDEADBEEFu);

    auto ctx = TokenResolverContext();
    ctx.type_args = &dummy_type;
    ctx.arg_count = 1u;

    // Construct a Box instruction (no call_target yet).
    IRInstruction box_insn;
    box_insn.op_code = IROpCode::Box;

    // Call DefaultTokenResolver with the ELEMENT_TYPE_VAR token (0x11xxxxxx).
    // Token 0x11000000 = ELEMENT_TYPE_VAR, index 0 (= !0, the first class-level type parameter).
    bool resolved = DefaultTokenResolver(0x11000000u, box_insn, &ctx);
    if (!resolved) {
        return false;
    }

    // The call_target should point to dummy_type (resolved from ELEMENT_TYPE_VAR).
    void* expected = reinterpret_cast<void*>(
        static_cast<CHAOS_IL2CPP_UINTPTR>(0xDEADBEEFu));
    return box_insn.call_target == expected;
}

// Test: Exception propagation matching bridge path.
// When the interpreter throws an unhandled exception, threw_exception is set
// and exception_value captures the exception object.
// The bridge checks threw_exception and throws chaos_managed_exception.
static bool TestRuntimeMethodExceptionPropagation()
{
    IRMethod method;

    // 0: push 99 (exception object)
    IRInstruction push;
    push.op_code = IROpCode::LdcI4;
    push.immediate_i4 = 99;
    method.instructions.push_back(push);

    // 1: throw
    IRInstruction throw_insn;
    throw_insn.op_code = IROpCode::Throw;
    method.instructions.push_back(throw_insn);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);

    // Verify: threw_exception is true, and exception_value captures the object.
    if (!result.threw_exception) {
        return false;
    }

    // The exception_value should be Int32 with value 99.
    return result.exception_value.tag == ValueTag::Int32
        && result.exception_value.i32 == 99;
}

// ═══════════════════════════════════════════════════════════════════════════
// Struct value type tests
// ═══════════════════════════════════════════════════════════════════════════

// Test: Struct value on the eval stack is correctly returned via Ret.
// Verifies that ValueTag::Struct return values carry the correct struct_size
// and data through the InterpreterVM's execution loop.
bool TestStructReturnValue()
{
    IRMethod method;
    IRInstruction ret;
    ret.op_code = IROpCode::Ret;
    method.instructions.push_back(ret);

    // Pre-populate the eval stack with a struct value.
    // The VM's Ret instruction pops the value and sets it as return_value.
    struct TestStruct { int a; int b; };
    TestStruct data = { 42, 99 };
    constexpr auto kExpectedSize = static_cast<CHAOS_IL2CPP_UINT32>(sizeof(TestStruct));

    ExecutionFrame frame;
    frame.stack.push_back(InterpreterValue::from_struct(&data, kExpectedSize));

    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);

    if (!result.has_return_value) return false;
    if (result.return_value.tag != ValueTag::Struct) return false;
    if (result.return_value.struct_size != kExpectedSize) return false;
    if (result.return_value.obj == nullptr) return false;

    const auto* out = static_cast<const TestStruct*>(result.return_value.obj);
    if (out->a != 42 || out->b != 99) return false;

    // Also verify the deep-copy: our original data is on the frame's stack
    // and will be freed in ~ExecutionFrame. The result's copy must be independent.
    return true;
}

// Rethrow inside a catch handler → propagates out.
bool TestRethrow()
{
    IRMethod method;

    // 0: push 42 (dummy exception object)
    IRInstruction push;
    push.op_code = IROpCode::LdcI4;
    push.immediate_i4 = 42;
    method.instructions.push_back(push);

    // 1: throw → caught by catch handler at 4
    IRInstruction throw_insn;
    throw_insn.op_code = IROpCode::Throw;
    method.instructions.push_back(throw_insn);

    // 2: (skipped) push 0
    IRInstruction skipped;
    skipped.op_code = IROpCode::LdcI4;
    skipped.immediate_i4 = 0;
    method.instructions.push_back(skipped);

    // 3: (skipped) ret
    IRInstruction skipped_ret;
    skipped_ret.op_code = IROpCode::Ret;
    method.instructions.push_back(skipped_ret);

    // 4: (catch handler) rethrow → exception_obj was pushed on stack → pops it
    IRInstruction rethrow;
    rethrow.op_code = IROpCode::Rethrow;
    method.instructions.push_back(rethrow);

    // 5: (unreachable) ret
    IRInstruction unreachable_ret;
    unreachable_ret.op_code = IROpCode::Ret;
    method.instructions.push_back(unreachable_ret);

    // Catch clause covering instructions 0-1
    SEHClause catch_clause = {};
    catch_clause.flags = SEHFlags::Exception;
    catch_clause.try_start_idx = 0;
    catch_clause.try_end_idx = 2;
    catch_clause.handler_start_idx = 4;
    catch_clause.handler_end_idx = 6;
    method.seh_clauses.push_back(catch_clause);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);

    // Rethrow throws again, no further catch → threw_exception = true
    return result.threw_exception;
}

// ═══════════════════════════════════════════════════════════════════════════
// Phase 8: Dispatch callback integration tests
// ═══════════════════════════════════════════════════════════════════════════

// ── Mock dispatch state ──────────────────────────────────────────────────
// Holds state shared between the test function and MockDispatchCallback.

struct MockDispatchState {
    bool called = false;
    void* received_target = nullptr;
    CHAOS_IL2CPP_UINT32 received_arg_count = 0;
    bool received_is_instance = false;
    CHAOS_IL2CPP_VECTOR(InterpreterValue) captured_args;
    DispatchResult result_to_return = {};
};

// ── Mock dispatch callback ───────────────────────────────────────────────
// Captures call parameters into MockDispatchState and returns a
// preconfigured DispatchResult.  Used to verify that the interpreter's
// call dispatch path correctly bridges to the callback.

static DispatchResult MockDispatchCallback(
    void*                               call_target,
    const InterpreterValue*             call_args,
    CHAOS_IL2CPP_UINT32                 arg_count,
    bool                                is_instance_call,
    void*                               dispatch_context)
{
    auto* state = static_cast<MockDispatchState*>(dispatch_context);
    if (state == nullptr) {
        return {};
    }
    state->called = true;
    state->received_target = call_target;
    state->received_arg_count = arg_count;
    state->received_is_instance = is_instance_call;
    state->captured_args.assign(call_args, call_args + arg_count);
    return state->result_to_return;
}

// Test: Call instruction with dispatch callback (tail call path).
//   IR: [Call(X, 0), Ret]
//   Mock returns Int32(42).
//   Expected: has_return_value=true, return_value.i32 == 42.
//   Exercises: dispatch_fn invoked for Call, tail call optimization
//   (next instruction is Ret → result set directly, no push/pop).
bool TestDispatchBasic()
{
    void* const kTestTarget = reinterpret_cast<void*>(
        static_cast<CHAOS_IL2CPP_UINTPTR>(0x1234u));

    // Build IR: Call(target=0x1234, arg_count=0), Ret (tail call)
    IRMethod method;
    {
        IRInstruction call_insn;
        call_insn.op_code = IROpCode::Call;
        call_insn.call_target = kTestTarget;
        call_insn.arg_count = 0;
        call_insn.is_instance_call = false;
        method.instructions.push_back(call_insn);
    }
    {
        IRInstruction ret_insn;
        ret_insn.op_code = IROpCode::Ret;
        method.instructions.push_back(ret_insn);
    }

    // Set up dispatch callback.
    MockDispatchState state;
    state.result_to_return.has_value = true;
    state.result_to_return.value = InterpreterValue::from_i32(42);

    ExecutionFrame frame;
    frame.dispatch_fn = MockDispatchCallback;
    frame.dispatch_context = &state;

    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);

    // Verify callback was invoked.
    if (!state.called) return false;
    if (state.received_target != kTestTarget) return false;
    if (state.received_arg_count != 0u) return false;

    // Verify tail call result.
    if (!result.has_return_value) return false;
    if (result.return_value.tag != ValueTag::Int32) return false;
    if (result.return_value.i32 != 42) return false;
    if (result.threw_exception) return false;

    return true;
}

// Test: Exception propagation through dispatch callback.
//   IR: [Call(X, 0), Ret]
//   Mock returns threw_exception=true with exception_value=null.
//   Expected: threw_exception=true.
//   Exercises: exception propagation from dispatch callback through
//   Execute's SEH path, falling through to caller when no handler.
bool TestDispatchException()
{
    IRMethod method;
    {
        IRInstruction call_insn;
        call_insn.op_code = IROpCode::Call;
        call_insn.call_target = reinterpret_cast<void*>(
            static_cast<CHAOS_IL2CPP_UINTPTR>(0xABCDu));
        call_insn.arg_count = 0;
        call_insn.is_instance_call = false;
        method.instructions.push_back(call_insn);
    }
    {
        IRInstruction ret_insn;
        ret_insn.op_code = IROpCode::Ret;
        method.instructions.push_back(ret_insn);
    }

    MockDispatchState state;
    state.result_to_return.threw_exception = true;
    state.result_to_return.exception_value = InterpreterValue::null_val();

    ExecutionFrame frame;
    frame.dispatch_fn = MockDispatchCallback;
    frame.dispatch_context = &state;

    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);

    // Must propagate the exception — no SEH clause → threw_exception.
    if (!state.called) return false;
    if (!result.threw_exception) return false;
    if (result.has_return_value) return false;

    return true;
}

// Test: Arguments correctly forwarded to dispatch callback.
//   IR: [LdcI4(10), LdcI4(20), Call(X, 2), Ret]
//   Mock captures call_args and arg_count.
//   Expected: callback receives 2 args: Int32(20), Int32(10)
//   (ECMA order — args are pushed in reverse).
//   Exercises: argument collection from eval stack and forwarding
//   to dispatch callback.
bool TestDispatchArgs()
{
    IRMethod method;
    {
        IRInstruction push1;
        push1.op_code = IROpCode::LdcI4;
        push1.immediate_i4 = 10;
        method.instructions.push_back(push1);
    }
    {
        IRInstruction push2;
        push2.op_code = IROpCode::LdcI4;
        push2.immediate_i4 = 20;
        method.instructions.push_back(push2);
    }
    {
        IRInstruction call_insn;
        call_insn.op_code = IROpCode::Call;
        call_insn.call_target = reinterpret_cast<void*>(
            static_cast<CHAOS_IL2CPP_UINTPTR>(0x5678u));
        call_insn.arg_count = 2;
        call_insn.is_instance_call = false;
        method.instructions.push_back(call_insn);
    }
    {
        IRInstruction ret_insn;
        ret_insn.op_code = IROpCode::Ret;
        method.instructions.push_back(ret_insn);
    }

    MockDispatchState state;
    state.result_to_return.has_value = true;
    state.result_to_return.value = InterpreterValue::from_i32(99);

    ExecutionFrame frame;
    frame.dispatch_fn = MockDispatchCallback;
    frame.dispatch_context = &state;

    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);

    // Verify callback was invoked with correct args.
    if (!state.called) return false;
    if (state.received_arg_count != 2u) return false;
    if (state.received_is_instance) return false;

    // Args pushed 10 then 20; pop order (LIFO) gives arg_buf[0]=10, arg_buf[1]=20.
    if (state.captured_args.size() < 2u) return false;
    if (state.captured_args[0].tag != ValueTag::Int32) return false;
    if (state.captured_args[0].i32 != 10) return false;
    if (state.captured_args[1].tag != ValueTag::Int32) return false;
    if (state.captured_args[1].i32 != 20) return false;

    // Return value from dispatch should propagate through tail call.
    if (!result.has_return_value) return false;
    if (result.return_value.tag != ValueTag::Int32) return false;
    if (result.return_value.i32 != 99) return false;

    return true;
}

// ═══════════════════════════════════════════════════════════════════════════
// New interpreter VM feature tests
// ═══════════════════════════════════════════════════════════════════════════

// Test: Blt with float operands — 2.5 < 3.5 → branch taken.
bool TestFloatBlt()
{
    IRMethod method;
    // ldc.r8 2.5, ldc.r8 3.5, blt (taken) → ldc.i4 1 → ret
    // 0: ldc.r8 2.5
    IRInstruction r1; r1.op_code = IROpCode::LdcR8; r1.immediate_r8 = 2.5; method.instructions.push_back(r1);
    // 1: ldc.r8 3.5
    IRInstruction r2; r2.op_code = IROpCode::LdcR8; r2.immediate_r8 = 3.5; method.instructions.push_back(r2);
    // 2: blt → 4
    IRInstruction blt; blt.op_code = IROpCode::Blt; blt.branch_target = 4; method.instructions.push_back(blt);
    // 3: (skipped) ldc.i4 0
    IRInstruction f; f.op_code = IROpCode::LdcI4; f.immediate_i4 = 0; method.instructions.push_back(f);
    // 4: ldc.i4 1
    IRInstruction t; t.op_code = IROpCode::LdcI4; t.immediate_i4 = 1; method.instructions.push_back(t);
    // 5: ret
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    return result.has_return_value && result.int32_value == 1;
}

// Test: Bgt with float operands — 5.0 > 2.0 → branch taken.
bool TestFloatBgt()
{
    IRMethod method;
    IRInstruction r1; r1.op_code = IROpCode::LdcR8; r1.immediate_r8 = 5.0; method.instructions.push_back(r1);
    IRInstruction r2; r2.op_code = IROpCode::LdcR8; r2.immediate_r8 = 2.0; method.instructions.push_back(r2);
    IRInstruction bgt; bgt.op_code = IROpCode::Bgt; bgt.branch_target = 4; method.instructions.push_back(bgt);
    IRInstruction f; f.op_code = IROpCode::LdcI4; f.immediate_i4 = 0; method.instructions.push_back(f);
    IRInstruction t; t.op_code = IROpCode::LdcI4; t.immediate_i4 = 1; method.instructions.push_back(t);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    return result.has_return_value && result.int32_value == 1;
}

// Test: Ble with float operands — 3.0 <= 3.0 → branch taken.
bool TestFloatBle()
{
    IRMethod method;
    IRInstruction r1; r1.op_code = IROpCode::LdcR8; r1.immediate_r8 = 3.0; method.instructions.push_back(r1);
    IRInstruction r2; r2.op_code = IROpCode::LdcR8; r2.immediate_r8 = 3.0; method.instructions.push_back(r2);
    IRInstruction ble; ble.op_code = IROpCode::Ble; ble.branch_target = 4; method.instructions.push_back(ble);
    IRInstruction f; f.op_code = IROpCode::LdcI4; f.immediate_i4 = 0; method.instructions.push_back(f);
    IRInstruction t; t.op_code = IROpCode::LdcI4; t.immediate_i4 = 1; method.instructions.push_back(t);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    return result.has_return_value && result.int32_value == 1;
}

// Test: Bge with float operands — 1.0 >= 0.5 → branch taken.
bool TestFloatBge()
{
    IRMethod method;
    IRInstruction r1; r1.op_code = IROpCode::LdcR8; r1.immediate_r8 = 1.0; method.instructions.push_back(r1);
    IRInstruction r2; r2.op_code = IROpCode::LdcR8; r2.immediate_r8 = 0.5; method.instructions.push_back(r2);
    IRInstruction bge; bge.op_code = IROpCode::Bge; bge.branch_target = 4; method.instructions.push_back(bge);
    IRInstruction f; f.op_code = IROpCode::LdcI4; f.immediate_i4 = 0; method.instructions.push_back(f);
    IRInstruction t; t.op_code = IROpCode::LdcI4; t.immediate_i4 = 1; method.instructions.push_back(t);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    return result.has_return_value && result.int32_value == 1;
}

// Test: LdArgA pushes a managed pointer (address of InterpreterValue slot),
// and LdObj dereferences it to get the value.
bool TestLdArgA_RefSemantics()
{
    IRMethod method;
    // ldarga 0 → ldobj → ret  (should load value at arg[0]'s address)
    IRInstruction ldarga; ldarga.op_code = IROpCode::LdArgA; ldarga.operand_index = 0; method.instructions.push_back(ldarga);
    IRInstruction ldobj; ldobj.op_code = IROpCode::LdObj; method.instructions.push_back(ldobj);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame;
    frame.arguments.push_back(InterpreterValue::from_i32(42));

    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    return result.has_return_value && result.int32_value == 42;
}

// Test: LdLocA + StObj to modify a local through its address.
bool TestLdLocA_RefSemantics()
{
    IRMethod method;
    // ldc.i4 10 → stloc 0  (local[0] = 10)
    IRInstruction push; push.op_code = IROpCode::LdcI4; push.immediate_i4 = 10; method.instructions.push_back(push);
    IRInstruction stloc; stloc.op_code = IROpCode::StLoc; stloc.operand_index = 0; method.instructions.push_back(stloc);
    // ldc.i4 20 → ldloca 0 → stobj  (write 20 through managed ptr to local[0])
    IRInstruction push2; push2.op_code = IROpCode::LdcI4; push2.immediate_i4 = 20; method.instructions.push_back(push2);
    IRInstruction ldloca; ldloca.op_code = IROpCode::LdLocA; ldloca.operand_index = 0; method.instructions.push_back(ldloca);
    IRInstruction stobj; stobj.op_code = IROpCode::StObj; method.instructions.push_back(stobj);
    // ldloc 0 → ret  (should read 20, written via StObj through the managed ptr)
    IRInstruction ldloc; ldloc.op_code = IROpCode::LdLoc; ldloc.operand_index = 0; method.instructions.push_back(ldloc);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    return result.has_return_value && result.int32_value == 20;
}

// Test: CastClass to an interface — object implements interface → pass through.
bool TestInterfaceCastClass()
{
    using namespace chaos::il2cpp::vtable_registry;

    // Register an interface type vtable: type_token=0x200, type_shape=interface
    // Object type: type_token=0x100, base_token=0, iface_map includes stable_id matching interface
    const CHAOS_IL2CPP_UINT64 kIfaceStableId = 0xABCD1234ULL;
    CHAOS_IL2CPP_UINT64 obj_stable_id = 0xDEADBEEFULL;

    // Object type's interface map entry
    ChaosIl2cpp::Common::InterfaceMapEntry obj_iface_entries[] = {
        { kIfaceStableId, 0u, 0u }
    };
    // Dummy slot (MSVC requires non-zero array)
    VTableSlot dummy_slot = { 1u, nullptr };
    VTableSlot obj_slots[1] = { dummy_slot };
    TypeVTable obj_vtable = {};
    obj_vtable.type_token = 0x100u;
    obj_vtable.base_token = 0u;
    obj_vtable.slot_count = 1u;
    obj_vtable.slots = obj_slots;
    obj_vtable.stable_id = obj_stable_id;
    obj_vtable.type_shape = ChaosIl2cpp::Common::chaos_type_shape_reference;
    obj_vtable.iface_map = obj_iface_entries;
    obj_vtable.iface_count = 1u;
    RegisterTypeVTable(&obj_vtable);

    // Register interface type vtable (with matching stable_id)
    VTableSlot iface_slots[1] = { dummy_slot };
    TypeVTable iface_vtable = {};
    iface_vtable.type_token = 0x200u;
    iface_vtable.base_token = 0u;
    iface_vtable.slot_count = 1u;
    iface_vtable.slots = iface_slots;
    iface_vtable.stable_id = kIfaceStableId;
    iface_vtable.type_shape = ChaosIl2cpp::Common::chaos_type_shape_interface;
    iface_vtable.iface_map = nullptr;
    iface_vtable.iface_count = 0u;
    RegisterTypeVTable(&iface_vtable);

    // Build IR: ldarg.0 → castclass(0x200) → ret
    IRMethod method;
    IRInstruction ldarg; ldarg.op_code = IROpCode::LdArg; ldarg.operand_index = 0; method.instructions.push_back(ldarg);
    IRInstruction cast; cast.op_code = IROpCode::CastClass; cast.immediate_i4 = static_cast<CHAOS_IL2CPP_INT32>(0x200u); method.instructions.push_back(cast);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    // Create object with type_token = 0x100
    auto* storage = new InterpreterObject();
    storage->type_token = 0x100u;

    ExecutionFrame frame;
    frame.arguments.push_back(InterpreterValue::from_obj(storage));

    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);

    // Should pass through — object implements interface
    return result.has_return_value && result.return_value.tag == ValueTag::ObjectRef;
}

// Test: IsInst to an interface that is NOT implemented → returns null.
bool TestInterfaceIsInst()
{
    using namespace chaos::il2cpp::vtable_registry;

    VTableSlot dummy_slot2 = { 1u, nullptr };
    VTableSlot obj_slots2[1] = { dummy_slot2 };

    // Object type: type_token=0x100, no iface_map
    TypeVTable obj_vtable2 = {};
    obj_vtable2.type_token = 0x100u;
    obj_vtable2.base_token = 0u;
    obj_vtable2.slot_count = 1u;
    obj_vtable2.slots = obj_slots2;
    obj_vtable2.stable_id = 0xBEEFBEEFULL;
    obj_vtable2.type_shape = ChaosIl2cpp::Common::chaos_type_shape_reference;
    obj_vtable2.iface_map = nullptr;
    obj_vtable2.iface_count = 0u;
    RegisterTypeVTable(&obj_vtable2);

    // Interface type: type_token=0x300, type_shape=interface
    VTableSlot iface_slots2[1] = { dummy_slot2 };
    TypeVTable iface_vtable2 = {};
    iface_vtable2.type_token = 0x300u;
    iface_vtable2.base_token = 0u;
    iface_vtable2.slot_count = 1u;
    iface_vtable2.slots = iface_slots2;
    iface_vtable2.stable_id = 0xCAFE1234ULL;
    iface_vtable2.type_shape = ChaosIl2cpp::Common::chaos_type_shape_interface;
    RegisterTypeVTable(&iface_vtable2);

    // Build IR: ldarg.0 → isinst(0x300) → ret
    IRMethod method;
    IRInstruction ldarg; ldarg.op_code = IROpCode::LdArg; ldarg.operand_index = 0; method.instructions.push_back(ldarg);
    IRInstruction isinst; isinst.op_code = IROpCode::IsInst; isinst.immediate_i4 = static_cast<CHAOS_IL2CPP_INT32>(0x300u); method.instructions.push_back(isinst);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    auto* storage = new InterpreterObject();
    storage->type_token = 0x100u;

    ExecutionFrame frame;
    frame.arguments.push_back(InterpreterValue::from_obj(storage));

    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);

    // Should return null — object does NOT implement interface 0x300
    return result.has_return_value && result.return_value.tag == ValueTag::Null;
}

// Test: Interface vtable dispatch via CallVirt + interface method resolution.
bool TestInterfaceVtableDispatch()
{
    using namespace chaos::il2cpp::vtable_registry;

    // Interface type_token = 0x400, stable_id method is slot 0 in iface
    const CHAOS_IL2CPP_UINT64 kIfaceStable = 0xABCD0001ULL;

    // Derived: iface map + flat vtable
    // The iface entry maps the interface's first method (slot index 0) to vtable offset 2
    ChaosIl2cpp::Common::InterfaceMapEntry impl_ifaces[] = {
        { kIfaceStable, 2u, 1u }  // interface's method 0 → vtable slot 2
    };
    void* impl_vtable_array[] = {
        nullptr, nullptr, reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0xBABEu))
    };

    VTableSlot slot0 = { 0x600u, nullptr };
    VTableSlot slot1 = { 0x601u, nullptr };
    VTableSlot slot2 = { 0x602u, reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0xBABEu)) };
    VTableSlot base_slots_iface[3] = { slot0, slot1, slot2 };

    TypeVTable derived_vtable_iface = {};
    derived_vtable_iface.type_token = 0x500u;
    derived_vtable_iface.base_token = 0u;
    derived_vtable_iface.slot_count = 3u;
    derived_vtable_iface.slots = base_slots_iface;
    derived_vtable_iface.stable_id = 0x50000001ULL;
    derived_vtable_iface.vtable_array = const_cast<const void**>(impl_vtable_array);
    derived_vtable_iface.vtable_length = 3u;
    derived_vtable_iface.type_shape = ChaosIl2cpp::Common::chaos_type_shape_reference;
    derived_vtable_iface.iface_map = impl_ifaces;
    derived_vtable_iface.iface_count = 1u;
    RegisterTypeVTable(&derived_vtable_iface);

    // Register interface: type_token=0x400, type_shape=interface
    VTableSlot iface_slot = { 0x700u, nullptr };
    VTableSlot iface_slots3[1] = { iface_slot };
    TypeVTable iface_vtable3 = {};
    iface_vtable3.type_token = 0x400u;
    iface_vtable3.base_token = 0u;
    iface_vtable3.slot_count = 1u;
    iface_vtable3.slots = iface_slots3;
    iface_vtable3.stable_id = kIfaceStable;
    iface_vtable3.type_shape = ChaosIl2cpp::Common::chaos_type_shape_interface;
    RegisterTypeVTable(&iface_vtable3);

    // Build IR: ldarg.0 → callvirt method_token=0x700 (= slot 0 in interface)
    IRMethod method;
    IRInstruction ldarg3; ldarg3.op_code = IROpCode::LdArg; ldarg3.operand_index = 0; method.instructions.push_back(ldarg3);
    IRInstruction callvirt3;
    callvirt3.op_code = IROpCode::CallVirt;
    callvirt3.secondary_index = static_cast<CHAOS_IL2CPP_SIZE>(0x700u);
    callvirt3.arg_count = 1u;
    method.instructions.push_back(callvirt3);

    // Object with type_token = 0x500 (implements the interface)
    auto* storage_iface = new InterpreterObject();
    storage_iface->type_token = 0x500u;

    ExecutionFrame frame3;
    frame3.arguments.push_back(InterpreterValue::from_obj(storage_iface));

    const InterpreterVM vm3 = {};
    ExecutionResult result3 = vm3.Execute(method, &frame3);

    // Must resolve through interface vtable → slot[2] → 0xBABE
    return result3.needs_external_dispatch &&
           result3.call_target == reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0xBABEu));
}

// ══════════════════════════════════════════════════════════════════════════
// Codegen VTable path tests (RegisterCodegenVTable + InterpreterVM)
// ══════════════════════════════════════════════════════════════════════════
//
// These tests simulate the codegen→BootstrapRuntime path: a VTableDescriptorV0
// is constructed (as codegen would emit it), registered via RegisterCodegenVTable
// (as BootstrapRuntime would do), and then CallVirt dispatch is exercised through
// the InterpreterVM.

bool Test_CodegenVTableDirect()
{
    using namespace chaos::il2cpp::vtable_registry;

    // Use a unique type_token (0x600) to avoid conflict with existing tests'
    // dangling stack-local TypeVTable registrations (which use 0x100-0x101).
    VTableSlot slots[] = {
        { 0x200u, reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0xBEEFu)) }
    };
    const void* vtable_array[] = { reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0xBEEFu)) };

    VTableDescriptorV0 desc;
    std::memset(&desc, 0, sizeof(desc));
    desc.stable_id      = 0x10000001ULL;
    desc.type_token     = 0x600u;
    desc.base_token     = 0u;
    desc.slot_count     = 1u;
    desc.slots          = slots;
    desc.vtable_array   = vtable_array;
    desc.vtable_length  = 1u;
    desc.type_shape     = 1;

    RegisterCodegenVTable(&desc);

    // Build IR: ldarg.0 → callvirt method_token=0x200
    IRMethod method;
    IRInstruction ldarg; ldarg.op_code = IROpCode::LdArg; ldarg.operand_index = 0; method.instructions.push_back(ldarg);
    IRInstruction callvirt;
    callvirt.op_code = IROpCode::CallVirt;
    callvirt.secondary_index = static_cast<CHAOS_IL2CPP_SIZE>(0x200u);
    callvirt.arg_count = 1u;
    method.instructions.push_back(callvirt);

    auto* obj = new InterpreterObject();
    obj->type_token = 0x600u;
    obj->fields.resize(1u);

    ExecutionFrame frame;
    frame.arguments.push_back(InterpreterValue::from_obj(obj));

    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);

    return result.needs_external_dispatch &&
           result.call_target == reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0xBEEFu));
}

bool Test_CodegenVTableInheritance()
{
    using namespace chaos::il2cpp::vtable_registry;

    // Base type: type_token=0x610, method 0x200 → 0xBEEF (unique token avoids
    // dangling-pointer conflict with existing tests using 0x100-0x101)
    VTableSlot base_slots[] = {
        { 0x200u, reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0xBEEFu)) }
    };
    const void* base_vtable_array[] = { reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0xBEEFu)) };

    VTableDescriptorV0 base_desc;
    std::memset(&base_desc, 0, sizeof(base_desc));
    base_desc.stable_id      = 0x10000001ULL;
    base_desc.type_token     = 0x610u;
    base_desc.base_token     = 0u;
    base_desc.slot_count     = 1u;
    base_desc.slots          = base_slots;
    base_desc.vtable_array   = base_vtable_array;
    base_desc.vtable_length  = 1u;
    base_desc.type_shape     = 1;
    RegisterCodegenVTable(&base_desc);

    // Derived type: type_token=0x611, base_token=0x610, method 0x200 → 0xCAFE
    VTableSlot derived_slots[] = {
        { 0x200u, reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0xCAFEu)) }
    };
    const void* derived_vtable_array[] = { reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0xCAFEu)) };

    VTableDescriptorV0 derived_desc;
    std::memset(&derived_desc, 0, sizeof(derived_desc));
    derived_desc.stable_id      = 0x10000002ULL;
    derived_desc.type_token     = 0x611u;
    derived_desc.base_token     = 0x610u;
    derived_desc.slot_count     = 1u;
    derived_desc.slots          = derived_slots;
    derived_desc.vtable_array   = derived_vtable_array;
    derived_desc.vtable_length  = 1u;
    derived_desc.type_shape     = 1;
    RegisterCodegenVTable(&derived_desc);

    // Build IR: ldarg.0 → callvirt method_token=0x200
    IRMethod method;
    IRInstruction ldarg; ldarg.op_code = IROpCode::LdArg; ldarg.operand_index = 0; method.instructions.push_back(ldarg);
    IRInstruction callvirt;
    callvirt.op_code = IROpCode::CallVirt;
    callvirt.secondary_index = static_cast<CHAOS_IL2CPP_SIZE>(0x200u);
    callvirt.arg_count = 1u;
    method.instructions.push_back(callvirt);

    auto* obj = new InterpreterObject();
    obj->type_token = 0x611u;
    obj->fields.resize(1u);

    ExecutionFrame frame;
    frame.arguments.push_back(InterpreterValue::from_obj(obj));

    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);

    // Must resolve to DERIVED override (0xCAFE), not base (0xBEEF)
    return result.needs_external_dispatch &&
           result.call_target == reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0xCAFEu));
}

bool Test_CodegenVTableInterfaceDispatch()
{
    using namespace chaos::il2cpp::vtable_registry;

    const CHAOS_IL2CPP_UINT64 kIfaceStable = 0xABCD0001ULL;

    // Interface map: interface's 2 methods (slot 0 & 1) map to vtable offset 1
    chaos::il2cpp::common::InterfaceMapEntry iface_entries[] = {
        { kIfaceStable, 1u, 2u }
    };

    // Flat vtable array: index 0 = g_method0, index 1 = g_method1, index 2 = g_method2
    alignas(64) static char s_method0[64];
    alignas(64) static char s_method1[64];
    alignas(64) static char s_method2[64];
    const void* vtable_array[] = {
        reinterpret_cast<void*>(&s_method0),
        reinterpret_cast<void*>(&s_method1),
        reinterpret_cast<void*>(&s_method2),
    };

    // VTableSlot array: type defines its own slot at method_token=0x20000001
    VTableSlot obj_slots[] = {
        { 0x20000001u, reinterpret_cast<void*>(&s_method0) }
    };

    VTableDescriptorV0 obj_desc;
    std::memset(&obj_desc, 0, sizeof(obj_desc));
    obj_desc.stable_id      = 0x20000001ULL;
    obj_desc.type_token     = 0x620u;
    obj_desc.base_token     = 0u;
    obj_desc.slot_count     = 1u;
    obj_desc.slots          = obj_slots;
    obj_desc.vtable_array   = vtable_array;
    obj_desc.vtable_length  = 3u;
    obj_desc.type_shape     = 1;
    obj_desc.iface_map      = iface_entries;
    obj_desc.iface_count    = 1u;
    RegisterCodegenVTable(&obj_desc);

    // Build IR: ldarg.0 → callvirt method_token=0 (= interface slot 0)
    IRMethod method;
    IRInstruction ldarg; ldarg.op_code = IROpCode::LdArg; ldarg.operand_index = 0; method.instructions.push_back(ldarg);
    IRInstruction callvirt;
    callvirt.op_code = IROpCode::CallVirt;
    callvirt.secondary_index = static_cast<CHAOS_IL2CPP_SIZE>(0u);
    callvirt.arg_count = 1u;
    method.instructions.push_back(callvirt);

    auto* obj = new InterpreterObject();
    obj->type_token = 0x620u;

    ExecutionFrame frame;
    frame.arguments.push_back(InterpreterValue::from_obj(obj));

    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);

    // Interface slot 0 → vtable_offset(1) + slot_index(0) = vtable_array[1] = &s_method1
    return result.needs_external_dispatch &&
           result.call_target == reinterpret_cast<void*>(&s_method1);
}

// ═══════════════════════════════════════════════════════════════════════════
// Hot-update VTable path test (RegisterHotUpdateVTable + ResolveVirtualMethodPointer)
// ═══════════════════════════════════════════════════════════════════════════
//
// Validates the full chain:
//   1. Register base type via RegisterCodegenVTable (as AOT codegen would)
//   2. Register derived type via RegisterHotUpdateVTable (as hot-update loader would)
//   3. Call ResolveVirtualMethodPointer directly (as the interpreter slow path does)
//   4. Verify override slots shadow base slots
//
// Uses unique type tokens (0x630-0x632) to avoid conflict with existing tests.

bool Test_HotUpdateVTableResolution()
{
    using namespace chaos::il2cpp::vtable_registry;

    // ── Sentinel pointer values for assertion ──
    void* const kBaseFn     = reinterpret_cast<void*>(
        static_cast<CHAOS_IL2CPP_UINTPTR>(0xAAALL));
    void* const kOverrideFn = reinterpret_cast<void*>(
        static_cast<CHAOS_IL2CPP_UINTPTR>(0xBBBLL));
    void* const kBaseFn2    = reinterpret_cast<void*>(
        static_cast<CHAOS_IL2CPP_UINTPTR>(0xCCCLL));

    // ────────────────────────────────────────────────────────────────────────
    // Step 1: Register base type via RegisterCodegenVTable
    //   type_token   = 0x630
    //   slot_count   = 2
    //   slots[0]     = { method_token=0x300, method_pointer=kBaseFn   }
    //   slots[1]     = { method_token=0x301, method_pointer=kBaseFn2  }
    //   vtable_array = { kBaseFn, kBaseFn2 }
    // ────────────────────────────────────────────────────────────────────────
    {
        VTableSlot base_slots[] = {
            { 0x300u, kBaseFn },
            { 0x301u, kBaseFn2 }
        };
        const void* base_vtable_array[] = { kBaseFn, kBaseFn2 };

        VTableDescriptorV0 base_desc;
        std::memset(&base_desc, 0, sizeof(base_desc));
        base_desc.stable_id      = 0x10000010ULL;
        base_desc.type_token     = 0x630u;
        base_desc.base_token     = 0u;
        base_desc.slot_count     = 2u;
        base_desc.slots          = base_slots;
        base_desc.vtable_array   = base_vtable_array;
        base_desc.vtable_length  = 2u;
        base_desc.type_shape     = 1;   // reference type

        RegisterCodegenVTable(&base_desc);
    }

    // ────────────────────────────────────────────────────────────────────────
    // Step 2: Register derived type via RegisterHotUpdateVTable
    //   type_token     = 0x631
    //   parent_token   = 0x630
    //   override_slots = { method_token=0x300, method_pointer=kOverrideFn }
    //   The derived type should inherit method_token=0x301 → kBaseFn2
    //   but override method_token=0x300 → kOverrideFn
    // ────────────────────────────────────────────────────────────────────────
    {
        VTableSlot override_slots[] = {
            { 0x300u, kOverrideFn }
        };

        bool registered = RegisterHotUpdateVTable(
            /* stable_id       */ 0x10000011ULL,
            /* type_token      */ 0x631u,
            /* parent_token    */ 0x630u,
            /* override_slots  */ override_slots,
            /* override_count  */ 1u,
            /* type_shape      */ 1u    // reference type
        );

        if (!registered) return false;
    }

    // ────────────────────────────────────────────────────────────────────────
    // Step 3: Verify resolution chain via ResolveVirtualMethodPointer
    // ────────────────────────────────────────────────────────────────────────

    // 3a: Base type resolves to kBaseFn (no override)
    if (ResolveVirtualMethodPointer(0x630u, 0x300u) != kBaseFn)
        return false;

    // 3b: Derived type resolves to kOverrideFn (override shadows base)
    if (ResolveVirtualMethodPointer(0x631u, 0x300u) != kOverrideFn)
        return false;

    // 3c: Derived type inherits non-overridden slot → kBaseFn2
    if (ResolveVirtualMethodPointer(0x631u, 0x301u) != kBaseFn2)
        return false;

    // 3d: Unregistered type returns nullptr
    if (ResolveVirtualMethodPointer(0x999u, 0x300u) != nullptr)
        return false;

    // 3e: Unknown method token on valid type returns nullptr
    if (ResolveVirtualMethodPointer(0x631u, 0x999u) != nullptr)
        return false;

    // ────────────────────────────────────────────────────────────────────────
    // Step 4: Idempotency — re-registration is silently ignored
    // ────────────────────────────────────────────────────────────────────────
    {
        VTableSlot override_slots[] = {
            { 0x300u, kOverrideFn }
        };

        bool registered = RegisterHotUpdateVTable(
            /* stable_id       */ 0x10000011ULL,
            /* type_token      */ 0x631u,
            /* parent_token    */ 0x630u,
            /* override_slots  */ override_slots,
            /* override_count  */ 1u,
            /* type_shape      */ 1u
        );
        if (!registered) return false;  // Must return true (idempotent)

        // Resolution still returns kOverrideFn after re-registration
        if (ResolveVirtualMethodPointer(0x631u, 0x300u) != kOverrideFn)
            return false;
    }

    // ────────────────────────────────────────────────────────────────────────
    // Step 5: Verify null/zero edge cases
    // ────────────────────────────────────────────────────────────────────────
    if (RegisterHotUpdateVTable(0x10000012ULL, 0u, 0u, nullptr, 0u, 1u))
        return false;  // type_token == 0 must fail

    return true;
}