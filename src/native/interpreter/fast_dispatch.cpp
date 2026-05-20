#include "fast_dispatch.h"

#include "instantiation_engine.h"
#include "vtable_registry.h"
namespace ri = chaos::il2cpp::runtime_instantiation;
namespace vr = chaos::il2cpp::vtable_registry;

#include <climits>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#define CHAOS_IL2CPP_LOG_LEVEL 1  // WARN+ERROR visible; DEBUG/INFO compiled out for hot-path perf
#include <chaos/log.h>
#include <chaos/profile.h>
#include <gc/gc_bgc_inline.h>
#include <gc/gc_helpers.h>

// Global static field storage from the full InterpreterVM.
// FastFrame reads/writes this directly so StSFld/LdSFld don't trigger fallback.
namespace chaos::il2cpp::interpreter {
struct InterpreterValue;
extern CHAOS_IL2CPP_VECTOR(InterpreterValue) g_static_fields;
}

namespace chaos::il2cpp::runtime_core {

// ── TLS box object pool ──────────────────────────────────────────────
// Avoids per-call malloc/free for Box/NewObj by reusing InterpreterObjects.
// Each object's vector is pre-sized with capacity 1 so fields.resize(1)
// in Handle_Box avoids a second heap allocation.
static constexpr uint32_t kBoxPoolSize = 8;
static thread_local interpreter::InterpreterObject* tls_box_pool[kBoxPoolSize] = {};
static thread_local uint32_t tls_box_pool_count = 0;

/// Return a boxed InterpreterObject to the TLS pool (or free if pool full).
static void ReturnBoxToPool(void* p) noexcept {
    auto* obj = static_cast<interpreter::InterpreterObject*>(p);
    if (tls_box_pool_count < kBoxPoolSize) {
        // Keep the inline field storage alive — just clear elements and type_token.
        // Avoids ~InterpreterObject + placement new + malloc/free cycle.
        obj->fields.clear();
        obj->type_token = 0;
        tls_box_pool[tls_box_pool_count++] = obj;
    } else {
        obj->~InterpreterObject();
        CHAOS_IL2CPP_FREE(obj);
    }
}

/// Acquire an InterpreterObject for boxing, from pool or malloc.
static interpreter::InterpreterObject* AcquireBoxedObject() noexcept {
    if (tls_box_pool_count > 0) {
        --tls_box_pool_count;
        auto* obj = tls_box_pool[tls_box_pool_count];
        obj->fields.clear();
        obj->type_token = 0;
        return obj;
    }
    auto* obj = static_cast<interpreter::InterpreterObject*>(
        CHAOS_IL2CPP_MALLOC(sizeof(interpreter::InterpreterObject)));
    if (obj == nullptr) return nullptr;
    ::new (obj) interpreter::InterpreterObject();
    return obj;
}

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
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_LdcI4");
    frame.PushI32(instr.immediate_i4);
    ++frame.pc;
}

static void Handle_LdcI8(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_LdcI8");
    frame.PushI64(instr.immediate_i8);
    ++frame.pc;
}

static void Handle_LdcR4(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_LdcR4");
    frame.PushF32(static_cast<float>(instr.immediate_r8));
    ++frame.pc;
}

static void Handle_LdcR8(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_LdcR8");
    frame.PushF64(instr.immediate_r8);
    ++frame.pc;
}

static void Handle_LdStr(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_LdStr");
    frame.PushObj(const_cast<char*>(instr.string_operand));
    ++frame.pc;
}

static void Handle_LdNull(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_LdNull");
    frame.PushNull();
    ++frame.pc;
}

static void Handle_LdArg(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_LdArg");
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
        // Out of bounds — push zero / null as a safe default instead of crashing.
        frame.stack[frame.sp] = 0;
        frame.stack_tags[frame.sp] = static_cast<uint8_t>(interpreter::ValueTag::Int32);
        ++frame.sp;
        ++frame.pc;
        return;
    }
    // Args are stored as raw AOT bytes; reinterpret as uint64_t*.
    if (frame.args == nullptr) {
        frame.stack[frame.sp] = 0;
        frame.stack_tags[frame.sp] = static_cast<uint8_t>(interpreter::ValueTag::Int32);
        ++frame.sp;
        ++frame.pc;
        return;
    }
    const auto* arg_base = static_cast<const uint64_t*>(frame.args);
    uint32_t idx = static_cast<uint32_t>(instr.operand_index);
    frame.stack[frame.sp] = arg_base[idx];
    // Use cached arg type tags when available (set by SetupFastFrame from PatchMethod::cached_arg_types).
    // Falls back to ObjectRef when no type info is available (legacy behavior).
    frame.stack_tags[frame.sp] = (frame.arg_type_tags != nullptr && idx < frame.arg_count)
        ? frame.arg_type_tags[idx]
        : static_cast<uint8_t>(interpreter::ValueTag::ObjectRef);
    ++frame.sp;
    ++frame.pc;
}

static void Handle_LdLoc(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_LdLoc");
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
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_StLoc");
    uint32_t idx = static_cast<uint32_t>(instr.operand_index);
    if (frame.sp == 0) { frame.threw_exception = true; frame.pc = 9999; return; }
    --frame.sp;
    frame.locals[idx] = frame.stack[frame.sp];
    frame.local_tags[idx] = frame.stack_tags[frame.sp];
    if (idx >= frame.local_count) frame.local_count = idx + 1;
    ++frame.pc;
}

static void Handle_Pop(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Pop");
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
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Br");
    frame.pc = static_cast<uint32_t>(instr.branch_target);
}

static void Handle_BrTrue(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_BrTrue");
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
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_BrFalse");
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
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Beq");
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
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Blt");
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
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Bgt");
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
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Ble");
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
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Bge");
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
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_BneUn");
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
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_BgeUn");
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
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_BgtUn");
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
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_BleUn");
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
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_BltUn");
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

// ── Tag validation helper (CHECK builds only — verifies stack tags match
// expected integer type.  Catches IR-to-FastExecute tag mismatches early
// with a hard assertion instead of silent data corruption).
// No-op in SHIP/PROFILE builds (CHAOS_IL2CPP_ASSERT compiled out).
static void AssertInt32Tag(const FastFrame& frame, uint32_t stack_idx) noexcept {
    CHAOS_IL2CPP_ASSERT(stack_idx < FastFrame::kMaxStack);
    CHAOS_IL2CPP_ASSERT(
        frame.stack_tags[stack_idx] == static_cast<uint8_t>(interpreter::ValueTag::Int32) ||
        frame.stack_tags[stack_idx] == static_cast<uint8_t>(interpreter::ValueTag::Int64) ||
        frame.stack_tags[stack_idx] == static_cast<uint8_t>(interpreter::ValueTag::ObjectRef) ||
        frame.stack_tags[stack_idx] == static_cast<uint8_t>(interpreter::ValueTag::Null)
        && "Handle_Add/Sub/Mul/etc: expected Int32/Int64 tag, got Float32/Float64");
}

static void Handle_Add(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Add");
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    AssertInt32Tag(frame, frame.sp - 1);
    AssertInt32Tag(frame, frame.sp - 2);
    int32_t r = static_cast<int32_t>(frame.stack[--frame.sp]);
    int32_t l = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI32(l + r);
    ++frame.pc;
}

static void Handle_Sub(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Sub");
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    AssertInt32Tag(frame, frame.sp - 1);
    AssertInt32Tag(frame, frame.sp - 2);
    int32_t r = static_cast<int32_t>(frame.stack[--frame.sp]);
    int32_t l = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI32(l - r);
    ++frame.pc;
}

static void Handle_Mul(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Mul");
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    AssertInt32Tag(frame, frame.sp - 1);
    AssertInt32Tag(frame, frame.sp - 2);
    int32_t r = static_cast<int32_t>(frame.stack[--frame.sp]);
    int32_t l = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI32(l * r);
    ++frame.pc;
}

static void Handle_Div(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Div");
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    AssertInt32Tag(frame, frame.sp - 1);
    AssertInt32Tag(frame, frame.sp - 2);
    int32_t r = static_cast<int32_t>(frame.stack[--frame.sp]);
    int32_t l = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI32(l / r);
    ++frame.pc;
}

static void Handle_Rem(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Rem");
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    AssertInt32Tag(frame, frame.sp - 1);
    AssertInt32Tag(frame, frame.sp - 2);
    int32_t r = static_cast<int32_t>(frame.stack[--frame.sp]);
    int32_t l = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI32(l % r);
    ++frame.pc;
}

static void Handle_Neg(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Neg");
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
        AssertInt32Tag(frame, frame.sp - 1);
    int32_t v = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI32(-v);
    ++frame.pc;
}

static void Handle_And(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_And");
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    AssertInt32Tag(frame, frame.sp - 1);
    AssertInt32Tag(frame, frame.sp - 2);
    int32_t r = static_cast<int32_t>(frame.stack[--frame.sp]);
    int32_t l = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI32(l & r);
    ++frame.pc;
}

static void Handle_Or(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Or");
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    AssertInt32Tag(frame, frame.sp - 1);
    AssertInt32Tag(frame, frame.sp - 2);
    int32_t r = static_cast<int32_t>(frame.stack[--frame.sp]);
    int32_t l = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI32(l | r);
    ++frame.pc;
}

static void Handle_Xor(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Xor");
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    AssertInt32Tag(frame, frame.sp - 1);
    AssertInt32Tag(frame, frame.sp - 2);
    int32_t r = static_cast<int32_t>(frame.stack[--frame.sp]);
    int32_t l = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI32(l ^ r);
    ++frame.pc;
}

static void Handle_Not(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Not");
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
        AssertInt32Tag(frame, frame.sp - 1);
    int32_t v = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI32(~v);
    ++frame.pc;
}

static void Handle_Shl(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Shl");
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
        AssertInt32Tag(frame, frame.sp - 1);
    AssertInt32Tag(frame, frame.sp - 2);
    int32_t a = static_cast<int32_t>(frame.stack[--frame.sp]);
    int32_t v = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI32(v << (a & 0x1F));
    ++frame.pc;
}

static void Handle_Shr(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Shr");
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
        AssertInt32Tag(frame, frame.sp - 1);
    AssertInt32Tag(frame, frame.sp - 2);
    int32_t a = static_cast<int32_t>(frame.stack[--frame.sp]);
    int32_t v = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI32(v >> (a & 0x1F));
    ++frame.pc;
}

static void Handle_ShrUn(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_ShrUn");
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
        AssertInt32Tag(frame, frame.sp - 1);
    AssertInt32Tag(frame, frame.sp - 2);
    int32_t a = static_cast<int32_t>(frame.stack[--frame.sp]);
    uint32_t v = static_cast<uint32_t>(frame.stack[--frame.sp]);
    frame.PushI32(static_cast<int32_t>(v >> (a & 0x1F)));
    ++frame.pc;
}

static void Handle_Ceq(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Ceq");
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
        AssertInt32Tag(frame, frame.sp - 1);
    AssertInt32Tag(frame, frame.sp - 2);
    uint64_t r = frame.stack[--frame.sp];
    uint64_t l = frame.stack[--frame.sp];
    frame.PushI32(l == r ? 1 : 0);
    ++frame.pc;
}

static void Handle_Clt(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Clt");
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    AssertInt32Tag(frame, frame.sp - 1);
    AssertInt32Tag(frame, frame.sp - 2);
    int32_t r = static_cast<int32_t>(frame.stack[--frame.sp]);
    int32_t l = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI32(l < r ? 1 : 0);
    ++frame.pc;
}

static void Handle_Cgt(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Cgt");
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    AssertInt32Tag(frame, frame.sp - 1);
    AssertInt32Tag(frame, frame.sp - 2);
    int32_t r = static_cast<int32_t>(frame.stack[--frame.sp]);
    int32_t l = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI32(l > r ? 1 : 0);
    ++frame.pc;
}

static void Handle_Conv_I4(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Conv_I4");
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    // Read as int32 (truncation semantics for Conv_I4).
    int32_t val = static_cast<int32_t>(frame.stack[frame.sp - 1]);
    frame.stack_tags[frame.sp - 1] = static_cast<uint8_t>(interpreter::ValueTag::Int32);
    frame.stack[frame.sp - 1] = static_cast<uint64_t>(val);
    ++frame.pc;
}

static void Handle_Conv_I8(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Conv_I8");
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    int64_t val = static_cast<int64_t>(frame.stack[frame.sp - 1]);
    frame.stack_tags[frame.sp - 1] = static_cast<uint8_t>(interpreter::ValueTag::Int64);
    frame.stack[frame.sp - 1] = static_cast<uint64_t>(val);
    ++frame.pc;
}

static void Handle_Conv_R4(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Conv_R4");
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    float val = static_cast<float>(static_cast<int32_t>(frame.stack[frame.sp - 1]));
    frame.stack_tags[frame.sp - 1] = static_cast<uint8_t>(interpreter::ValueTag::Float32);
    std::memcpy(&frame.stack[frame.sp - 1], &val, sizeof(float));
    ++frame.pc;
}

static void Handle_Conv_R8(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Conv_R8");
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    double val = static_cast<double>(static_cast<int32_t>(frame.stack[frame.sp - 1]));
    frame.stack_tags[frame.sp - 1] = static_cast<uint8_t>(interpreter::ValueTag::Float64);
    std::memcpy(&frame.stack[frame.sp - 1], &val, sizeof(double));
    ++frame.pc;
}

static void Handle_Box(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Box");
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    // Box: wrap the top-of-stack value into a heap-allocated InterpreterObject.
    // Uses TLS pool to avoid malloc per call; vector is pre-sized with capacity 1.
    auto* boxed = AcquireBoxedObject();
    if (boxed == nullptr) { frame.threw_exception = true; frame.pc = 9999; return; }
    frame.TrackPool(boxed, ReturnBoxToPool);
    boxed->fields.resize(1);
    boxed->fields[0] = frame.PopIV();
    frame.PushObj(boxed);
    ++frame.pc;
}

static void Handle_Unbox(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Unbox");
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
    // Read from the global static field vector directly.
    auto& sfields = interpreter::g_static_fields;
    if (sfields.size() <= instr.field_offset) {
        sfields.resize(instr.field_offset + 1u);
    }
    frame.PushIV(sfields[instr.field_offset]);
    ++frame.pc;
}

static void Handle_StSFld(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    // Pop value and write to the global static field vector.
    auto& sfields = interpreter::g_static_fields;
    if (sfields.size() <= instr.field_offset) {
        sfields.resize(instr.field_offset + 1u);
    }
    using chaos::il2cpp::runtime_core::BgcSatbPreWriteBarrier;
    BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&sfields[instr.field_offset].obj));
    sfields[instr.field_offset] = frame.PopIV();
    ++frame.pc;
}

static void Handle_NewObj(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_NewObj");
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
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_NewArr");
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
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_LdLen");
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    auto* arr = static_cast<interpreter::ArrayStorage*>(frame.PopObj());
    if (arr == nullptr) { frame.PushI32(0); ++frame.pc; return; }
    frame.PushI32(static_cast<int32_t>(arr->elements.size()));
    ++frame.pc;
}

static void Handle_LdFld(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_LdFld");
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    void* obj = frame.PopObj();
    if (obj == nullptr) { frame.PushNull(); ++frame.pc; return; }
    auto* storage = static_cast<interpreter::InterpreterObject*>(obj);
    uint32_t idx = static_cast<uint32_t>(instr.field_offset);
    // Fast path: well-formed code always has idx < fields.size().
    // The resize safety net catches rare edge cases.
    if (idx < storage->fields.size()) {
        frame.PushIV(storage->fields[idx]);
    } else {
        storage->fields.resize(idx + 1);
        frame.PushIV(storage->fields[idx]);
    }
    ++frame.pc;
}

static void Handle_StFld(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_StFld");
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    auto val = frame.PopIV();
    void* obj = frame.PopObj();
    if (obj == nullptr) { ++frame.pc; return; }
    auto* storage = static_cast<interpreter::InterpreterObject*>(obj);
    uint32_t idx = static_cast<uint32_t>(instr.field_offset);
    // SATB pre-write barrier: record old obj pointer before overwriting.
    using chaos::il2cpp::runtime_core::BgcSatbPreWriteBarrier;
    BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&storage->fields[idx].obj));
    // Fast path: skip bounds check when field is within current allocation.
    if (idx < storage->fields.size()) {
        storage->fields[idx] = val;
    } else {
        storage->fields.resize(idx + 1);
        storage->fields[idx] = val;
    }
    chaos_gc_dirty_card(obj);
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

// ── Path-specific Call handlers ───────────────────────────────────────
// Split from Handle_Call to improve icache locality: each path-specific
// function is small enough to fit in a single icache line, while the
// dispatcher handles arg popping and routing once.

static void Handle_Call_DoAotDirect(FastFrame& frame,
    const interpreter::IRInstruction& instr,
    uint64_t* raw_args, uint8_t* raw_tags, uint32_t ac) noexcept
{
    // Call via uniform 8-arg signature.
    using DirectFn = uint64_t (*)(uint64_t, uint64_t, uint64_t, uint64_t,
                                  uint64_t, uint64_t, uint64_t, uint64_t);
    auto fn = reinterpret_cast<DirectFn>(instr.direct_fn);
    uint64_t a0 = (ac > 0) ? raw_args[0] : 0;
    uint64_t a1 = (ac > 1) ? raw_args[1] : 0;
    uint64_t a2 = (ac > 2) ? raw_args[2] : 0;
    uint64_t a3 = (ac > 3) ? raw_args[3] : 0;
    uint64_t a4 = (ac > 4) ? raw_args[4] : 0;
    uint64_t a5 = (ac > 5) ? raw_args[5] : 0;
    uint64_t a6 = (ac > 6) ? raw_args[6] : 0;
    uint64_t a7 = (ac > 7) ? raw_args[7] : 0;
    uint64_t result = fn(a0, a1, a2, a3, a4, a5, a6, a7);

    // Read ret_tag from CachedCallInfo.
    // Default Int32 is safe: codegen emits correct Ret-type opcodes so the
    // tag is only used for FastFrame type tracking, not data interpretation.
    uint8_t ret_tag = static_cast<uint8_t>(interpreter::ValueTag::Int32);
    if (frame.call_cache != nullptr && frame.pc < frame.call_count) {
        const auto* cc = static_cast<const ri::CachedCallInfo*>(frame.call_cache);
        if (cc[frame.pc].ret_tag != 0xFF) {
            ret_tag = cc[frame.pc].ret_tag;
        }
    }

    // Push return value with correct tag.
    if (ret_tag != static_cast<uint8_t>(interpreter::ValueTag::Void)) {
        frame.stack[frame.sp] = result;
        frame.stack_tags[frame.sp] = ret_tag;
        ++frame.sp;
    }
    ++frame.pc;
}

static void Handle_Call_DoMIC(FastFrame& frame,
    const interpreter::IRInstruction&,
    uint64_t* raw_args, uint8_t*, uint32_t ac,
    const ri::CachedCallInfo* cache_info) noexcept
{
    using DirectFn = uint64_t (*)(uint64_t, uint64_t, uint64_t, uint64_t,
                                  uint64_t, uint64_t, uint64_t, uint64_t);
    auto fn = reinterpret_cast<DirectFn>(cache_info->direct_ptr);
    uint64_t a0 = (ac > 0) ? raw_args[0] : 0;
    uint64_t a1 = (ac > 1) ? raw_args[1] : 0;
    uint64_t a2 = (ac > 2) ? raw_args[2] : 0;
    uint64_t a3 = (ac > 3) ? raw_args[3] : 0;
    uint64_t a4 = (ac > 4) ? raw_args[4] : 0;
    uint64_t a5 = (ac > 5) ? raw_args[5] : 0;
    uint64_t a6 = (ac > 6) ? raw_args[6] : 0;
    uint64_t a7 = (ac > 7) ? raw_args[7] : 0;
    uint64_t result = fn(a0, a1, a2, a3, a4, a5, a6, a7);

    // Push return value with cached tag.
    auto ret_tag = static_cast<uint8_t>(cache_info->ret_tag);
    if (ret_tag != static_cast<uint8_t>(interpreter::ValueTag::Void) &&
        ret_tag != static_cast<uint8_t>(interpreter::ValueTag::Struct)) {
        frame.stack[frame.sp] = result;
        frame.stack_tags[frame.sp] = ret_tag;
        ++frame.sp;
    }
    ++frame.pc;
}

static void Handle_Call_DoRaw(FastFrame& frame,
    const interpreter::IRInstruction& instr,
    uint64_t* raw_args, uint8_t* raw_tags, uint32_t ac,
    const ri::CachedCallInfo* cache_info) noexcept
{
    auto dret = ri::InterpreterDispatchRaw(
        instr.call_target, raw_args, raw_tags, ac,
        instr.is_instance_call,
        frame.dispatch_ctx,
        cache_info);

    if (dret.threw_exception) {
        frame.threw_exception = true;
        frame.exception_obj_val = dret.exception_obj;
        frame.pc = 9999;
        return;
    }

    if (dret.has_value) {
        if (dret.tag == static_cast<uint8_t>(interpreter::ValueTag::Struct) &&
            dret.struct_data != nullptr) {
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

// ── PopCallArgs: RAII helper for popping call arguments from FastFrame ──
// Shared by Handle_Call and Handle_CallVirt to eliminate duplicated
// small-buffer / malloc arg-pop logic.
struct PopCallArgs {
    uint64_t* args;
    uint8_t*  tags;
    uint32_t  count;
    bool      heap_allocated;

    PopCallArgs(FastFrame& frame, uint32_t ac) noexcept
        : count(ac), heap_allocated(ac > 8) {
        if (ac <= 8) {
            args = stack_buf;
            tags = tags_buf;
        } else {
            args = static_cast<uint64_t*>(CHAOS_IL2CPP_MALLOC(sizeof(uint64_t) * ac));
            tags = static_cast<uint8_t*>(CHAOS_IL2CPP_MALLOC(sizeof(uint8_t) * ac));
        }
        if (args == nullptr || tags == nullptr) {
            if (heap_allocated) { CHAOS_IL2CPP_FREE(args); CHAOS_IL2CPP_FREE(tags); }
            args = nullptr;
            tags = nullptr;
            return;
        }
        for (uint32_t i = ac; i > 0; --i) {
            --frame.sp;
            args[i - 1] = frame.stack[frame.sp];
            tags[i - 1] = frame.stack_tags[frame.sp];
        }
    }

    ~PopCallArgs() noexcept {
        if (heap_allocated) {
            CHAOS_IL2CPP_FREE(args);
            CHAOS_IL2CPP_FREE(tags);
        }
    }

    PopCallArgs(const PopCallArgs&) = delete;
    PopCallArgs& operator=(const PopCallArgs&) = delete;

private:
    uint64_t stack_buf[8];
    uint8_t  tags_buf[8];
};

static void Handle_Call(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Call");
    if (frame.sp < instr.arg_count) {
        frame.threw_exception = true; frame.pc = 9999; return;
    }
    if (instr.call_target == nullptr && instr.direct_fn == nullptr) { ++frame.pc; return; }

    uint32_t ac = static_cast<uint32_t>(instr.arg_count);

    // ── Zero-arg fast path ─────────────────────────────────────────
    // Skip arg allocation + pop loop when there are no arguments.
    // Direct function call with no args: just invoke and push return.
    if (ac == 0 && instr.direct_fn != nullptr) {
        using DirectFn0 = uint64_t (*)();
        auto fn = reinterpret_cast<DirectFn0>(instr.direct_fn);
        uint64_t result = fn();
        uint8_t ret_tag = static_cast<uint8_t>(interpreter::ValueTag::Int32);
        if (frame.call_cache != nullptr && frame.pc < frame.call_count) {
            const auto* cc = static_cast<const ri::CachedCallInfo*>(frame.call_cache);
            if (cc[frame.pc].ret_tag != 0xFF) ret_tag = cc[frame.pc].ret_tag;
        }
        if (ret_tag != static_cast<uint8_t>(interpreter::ValueTag::Void)) {
            frame.stack[frame.sp] = result;
            frame.stack_tags[frame.sp] = ret_tag;
            ++frame.sp;
        }
        ++frame.pc;
        return;
    }

    // ── Common arg pop via RAII ─────────────────────────────────────
    PopCallArgs pa(frame, ac);
    if (pa.args == nullptr || pa.tags == nullptr) {
        frame.threw_exception = true; frame.pc = 9999; return;
    }

    // ── Route to path-specific handler ──────────────────────────────
    // AotDirectDispatch takes priority: direct_fn is a pre-resolved
    // chaos_external_runtime_* function pointer that always works.
    if (instr.direct_fn != nullptr) {
        Handle_Call_DoAotDirect(frame, instr, pa.args, pa.tags, ac);
        return;
    }

    if (instr.call_target == nullptr) {
        frame.threw_exception = true; frame.pc = 9999; return;
    }

    // Look up call-site metadata cache (pre-populated during IR lowering).
    const ri::CachedCallInfo* cache_info = nullptr;
    if (frame.call_cache != nullptr && frame.pc < frame.call_count) {
        const auto* cc = static_cast<const ri::CachedCallInfo*>(frame.call_cache);
        if (cc[frame.pc].ret_tag != 0xFF) {
            cache_info = &cc[frame.pc];
        }
    }

    // MIC path: cached direct native function pointer, not hotpatched.
    if (cache_info != nullptr &&
        cache_info->direct_ptr != nullptr &&
        !cache_info->is_patched) {
        Handle_Call_DoMIC(frame, instr, pa.args, pa.tags, ac, cache_info);
        return;
    }

    // Raw dispatch fallback.
    Handle_Call_DoRaw(frame, instr, pa.args, pa.tags, ac, cache_info);
}

static void Handle_Ret(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Ret");
    if (frame.sp > 0) {
        frame.has_ret = true;
        frame.ret_val = frame.stack[frame.sp - 1];
        frame.ret_tag = frame.stack_tags[frame.sp - 1];
    }
    frame.pc = 0xFFffFFffu; // sentinel: stop
}

// ── FastFrame helpers for type-aware stack operations ──────────────────────

// Push an InterpreterValue from the register file's tagged register value.
// This mirrors PopIV but in reverse — writes a typed value to the stack.
static void FastFrame_PushTagged(FastFrame& frame, uint64_t val, uint8_t tag_byte) noexcept {
    auto tag = static_cast<interpreter::ValueTag>(tag_byte);
    switch (tag) {
    case interpreter::ValueTag::Int32:
        frame.PushI32(static_cast<int32_t>(val)); break;
    case interpreter::ValueTag::Int64:
        frame.PushI64(static_cast<int64_t>(val)); break;
    case interpreter::ValueTag::Float32: {
        float fv; std::memcpy(&fv, &val, sizeof(float));
        frame.PushF32(fv); break;
    }
    case interpreter::ValueTag::Float64: {
        double dv; std::memcpy(&dv, &val, sizeof(double));
        frame.PushF64(dv); break;
    }
    default:
        frame.PushObj(reinterpret_cast<void*>(val)); break;
    }
}

static void Handle_Unsupported(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    frame.threw_exception = true;
    frame.pc = 9999; // signal fallback needed
}

// Map: handled by Handle_Unsupported for all opcodes not explicitly defined.
// Below we alias the common unsupported ones to the same function.

// SEH opcodes
// ── CallVirt MIC (Monomorphic Inline Cache) ─────────────────────────────
// Handle_CallVirt resolves virtual dispatch through the vtable and caches
// the result per (call_site, receiver_type) pair.  Monomorphic call sites
// (same receiver type every time) resolve once and call directly thereafter,
// skipping the ~2200ns ResolveVirtualMethodPointer call.

// Shared direct-call helper: calls fn_ptr with up to 8 raw uint64_t args.
static uint64_t CallDirectVoidPtr(void* fn_ptr, const uint64_t* raw_args, uint32_t ac) noexcept {
    using DirectFn = uint64_t (*)(uint64_t, uint64_t, uint64_t, uint64_t,
                                  uint64_t, uint64_t, uint64_t, uint64_t);
    auto fn = reinterpret_cast<DirectFn>(fn_ptr);
    return fn(
        (ac > 0) ? raw_args[0] : 0,
        (ac > 1) ? raw_args[1] : 0,
        (ac > 2) ? raw_args[2] : 0,
        (ac > 3) ? raw_args[3] : 0,
        (ac > 4) ? raw_args[4] : 0,
        (ac > 5) ? raw_args[5] : 0,
        (ac > 6) ? raw_args[6] : 0,
        (ac > 7) ? raw_args[7] : 0);
}

static void Handle_CallVirt(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_CallVirt");
    if (frame.sp < instr.arg_count) {
        frame.threw_exception = true; frame.pc = 9999; return;
    }

    uint32_t ac = static_cast<uint32_t>(instr.arg_count);

    // ── Common arg pop via RAII (shared with Handle_Call pattern) ───
    PopCallArgs pa(frame, ac);
    if (pa.args == nullptr || pa.tags == nullptr) {
        frame.threw_exception = true; frame.pc = 9999; return;
    }

    // ── MIC fast path: cached vtable entry ─────────────────────────────
    if (ac > 0 && pa.args[0] != 0 &&
        frame.call_cache != nullptr && frame.pc < frame.call_count) {
        auto* cc = static_cast<ri::CachedCallInfo*>(const_cast<void*>(frame.call_cache));
        auto& mic = cc[frame.pc];

        if (mic.ret_tag != 0xFF &&
            mic.ret_tag != static_cast<uint8_t>(interpreter::ValueTag::Struct)) {
            uint32_t receiver_token = static_cast<interpreter::InterpreterObject*>(
                reinterpret_cast<void*>(pa.args[0]))->type_token;

            // MIC read: relaxed load (benign race — all racers compute same value).
            if (mic.mic_type_token.load(std::memory_order_relaxed) == receiver_token &&
                mic.mic_dispatch_ptr.load(std::memory_order_relaxed) != nullptr &&
                mic.mic_generation.load(std::memory_order_relaxed) ==
                    g_patch_generation.load(std::memory_order_relaxed)) {
                // MIC hit — call cached vtable entry directly.
                CHAOS_IL2CPP_PROFILE_SCOPE("Handle_CallVirt_MicHit");
                uint64_t result = CallDirectVoidPtr(
                    mic.mic_dispatch_ptr.load(std::memory_order_relaxed), pa.args, ac);

                if (mic.ret_tag != static_cast<uint8_t>(interpreter::ValueTag::Void)) {
                    frame.stack[frame.sp] = result;
                    frame.stack_tags[frame.sp] = mic.ret_tag;
                    ++frame.sp;
                }

                ++frame.pc;
                return;
            }

            // MIC miss: resolve vtable and cache.
            CHAOS_IL2CPP_PROFILE_SCOPE("Handle_CallVirt_MicMiss");
            uint32_t declared_method_token = static_cast<uint32_t>(instr.secondary_index);
            if (receiver_token != 0 && declared_method_token != 0) {
                void* resolved = vr::ResolveVirtualMethodPointer(
                    receiver_token, declared_method_token);
                if (resolved != nullptr) {
                    // Cache for next time (benign race: all racers compute same value).
                    mic.mic_dispatch_ptr.store(resolved, std::memory_order_relaxed);
                    mic.mic_type_token.store(receiver_token, std::memory_order_relaxed);
                    mic.mic_generation.store(
                        g_patch_generation.load(std::memory_order_relaxed),
                        std::memory_order_relaxed);

                    uint64_t result = CallDirectVoidPtr(resolved, pa.args, ac);

                    if (mic.ret_tag != static_cast<uint8_t>(interpreter::ValueTag::Void)) {
                        frame.stack[frame.sp] = result;
                        frame.stack_tags[frame.sp] = mic.ret_tag;
                        ++frame.sp;
                    }
    
                    ++frame.pc;
                    return;
                }
            }
        }
    }

    // ── Raw dispatch fallback ─────────────────────────────────────────
    // Vtable resolution failed (null receiver, interface dispatch, struct
    // return, or no call_cache).  Delegate to InterpreterDispatchRaw.
    const ri::CachedCallInfo* cache_info = nullptr;
    if (frame.call_cache != nullptr && frame.pc < frame.call_count) {
        const auto* cc = static_cast<const ri::CachedCallInfo*>(frame.call_cache);
        if (cc[frame.pc].ret_tag != 0xFF) cache_info = &cc[frame.pc];
    }

    Handle_Call_DoRaw(frame, instr, pa.args, pa.tags, ac, cache_info);

}

static void Handle_CallBridge(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept { Handle_Call(frame, instr); }
static void Handle_Throw(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept { Handle_Unsupported(frame, instr); }
static void Handle_Leave(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    frame.pc = static_cast<uint32_t>(instr.branch_target);
}
static void Handle_EndFinally(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept { Handle_Unsupported(frame, instr); }
static void Handle_EndFilter(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept { Handle_Unsupported(frame, instr); }
static void Handle_Rethrow(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept { Handle_Unsupported(frame, instr); }

// ── Extended fast-path handlers ────────────────────────────────────────

static void Handle_LdElem(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_LdElem");
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    uint32_t index = static_cast<uint32_t>(frame.stack[--frame.sp]);
    auto* arr = static_cast<interpreter::ArrayStorage*>(
        reinterpret_cast<void*>(frame.stack[--frame.sp]));
    if (arr == nullptr || index >= arr->elements.size()) {
        frame.PushNull(); ++frame.pc; return;
    }
    frame.PushIV(arr->elements[index]);
    ++frame.pc;
}

static void Handle_StElem(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_StElem");
    if (frame.sp < 3) { frame.threw_exception = true; frame.pc = 9999; return; }
    auto val = frame.PopIV();
    uint32_t index = static_cast<uint32_t>(frame.stack[--frame.sp]);
    auto* arr = static_cast<interpreter::ArrayStorage*>(
        reinterpret_cast<void*>(frame.stack[--frame.sp]));
    if (arr == nullptr) { ++frame.pc; return; }
    if (index >= arr->elements.size()) {
        arr->elements.resize(index + 1u);
    }
    // SATB pre-write barrier: record old obj pointer before overwriting.
    using chaos::il2cpp::runtime_core::BgcSatbPreWriteBarrier;
    BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&arr->elements[index].obj));
    arr->elements[index] = val;
    chaos_gc_dirty_card(arr);
    ++frame.pc;
}

static void Handle_DivUn(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_DivUn");
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
        AssertInt32Tag(frame, frame.sp - 1);
    AssertInt32Tag(frame, frame.sp - 2);
    uint32_t r = static_cast<uint32_t>(frame.stack[--frame.sp]);
    uint32_t l = static_cast<uint32_t>(frame.stack[--frame.sp]);
    frame.PushI32(static_cast<int32_t>(l / r));
    ++frame.pc;
}

static void Handle_RemUn(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_RemUn");
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
        AssertInt32Tag(frame, frame.sp - 1);
    AssertInt32Tag(frame, frame.sp - 2);
    uint32_t r = static_cast<uint32_t>(frame.stack[--frame.sp]);
    uint32_t l = static_cast<uint32_t>(frame.stack[--frame.sp]);
    frame.PushI32(static_cast<int32_t>(l % r));
    ++frame.pc;
}

static void Handle_ConvRUn(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_ConvRUn");
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    uint32_t v = static_cast<uint32_t>(frame.stack[--frame.sp]);
    frame.PushF32(static_cast<float>(v));
    ++frame.pc;
}

static void Handle_ConvI(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_ConvI");
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    int32_t v = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI32(v);
    ++frame.pc;
}

static void Handle_ConvU(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_ConvU");
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    uint32_t v = static_cast<uint32_t>(frame.stack[--frame.sp]);
    frame.PushI32(static_cast<int32_t>(v));
    ++frame.pc;
}

static void Handle_LdInd(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_LdInd");
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    void* ptr = reinterpret_cast<void*>(frame.stack[--frame.sp]);
    if (ptr == nullptr) { frame.PushNull(); ++frame.pc; return; }
    frame.PushI64(static_cast<int64_t>(*static_cast<uint64_t*>(ptr)));
    ++frame.pc;
}

static void Handle_StInd(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_StInd");
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    uint64_t val = frame.stack[--frame.sp];
    void* ptr = reinterpret_cast<void*>(frame.stack[--frame.sp]);
    if (ptr != nullptr) {
        *static_cast<uint64_t*>(ptr) = val;
    }
    ++frame.pc;
}

static void Handle_Switch(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Switch");
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    int32_t index = static_cast<int32_t>(frame.stack[--frame.sp]);
    if (index >= 0 && instr.switch_targets != nullptr &&
        static_cast<uint32_t>(index) < instr.secondary_index) {
        frame.pc = static_cast<uint32_t>(instr.switch_targets[index]);
    } else if (instr.branch_target != static_cast<CHAOS_IL2CPP_SIZE>(-1)) {
        frame.pc = static_cast<uint32_t>(instr.branch_target);
    } else {
        ++frame.pc;
    }
}

static void Handle_LdToken(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_LdToken");
    frame.PushI32(instr.immediate_i4);
    ++frame.pc;
}

static void Handle_InitObj(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_InitObj");
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    void* ptr = reinterpret_cast<void*>(frame.stack[--frame.sp]);
    if (ptr != nullptr) {
        std::memset(ptr, 0, sizeof(interpreter::InterpreterValue));
    }
    ++frame.pc;
}

static void Handle_SizeOf(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_SizeOf");
    frame.PushI32(instr.immediate_i4);
    ++frame.pc;
}

static void Handle_LdFtn(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_LdFtn");
    frame.PushObj(instr.call_target);
    ++frame.pc;
}

static void Handle_LdVirtFtn(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    // Complex — needs virtual dispatch resolution. Fallback.
    Handle_Unsupported(frame, instr);
}

static void Handle_LdArgA(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_LdArgA");
    uint32_t idx = static_cast<uint32_t>(instr.operand_index);
    if (idx < frame.arg_count && frame.args != nullptr) {
        auto* arg_base = static_cast<const uint64_t*>(frame.args);
        frame.PushObj(const_cast<uint64_t*>(&arg_base[idx]));
    } else {
        frame.PushNull();
    }
    ++frame.pc;
}

static void Handle_LdLocA(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_LdLocA");
    uint32_t idx = static_cast<uint32_t>(instr.operand_index);
    if (idx < FastFrame::kMaxLocals) {
        frame.PushObj(&frame.locals[idx]);
    } else {
        frame.PushNull();
    }
    ++frame.pc;
}

static void Handle_LocAlloc(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_LocAlloc");
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    uint32_t size = static_cast<uint32_t>(frame.stack[--frame.sp]);
    void* mem = CHAOS_IL2CPP_MALLOC(size);
    if (mem == nullptr) { frame.PushNull(); ++frame.pc; return; }
    std::memset(mem, 0, size);
    frame.PushObj(mem);
    ++frame.pc;
}

static void Handle_Break(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    // Break is a no-op in FastExecute context.
    ++frame.pc;
}

static void Handle_LdObj(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_LdObj");
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    auto* iv = static_cast<interpreter::InterpreterValue*>(
        reinterpret_cast<void*>(frame.stack[--frame.sp]));
    if (iv != nullptr) {
        frame.PushIV(*iv);
    } else {
        frame.PushNull();
    }
    ++frame.pc;
}

static void Handle_StObj(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_StObj");
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    auto val = frame.PopIV();
    auto* iv = static_cast<interpreter::InterpreterValue*>(
        reinterpret_cast<void*>(frame.stack[--frame.sp]));
    if (iv != nullptr) {
        // SATB pre-write barrier: record old obj pointer before overwriting.
        using chaos::il2cpp::runtime_core::BgcSatbPreWriteBarrier;
        BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&iv->obj));
        *iv = val;
        chaos_gc_dirty_card(iv);
    }
    ++frame.pc;
}

static void Handle_Cpblk(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    Handle_Unsupported(frame, {});
}

static void Handle_InitBlk(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    Handle_Unsupported(frame, {});
}

static void Handle_LdElemA(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    // Load element address — in stack model, push element value (same as LdElem).
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_LdElemA");
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    uint32_t index = static_cast<uint32_t>(frame.stack[--frame.sp]);
    auto* arr = static_cast<interpreter::ArrayStorage*>(
        reinterpret_cast<void*>(frame.stack[--frame.sp]));
    if (arr == nullptr || index >= arr->elements.size()) {
        frame.PushNull(); ++frame.pc; return;
    }
    frame.PushIV(arr->elements[index]);
    ++frame.pc;
}

static void Handle_CastClass(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    // CastClass: null passthrough, non-null always succeeds in fast path.
    // If the actual type check would fail, the managed code will throw later.
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_CastClass");
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    // Keep the object on stack unchanged — CastClass is a type assertion.
    ++frame.pc;
}

static void Handle_IsInst(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    // IsInst: null returns null, non-null returns the object (passthrough).
    // Like CastClass, the actual type check is deferred.
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_IsInst");
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    // Keep the object on stack unchanged.
    ++frame.pc;
}

static void Handle_CallVirtConstrained(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    Handle_Unsupported(frame, instr);
}

static bool AddOverflowI32(int32_t l, int32_t r, int32_t& result) noexcept {
    int64_t wide = static_cast<int64_t>(l) + static_cast<int64_t>(r);
    if (wide > INT32_MAX || wide < INT32_MIN) return true;
    result = static_cast<int32_t>(wide);
    return false;
}

static bool SubOverflowI32(int32_t l, int32_t r, int32_t& result) noexcept {
    int64_t wide = static_cast<int64_t>(l) - static_cast<int64_t>(r);
    if (wide > INT32_MAX || wide < INT32_MIN) return true;
    result = static_cast<int32_t>(wide);
    return false;
}

static bool MulOverflowI32(int32_t l, int32_t r, int32_t& result) noexcept {
    int64_t wide = static_cast<int64_t>(l) * static_cast<int64_t>(r);
    if (wide > INT32_MAX || wide < INT32_MIN) return true;
    result = static_cast<int32_t>(wide);
    return false;
}

static void Handle_AddOvf(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    // Overflow-checked add — fallback to InterpreterVM on overflow
    // so the VM can throw System.OverflowException.
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_AddOvf");
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    int32_t r = static_cast<int32_t>(frame.stack[--frame.sp]);
    int32_t l = static_cast<int32_t>(frame.stack[--frame.sp]);
    int32_t result;
    if (AddOverflowI32(l, r, result)) {
        frame.threw_exception = true;
        frame.pc = 9999;
        return;
    }
    frame.PushI32(result);
    ++frame.pc;
}

static void Handle_SubOvf(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_SubOvf");
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    int32_t r = static_cast<int32_t>(frame.stack[--frame.sp]);
    int32_t l = static_cast<int32_t>(frame.stack[--frame.sp]);
    int32_t result;
    if (SubOverflowI32(l, r, result)) {
        frame.threw_exception = true;
        frame.pc = 9999;
        return;
    }
    frame.PushI32(result);
    ++frame.pc;
}

static void Handle_MulOvf(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_MulOvf");
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    int32_t r = static_cast<int32_t>(frame.stack[--frame.sp]);
    int32_t l = static_cast<int32_t>(frame.stack[--frame.sp]);
    int32_t result;
    if (MulOverflowI32(l, r, result)) {
        frame.threw_exception = true;
        frame.pc = 9999;
        return;
    }
    frame.PushI32(result);
    ++frame.pc;
}

static void Handle_ConvOvfI(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    // conv.ovf.i: native int from native int — always valid for same size.
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_ConvOvfI");
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    int32_t v = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI32(v);
    ++frame.pc;
}

static void Handle_ConvOvfI4(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    // conv.ovf.i4: int32→int32 — identity.
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_ConvOvfI4");
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    int32_t v = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI32(v);
    ++frame.pc;
}

static void Handle_ConvOvfI8(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    // conv.ovf.i8: int32→int64 — sign-extend, always valid.
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_ConvOvfI8");
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    int32_t v = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI64(static_cast<int64_t>(v));
    ++frame.pc;
}

static void Handle_ConvOvfU(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    // conv.ovf.u: int32→native uint — negative values overflow.
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_ConvOvfU");
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    int32_t v = static_cast<int32_t>(frame.stack[--frame.sp]);
    if (v < 0) {
        frame.threw_exception = true;
        frame.pc = 9999;
        return;
    }
    frame.PushI32(v);
    ++frame.pc;
}

static void Handle_ConvOvfU4(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    // conv.ovf.u4: int32→uint32 — negative values overflow.
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_ConvOvfU4");
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    int32_t v = static_cast<int32_t>(frame.stack[--frame.sp]);
    if (v < 0) {
        frame.threw_exception = true;
        frame.pc = 9999;
        return;
    }
    frame.PushI32(v);
    ++frame.pc;
}

static void Handle_ConvOvfU8(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    // conv.ovf.u8: int32→uint64 — negative values overflow.
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_ConvOvfU8");
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    int32_t v = static_cast<int32_t>(frame.stack[--frame.sp]);
    if (v < 0) {
        frame.threw_exception = true;
        frame.pc = 9999;
        return;
    }
    frame.PushI64(static_cast<uint64_t>(v));
    ++frame.pc;
}

// ── Switch dispatch (replaces kHandlers[99] table for better branch prediction) ──

#define DISPATCH_CASE(n, name) case n: Handle_##name(frame, instrs[frame.pc]); break

// ── FastExecute ────────────────────────────────────────────────────────

// Opcode frequency histogram (thread-local, zero-cost when unread).
// Dump with DumpFastExecuteOpcodeHistogram().
static thread_local uint64_t g_fast_op_freq[99] = {};

void DumpFastExecuteOpcodeHistogram() noexcept {
    printf("=== FastExecute opcode frequency ===\n");
    for (int i = 0; i < 99; ++i) {
        if (g_fast_op_freq[i] > 0) {
            printf("  %2d: %llu\n", i,
                   static_cast<unsigned long long>(g_fast_op_freq[i]));
        }
    }
}

bool FastExecute(FastFrame& frame,
                 const interpreter::IRInstruction* instrs,
                 uint32_t instr_count) noexcept {
    frame.pc = 0;

    // TLS hoist: read once before the loop — thread identity is stable
    // during FastExecute.
    auto* thread = threading::tls_this_thread;

    // Decimated interrupt check: every 64 instructions is sufficient for
    // cooperative service request polling.  A ~64-instruction latency on
    // thread abort detection is unnoticeable.
    uint32_t check_counter = 0;
    static constexpr uint32_t kCheckMask = 63;

    while (frame.pc < instr_count) {
        CHAOS_IL2CPP_PROFILE_SCOPE("FastExecute");
        uint32_t op_val = static_cast<uint32_t>(instrs[frame.pc].op_code);
        if (op_val >= 99) {
            frame.threw_exception = true;
            return false;
        }
        ++g_fast_op_freq[op_val];  // opcode histogram (thread-local, ~1 cycle)

        switch (op_val) {
            DISPATCH_CASE( 0, LdcI4);
            DISPATCH_CASE( 1, LdcI8);
            DISPATCH_CASE( 2, LdcR4);
            DISPATCH_CASE( 3, LdcR8);
            DISPATCH_CASE( 4, LdStr);
            DISPATCH_CASE( 5, LdNull);
            DISPATCH_CASE( 6, LdArg);
            DISPATCH_CASE( 7, LdLoc);
            DISPATCH_CASE( 8, StLoc);
            DISPATCH_CASE( 9, StArg);
            DISPATCH_CASE(10, LdFld);
            DISPATCH_CASE(11, StFld);
            DISPATCH_CASE(12, LdSFld);
            DISPATCH_CASE(13, StSFld);
            DISPATCH_CASE(14, Call);
            DISPATCH_CASE(15, CallVirt);
            DISPATCH_CASE(16, CallBridge);
            DISPATCH_CASE(17, Br);
            DISPATCH_CASE(18, BrTrue);
            DISPATCH_CASE(19, BrFalse);
            DISPATCH_CASE(20, Beq);
            DISPATCH_CASE(21, Blt);
            DISPATCH_CASE(22, Bgt);
            DISPATCH_CASE(23, Ble);
            DISPATCH_CASE(24, Bge);
            DISPATCH_CASE(25, Add);
            DISPATCH_CASE(26, Sub);
            DISPATCH_CASE(27, Mul);
            DISPATCH_CASE(28, Div);
            DISPATCH_CASE(29, Rem);
            DISPATCH_CASE(30, Neg);
            DISPATCH_CASE(31, Ceq);
            DISPATCH_CASE(32, Clt);
            DISPATCH_CASE(33, Cgt);
            DISPATCH_CASE(34, NewObj);
            DISPATCH_CASE(35, Box);
            DISPATCH_CASE(36, Unbox);
            DISPATCH_CASE(37, CastClass);
            DISPATCH_CASE(38, IsInst);
            DISPATCH_CASE(39, Conv_I4);
            DISPATCH_CASE(40, Conv_I8);
            DISPATCH_CASE(41, Conv_R4);
            DISPATCH_CASE(42, Conv_R8);
            DISPATCH_CASE(43, NewArr);
            DISPATCH_CASE(44, LdElem);
            DISPATCH_CASE(45, StElem);
            DISPATCH_CASE(46, LdLen);
            DISPATCH_CASE(47, Pop);
            DISPATCH_CASE(48, Throw);
            DISPATCH_CASE(49, Rethrow);
            DISPATCH_CASE(50, Leave);
            DISPATCH_CASE(51, EndFinally);
            DISPATCH_CASE(52, EndFilter);
            DISPATCH_CASE(53, Ret);
            DISPATCH_CASE(54, Dup);
            DISPATCH_CASE(55, DivUn);
            DISPATCH_CASE(56, RemUn);
            DISPATCH_CASE(57, And);
            DISPATCH_CASE(58, Or);
            DISPATCH_CASE(59, Xor);
            DISPATCH_CASE(60, Not);
            DISPATCH_CASE(61, Shl);
            DISPATCH_CASE(62, Shr);
            DISPATCH_CASE(63, ShrUn);
            DISPATCH_CASE(64, ConvRUn);
            DISPATCH_CASE(65, ConvI);
            DISPATCH_CASE(66, ConvU);
            DISPATCH_CASE(67, LdInd);
            DISPATCH_CASE(68, StInd);
            DISPATCH_CASE(69, Switch);
            DISPATCH_CASE(70, LdToken);
            DISPATCH_CASE(71, InitObj);
            DISPATCH_CASE(72, SizeOf);
            DISPATCH_CASE(73, LdFtn);
            DISPATCH_CASE(74, LdVirtFtn);
            DISPATCH_CASE(75, LdArgA);
            DISPATCH_CASE(76, LdLocA);
            DISPATCH_CASE(77, LocAlloc);
            DISPATCH_CASE(78, Break);
            DISPATCH_CASE(79, BneUn);
            DISPATCH_CASE(80, BgeUn);
            DISPATCH_CASE(81, BgtUn);
            DISPATCH_CASE(82, BleUn);
            DISPATCH_CASE(83, BltUn);
            DISPATCH_CASE(84, AddOvf);
            DISPATCH_CASE(85, SubOvf);
            DISPATCH_CASE(86, MulOvf);
            DISPATCH_CASE(87, ConvOvfI);
            DISPATCH_CASE(88, ConvOvfI4);
            DISPATCH_CASE(89, ConvOvfI8);
            DISPATCH_CASE(90, ConvOvfU);
            DISPATCH_CASE(91, ConvOvfU4);
            DISPATCH_CASE(92, ConvOvfU8);
            DISPATCH_CASE(93, LdObj);
            DISPATCH_CASE(94, StObj);
            DISPATCH_CASE(95, LdElemA);
            DISPATCH_CASE(96, Cpblk);
            DISPATCH_CASE(97, InitBlk);
            DISPATCH_CASE(98, CallVirtConstrained);
            default:
                Handle_Unsupported(frame, instrs[frame.pc]);
                break;
        }

        // Decimated thread abort/interrupt check — every 64 instructions.
        // Uses memory_order_relaxed: these are service-request flags, not
        // data dependencies, so acquire ordering is unnecessary.
        if (frame.pc != 0xFFffFFffu && frame.pc != 9999) {
            if ((++check_counter & kCheckMask) == 0) {
                if (thread != nullptr) {
                    if (thread->pending_abort.load(std::memory_order_relaxed)) {
                        frame.threw_exception = true;
                        frame.pc = 9999;
                    } else if (thread->pending_interrupt.load(std::memory_order_relaxed)) {
                        frame.threw_exception = true;
                        frame.pc = 9999;
                    }
                }
            }
        }

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