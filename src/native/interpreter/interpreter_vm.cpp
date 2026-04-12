#include "interpreter_vm.h"

#include <stdexcept>
#include <vector>

namespace chaos::il2cpp::interpreter {

namespace {

struct ObjectStorage {
    std::vector<InterpreterValue> fields = {};
};

struct ArrayStorage {
    std::vector<InterpreterValue> elements = {};
};

struct BoxedValue {
    InterpreterValue value = {};
};

std::vector<InterpreterValue> g_static_fields;

int32_t ReadInt32(const InterpreterValue& value) {
    switch (value.tag) {
        case ValueTag::Int32:
            return value.i32;
        case ValueTag::Int64:
            return static_cast<int32_t>(value.i64);
        case ValueTag::Float32:
            return static_cast<int32_t>(value.f32);
        case ValueTag::Float64:
            return static_cast<int32_t>(value.f64);
        case ValueTag::Null:
            return 0;
        default:
            throw std::runtime_error("value is not convertible to int32");
    }
}

int64_t ReadInt64(const InterpreterValue& value) {
    switch (value.tag) {
        case ValueTag::Int32:
            return static_cast<int64_t>(value.i32);
        case ValueTag::Int64:
            return value.i64;
        case ValueTag::Float32:
            return static_cast<int64_t>(value.f32);
        case ValueTag::Float64:
            return static_cast<int64_t>(value.f64);
        case ValueTag::Null:
            return 0;
        default:
            throw std::runtime_error("value is not convertible to int64");
    }
}

float ReadFloat32(const InterpreterValue& value) {
    switch (value.tag) {
        case ValueTag::Int32:
            return static_cast<float>(value.i32);
        case ValueTag::Int64:
            return static_cast<float>(value.i64);
        case ValueTag::Float32:
            return value.f32;
        case ValueTag::Float64:
            return static_cast<float>(value.f64);
        case ValueTag::Null:
            return 0.0f;
        default:
            throw std::runtime_error("value is not convertible to float32");
    }
}

double ReadFloat64(const InterpreterValue& value) {
    switch (value.tag) {
        case ValueTag::Int32:
            return static_cast<double>(value.i32);
        case ValueTag::Int64:
            return static_cast<double>(value.i64);
        case ValueTag::Float32:
            return static_cast<double>(value.f32);
        case ValueTag::Float64:
            return value.f64;
        case ValueTag::Null:
            return 0.0;
        default:
            throw std::runtime_error("value is not convertible to float64");
    }
}

ObjectStorage* RequireObject(const InterpreterValue& value) {
    if (value.tag != ValueTag::ObjectRef || value.obj == nullptr) {
        throw std::runtime_error("object reference required");
    }

    return static_cast<ObjectStorage*>(value.obj);
}

ArrayStorage* RequireArray(const InterpreterValue& value) {
    if (value.tag != ValueTag::ObjectRef || value.obj == nullptr) {
        throw std::runtime_error("array reference required");
    }

    return static_cast<ArrayStorage*>(value.obj);
}

}  // namespace

InterpreterValue InterpreterValue::from_i32(int32_t value) {
    return InterpreterValue(value);
}

InterpreterValue InterpreterValue::from_i64(int64_t value) {
    InterpreterValue result = {};
    result.tag = ValueTag::Int64;
    result.i64 = value;
    return result;
}

InterpreterValue InterpreterValue::from_f32(float value) {
    InterpreterValue result = {};
    result.tag = ValueTag::Float32;
    result.f32 = value;
    return result;
}

InterpreterValue InterpreterValue::from_f64(double value) {
    InterpreterValue result = {};
    result.tag = ValueTag::Float64;
    result.f64 = value;
    return result;
}

InterpreterValue InterpreterValue::from_obj(void* value) {
    InterpreterValue result = {};
    result.tag = ValueTag::ObjectRef;
    result.obj = value;
    return result;
}

InterpreterValue InterpreterValue::null_val() {
    InterpreterValue result = {};
    result.tag = ValueTag::Null;
    result.obj = nullptr;
    return result;
}

size_t InterpreterVM::GetBranchTarget(const IRMethod& method, size_t target) {
    if (target >= method.instructions.size()) {
        throw std::out_of_range("branch_target");
    }

    return target;
}

void InterpreterVM::EnsureLocal(std::vector<InterpreterValue>* locals, size_t index) {
    if (locals == nullptr) {
        throw std::invalid_argument("locals");
    }

    if (locals->size() <= index) {
        locals->resize(index + 1u, InterpreterValue{});
    }
}

InterpreterValue InterpreterVM::Pop(std::vector<InterpreterValue>* stack) {
    if (stack == nullptr || stack->empty()) {
        throw std::runtime_error("evaluation stack underflow");
    }

    const InterpreterValue value = stack->back();
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
                frame->stack.push_back(InterpreterValue::from_i32(instruction.immediate_i4));
                break;
            case IROpCode::LdcI8:
                frame->stack.push_back(InterpreterValue::from_i64(instruction.immediate_i8));
                break;
            case IROpCode::LdcR4:
                frame->stack.push_back(InterpreterValue::from_f32(static_cast<float>(instruction.immediate_r8)));
                break;
            case IROpCode::LdcR8:
                frame->stack.push_back(InterpreterValue::from_f64(instruction.immediate_r8));
                break;
            case IROpCode::LdStr:
                frame->stack.push_back(InterpreterValue::from_obj(const_cast<char*>(instruction.string_operand)));
                break;
            case IROpCode::LdNull:
                frame->stack.push_back(InterpreterValue::null_val());
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
            case IROpCode::StArg: {
                if (instruction.operand_index < 0) {
                    throw std::out_of_range("argument");
                }

                const size_t argument_index = static_cast<size_t>(instruction.operand_index);
                if (frame->arguments.size() <= argument_index) {
                    frame->arguments.resize(argument_index + 1u);
                }

                frame->arguments[argument_index] = Pop(&frame->stack);
                break;
            }
            case IROpCode::LdSFld: {
                if (g_static_fields.size() <= instruction.field_offset) {
                    g_static_fields.resize(instruction.field_offset + 1u);
                }

                frame->stack.push_back(g_static_fields[instruction.field_offset]);
                break;
            }
            case IROpCode::StSFld: {
                if (g_static_fields.size() <= instruction.field_offset) {
                    g_static_fields.resize(instruction.field_offset + 1u);
                }

                g_static_fields[instruction.field_offset] = Pop(&frame->stack);
                break;
            }
            case IROpCode::Beq: {
                const InterpreterValue right = Pop(&frame->stack);
                const InterpreterValue left = Pop(&frame->stack);
                if (ReadFloat64(left) == ReadFloat64(right)) {
                    instruction_index = GetBranchTarget(method, instruction.branch_target);
                    continue;
                }
                break;
            }
            case IROpCode::Blt: {
                const int32_t right = ReadInt32(Pop(&frame->stack));
                const int32_t left = ReadInt32(Pop(&frame->stack));
                if (left < right) {
                    instruction_index = GetBranchTarget(method, instruction.branch_target);
                    continue;
                }
                break;
            }
            case IROpCode::Bgt: {
                const int32_t right = ReadInt32(Pop(&frame->stack));
                const int32_t left = ReadInt32(Pop(&frame->stack));
                if (left > right) {
                    instruction_index = GetBranchTarget(method, instruction.branch_target);
                    continue;
                }
                break;
            }
            case IROpCode::Ble: {
                const int32_t right = ReadInt32(Pop(&frame->stack));
                const int32_t left = ReadInt32(Pop(&frame->stack));
                if (left <= right) {
                    instruction_index = GetBranchTarget(method, instruction.branch_target);
                    continue;
                }
                break;
            }
            case IROpCode::Bge: {
                const int32_t right = ReadInt32(Pop(&frame->stack));
                const int32_t left = ReadInt32(Pop(&frame->stack));
                if (left >= right) {
                    instruction_index = GetBranchTarget(method, instruction.branch_target);
                    continue;
                }
                break;
            }
            case IROpCode::Add: {
                const int32_t right = ReadInt32(Pop(&frame->stack));
                const int32_t left = ReadInt32(Pop(&frame->stack));
                frame->stack.push_back(InterpreterValue::from_i32(left + right));
                break;
            }
            case IROpCode::Sub: {
                const int32_t right = ReadInt32(Pop(&frame->stack));
                const int32_t left = ReadInt32(Pop(&frame->stack));
                frame->stack.push_back(InterpreterValue::from_i32(left - right));
                break;
            }
            case IROpCode::Mul: {
                const int32_t right = ReadInt32(Pop(&frame->stack));
                const int32_t left = ReadInt32(Pop(&frame->stack));
                frame->stack.push_back(InterpreterValue::from_i32(left * right));
                break;
            }
            case IROpCode::Div: {
                const int32_t right = ReadInt32(Pop(&frame->stack));
                const int32_t left = ReadInt32(Pop(&frame->stack));
                frame->stack.push_back(InterpreterValue::from_i32(left / right));
                break;
            }
            case IROpCode::Rem: {
                const int32_t right = ReadInt32(Pop(&frame->stack));
                const int32_t left = ReadInt32(Pop(&frame->stack));
                frame->stack.push_back(InterpreterValue::from_i32(left % right));
                break;
            }
            case IROpCode::Neg: {
                const int32_t value = ReadInt32(Pop(&frame->stack));
                frame->stack.push_back(InterpreterValue::from_i32(-value));
                break;
            }
            case IROpCode::Ceq: {
                const InterpreterValue right = Pop(&frame->stack);
                const InterpreterValue left = Pop(&frame->stack);
                const bool equal = left.tag == ValueTag::ObjectRef || left.tag == ValueTag::Null
                    ? left.obj == right.obj
                    : ReadFloat64(left) == ReadFloat64(right);
                frame->stack.push_back(InterpreterValue::from_i32(equal ? 1 : 0));
                break;
            }
            case IROpCode::Clt: {
                const int32_t right = ReadInt32(Pop(&frame->stack));
                const int32_t left = ReadInt32(Pop(&frame->stack));
                frame->stack.push_back(InterpreterValue::from_i32(left < right ? 1 : 0));
                break;
            }
            case IROpCode::Cgt: {
                const int32_t right = ReadInt32(Pop(&frame->stack));
                const int32_t left = ReadInt32(Pop(&frame->stack));
                frame->stack.push_back(InterpreterValue::from_i32(left > right ? 1 : 0));
                break;
            }
            case IROpCode::Br:
                instruction_index = GetBranchTarget(method, instruction.branch_target);
                continue;
            case IROpCode::BrTrue:
                if (ReadInt32(Pop(&frame->stack)) != 0) {
                    instruction_index = GetBranchTarget(method, instruction.branch_target);
                    continue;
                }
                break;
            case IROpCode::BrFalse:
                if (ReadInt32(Pop(&frame->stack)) == 0) {
                    instruction_index = GetBranchTarget(method, instruction.branch_target);
                    continue;
                }
                break;
            case IROpCode::NewObj: {
                auto* storage = new ObjectStorage();
                storage->fields.resize(instruction.secondary_index == 0u ? 1u : instruction.secondary_index);
                frame->stack.push_back(InterpreterValue::from_obj(storage));
                break;
            }
            case IROpCode::NewArr: {
                const size_t length = static_cast<size_t>(ReadInt32(Pop(&frame->stack)));
                auto* storage = new ArrayStorage();
                storage->elements.resize(length);
                frame->stack.push_back(InterpreterValue::from_obj(storage));
                break;
            }
            case IROpCode::LdFld: {
                auto* object = RequireObject(Pop(&frame->stack));
                if (object->fields.size() <= instruction.field_offset) {
                    object->fields.resize(instruction.field_offset + 1u);
                }
                frame->stack.push_back(object->fields[instruction.field_offset]);
                break;
            }
            case IROpCode::StFld: {
                const InterpreterValue value = Pop(&frame->stack);
                auto* object = RequireObject(Pop(&frame->stack));
                if (object->fields.size() <= instruction.field_offset) {
                    object->fields.resize(instruction.field_offset + 1u);
                }
                object->fields[instruction.field_offset] = value;
                break;
            }
            case IROpCode::LdElem: {
                const size_t index = static_cast<size_t>(ReadInt32(Pop(&frame->stack)));
                auto* array = RequireArray(Pop(&frame->stack));
                if (index >= array->elements.size()) {
                    throw std::out_of_range("array_index");
                }
                frame->stack.push_back(array->elements[index]);
                break;
            }
            case IROpCode::StElem: {
                const InterpreterValue value = Pop(&frame->stack);
                const size_t index = static_cast<size_t>(ReadInt32(Pop(&frame->stack)));
                auto* array = RequireArray(Pop(&frame->stack));
                if (index >= array->elements.size()) {
                    throw std::out_of_range("array_index");
                }
                array->elements[index] = value;
                break;
            }
            case IROpCode::LdLen: {
                auto* array = RequireArray(Pop(&frame->stack));
                frame->stack.push_back(InterpreterValue::from_i32(static_cast<int32_t>(array->elements.size())));
                break;
            }
            case IROpCode::Pop:
                (void)Pop(&frame->stack);
                break;
            case IROpCode::Box: {
                auto* boxed = new BoxedValue();
                boxed->value = Pop(&frame->stack);
                frame->stack.push_back(InterpreterValue::from_obj(boxed));
                break;
            }
            case IROpCode::Unbox: {
                const InterpreterValue boxed_value = Pop(&frame->stack);
                if (boxed_value.tag != ValueTag::ObjectRef || boxed_value.obj == nullptr) {
                    throw std::runtime_error("boxed value required");
                }
                frame->stack.push_back(static_cast<BoxedValue*>(boxed_value.obj)->value);
                break;
            }
            case IROpCode::CastClass:
            case IROpCode::IsInst:
                frame->stack.push_back(Pop(&frame->stack));
                break;
            case IROpCode::Call:
            case IROpCode::CallVirt:
            case IROpCode::CallBridge:
                throw std::runtime_error("call dispatch requires external bridge");
            case IROpCode::Conv_I4:
                frame->stack.push_back(InterpreterValue::from_i32(ReadInt32(Pop(&frame->stack))));
                break;
            case IROpCode::Conv_I8:
                frame->stack.push_back(InterpreterValue::from_i64(ReadInt64(Pop(&frame->stack))));
                break;
            case IROpCode::Conv_R4:
                frame->stack.push_back(InterpreterValue::from_f32(ReadFloat32(Pop(&frame->stack))));
                break;
            case IROpCode::Conv_R8:
                frame->stack.push_back(InterpreterValue::from_f64(ReadFloat64(Pop(&frame->stack))));
                break;
            case IROpCode::Throw:
                throw std::runtime_error("interpreter throw");
            case IROpCode::Rethrow:
                throw std::runtime_error("interpreter rethrow");
            case IROpCode::Leave:
                instruction_index = GetBranchTarget(method, instruction.branch_target);
                continue;
            case IROpCode::EndFinally:
            case IROpCode::EndFilter:
                break;
            case IROpCode::Ret:
                if (!frame->stack.empty()) {
                    result.has_return_value = true;
                    result.return_value = Pop(&frame->stack);
                    if (result.return_value.tag == ValueTag::Int32) {
                        result.int32_value = result.return_value.i32;
                    }
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
