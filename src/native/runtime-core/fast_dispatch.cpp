#include "fast_dispatch.h"

#include "runtime_instantiation.h"
namespace ri = chaos::il2cpp::runtime_instantiation;

#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace chaos::il2cpp::runtime_core {

// ── InterpreterValue ↔ FastFrame conversion ────────────────────────────

void FastFrame::PushIV(const interpreter::InterpreterValue& iv) noexcept {
    switch (iv.tag) {
    case interpreter::ValueTag::Int32:
        PushI32(iv.i32); break;
    case interpreter::ValueTag::Int64:
        PushI64(iv.i64); break;
    case interpreter::ValueTag::Float32:
        PushF32(iv.f32); break;
    case interpreter::ValueTag::Float64:
        PushF64(iv.f64); break;
    case interpreter::ValueTag::Null:
        PushNull(); break;
    default:
        PushObj(iv.obj); break;
    }
}

interpreter::InterpreterValue FastFrame::PopIV() noexcept {
    --sp;
    auto tag = static_cast<interpreter::ValueTag>(stack_tags[sp]);
    interpreter::InterpreterValue result;
    switch (tag) {
    case interpreter::ValueTag::Int32:
        result = interpreter::InterpreterValue::from_i32(static_cast<int32_t>(stack[sp]));
        break;
    case interpreter::ValueTag::Int64:
        result = interpreter::InterpreterValue::from_i64(static_cast<int64_t>(stack[sp]));
        break;
    case interpreter::ValueTag::Float32: {
        float v;
        std::memcpy(&v, &stack[sp], sizeof(float));
        result = interpreter::InterpreterValue::from_f32(v);
        break;
    }
    case interpreter::ValueTag::Float64: {
        double v;
        std::memcpy(&v, &stack[sp], sizeof(double));
        result = interpreter::InterpreterValue::from_f64(v);
        break;
    }
    case interpreter::ValueTag::Null:
        result = interpreter::InterpreterValue::null_val();
        break;
    default:
        result = interpreter::InterpreterValue::from_obj(reinterpret_cast<void*>(stack[sp]));
        break;
    }
    return result;
}

// ── Op Handlers ────────────────────────────────────────────────────────
// Each handler is a small function. Naming: Handle_<IROpCode>.

static void Handle_LdcI4(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    frame.PushI32(instr.immediate_i4);
    ++frame.pc;
}

static void Handle_LdcI8(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    frame.PushI64(instr.immediate_i8);
    ++frame.pc;
}

static void Handle_LdcR4(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    frame.PushF32(static_cast<float>(instr.immediate_r8));
    ++frame.pc;
}

static void Handle_LdcR8(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    frame.PushF64(instr.immediate_r8);
    ++frame.pc;
}

static void Handle_LdStr(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    frame.PushObj(const_cast<char*>(instr.string_operand));
    ++frame.pc;
}

static void Handle_LdNull(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    frame.PushNull();
    ++frame.pc;
}

static void Handle_LdArg(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    // Inlined LdArg: secondary_index > 0 indicates the callee's arg count.
    // The args are already on the FastFrame stack (pushed by the caller before the
    // now-inlined Call instruction).  Read from stack[sp - callee_argc + operand_idx].
    if (instr.secondary_index > 0) {
        uint32_t callee_argc = static_cast<uint32_t>(instr.secondary_index);
        uint32_t idx = static_cast<uint32_t>(instr.operand_index);
        if (idx >= callee_argc || frame.sp < callee_argc) {
            frame.threw_exception = true;
            frame.pc = 9999;
            return;
        }
        uint32_t stack_idx = frame.sp - callee_argc + idx;
        frame.stack[frame.sp] = frame.stack[stack_idx];
        frame.stack_tags[frame.sp] = frame.stack_tags[stack_idx];
        ++frame.sp;
        ++frame.pc;
        return;
    }

    if (instr.operand_index < 0 ||
        static_cast<uint32_t>(instr.operand_index) >= frame.arg_count) {
        frame.threw_exception = true;
        frame.pc = 9999; // signal error
        return;
    }
    // Args are stored as raw AOT bytes; reinterpret as uint64_t*.
    const auto* arg_base = static_cast<const uint64_t*>(frame.args);
    uint32_t idx = static_cast<uint32_t>(instr.operand_index);
    frame.stack[frame.sp] = arg_base[idx];
    frame.stack_tags[frame.sp] = static_cast<uint8_t>(interpreter::ValueTag::ObjectRef);
    ++frame.sp;
    ++frame.pc;
}

static void Handle_LdLoc(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    uint32_t idx = static_cast<uint32_t>(instr.operand_index);
    if (idx >= frame.local_count) {
        // Zero-initialized on first access.
        frame.locals[idx] = 0;
        frame.local_tags[idx] = static_cast<uint8_t>(interpreter::ValueTag::Int32);
        if (idx >= frame.local_count) frame.local_count = idx + 1;
    }
    frame.stack[frame.sp] = frame.locals[idx];
    frame.stack_tags[frame.sp] = frame.local_tags[idx];
    ++frame.sp;
    ++frame.pc;
}

static void Handle_StLoc(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    uint32_t idx = static_cast<uint32_t>(instr.operand_index);
    if (frame.sp == 0) { frame.threw_exception = true; frame.pc = 9999; return; }
    --frame.sp;
    frame.locals[idx] = frame.stack[frame.sp];
    frame.local_tags[idx] = frame.stack_tags[frame.sp];
    if (idx >= frame.local_count) frame.local_count = idx + 1;
    ++frame.pc;
}

static void Handle_Pop(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    if (frame.sp > 0) --frame.sp;
    ++frame.pc;
}

static void Handle_Dup(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    if (frame.sp == 0 || frame.sp >= FastFrame::kMaxStack) {
        frame.threw_exception = true; frame.pc = 9999; return;
    }
    frame.stack[frame.sp] = frame.stack[frame.sp - 1];
    frame.stack_tags[frame.sp] = frame.stack_tags[frame.sp - 1];
    ++frame.sp;
    ++frame.pc;
}

static void Handle_Br(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    frame.pc = static_cast<uint32_t>(instr.branch_target);
}

static void Handle_BrTrue(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    if (frame.sp == 0) { frame.threw_exception = true; frame.pc = 9999; return; }
    int32_t val = static_cast<int32_t>(frame.stack[frame.sp - 1]);
    --frame.sp;
    if (val != 0) {
        frame.pc = static_cast<uint32_t>(instr.branch_target);
    } else {
        ++frame.pc;
    }
}

static void Handle_BrFalse(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    if (frame.sp == 0) { frame.threw_exception = true; frame.pc = 9999; return; }
    int32_t val = static_cast<int32_t>(frame.stack[frame.sp - 1]);
    --frame.sp;
    if (val == 0) {
        frame.pc = static_cast<uint32_t>(instr.branch_target);
    } else {
        ++frame.pc;
    }
}

static void Handle_Beq(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    uint64_t r = frame.stack[frame.sp - 1];
    uint64_t l = frame.stack[frame.sp - 2];
    frame.sp -= 2;
    if (l == r) {
        frame.pc = static_cast<uint32_t>(instr.branch_target);
    } else {
        ++frame.pc;
    }
}

static void Handle_Blt(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    int32_t r = static_cast<int32_t>(frame.stack[frame.sp - 1]);
    int32_t l = static_cast<int32_t>(frame.stack[frame.sp - 2]);
    frame.sp -= 2;
    if (l < r) {
        frame.pc = static_cast<uint32_t>(instr.branch_target);
    } else {
        ++frame.pc;
    }
}

static void Handle_Bgt(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    int32_t r = static_cast<int32_t>(frame.stack[frame.sp - 1]);
    int32_t l = static_cast<int32_t>(frame.stack[frame.sp - 2]);
    frame.sp -= 2;
    if (l > r) {
        frame.pc = static_cast<uint32_t>(instr.branch_target);
    } else {
        ++frame.pc;
    }
}

static void Handle_Ble(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    int32_t r = static_cast<int32_t>(frame.stack[frame.sp - 1]);
    int32_t l = static_cast<int32_t>(frame.stack[frame.sp - 2]);
    frame.sp -= 2;
    if (l <= r) {
        frame.pc = static_cast<uint32_t>(instr.branch_target);
    } else {
        ++frame.pc;
    }
}

static void Handle_Bge(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    int32_t r = static_cast<int32_t>(frame.stack[frame.sp - 1]);
    int32_t l = static_cast<int32_t>(frame.stack[frame.sp - 2]);
    frame.sp -= 2;
    if (l >= r) {
        frame.pc = static_cast<uint32_t>(instr.branch_target);
    } else {
        ++frame.pc;
    }
}

static void Handle_BneUn(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    uint32_t r = static_cast<uint32_t>(frame.stack[frame.sp - 1]);
    uint32_t l = static_cast<uint32_t>(frame.stack[frame.sp - 2]);
    frame.sp -= 2;
    if (l != r) {
        frame.pc = static_cast<uint32_t>(instr.branch_target);
    } else {
        ++frame.pc;
    }
}

static void Handle_BgeUn(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    uint32_t r = static_cast<uint32_t>(frame.stack[frame.sp - 1]);
    uint32_t l = static_cast<uint32_t>(frame.stack[frame.sp - 2]);
    frame.sp -= 2;
    if (l >= r) {
        frame.pc = static_cast<uint32_t>(instr.branch_target);
    } else {
        ++frame.pc;
    }
}

static void Handle_BgtUn(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    uint32_t r = static_cast<uint32_t>(frame.stack[frame.sp - 1]);
    uint32_t l = static_cast<uint32_t>(frame.stack[frame.sp - 2]);
    frame.sp -= 2;
    if (l > r) {
        frame.pc = static_cast<uint32_t>(instr.branch_target);
    } else {
        ++frame.pc;
    }
}

static void Handle_BleUn(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    uint32_t r = static_cast<uint32_t>(frame.stack[frame.sp - 1]);
    uint32_t l = static_cast<uint32_t>(frame.stack[frame.sp - 2]);
    frame.sp -= 2;
    if (l <= r) {
        frame.pc = static_cast<uint32_t>(instr.branch_target);
    } else {
        ++frame.pc;
    }
}

static void Handle_BltUn(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    uint32_t r = static_cast<uint32_t>(frame.stack[frame.sp - 1]);
    uint32_t l = static_cast<uint32_t>(frame.stack[frame.sp - 2]);
    frame.sp -= 2;
    if (l < r) {
        frame.pc = static_cast<uint32_t>(instr.branch_target);
    } else {
        ++frame.pc;
    }
}

static void Handle_Add(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    int32_t r = static_cast<int32_t>(frame.stack[--frame.sp]);
    int32_t l = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI32(l + r);
    ++frame.pc;
}

static void Handle_Sub(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    int32_t r = static_cast<int32_t>(frame.stack[--frame.sp]);
    int32_t l = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI32(l - r);
    ++frame.pc;
}

static void Handle_Mul(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    int32_t r = static_cast<int32_t>(frame.stack[--frame.sp]);
    int32_t l = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI32(l * r);
    ++frame.pc;
}

static void Handle_Div(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    int32_t r = static_cast<int32_t>(frame.stack[--frame.sp]);
    int32_t l = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI32(l / r);
    ++frame.pc;
}

static void Handle_Rem(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    int32_t r = static_cast<int32_t>(frame.stack[--frame.sp]);
    int32_t l = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI32(l % r);
    ++frame.pc;
}

static void Handle_Neg(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    int32_t v = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI32(-v);
    ++frame.pc;
}

static void Handle_And(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    int32_t r = static_cast<int32_t>(frame.stack[--frame.sp]);
    int32_t l = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI32(l & r);
    ++frame.pc;
}

static void Handle_Or(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    int32_t r = static_cast<int32_t>(frame.stack[--frame.sp]);
    int32_t l = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI32(l | r);
    ++frame.pc;
}

static void Handle_Xor(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    int32_t r = static_cast<int32_t>(frame.stack[--frame.sp]);
    int32_t l = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI32(l ^ r);
    ++frame.pc;
}

static void Handle_Not(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    int32_t v = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI32(~v);
    ++frame.pc;
}

static void Handle_Shl(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    int32_t a = static_cast<int32_t>(frame.stack[--frame.sp]);
    int32_t v = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI32(v << (a & 0x1F));
    ++frame.pc;
}

static void Handle_Shr(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    int32_t a = static_cast<int32_t>(frame.stack[--frame.sp]);
    int32_t v = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI32(v >> (a & 0x1F));
    ++frame.pc;
}

static void Handle_ShrUn(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    int32_t a = static_cast<int32_t>(frame.stack[--frame.sp]);
    uint32_t v = static_cast<uint32_t>(frame.stack[--frame.sp]);
    frame.PushI32(static_cast<int32_t>(v >> (a & 0x1F)));
    ++frame.pc;
}

static void Handle_Ceq(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    uint64_t r = frame.stack[--frame.sp];
    uint64_t l = frame.stack[--frame.sp];
    frame.PushI32(l == r ? 1 : 0);
    ++frame.pc;
}

static void Handle_Clt(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    int32_t r = static_cast<int32_t>(frame.stack[--frame.sp]);
    int32_t l = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI32(l < r ? 1 : 0);
    ++frame.pc;
}

static void Handle_Cgt(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    int32_t r = static_cast<int32_t>(frame.stack[--frame.sp]);
    int32_t l = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI32(l > r ? 1 : 0);
    ++frame.pc;
}

static void Handle_Conv_I4(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    // Read as int32 (truncation semantics for Conv_I4).
    int32_t val = static_cast<int32_t>(frame.stack[frame.sp - 1]);
    frame.stack_tags[frame.sp - 1] = static_cast<uint8_t>(interpreter::ValueTag::Int32);
    frame.stack[frame.sp - 1] = static_cast<uint64_t>(val);
    ++frame.pc;
}

static void Handle_Conv_I8(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    int64_t val = static_cast<int64_t>(frame.stack[frame.sp - 1]);
    frame.stack_tags[frame.sp - 1] = static_cast<uint8_t>(interpreter::ValueTag::Int64);
    frame.stack[frame.sp - 1] = static_cast<uint64_t>(val);
    ++frame.pc;
}

static void Handle_Conv_R4(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    float val = static_cast<float>(static_cast<int32_t>(frame.stack[frame.sp - 1]));
    frame.stack_tags[frame.sp - 1] = static_cast<uint8_t>(interpreter::ValueTag::Float32);
    std::memcpy(&frame.stack[frame.sp - 1], &val, sizeof(float));
    ++frame.pc;
}

static void Handle_Conv_R8(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    double val = static_cast<double>(static_cast<int32_t>(frame.stack[frame.sp - 1]));
    frame.stack_tags[frame.sp - 1] = static_cast<uint8_t>(interpreter::ValueTag::Float64);
    std::memcpy(&frame.stack[frame.sp - 1], &val, sizeof(double));
    ++frame.pc;
}

static void Handle_Box(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    // Box: wrap the top-of-stack value into a heap-allocated InterpreterObject.
    auto* boxed = static_cast<interpreter::InterpreterObject*>(
        CHAOS_IL2CPP_MALLOC(sizeof(interpreter::InterpreterObject)));
    if (boxed == nullptr) { frame.threw_exception = true; frame.pc = 9999; return; }
    ::new (boxed) interpreter::InterpreterObject();
    frame.Track(boxed, frame.Dtor<interpreter::InterpreterObject>);
    boxed->fields.resize(1);
    boxed->fields[0] = frame.PopIV();
    frame.PushObj(boxed);
    ++frame.pc;
}

static void Handle_Unbox(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    void* obj = frame.PopObj();
    if (obj == nullptr) { frame.PushNull(); ++frame.pc; return; }
    auto* boxed = static_cast<interpreter::InterpreterObject*>(obj);
    if (!boxed->fields.empty()) {
        frame.PushIV(boxed->fields[0]);
    } else {
        frame.PushNull();
    }
    ++frame.pc;
}

static void Handle_LdSFld(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    // Read from global static fields via InterpreterVM's g_static_fields.
    // We temporarily convert to InterpreterValue, read, then push onto FastFrame.
    // TODO: direct static field access without InterpreterValue round-trip.
    interpreter::ExecutionResult dummy;
    (void)dummy;
    // For now, fall back to InterpreterVM for LdSFld/StSFld.
    frame.threw_exception = true; // signal: try fallback
    frame.pc = 9999;
}

static void Handle_StSFld(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    // Fallback for StSFld too.
    frame.threw_exception = true;
    frame.pc = 9999;
}

static void Handle_NewObj(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    auto* storage = static_cast<interpreter::InterpreterObject*>(
        CHAOS_IL2CPP_MALLOC(sizeof(interpreter::InterpreterObject)));
    if (storage == nullptr) { frame.threw_exception = true; frame.pc = 9999; return; }
    ::new (storage) interpreter::InterpreterObject();
    frame.Track(storage, frame.Dtor<interpreter::InterpreterObject>);
    uint32_t field_count = static_cast<uint32_t>(
        instr.secondary_index == 0u ? 1u : instr.secondary_index);
    storage->fields.resize(field_count);
    storage->type_token = static_cast<uint32_t>(instr.immediate_i4);
    frame.PushObj(storage);
    ++frame.pc;
}

static void Handle_NewArr(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    uint32_t len = static_cast<uint32_t>(frame.stack[--frame.sp]);
    auto* arr = static_cast<interpreter::ArrayStorage*>(
        CHAOS_IL2CPP_MALLOC(sizeof(interpreter::ArrayStorage)));
    if (arr == nullptr) { frame.threw_exception = true; frame.pc = 9999; return; }
    ::new (arr) interpreter::ArrayStorage();
    frame.Track(arr, frame.Dtor<interpreter::ArrayStorage>);
    arr->elements.resize(len);
    frame.PushObj(arr);
    ++frame.pc;
}

static void Handle_LdLen(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    auto* arr = static_cast<interpreter::ArrayStorage*>(frame.PopObj());
    if (arr == nullptr) { frame.PushI32(0); ++frame.pc; return; }
    frame.PushI32(static_cast<int32_t>(arr->elements.size()));
    ++frame.pc;
}

static void Handle_LdFld(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    void* obj = frame.PopObj();
    if (obj == nullptr) { frame.PushNull(); ++frame.pc; return; }
    auto* storage = static_cast<interpreter::InterpreterObject*>(obj);
    uint32_t idx = static_cast<uint32_t>(instr.field_offset);
    if (idx >= storage->fields.size()) {
        storage->fields.resize(idx + 1);
    }
    frame.PushIV(storage->fields[idx]);
    ++frame.pc;
}

static void Handle_StFld(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    auto val = frame.PopIV();
    void* obj = frame.PopObj();
    if (obj == nullptr) { ++frame.pc; return; }
    auto* storage = static_cast<interpreter::InterpreterObject*>(obj);
    uint32_t idx = static_cast<uint32_t>(instr.field_offset);
    if (idx >= storage->fields.size()) {
        storage->fields.resize(idx + 1);
    }
    storage->fields[idx] = val;
    ++frame.pc;
}

static void Handle_StArg(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    // FastFrame doesn't support mutable args — fallback.
    frame.threw_exception = true;
    frame.pc = 9999;
}

// ── Call handling ──────────────────────────────────────────────────────
// Uses InterpreterDispatchRaw to skip InterpreterValue round-trip,
// building ABI argv[] directly from FastFrame's raw uint64_t stack values.
// This eliminates ~600ns of InterpreterDispatch argument marshaling overhead
// for each Call instruction (Layer 4 optimization).

static void Handle_Call(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    if (frame.sp < instr.arg_count) {
        frame.threw_exception = true; frame.pc = 9999; return;
    }
    if (instr.call_target == nullptr && instr.direct_fn == nullptr) { ++frame.pc; return; }

    uint32_t ac = static_cast<uint32_t>(instr.arg_count);

    // ── AotDirectDispatch fast path: call chaos_external_runtime_* directly ──
    // When direct_fn is pre-resolved (during IR lowering via kAotDirectFnTable),
    // we call the AOT function pointer directly, skipping ~2200ns method_invoke.
    // The chaos_external_runtime_* functions accept CHAOS_IL2CPP_INTPTR args.
    if (instr.direct_fn != nullptr) {
        using DirectFn1 = CHAOS_IL2CPP_UINT16 (*)(CHAOS_IL2CPP_INTPTR);
        using DirectFn2 = CHAOS_IL2CPP_UINT16 (*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR);

        // Pop args in reverse order from FastFrame stack.
        CHAOS_IL2CPP_INTPTR raw_args[2] = {0, 0};
        for (uint32_t i = ac; i > 0; --i) {
            --frame.sp;
            if (i - 1 < 2) {
                raw_args[i - 1] = static_cast<CHAOS_IL2CPP_INTPTR>(frame.stack[frame.sp]);
            }
        }

        CHAOS_IL2CPP_UINT16 result;
        if (ac <= 1) {
            result = reinterpret_cast<DirectFn1>(instr.direct_fn)(raw_args[0]);
        } else {
            result = reinterpret_cast<DirectFn2>(instr.direct_fn)(raw_args[0], raw_args[1]);
        }

        // Push result as Int32 (UINT16 fits in Int32).
        frame.stack[frame.sp] = static_cast<uint64_t>(result);
        frame.stack_tags[frame.sp] = static_cast<uint8_t>(interpreter::ValueTag::Int32);
        ++frame.sp;
        ++frame.pc;
        return;
    }

    if (frame.dispatch_fn == nullptr) {
        frame.threw_exception = true; frame.pc = 9999; return;
    }

    // Stack-allocated arrays for arg_count ≤ 8 (common case).
    // Avoids malloc/free overhead on every Call instruction.
    uint64_t raw_args_stack[8];
    uint8_t  raw_tags_stack[8];
    auto* raw_args = (ac <= 8) ? raw_args_stack
        : static_cast<uint64_t*>(CHAOS_IL2CPP_MALLOC(sizeof(uint64_t) * ac));
    auto* raw_tags = (ac <= 8) ? raw_tags_stack
        : static_cast<uint8_t*>(CHAOS_IL2CPP_MALLOC(sizeof(uint8_t) * ac));

    if (raw_args == nullptr || raw_tags == nullptr) {
        if (ac > 8) { CHAOS_IL2CPP_FREE(raw_args); CHAOS_IL2CPP_FREE(raw_tags); }
        frame.threw_exception = true; frame.pc = 9999; return;
    }

    // Pop args in reverse order from the FastFrame stack.
    for (uint32_t i = ac; i > 0; --i) {
        --frame.sp;
        raw_args[i - 1] = frame.stack[frame.sp];
        raw_tags[i - 1] = frame.stack_tags[frame.sp];
    }

    // Look up call-site metadata cache (pre-populated during IR lowering).
    const ri::CachedCallInfo* cache_info = nullptr;
    if (frame.call_cache != nullptr && frame.pc < frame.call_count) {
        const auto* cc = static_cast<const ri::CachedCallInfo*>(frame.call_cache);
        if (cc[frame.pc].ret_tag != 0xFF) {
            cache_info = &cc[frame.pc];
        }
    }

    // Dispatch directly via InterpreterDispatchRaw — no InterpreterValue[].
    auto dret = ri::InterpreterDispatchRaw(
        instr.call_target, raw_args, raw_tags, ac,
        instr.is_instance_call,
        frame.dispatch_ctx,
        cache_info);

    if (ac > 8) { CHAOS_IL2CPP_FREE(raw_args); CHAOS_IL2CPP_FREE(raw_tags); }

    // Handle dispatch result.
    if (dret.threw_exception) {
        frame.threw_exception = true;
        frame.exception_obj_val = dret.exception_obj;
        frame.pc = 9999;
        return;
    }

    if (dret.has_value) {
        if (dret.tag == static_cast<uint8_t>(interpreter::ValueTag::Struct) &&
            dret.struct_data != nullptr) {
            // Struct return: struct_data is raw-domain (std::malloc'd by interpreter dispatch).
            // Push the pointer as the value; Track() frees it via CleanupTracked().
            frame.stack[frame.sp] = reinterpret_cast<uint64_t>(dret.struct_data);
            frame.Track(dret.struct_data, [](void* p) noexcept { std::free(p); });
        } else {
            frame.stack[frame.sp] = dret.value;
        }
        frame.stack_tags[frame.sp] = dret.tag;
        ++frame.sp;
    }
    ++frame.pc;
}

static void Handle_Ret(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    if (frame.sp > 0) {
        frame.has_ret = true;
        frame.ret_val = frame.stack[frame.sp - 1];
        frame.ret_tag = frame.stack_tags[frame.sp - 1];
    }
    frame.pc = 0xFFffFFffu; // sentinel: stop
}

// ── Unsupported / fallback handlers ─────────────────────────────────────

static void Handle_Unsupported(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    frame.threw_exception = true;
    frame.pc = 9999; // signal fallback needed
}

// Map: handled by Handle_Unsupported for all opcodes not explicitly defined.
// Below we alias the common unsupported ones to the same function.

// SEH opcodes
static void Handle_Throw(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept { Handle_Unsupported(frame, instr); }
static void Handle_Leave(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept { Handle_Unsupported(frame, instr); }
static void Handle_EndFinally(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept { Handle_Unsupported(frame, instr); }
static void Handle_Rethrow(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept { Handle_Unsupported(frame, instr); }

// ── Dispatch table ─────────────────────────────────────────────────────

#define HANDLER_ENTRY(name) Handle_##name
#define UNSUPPORTED Handle_Unsupported

const OpHandler kHandlers[99] = {
    /*  0 */ HANDLER_ENTRY(LdcI4),
    /*  1 */ HANDLER_ENTRY(LdcI8),
    /*  2 */ HANDLER_ENTRY(LdcR4),
    /*  3 */ HANDLER_ENTRY(LdcR8),
    /*  4 */ HANDLER_ENTRY(LdStr),
    /*  5 */ HANDLER_ENTRY(LdNull),
    /*  6 */ HANDLER_ENTRY(LdArg),
    /*  7 */ HANDLER_ENTRY(LdLoc),
    /*  8 */ HANDLER_ENTRY(StLoc),
    /*  9 */ HANDLER_ENTRY(StArg),
    /* 10 */ HANDLER_ENTRY(LdFld),
    /* 11 */ HANDLER_ENTRY(StFld),
    /* 12 */ HANDLER_ENTRY(LdSFld),
    /* 13 */ HANDLER_ENTRY(StSFld),
    /* 14 */ HANDLER_ENTRY(Call),
    /* 15 */ UNSUPPORTED,  // CallVirt
    /* 16 */ UNSUPPORTED,  // CallBridge
    /* 17 */ HANDLER_ENTRY(Br),
    /* 18 */ HANDLER_ENTRY(BrTrue),
    /* 19 */ HANDLER_ENTRY(BrFalse),
    /* 20 */ HANDLER_ENTRY(Beq),
    /* 21 */ HANDLER_ENTRY(Blt),
    /* 22 */ HANDLER_ENTRY(Bgt),
    /* 23 */ HANDLER_ENTRY(Ble),
    /* 24 */ HANDLER_ENTRY(Bge),
    /* 25 */ HANDLER_ENTRY(Add),
    /* 26 */ HANDLER_ENTRY(Sub),
    /* 27 */ HANDLER_ENTRY(Mul),
    /* 28 */ HANDLER_ENTRY(Div),
    /* 29 */ HANDLER_ENTRY(Rem),
    /* 30 */ HANDLER_ENTRY(Neg),
    /* 31 */ HANDLER_ENTRY(Ceq),
    /* 32 */ HANDLER_ENTRY(Clt),
    /* 33 */ HANDLER_ENTRY(Cgt),
    /* 34 */ HANDLER_ENTRY(NewObj),
    /* 35 */ HANDLER_ENTRY(Box),
    /* 36 */ HANDLER_ENTRY(Unbox),
    /* 37 */ UNSUPPORTED,  // CastClass
    /* 38 */ UNSUPPORTED,  // IsInst
    /* 39 */ HANDLER_ENTRY(Conv_I4),
    /* 40 */ HANDLER_ENTRY(Conv_I8),
    /* 41 */ HANDLER_ENTRY(Conv_R4),
    /* 42 */ HANDLER_ENTRY(Conv_R8),
    /* 43 */ HANDLER_ENTRY(NewArr),
    /* 44 */ UNSUPPORTED,  // LdElem
    /* 45 */ UNSUPPORTED,  // StElem
    /* 46 */ HANDLER_ENTRY(LdLen),
    /* 47 */ HANDLER_ENTRY(Pop),
    /* 48 */ HANDLER_ENTRY(Throw),
    /* 49 */ HANDLER_ENTRY(Rethrow),
    /* 50 */ HANDLER_ENTRY(Leave),
    /* 51 */ HANDLER_ENTRY(EndFinally),
    /* 52 */ UNSUPPORTED,  // EndFilter
    /* 53 */ HANDLER_ENTRY(Ret),
    /* 54 */ HANDLER_ENTRY(Dup),
    /* 55 */ UNSUPPORTED,  // DivUn
    /* 56 */ UNSUPPORTED,  // RemUn
    /* 57 */ HANDLER_ENTRY(And),
    /* 58 */ HANDLER_ENTRY(Or),
    /* 59 */ HANDLER_ENTRY(Xor),
    /* 60 */ HANDLER_ENTRY(Not),
    /* 61 */ HANDLER_ENTRY(Shl),
    /* 62 */ HANDLER_ENTRY(Shr),
    /* 63 */ HANDLER_ENTRY(ShrUn),
    /* 64 */ UNSUPPORTED,  // ConvRUn
    /* 65 */ UNSUPPORTED,  // ConvI
    /* 66 */ UNSUPPORTED,  // ConvU
    /* 67 */ UNSUPPORTED,  // LdInd
    /* 68 */ UNSUPPORTED,  // StInd
    /* 69 */ UNSUPPORTED,  // Switch
    /* 70 */ UNSUPPORTED,  // LdToken
    /* 71 */ UNSUPPORTED,  // InitObj
    /* 72 */ UNSUPPORTED,  // SizeOf
    /* 73 */ UNSUPPORTED,  // LdFtn
    /* 74 */ UNSUPPORTED,  // LdVirtFtn
    /* 75 */ UNSUPPORTED,  // LdArgA
    /* 76 */ UNSUPPORTED,  // LdLocA
    /* 77 */ UNSUPPORTED,  // LocAlloc
    /* 78 */ UNSUPPORTED,  // Break
    /* 79 */ HANDLER_ENTRY(BneUn),
    /* 80 */ HANDLER_ENTRY(BgeUn),
    /* 81 */ HANDLER_ENTRY(BgtUn),
    /* 82 */ HANDLER_ENTRY(BleUn),
    /* 83 */ HANDLER_ENTRY(BltUn),
    /* 84 */ UNSUPPORTED,  // AddOvf
    /* 85 */ UNSUPPORTED,  // SubOvf
    /* 86 */ UNSUPPORTED,  // MulOvf
    /* 87 */ UNSUPPORTED,  // ConvOvfI
    /* 88 */ UNSUPPORTED,  // ConvOvfI4
    /* 89 */ UNSUPPORTED,  // ConvOvfI8
    /* 90 */ UNSUPPORTED,  // ConvOvfU
    /* 91 */ UNSUPPORTED,  // ConvOvfU4
    /* 92 */ UNSUPPORTED,  // ConvOvfU8
    /* 93 */ UNSUPPORTED,  // LdObj
    /* 94 */ UNSUPPORTED,  // StObj
    /* 95 */ UNSUPPORTED,  // LdElemA
    /* 96 */ UNSUPPORTED,  // Cpblk
    /* 97 */ UNSUPPORTED,  // InitBlk
    /* 98 */ UNSUPPORTED,  // CallVirtConstrained
};

#undef HANDLER_ENTRY
#undef UNSUPPORTED

// ── FastExecute ────────────────────────────────────────────────────────

bool FastExecute(FastFrame& frame,
                 const interpreter::IRInstruction* instrs,
                 uint32_t instr_count) noexcept {
    frame.pc = 0;

    while (frame.pc < instr_count) {
        uint32_t op_val = static_cast<uint32_t>(instrs[frame.pc].op_code);
        if (op_val >= 99) {
            frame.threw_exception = true;
            return false;
        }

        kHandlers[op_val](frame, instrs[frame.pc]);

        // Check for fallback signal — free tracked objects before returning.
        if (frame.threw_exception && frame.pc == 9999) {
            frame.CleanupTracked();
            return false;
        }

        // Check for normal termination (Ret handler sets pc to sentinel).
        if (frame.pc == 0xFFffFFffu) {
            frame.CleanupTracked();
            return true;
        }
    }

    frame.CleanupTracked();
    return true;
}

// ── Helper: convert FastFrame result back to ExecutionResult ────────────

}  // namespace chaos::il2cpp::runtime_core