#include "fast_dispatch.h"

#include <chaos/compiler_hints.h>
#include <chaos/runtime/execution_config.h>

#include "instantiation_engine.h"
#include "vtable_registry.h"
#include "reflection_query_model.h"
namespace ri = chaos::il2cpp::runtime_instantiation;
namespace vr = chaos::il2cpp::vtable_registry;

#include "osr_state.h"
#include "ir_reg_alloc.h"
#include <tier_manager.h>
#include <jit_engine.h>
#include <jit_seh.h>
#include <jit_helpers.h>

#include <memory_domain.h>
#include <patch_loader.h>

#include <eventpipe/ep_exception_bridge.h>
#include <diagnostics/debugger/dbg_runtime.h>

// Forward declarations from entry_direct.cpp
namespace chaos::il2cpp::runtime_core {
bool OptimizeToTier2(PatchMethod* pm) noexcept;
}

#include <climits>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#define CHAOS_IL2CPP_LOG_LEVEL 1  // WARN+ERROR visible; DEBUG/INFO compiled out for hot-path perf
#include <chaos/log.h>
#include <chaos/profile.h>
#include <chaos/pal/pal_eh.h>
#include <gc/gc_bgc_inline.h>
#include <gc/gc_root_change.h>
#include <gc/gc_helpers.h>

// Global static field storage from the full InterpreterVM.
// FastFrame reads/writes this directly so StSFld/LdSFld don't trigger fallback.
namespace chaos::il2cpp::interpreter {
struct InterpreterValue;
extern CHAOS_IL2CPP_VECTOR(InterpreterValue) g_static_fields;
}

namespace chaos::il2cpp::runtime_core {

using chaos::il2cpp::pal::PalTryCallNoExcept;
using chaos::il2cpp::runtime::kRuntimeConfig;

// ── TLS box object pool ──────────────────────────────────────────────
// Avoids per-call malloc/free for Box/NewObj by reusing InterpreterObjects.
// Each object's vector is pre-sized with capacity 1 so fields.resize(1)
// in Handle_Box avoids a second heap allocation.
static constexpr uint32_t kBoxPoolSize = 8;
static thread_local interpreter::InterpreterObject* tls_box_pool[kBoxPoolSize] = {};
static thread_local uint32_t tls_box_pool_count = 0;

/// Domain-aware destructor + free for InterpreterObject (Handle_NewObj).
static void DomainFreeInterpreterObject(void* p) noexcept {
    static_cast<interpreter::InterpreterObject*>(p)->~InterpreterObject();
    CHAOS_IL2CPP_DOMAIN_CURRENT_FREE(p);
}

/// Domain-aware destructor + free for ArrayStorage (Handle_NewArr).
static void DomainFreeArrayStorage(void* p) noexcept {
    auto* arr = static_cast<interpreter::ArrayStorage*>(p);
    if (arr->is_flat) {
        if (arr->flat_data != nullptr) {
            CHAOS_IL2CPP_DOMAIN_CURRENT_FREE(arr->flat_data);
        }
    } else {
        arr->elements.~vector();
    }
    CHAOS_IL2CPP_DOMAIN_CURRENT_FREE(p);
}

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
        CHAOS_IL2CPP_DOMAIN_CURRENT_FREE(obj);
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
        CHAOS_IL2CPP_DOMAIN_CURRENT_ALLOCATE(sizeof(interpreter::InterpreterObject)));
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

// ── SEH helper: find innermost try clause covering idx with finally/fault ──
static int FindEnclosingFinally(const interpreter::SEHClause* clauses,
                                 uint32_t clause_count,
                                 uint32_t idx) noexcept {
    for (int i = static_cast<int>(clause_count) - 1; i >= 0; --i) {
        const auto& clause = clauses[static_cast<uint32_t>(i)];
        if (idx >= clause.try_start_idx && idx < clause.try_end_idx) {
            const auto flags = static_cast<uint32_t>(clause.flags);
            if (flags == static_cast<uint32_t>(interpreter::SEHFlags::Finally) ||
                flags == static_cast<uint32_t>(interpreter::SEHFlags::Fault)) {
                return i;
            }
        }
    }
    return -1;
}

// ── SEH helper: find innermost try clause covering idx with catch handler ──
// upper_excl: exclusive upper bound for the reverse scan (for resumed search
// after a filter rejected the exception).  -1 = scan all clauses.
static int FindEnclosingCatch(const interpreter::SEHClause* clauses,
                               uint32_t clause_count,
                               uint32_t idx,
                               int upper_excl = -1) noexcept {
    int upper = (upper_excl < 0) ? static_cast<int>(clause_count) : upper_excl;
    for (int i = upper - 1; i >= 0; --i) {
        const auto& clause = clauses[static_cast<uint32_t>(i)];
        if (idx >= clause.try_start_idx && idx < clause.try_end_idx) {
            const auto flags = static_cast<uint32_t>(clause.flags);
            if (flags == static_cast<uint32_t>(interpreter::SEHFlags::Exception) ||
                flags == static_cast<uint32_t>(interpreter::SEHFlags::Filter)) {
                return i;
            }
        }
    }
    return -1;
}

// ── SEH helper: build finally/fault unwind list for a given catch clause ──
// Stores indices in frame.unwind_finally_list and returns count.
static int SetupFinallyUnwind(FastFrame& frame,
                               const interpreter::SEHClause* clauses,
                               uint32_t clause_count,
                               int catch_idx,
                               uint32_t ip) noexcept {
    frame.unwind_finally_count = 0;
    if (catch_idx < 0) return 0;
    const auto& catch_clause = clauses[static_cast<uint32_t>(catch_idx)];
    for (uint32_t i = 0; i < clause_count; ++i) {
        if (static_cast<int>(i) == catch_idx) continue;
        const auto& clause = clauses[i];
        const auto flags = static_cast<uint32_t>(clause.flags);
        if (flags == static_cast<uint32_t>(interpreter::SEHFlags::Finally) ||
            flags == static_cast<uint32_t>(interpreter::SEHFlags::Fault)) {
            // Only unwind finally/fault blocks nested within the catch's try region.
            if (ip >= clause.try_start_idx && ip < clause.try_end_idx &&
                clause.try_start_idx >= catch_clause.try_start_idx &&
                clause.try_end_idx <= catch_clause.try_end_idx &&
                frame.unwind_finally_count < FastFrame::kMaxUnwindDepth) {
                frame.unwind_finally_list[frame.unwind_finally_count++] = static_cast<int32_t>(i);
            }
        }
    }
    return frame.unwind_finally_count;
}

// ── SEH helper: dispatch an in-flight exception to the next handler ──
// Returns true if a handler (or filter) was selected and frame.pc was set.
// Returns false if no handler exists (caller must propagate via threw_exception).
//
// throw_pc: the pc at which the exception was thrown (for finally collection)
// upper_excl: exclusive upper bound for the search (resume after filter reject)
static bool DispatchExceptionToHandler(FastFrame& frame,
                                       void* exc_obj,
                                       uint32_t throw_pc,
                                       int upper_excl) noexcept {
    int catch_idx = FindEnclosingCatch(frame.seh_clauses, frame.seh_clause_count,
                                        throw_pc, upper_excl);
    if (catch_idx < 0) return false;

    const auto& clause = frame.seh_clauses[static_cast<uint32_t>(catch_idx)];
    const auto cflags = static_cast<uint32_t>(clause.flags);

    if (cflags == static_cast<uint32_t>(interpreter::SEHFlags::Filter)) {
        // Filter clause: push exception, transfer to filter expression.
        // EndFilter then commits or rejects.
        frame.filter_evaluating = true;
        frame.filter_active_clause = catch_idx;
        frame.filter_search_resume_idx = catch_idx;  // exclude on reject
        frame.filter_throw_pc = throw_pc;
        frame.exception_obj_val = exc_obj;
        if (frame.sp >= FastFrame::kMaxStack) {
            frame.threw_exception = true;
            frame.pc = 9999;
            return true;
        }
        frame.stack[frame.sp] = reinterpret_cast<uint64_t>(exc_obj);
        frame.stack_tags[frame.sp] = static_cast<uint8_t>(interpreter::ValueTag::ObjectRef);
        ++frame.sp;
        frame.pc = static_cast<uint32_t>(clause.handler_start_idx);
        return true;
    }

    // Plain catch: walk finally/fault unwind list before entering the handler.
    SetupFinallyUnwind(frame, frame.seh_clauses, frame.seh_clause_count,
                       catch_idx, throw_pc);
    if (frame.unwind_finally_count > 0) {
        frame.exception_in_flight = true;
        frame.unwind_catch_clause = static_cast<int32_t>(catch_idx);
        frame.unwind_finally_current = 0;
        int first_finally = frame.unwind_finally_list[0];
        frame.pc = static_cast<uint32_t>(
            frame.seh_clauses[static_cast<uint32_t>(first_finally)].handler_start_idx);
        frame.exception_obj_val = exc_obj;
    } else {
        frame.pc = static_cast<uint32_t>(clause.handler_start_idx);
        if (frame.sp >= FastFrame::kMaxStack) {
            frame.threw_exception = true;
            frame.pc = 9999;
            return true;
        }
        frame.stack[frame.sp] = reinterpret_cast<uint64_t>(exc_obj);
        frame.stack_tags[frame.sp] = static_cast<uint8_t>(interpreter::ValueTag::ObjectRef);
        ++frame.sp;
        frame.exception_obj_val = exc_obj;
    }
    return true;
}

// ── Op Handlers ────────────────────────────────────────────────────────
// Each handler is a small function. Naming: Handle_<IROpCode>.

static void Handle_LdcI4(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_LdcI4");
    frame.PushI32_NC(instr.immediate_i4);
    ++frame.pc;
}

static void Handle_LdcI8(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_LdcI8");
    frame.PushI64_NC(instr.immediate_i8);
    ++frame.pc;
}

static void Handle_LdcR4(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_LdcR4");
    frame.PushF32_NC(static_cast<float>(instr.immediate_r8));
    ++frame.pc;
}

static void Handle_LdcR8(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_LdcR8");
    frame.PushF64_NC(instr.immediate_r8);
    ++frame.pc;
}

static void Handle_LdStr(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_LdStr");
    if (instr.string_operand != nullptr) {
        frame.PushObj_NC(const_cast<char*>(instr.string_operand));
    } else {
        frame.PushNull_NC();
    }
    ++frame.pc;
}

static void Handle_LdNull(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_LdNull");
    frame.PushNull_NC();
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

// ── PGO branch profile recording (T5 collection) ──────────────────────────
// REMOVED: branch_profiles and BranchProfile were removed from PatchMethod.
static void RecordBranch(FastFrame& frame, bool taken) noexcept {
    (void)frame;
    (void)taken;
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
    bool taken = (val != 0);
    RecordBranch(frame, taken);
    if (taken) {
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
    bool taken = (val == 0);
    RecordBranch(frame, taken);
    if (taken) {
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
    bool taken = (l == r);
    RecordBranch(frame, taken);
    if (taken) {
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
    bool taken = (l < r);
    RecordBranch(frame, taken);
    if (taken) {
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
    bool taken = (l > r);
    RecordBranch(frame, taken);
    if (taken) {
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
    bool taken = (l <= r);
    RecordBranch(frame, taken);
    if (taken) {
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
    bool taken = (l >= r);
    RecordBranch(frame, taken);
    if (taken) {
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
    bool taken = (l != r);
    RecordBranch(frame, taken);
    if (taken) {
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
    bool taken = (l >= r);
    RecordBranch(frame, taken);
    if (taken) {
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
    bool taken = (l > r);
    RecordBranch(frame, taken);
    if (taken) {
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
    bool taken = (l <= r);
    RecordBranch(frame, taken);
    if (taken) {
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
    bool taken = (l < r);
    RecordBranch(frame, taken);
    if (taken) {
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
    frame.PushI32_NC(l + r);
    ++frame.pc;
}

static void Handle_Sub(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Sub");
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    AssertInt32Tag(frame, frame.sp - 1);
    AssertInt32Tag(frame, frame.sp - 2);
    int32_t r = static_cast<int32_t>(frame.stack[--frame.sp]);
    int32_t l = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI32_NC(l - r);
    ++frame.pc;
}

static void Handle_Mul(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Mul");
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    AssertInt32Tag(frame, frame.sp - 1);
    AssertInt32Tag(frame, frame.sp - 2);
    int32_t r = static_cast<int32_t>(frame.stack[--frame.sp]);
    int32_t l = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI32_NC(l * r);
    ++frame.pc;
}

static void Handle_Div(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Div");
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    AssertInt32Tag(frame, frame.sp - 1);
    AssertInt32Tag(frame, frame.sp - 2);
    int32_t r = static_cast<int32_t>(frame.stack[--frame.sp]);
    int32_t l = static_cast<int32_t>(frame.stack[--frame.sp]);
    if (r == 0) { frame.threw_exception = true; frame.pc = 9999; return; }
    // INT32_MIN / -1 overflows idiv on x86 (UB in C++ too).
    // Push the wraparound result manually to avoid SIGFPE.
    if (l == INT32_MIN && r == -1) {
        frame.PushI32_NC(INT32_MIN);
        ++frame.pc;
        return;
    }
    frame.PushI32_NC(l / r);
    ++frame.pc;
}

static void Handle_Rem(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Rem");
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    AssertInt32Tag(frame, frame.sp - 1);
    AssertInt32Tag(frame, frame.sp - 2);
    int32_t r = static_cast<int32_t>(frame.stack[--frame.sp]);
    int32_t l = static_cast<int32_t>(frame.stack[--frame.sp]);
    if (r == 0) { frame.threw_exception = true; frame.pc = 9999; return; }
    // INT32_MIN % -1 overflows idiv on x86 (UB in C++).  Result is 0.
    if (l == INT32_MIN && r == -1) {
        frame.PushI32_NC(0);
        ++frame.pc;
        return;
    }
    frame.PushI32_NC(l % r);
    ++frame.pc;
}

static void Handle_Neg(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Neg");
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
        AssertInt32Tag(frame, frame.sp - 1);
    int32_t v = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI32_NC(-v);
    ++frame.pc;
}

static void Handle_And(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_And");
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    AssertInt32Tag(frame, frame.sp - 1);
    AssertInt32Tag(frame, frame.sp - 2);
    int32_t r = static_cast<int32_t>(frame.stack[--frame.sp]);
    int32_t l = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI32_NC(l & r);
    ++frame.pc;
}

static void Handle_Or(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Or");
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    AssertInt32Tag(frame, frame.sp - 1);
    AssertInt32Tag(frame, frame.sp - 2);
    int32_t r = static_cast<int32_t>(frame.stack[--frame.sp]);
    int32_t l = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI32_NC(l | r);
    ++frame.pc;
}

static void Handle_Xor(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Xor");
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    AssertInt32Tag(frame, frame.sp - 1);
    AssertInt32Tag(frame, frame.sp - 2);
    int32_t r = static_cast<int32_t>(frame.stack[--frame.sp]);
    int32_t l = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI32_NC(l ^ r);
    ++frame.pc;
}

static void Handle_Not(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Not");
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
        AssertInt32Tag(frame, frame.sp - 1);
    int32_t v = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI32_NC(~v);
    ++frame.pc;
}

static void Handle_Shl(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Shl");
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
        AssertInt32Tag(frame, frame.sp - 1);
    AssertInt32Tag(frame, frame.sp - 2);
    int32_t a = static_cast<int32_t>(frame.stack[--frame.sp]);
    int32_t v = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI32_NC(v << (a & 0x1F));
    ++frame.pc;
}

static void Handle_Shr(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Shr");
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
        AssertInt32Tag(frame, frame.sp - 1);
    AssertInt32Tag(frame, frame.sp - 2);
    int32_t a = static_cast<int32_t>(frame.stack[--frame.sp]);
    int32_t v = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI32_NC(v >> (a & 0x1F));
    ++frame.pc;
}

static void Handle_ShrUn(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_ShrUn");
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
        AssertInt32Tag(frame, frame.sp - 1);
    AssertInt32Tag(frame, frame.sp - 2);
    int32_t a = static_cast<int32_t>(frame.stack[--frame.sp]);
    uint32_t v = static_cast<uint32_t>(frame.stack[--frame.sp]);
    frame.PushI32_NC(static_cast<int32_t>(v >> (a & 0x1F)));
    ++frame.pc;
}

static void Handle_Ceq(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Ceq");
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
        AssertInt32Tag(frame, frame.sp - 1);
    AssertInt32Tag(frame, frame.sp - 2);
    uint64_t r = frame.stack[--frame.sp];
    uint64_t l = frame.stack[--frame.sp];
    frame.PushI32_NC(l == r ? 1 : 0);
    ++frame.pc;
}

static void Handle_Clt(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Clt");
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    AssertInt32Tag(frame, frame.sp - 1);
    AssertInt32Tag(frame, frame.sp - 2);
    int32_t r = static_cast<int32_t>(frame.stack[--frame.sp]);
    int32_t l = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI32_NC(l < r ? 1 : 0);
    ++frame.pc;
}

static void Handle_Cgt(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Cgt");
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    AssertInt32Tag(frame, frame.sp - 1);
    AssertInt32Tag(frame, frame.sp - 2);
    int32_t r = static_cast<int32_t>(frame.stack[--frame.sp]);
    int32_t l = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI32_NC(l > r ? 1 : 0);
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

static void Handle_Box(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Box");
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    // Box: wrap the top-of-stack value into a heap-allocated InterpreterObject.
    // Uses TLS pool to avoid malloc per call; vector is pre-sized with capacity 1.
    auto* boxed = AcquireBoxedObject();
    if (boxed == nullptr) { frame.threw_exception = true; frame.pc = 9999; return; }
    frame.TrackPool(boxed, ReturnBoxToPool);
    boxed->fields.resize(1);
    boxed->fields[0] = frame.PopIV();
    boxed->type_token = static_cast<uint32_t>(instr.immediate_i4);
    frame.PushObj_NC(boxed);
    ++frame.pc;
}

static void Handle_Unbox(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Unbox");
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    void* obj = frame.PopObj();
    if (obj == nullptr) { frame.PushNull_NC(); ++frame.pc; return; }
    auto* boxed = static_cast<interpreter::InterpreterObject*>(obj);
    if (!boxed->fields.empty()) {
        frame.PushIV(boxed->fields[0]);
    } else {
        frame.PushNull_NC();
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
    chaos::il2cpp::runtime_core::BgcRecordRootChange(
        reinterpret_cast<void**>(&sfields[instr.field_offset].obj),
        sfields[instr.field_offset].obj);
    sfields[instr.field_offset] = frame.PopIV();
    ++frame.pc;
}

static void Handle_NewObj(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_NewObj");
    auto* storage = static_cast<interpreter::InterpreterObject*>(
        CHAOS_IL2CPP_DOMAIN_CURRENT_ALLOCATE(sizeof(interpreter::InterpreterObject)));
    if (storage == nullptr) { frame.threw_exception = true; frame.pc = 9999; return; }
    ::new (storage) interpreter::InterpreterObject();
    frame.TrackPool(storage, DomainFreeInterpreterObject);
    uint32_t field_count = static_cast<uint32_t>(
        instr.secondary_index == 0u ? 1u : instr.secondary_index);
    storage->fields.resize(field_count);
    storage->type_token = static_cast<uint32_t>(instr.immediate_i4);
    frame.PushObj_NC(storage);
    ++frame.pc;
}

static void Handle_NewArr(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_NewArr");
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    uint32_t len = static_cast<uint32_t>(frame.stack[--frame.sp]);
    auto* arr = static_cast<interpreter::ArrayStorage*>(
        CHAOS_IL2CPP_DOMAIN_CURRENT_ALLOCATE(sizeof(interpreter::ArrayStorage)));
    if (arr == nullptr) { frame.threw_exception = true; frame.pc = 9999; return; }
    ::new (arr) interpreter::ArrayStorage();
    frame.TrackPool(arr, DomainFreeArrayStorage);
    arr->type_token = static_cast<uint32_t>(instr.immediate_i4);

    // Check if this is a primitive typed array suitable for flat storage.
    // Uses the TypeInfoHandle from call_target to query the type name.
    if (instr.call_target != nullptr) {
        auto type_handle = static_cast<TypeInfoHandle>(
            reinterpret_cast<uintptr_t>(instr.call_target));
        auto* type_desc = chaos::il2cpp::runtime_core::TryDecodeReflectionQueryTypeHandle(type_handle);
        if (type_desc != nullptr && type_desc->name_utf8 != nullptr) {
            auto elem_info = interpreter::GetFlatArrayElementInfo(type_desc->name_utf8);
            if (elem_info.size > 0) {
                // Flat typed array: allocate contiguous raw memory.
                arr->is_flat = true;
                arr->flat_element_size = elem_info.size;
                arr->flat_element_tag = elem_info.value_tag;
                arr->flat_length = len;
                if (len > 0) {
                    // Defensive overflow check: len * elem_info.size must fit in size_t.
                    size_t alloc_size = static_cast<size_t>(len) * elem_info.size;
                    if (elem_info.size > 0 && alloc_size / elem_info.size != static_cast<size_t>(len)) {
                        frame.threw_exception = true; frame.pc = 9999; return;
                    }
                    arr->flat_data = CHAOS_IL2CPP_DOMAIN_CURRENT_ALLOCATE(alloc_size);
                    if (arr->flat_data == nullptr) {
                        frame.threw_exception = true; frame.pc = 9999; return;
                    }
                    std::memset(arr->flat_data, 0, alloc_size);
                }
                frame.PushObj_NC(arr);
                ++frame.pc;
                return;
            }
        }
    }

    // Fallback: reference-type or non-primitive array (existing behavior).
    arr->elements.resize(len);
    frame.PushObj_NC(arr);
    ++frame.pc;
}

static void Handle_LdLen(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_LdLen");
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    auto* arr = static_cast<interpreter::ArrayStorage*>(frame.PopObj());
    if (arr == nullptr) { frame.PushI32_NC(0); ++frame.pc; return; }
    frame.PushI32_NC(static_cast<int32_t>(arr->length()));
    ++frame.pc;
}

static void Handle_LdFld(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_LdFld");
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    void* obj = frame.PopObj();
    if (obj == nullptr) { frame.PushNull_NC(); ++frame.pc; return; }
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
    uint64_t* raw_args, uint8_t* raw_tags, uint32_t ac)
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

    uint64_t result = 0;
    bool caught = PalTryCallNoExcept(fn, a0, a1, a2, a3, a4, a5, a6, a7, result);
    if (caught) {
        frame.threw_exception = true;
        frame.pc = 9999;
        return;
    }

    // Read ret_tag from CachedCallInfo.
    // ret_tag is pre-computed by the interpreter (not codegen) to match the
    // return ABI kind. It controls FastFrame stack type tracking (Int32/Int64/
    // Float32/Float64/Void). When cache misses (0xFF), default Int32 is safe
    // because generated code always emits correct Ret-type opcodes downstream.
    // CHECK-build validation: when cache hits, ret_tag must be a valid ValueTag.
    uint8_t ret_tag = static_cast<uint8_t>(interpreter::ValueTag::Int32);
    if (frame.call_cache != nullptr && frame.pc < frame.call_count) {
        const auto* cc = static_cast<const ri::CachedCallInfo*>(frame.call_cache);
        if (cc[frame.pc].ret_tag != 0xFF) {
            ret_tag = cc[frame.pc].ret_tag;
            CHAOS_IL2CPP_ASSERT(ret_tag <= static_cast<uint8_t>(interpreter::ValueTag::Struct));
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
    const ri::CachedCallInfo* cache_info)
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
    uint64_t result = 0;
    bool caught = PalTryCallNoExcept(fn, a0, a1, a2, a3, a4, a5, a6, a7, result);
    if (caught) {
        frame.threw_exception = true;
        frame.pc = 9999;
        return;
    }

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
    const ri::CachedCallInfo* cache_info)
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
            frame.TrackPool(dret.struct_data, [](void* p) noexcept { CHAOS_IL2CPP_DOMAIN_CURRENT_FREE(p); });
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
};;

// Direct function inline path for ac ≤ 8: extracted to avoid MSVC C2712
// (cannot use __try in a function that also has objects with non-trivial
// destructors like PopCallArgs).
static void Handle_Call_DirectInline(FastFrame& frame, const interpreter::IRInstruction& instr, uint32_t ac) {
    uint64_t pop_buf[8];
    for (uint32_t i = ac; i > 0; --i) {
        --frame.sp;
        pop_buf[i - 1] = frame.stack[frame.sp];
    }
    using DirectFn = uint64_t (*)(uint64_t, uint64_t, uint64_t, uint64_t,
                                  uint64_t, uint64_t, uint64_t, uint64_t);
    auto fn = reinterpret_cast<DirectFn>(instr.direct_fn);
    uint64_t a0 = pop_buf[0];
    uint64_t a1 = (ac > 1) ? pop_buf[1] : 0;
    uint64_t a2 = (ac > 2) ? pop_buf[2] : 0;
    uint64_t a3 = (ac > 3) ? pop_buf[3] : 0;
    uint64_t a4 = (ac > 4) ? pop_buf[4] : 0;
    uint64_t a5 = (ac > 5) ? pop_buf[5] : 0;
    uint64_t a6 = (ac > 6) ? pop_buf[6] : 0;
    uint64_t a7 = (ac > 7) ? pop_buf[7] : 0;
    uint64_t result = 0;
    bool caught = PalTryCallNoExcept(fn, a0, a1, a2, a3, a4, a5, a6, a7, result);
    if (caught) {
        frame.threw_exception = true;
        frame.pc = 9999;
        return;
    }
    uint8_t ret_tag = (instr.direct_ret_tag != 0xFF)
        ? instr.direct_ret_tag
        : static_cast<uint8_t>(interpreter::ValueTag::Int32);
    if (ret_tag != static_cast<uint8_t>(interpreter::ValueTag::Void)) {
        frame.stack[frame.sp] = result;
        frame.stack_tags[frame.sp] = ret_tag;
        ++frame.sp;
    }
    ++frame.pc;
}


static void Handle_Call(FastFrame& frame, const interpreter::IRInstruction& instr) {
    if (frame.sp < instr.arg_count) {
        frame.threw_exception = true; frame.pc = 9999; return;
    }
    if (instr.call_target == nullptr && instr.direct_fn == nullptr) { ++frame.pc; return; }

    uint32_t ac = static_cast<uint32_t>(instr.arg_count);

    // ── Zero-arg fast path ─────────────────────────────────────────
    // Skip arg allocation + pop loop when there are no arguments.
    // Direct function call with no args: use PalTryCallNoExcept for EH protection.
    if (ac == 0 && instr.direct_fn != nullptr) {
        using DirectFn = uint64_t (*)(uint64_t, uint64_t, uint64_t, uint64_t,
                                      uint64_t, uint64_t, uint64_t, uint64_t);
        auto fn = reinterpret_cast<DirectFn>(instr.direct_fn);
        uint64_t result = 0;
        bool caught = PalTryCallNoExcept(fn, 0, 0, 0, 0, 0, 0, 0, 0, result);
        if (caught) {
            frame.threw_exception = true;
            frame.pc = 9999;
            return;
        }
        uint8_t ret_tag = (instr.direct_ret_tag != 0xFF)
            ? instr.direct_ret_tag
            : static_cast<uint8_t>(interpreter::ValueTag::Int32);
        if (ret_tag != static_cast<uint8_t>(interpreter::ValueTag::Void)) {
            frame.stack[frame.sp] = result;
            frame.stack_tags[frame.sp] = ret_tag;
            ++frame.sp;
        }
        ++frame.pc;
        return;
    }

    // ââ Direct function inline path (ac <= 8): delegate to helper ââ
    // Extracted to avoid MSVC C2712 (cannot use __try in function with
    // PopCallArgs non-trivial destructor).
    if (instr.direct_fn != nullptr && ac <= 8) {
        Handle_Call_DirectInline(frame, instr, ac);
        return;
    }

    // ── Common arg pop via RAII ─────────────────────────────────────
    PopCallArgs pa(frame, ac);
    if (pa.args == nullptr || pa.tags == nullptr) {
        frame.threw_exception = true; frame.pc = 9999; return;
    }

    // ── Route to path-specific handler ──────────────────────────────
    // direct_fn with >8 args falls back to DoRaw (uniform 8-arg signature
    // can't handle more than 8).  The ac ≤ 8 direct_fn path is handled by
    // the Handle_Call_DirectInline helper handles it; only the >8 case reaches here.
    if (instr.direct_fn != nullptr) {
        Handle_Call_DoRaw(frame, instr, pa.args, pa.tags, ac, nullptr);
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
        frame.PushI32_NC(static_cast<int32_t>(val)); break;
    case interpreter::ValueTag::Int64:
        frame.PushI64_NC(static_cast<int64_t>(val)); break;
    case interpreter::ValueTag::Float32: {
        float fv; std::memcpy(&fv, &val, sizeof(float));
        frame.PushF32_NC(fv); break;
    }
    case interpreter::ValueTag::Float64: {
        double dv; std::memcpy(&dv, &val, sizeof(double));
        frame.PushF64_NC(dv); break;
    }
    default:
        frame.PushObj_NC(reinterpret_cast<void*>(val)); break;
    }
}

static void Handle_Unsupported(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    CHAOS_IL2CPP_LOG_WARN_M("FastExecute", "unsupported opcode={} at pc={}", static_cast<int>(instr.op_code), frame.pc);
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
// Note: Callers should use PalTryCallNoExcept directly for EH protection.
// The MIC paths (Handle_CallVirt, Handle_CallVirtConstrained) inline their
// own PalTryCallNoExcept calls instead of using this helper.
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
                // MIC hit — call cached vtable entry directly with EH protection.
                CHAOS_IL2CPP_PROFILE_SCOPE("Handle_CallVirt_MicHit");
                using DirectFn = uint64_t (*)(uint64_t, uint64_t, uint64_t, uint64_t,
                                              uint64_t, uint64_t, uint64_t, uint64_t);
                auto fn = reinterpret_cast<DirectFn>(
                    mic.mic_dispatch_ptr.load(std::memory_order_relaxed));
                uint64_t a0 = (ac > 0) ? pa.args[0] : 0;
                uint64_t a1 = (ac > 1) ? pa.args[1] : 0;
                uint64_t a2 = (ac > 2) ? pa.args[2] : 0;
                uint64_t a3 = (ac > 3) ? pa.args[3] : 0;
                uint64_t a4 = (ac > 4) ? pa.args[4] : 0;
                uint64_t a5 = (ac > 5) ? pa.args[5] : 0;
                uint64_t a6 = (ac > 6) ? pa.args[6] : 0;
                uint64_t a7 = (ac > 7) ? pa.args[7] : 0;
                uint64_t result = 0;
                bool caught = PalTryCallNoExcept(fn, a0, a1, a2, a3, a4, a5, a6, a7, result);
                if (caught) {
                    frame.threw_exception = true;
                    frame.pc = 9999;
                    return;
                }

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

                    // Call the resolved entry with EH protection.
                    using DirectFn = uint64_t (*)(uint64_t, uint64_t, uint64_t, uint64_t,
                                                  uint64_t, uint64_t, uint64_t, uint64_t);
                    auto fn = reinterpret_cast<DirectFn>(resolved);
                    uint64_t a0 = (ac > 0) ? pa.args[0] : 0;
                    uint64_t a1 = (ac > 1) ? pa.args[1] : 0;
                    uint64_t a2 = (ac > 2) ? pa.args[2] : 0;
                    uint64_t a3 = (ac > 3) ? pa.args[3] : 0;
                    uint64_t a4 = (ac > 4) ? pa.args[4] : 0;
                    uint64_t a5 = (ac > 5) ? pa.args[5] : 0;
                    uint64_t a6 = (ac > 6) ? pa.args[6] : 0;
                    uint64_t a7 = (ac > 7) ? pa.args[7] : 0;
                    uint64_t result = 0;
                    bool caught = PalTryCallNoExcept(fn, a0, a1, a2, a3, a4, a5, a6, a7, result);
                    if (caught) {
                        frame.threw_exception = true;
                        frame.pc = 9999;
                        return;
                    }

                    if (mic.ret_tag != static_cast<uint8_t>(interpreter::ValueTag::Void)) {
                        frame.stack[frame.sp] = result;
                        frame.stack_tags[frame.sp] = mic.ret_tag;
                        ++frame.sp;
                    }

                    ++frame.pc;
                    return;
                }
            }
            // ── Monomorphic shortcut via call_cache / instr.direct_fn ────
            // When vtable resolution failed (null secondary_index, or
            // ResolveVirtualMethodPointer returned nullptr), use the
            // declaring type's AOT direct_fn as a best-effort fallback.
            //
            // Two sources:
            //   1. mic.direct_ptr — set by Phase 2.3 when PrecacheCallTarget
            //      missed but deserializer resolved direct_fn (non-patched).
            //   2. instr.direct_fn — deserializer's own resolution, also
            //      available for patched methods (since CallVirt now included
            //      in direct_fn resolution at aot_core_ir_reader.cpp:503-506).
            //
            // This is correct for monomorphic call sites (the common case
            // in benchmarks).  For polymorphic sites, the declaring type's
            // method may be the wrong override — but this is still better
            // than going through method_invoke (~1500-2200ns).
            void* dfn = (mic.direct_ptr != nullptr) ? mic.direct_ptr : instr.direct_fn;
            if (dfn != nullptr && ac <= 8) {
                CHAOS_IL2CPP_PROFILE_SCOPE("Handle_CallVirt_DirectFn");
                using DirectFn = uint64_t (*)(uint64_t, uint64_t, uint64_t, uint64_t,
                                              uint64_t, uint64_t, uint64_t, uint64_t);
                auto fn = reinterpret_cast<DirectFn>(dfn);
                uint64_t a0 = (ac > 0) ? pa.args[0] : 0;
                uint64_t a1 = (ac > 1) ? pa.args[1] : 0;
                uint64_t a2 = (ac > 2) ? pa.args[2] : 0;
                uint64_t a3 = (ac > 3) ? pa.args[3] : 0;
                uint64_t a4 = (ac > 4) ? pa.args[4] : 0;
                uint64_t a5 = (ac > 5) ? pa.args[5] : 0;
                uint64_t a6 = (ac > 6) ? pa.args[6] : 0;
                uint64_t a7 = (ac > 7) ? pa.args[7] : 0;
                uint64_t result = 0;
                bool caught = PalTryCallNoExcept(fn, a0, a1, a2, a3, a4, a5, a6, a7, result);
                if (caught) {
                    frame.threw_exception = true;
                    frame.pc = 9999;
                    return;
                }
                if (mic.ret_tag != static_cast<uint8_t>(interpreter::ValueTag::Void)) {
                    frame.stack[frame.sp] = result;
                    frame.stack_tags[frame.sp] = mic.ret_tag;
                }
                ++frame.pc;
                return;
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

// ── SEH handlers ──────────────────────────────────────────────────────────
// These replace the previous Handle_Unsupported stubs.  SEH methods now
// execute entirely within FastExecute instead of falling back to InterpreterVM.

static void Handle_Throw(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Throw");
    (void)instr;
    // Pop the exception object from the stack.
    if (frame.sp == 0) { frame.threw_exception = true; frame.pc = 9999; return; }
    void* exc_obj = reinterpret_cast<void*>(frame.stack[--frame.sp]);

    // throw null → NullReferenceException per ECMA-335 §4.21.
    if (exc_obj == nullptr) {
        frame.threw_exception = true;
        frame.pc = 9999;
        return;
    }

    // Phase 1: Search for a matching catch handler (innermost first).
    if (frame.seh_clauses != nullptr && frame.seh_clause_count > 0) {
        if (DispatchExceptionToHandler(frame, exc_obj, frame.pc, /*upper_excl=*/-1)) {
            return;
        }
    }

    // No catch handler found — propagate to caller via DispatchResult.
    frame.threw_exception = true;
    frame.exception_obj_val = exc_obj;
    frame.pc = 9999;
}

static void Handle_Rethrow(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Rethrow");
    (void)instr;
    // Rethrow uses the currently active exception (stored in exception_obj_val
    // by the catch handler).  Same propagation logic as Handle_Throw but uses
    // the stored exception rather than popping from the stack.

    // Continue SEH search from current pc (after catch handler completes,
    // rethrow searches for enclosing handlers).
    if (frame.seh_clauses != nullptr && frame.seh_clause_count > 0) {
        // Use the already-stored exception_obj_val, or fall back to stack pop.
        void* exc_obj = frame.exception_obj_val;
        if (exc_obj == nullptr && frame.sp > 0) {
            exc_obj = reinterpret_cast<void*>(frame.stack[--frame.sp]);
        }
        if (exc_obj == nullptr) { frame.threw_exception = true; frame.pc = 9999; return; }

        // Walk up: search for catch handlers OUTSIDE the current handler's try region.
        // We re-use DispatchExceptionToHandler so Filter clauses are evaluated
        // through their filter expressions instead of being treated as catch-all.
        if (DispatchExceptionToHandler(frame, exc_obj, frame.pc, /*upper_excl=*/-1)) {
            return;
        }
    }

    // No enclosing catch — propagate to caller.
    frame.threw_exception = true;
    frame.pc = 9999;
}

static void Handle_Leave(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Leave");
    uint32_t target = static_cast<uint32_t>(instr.branch_target);

    // Check if we're inside a try block that has a finally/fault handler.
    if (frame.seh_clauses != nullptr && frame.seh_clause_count > 0) {
        int finally_idx = FindEnclosingFinally(frame.seh_clauses, frame.seh_clause_count, frame.pc);
        if (finally_idx >= 0) {
            // Leave from inside a try with finally — execute the finally first.
            const auto& clause = frame.seh_clauses[static_cast<uint32_t>(finally_idx)];
            frame.pending_leave = true;
            frame.pending_leave_target = target;
            frame.pc = static_cast<uint32_t>(clause.handler_start_idx);
            return;
        }
    }

    // No finally — direct branch.
    frame.pc = target;
}

static void Handle_EndFinally(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_EndFinally");

    if (frame.exception_in_flight) {
        // Phase 2 unwind: advance to the next finally/fault handler.
        frame.unwind_finally_current++;
        if (frame.unwind_finally_current < frame.unwind_finally_count) {
            int next_finally = frame.unwind_finally_list[frame.unwind_finally_current];
            frame.pc = static_cast<uint32_t>(frame.seh_clauses[static_cast<uint32_t>(next_finally)].handler_start_idx);
            return;
        }
        // All finally/fault handlers done — transfer to catch handler.
        frame.exception_in_flight = false;
        const auto& catch_clause = frame.seh_clauses[static_cast<uint32_t>(frame.unwind_catch_clause)];
        frame.pc = static_cast<uint32_t>(catch_clause.handler_start_idx);
        // Push exception object onto stack for the catch handler.
        if (frame.sp >= FastFrame::kMaxStack) { frame.threw_exception = true; frame.pc = 9999; return; }
        frame.stack[frame.sp] = reinterpret_cast<uint64_t>(frame.exception_obj_val);
        frame.stack_tags[frame.sp] = static_cast<uint8_t>(interpreter::ValueTag::ObjectRef);
        ++frame.sp;
        frame.exception_obj_val = nullptr;
        return;
    }

    if (frame.pending_leave) {
        // Normal finally completion after Leave — jump to the leave target.
        uint32_t target = frame.pending_leave_target;
        frame.pending_leave = false;
        frame.pc = target;
        return;
    }

    // Normal endfinally — continue sequentially.
    ++frame.pc;
}

static void Handle_EndFilter(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_EndFilter");
    // EndFilter consumes a single i32 from the stack:
    //   non-zero (1) → exception accepted; transfer to handler
    //   zero    (0) → exception rejected; resume search outward
    //
    // If invoked outside an in-flight filter (defensive), drop the result and
    // continue sequentially.
    if (!frame.filter_evaluating) {
        if (frame.sp > 0) --frame.sp;
        ++frame.pc;
        return;
    }

    // Pop filter result.
    int32_t filter_result = 0;
    if (frame.sp > 0) {
        --frame.sp;
        filter_result = static_cast<int32_t>(frame.stack[frame.sp]);
    }

    const int32_t active = frame.filter_active_clause;
    const int32_t resume_excl = frame.filter_search_resume_idx;
    const uint32_t throw_pc = frame.filter_throw_pc;
    void* exc_obj = frame.exception_obj_val;

    frame.filter_evaluating = false;
    frame.filter_active_clause = -1;

    if (filter_result != 0) {
        // Filter accepts: enter the handler for the active clause, walking
        // any nested finally/fault first.
        if (active < 0 || frame.seh_clauses == nullptr) {
            frame.threw_exception = true;
            frame.pc = 9999;
            return;
        }
        const auto& clause = frame.seh_clauses[static_cast<uint32_t>(active)];
        SetupFinallyUnwind(frame, frame.seh_clauses, frame.seh_clause_count,
                           active, throw_pc);
        if (frame.unwind_finally_count > 0) {
            frame.exception_in_flight = true;
            frame.unwind_catch_clause = active;
            frame.unwind_finally_current = 0;
            int first_finally = frame.unwind_finally_list[0];
            frame.pc = static_cast<uint32_t>(
                frame.seh_clauses[static_cast<uint32_t>(first_finally)].handler_start_idx);
            frame.exception_obj_val = exc_obj;
        } else {
            frame.pc = static_cast<uint32_t>(clause.handler_start_idx);
            if (frame.sp < FastFrame::kMaxStack) {
                frame.stack[frame.sp] = reinterpret_cast<uint64_t>(exc_obj);
                frame.stack_tags[frame.sp] = static_cast<uint8_t>(interpreter::ValueTag::ObjectRef);
                ++frame.sp;
            }
        }
        return;
    }

    // Filter rejects: resume search excluding the rejected clause.
    if (frame.seh_clauses != nullptr && frame.seh_clause_count > 0 &&
        DispatchExceptionToHandler(frame, exc_obj, throw_pc, resume_excl)) {
        return;
    }
    // No further handler — propagate to caller.
    frame.threw_exception = true;
    frame.exception_obj_val = exc_obj;
    frame.pc = 9999;
}

// ── Extended fast-path handlers ────────────────────────────────────────

// ── Flat array element read helper ───────────────────────────────────
// Reads a flat array element at the given index and pushes it onto the
// frame stack with the correct ValueTag based on flat_element_tag.
static void PushFlatArrayElement(FastFrame& frame, interpreter::ArrayStorage* arr, uint32_t index) noexcept {
    void* elem_ptr = static_cast<char*>(arr->flat_data) + index * arr->flat_element_size;
    switch (arr->flat_element_size) {
    case 1:
        frame.PushI32_NC(*static_cast<int8_t*>(elem_ptr));
        return;
    case 2:
        frame.PushI32_NC(*static_cast<int16_t*>(elem_ptr));
        return;
    case 4:
        if (arr->flat_element_tag == static_cast<uint8_t>(interpreter::ValueTag::Float32)) {
            float v;
            std::memcpy(&v, elem_ptr, sizeof(float));
            frame.PushF32_NC(v);
        } else {
            frame.PushI32_NC(*static_cast<int32_t*>(elem_ptr));
        }
        return;
    case 8:
        if (arr->flat_element_tag == static_cast<uint8_t>(interpreter::ValueTag::Float64)) {
            double v;
            std::memcpy(&v, elem_ptr, sizeof(double));
            frame.PushF64_NC(v);
        } else {
            frame.PushI64_NC(*static_cast<int64_t*>(elem_ptr));
        }
        return;
    default:
        frame.PushI32_NC(0);
        return;
    }
}

static void Handle_LdElem(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_LdElem");
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    uint32_t index = static_cast<uint32_t>(frame.stack[--frame.sp]);
    auto* arr = static_cast<interpreter::ArrayStorage*>(
        reinterpret_cast<void*>(frame.stack[--frame.sp]));
    if (arr == nullptr) { frame.PushNull_NC(); ++frame.pc; return; }

    if (arr->is_flat) {
        if (index >= arr->flat_length) { frame.PushNull_NC(); ++frame.pc; return; }
        PushFlatArrayElement(frame, arr, index);
    } else {
        if (index >= arr->elements.size()) { frame.PushNull_NC(); ++frame.pc; return; }
        frame.PushIV(arr->elements[index]);
    }
    ++frame.pc;
}

// NoChk variant: caller guarantees arr != nullptr and index < size.
static void Handle_LdElemNoChk(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_LdElemNoChk");
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    uint32_t index = static_cast<uint32_t>(frame.stack[--frame.sp]);
    auto* arr = static_cast<interpreter::ArrayStorage*>(
        reinterpret_cast<void*>(frame.stack[--frame.sp]));

    if (arr->is_flat) {
        PushFlatArrayElement(frame, arr, index);
    } else {
        frame.PushIV(arr->elements[index]);
    }
    ++frame.pc;
}

// NoChk variant: caller guarantees arr != nullptr and index < size.
// Replaces Handle_StElem in hotupdate IR when bounds are known safe.
static void Handle_StElemNoChk(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_StElemNoChk");
    if (frame.sp < 3) { frame.threw_exception = true; frame.pc = 9999; return; }
    auto val = frame.PopIV();
    uint32_t index = static_cast<uint32_t>(frame.stack[--frame.sp]);
    auto* arr = static_cast<interpreter::ArrayStorage*>(
        reinterpret_cast<void*>(frame.stack[--frame.sp]));

    if (arr->is_flat) {
        // Flat arrays only hold blittable primitives — no write barrier needed.
        void* elem_ptr = static_cast<char*>(arr->flat_data) + index * arr->flat_element_size;
        switch (arr->flat_element_size) {
        case 1: *static_cast<int8_t*>(elem_ptr) = static_cast<int8_t>(val.i32); break;
        case 2: *static_cast<int16_t*>(elem_ptr) = static_cast<int16_t>(val.i32); break;
        case 4:
            if (arr->flat_element_tag == static_cast<uint8_t>(interpreter::ValueTag::Float32))
                std::memcpy(elem_ptr, &val.f32, sizeof(float));
            else
                std::memcpy(elem_ptr, &val.i32, sizeof(int32_t));
            break;
        case 8:
            if (arr->flat_element_tag == static_cast<uint8_t>(interpreter::ValueTag::Float64))
                std::memcpy(elem_ptr, &val.f64, sizeof(double));
            else
                std::memcpy(elem_ptr, &val.i64, sizeof(int64_t));
            break;
        default: break;
        }
    } else {
        using chaos::il2cpp::runtime_core::BgcSatbPreWriteBarrier;
        BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&arr->elements[index].obj));
        arr->elements[index] = val;
        chaos_gc_dirty_card(arr);
    }
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

    if (arr->is_flat) {
        if (index >= arr->flat_length) {
            // Flat arrays don't support dynamic resize — bounds error, skip.
            ++frame.pc; return;
        }
        void* elem_ptr = static_cast<char*>(arr->flat_data) + index * arr->flat_element_size;
        switch (arr->flat_element_size) {
        case 1: *static_cast<int8_t*>(elem_ptr) = static_cast<int8_t>(val.i32); break;
        case 2: *static_cast<int16_t*>(elem_ptr) = static_cast<int16_t>(val.i32); break;
        case 4:
            if (arr->flat_element_tag == static_cast<uint8_t>(interpreter::ValueTag::Float32))
                std::memcpy(elem_ptr, &val.f32, sizeof(float));
            else
                std::memcpy(elem_ptr, &val.i32, sizeof(int32_t));
            break;
        case 8:
            if (arr->flat_element_tag == static_cast<uint8_t>(interpreter::ValueTag::Float64))
                std::memcpy(elem_ptr, &val.f64, sizeof(double));
            else
                std::memcpy(elem_ptr, &val.i64, sizeof(int64_t));
            break;
        default: break;
        }
    } else {
        if (index >= arr->elements.size()) {
            arr->elements.resize(index + 1u);
        }
        // SATB pre-write barrier: record old obj pointer before overwriting.
        using chaos::il2cpp::runtime_core::BgcSatbPreWriteBarrier;
        BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&arr->elements[index].obj));
        arr->elements[index] = val;
        chaos_gc_dirty_card(arr);
    }
    ++frame.pc;
}

static void Handle_DivUn(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_DivUn");
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
        AssertInt32Tag(frame, frame.sp - 1);
    AssertInt32Tag(frame, frame.sp - 2);
    uint32_t r = static_cast<uint32_t>(frame.stack[--frame.sp]);
    uint32_t l = static_cast<uint32_t>(frame.stack[--frame.sp]);
    if (r == 0) { frame.threw_exception = true; frame.pc = 9999; return; }
    frame.PushI32_NC(static_cast<int32_t>(l / r));
    ++frame.pc;
}

static void Handle_RemUn(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_RemUn");
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
        AssertInt32Tag(frame, frame.sp - 1);
    AssertInt32Tag(frame, frame.sp - 2);
    uint32_t r = static_cast<uint32_t>(frame.stack[--frame.sp]);
    uint32_t l = static_cast<uint32_t>(frame.stack[--frame.sp]);
    if (r == 0) { frame.threw_exception = true; frame.pc = 9999; return; }
    frame.PushI32_NC(static_cast<int32_t>(l % r));
    ++frame.pc;
}

static void Handle_ConvRUn(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_ConvRUn");
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    uint32_t v = static_cast<uint32_t>(frame.stack[--frame.sp]);
    frame.PushF32_NC(static_cast<float>(v));
    ++frame.pc;
}

static void Handle_ConvI(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_ConvI");
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    int32_t v = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI32_NC(v);
    ++frame.pc;
}

static void Handle_ConvU(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_ConvU");
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    uint32_t v = static_cast<uint32_t>(frame.stack[--frame.sp]);
    frame.PushI32_NC(static_cast<int32_t>(v));
    ++frame.pc;
}

static void Handle_LdInd(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_LdInd");
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    void* ptr = reinterpret_cast<void*>(frame.stack[--frame.sp]);
    if (ptr == nullptr) { frame.PushNull_NC(); ++frame.pc; return; }
    frame.PushI64_NC(static_cast<int64_t>(*static_cast<uint64_t*>(ptr)));
    ++frame.pc;
}

static void Handle_StInd(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_StInd");
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    uint64_t val = frame.stack[--frame.sp];
    void* ptr = reinterpret_cast<void*>(frame.stack[--frame.sp]);
    if (ptr != nullptr) {
        if (chaos_is_gc_pointer(ptr)) {
            using chaos::il2cpp::runtime_core::BgcSatbPreWriteBarrier;
            BgcSatbPreWriteBarrier(reinterpret_cast<void**>(ptr));
        }
        *static_cast<uint64_t*>(ptr) = val;
        chaos_gc_dirty_card(ptr);
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
    frame.PushI32_NC(instr.immediate_i4);
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
    frame.PushI32_NC(instr.immediate_i4);
    ++frame.pc;
}

static void Handle_LdFtn(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_LdFtn");
    frame.PushObj_NC(instr.call_target);
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
        frame.PushObj_NC(const_cast<uint64_t*>(&arg_base[idx]));
    } else {
        frame.PushNull_NC();
    }
    ++frame.pc;
}

static void Handle_LdLocA(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_LdLocA");
    uint32_t idx = static_cast<uint32_t>(instr.operand_index);
    if (idx < FastFrame::kMaxLocals) {
        frame.PushObj_NC(&frame.locals[idx]);
    } else {
        frame.PushNull_NC();
    }
    ++frame.pc;
}

static void Handle_LocAlloc(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_LocAlloc");
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    uint32_t size = static_cast<uint32_t>(frame.stack[--frame.sp]);
    void* mem = CHAOS_IL2CPP_MALLOC(size);
    if (mem == nullptr) { frame.PushNull_NC(); ++frame.pc; return; }
    std::memset(mem, 0, size);
    frame.PushObj_NC(mem);
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
        frame.PushNull_NC();
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
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Cpblk");
    // Stack (bottom→top): dst, src, size
    if (frame.sp < 3) { frame.threw_exception = true; frame.pc = 9999; return; }
    uint32_t cp_size = static_cast<uint32_t>(frame.stack[--frame.sp]);
    void* src_ptr = reinterpret_cast<void*>(frame.stack[--frame.sp]);
    void* dst_ptr = reinterpret_cast<void*>(frame.stack[--frame.sp]);
    if (dst_ptr != nullptr && src_ptr != nullptr) {
        std::memcpy(dst_ptr, src_ptr, cp_size);
    }
    ++frame.pc;
}

static void Handle_InitBlk(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_InitBlk");
    // Stack (bottom→top): addr, value, size
    if (frame.sp < 3) { frame.threw_exception = true; frame.pc = 9999; return; }
    uint32_t init_size = static_cast<uint32_t>(frame.stack[--frame.sp]);
    int32_t init_value = static_cast<int32_t>(frame.stack[--frame.sp]);
    void* ptr = reinterpret_cast<void*>(frame.stack[--frame.sp]);
    if (ptr != nullptr) {
        std::memset(ptr, init_value, init_size);
    }
    ++frame.pc;
}

static void Handle_LdElemA(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    // Load element address — in stack model, push element value (same as LdElem).
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_LdElemA");
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    uint32_t index = static_cast<uint32_t>(frame.stack[--frame.sp]);
    auto* arr = static_cast<interpreter::ArrayStorage*>(
        reinterpret_cast<void*>(frame.stack[--frame.sp]));
    if (arr == nullptr) { frame.PushNull_NC(); ++frame.pc; return; }

    if (arr->is_flat) {
        if (index >= arr->flat_length) { frame.PushNull_NC(); ++frame.pc; return; }
        PushFlatArrayElement(frame, arr, index);
    } else {
        if (index >= arr->elements.size()) { frame.PushNull_NC(); ++frame.pc; return; }
        frame.PushIV(arr->elements[index]);
    }
    ++frame.pc;
}

// NoChk variant: caller guarantees arr != nullptr and index < size.
static void Handle_LdElemANoChk(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_LdElemANoChk");
    if (frame.sp < 2) { frame.threw_exception = true; frame.pc = 9999; return; }
    uint32_t index = static_cast<uint32_t>(frame.stack[--frame.sp]);
    auto* arr = static_cast<interpreter::ArrayStorage*>(
        reinterpret_cast<void*>(frame.stack[--frame.sp]));

    if (arr->is_flat) {
        PushFlatArrayElement(frame, arr, index);
    } else {
        frame.PushIV(arr->elements[index]);
    }
    ++frame.pc;
}

static void Handle_CastClass(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_CastClass");
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }

    uint8_t tag = frame.stack_tags[frame.sp - 1];
    // Null → pass through.
    if (tag == static_cast<uint8_t>(interpreter::ValueTag::Null)) {
        ++frame.pc;
        return;
    }
    // Non-object → pass through (CastClass on non-ref is a no-op).
    if (tag != static_cast<uint8_t>(interpreter::ValueTag::ObjectRef)) {
        ++frame.pc;
        return;
    }

    // Target type token from IR builder (0 = no metadata available).
    uint32_t target_type_token = static_cast<uint32_t>(instr.immediate_i4);
    if (target_type_token == 0u) {
        ++frame.pc;
        return;
    }

    // Read object's type_token (first field of InterpreterObject).
    void* raw_obj = reinterpret_cast<void*>(frame.stack[frame.sp - 1]);
    if (raw_obj == nullptr) {
        ++frame.pc;
        return;
    }
    auto* obj = static_cast<interpreter::InterpreterObject*>(raw_obj);
    uint32_t obj_type_token = obj->type_token;

    // Walk inheritance chain (exact match → base types).
    bool compatible = false;
    uint32_t current_token = obj_type_token;
    while (current_token != 0u) {
        if (current_token == target_type_token) {
            compatible = true;
            break;
        }
        const auto* vtable = vr::TryGetTypeVTable(current_token);
        if (vtable == nullptr) break;
        current_token = vtable->base_token;
    }

    // Interface compatibility check.
    if (!compatible) {
        const auto* target_vtable = vr::TryGetTypeVTable(target_type_token);
        if (target_vtable != nullptr &&
            target_vtable->type_shape == chaos::il2cpp::common::chaos_type_shape_interface) {
            uint64_t target_stable_id = target_vtable->stable_id;
            uint32_t scan_token = obj_type_token;
            while (scan_token != 0u && !compatible) {
                const auto* scan_vtable = vr::TryGetTypeVTable(scan_token);
                if (scan_vtable == nullptr) break;
                // Check AOT iface_map.
                if (scan_vtable->iface_map != nullptr && scan_vtable->iface_count > 0u) {
                    const auto* entries = static_cast<const chaos::il2cpp::common::InterfaceMapEntry*>(
                        scan_vtable->iface_map);
                    for (uint32_t i = 0u; i < scan_vtable->iface_count; ++i) {
                        if (entries[i].iface_stable_id == target_stable_id) {
                            compatible = true;
                            break;
                        }
                    }
                }
                // Check runtime_iface_map (hot-update additions).
                if (!compatible && scan_vtable->runtime_iface_map != nullptr &&
                    scan_vtable->runtime_iface_count > 0u) {
                    const auto* rentries = static_cast<const chaos::il2cpp::common::InterfaceMapEntry*>(
                        scan_vtable->runtime_iface_map);
                    for (uint32_t i = 0u; i < scan_vtable->runtime_iface_count; ++i) {
                        if (rentries[i].iface_stable_id == target_stable_id) {
                            compatible = true;
                            break;
                        }
                    }
                }
                scan_token = scan_vtable->base_token;
            }
        }
    }

    if (!compatible) {
        frame.threw_exception = true;
        frame.pc = 9999;
        return;
    }
    // Object on stack unchanged — type assertion passed.
    ++frame.pc;
}

static void Handle_IsInst(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_IsInst");
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }

    uint8_t tag = frame.stack_tags[frame.sp - 1];
    // Null → return null.
    if (tag == static_cast<uint8_t>(interpreter::ValueTag::Null)) {
        frame.stack[frame.sp - 1] = 0;
        frame.stack_tags[frame.sp - 1] = static_cast<uint8_t>(interpreter::ValueTag::Null);
        ++frame.pc;
        return;
    }
    // Non-object → return null.
    if (tag != static_cast<uint8_t>(interpreter::ValueTag::ObjectRef)) {
        frame.stack[frame.sp - 1] = 0;
        frame.stack_tags[frame.sp - 1] = static_cast<uint8_t>(interpreter::ValueTag::Null);
        ++frame.pc;
        return;
    }

    uint32_t target_type_token = static_cast<uint32_t>(instr.immediate_i4);
    if (target_type_token == 0u) {
        ++frame.pc;
        return;
    }

    void* raw_obj = reinterpret_cast<void*>(frame.stack[frame.sp - 1]);
    if (raw_obj == nullptr) {
        frame.stack[frame.sp - 1] = 0;
        frame.stack_tags[frame.sp - 1] = static_cast<uint8_t>(interpreter::ValueTag::Null);
        ++frame.pc;
        return;
    }
    auto* obj = static_cast<interpreter::InterpreterObject*>(raw_obj);
    uint32_t obj_type_token = obj->type_token;

    bool compatible = false;
    uint32_t current_token = obj_type_token;
    while (current_token != 0u) {
        if (current_token == target_type_token) {
            compatible = true;
            break;
        }
        const auto* vtable = vr::TryGetTypeVTable(current_token);
        if (vtable == nullptr) break;
        current_token = vtable->base_token;
    }

    if (!compatible) {
        const auto* target_vtable = vr::TryGetTypeVTable(target_type_token);
        if (target_vtable != nullptr &&
            target_vtable->type_shape == chaos::il2cpp::common::chaos_type_shape_interface) {
            uint64_t target_stable_id = target_vtable->stable_id;
            uint32_t scan_token = obj_type_token;
            while (scan_token != 0u && !compatible) {
                const auto* scan_vtable = vr::TryGetTypeVTable(scan_token);
                if (scan_vtable == nullptr) break;
                if (scan_vtable->iface_map != nullptr && scan_vtable->iface_count > 0u) {
                    const auto* entries = static_cast<const chaos::il2cpp::common::InterfaceMapEntry*>(
                        scan_vtable->iface_map);
                    for (uint32_t i = 0u; i < scan_vtable->iface_count; ++i) {
                        if (entries[i].iface_stable_id == target_stable_id) {
                            compatible = true;
                            break;
                        }
                    }
                }
                if (!compatible && scan_vtable->runtime_iface_map != nullptr &&
                    scan_vtable->runtime_iface_count > 0u) {
                    const auto* rentries = static_cast<const chaos::il2cpp::common::InterfaceMapEntry*>(
                        scan_vtable->runtime_iface_map);
                    for (uint32_t i = 0u; i < scan_vtable->runtime_iface_count; ++i) {
                        if (rentries[i].iface_stable_id == target_stable_id) {
                            compatible = true;
                            break;
                        }
                    }
                }
                scan_token = scan_vtable->base_token;
            }
        }
    }

    if (!compatible) {
        // IsInst returns null on mismatch.
        frame.stack[frame.sp - 1] = 0;
        frame.stack_tags[frame.sp - 1] = static_cast<uint8_t>(interpreter::ValueTag::Null);
    }
    // On match: object on stack unchanged.
    ++frame.pc;
}

static void Handle_CallVirtConstrained(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_CallVirtConstrained");
    if (frame.sp < instr.arg_count) {
        frame.threw_exception = true; frame.pc = 9999; return;
    }

    uint32_t ac = static_cast<uint32_t>(instr.arg_count);

    // ── Common arg pop via RAII ─────────────────────────────────────
    PopCallArgs pa(frame, ac);
    if (pa.args == nullptr || pa.tags == nullptr) {
        frame.threw_exception = true; frame.pc = 9999; return;
    }

    // ── Check receiver type ───────────────────────────────────────
    // If the receiver is a value type (not ObjectRef), call directly
    // without boxing or vtable dispatch (the .constrained. prefix semantic).
    if (ac > 0 && pa.tags[0] != static_cast<uint8_t>(interpreter::ValueTag::ObjectRef)) {
        // Struct / value type receiver — call directly.
        if (instr.direct_fn != nullptr) {
            // AotDirectDispatch for constrained call.
            if (ac > 8) {
                Handle_Call_DoRaw(frame, instr, pa.args, pa.tags, ac, nullptr);
                return;
            }
            Handle_Call_DoAotDirect(frame, instr, pa.args, pa.tags, ac);
            return;
        }
        // Raw dispatch fallback for constrained call.
        const ri::CachedCallInfo* cache_info = nullptr;
        if (frame.call_cache != nullptr && frame.pc < frame.call_count) {
            const auto* cc = static_cast<const ri::CachedCallInfo*>(frame.call_cache);
            if (cc[frame.pc].ret_tag != 0xFF) cache_info = &cc[frame.pc];
        }
        Handle_Call_DoRaw(frame, instr, pa.args, pa.tags, ac, cache_info);
        return;
    }

    // ── ObjectRef receiver — normal virtual dispatch (same as CallVirt) ──
    // MIC fast path: cached vtable entry.
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
                // MIC hit — call cached vtable entry directly with EH protection.
                CHAOS_IL2CPP_PROFILE_SCOPE("Handle_CallVirtConstrained_MicHit");
                using DirectFn = uint64_t (*)(uint64_t, uint64_t, uint64_t, uint64_t,
                                              uint64_t, uint64_t, uint64_t, uint64_t);
                auto fn = reinterpret_cast<DirectFn>(
                    mic.mic_dispatch_ptr.load(std::memory_order_relaxed));
                uint64_t a0 = (ac > 0) ? pa.args[0] : 0;
                uint64_t a1 = (ac > 1) ? pa.args[1] : 0;
                uint64_t a2 = (ac > 2) ? pa.args[2] : 0;
                uint64_t a3 = (ac > 3) ? pa.args[3] : 0;
                uint64_t a4 = (ac > 4) ? pa.args[4] : 0;
                uint64_t a5 = (ac > 5) ? pa.args[5] : 0;
                uint64_t a6 = (ac > 6) ? pa.args[6] : 0;
                uint64_t a7 = (ac > 7) ? pa.args[7] : 0;
                uint64_t result = 0;
                bool caught = PalTryCallNoExcept(fn, a0, a1, a2, a3, a4, a5, a6, a7, result);
                if (caught) {
                    frame.threw_exception = true;
                    frame.pc = 9999;
                    return;
                }

                if (mic.ret_tag != static_cast<uint8_t>(interpreter::ValueTag::Void)) {
                    frame.stack[frame.sp] = result;
                    frame.stack_tags[frame.sp] = mic.ret_tag;
                    ++frame.sp;
                }

                ++frame.pc;
                return;
            }

            // MIC miss: resolve vtable and cache.
            CHAOS_IL2CPP_PROFILE_SCOPE("Handle_CallVirtConstrained_MicMiss");
            uint32_t declared_method_token = static_cast<uint32_t>(instr.secondary_index);
            if (receiver_token != 0 && declared_method_token != 0) {
                void* resolved = vr::ResolveVirtualMethodPointer(
                    receiver_token, declared_method_token);
                if (resolved != nullptr) {
                    using DirectFn = uint64_t (*)(uint64_t, uint64_t, uint64_t, uint64_t,
                                                  uint64_t, uint64_t, uint64_t, uint64_t);
                    mic.mic_dispatch_ptr.store(resolved, std::memory_order_relaxed);
                    mic.mic_type_token.store(receiver_token, std::memory_order_relaxed);
                    mic.mic_generation.store(
                        g_patch_generation.load(std::memory_order_relaxed),
                        std::memory_order_relaxed);

                    // Call resolved entry with EH protection.
                    auto fn2 = reinterpret_cast<DirectFn>(resolved);
                    uint64_t b0 = (ac > 0) ? pa.args[0] : 0;
                    uint64_t b1 = (ac > 1) ? pa.args[1] : 0;
                    uint64_t b2 = (ac > 2) ? pa.args[2] : 0;
                    uint64_t b3 = (ac > 3) ? pa.args[3] : 0;
                    uint64_t b4 = (ac > 4) ? pa.args[4] : 0;
                    uint64_t b5 = (ac > 5) ? pa.args[5] : 0;
                    uint64_t b6 = (ac > 6) ? pa.args[6] : 0;
                    uint64_t b7 = (ac > 7) ? pa.args[7] : 0;
                    uint64_t result2 = 0;
                    bool caught2 = PalTryCallNoExcept(fn2, b0, b1, b2, b3, b4, b5, b6, b7, result2);
                    if (caught2) {
                        frame.threw_exception = true;
                        frame.pc = 9999;
                        return;
                    }

                    if (mic.ret_tag != static_cast<uint8_t>(interpreter::ValueTag::Void)) {
                        frame.stack[frame.sp] = result2;
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
    const ri::CachedCallInfo* cache_info = nullptr;
    if (frame.call_cache != nullptr && frame.pc < frame.call_count) {
        const auto* cc = static_cast<const ri::CachedCallInfo*>(frame.call_cache);
        if (cc[frame.pc].ret_tag != 0xFF) cache_info = &cc[frame.pc];
    }

    Handle_Call_DoRaw(frame, instr, pa.args, pa.tags, ac, cache_info);
}

static bool AddOverflowI32(int32_t l, int32_t r, int32_t& result) noexcept {
    int64_t wide = static_cast<int64_t>(l) + static_cast<int64_t>(r);
    if (wide > INT32_MAX || wide < INT32_MIN) return true;
    result = static_cast<int32_t>(wide);
    return false;
}

// ── Calli: indirect call through function pointer ──────────────────────────
// IL stack layout (bottom→top): ..., arg0, arg1, ..., argN-1, fn_ptr
// Pop fn_ptr first, then args, then call through InterpreterDispatchRaw.
static void Handle_Calli(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_Calli");
    // Need at least fn_ptr (+ args) on stack.
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }

    uint32_t ac = static_cast<uint32_t>(instr.arg_count);

    // Pop function pointer from top of stack.
    void* fn_ptr = reinterpret_cast<void*>(frame.stack[--frame.sp]);

    if (fn_ptr == nullptr) { ++frame.pc; return; }

    // ── Common arg pop via RAII ─────────────────────────────────────
    PopCallArgs pa(frame, ac);
    if (pa.args == nullptr || pa.tags == nullptr) {
        frame.threw_exception = true; frame.pc = 9999; return;
    }

    // ── Dispatch through InterpreterDispatchRaw ──────────────────────
    const ri::CachedCallInfo* cache_info = nullptr;
    if (frame.call_cache != nullptr && frame.pc < frame.call_count) {
        const auto* cc = static_cast<const ri::CachedCallInfo*>(frame.call_cache);
        if (cc[frame.pc].ret_tag != 0xFF) cache_info = &cc[frame.pc];
    }

    auto dret = ri::InterpreterDispatchRaw(
        fn_ptr, pa.args, pa.tags, ac,
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
            frame.TrackPool(dret.struct_data, [](void* p) noexcept { CHAOS_IL2CPP_DOMAIN_CURRENT_FREE(p); });
        } else {
            frame.stack[frame.sp] = dret.value;
        }
        frame.stack_tags[frame.sp] = dret.tag;
        ++frame.sp;
    }
    ++frame.pc;
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
    frame.PushI32_NC(result);
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
    frame.PushI32_NC(result);
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
    frame.PushI32_NC(result);
    ++frame.pc;
}

static void Handle_ConvOvfI(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    // conv.ovf.i: native int from native int — always valid for same size.
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_ConvOvfI");
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    int32_t v = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI32_NC(v);
    ++frame.pc;
}

static void Handle_ConvOvfI4(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    // conv.ovf.i4: int32→int32 — identity.
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_ConvOvfI4");
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    int32_t v = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI32_NC(v);
    ++frame.pc;
}

static void Handle_ConvOvfI8(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    // conv.ovf.i8: int32→int64 — sign-extend, always valid.
    CHAOS_IL2CPP_PROFILE_SCOPE("Handle_ConvOvfI8");
    if (frame.sp < 1) { frame.threw_exception = true; frame.pc = 9999; return; }
    int32_t v = static_cast<int32_t>(frame.stack[--frame.sp]);
    frame.PushI64_NC(static_cast<int64_t>(v));
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
    frame.PushI32_NC(v);
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
    frame.PushI32_NC(v);
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
    frame.PushI64_NC(static_cast<uint64_t>(v));
    ++frame.pc;
}

// ── Switch dispatch (replaces kHandlers[99] table for better branch prediction) ──

#define DISPATCH_CASE(n, name) case n: Handle_##name(frame, instrs[frame.pc]); break

// ── FastExecute ────────────────────────────────────────────────────────

// OSR loop threshold — matched to ir_reg_alloc.cpp kOsrLoopThreshold.
static constexpr uint32_t kFastOsrLoopThreshold = 100;

// ── TryFastOsrPromotion ─────────────────────────────────────────────────
// Called from the FastExecute main loop when a hot backedge is detected.
// Triggers tier upgrades (T1→T2, T2→T3, T3→T4) and, when T4 codegen
// succeeds with an OSR entry point, performs full on-stack replacement
// to transfer execution to native code mid-loop.
//
// Returns true if OSR took over execution (frame.pc set to sentinel),
// false if the caller should continue FastExecute normally.
static bool TryFastOsrPromotion(FastFrame& frame) noexcept {
    if (frame.patch_method == nullptr) return false;
    auto* pm = static_cast<chaos::il2cpp::runtime_core::PatchMethod*>(frame.patch_method);
    using PM = chaos::il2cpp::runtime_core::PatchMethod;

    auto call_count = pm->call_count.load(std::memory_order_relaxed);
    auto tier = pm->tier_state.load(std::memory_order_acquire);

    // ── T1→T2: Optimize to register IR ────────────────────────────────
    if (tier == PM::kStackInterpreted && call_count >= TierManager::Get().GetAdaptiveT1Threshold()) {
        uint32_t expected = PM::kStackInterpreted;
        if (pm->tier_state.compare_exchange_strong(expected, PM::kRegisterLowering, std::memory_order_acq_rel)) {
            OptimizeToTier2(pm);
            pm->tier_state.store(PM::kRegisterMapped, std::memory_order_release);
        }
    }

    // ── T2→T3: Enqueue for background optimization ────────────────────
    tier = pm->tier_state.load(std::memory_order_acquire);
    if (tier == PM::kRegisterMapped && call_count >= TierManager::Get().GetAdaptiveT2Threshold()) {
        uint32_t expected = PM::kRegisterMapped;
        if (pm->tier_state.compare_exchange_strong(expected, PM::kOptimizeLowering, std::memory_order_acq_rel)) {
            if (!TierManager::Get().EnqueueOptimization(pm)) {
                pm->tier_state.store(PM::kRegisterMapped, std::memory_order_release);
            }
        }
    }

    // ── T3→T4: Trigger native codegen + optional OSR ──────────────────
    tier = pm->tier_state.load(std::memory_order_acquire);
    if (tier == PM::kOptimizedRegister) {
        uint32_t backoff_base = PM::kJitThreshold + pm->codegen_fail_count * 1000;
        if (call_count >= backoff_base) {
            uint32_t t4_expected = PM::kOptimizedRegister;
            if (pm->tier_state.compare_exchange_strong(t4_expected, PM::kJitted, std::memory_order_acq_rel)) {
                auto* rm = static_cast<interpreter::RegisterMethod*>(pm->cached_optimized_reg_method);
                if (rm == nullptr) rm = static_cast<interpreter::RegisterMethod*>(pm->cached_reg_method);
                if (rm != nullptr && rm->instructions.size() > 0) {
                if constexpr (kRuntimeConfig.jit) {
                    chaos::il2cpp::jit::CompileConfig cfg;
                    cfg.enable_deopt = true;
                    cfg.enable_liveness = true;
                    cfg.safepoint_fn = reinterpret_cast<void*>(&threading::SafepointPoll);
                    cfg.cooperative_fn = reinterpret_cast<void*>(&threading::EnterCooperativeMode);
                    cfg.preemptive_fn = reinterpret_cast<void*>(&threading::EnterPreemptiveMode);
                    auto* nm = chaos::il2cpp::jit::Compile(*rm, cfg);
                    if (nm != nullptr) {
                        pm->cached_native_method = nm;
                        chaos::il2cpp::jit::RegisterNativeCodeSection(nm->code, nm->code_size, nm);

                        // OSR V2: If native code has an OSR entry, transfer
                        // execution to native code with full frame state.
                        if (nm->osr_entry_offset != 0 &&
                            frame.pc < static_cast<uint32_t>(rm->instructions.size()) &&
                            frame.pc < static_cast<uint32_t>(rm->stack_map.entries.size())) {

                            // Capture FastFrame state into OsrState.
                            interpreter::OsrState osr;
                            interpreter::CaptureFastFrame(osr, frame);
                            frame.tracked_cnt = 0;  // Ownership transferred to OsrState

                            // Build RegisterFrame from captured state.
                            interpreter::RegisterFrame rf = {};
                            rf.patch_method = pm;
                            rf.args = frame.args;
                            rf.arg_count = frame.arg_count;
                            rf.dispatch_fn = frame.dispatch_fn;
                            rf.dispatch_ctx = frame.dispatch_ctx;
                            rf.call_cache = pm->call_cache;
                            rf.call_count = static_cast<uint32_t>(rm->instructions.size());
                            rf.seh_clauses = rm->seh_clauses.data();
                            rf.seh_clause_count = static_cast<uint32_t>(rm->seh_clauses.size());
                            rf.catch_handler_entries = rm->catch_handler_entries.data();
                            rf.catch_handler_count = static_cast<uint32_t>(rm->catch_handler_entries.size());
                            rf.pc = frame.pc;  // Loop header PC (backedge target)

                            // Restore captured state into RegisterFrame using
                            // the stack map entry at the loop header PC.
                            const auto& stack_entry = rm->stack_map.entries[frame.pc];
                            interpreter::RestoreOsrToRegisterFrame(
                                osr, rf, stack_entry, frame.arg_count, frame.local_count);

                            // Set OSR resume PC to trigger OSR entry on
                            // the next backward branch check in RegisterExecute.
                            chaos::il2cpp::jit::g_jit_deopt_state.osr_resume_pc = frame.pc;

                            // Transfer tracked objects from OsrState to RegisterFrame.
                            for (uint32_t i = 0; i < osr.tracked_cnt && i < interpreter::RegisterFrame::kMaxTracked; ++i) {
                                rf.tracked_objs[rf.tracked_cnt] = osr.tracked_objs[i];
                                rf.tracked_dtors[rf.tracked_cnt] = osr.tracked_dtors[i];
                                ++rf.tracked_cnt;
                            }
                            // Clear OsrState to prevent double-free on destructor.
                            osr.tracked_cnt = 0;

                            // Re-dispatch into RegisterExecute (which will
                            // detect OSR entry and promote to native code).
                            bool reg_ok = interpreter::RegisterExecute(
                                rf, rm->instructions.data(),
                                static_cast<uint32_t>(rm->instructions.size()));

                            if (reg_ok) {
                                frame.has_ret = rf.has_ret;
                                frame.ret_val = rf.ret_val;
                                frame.ret_tag = rf.ret_tag;
                                frame.pc = 0xFFffFFffu;
                                return true;  // OSR succeeded
                            }
                            // OSR failed — fall through to continue FastExecute.
                            // FastFrame state was captured to OsrState but with
                            // tracked_cnt=0 the pool Release won't double-free.
                            // The OsrState destructor cleans up original tracked
                            // objects. The caller (FastExecute loop) is at the
                            // backedge target PC which is the loop header — the
                            // loop will re-execute naturally.
                        }
                    } else {
                        ++pm->codegen_fail_count;
                        if (pm->codegen_fail_count >= PM::kMaxCodegenFailures) {
                            pm->tier_state.store(PM::kJitSkip, std::memory_order_release);
                        } else {
                            pm->tier_state.store(PM::kOptimizedRegister, std::memory_order_release);
                        }
                    }
                } else {
                    // JIT disabled — no native compilation in AOT-only builds.
                    // Set permanent skip so the tier-up path is never retried.
                    pm->tier_state.store(PM::kJitSkip, std::memory_order_release);
                }
                }
            }
        }
    }

    return false;
}

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

    // REMOVED: branch_profiles lazy allocation — fields removed from PatchMethod.

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
        if (op_val > 99) {
            frame.threw_exception = true;
            return false;
        }
        ++g_fast_op_freq[op_val];  // opcode histogram (thread-local, ~1 cycle)

        uint32_t prev_pc = frame.pc;

        // Debugger breakpoint/stepping check.
        // Hoisted behind g_dbg_any_breakpoints gate: when no debugger is active
        // (99.99% of instructions), a single relaxed atomic load eliminates the
        // patch_method read, null-check, token extraction, and DbgShouldPause call.
        if (CHAOS_IL2CPP_UNLIKELY(
                diagnostics::g_dbg_any_breakpoints.load(std::memory_order_relaxed)))
        {
            uint32_t method_token = 0;
            auto* pm = static_cast<PatchMethod*>(frame.patch_method);
            if (pm) method_token = pm->token;
            if (diagnostics::DbgShouldPause(method_token, frame.pc, 0)) {
                // Capture stack frame snapshot: walk prev_frame chain.
                diagnostics::DbgClearFrameSnapshot();
                auto& snap = diagnostics::DbgGetFrameSnapshot();
                int fi = 0;
                const runtime_core::FastFrame* walk = &frame;
                while (walk && fi < diagnostics::kDbgMaxCapturedFrames) {
                    uint32_t tk = 0;
                    auto* wpm = static_cast<PatchMethod*>(walk->patch_method);
                    if (wpm) tk = wpm->token;
                    snap.frames[fi].method_token = tk;
                    snap.frames[fi].il_offset   = walk->pc;
                    ++fi;
                    walk = static_cast<const runtime_core::FastFrame*>(walk->prev_frame);
                }
                snap.frame_count = fi;

                // Capture local variables from the top frame.
                int li = 0;
                uint32_t max_locals = frame.local_count < diagnostics::kDbgMaxCapturedLocals
                    ? frame.local_count : diagnostics::kDbgMaxCapturedLocals;
                for (uint32_t i = 0; i < max_locals; ++i) {
                    snap.locals[li].index = i;
                    snap.locals[li].value = frame.locals[i];
                    snap.locals[li].tag   = frame.local_tags[i];
                    ++li;
                }
                snap.local_count = li;

                diagnostics::DbgNotifyPaused(method_token, frame.pc);
            }
        }

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
            DISPATCH_CASE(99, Calli);
            // NoChk variants: bounds check already verified by preceding IR.
            DISPATCH_CASE(104, LdElemNoChk);
            DISPATCH_CASE(105, StElemNoChk);
            DISPATCH_CASE(106, LdElemANoChk);
            default:
                Handle_Unsupported(frame, instrs[frame.pc]);
                break;
        }

        // OSR: Detect hot-loop backward branch — if the handler took a branch
        // and the target is an earlier instruction, it's a loop backedge.
        if (frame.pc < instr_count && frame.pc < prev_pc) {
            uint32_t threshold = frame.osr_reenable ? 1 : kFastOsrLoopThreshold;
            frame.osr_reenable = false;  // one-shot
            if (++frame.loop_counter >= threshold) {
                if (TryFastOsrPromotion(frame)) continue;  // OSR took over
            }
        } else if (frame.pc == prev_pc + 1) {
            // Sequential execution — slowly decay loop_counter to prevent
            // unbounded growth in methods that exit loops.
            if (frame.loop_counter > 0) --frame.loop_counter;
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