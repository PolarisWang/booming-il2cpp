#include "interpreter_vm.h"

#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <vector>

#include "vtable_registry.h"

namespace chaos::il2cpp::interpreter {

namespace {

using ObjectStorage = InterpreterObject;

/// Max argument count for stack-allocated call_args buffer in Call handler.
/// The vast majority of managed methods have < 8 parameters.
static constexpr CHAOS_IL2CPP_UINT32 kMaxCallArgs = 8u;

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
    // Free localloc allocations.
    for (auto* block : localloc_blocks) {
        std::free(block);
    }
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

    // ── SEH helper lambdas (capture locals by reference) ──────────────────

    // Phase 1: find the innermost catch/filter handler covering ip.
    auto findCatchHandler = [&](CHAOS_IL2CPP_SIZE ip) -> int {
        for (int i = static_cast<int>(method.seh_clauses.size()) - 1; i >= 0; --i) {
            const auto& clause = method.seh_clauses[static_cast<CHAOS_IL2CPP_SIZE>(i)];
            if (ip >= clause.try_start_idx && ip < clause.try_end_idx) {
                const auto flags = static_cast<uint32_t>(clause.flags);
                if (flags == static_cast<uint32_t>(SEHFlags::Exception) ||
                    flags == static_cast<uint32_t>(SEHFlags::Filter)) {
                    return i;
                }
            }
        }
        return -1;
    };

    // Phase 2: build the finally/fault unwind list for a given catch clause.
    // Stores indices in unwind_finally_list and returns the count.
    auto setupFinallyUnwind = [&](int catch_idx, CHAOS_IL2CPP_SIZE ip) {
        unwind_finally_count = 0;
        const auto& catch_clause = method.seh_clauses[static_cast<CHAOS_IL2CPP_SIZE>(catch_idx)];
        for (int i = 0; i < static_cast<int>(method.seh_clauses.size()); ++i) {
            if (i == catch_idx) continue;
            const auto& clause = method.seh_clauses[static_cast<CHAOS_IL2CPP_SIZE>(i)];
            const auto flags = static_cast<uint32_t>(clause.flags);
            if (flags == static_cast<uint32_t>(SEHFlags::Finally) ||
                flags == static_cast<uint32_t>(SEHFlags::Fault)) {
                if (ip >= clause.try_start_idx &&
                    ip < clause.try_end_idx &&
                    clause.try_start_idx >= catch_clause.try_start_idx &&
                    clause.try_end_idx <= catch_clause.try_end_idx &&
                    unwind_finally_count < kMaxUnwindDepth) {
                    unwind_finally_list[unwind_finally_count++] = i;
                }
            }
        }
    };

    // ── Action for handleDispatchResult — controls switch flow in call handlers. ──
    enum class DispatchAction { Return, Continue, Break };

    // Shared post-dispatch handler for Call/CallVirt/CallBridge.
    // Encapsulates SEH catch/finally dispatch and tail-call optimization.
    // Called after frame->dispatch_fn returns a DispatchResult.
    // Returns DispatchAction to control the outer switch/loop flow.
    auto handleDispatchResult = [&](const DispatchResult& dret) -> DispatchAction {
        if (dret.threw_exception) {
            exception_obj = dret.exception_value;
            const int catch_idx = findCatchHandler(instruction_index);
            if (catch_idx >= 0) {
                const auto& catch_clause = method.seh_clauses[
                    static_cast<CHAOS_IL2CPP_SIZE>(catch_idx)];
                setupFinallyUnwind(catch_idx, instruction_index);
                if (unwind_finally_count > 0) {
                    exception_in_flight = true;
                    unwind_catch_clause = catch_idx;
                    unwind_finally_current = 0;
                    instruction_index = method.seh_clauses[
                        static_cast<CHAOS_IL2CPP_SIZE>(unwind_finally_list[0])].handler_start_idx;
                    in_handler = true;
                    active_handler_clause = unwind_finally_list[0];
                } else {
                    instruction_index = catch_clause.handler_start_idx;
                    in_handler = true;
                    active_handler_clause = catch_idx;
                    frame->stack.push_back(exception_obj);
                }
                return DispatchAction::Continue;
            }
            // No catch handler — propagate to caller.
            result.threw_exception = true;
            result.exception_value = exception_obj;
            return DispatchAction::Return;
        }

        if (dret.has_value) {
            // Tail call: if the next instruction is Ret, skip the stack round-trip.
            if (instruction_index + 1u < method.instructions.size() &&
                method.instructions[instruction_index + 1u].op_code == IROpCode::Ret) {
                result.has_return_value = true;
                result.return_value = dret.value;
                return DispatchAction::Return;
            }
            frame->stack.push_back(dret.value);
        }
        return DispatchAction::Break;
    };

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
                        const CHAOS_IL2CPP_SIZE remaining = instance.struct_size - offset;
                        if (value.tag == ValueTag::Struct && value.obj != nullptr) {
                            // Large struct field: copy full struct data (clamped to remaining space).
                            const CHAOS_IL2CPP_SIZE copy_size = (value.struct_size < remaining)
                                ? value.struct_size : remaining;
                            std::memcpy(dst, value.obj, copy_size);
                        } else {
                            // Small field: read from scalar value.
                            CHAOS_IL2CPP_INT64 raw = 0;
                            if (value.tag == ValueTag::Int32 || value.tag == ValueTag::Float32) {
                                raw = value.i32;
                            } else if (value.tag == ValueTag::Int64 || value.tag == ValueTag::Float64) {
                                raw = value.i64;
                            } else {
                                raw = reinterpret_cast<CHAOS_IL2CPP_INT64>(value.obj);
                            }
                            const CHAOS_IL2CPP_SIZE write_size = (remaining < sizeof(void*))
                                ? remaining
                                : sizeof(void*);
                            std::memcpy(dst, &raw, write_size);
                        }
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
            case IROpCode::IsInst: {
                const InterpreterValue cast_val = Pop(&frame->stack);
                // Null: CastClass passes through, IsInst returns null
                if (cast_val.tag == ValueTag::Null) {
                    if (instruction.op_code == IROpCode::IsInst) {
                        frame->stack.push_back(InterpreterValue::null_val());
                    } else {
                        frame->stack.push_back(cast_val);
                    }
                    break;
                }
                // Non-object: CastClass passes through, IsInst returns null
                if (cast_val.tag != ValueTag::ObjectRef || cast_val.obj == nullptr) {
                    if (instruction.op_code == IROpCode::IsInst) {
                        frame->stack.push_back(InterpreterValue::null_val());
                    } else {
                        frame->stack.push_back(cast_val);
                    }
                    break;
                }
                // Target type token stored in immediate_i4 by the IR builder
                const CHAOS_IL2CPP_UINT32 target_type_token =
                    static_cast<CHAOS_IL2CPP_UINT32>(instruction.immediate_i4);
                if (target_type_token == 0u) {
                    // No target type info — pass through (safe fallback)
                    frame->stack.push_back(cast_val);
                    break;
                }
                // Walk the inheritance chain using vtable_registry
                auto* obj = static_cast<InterpreterObject*>(cast_val.obj);
                const CHAOS_IL2CPP_UINT32 obj_type_token = obj->type_token;
                bool compatible = false;
                CHAOS_IL2CPP_UINT32 current_token = obj_type_token;
                while (current_token != 0u) {
                    if (current_token == target_type_token) {
                        compatible = true;
                        break;
                    }
                    // Walk to base type via vtable_registry
                    const auto* vtable = chaos::il2cpp::vtable_registry::TryGetTypeVTable(current_token);
                    if (vtable == nullptr) {
                        break;
                    }
                    current_token = vtable->base_token;
                }
                // Interface compatibility check: also scan implemented interfaces
                // (Phase A+: add isinst/castclass interface check when iface_map is available)
                if (compatible) {
                    frame->stack.push_back(cast_val);
                } else if (instruction.op_code == IROpCode::IsInst) {
                    frame->stack.push_back(InterpreterValue::null_val());
                } else {
                    throw CHAOS_IL2CPP_RUNTIME_ERROR("InvalidCastException: type mismatch in CastClass");
                }
                break;
            }
            case IROpCode::Call:
            case IROpCode::CallBridge: {
                const CHAOS_IL2CPP_SIZE arg_count = static_cast<CHAOS_IL2CPP_SIZE>(instruction.arg_count);

                if (frame->dispatch_fn != nullptr) {
                    // Inline dispatch via callback (Phase 4+).
                    // Stack-allocate for small arg counts (common case).
                    InterpreterValue local_buf[kMaxCallArgs];
                    auto* arg_buf = (arg_count <= kMaxCallArgs)
                        ? local_buf
                        : static_cast<InterpreterValue*>(std::malloc(
                            sizeof(InterpreterValue) * arg_count));

                    for (CHAOS_IL2CPP_SIZE ai = arg_count; ai > 0u; --ai) {
                        arg_buf[ai - 1u] = Pop(&frame->stack);
                    }

                    DispatchResult dret = frame->dispatch_fn(
                        instruction.call_target, arg_buf,
                        static_cast<CHAOS_IL2CPP_UINT32>(arg_count),
                        instruction.is_instance_call,
                        frame->dispatch_context);

                    if (arg_count > kMaxCallArgs) std::free(arg_buf);

                    const DispatchAction da = handleDispatchResult(dret);
                    if (da == DispatchAction::Return) return result;
                    if (da == DispatchAction::Continue) continue;
                    break;
                }

                // Fallback: existing external-dispatch behavior.
                result.needs_external_dispatch = true;
                result.call_target = instruction.call_target;
                result.call_args.resize(arg_count);
                for (CHAOS_IL2CPP_SIZE ai = arg_count; ai > 0u; --ai) {
                    result.call_args[ai - 1u] = Pop(&frame->stack);
                }
                return result;
            }
            case IROpCode::CallVirtConstrained: {
                // Constrained CallVirt: the .constrained. prefix indicates that
                // if 'this' is a value type, the call should be made directly
                // (without boxing). For reference types, normal virtual dispatch.
                const CHAOS_IL2CPP_SIZE cv_arg_count = static_cast<CHAOS_IL2CPP_SIZE>(instruction.arg_count);

                // Collect call args (needed by both paths).
                InterpreterValue local_buf[kMaxCallArgs];
                auto* cv_args = (cv_arg_count <= kMaxCallArgs)
                    ? local_buf
                    : static_cast<InterpreterValue*>(std::malloc(
                        sizeof(InterpreterValue) * cv_arg_count));
                for (CHAOS_IL2CPP_SIZE ai = cv_arg_count; ai > 0u; --ai) {
                    cv_args[ai - 1u] = Pop(&frame->stack);
                }

                // Resolve call target (vtable or direct).
                void* resolved_target = instruction.call_target;
                if (cv_arg_count > 0u) {
                    const InterpreterValue& this_val = cv_args[0u];
                    if (this_val.tag != ValueTag::Struct) {
                        // Reference or boxed value — normal virtual dispatch.
                        CHAOS_IL2CPP_UINT32 inst_type_token = 0u;
                        if (this_val.tag == ValueTag::ObjectRef && this_val.obj != nullptr) {
                            inst_type_token = static_cast<InterpreterObject*>(this_val.obj)->type_token;
                        }
                        const CHAOS_IL2CPP_UINT32 decl_method_token =
                            static_cast<CHAOS_IL2CPP_UINT32>(instruction.secondary_index);
                        if (inst_type_token != 0u && decl_method_token != 0u) {
                            void* vtable_resolved = chaos::il2cpp::vtable_registry::ResolveVirtualMethodPointer(
                                inst_type_token, decl_method_token);
                            if (vtable_resolved != nullptr) {
                                resolved_target = vtable_resolved;
                            }
                        }
                    }
                }

                if (frame->dispatch_fn != nullptr) {
                    DispatchResult dret = frame->dispatch_fn(
                        resolved_target, cv_args,
                        static_cast<CHAOS_IL2CPP_UINT32>(cv_arg_count),
                        true, /* CallVirtConstrained is always instance */
                        frame->dispatch_context);

                    if (cv_arg_count > kMaxCallArgs) std::free(cv_args);

                    const DispatchAction da_cv = handleDispatchResult(dret);
                    if (da_cv == DispatchAction::Return) return result;
                    if (da_cv == DispatchAction::Continue) continue;
                    break;
                }

                // Fallback: existing external-dispatch behavior.
                // Copy args BEFORE freeing the heap buffer (if any).
                result.call_args.resize(cv_arg_count);
                for (CHAOS_IL2CPP_SIZE ai = 0u; ai < cv_arg_count; ++ai) {
                    result.call_args[ai] = cv_args[ai];
                }
                if (cv_arg_count > kMaxCallArgs) std::free(cv_args);
                result.call_target = resolved_target;
                result.needs_external_dispatch = true;
                return result;
            }
            case IROpCode::CallVirt: {
                const CHAOS_IL2CPP_SIZE arg_count_v = static_cast<CHAOS_IL2CPP_SIZE>(instruction.arg_count);

                // Collect call args.
                InterpreterValue local_buf_v[kMaxCallArgs];
                auto* cv_args_v = (arg_count_v <= kMaxCallArgs)
                    ? local_buf_v
                    : static_cast<InterpreterValue*>(std::malloc(
                        sizeof(InterpreterValue) * arg_count_v));
                for (CHAOS_IL2CPP_SIZE ai = arg_count_v; ai > 0u; --ai) {
                    cv_args_v[ai - 1u] = Pop(&frame->stack);
                }

                // Resolve virtual dispatch.
                void* resolved_target_v = instruction.call_target;
                if (arg_count_v > 0u) {
                    const InterpreterValue& this_val = cv_args_v[0u];
                    CHAOS_IL2CPP_UINT32 instance_type_token = 0u;
                    if (this_val.tag == ValueTag::ObjectRef && this_val.obj != nullptr) {
                        instance_type_token = static_cast<InterpreterObject*>(this_val.obj)->type_token;
                    }

                    const CHAOS_IL2CPP_UINT32 declared_method_token =
                        static_cast<CHAOS_IL2CPP_UINT32>(instruction.secondary_index);

                    if (instance_type_token != 0u && declared_method_token != 0u) {
                        void* vtable_resolved = chaos::il2cpp::vtable_registry::ResolveVirtualMethodPointer(
                            instance_type_token, declared_method_token);
                        if (vtable_resolved != nullptr) {
                            resolved_target_v = vtable_resolved;
                        }
                    }
                }

                if (frame->dispatch_fn != nullptr) {
                    DispatchResult dret_v = frame->dispatch_fn(
                        resolved_target_v, cv_args_v,
                        static_cast<CHAOS_IL2CPP_UINT32>(arg_count_v),
                        true, /* CallVirt is always instance */
                        frame->dispatch_context);

                    if (arg_count_v > kMaxCallArgs) std::free(cv_args_v);

                    const DispatchAction da_v = handleDispatchResult(dret_v);
                    if (da_v == DispatchAction::Return) return result;
                    if (da_v == DispatchAction::Continue) continue;
                    break;
                }

                // Fallback: existing external-dispatch behavior.
                // Copy args BEFORE freeing the heap buffer (if any).
                result.call_args.resize(arg_count_v);
                for (CHAOS_IL2CPP_SIZE ai = 0u; ai < arg_count_v; ++ai) {
                    result.call_args[ai] = cv_args_v[ai];
                }
                if (arg_count_v > kMaxCallArgs) std::free(cv_args_v);
                result.call_target = resolved_target_v;
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
                const int catch_idx = findCatchHandler(instruction_index);

                if (catch_idx >= 0) {
                    // Found a catch handler.  Build the unwind list.
                    const auto& catch_clause = method.seh_clauses[static_cast<CHAOS_IL2CPP_SIZE>(catch_idx)];
                    setupFinallyUnwind(catch_idx, instruction_index);

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
                    result.exception_value = exception_obj;
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
                // stind.*: pop value then address; write value bytes to address.
                const InterpreterValue stind_val = Pop(&frame->stack);
                const InterpreterValue stind_addr = Pop(&frame->stack);
                // Determine write size from type discriminator (immediate_i4).
                CHAOS_IL2CPP_SIZE stind_write_size = sizeof(void*);
                switch (instruction.immediate_i4) {
                    case 0: case 1: stind_write_size = 1; break;  // i1, u1
                    case 2: case 3: stind_write_size = 2; break;  // i2, u2
                    case 4: case 5: stind_write_size = 4; break;  // i4, u4
                    case 6:          stind_write_size = 8; break;  // i8
                    case 7:          stind_write_size = sizeof(void*); break; // i
                    case 8:          stind_write_size = 4; break;  // r4
                    case 9:          stind_write_size = 8; break;  // r8
                    case 10:         stind_write_size = sizeof(void*); break; // ref
                    default:         stind_write_size = sizeof(void*); break;
                }
                void* stind_dst = (stind_addr.tag == ValueTag::Struct || stind_addr.tag == ValueTag::ObjectRef)
                    ? stind_addr.obj : nullptr;
                if (stind_dst != nullptr) {
                    CHAOS_IL2CPP_INT64 stind_raw = 0;
                    if (stind_val.tag == ValueTag::Int32 || stind_val.tag == ValueTag::Float32) {
                        stind_raw = stind_val.i32;
                    } else if (stind_val.tag == ValueTag::Int64 || stind_val.tag == ValueTag::Float64) {
                        stind_raw = stind_val.i64;
                    } else {
                        stind_raw = reinterpret_cast<CHAOS_IL2CPP_INT64>(stind_val.obj);
                    }
                    std::memcpy(stind_dst, &stind_raw, stind_write_size);
                }
                break;
            }
            case IROpCode::Switch: {
                const CHAOS_IL2CPP_INT32 index = ReadInt32(Pop(&frame->stack));
                // Check if index is within valid range of case targets.
                if (index >= 0 && instruction.switch_targets != nullptr &&
                    static_cast<CHAOS_IL2CPP_UINT32>(index) < instruction.switch_target_count) {
                    instruction_index = instruction.switch_targets[index];
                    continue;
                }
                // Out of range: use default target (branch_target), if set.
                if (instruction.branch_target != static_cast<CHAOS_IL2CPP_SIZE>(-1)) {
                    instruction_index = GetBranchTarget(method, instruction.branch_target);
                    continue;
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
                // Pop the object, then push the resolved virtual function pointer.
                const InterpreterValue ldvirtftn_obj = Pop(&frame->stack);
                if (ldvirtftn_obj.tag != ValueTag::ObjectRef || ldvirtftn_obj.obj == nullptr) {
                    frame->stack.push_back(InterpreterValue::null_val());
                    break;
                }
                const CHAOS_IL2CPP_UINT32 inst_type_token =
                    static_cast<InterpreterObject*>(ldvirtftn_obj.obj)->type_token;
                const CHAOS_IL2CPP_UINT32 decl_method_token =
                    static_cast<CHAOS_IL2CPP_UINT32>(instruction.secondary_index);
                void* resolved = nullptr;
                if (inst_type_token != 0u && decl_method_token != 0u) {
                    resolved = chaos::il2cpp::vtable_registry::ResolveVirtualMethodPointer(
                        inst_type_token, decl_method_token);
                }
                if (resolved != nullptr) {
                    frame->stack.push_back(InterpreterValue::from_obj(resolved));
                } else {
                    // Fallback: push the declared method's call_target
                    frame->stack.push_back(InterpreterValue::from_obj(instruction.call_target));
                }
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
                    // Register for automatic cleanup on frame exit.
                    frame->localloc_blocks.push_back(buf);
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

            // ── Phase A+: Full coverage expansion ─────────────────────────
            case IROpCode::Break:
                // Break (debug opcode) — treat as NOP.
                break;
            case IROpCode::BneUn: {
                const CHAOS_IL2CPP_UINT32 right = static_cast<CHAOS_IL2CPP_UINT32>(ReadInt32(Pop(&frame->stack)));
                const CHAOS_IL2CPP_UINT32 left  = static_cast<CHAOS_IL2CPP_UINT32>(ReadInt32(Pop(&frame->stack)));
                if (left != right) {
                    instruction_index = GetBranchTarget(method, instruction.branch_target);
                    continue;
                }
                break;
            }
            case IROpCode::BgeUn: {
                const CHAOS_IL2CPP_UINT32 right = static_cast<CHAOS_IL2CPP_UINT32>(ReadInt32(Pop(&frame->stack)));
                const CHAOS_IL2CPP_UINT32 left  = static_cast<CHAOS_IL2CPP_UINT32>(ReadInt32(Pop(&frame->stack)));
                if (left >= right) {
                    instruction_index = GetBranchTarget(method, instruction.branch_target);
                    continue;
                }
                break;
            }
            case IROpCode::BgtUn: {
                const CHAOS_IL2CPP_UINT32 right = static_cast<CHAOS_IL2CPP_UINT32>(ReadInt32(Pop(&frame->stack)));
                const CHAOS_IL2CPP_UINT32 left  = static_cast<CHAOS_IL2CPP_UINT32>(ReadInt32(Pop(&frame->stack)));
                if (left > right) {
                    instruction_index = GetBranchTarget(method, instruction.branch_target);
                    continue;
                }
                break;
            }
            case IROpCode::BleUn: {
                const CHAOS_IL2CPP_UINT32 right = static_cast<CHAOS_IL2CPP_UINT32>(ReadInt32(Pop(&frame->stack)));
                const CHAOS_IL2CPP_UINT32 left  = static_cast<CHAOS_IL2CPP_UINT32>(ReadInt32(Pop(&frame->stack)));
                if (left <= right) {
                    instruction_index = GetBranchTarget(method, instruction.branch_target);
                    continue;
                }
                break;
            }
            case IROpCode::BltUn: {
                const CHAOS_IL2CPP_UINT32 right = static_cast<CHAOS_IL2CPP_UINT32>(ReadInt32(Pop(&frame->stack)));
                const CHAOS_IL2CPP_UINT32 left  = static_cast<CHAOS_IL2CPP_UINT32>(ReadInt32(Pop(&frame->stack)));
                if (left < right) {
                    instruction_index = GetBranchTarget(method, instruction.branch_target);
                    continue;
                }
                break;
            }
            case IROpCode::AddOvf: {
                const CHAOS_IL2CPP_INT32 right = ReadInt32(Pop(&frame->stack));
                const CHAOS_IL2CPP_INT32 left  = ReadInt32(Pop(&frame->stack));
                // Promote to 64-bit to detect signed overflow.
                const CHAOS_IL2CPP_INT64 result_64 = static_cast<CHAOS_IL2CPP_INT64>(left) +
                    static_cast<CHAOS_IL2CPP_INT64>(right);
                if (result_64 > static_cast<CHAOS_IL2CPP_INT64>(INT32_MAX) ||
                    result_64 < static_cast<CHAOS_IL2CPP_INT64>(INT32_MIN)) {
                    throw CHAOS_IL2CPP_RUNTIME_ERROR("OverflowException: arithmetic overflow in add.ovf");
                }
                frame->stack.push_back(InterpreterValue::from_i32(static_cast<CHAOS_IL2CPP_INT32>(result_64)));
                break;
            }
            case IROpCode::SubOvf: {
                const CHAOS_IL2CPP_INT32 right = ReadInt32(Pop(&frame->stack));
                const CHAOS_IL2CPP_INT32 left  = ReadInt32(Pop(&frame->stack));
                const CHAOS_IL2CPP_INT64 result_64 = static_cast<CHAOS_IL2CPP_INT64>(left) -
                    static_cast<CHAOS_IL2CPP_INT64>(right);
                if (result_64 > static_cast<CHAOS_IL2CPP_INT64>(INT32_MAX) ||
                    result_64 < static_cast<CHAOS_IL2CPP_INT64>(INT32_MIN)) {
                    throw CHAOS_IL2CPP_RUNTIME_ERROR("OverflowException: arithmetic overflow in sub.ovf");
                }
                frame->stack.push_back(InterpreterValue::from_i32(static_cast<CHAOS_IL2CPP_INT32>(result_64)));
                break;
            }
            case IROpCode::MulOvf: {
                const CHAOS_IL2CPP_INT32 right = ReadInt32(Pop(&frame->stack));
                const CHAOS_IL2CPP_INT32 left  = ReadInt32(Pop(&frame->stack));
                // Use compiler builtin for overflow-checked multiplication.
                CHAOS_IL2CPP_INT32 mul_result = 0;
#if defined(__GNUC__) || defined(__clang__)
                if (__builtin_mul_overflow(left, right, &mul_result)) {
                    throw CHAOS_IL2CPP_RUNTIME_ERROR("OverflowException: arithmetic overflow in mul.ovf");
                }
#else
                // MSVC fallback: promote to 64-bit and check range.
                const CHAOS_IL2CPP_INT64 mul_64 = static_cast<CHAOS_IL2CPP_INT64>(left) *
                    static_cast<CHAOS_IL2CPP_INT64>(right);
                if (mul_64 > static_cast<CHAOS_IL2CPP_INT64>(INT32_MAX) ||
                    mul_64 < static_cast<CHAOS_IL2CPP_INT64>(INT32_MIN)) {
                    throw CHAOS_IL2CPP_RUNTIME_ERROR("OverflowException: arithmetic overflow in mul.ovf");
                }
                mul_result = static_cast<CHAOS_IL2CPP_INT32>(mul_64);
#endif
                frame->stack.push_back(InterpreterValue::from_i32(mul_result));
                break;
            }
            case IROpCode::ConvOvfI: {
                const CHAOS_IL2CPP_INT64 val = ReadInt64(Pop(&frame->stack));
                if (val > static_cast<CHAOS_IL2CPP_INT64>(INT32_MAX) ||
                    val < static_cast<CHAOS_IL2CPP_INT64>(INT32_MIN)) {
                    throw CHAOS_IL2CPP_RUNTIME_ERROR("OverflowException: conv.ovf.i");
                }
                frame->stack.push_back(InterpreterValue::from_i32(static_cast<CHAOS_IL2CPP_INT32>(val)));
                break;
            }
            case IROpCode::ConvOvfI4: {
                const CHAOS_IL2CPP_INT64 val = ReadInt64(Pop(&frame->stack));
                if (val > static_cast<CHAOS_IL2CPP_INT64>(INT32_MAX) ||
                    val < static_cast<CHAOS_IL2CPP_INT64>(INT32_MIN)) {
                    throw CHAOS_IL2CPP_RUNTIME_ERROR("OverflowException: conv.ovf.i4");
                }
                frame->stack.push_back(InterpreterValue::from_i32(static_cast<CHAOS_IL2CPP_INT32>(val)));
                break;
            }
            case IROpCode::ConvOvfI8:
                frame->stack.push_back(InterpreterValue::from_i64(ReadInt64(Pop(&frame->stack))));
                break;
            case IROpCode::ConvOvfU: {
                const CHAOS_IL2CPP_INT64 val = ReadInt64(Pop(&frame->stack));
                if (val < 0 || val > static_cast<CHAOS_IL2CPP_INT64>(UINT32_MAX)) {
                    throw CHAOS_IL2CPP_RUNTIME_ERROR("OverflowException: conv.ovf.u");
                }
                frame->stack.push_back(InterpreterValue::from_i32(static_cast<CHAOS_IL2CPP_INT32>(val)));
                break;
            }
            case IROpCode::ConvOvfU4: {
                const CHAOS_IL2CPP_INT64 val = ReadInt64(Pop(&frame->stack));
                if (val < 0 || val > static_cast<CHAOS_IL2CPP_INT64>(UINT32_MAX)) {
                    throw CHAOS_IL2CPP_RUNTIME_ERROR("OverflowException: conv.ovf.u4");
                }
                frame->stack.push_back(InterpreterValue::from_i32(static_cast<CHAOS_IL2CPP_INT32>(val)));
                break;
            }
            case IROpCode::ConvOvfU8: {
                const CHAOS_IL2CPP_INT64 val = ReadInt64(Pop(&frame->stack));
                if (val < 0) {
                    throw CHAOS_IL2CPP_RUNTIME_ERROR("OverflowException: conv.ovf.u8");
                }
                frame->stack.push_back(InterpreterValue::from_i64(val));
                break;
            }
            case IROpCode::LdObj: {
                // Pop address, push value (similar to LdInd).
                const InterpreterValue addr_val = Pop(&frame->stack);
                frame->stack.push_back(InterpreterValue::from_i32(ReadInt32(addr_val)));
                break;
            }
            case IROpCode::StObj: {
                // StObj: pop value then address; write value bytes to address.
                const InterpreterValue stobj_val = Pop(&frame->stack);
                const InterpreterValue stobj_addr = Pop(&frame->stack);
                void* stobj_dst = (stobj_addr.tag == ValueTag::Struct || stobj_addr.tag == ValueTag::ObjectRef)
                    ? stobj_addr.obj : nullptr;
                if (stobj_dst != nullptr) {
                    CHAOS_IL2CPP_SIZE stobj_write_size = sizeof(void*);
                    const void* stobj_src = nullptr;
                    CHAOS_IL2CPP_INT64 stobj_raw = 0;
                    if (stobj_val.tag == ValueTag::Struct && stobj_val.obj != nullptr) {
                        stobj_src = stobj_val.obj;
                        stobj_write_size = stobj_val.struct_size;
                    } else if (stobj_val.tag == ValueTag::Int32 || stobj_val.tag == ValueTag::Float32) {
                        stobj_raw = stobj_val.i32;
                        stobj_src = &stobj_raw;
                        stobj_write_size = 4;
                    } else if (stobj_val.tag == ValueTag::Int64 || stobj_val.tag == ValueTag::Float64) {
                        stobj_raw = stobj_val.i64;
                        stobj_src = &stobj_raw;
                        stobj_write_size = 8;
                    } else {
                        stobj_raw = reinterpret_cast<CHAOS_IL2CPP_INT64>(stobj_val.obj);
                        stobj_src = &stobj_raw;
                        stobj_write_size = sizeof(void*);
                    }
                    std::memcpy(stobj_dst, stobj_src, stobj_write_size);
                }
                break;
            }
            case IROpCode::LdElemA: {
                // Load element address — in our stack model, push element value.
                const CHAOS_IL2CPP_SIZE index = static_cast<CHAOS_IL2CPP_SIZE>(ReadInt32(Pop(&frame->stack)));
                auto* array = RequireArray(Pop(&frame->stack));
                if (index >= array->elements.size()) {
                    throw CHAOS_IL2CPP_OUT_OF_RANGE("array_index");
                }
                frame->stack.push_back(array->elements[index]);
                break;
            }
            case IROpCode::Cpblk: {
                const CHAOS_IL2CPP_UINT32 cp_size = static_cast<CHAOS_IL2CPP_UINT32>(ReadInt32(Pop(&frame->stack)));
                const InterpreterValue src_val = Pop(&frame->stack);
                const InterpreterValue dst_val = Pop(&frame->stack);
                void* dst_ptr = (dst_val.tag == ValueTag::Struct || dst_val.tag == ValueTag::ObjectRef) ? dst_val.obj : nullptr;
                void* src_ptr = (src_val.tag == ValueTag::Struct || src_val.tag == ValueTag::ObjectRef) ? src_val.obj : nullptr;
                if (dst_ptr != nullptr && src_ptr != nullptr) {
                    std::memcpy(dst_ptr, src_ptr, cp_size);
                }
                break;
            }
            case IROpCode::InitBlk: {
                const CHAOS_IL2CPP_UINT32 init_size = static_cast<CHAOS_IL2CPP_UINT32>(ReadInt32(Pop(&frame->stack)));
                const CHAOS_IL2CPP_INT32 init_value = ReadInt32(Pop(&frame->stack));
                const InterpreterValue addr_val = Pop(&frame->stack);
                void* ptr = (addr_val.tag == ValueTag::Struct || addr_val.tag == ValueTag::ObjectRef) ? addr_val.obj : nullptr;
                if (ptr != nullptr) {
                    std::memset(ptr, init_value, init_size);
                }
                break;
            }
            default:
                throw CHAOS_IL2CPP_RUNTIME_ERROR("unsupported opcode");
        }

        ++instruction_index;
    }

    return result;
}

}  // namespace chaos::il2cpp::interpreter
