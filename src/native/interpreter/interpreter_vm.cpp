#include "interpreter_vm.h"

#include <stdexcept>

namespace chaos::il2cpp::interpreter {

size_t InterpreterVM::GetBranchTarget(const IRMethod& method, size_t target) {
    if (target >= method.instructions.size()) {
        throw std::out_of_range("branch_target");
    }

    return target;
}

void InterpreterVM::EnsureLocal(std::vector<int32_t>* locals, size_t index) {
    if (locals == nullptr) {
        throw std::invalid_argument("locals");
    }

    if (locals->size() <= index) {
        locals->resize(index + 1u, 0);
    }
}

int32_t InterpreterVM::Pop(std::vector<int32_t>* stack) {
    if (stack == nullptr || stack->empty()) {
        throw std::runtime_error("evaluation stack underflow");
    }

    const int32_t value = stack->back();
    stack->pop_back();
    return value;
}

ExecutionResult InterpreterVM::Execute(const IRMethod& method, ExecutionFrame* frame) const {
    if (frame == nullptr) {
        throw std::invalid_argument("frame");
    }

    ExecutionResult result = {};
    size_t instruction_index = 0;

    while (instruction_index < method.instructions.size()) {
        const IRInstruction& instruction = method.instructions[instruction_index];
        switch (instruction.op_code) {
            case IROpCode::LdcI4:
                frame->stack.push_back(instruction.immediate_i4);
                break;
            case IROpCode::LdArg:
                if (instruction.operand_index < 0 ||
                    static_cast<size_t>(instruction.operand_index) >= frame->arguments.size()) {
                    throw std::out_of_range("argument");
                }

                frame->stack.push_back(frame->arguments[static_cast<size_t>(instruction.operand_index)]);
                break;
            case IROpCode::LdLoc:
                EnsureLocal(&frame->locals, static_cast<size_t>(instruction.operand_index));
                frame->stack.push_back(frame->locals[static_cast<size_t>(instruction.operand_index)]);
                break;
            case IROpCode::StLoc: {
                EnsureLocal(&frame->locals, static_cast<size_t>(instruction.operand_index));
                frame->locals[static_cast<size_t>(instruction.operand_index)] = Pop(&frame->stack);
                break;
            }
            case IROpCode::Add: {
                const int32_t right = Pop(&frame->stack);
                const int32_t left = Pop(&frame->stack);
                frame->stack.push_back(left + right);
                break;
            }
            case IROpCode::Sub: {
                const int32_t right = Pop(&frame->stack);
                const int32_t left = Pop(&frame->stack);
                frame->stack.push_back(left - right);
                break;
            }
            case IROpCode::Mul: {
                const int32_t right = Pop(&frame->stack);
                const int32_t left = Pop(&frame->stack);
                frame->stack.push_back(left * right);
                break;
            }
            case IROpCode::Div: {
                const int32_t right = Pop(&frame->stack);
                const int32_t left = Pop(&frame->stack);
                frame->stack.push_back(left / right);
                break;
            }
            case IROpCode::Rem: {
                const int32_t right = Pop(&frame->stack);
                const int32_t left = Pop(&frame->stack);
                frame->stack.push_back(left % right);
                break;
            }
            case IROpCode::Ceq: {
                const int32_t right = Pop(&frame->stack);
                const int32_t left = Pop(&frame->stack);
                frame->stack.push_back(left == right ? 1 : 0);
                break;
            }
            case IROpCode::Clt: {
                const int32_t right = Pop(&frame->stack);
                const int32_t left = Pop(&frame->stack);
                frame->stack.push_back(left < right ? 1 : 0);
                break;
            }
            case IROpCode::Cgt: {
                const int32_t right = Pop(&frame->stack);
                const int32_t left = Pop(&frame->stack);
                frame->stack.push_back(left > right ? 1 : 0);
                break;
            }
            case IROpCode::Br:
                instruction_index = GetBranchTarget(method, instruction.branch_target);
                continue;
            case IROpCode::BrTrue:
                if (Pop(&frame->stack) != 0) {
                    instruction_index = GetBranchTarget(method, instruction.branch_target);
                    continue;
                }
                break;
            case IROpCode::BrFalse:
                if (Pop(&frame->stack) == 0) {
                    instruction_index = GetBranchTarget(method, instruction.branch_target);
                    continue;
                }
                break;
            case IROpCode::Ret:
                if (!frame->stack.empty()) {
                    result.has_return_value = true;
                    result.int32_value = Pop(&frame->stack);
                }

                return result;
            default:
                throw std::runtime_error("unsupported opcode");
        }

        ++instruction_index;
    }

    return result;
}

}  // namespace chaos::il2cpp::interpreter
