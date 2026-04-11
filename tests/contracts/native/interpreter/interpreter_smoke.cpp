#include "interpreter_vm.h"

#include <iostream>

namespace {

using chaos::il2cpp::interpreter::ExecutionFrame;
using chaos::il2cpp::interpreter::ExecutionResult;
using chaos::il2cpp::interpreter::IRInstruction;
using chaos::il2cpp::interpreter::IRMethod;
using chaos::il2cpp::interpreter::IROpCode;
using chaos::il2cpp::interpreter::InterpreterVM;

}  // namespace

int main() {
    IRMethod add_method = {};
    add_method.instructions = {
        IRInstruction{IROpCode::LdArg, 0, 0},
        IRInstruction{IROpCode::StLoc, 0, 0},
        IRInstruction{IROpCode::LdLoc, 0, 0},
        IRInstruction{IROpCode::LdArg, 1, 0},
        IRInstruction{IROpCode::Add, 0, 0},
        IRInstruction{IROpCode::Ret, 0, 0},
    };

    ExecutionFrame add_frame = {};
    add_frame.arguments = {20, 22};

    const InterpreterVM vm = {};
    const ExecutionResult add_result = vm.Execute(add_method, &add_frame);
    if (!add_result.has_return_value || add_result.int32_value != 42) {
        std::cerr << "native-interpreter-smoke=unexpected" << std::endl;
        return 1;
    }

    IRMethod branch_method = {};
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

    ExecutionFrame branch_frame = {};
    branch_frame.arguments = {2};
    const ExecutionResult branch_result = vm.Execute(branch_method, &branch_frame);
    if (!branch_result.has_return_value || branch_result.int32_value != 42) {
        std::cerr << "native-interpreter-branch=unexpected" << std::endl;
        return 1;
    }

    std::cout << "native-interpreter-path=ldarg,stloc,ldloc,ldarg,add,ret" << std::endl;
    std::cout << "native-interpreter-smoke=" << add_result.int32_value << std::endl;
    std::cout << "native-interpreter-branch=" << branch_result.int32_value << std::endl;
    return 0;
}
