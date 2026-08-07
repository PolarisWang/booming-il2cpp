/// interpreter_smoke_isolated_test.cpp — Minimal isolated InterpreterVM smoke test (gtest)
/// Exercises: ldc.i4 + ret (isolated from other tests)

#include "interpreter_vm.h"
#include <gtest/gtest.h>

// AOT symbol stubs (required by chaos_interpreter linkage)
extern "C" const char* kChaosExternalRuntimeSubjects[] = { nullptr };
extern "C" void* kChaosExternalRuntimeFnTable[] = { nullptr };
extern "C" int kChaosExternalRuntimeCount = 0;
extern "C" void* chaos_il2cpp_aot_hotpatch_module = nullptr;

using namespace chaos::il2cpp::interpreter;

TEST(Interpreter_SmokeIsolated, LdcI4Ret) {
    IRMethod method;
    IRInstruction push; push.op_code = IROpCode::LdcI4; push.immediate_i4 = 42; method.instructions.push_back(push);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);

    EXPECT_TRUE(result.has_return_value);
    EXPECT_EQ(result.int32_value, 42);
}
