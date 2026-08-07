/// interpreter_smoke_test.cpp — InterpreterVM smoke tests (gtest)
/// Exercises: add, branch (brtrue + ceq), external runtime stubs

#include "interpreter_vm.h"
#include <gtest/gtest.h>

// AOT symbol stubs (required by chaos_interpreter library linkage)
extern "C" const char* kChaosExternalRuntimeSubjects[] = { nullptr };
extern "C" void* kChaosExternalRuntimeFnTable[] = { nullptr };
extern "C" int kChaosExternalRuntimeCount = 0;
extern "C" void* chaos_il2cpp_aot_hotpatch_module = nullptr;

using chaos::il2cpp::interpreter::ExecutionFrame;
using chaos::il2cpp::interpreter::ExecutionResult;
using chaos::il2cpp::interpreter::IRInstruction;
using chaos::il2cpp::interpreter::IRMethod;
using chaos::il2cpp::interpreter::IROpCode;
using chaos::il2cpp::interpreter::InterpreterVM;

TEST(Interpreter_Smoke, AddAndBranch) {
    // Test 1: ldarg.0 + ldarg.1 + add + ret → 20 + 22 = 42
    {
        IRMethod add_method;
        add_method.instructions = {
            IRInstruction{IROpCode::LdArg, 0, 0},
            IRInstruction{IROpCode::StLoc, 0, 0},
            IRInstruction{IROpCode::LdLoc, 0, 0},
            IRInstruction{IROpCode::LdArg, 1, 0},
            IRInstruction{IROpCode::Add, 0, 0},
            IRInstruction{IROpCode::Ret, 0, 0},
        };

        ExecutionFrame frame;
        frame.arguments = {20, 22};

        const InterpreterVM vm = {};
        const ExecutionResult result = vm.Execute(add_method, &frame);

        EXPECT_TRUE(result.has_return_value);
        EXPECT_EQ(result.int32_value, 42);
    }

    // Test 2: branch with ceq — ldarg.0 + 40 + add → ceq 42 → brtrue → ret 42
    {
        IRMethod branch_method;
        branch_method.instructions = {
            IRInstruction{IROpCode::LdcI4, 0, 40},
            IRInstruction{IROpCode::LdArg, 0, 0},
            IRInstruction{IROpCode::Add, 0, 0},
            IRInstruction{IROpCode::LdcI4, 0, 42},
            IRInstruction{IROpCode::Ceq, 0, 0},
            IRInstruction{IROpCode::BrTrue, 0, 0, 8},
            IRInstruction{IROpCode::LdcI4, 0, -1},
            IRInstruction{IROpCode::Ret, 0, 0},
            IRInstruction{IROpCode::LdcI4, 0, 42},
            IRInstruction{IROpCode::Ret, 0, 0},
        };

        ExecutionFrame frame;
        frame.arguments = {2};

        const InterpreterVM vm = {};
        const ExecutionResult result = vm.Execute(branch_method, &frame);

        EXPECT_TRUE(result.has_return_value);
        EXPECT_EQ(result.int32_value, 42);
    }
}
