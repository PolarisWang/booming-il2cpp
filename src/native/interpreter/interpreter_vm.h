#ifndef CHAOS_IL2CPP_INTERPRETER_VM_H_
#define CHAOS_IL2CPP_INTERPRETER_VM_H_

#include <cstddef>
#include <cstdint>
#include <vector>

namespace chaos::il2cpp::interpreter {

enum class IROpCode {
    LdcI4 = 0,
    LdArg = 1,
    LdLoc = 2,
    StLoc = 3,
    Add = 4,
    Sub = 5,
    Mul = 6,
    Div = 7,
    Rem = 8,
    Ceq = 9,
    Clt = 10,
    Cgt = 11,
    Br = 12,
    BrTrue = 13,
    BrFalse = 14,
    Ret = 15,
};

struct IRInstruction {
    IROpCode op_code = IROpCode::Ret;
    int32_t operand_index = 0;
    int32_t immediate_i4 = 0;
    size_t branch_target = 0;
};

struct IRMethod {
    std::vector<IRInstruction> instructions = {};
};

struct ExecutionFrame {
    std::vector<int32_t> arguments = {};
    std::vector<int32_t> locals = {};
    std::vector<int32_t> stack = {};
};

struct ExecutionResult {
    bool has_return_value = false;
    int32_t int32_value = 0;
};

class InterpreterVM {
public:
    ExecutionResult Execute(const IRMethod& method, ExecutionFrame* frame) const;

private:
    static size_t GetBranchTarget(const IRMethod& method, size_t target);
    static void EnsureLocal(std::vector<int32_t>* locals, size_t index);
    static int32_t Pop(std::vector<int32_t>* stack);
};

}  // namespace chaos::il2cpp::interpreter

#endif  // CHAOS_IL2CPP_INTERPRETER_VM_H_
