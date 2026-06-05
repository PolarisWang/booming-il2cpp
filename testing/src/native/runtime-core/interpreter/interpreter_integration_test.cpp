/// interpreter_integration_test.cpp — Integration tests for the IR->execution pipeline (gtest)
///
/// Tests: arithmetic ops, branches, bitwise ops, CallVirt/VTable dispatch, SEH,
/// runtime-generic-method simulation, struct return values, dispatch callbacks,
/// float/interface features, codegen VTable path, hot-update VTable resolution,
/// interface offset cache, and method replacement dispatch.
///
/// Merged from: interpreter_integration.cpp + method_replacement_test.cpp

#include "interpreter_vm.h"
#include "vtable_registry.h"
#include "token_resolver.h"
#include "hotpatch_table.h"
#include "method_replacement.h"

#include <chaos/common.h>
#include <chaos/type_info.h>
#include <codegen_bridge.h>

#include <gtest/gtest.h>

#include <cstring>
#include <cstdint>

// Stubs for symbols normally emitted by codegen
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module = nullptr;
extern "C" const char* kChaosExternalRuntimeSubjects[1] = { nullptr };
extern "C" void* kChaosExternalRuntimeFnTable[1] = { nullptr };
extern "C" int32_t kChaosExternalRuntimeCount = 0;

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
// Basic opcode tests
// ════════════════════════════════════════════════════════════════════════════

static IRMethod MakeSimpleMethod(IROpCode op, CHAOS_IL2CPP_INT32 imm = 0) {
    IRMethod method;
    IRInstruction i; i.op_code = op; i.immediate_i4 = imm; method.instructions.push_back(i);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);
    return method;
}

TEST(Interpreter_Integration, LdcI4_0) {
    ExecutionFrame frame;
    const InterpreterVM vm = {};
    IRMethod method = MakeSimpleMethod(IROpCode::LdcI4, 0);
    ExecutionResult result = vm.Execute(method, &frame);
    EXPECT_TRUE(result.has_return_value);
    EXPECT_EQ(result.int32_value, 0);
}

TEST(Interpreter_Integration, LdcI4_S) {
    IRMethod method = MakeSimpleMethod(IROpCode::LdcI4, 42);
    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    EXPECT_TRUE(result.has_return_value);
    EXPECT_EQ(result.int32_value, 42);
}

TEST(Interpreter_Integration, LdcI4_2_3_Add) {
    IRMethod method;
    IRInstruction i1; i1.op_code = IROpCode::LdcI4; i1.immediate_i4 = 2; method.instructions.push_back(i1);
    IRInstruction i2; i2.op_code = IROpCode::LdcI4; i2.immediate_i4 = 3; method.instructions.push_back(i2);
    IRInstruction add; add.op_code = IROpCode::Add; method.instructions.push_back(add);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    EXPECT_TRUE(result.has_return_value);
    EXPECT_EQ(result.int32_value, 5);
}

TEST(Interpreter_Integration, LdArgAdd) {
    IRMethod method;
    IRInstruction a0; a0.op_code = IROpCode::LdArg; a0.operand_index = 0; method.instructions.push_back(a0);
    IRInstruction a1; a1.op_code = IROpCode::LdArg; a1.operand_index = 1; method.instructions.push_back(a1);
    IRInstruction add; add.op_code = IROpCode::Add; method.instructions.push_back(add);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame;
    frame.arguments.push_back(InterpreterValue::from_i32(10));
    frame.arguments.push_back(InterpreterValue::from_i32(32));
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    EXPECT_TRUE(result.has_return_value);
    EXPECT_EQ(result.int32_value, 42);
}

TEST(Interpreter_Integration, StLocLdLoc) {
    IRMethod method;
    IRInstruction push; push.op_code = IROpCode::LdcI4; push.immediate_i4 = 42; method.instructions.push_back(push);
    IRInstruction st;   st.op_code = IROpCode::StLoc; st.operand_index = 0; method.instructions.push_back(st);
    IRInstruction ld;   ld.op_code = IROpCode::LdLoc; ld.operand_index = 0; method.instructions.push_back(ld);
    IRInstruction ret;  ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    EXPECT_TRUE(result.has_return_value);
    EXPECT_EQ(result.int32_value, 42);
}

TEST(Interpreter_Integration, Sub) {
    IRMethod method;
    IRInstruction a; a.op_code = IROpCode::LdcI4; a.immediate_i4 = 10; method.instructions.push_back(a);
    IRInstruction b; b.op_code = IROpCode::LdcI4; b.immediate_i4 = 3; method.instructions.push_back(b);
    IRInstruction sub; sub.op_code = IROpCode::Sub; method.instructions.push_back(sub);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    EXPECT_TRUE(result.has_return_value);
    EXPECT_EQ(result.int32_value, 7);
}

TEST(Interpreter_Integration, Mul) {
    IRMethod method;
    IRInstruction a; a.op_code = IROpCode::LdcI4; a.immediate_i4 = 6; method.instructions.push_back(a);
    IRInstruction b; b.op_code = IROpCode::LdcI4; b.immediate_i4 = 7; method.instructions.push_back(b);
    IRInstruction mul; mul.op_code = IROpCode::Mul; method.instructions.push_back(mul);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    EXPECT_TRUE(result.has_return_value);
    EXPECT_EQ(result.int32_value, 42);
}

TEST(Interpreter_Integration, LdNull) {
    IRMethod method;
    IRInstruction null; null.op_code = IROpCode::LdNull; method.instructions.push_back(null);
    IRInstruction pop; pop.op_code = IROpCode::Pop; method.instructions.push_back(pop);
    IRInstruction push; push.op_code = IROpCode::LdcI4; push.immediate_i4 = 0; method.instructions.push_back(push);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    EXPECT_TRUE(result.has_return_value);
    EXPECT_EQ(result.int32_value, 0);
}

// ════════════════════════════════════════════════════════════════════════════
// Branch tests
// ════════════════════════════════════════════════════════════════════════════

TEST(Interpreter_Integration, BrTrue_Taken) {
    IRMethod method;
    IRInstruction push1; push1.op_code = IROpCode::LdcI4; push1.immediate_i4 = 1; method.instructions.push_back(push1);
    IRInstruction br;    br.op_code = IROpCode::BrTrue; br.branch_target = 4; method.instructions.push_back(br);
    IRInstruction push0; push0.op_code = IROpCode::LdcI4; push0.immediate_i4 = 0; method.instructions.push_back(push0);
    IRInstruction ret1;  ret1.op_code = IROpCode::Ret; method.instructions.push_back(ret1);
    IRInstruction push42; push42.op_code = IROpCode::LdcI4; push42.immediate_i4 = 42; method.instructions.push_back(push42);
    IRInstruction ret2;   ret2.op_code = IROpCode::Ret; method.instructions.push_back(ret2);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    EXPECT_TRUE(result.has_return_value);
    EXPECT_EQ(result.int32_value, 42);
}

TEST(Interpreter_Integration, BrTrue_NotTaken) {
    IRMethod method;
    IRInstruction push0; push0.op_code = IROpCode::LdcI4; push0.immediate_i4 = 0; method.instructions.push_back(push0);
    IRInstruction br;    br.op_code = IROpCode::BrTrue; br.branch_target = 4; method.instructions.push_back(br);
    IRInstruction push1; push1.op_code = IROpCode::LdcI4; push1.immediate_i4 = 1; method.instructions.push_back(push1);
    IRInstruction ret;   ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);
    IRInstruction push42; push42.op_code = IROpCode::LdcI4; push42.immediate_i4 = 42; method.instructions.push_back(push42);
    IRInstruction ret2;   ret2.op_code = IROpCode::Ret; method.instructions.push_back(ret2);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    EXPECT_TRUE(result.has_return_value);
    EXPECT_EQ(result.int32_value, 1);
}

TEST(Interpreter_Integration, Beq_Equal) {
    IRMethod method;
    IRInstruction a0; a0.op_code = IROpCode::LdArg; a0.operand_index = 0; method.instructions.push_back(a0);
    IRInstruction a1; a1.op_code = IROpCode::LdArg; a1.operand_index = 1; method.instructions.push_back(a1);
    IRInstruction beq; beq.op_code = IROpCode::Beq; beq.branch_target = 5; method.instructions.push_back(beq);
    IRInstruction push0; push0.op_code = IROpCode::LdcI4; push0.immediate_i4 = 0; method.instructions.push_back(push0);
    IRInstruction ret1;  ret1.op_code = IROpCode::Ret; method.instructions.push_back(ret1);
    IRInstruction push1; push1.op_code = IROpCode::LdcI4; push1.immediate_i4 = 1; method.instructions.push_back(push1);
    IRInstruction ret2;  ret2.op_code = IROpCode::Ret; method.instructions.push_back(ret2);

    ExecutionFrame frame;
    frame.arguments.push_back(InterpreterValue::from_i32(42));
    frame.arguments.push_back(InterpreterValue::from_i32(42));
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    EXPECT_TRUE(result.has_return_value);
    EXPECT_EQ(result.int32_value, 1);
}

TEST(Interpreter_Integration, Beq_NotEqual) {
    IRMethod method;
    IRInstruction a0; a0.op_code = IROpCode::LdArg; a0.operand_index = 0; method.instructions.push_back(a0);
    IRInstruction a1; a1.op_code = IROpCode::LdArg; a1.operand_index = 1; method.instructions.push_back(a1);
    IRInstruction beq; beq.op_code = IROpCode::Beq; beq.branch_target = 5; method.instructions.push_back(beq);
    IRInstruction push0; push0.op_code = IROpCode::LdcI4; push0.immediate_i4 = 0; method.instructions.push_back(push0);
    IRInstruction ret1;  ret1.op_code = IROpCode::Ret; method.instructions.push_back(ret1);
    IRInstruction push1; push1.op_code = IROpCode::LdcI4; push1.immediate_i4 = 1; method.instructions.push_back(push1);
    IRInstruction ret2;  ret2.op_code = IROpCode::Ret; method.instructions.push_back(ret2);

    ExecutionFrame frame;
    frame.arguments.push_back(InterpreterValue::from_i32(10));
    frame.arguments.push_back(InterpreterValue::from_i32(99));
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    EXPECT_TRUE(result.has_return_value);
    EXPECT_EQ(result.int32_value, 0);
}

// ════════════════════════════════════════════════════════════════════════════
// Phase A: New opcode tests
// ════════════════════════════════════════════════════════════════════════════

TEST(Interpreter_Integration, Dup) {
    IRMethod method;
    IRInstruction p5; p5.op_code = IROpCode::LdcI4; p5.immediate_i4 = 5; method.instructions.push_back(p5);
    IRInstruction p3; p3.op_code = IROpCode::LdcI4; p3.immediate_i4 = 3; method.instructions.push_back(p3);
    IRInstruction dup; dup.op_code = IROpCode::Dup; method.instructions.push_back(dup);
    IRInstruction a1; a1.op_code = IROpCode::Add; method.instructions.push_back(a1);
    IRInstruction a2; a2.op_code = IROpCode::Add; method.instructions.push_back(a2);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    EXPECT_TRUE(result.has_return_value);
    EXPECT_EQ(result.int32_value, 11);
}

TEST(Interpreter_Integration, DivUn) {
    IRMethod method;
    IRInstruction a; a.op_code = IROpCode::LdcI4; a.immediate_i4 = -1; method.instructions.push_back(a);
    IRInstruction b; b.op_code = IROpCode::LdcI4; b.immediate_i4 = 2; method.instructions.push_back(b);
    IRInstruction du; du.op_code = IROpCode::DivUn; method.instructions.push_back(du);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    EXPECT_TRUE(result.has_return_value);
    EXPECT_EQ(result.int32_value, 2147483647);
}

TEST(Interpreter_Integration, RemUn) {
    IRMethod method;
    IRInstruction a; a.op_code = IROpCode::LdcI4; a.immediate_i4 = -1; method.instructions.push_back(a);
    IRInstruction b; b.op_code = IROpCode::LdcI4; b.immediate_i4 = 3; method.instructions.push_back(b);
    IRInstruction ru; ru.op_code = IROpCode::RemUn; method.instructions.push_back(ru);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    EXPECT_TRUE(result.has_return_value);
    EXPECT_EQ(result.int32_value, 0);
}

TEST(Interpreter_Integration, BitwiseAnd) {
    IRMethod method;
    IRInstruction a; a.op_code = IROpCode::LdcI4; a.immediate_i4 = 3; method.instructions.push_back(a);
    IRInstruction b; b.op_code = IROpCode::LdcI4; b.immediate_i4 = 6; method.instructions.push_back(b);
    IRInstruction and_instr; and_instr.op_code = IROpCode::And; method.instructions.push_back(and_instr);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    EXPECT_TRUE(result.has_return_value);
    EXPECT_EQ(result.int32_value, 2);
}

TEST(Interpreter_Integration, BitwiseOr) {
    IRMethod method;
    IRInstruction a; a.op_code = IROpCode::LdcI4; a.immediate_i4 = 3; method.instructions.push_back(a);
    IRInstruction b; b.op_code = IROpCode::LdcI4; b.immediate_i4 = 6; method.instructions.push_back(b);
    IRInstruction or_instr; or_instr.op_code = IROpCode::Or; method.instructions.push_back(or_instr);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    EXPECT_TRUE(result.has_return_value);
    EXPECT_EQ(result.int32_value, 7);
}

TEST(Interpreter_Integration, BitwiseXor) {
    IRMethod method;
    IRInstruction a; a.op_code = IROpCode::LdcI4; a.immediate_i4 = 3; method.instructions.push_back(a);
    IRInstruction b; b.op_code = IROpCode::LdcI4; b.immediate_i4 = 6; method.instructions.push_back(b);
    IRInstruction xor_instr; xor_instr.op_code = IROpCode::Xor; method.instructions.push_back(xor_instr);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    EXPECT_TRUE(result.has_return_value);
    EXPECT_EQ(result.int32_value, 5);
}

TEST(Interpreter_Integration, BitwiseNot) {
    IRMethod method;
    IRInstruction a; a.op_code = IROpCode::LdcI4; a.immediate_i4 = 0; method.instructions.push_back(a);
    IRInstruction not_instr; not_instr.op_code = IROpCode::Not; method.instructions.push_back(not_instr);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    EXPECT_TRUE(result.has_return_value);
    EXPECT_EQ(result.int32_value, -1);
}

TEST(Interpreter_Integration, ShiftLeft) {
    IRMethod method;
    IRInstruction a; a.op_code = IROpCode::LdcI4; a.immediate_i4 = 3; method.instructions.push_back(a);
    IRInstruction b; b.op_code = IROpCode::LdcI4; b.immediate_i4 = 2; method.instructions.push_back(b);
    IRInstruction shl; shl.op_code = IROpCode::Shl; method.instructions.push_back(shl);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    EXPECT_TRUE(result.has_return_value);
    EXPECT_EQ(result.int32_value, 12);
}

TEST(Interpreter_Integration, ShiftRightArith) {
    IRMethod method;
    IRInstruction a; a.op_code = IROpCode::LdcI4; a.immediate_i4 = -8; method.instructions.push_back(a);
    IRInstruction b; b.op_code = IROpCode::LdcI4; b.immediate_i4 = 2; method.instructions.push_back(b);
    IRInstruction shr; shr.op_code = IROpCode::Shr; method.instructions.push_back(shr);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    EXPECT_TRUE(result.has_return_value);
    EXPECT_EQ(result.int32_value, -2);
}

TEST(Interpreter_Integration, ShiftRightLogical) {
    IRMethod method;
    IRInstruction a; a.op_code = IROpCode::LdcI4; a.immediate_i4 = -8; method.instructions.push_back(a);
    IRInstruction b; b.op_code = IROpCode::LdcI4; b.immediate_i4 = 2; method.instructions.push_back(b);
    IRInstruction shru; shru.op_code = IROpCode::ShrUn; method.instructions.push_back(shru);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    EXPECT_TRUE(result.has_return_value);
    EXPECT_EQ(result.int32_value, 1073741822);
}

TEST(Interpreter_Integration, ConvRUn) {
    IRMethod method;
    IRInstruction a; a.op_code = IROpCode::LdcI4; a.immediate_i4 = 3; method.instructions.push_back(a);
    IRInstruction cru; cru.op_code = IROpCode::ConvRUn; method.instructions.push_back(cru);
    IRInstruction ci; ci.op_code = IROpCode::Conv_I4; method.instructions.push_back(ci);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    EXPECT_TRUE(result.has_return_value);
    EXPECT_EQ(result.int32_value, 3);
}

TEST(Interpreter_Integration, ConvI) {
    IRMethod method;
    IRInstruction push; push.op_code = IROpCode::LdcI4; push.immediate_i4 = 5; method.instructions.push_back(push);
    IRInstruction ci; ci.op_code = IROpCode::ConvI; method.instructions.push_back(ci);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    EXPECT_TRUE(result.has_return_value);
    EXPECT_EQ(result.int32_value, 5);
}

TEST(Interpreter_Integration, ConvU) {
    IRMethod method;
    IRInstruction push; push.op_code = IROpCode::LdcI4; push.immediate_i4 = 5; method.instructions.push_back(push);
    IRInstruction cu; cu.op_code = IROpCode::ConvU; method.instructions.push_back(cu);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    EXPECT_TRUE(result.has_return_value);
    EXPECT_EQ(result.int32_value, 5);
}

TEST(Interpreter_Integration, SizeOf) {
    IRMethod method;
    IRInstruction so; so.op_code = IROpCode::SizeOf; so.immediate_i4 = static_cast<CHAOS_IL2CPP_INT32>(0x12345u); method.instructions.push_back(so);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    EXPECT_TRUE(result.has_return_value);
    EXPECT_EQ(result.int32_value, static_cast<CHAOS_IL2CPP_INT32>(0x12345u));
}

TEST(Interpreter_Integration, ExtendedLdFtn) {
    IRMethod method;
    IRInstruction ldftn; ldftn.op_code = IROpCode::LdFtn; ldftn.call_target = reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0xCCDDAABBu)); method.instructions.push_back(ldftn);
    IRInstruction pop; pop.op_code = IROpCode::Pop; method.instructions.push_back(pop);
    IRInstruction push; push.op_code = IROpCode::LdcI4; push.immediate_i4 = 1; method.instructions.push_back(push);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    EXPECT_TRUE(result.has_return_value);
    EXPECT_EQ(result.int32_value, 1);
}

TEST(Interpreter_Integration, ExtendedLdArg) {
    IRMethod method;
    IRInstruction ldarg; ldarg.op_code = IROpCode::LdArg; ldarg.operand_index = 0; method.instructions.push_back(ldarg);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame;
    frame.arguments.push_back(InterpreterValue::from_i32(99));
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    EXPECT_TRUE(result.has_return_value);
    EXPECT_EQ(result.int32_value, 99);
}

TEST(Interpreter_Integration, ExtendedLdLoc) {
    IRMethod method;
    IRInstruction push; push.op_code = IROpCode::LdcI4; push.immediate_i4 = 42; method.instructions.push_back(push);
    IRInstruction st; st.op_code = IROpCode::StLoc; st.operand_index = 0; method.instructions.push_back(st);
    IRInstruction ld; ld.op_code = IROpCode::LdLoc; ld.operand_index = 0; method.instructions.push_back(ld);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    EXPECT_TRUE(result.has_return_value);
    EXPECT_EQ(result.int32_value, 42);
}

// ════════════════════════════════════════════════════════════════════════════
// CallVirt / VTable dispatch tests
// ════════════════════════════════════════════════════════════════════════════

TEST(Interpreter_Integration, CallVirtDirectResolution) {
    using namespace chaos::il2cpp::vtable_registry;

    static VTableSlot base_slots[] = {
        { 0x200u, reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0xBEEFu)) }
    };
    static TypeVTable base_vtable = {};
    base_vtable.type_token = 0x100u;
    base_vtable.base_token = 0u;
    base_vtable.slot_count = 1u;
    base_vtable.slots = base_slots;
    RegisterTypeVTable(&base_vtable);

    IRMethod method;
    IRInstruction ldarg; ldarg.op_code = IROpCode::LdArg; ldarg.operand_index = 0; method.instructions.push_back(ldarg);
    IRInstruction callvirt; callvirt.op_code = IROpCode::CallVirt;
    callvirt.secondary_index = static_cast<CHAOS_IL2CPP_SIZE>(0x200u);
    callvirt.arg_count = 1u;
    method.instructions.push_back(callvirt);

    auto* storage = new InterpreterObject();
    storage->type_token = 0x100u;
    storage->fields.resize(1u);

    ExecutionFrame frame;
    frame.arguments.push_back(InterpreterValue::from_obj(storage));

    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);

    EXPECT_TRUE(result.needs_external_dispatch);
    EXPECT_EQ(result.call_target, reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0xBEEFu)));
}

TEST(Interpreter_Integration, CallVirtInheritanceChain) {
    using namespace chaos::il2cpp::vtable_registry;

    static VTableSlot base_slots[] = {
        { 0x200u, reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0xBEEFu)) }
    };
    static TypeVTable base_vtable = {};
    base_vtable.type_token = 0x100u;
    base_vtable.base_token = 0u;
    base_vtable.slot_count = 1u;
    base_vtable.slots = base_slots;
    RegisterTypeVTable(&base_vtable);

    static VTableSlot derived_slots[] = {
        { 0x200u, reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0xCAFEu)) }
    };
    static TypeVTable derived_vtable = {};
    derived_vtable.type_token = 0x101u;
    derived_vtable.base_token = 0x100u;
    derived_vtable.slot_count = 1u;
    derived_vtable.slots = derived_slots;
    RegisterTypeVTable(&derived_vtable);

    IRMethod method;
    IRInstruction ldarg; ldarg.op_code = IROpCode::LdArg; ldarg.operand_index = 0; method.instructions.push_back(ldarg);
    IRInstruction callvirt; callvirt.op_code = IROpCode::CallVirt;
    callvirt.secondary_index = static_cast<CHAOS_IL2CPP_SIZE>(0x200u);
    callvirt.arg_count = 1u;
    method.instructions.push_back(callvirt);

    auto* storage = new InterpreterObject();
    storage->type_token = 0x101u;
    storage->fields.resize(1u);

    ExecutionFrame frame;
    frame.arguments.push_back(InterpreterValue::from_obj(storage));

    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);

    EXPECT_TRUE(result.needs_external_dispatch);
    EXPECT_EQ(result.call_target, reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0xCAFEu)));
}

// ════════════════════════════════════════════════════════════════════════════
// Codegen VTable path tests
// ════════════════════════════════════════════════════════════════════════════

TEST(Interpreter_Integration, CodegenVTableDirect) {
    using namespace chaos::il2cpp::vtable_registry;

    static VTableSlot slots[] = {
        { 0x200u, reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0xBEEFu)) }
    };
    static const void* vtable_array[] = { reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0xBEEFu)) };

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

    IRMethod method;
    IRInstruction ldarg; ldarg.op_code = IROpCode::LdArg; ldarg.operand_index = 0; method.instructions.push_back(ldarg);
    IRInstruction callvirt; callvirt.op_code = IROpCode::CallVirt;
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

    EXPECT_TRUE(result.needs_external_dispatch);
    EXPECT_EQ(result.call_target, reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0xBEEFu)));
}

TEST(Interpreter_Integration, CodegenVTableInheritance) {
    using namespace chaos::il2cpp::vtable_registry;

    static VTableSlot base_slots[] = {
        { 0x200u, reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0xBEEFu)) }
    };
    static const void* base_vtable_array[] = { reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0xBEEFu)) };

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

    static VTableSlot derived_slots[] = {
        { 0x200u, reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0xCAFEu)) }
    };
    static const void* derived_vtable_array[] = { reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0xCAFEu)) };

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

    IRMethod method;
    IRInstruction ldarg; ldarg.op_code = IROpCode::LdArg; ldarg.operand_index = 0; method.instructions.push_back(ldarg);
    IRInstruction callvirt; callvirt.op_code = IROpCode::CallVirt;
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

    EXPECT_TRUE(result.needs_external_dispatch);
    EXPECT_EQ(result.call_target, reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0xCAFEu)));
}

TEST(Interpreter_Integration, CodegenVTableInterfaceDispatch) {
    using namespace chaos::il2cpp::vtable_registry;

    const CHAOS_IL2CPP_UINT64 kIfaceStable = 0xABCD0001ULL;

    static chaos::il2cpp::common::InterfaceMapEntry iface_entries[] = {
        { kIfaceStable, 1u, 2u }
    };

    alignas(64) static char s_method0[64];
    alignas(64) static char s_method1[64];
    alignas(64) static char s_method2[64];
    static const void* vtable_array[] = {
        reinterpret_cast<void*>(&s_method0),
        reinterpret_cast<void*>(&s_method1),
        reinterpret_cast<void*>(&s_method2),
    };

    static VTableSlot obj_slots[] = {
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

    IRMethod method;
    IRInstruction ldarg; ldarg.op_code = IROpCode::LdArg; ldarg.operand_index = 0; method.instructions.push_back(ldarg);
    IRInstruction callvirt; callvirt.op_code = IROpCode::CallVirt;
    callvirt.secondary_index = static_cast<CHAOS_IL2CPP_SIZE>(0u);
    callvirt.arg_count = 1u;
    method.instructions.push_back(callvirt);

    auto* obj = new InterpreterObject();
    obj->type_token = 0x620u;

    ExecutionFrame frame;
    frame.arguments.push_back(InterpreterValue::from_obj(obj));

    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);

    EXPECT_TRUE(result.needs_external_dispatch);
    EXPECT_EQ(result.call_target, reinterpret_cast<void*>(&s_method1));
}

// ════════════════════════════════════════════════════════════════════════════
// Hot-update VTable path test
// ════════════════════════════════════════════════════════════════════════════

TEST(Interpreter_Integration, HotUpdateVTableResolution) {
    using namespace chaos::il2cpp::vtable_registry;

    void* const kBaseFn     = reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0xAAALL));
    void* const kOverrideFn = reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0xBBBLL));
    void* const kBaseFn2    = reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0xCCCLL));

    // Register base type
    {
        static VTableSlot base_slots[] = { { 0x300u, kBaseFn }, { 0x301u, kBaseFn2 } };
        static const void* base_vtable_array[] = { kBaseFn, kBaseFn2 };
        VTableDescriptorV0 base_desc;
        std::memset(&base_desc, 0, sizeof(base_desc));
        base_desc.stable_id      = 0x10000010ULL;
        base_desc.type_token     = 0x630u;
        base_desc.base_token     = 0u;
        base_desc.slot_count     = 2u;
        base_desc.slots          = base_slots;
        base_desc.vtable_array   = base_vtable_array;
        base_desc.vtable_length  = 2u;
        base_desc.type_shape     = 1;
        RegisterCodegenVTable(&base_desc);
    }

    // Register derived via RegisterHotUpdateVTable
    {
        static VTableSlot override_slots[] = { { 0x300u, kOverrideFn } };
        bool registered = RegisterHotUpdateVTable(
            0x10000011ULL, 0x631u, 0x630u, override_slots, 1u, 1u);
        ASSERT_TRUE(registered);
    }

    EXPECT_EQ(ResolveVirtualMethodPointer(0x630u, 0x300u), kBaseFn);
    EXPECT_EQ(ResolveVirtualMethodPointer(0x631u, 0x300u), kOverrideFn);
    EXPECT_EQ(ResolveVirtualMethodPointer(0x631u, 0x301u), kBaseFn2);
    EXPECT_EQ(ResolveVirtualMethodPointer(0x999u, 0x300u), nullptr);
    EXPECT_EQ(ResolveVirtualMethodPointer(0x631u, 0x999u), nullptr);

    // Idempotency
    {
        static VTableSlot override_slots[] = { { 0x300u, kOverrideFn } };
        EXPECT_TRUE(RegisterHotUpdateVTable(0x10000011ULL, 0x631u, 0x630u, override_slots, 1u, 1u));
        EXPECT_EQ(ResolveVirtualMethodPointer(0x631u, 0x300u), kOverrideFn);
    }

    EXPECT_FALSE(RegisterHotUpdateVTable(0x10000012ULL, 0u, 0u, nullptr, 0u, 1u));
}

// ════════════════════════════════════════════════════════════════════════════
// SEH tests
// ════════════════════════════════════════════════════════════════════════════

TEST(Interpreter_Integration, ThrowUnhandled) {
    IRMethod method;
    IRInstruction push; push.op_code = IROpCode::LdcI4; push.immediate_i4 = 1; method.instructions.push_back(push);
    IRInstruction throw_insn; throw_insn.op_code = IROpCode::Throw; method.instructions.push_back(throw_insn);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    EXPECT_TRUE(result.threw_exception);
}

TEST(Interpreter_Integration, ThrowCatch) {
    IRMethod method;

    IRInstruction push; push.op_code = IROpCode::LdcI4; push.immediate_i4 = 42; method.instructions.push_back(push);
    IRInstruction throw_insn; throw_insn.op_code = IROpCode::Throw; method.instructions.push_back(throw_insn);
    IRInstruction skipped_push; skipped_push.op_code = IROpCode::LdcI4; skipped_push.immediate_i4 = 0; method.instructions.push_back(skipped_push);
    IRInstruction skipped_ret; skipped_ret.op_code = IROpCode::Ret; method.instructions.push_back(skipped_ret);
    IRInstruction handler_push; handler_push.op_code = IROpCode::LdcI4; handler_push.immediate_i4 = 1; method.instructions.push_back(handler_push);
    IRInstruction handler_ret; handler_ret.op_code = IROpCode::Ret; method.instructions.push_back(handler_ret);

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

    EXPECT_FALSE(result.threw_exception);
    EXPECT_TRUE(result.has_return_value);
    EXPECT_EQ(result.int32_value, 1);
}

TEST(Interpreter_Integration, ThrowFinallyUnwind) {
    IRMethod method;

    IRInstruction push; push.op_code = IROpCode::LdcI4; push.immediate_i4 = 42; method.instructions.push_back(push);
    IRInstruction throw_insn; throw_insn.op_code = IROpCode::Throw; method.instructions.push_back(throw_insn);
    IRInstruction skipped; skipped.op_code = IROpCode::LdcI4; skipped.immediate_i4 = 0; method.instructions.push_back(skipped);
    IRInstruction skipped_ret; skipped_ret.op_code = IROpCode::Ret; method.instructions.push_back(skipped_ret);
    IRInstruction endfinally; endfinally.op_code = IROpCode::EndFinally; method.instructions.push_back(endfinally);
    IRInstruction catch_push; catch_push.op_code = IROpCode::LdcI4; catch_push.immediate_i4 = 1; method.instructions.push_back(catch_push);
    IRInstruction catch_ret; catch_ret.op_code = IROpCode::Ret; method.instructions.push_back(catch_ret);

    SEHClause finally_clause = {};
    finally_clause.flags = SEHFlags::Finally;
    finally_clause.try_start_idx = 0;
    finally_clause.try_end_idx = 2;
    finally_clause.handler_start_idx = 4;
    finally_clause.handler_end_idx = 5;
    method.seh_clauses.push_back(finally_clause);

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

    EXPECT_FALSE(result.threw_exception);
    EXPECT_TRUE(result.has_return_value);
    EXPECT_EQ(result.int32_value, 1);
}

TEST(Interpreter_Integration, LeaveFinally) {
    IRMethod method;

    IRInstruction leave; leave.op_code = IROpCode::Leave; leave.branch_target = 3; method.instructions.push_back(leave);
    IRInstruction skipped; skipped.op_code = IROpCode::LdcI4; skipped.immediate_i4 = 0; method.instructions.push_back(skipped);
    IRInstruction skipped_ret; skipped_ret.op_code = IROpCode::Ret; method.instructions.push_back(skipped_ret);
    IRInstruction target_push; target_push.op_code = IROpCode::LdcI4; target_push.immediate_i4 = 1; method.instructions.push_back(target_push);
    IRInstruction target_ret; target_ret.op_code = IROpCode::Ret; method.instructions.push_back(target_ret);
    IRInstruction endfinally; endfinally.op_code = IROpCode::EndFinally; method.instructions.push_back(endfinally);

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

    EXPECT_FALSE(result.threw_exception);
    EXPECT_TRUE(result.has_return_value);
    EXPECT_EQ(result.int32_value, 1);
}

TEST(Interpreter_Integration, Rethrow) {
    IRMethod method;

    IRInstruction push; push.op_code = IROpCode::LdcI4; push.immediate_i4 = 42; method.instructions.push_back(push);
    IRInstruction throw_insn; throw_insn.op_code = IROpCode::Throw; method.instructions.push_back(throw_insn);
    IRInstruction skipped; skipped.op_code = IROpCode::LdcI4; skipped.immediate_i4 = 0; method.instructions.push_back(skipped);
    IRInstruction skipped_ret; skipped_ret.op_code = IROpCode::Ret; method.instructions.push_back(skipped_ret);
    IRInstruction rethrow; rethrow.op_code = IROpCode::Rethrow; method.instructions.push_back(rethrow);
    IRInstruction unreachable_ret; unreachable_ret.op_code = IROpCode::Ret; method.instructions.push_back(unreachable_ret);

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

    EXPECT_TRUE(result.threw_exception);
}

// ════════════════════════════════════════════════════════════════════════════
// Runtime-generic-method simulation tests
// ════════════════════════════════════════════════════════════════════════════

TEST(Interpreter_Integration, RuntimeMethodExecute) {
    IRMethod method;
    IRInstruction a0; a0.op_code = IROpCode::LdArg; a0.operand_index = 0; method.instructions.push_back(a0);
    IRInstruction a1; a1.op_code = IROpCode::LdArg; a1.operand_index = 1; method.instructions.push_back(a1);
    IRInstruction add; add.op_code = IROpCode::Add; method.instructions.push_back(add);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame;
    frame.arguments.push_back(InterpreterValue::from_i32(10));
    frame.arguments.push_back(InterpreterValue::from_i32(20));

    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);

    EXPECT_TRUE(result.has_return_value);
    EXPECT_EQ(result.return_value.tag, ValueTag::Int32);
    EXPECT_EQ(result.return_value.i32, 30);
}

TEST(Interpreter_Integration, RuntimeMethodReturnValueDispatch) {
    IRMethod method = MakeSimpleMethod(IROpCode::LdcI4, 42);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);

    ASSERT_TRUE(result.has_return_value);
    EXPECT_EQ(result.return_value.tag, ValueTag::Int32);
    EXPECT_EQ(result.return_value.i32, 42);
}

TEST(Interpreter_Integration, RuntimeMethodTypeParamResolution) {
    TypeInfoHandle dummy_type = static_cast<TypeInfoHandle>(0xDEADBEEFu);

    auto ctx = TokenResolverContext();
    ctx.type_args = &dummy_type;
    ctx.arg_count = 1u;

    IRInstruction box_insn;
    box_insn.op_code = IROpCode::Box;

    bool resolved = DefaultTokenResolver(0x11000000u, box_insn, &ctx);
    ASSERT_TRUE(resolved);

    void* expected = reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0xDEADBEEFu));
    EXPECT_EQ(box_insn.call_target, expected);
}

TEST(Interpreter_Integration, RuntimeMethodExceptionPropagation) {
    IRMethod method;

    IRInstruction push; push.op_code = IROpCode::LdcI4; push.immediate_i4 = 99; method.instructions.push_back(push);
    IRInstruction throw_insn; throw_insn.op_code = IROpCode::Throw; method.instructions.push_back(throw_insn);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);

    EXPECT_TRUE(result.threw_exception);
    EXPECT_EQ(result.exception_value.tag, ValueTag::Int32);
    EXPECT_EQ(result.exception_value.i32, 99);
}

// ════════════════════════════════════════════════════════════════════════════
// Struct return value test
// ════════════════════════════════════════════════════════════════════════════

TEST(Interpreter_Integration, StructReturnValue) {
    IRMethod method;
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    struct TestStruct { int a; int b; };
    TestStruct data = { 42, 99 };
    constexpr auto kExpectedSize = static_cast<CHAOS_IL2CPP_UINT32>(sizeof(TestStruct));

    ExecutionFrame frame;
    frame.stack.push_back(InterpreterValue::from_struct(&data, kExpectedSize));

    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);

    ASSERT_TRUE(result.has_return_value);
    EXPECT_EQ(result.return_value.tag, ValueTag::Struct);
    EXPECT_EQ(result.return_value.struct_size, kExpectedSize);
    ASSERT_NE(result.return_value.obj, nullptr);

    const auto* out = static_cast<const TestStruct*>(result.return_value.obj);
    EXPECT_EQ(out->a, 42);
    EXPECT_EQ(out->b, 99);
}

// ════════════════════════════════════════════════════════════════════════════
// Dispatch callback integration tests
// ════════════════════════════════════════════════════════════════════════════

struct MockDispatchState {
    bool called = false;
    void* received_target = nullptr;
    CHAOS_IL2CPP_UINT32 received_arg_count = 0;
    bool received_is_instance = false;
    CHAOS_IL2CPP_VECTOR(InterpreterValue) captured_args;
    DispatchResult result_to_return = {};
};

static DispatchResult MockDispatchCallback(
    void* call_target, const InterpreterValue* call_args,
    CHAOS_IL2CPP_UINT32 arg_count, bool is_instance_call,
    void* dispatch_context) {
    auto* state = static_cast<MockDispatchState*>(dispatch_context);
    if (state == nullptr) return {};
    state->called = true;
    state->received_target = call_target;
    state->received_arg_count = arg_count;
    state->received_is_instance = is_instance_call;
    state->captured_args.assign(call_args, call_args + arg_count);
    return state->result_to_return;
}

TEST(Interpreter_Integration, DispatchBasic) {
    void* const kTestTarget = reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0x1234u));

    IRMethod method;
    IRInstruction call_insn; call_insn.op_code = IROpCode::Call;
    call_insn.call_target = kTestTarget; call_insn.arg_count = 0; call_insn.is_instance_call = false;
    method.instructions.push_back(call_insn);
    IRInstruction ret_insn; ret_insn.op_code = IROpCode::Ret; method.instructions.push_back(ret_insn);

    MockDispatchState state;
    state.result_to_return.has_value = true;
    state.result_to_return.value = InterpreterValue::from_i32(42);

    ExecutionFrame frame;
    frame.dispatch_fn = MockDispatchCallback;
    frame.dispatch_context = &state;

    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);

    EXPECT_TRUE(state.called);
    EXPECT_EQ(state.received_target, kTestTarget);
    EXPECT_EQ(state.received_arg_count, 0u);
    EXPECT_TRUE(result.has_return_value);
    EXPECT_EQ(result.return_value.tag, ValueTag::Int32);
    EXPECT_EQ(result.return_value.i32, 42);
    EXPECT_FALSE(result.threw_exception);
}

TEST(Interpreter_Integration, DispatchException) {
    IRMethod method;
    IRInstruction call_insn; call_insn.op_code = IROpCode::Call;
    call_insn.call_target = reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0xABCDu));
    call_insn.arg_count = 0; call_insn.is_instance_call = false;
    method.instructions.push_back(call_insn);
    IRInstruction ret_insn; ret_insn.op_code = IROpCode::Ret; method.instructions.push_back(ret_insn);

    MockDispatchState state;
    state.result_to_return.threw_exception = true;
    state.result_to_return.exception_value = InterpreterValue::null_val();

    ExecutionFrame frame;
    frame.dispatch_fn = MockDispatchCallback;
    frame.dispatch_context = &state;

    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);

    EXPECT_TRUE(state.called);
    EXPECT_TRUE(result.threw_exception);
    EXPECT_FALSE(result.has_return_value);
}

TEST(Interpreter_Integration, DispatchArgs) {
    IRMethod method;
    IRInstruction push1; push1.op_code = IROpCode::LdcI4; push1.immediate_i4 = 10; method.instructions.push_back(push1);
    IRInstruction push2; push2.op_code = IROpCode::LdcI4; push2.immediate_i4 = 20; method.instructions.push_back(push2);
    IRInstruction call_insn; call_insn.op_code = IROpCode::Call;
    call_insn.call_target = reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0x5678u));
    call_insn.arg_count = 2; call_insn.is_instance_call = false;
    method.instructions.push_back(call_insn);
    IRInstruction ret_insn; ret_insn.op_code = IROpCode::Ret; method.instructions.push_back(ret_insn);

    MockDispatchState state;
    state.result_to_return.has_value = true;
    state.result_to_return.value = InterpreterValue::from_i32(99);

    ExecutionFrame frame;
    frame.dispatch_fn = MockDispatchCallback;
    frame.dispatch_context = &state;

    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);

    EXPECT_TRUE(state.called);
    EXPECT_EQ(state.received_arg_count, 2u);
    ASSERT_GE(state.captured_args.size(), 2u);
    EXPECT_EQ(state.captured_args[0].tag, ValueTag::Int32);
    EXPECT_EQ(state.captured_args[0].i32, 10);
    EXPECT_EQ(state.captured_args[1].tag, ValueTag::Int32);
    EXPECT_EQ(state.captured_args[1].i32, 20);
    EXPECT_TRUE(result.has_return_value);
    EXPECT_EQ(result.return_value.i32, 99);
}

// ════════════════════════════════════════════════════════════════════════════
// Interface offset cache + runtime_iface_map tests
// ════════════════════════════════════════════════════════════════════════════

TEST(Interpreter_Integration, ChaosFindInterfaceOffset) {
    using namespace chaos::il2cpp::vtable_registry;
    using chaos::il2cpp::common::InterfaceMapEntry;

    static void* kBaseFn   = reinterpret_cast<void*>(0xBADF00D1);
    static void* kBaseFn2  = reinterpret_cast<void*>(0xBADF00D2);
    static void* kIfaceFn1 = reinterpret_cast<void*>(0xBADF00D3);
    static void* kIfaceFn2 = reinterpret_cast<void*>(0xBADF00D4);

    static const InterfaceMapEntry s_iface_map[] = {
        { 0x1001ULL, 2u, 2u }
    };
    static const void* s_vtable_array[] = { kBaseFn, kBaseFn2, kIfaceFn1, kIfaceFn2 };
    static const VTableSlot s_slots[] = { { 0x300u, kBaseFn }, { 0x301u, kBaseFn2 } };

    const VTableDescriptorV0 base_desc = {
        0x20000001ULL, 0x640u, 0u, 2u, s_slots, s_vtable_array, 4u, 1u, {0,0,0}, s_iface_map, 1u
    };

    UnregisterTypeVTable(0x640u);
    UnregisterTypeVTable(0x641u);
    RegisterCodegenVTable(&base_desc);

    EXPECT_EQ(chaos_find_interface_offset(0x640u, 0x1001ULL), 2u);
    EXPECT_EQ(chaos_find_interface_offset(0x640u, 0xDEADULL), CHAOS_IL2CPP_UINT32_MAX);
    EXPECT_EQ(chaos_find_interface_offset(0x999u, 0x1001ULL), CHAOS_IL2CPP_UINT32_MAX);
    EXPECT_EQ(chaos_find_interface_offset(0u, 0x1001ULL), CHAOS_IL2CPP_UINT32_MAX);

    // Hot-update type + runtime interface
    static const VTableSlot hot_slots[] = { { 0x300u, kBaseFn }, { 0x301u, kBaseFn2 } };
    ASSERT_TRUE(RegisterHotUpdateVTable(0x20000002ULL, 0x641u, 0x640u, hot_slots, 2u, 1u));

    EXPECT_EQ(chaos_find_interface_offset(0x641u, 0x1002ULL), CHAOS_IL2CPP_UINT32_MAX);

    ASSERT_TRUE(RegisterTypeVTableRuntimeInterface(0x641u, 0x1002ULL, 3u, 1u));
    EXPECT_EQ(chaos_find_interface_offset(0x641u, 0x1002ULL), 3u);

    // Idempotent re-registration
    ASSERT_TRUE(RegisterTypeVTableRuntimeInterface(0x641u, 0x1002ULL, 3u, 1u));
    EXPECT_EQ(chaos_find_interface_offset(0x641u, 0x1002ULL), 3u);
}

TEST(Interpreter_Integration, HotUpdateVTableInterfaceResolution) {
    using namespace chaos::il2cpp::vtable_registry;
    using chaos::il2cpp::common::InterfaceMapEntry;

    static void* kBaseFn   = reinterpret_cast<void*>(0xCAFEBAB1);
    static void* kBaseFn2  = reinterpret_cast<void*>(0xCAFEBAB2);
    static void* kIfaceFn1 = reinterpret_cast<void*>(0xCAFEBAB3);
    static void* kIfaceFn2 = reinterpret_cast<void*>(0xCAFEBAB4);

    static const InterfaceMapEntry s_iface_map[] = { { 0x3001ULL, 2u, 2u } };
    static const void* s_vtable_array[] = { kBaseFn, kBaseFn2, kIfaceFn1, kIfaceFn2 };
    static const VTableSlot s_slots[] = { { 0x400u, kBaseFn }, { 0x401u, kBaseFn2 } };

    const VTableDescriptorV0 base_desc = {
        0x30000001ULL, 0x650u, 0u, 2u, s_slots, s_vtable_array, 4u, 1u, {0,0,0}, s_iface_map, 1u
    };

    UnregisterTypeVTable(0x650u);
    UnregisterTypeVTable(0x651u);
    RegisterCodegenVTable(&base_desc);

    static const VTableSlot hot_slots[] = { { 0x400u, kBaseFn }, { 0x401u, kBaseFn2 } };
    ASSERT_TRUE(RegisterHotUpdateVTable(0x30000002ULL, 0x651u, 0x650u, hot_slots, 2u, 1u));

    EXPECT_EQ(ResolveVirtualMethodPointer(0x651u, 0u), nullptr);

    ASSERT_TRUE(RegisterTypeVTableRuntimeInterface(0x651u, 0x3001ULL, 2u, 2u));
    EXPECT_EQ(ResolveVirtualMethodPointer(0x651u, 0u), kIfaceFn1);
    EXPECT_EQ(ResolveVirtualMethodPointer(0x651u, 1u), kIfaceFn2);
    EXPECT_EQ(ResolveVirtualMethodPointer(0x651u, 2u), nullptr);
    EXPECT_EQ(ResolveVirtualMethodPointer(0x650u, 0u), kIfaceFn1);
}

// ════════════════════════════════════════════════════════════════════════════
// Method replacement dispatch test (from method_replacement_test.cpp)
// ════════════════════════════════════════════════════════════════════════════

TEST(Interpreter_Integration, MethodReplacementDispatch) {
    using namespace chaos::il2cpp::runtime_core;

    void* const kOriginalFn = reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0x1111ULL));
    void* const kReplacementFn = reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0x2222ULL));
    constexpr CHAOS_IL2CPP_UINT32 kMethodToken = 0x42u;
    constexpr CHAOS_IL2CPP_UINT32 kSlot = 0u;

    // Register mock hotpatch module
    static HotpatchMethodEntryV0 s_method_entries[] = { { "MyMethod", kMethodToken, 0u } };
    static HotpatchTypeEntryV0 s_type_entries[] = { { "MyNS", "MyType", 0u, 1u } };
    static HotpatchSlotEntryV0 s_token_slot_entries[] = { { kMethodToken, kSlot } };
    static HotpatchEntryV0 s_entry_table[] = { { kOriginalFn, 0u, 0u } };

    HotpatchModuleV0 mock_module;
    std::memset(&mock_module, 0, sizeof(mock_module));
    mock_module.type_entries       = s_type_entries;
    mock_module.type_entry_count   = 1u;
    mock_module.method_entries     = s_method_entries;
    mock_module.method_entry_count = 1u;
    mock_module.token_slot_entries     = s_token_slot_entries;
    mock_module.token_slot_entry_count = 1u;
    mock_module.entry_table        = s_entry_table;
    mock_module.entry_table_size   = 1u;
    RegisterHotpatchModule(&mock_module);

    // Resolve before Register → nullptr
    EXPECT_EQ(chaos::il2cpp::method_replacement::Resolve(kMethodToken), nullptr);

    // Register → true
    ASSERT_TRUE(chaos::il2cpp::method_replacement::Register(kMethodToken, kReplacementFn));

    // Resolve after Register → thunk
    EXPECT_EQ(chaos::il2cpp::method_replacement::Resolve(kMethodToken), kReplacementFn);

    // Verify dispatch entry active
    auto& registry = GetHotpatchNameRegistry();
    uint64_t composite = registry.FindToken(kMethodToken);
    ASSERT_NE(composite, 0);

    uint32_t mod_id = ExtractModuleId(composite);
    HotpatchEntryV0* entry = registry.GetDispatchEntry(mod_id, kMethodToken);
    ASSERT_NE(entry, nullptr);
    EXPECT_TRUE(HotpatchIsActive(*entry));

    // Revert → true
    ASSERT_TRUE(chaos::il2cpp::method_replacement::Revert(kMethodToken));

    // Resolve after Revert → nullptr
    EXPECT_EQ(chaos::il2cpp::method_replacement::Resolve(kMethodToken), nullptr);

    // Dispatch entry no longer active
    EXPECT_FALSE(HotpatchIsActive(*entry));
}
