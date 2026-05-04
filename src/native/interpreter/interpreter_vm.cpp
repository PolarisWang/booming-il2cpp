#include "interpreter_vm.h"

#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <vector>

#include "vtable_registry.h"

namespace chaos::il2cpp::interpreter {

namespace {

using ObjectStorage = InterpreterObject;

struct ArrayStorage {
    CHAOS_IL2CPP_VECTOR(InterpreterValue) elements = {};
};

struct BoxedValue {
    InterpreterValue value = {};
};

CHAOS_IL2CPP_VECTOR(InterpreterValue) g_static_fields;

CHAOS_IL2CPP_INT32 ReadInt32(const InterpreterValue& value) {
    switch (value.tag) {
        case ValueTag::Int32:
            return value.i32;
        case ValueTag::Int64:
            return static_cast<CHAOS_IL2CPP_INT32>(value.i64);
        case ValueTag::Float32:
            return static_cast<CHAOS_IL2CPP_INT32>(value.f32);
        case ValueTag::Float64:
            return static_cast<CHAOS_IL2CPP_INT32>(value.f64);
        case ValueTag::Null:
            return 0;
        default:
            throw CHAOS_IL2CPP_RUNTIME_ERROR("value is not convertible to int32");
    }
}

CHAOS_IL2CPP_INT64 ReadInt64(const InterpreterValue& value) {
    switch (value.tag) {
        case ValueTag::Int32:
            return static_cast<CHAOS_IL2CPP_INT64>(value.i32);
        case ValueTag::Int64:
            return value.i64;
        case ValueTag::Float32:
            return static_cast<CHAOS_IL2CPP_INT64>(value.f32);
        case ValueTag::Float64:
            return static_cast<CHAOS_IL2CPP_INT64>(value.f64);
        case ValueTag::Null:
            return 0;
        default:
            throw CHAOS_IL2CPP_RUNTIME_ERROR("value is not convertible to int64");
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
            throw CHAOS_IL2CPP_RUNTIME_ERROR("value is not convertible to float32");
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
            throw CHAOS_IL2CPP_RUNTIME_ERROR("value is not convertible to float64");
    }
}

ObjectStorage* RequireObject(const InterpreterValue& value) {
    if (value.tag != ValueTag::ObjectRef || value.obj == nullptr) {
        throw CHAOS_IL2CPP_RUNTIME_ERROR("object reference required");
    }

    return static_cast<ObjectStorage*>(value.obj);
}

ArrayStorage* RequireArray(const InterpreterValue& value) {
    if (value.tag != ValueTag::ObjectRef || value.obj == nullptr) {
        throw CHAOS_IL2CPP_RUNTIME_ERROR("array reference required");
    }

    return static_cast<ArrayStorage*>(value.obj);
}

// ── SEH helper: find innermost try clause covering idx with finally/fault ──
static int FindEnclosingFinally(const IRMethod& method, CHAOS_IL2CPP_SIZE idx) {
    for (int i = static_cast<int>(method.seh_clauses.size()) - 1; i >= 0; --i) {
        const auto& clause = method.seh_clauses[static_cast<CHAOS_IL2CPP_SIZE>(i)];
        if (idx >= clause.try_start_idx && idx < clause.try_end_idx) {
            const auto flags = static_cast<uint32_t>(clause.flags);
            if (flags == static_cast<uint32_t>(SEHFlags::Finally) ||
                flags == static_cast<uint32_t>(SEHFlags::Fault)) {
                return i;
            }
        }
    }
    return -1;
}

// ── SEH helper: find innermost try clause covering idx with catch handler ──
static int FindEnclosingCatch(const IRMethod& method, CHAOS_IL2CPP_SIZE idx) {
    for (int i = static_cast<int>(method.seh_clauses.size()) - 1; i >= 0; --i) {
        const auto& clause = method.seh_clauses[static_cast<CHAOS_IL2CPP_SIZE>(i)];
        if (idx >= clause.try_start_idx && idx < clause.try_end_idx) {
            const auto flags = static_cast<uint32_t>(clause.flags);
            if (flags == static_cast<uint32_t>(SEHFlags::Exception) ||
                flags == static_cast<uint32_t>(SEHFlags::Filter)) {
                return i;
            }
        }
    }
    return -1;
}

}  // namespace

// ── InterpreterValue lifecycle ──────────────────────────────────────────

void InterpreterValue::FreeStruct() {
    if (tag == ValueTag::Struct && obj != nullptr) {
        std::free(obj);
        obj = nullptr;
        struct_size = 0u;
    }
}

InterpreterValue::InterpreterValue(const InterpreterValue& other)
    : tag(other.tag)
    , struct_size(other.struct_size)
{
    if (tag == ValueTag::Struct && other.obj != nullptr) {
        // Deep-copy struct data.
        obj = std::malloc(struct_size);
        if (obj != nullptr) {
            std::memcpy(obj, other.obj, struct_size);
        } else {
            struct_size = 0u;
        }
    } else if (tag == ValueTag::Int64) {
        i64 = other.i64;
    } else if (tag == ValueTag::Float64) {
        f64 = other.f64;
    } else {
        obj = other.obj;
    }
}

InterpreterValue& InterpreterValue::operator=(const InterpreterValue& other) {
    if (this == &other) {
        return *this;
    }

    // Free existing struct data before overwriting.
    FreeStruct();

    tag = other.tag;
    struct_size = other.struct_size;

    if (tag == ValueTag::Struct && other.obj != nullptr) {
        obj = std::malloc(struct_size);
        if (obj != nullptr) {
            std::memcpy(obj, other.obj, struct_size);
        } else {
            struct_size = 0u;
        }
    } else if (tag == ValueTag::Int64) {
        i64 = other.i64;
    } else if (tag == ValueTag::Float64) {
        f64 = other.f64;
    } else {
        obj = other.obj;
    }

    return *this;
}

InterpreterValue::~InterpreterValue() {
    FreeStruct();
}

// ── Factory methods ────────────────────────────────────────────────────

InterpreterValue InterpreterValue::from_i32(CHAOS_IL2CPP_INT32 value) {
    return InterpreterValue(value);
}

InterpreterValue InterpreterValue::from_i64(CHAOS_IL2CPP_INT64 value) {
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

InterpreterValue InterpreterValue::from_struct(const void* data, CHAOS_IL2CPP_UINT32 size) {
    InterpreterValue result = {};
    result.tag = ValueTag::Struct;
    result.struct_size = size;
    if (data != nullptr && size > 0u) {
        result.obj = std::malloc(size);
        if (result.obj != nullptr) {
            std::memcpy(result.obj, data, size);
        } else {
            result.struct_size = 0u;
        }
    }
    return result;
}

InterpreterValue InterpreterValue::null_val() {
    InterpreterValue result = {};
    result.tag = ValueTag::Null;
    result.obj = nullptr;
    return result;
}

// ── ExecutionFrame destructor ──────────────────────────────────────────

ExecutionFrame::~ExecutionFrame() {
    // Free struct data in all stored values.
    for (auto& v : arguments) { v.FreeStruct(); }
    for (auto& v : locals)    { v.FreeStruct(); }
    for (auto& v : stack)     { v.FreeStruct(); }
}

CHAOS_IL2CPP_SIZE InterpreterVM::GetBranchTarget(const IRMethod& method, CHAOS_IL2CPP_SIZE target) {
    if (target >= method.instructions.size()) {
        throw CHAOS_IL2CPP_OUT_OF_RANGE("branch_target");
    }

    return target;
}

void InterpreterVM::EnsureLocal(CHAOS_IL2CPP_VECTOR(InterpreterValue)* locals, CHAOS_IL2CPP_SIZE index) {
    if (locals == nullptr) {
        throw CHAOS_IL2CPP_INVALID_ARGUMENT("locals");
    }

    if (locals->size() <= index) {
        locals->resize(index + 1u, InterpreterValue{});
    }
}

InterpreterValue InterpreterVM::Pop(CHAOS_IL2CPP_VECTOR(InterpreterValue)* stack) {
    if (stack == nullptr || stack->empty()) {
        throw CHAOS_IL2CPP_RUNTIME_ERROR("evaluation stack underflow");
    }

    const InterpreterValue value = stack->back();
    stack->pop_back();
    return value;
}

ExecutionResult InterpreterVM::Execute(const IRMethod& method, ExecutionFrame* frame) const {
    if (frame == nullptr) {
        throw CHAOS_IL2CPP_INVALID_ARGUMENT("frame");
    }

    ExecutionResult result = {};
    CHAOS_IL2CPP_SIZE instruction_index = 0;

    // SEH execution state.
    bool        in_handler              = false;
    bool        pending_leave           = false;
    CHAOS_IL2CPP_SIZE pending_leave_target = 0u;
    int32_t     active_handler_clause   = -1;

    // Two-phase exception state.
    bool        exception_in_flight     = false;
    InterpreterValue exception_obj      = {};
    int32_t     unwind_catch_clause     = -1;
    static constexpr int kMaxUnwindDepth = 8;
    int32_t     unwind_finally_list[kMaxUnwindDepth] = {};
    int32_t     unwind_finally_count    = 0;
    int32_t     unwind_finally_current  = 0;

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
                    static_cast<CHAOS_IL2CPP_SIZE>(instruction.operand_index) >= frame->arguments.size()) {
                    throw CHAOS_IL2CPP_OUT_OF_RANGE("argument");
                }

                frame->stack.push_back(frame->arguments[static_cast<CHAOS_IL2CPP_SIZE>(instruction.operand_index)]);
                break;
            case IROpCode::LdLoc:
                EnsureLocal(&frame->locals, static_cast<CHAOS_IL2CPP_SIZE>(instruction.operand_index));
                frame->stack.push_back(frame->locals[static_cast<CHAOS_IL2CPP_SIZE>(instruction.operand_index)]);
                break;
            case IROpCode::StLoc: {
                EnsureLocal(&frame->locals, static_cast<CHAOS_IL2CPP_SIZE>(instruction.operand_index));
                frame->locals[static_cast<CHAOS_IL2CPP_SIZE>(instruction.operand_index)] = Pop(&frame->stack);
                break;
            }
            case IROpCode::StArg: {
                if (instruction.operand_index < 0) {
                    throw CHAOS_IL2CPP_OUT_OF_RANGE("argument");
                }

                const CHAOS_IL2CPP_SIZE argument_index = static_cast<CHAOS_IL2CPP_SIZE>(instruction.operand_index);
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
                const CHAOS_IL2CPP_INT32 right = ReadInt32(Pop(&frame->stack));
                const CHAOS_IL2CPP_INT32 left = ReadInt32(Pop(&frame->stack));
                if (left < right) {
                    instruction_index = GetBranchTarget(method, instruction.branch_target);
                    continue;
                }
                break;
            }
            case IROpCode::Bgt: {
                const CHAOS_IL2CPP_INT32 right = ReadInt32(Pop(&frame->stack));
                const CHAOS_IL2CPP_INT32 left = ReadInt32(Pop(&frame->stack));
                if (left > right) {
                    instruction_index = GetBranchTarget(method, instruction.branch_target);
                    continue;
                }
                break;
            }
            case IROpCode::Ble: {
                const CHAOS_IL2CPP_INT32 right = ReadInt32(Pop(&frame->stack));
                const CHAOS_IL2CPP_INT32 left = ReadInt32(Pop(&frame->stack));
                if (left <= right) {
                    instruction_index = GetBranchTarget(method, instruction.branch_target);
                    continue;
                }
                break;
            }
            case IROpCode::Bge: {
                const CHAOS_IL2CPP_INT32 right = ReadInt32(Pop(&frame->stack));
                const CHAOS_IL2CPP_INT32 left = ReadInt32(Pop(&frame->stack));
                if (left >= right) {
                    instruction_index = GetBranchTarget(method, instruction.branch_target);
                    continue;
                }
                break;
            }
            case IROpCode::Add: {
                const CHAOS_IL2CPP_INT32 right = ReadInt32(Pop(&frame->stack));
                const CHAOS_IL2CPP_INT32 left = ReadInt32(Pop(&frame->stack));
                frame->stack.push_back(InterpreterValue::from_i32(left + right));
                break;
            }
            case IROpCode::Sub: {
                const CHAOS_IL2CPP_INT32 right = ReadInt32(Pop(&frame->stack));
                const CHAOS_IL2CPP_INT32 left = ReadInt32(Pop(&frame->stack));
                frame->stack.push_back(InterpreterValue::from_i32(left - right));
                break;
            }
            case IROpCode::Mul: {
                const CHAOS_IL2CPP_INT32 right = ReadInt32(Pop(&frame->stack));
                const CHAOS_IL2CPP_INT32 left = ReadInt32(Pop(&frame->stack));
                frame->stack.push_back(InterpreterValue::from_i32(left * right));
                break;
            }
            case IROpCode::Div: {
                const CHAOS_IL2CPP_INT32 right = ReadInt32(Pop(&frame->stack));
                const CHAOS_IL2CPP_INT32 left = ReadInt32(Pop(&frame->stack));
                frame->stack.push_back(InterpreterValue::from_i32(left / right));
                break;
            }
            case IROpCode::Rem: {
                const CHAOS_IL2CPP_INT32 right = ReadInt32(Pop(&frame->stack));
                const CHAOS_IL2CPP_INT32 left = ReadInt32(Pop(&frame->stack));
                frame->stack.push_back(InterpreterValue::from_i32(left % right));
                break;
            }
            case IROpCode::Neg: {
                const CHAOS_IL2CPP_INT32 value = ReadInt32(Pop(&frame->stack));
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
                const CHAOS_IL2CPP_INT32 right = ReadInt32(Pop(&frame->stack));
                const CHAOS_IL2CPP_INT32 left = ReadInt32(Pop(&frame->stack));
                frame->stack.push_back(InterpreterValue::from_i32(left < right ? 1 : 0));
                break;
            }
            case IROpCode::Cgt: {
                const CHAOS_IL2CPP_INT32 right = ReadInt32(Pop(&frame->stack));
                const CHAOS_IL2CPP_INT32 left = ReadInt32(Pop(&frame->stack));
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
                // Type token is set by the token resolver (or test) via immediate_i4.
                storage->type_token = static_cast<CHAOS_IL2CPP_UINT32>(instruction.immediate_i4);
                frame->stack.push_back(InterpreterValue::from_obj(storage));
                break;
            }
            case IROpCode::NewArr: {
                const CHAOS_IL2CPP_SIZE length = static_cast<CHAOS_IL2CPP_SIZE>(ReadInt32(Pop(&frame->stack)));
                auto* storage = new ArrayStorage();
                storage->elements.resize(length);
                frame->stack.push_back(InterpreterValue::from_obj(storage));
                break;
            }
            case IROpCode::LdFld: {
                const InterpreterValue instance = Pop(&frame->stack);
                if (instance.tag == ValueTag::Struct) {
                    // Struct field access: read bytes at field_offset.
                    // field_size comes from immediate_i4 (set by the codegen emitter).
                    const CHAOS_IL2CPP_SIZE offset   = static_cast<CHAOS_IL2CPP_SIZE>(instruction.field_offset);
                    const CHAOS_IL2CPP_SIZE field_sz = static_cast<CHAOS_IL2CPP_SIZE>(instruction.immediate_i4);
                    if (instance.obj != nullptr && offset + field_sz <= instance.struct_size) {
                        const void* src = static_cast<const char*>(instance.obj) + offset;
                        if (field_sz <= sizeof(CHAOS_IL2CPP_INT64)) {
                            // Small field: copy as appropriate integer type.
                            CHAOS_IL2CPP_INT64 raw = 0;
                            std::memcpy(&raw, src, field_sz);
                            if (field_sz <= 4) {
                                frame->stack.push_back(InterpreterValue::from_i32(static_cast<CHAOS_IL2CPP_INT32>(raw)));
                            } else {
                                frame->stack.push_back(InterpreterValue::from_i64(raw));
                            }
                        } else {
                            // Large field: copy as struct (from_struct will malloc+copy internally).
                            auto val = InterpreterValue::from_struct(src, static_cast<CHAOS_IL2CPP_UINT32>(field_sz));
                            frame->stack.push_back(val);
                        }
                    } else {
                        frame->stack.push_back(InterpreterValue::null_val());
                    }
                } else {
                    // Object field access (existing behavior).
                    auto* object = RequireObject(instance);
                    if (object->fields.size() <= instruction.field_offset) {
                        object->fields.resize(instruction.field_offset + 1u);
                    }
                    frame->stack.push_back(object->fields[instruction.field_offset]);
                }
                break;
            }
            case IROpCode::StFld: {
                const InterpreterValue value = Pop(&frame->stack);
                const InterpreterValue instance = Pop(&frame->stack);
                if (instance.tag == ValueTag::Struct) {
                    // Struct field write: write bytes at field_offset.
                    const CHAOS_IL2CPP_SIZE offset = static_cast<CHAOS_IL2CPP_SIZE>(instruction.field_offset);
                    if (instance.obj != nullptr && offset <= instance.struct_size) {
                        void* dst = static_cast<char*>(instance.obj) + offset;
                        CHAOS_IL2CPP_INT64 raw = 0;
                        if (value.tag == ValueTag::Int32 || value.tag == ValueTag::Float32) {
                            raw = value.i32;
                        } else if (value.tag == ValueTag::Int64 || value.tag == ValueTag::Float64) {
                            raw = value.i64;
                        } else {
                            raw = reinterpret_cast<CHAOS_IL2CPP_INT64>(value.obj);
                        }
                        const CHAOS_IL2CPP_SIZE write_size = (instance.struct_size - offset < sizeof(void*))
                            ? static_cast<CHAOS_IL2CPP_SIZE>(instance.struct_size - offset)
                            : sizeof(void*);
                        std::memcpy(dst, &raw, write_size);
                    }
                } else {
                    // Object field access (existing behavior).
                    auto* object = RequireObject(instance);
                    if (object->fields.size() <= instruction.field_offset) {
                        object->fields.resize(instruction.field_offset + 1u);
                    }
                    object->fields[instruction.field_offset] = value;
                }
                break;
            }
            case IROpCode::LdElem: {
                const CHAOS_IL2CPP_SIZE index = static_cast<CHAOS_IL2CPP_SIZE>(ReadInt32(Pop(&frame->stack)));
                auto* array = RequireArray(Pop(&frame->stack));
                if (index >= array->elements.size()) {
                    throw CHAOS_IL2CPP_OUT_OF_RANGE("array_index");
                }
                frame->stack.push_back(array->elements[index]);
                break;
            }
            case IROpCode::StElem: {
                const InterpreterValue value = Pop(&frame->stack);
                const CHAOS_IL2CPP_SIZE index = static_cast<CHAOS_IL2CPP_SIZE>(ReadInt32(Pop(&frame->stack)));
                auto* array = RequireArray(Pop(&frame->stack));
                if (index >= array->elements.size()) {
                    throw CHAOS_IL2CPP_OUT_OF_RANGE("array_index");
                }
                array->elements[index] = value;
                break;
            }
            case IROpCode::LdLen: {
                auto* array = RequireArray(Pop(&frame->stack));
                frame->stack.push_back(InterpreterValue::from_i32(static_cast<CHAOS_IL2CPP_INT32>(array->elements.size())));
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
                    throw CHAOS_IL2CPP_RUNTIME_ERROR("boxed value required");
                }
                frame->stack.push_back(static_cast<BoxedValue*>(boxed_value.obj)->value);
                break;
            }
            case IROpCode::CastClass:
            case IROpCode::IsInst:
                frame->stack.push_back(Pop(&frame->stack));
                break;
            case IROpCode::Call:
            case IROpCode::CallBridge: {
                // External dispatch: collect call args from the stack.
                result.needs_external_dispatch = true;
                result.call_target = instruction.call_target;

                const CHAOS_IL2CPP_SIZE arg_count = static_cast<CHAOS_IL2CPP_SIZE>(instruction.arg_count);
                result.call_args.resize(arg_count);
                for (CHAOS_IL2CPP_SIZE ai = arg_count; ai > 0u; --ai) {
                    result.call_args[ai - 1u] = Pop(&frame->stack);
                }
                return result;
            }
            case IROpCode::CallVirt: {
                // Collect call args first (last arg pushed first in IL).
                const CHAOS_IL2CPP_SIZE arg_count = static_cast<CHAOS_IL2CPP_SIZE>(instruction.arg_count);
                result.call_args.resize(arg_count);
                for (CHAOS_IL2CPP_SIZE ai = arg_count; ai > 0u; --ai) {
                    result.call_args[ai - 1u] = Pop(&frame->stack);
                }

                // If instance method, resolve virtual dispatch.
                if (arg_count > 0u) {
                    const InterpreterValue& this_val = result.call_args[0u];
                    CHAOS_IL2CPP_UINT32 instance_type_token = 0u;
                    if (this_val.tag == ValueTag::ObjectRef && this_val.obj != nullptr) {
                        instance_type_token = static_cast<InterpreterObject*>(this_val.obj)->type_token;
                    }

                    const CHAOS_IL2CPP_UINT32 declared_method_token =
                        static_cast<CHAOS_IL2CPP_UINT32>(instruction.secondary_index);

                    if (instance_type_token != 0u && declared_method_token != 0u) {
                        void* resolved = chaos::il2cpp::vtable_registry::ResolveVirtualMethodPointer(
                            instance_type_token, declared_method_token);
                        if (resolved != nullptr) {
                            result.call_target = resolved;
                        } else {
                            // Fall back to declared method if vtable resolution fails.
                            result.call_target = instruction.call_target;
                        }
                    } else {
                        result.call_target = instruction.call_target;
                    }
                } else {
                    result.call_target = instruction.call_target;
                }

                result.needs_external_dispatch = true;
                return result;
            }
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
            case IROpCode::Rethrow: {
                // Pop the exception object.
                exception_obj = Pop(&frame->stack);

                // Phase 1: Search for a matching catch handler (innermost first).
                int catch_idx = -1;
                for (int i = static_cast<int>(method.seh_clauses.size()) - 1; i >= 0; --i) {
                    const auto& clause = method.seh_clauses[static_cast<CHAOS_IL2CPP_SIZE>(i)];
                    if (instruction_index >= clause.try_start_idx && instruction_index < clause.try_end_idx) {
                        const auto flags = static_cast<uint32_t>(clause.flags);
                        if (flags == static_cast<uint32_t>(SEHFlags::Exception) ||
                            flags == static_cast<uint32_t>(SEHFlags::Filter)) {
                            catch_idx = i;
                            break;
                        }
                    }
                }

                if (catch_idx >= 0) {
                    // Found a catch handler.  Build the unwind list: collect
                    // finally/fault clauses whose try range covers the throw
                    // point and is nested within (or equal to) the catch
                    // clause's try range.
                    const auto& catch_clause = method.seh_clauses[static_cast<CHAOS_IL2CPP_SIZE>(catch_idx)];
                    unwind_finally_count = 0;
                    for (int i = 0; i < static_cast<int>(method.seh_clauses.size()); ++i) {
                        if (i == catch_idx) continue;
                        const auto& clause = method.seh_clauses[static_cast<CHAOS_IL2CPP_SIZE>(i)];
                        const auto flags = static_cast<uint32_t>(clause.flags);
                        if (flags == static_cast<uint32_t>(SEHFlags::Finally) ||
                            flags == static_cast<uint32_t>(SEHFlags::Fault)) {
                            if (instruction_index >= clause.try_start_idx &&
                                instruction_index < clause.try_end_idx &&
                                clause.try_start_idx >= catch_clause.try_start_idx &&
                                clause.try_end_idx <= catch_clause.try_end_idx &&
                                unwind_finally_count < kMaxUnwindDepth) {
                                unwind_finally_list[unwind_finally_count++] = i;
                            }
                        }
                    }

                    if (unwind_finally_count > 0) {
                        // Phase 2: start unwinding through finally/fault handlers.
                        exception_in_flight = true;
                        unwind_catch_clause = catch_idx;
                        unwind_finally_current = 0;
                        const int first_finally = unwind_finally_list[0];
                        instruction_index = method.seh_clauses[static_cast<CHAOS_IL2CPP_SIZE>(first_finally)].handler_start_idx;
                        in_handler = true;
                        active_handler_clause = first_finally;
                    } else {
                        // No finally blocks — jump directly to catch handler.
                        instruction_index = catch_clause.handler_start_idx;
                        in_handler = true;
                        active_handler_clause = catch_idx;
                        frame->stack.push_back(exception_obj);
                    }
                } else {
                    // No catch handler — propagate to caller.
                    result.threw_exception = true;
                    return result;
                }
                continue;
            }
            case IROpCode::Leave: {
                // Check if leaving a try block that has a finally handler.
                const int finally_idx = FindEnclosingFinally(method, instruction_index);
                if (finally_idx >= 0) {
                    const auto& clause = method.seh_clauses[static_cast<CHAOS_IL2CPP_SIZE>(finally_idx)];
                    pending_leave = true;
                    pending_leave_target = instruction.branch_target;
                    in_handler = true;
                    active_handler_clause = finally_idx;
                    instruction_index = clause.handler_start_idx;
                    continue;
                }
                // No finally — normal branch.
                instruction_index = GetBranchTarget(method, instruction.branch_target);
                continue;
            }
            case IROpCode::EndFinally:
            case IROpCode::EndFilter: {
                if (exception_in_flight) {
                    // Phase 2 unwind: advance to the next finally/fault handler.
                    unwind_finally_current++;
                    if (unwind_finally_current < unwind_finally_count) {
                        const int next_finally = unwind_finally_list[unwind_finally_current];
                        instruction_index = method.seh_clauses[static_cast<CHAOS_IL2CPP_SIZE>(next_finally)].handler_start_idx;
                        active_handler_clause = next_finally;
                        continue;
                    } else {
                        // All finally/fault handlers done — transfer to catch.
                        const auto& catch_clause = method.seh_clauses[static_cast<CHAOS_IL2CPP_SIZE>(unwind_catch_clause)];
                        instruction_index = catch_clause.handler_start_idx;
                        in_handler = true;
                        active_handler_clause = unwind_catch_clause;
                        frame->stack.push_back(exception_obj);
                        exception_in_flight = false;
                        continue;
                    }
                } else if (pending_leave) {
                    // Resume from the pending leave branch target.
                    instruction_index = GetBranchTarget(method, pending_leave_target);
                    pending_leave = false;
                    in_handler = false;
                    active_handler_clause = -1;
                    continue;
                }
                // Normal endfinally — continue sequentially.
                in_handler = false;
                active_handler_clause = -1;
                break;
            }
            // ── Phase A: new opcodes ──────────────────────────────────────
            case IROpCode::Dup: {
                if (frame->stack.empty()) {
                    throw CHAOS_IL2CPP_RUNTIME_ERROR("dup on empty stack");
                }
                frame->stack.push_back(frame->stack.back());
                break;
            }
            case IROpCode::DivUn: {
                const CHAOS_IL2CPP_UINT32 right = static_cast<CHAOS_IL2CPP_UINT32>(ReadInt32(Pop(&frame->stack)));
                const CHAOS_IL2CPP_UINT32 left  = static_cast<CHAOS_IL2CPP_UINT32>(ReadInt32(Pop(&frame->stack)));
                frame->stack.push_back(InterpreterValue::from_i32(static_cast<CHAOS_IL2CPP_INT32>(left / right)));
                break;
            }
            case IROpCode::RemUn: {
                const CHAOS_IL2CPP_UINT32 right = static_cast<CHAOS_IL2CPP_UINT32>(ReadInt32(Pop(&frame->stack)));
                const CHAOS_IL2CPP_UINT32 left  = static_cast<CHAOS_IL2CPP_UINT32>(ReadInt32(Pop(&frame->stack)));
                frame->stack.push_back(InterpreterValue::from_i32(static_cast<CHAOS_IL2CPP_INT32>(left % right)));
                break;
            }
            case IROpCode::And: {
                const CHAOS_IL2CPP_INT32 right = ReadInt32(Pop(&frame->stack));
                const CHAOS_IL2CPP_INT32 left  = ReadInt32(Pop(&frame->stack));
                frame->stack.push_back(InterpreterValue::from_i32(left & right));
                break;
            }
            case IROpCode::Or: {
                const CHAOS_IL2CPP_INT32 right = ReadInt32(Pop(&frame->stack));
                const CHAOS_IL2CPP_INT32 left  = ReadInt32(Pop(&frame->stack));
                frame->stack.push_back(InterpreterValue::from_i32(left | right));
                break;
            }
            case IROpCode::Xor: {
                const CHAOS_IL2CPP_INT32 right = ReadInt32(Pop(&frame->stack));
                const CHAOS_IL2CPP_INT32 left  = ReadInt32(Pop(&frame->stack));
                frame->stack.push_back(InterpreterValue::from_i32(left ^ right));
                break;
            }
            case IROpCode::Not: {
                const CHAOS_IL2CPP_INT32 value = ReadInt32(Pop(&frame->stack));
                frame->stack.push_back(InterpreterValue::from_i32(~value));
                break;
            }
            case IROpCode::Shl: {
                const CHAOS_IL2CPP_INT32 amount = ReadInt32(Pop(&frame->stack));
                const CHAOS_IL2CPP_INT32 value  = ReadInt32(Pop(&frame->stack));
                frame->stack.push_back(InterpreterValue::from_i32(value << (amount & 0x1F)));
                break;
            }
            case IROpCode::Shr: {
                const CHAOS_IL2CPP_INT32 amount = ReadInt32(Pop(&frame->stack));
                const CHAOS_IL2CPP_INT32 value  = ReadInt32(Pop(&frame->stack));
                frame->stack.push_back(InterpreterValue::from_i32(value >> (amount & 0x1F)));
                break;
            }
            case IROpCode::ShrUn: {
                const CHAOS_IL2CPP_INT32 amount = ReadInt32(Pop(&frame->stack));
                const CHAOS_IL2CPP_UINT32 value = static_cast<CHAOS_IL2CPP_UINT32>(ReadInt32(Pop(&frame->stack)));
                frame->stack.push_back(InterpreterValue::from_i32(static_cast<CHAOS_IL2CPP_INT32>(value >> (amount & 0x1F))));
                break;
            }
            case IROpCode::ConvRUn: {
                const CHAOS_IL2CPP_UINT32 value = static_cast<CHAOS_IL2CPP_UINT32>(ReadInt32(Pop(&frame->stack)));
                frame->stack.push_back(InterpreterValue::from_f64(static_cast<double>(value)));
                break;
            }
            case IROpCode::ConvI:
                frame->stack.push_back(InterpreterValue::from_i32(ReadInt32(Pop(&frame->stack))));
                break;
            case IROpCode::ConvU: {
                const CHAOS_IL2CPP_UINT32 value = static_cast<CHAOS_IL2CPP_UINT32>(ReadInt32(Pop(&frame->stack)));
                frame->stack.push_back(InterpreterValue::from_i32(static_cast<CHAOS_IL2CPP_INT32>(value)));
                break;
            }
            case IROpCode::LdInd: {
                const InterpreterValue addr_val = Pop(&frame->stack);
                // ldind.* dereferences a pointer. In the stack-based interpreter
                // model, we reinterpret the value according to the type discriminator.
                switch (instruction.immediate_i4) {
                    case 0: case 1: case 2: case 3:  // i1, u1, i2, u2
                    case 4: case 5:  // i4, u4
                    default:
                        frame->stack.push_back(InterpreterValue::from_i32(ReadInt32(addr_val)));
                        break;
                    case 6:  // i8
                        frame->stack.push_back(InterpreterValue::from_i64(ReadInt64(addr_val)));
                        break;
                    case 7:  // i (native int)
                        frame->stack.push_back(InterpreterValue::from_i32(ReadInt32(addr_val)));
                        break;
                    case 8:  // r4
                        frame->stack.push_back(InterpreterValue::from_f32(ReadFloat32(addr_val)));
                        break;
                    case 9:  // r8
                        frame->stack.push_back(InterpreterValue::from_f64(ReadFloat64(addr_val)));
                        break;
                    case 10: // ref
                        frame->stack.push_back(addr_val);
                        break;
                }
                break;
            }
            case IROpCode::StInd: {
                // stind.*: pop value then address; in our stack model, discard both.
                (void)Pop(&frame->stack); // value
                (void)Pop(&frame->stack); // address
                break;
            }
            case IROpCode::Switch: {
                const CHAOS_IL2CPP_INT32 index = ReadInt32(Pop(&frame->stack));
                // Simplified: go to the default target.
                // Full implementation would store case targets.
                if (index < 0 || static_cast<CHAOS_IL2CPP_UINT32>(index) >= instruction.secondary_index) {
                    if (instruction.branch_target != static_cast<CHAOS_IL2CPP_SIZE>(-1)) {
                        instruction_index = GetBranchTarget(method, instruction.branch_target);
                        continue;
                    }
                }
                // Fall through if no default target.
                break;
            }
            case IROpCode::LdToken: {
                // Push the raw metadata token as a placeholder RuntimeHandle.
                frame->stack.push_back(InterpreterValue::from_i32(instruction.immediate_i4));
                break;
            }
            case IROpCode::InitObj: {
                const InterpreterValue addr = Pop(&frame->stack);
                if (addr.tag == ValueTag::Struct && addr.obj != nullptr) {
                    std::memset(addr.obj, 0, addr.struct_size);
                }
                // For value types on stack, initobj is a no-op (already default).
                break;
            }
            case IROpCode::SizeOf:
                frame->stack.push_back(InterpreterValue::from_i32(instruction.immediate_i4));
                break;
            case IROpCode::LdFtn:
                frame->stack.push_back(InterpreterValue::from_obj(instruction.call_target));
                break;
            case IROpCode::LdVirtFtn: {
                // Pop the object, then push the virtual function pointer.
                (void)Pop(&frame->stack);
                frame->stack.push_back(InterpreterValue::null_val());
                break;
            }
            case IROpCode::LdArgA: {
                if (instruction.operand_index < 0 ||
                    static_cast<CHAOS_IL2CPP_SIZE>(instruction.operand_index) >= frame->arguments.size()) {
                    throw CHAOS_IL2CPP_OUT_OF_RANGE("argument");
                }
                // In our stack model, load argument value.
                frame->stack.push_back(frame->arguments[static_cast<CHAOS_IL2CPP_SIZE>(instruction.operand_index)]);
                break;
            }
            case IROpCode::LdLocA: {
                EnsureLocal(&frame->locals, static_cast<CHAOS_IL2CPP_SIZE>(instruction.operand_index));
                frame->stack.push_back(frame->locals[static_cast<CHAOS_IL2CPP_SIZE>(instruction.operand_index)]);
                break;
            }
            case IROpCode::LocAlloc: {
                const CHAOS_IL2CPP_SIZE size = static_cast<CHAOS_IL2CPP_SIZE>(ReadInt32(Pop(&frame->stack)));
                void* buf = std::malloc(size > 0u ? size : 1u);
                if (buf != nullptr) {
                    std::memset(buf, 0, size > 0u ? size : 1u);
                }
                frame->stack.push_back(InterpreterValue::from_obj(buf));
                break;
            }
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
                throw CHAOS_IL2CPP_RUNTIME_ERROR("unsupported opcode");
        }

        ++instruction_index;
    }

    return result;
}

}  // namespace chaos::il2cpp::interpreter
