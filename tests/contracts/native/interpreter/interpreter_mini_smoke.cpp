#include "interpreter_vm.h"
#include <cstdio>

using namespace chaos::il2cpp::interpreter;

int main() {
    std::printf("starting\n");
    IRMethod method;
    IRInstruction push; push.op_code = IROpCode::LdcI4; push.immediate_i4 = 42; method.instructions.push_back(push);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);
    std::printf("executing\n");
    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    std::printf("done: has_return=%d val=%d\n", result.has_return_value, result.int32_value);
    return (result.has_return_value && result.int32_value == 42) ? 0 : 1;
}