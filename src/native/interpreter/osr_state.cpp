// ── OsrState converter implementations ────────────────────────────────────
//
// See osr_state.h for the canonical OsrState type.
// See also: wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/22-OSR状态转换.md

#include "osr_state.h"

#include "fast_dispatch.h"
#include "ir_reg_alloc.h"

#include <cstring>

namespace chaos::il2cpp::interpreter {

// ── helpers ───────────────────────────────────────────────────────────────

static uint8_t TagToByte(ValueTag tag) noexcept {
    return static_cast<uint8_t>(tag);
}

static ValueTag ByteToTag(uint8_t b) noexcept {
    return static_cast<ValueTag>(b);
}

static uint64_t ValueToU64(const InterpreterValue& v) noexcept {
    switch (v.tag) {
    case ValueTag::Int32:  return static_cast<uint64_t>(v.i32);
    case ValueTag::Int64:  return static_cast<uint64_t>(v.i64);
    case ValueTag::Float32: { uint64_t u; memcpy(&u, &v.f32, sizeof(u)); return u; }
    case ValueTag::Float64: { uint64_t u; memcpy(&u, &v.f64, sizeof(u)); return u; }
    case ValueTag::Struct:
    case ValueTag::ManagedPtr:
    case ValueTag::ObjectRef: return reinterpret_cast<uint64_t>(v.obj);
    case ValueTag::Null:      return 0;
    default:                 return 0;
    }
}

static InterpreterValue U64ToValue(uint64_t raw, uint8_t tag_byte, uint32_t struct_size) noexcept {
    auto tag = ByteToTag(tag_byte);
    InterpreterValue v;
    v.tag = tag;
    v.struct_size = struct_size;
    switch (tag) {
    case ValueTag::Int32:    v.i32 = static_cast<int32_t>(raw); break;
    case ValueTag::Int64:    v.i64 = static_cast<int64_t>(raw); break;
    case ValueTag::Float32:  memcpy(&v.f32, &raw, sizeof(float)); break;
    case ValueTag::Float64:  memcpy(&v.f64, &raw, sizeof(double)); break;
    case ValueTag::Struct:
    case ValueTag::ManagedPtr:
    case ValueTag::ObjectRef: v.obj = reinterpret_cast<void*>(raw); break;
    case ValueTag::Null:     v.obj = nullptr; break;
    default:                 break;
    }
    return v;
}

// ── CaptureVmState ────────────────────────────────────────────────────────
//
// Reads the current ExecutionFrame (vectors) and writes into OsrState.
// Leaves tracked_objs empty — the VM does not use the FastFrame tracking
// system for its interpreter-heap objects (InterpreterObject / ArrayStorage).
// Those objects continue to live on the native heap referenced by ObjectRef
// pointers; the target FastExecute tier will NOT clean them up (they were
// allocated before OSR).  This matches existing VM leak semantics.

void CaptureVmState(ExecutionResult& result, const ExecutionFrame& frame,
                    uint32_t pc, uint32_t local_count) noexcept {
    OsrState osr;
    osr.pc = pc;

    // Capture eval stack.
    const auto& src_stack = frame.stack;
    osr.sp = static_cast<uint32_t>(src_stack.size());
    if (osr.sp > OsrState::kMaxStack) {
        osr.sp = OsrState::kMaxStack;  // Truncate — OSR not possible for deep stacks.
    }
    for (uint32_t i = 0; i < osr.sp; ++i) {
        osr.stack[i]      = ValueToU64(src_stack[i]);
        osr.stack_tags[i] = TagToByte(src_stack[i].tag);
    }

    // Capture locals.
    osr.local_count = local_count > OsrState::kMaxLocals
                          ? OsrState::kMaxLocals
                          : local_count;
    for (uint32_t i = 0; i < osr.local_count && i < static_cast<uint32_t>(frame.locals.size()); ++i) {
        osr.locals[i]      = ValueToU64(frame.locals[i]);
        osr.local_tags[i]  = TagToByte(frame.locals[i].tag);
    }

    result.wants_osr  = true;
    result.osr_state  = std::move(osr);  // Transfer ownership.
}

// ── RestoreOsrToFastFrame ─────────────────────────────────────────────────

void RestoreOsrToFastFrame(const OsrState& osr,
                           runtime_core::FastFrame& frame) noexcept {
    frame.pc = osr.pc;
    frame.sp = osr.sp;
    memcpy(frame.stack,       osr.stack,       sizeof(frame.stack));
    memcpy(frame.stack_tags,  osr.stack_tags,  sizeof(frame.stack_tags));
    memcpy(frame.locals,      osr.locals,      sizeof(frame.locals));
    memcpy(frame.local_tags,  osr.local_tags,  sizeof(frame.local_tags));
    frame.local_count = osr.local_count;

    // NOTE: tracked objects from the VM path are NOT transferred (they were
    // never tracked in the VM).  The FastFrame starts with an empty tracked
    // list; any NEW allocations during FastExecute will be tracked normally
    // and cleaned up on normal return.
}

// ── CaptureFastFrame ──────────────────────────────────────────────────────
//
// Transfers tracked object ownership from FastFrame to OsrState so the
// FastFrame can be released back to the pool without calling CleanupTracked
// on objects that need to survive into the VM tier.

void CaptureFastFrame(OsrState& osr,
                      const runtime_core::FastFrame& frame) noexcept {
    osr.pc          = frame.pc;
    osr.sp          = frame.sp;
    osr.local_count = frame.local_count;

    memcpy(osr.stack,       frame.stack,       sizeof(osr.stack));
    memcpy(osr.stack_tags,  frame.stack_tags,  sizeof(osr.stack_tags));
    memcpy(osr.locals,      frame.locals,      sizeof(osr.locals));
    memcpy(osr.local_tags,  frame.local_tags,  sizeof(osr.local_tags));

    // Transfer tracked object ownership.
    osr.tracked_cnt = frame.tracked_cnt;
    memcpy(osr.tracked_objs, frame.tracked_objs, sizeof(osr.tracked_objs));
    memcpy(osr.tracked_dtors, frame.tracked_dtors, sizeof(osr.tracked_dtors));
}

// ── RestoreOsrToVmFrame ───────────────────────────────────────────────────

void RestoreOsrToVmFrame(const OsrState& osr, ExecutionFrame& frame) noexcept {
    // Rebuild eval stack from OsrState.
    auto& dst_stack = frame.stack;
    dst_stack.clear();
    for (uint32_t i = 0; i < osr.sp; ++i) {
        auto tag = ByteToTag(osr.stack_tags[i]);
        uint32_t struct_size = 0;
        dst_stack.push_back(U64ToValue(osr.stack[i], osr.stack_tags[i], struct_size));
    }

    // Rebuild locals.
    auto& dst_locals = frame.locals;
    dst_locals.clear();
    for (uint32_t i = 0; i < osr.local_count; ++i) {
        dst_locals.push_back(U64ToValue(osr.locals[i], osr.local_tags[i], 0));
    }
}

// ── CaptureRegisterFrame ───────────────────────────────────────────────────
//
// Reads the current RegisterFrame register file + RegStackMapEntry and writes
// into OsrState flat arrays.  Used when RegisterExecute hits an unsupported
// opcode and needs to demote to FastExecute/VM.

void CaptureRegisterFrame(OsrState& osr,
                          const RegisterFrame& frame,
                          const RegStackMapEntry& stack_entry,
                          uint32_t arg_count,
                          uint32_t local_count) noexcept {
    osr.pc = frame.pc;
    osr.sp = stack_entry.stack_depth;

    // Reconstruct evaluation stack from register file using the stack map.
    for (uint32_t i = 0; i < osr.sp && i < OsrState::kMaxStack; ++i) {
        int8_t vreg = stack_entry.slot_regs[i];
        if (vreg >= 0) {
            osr.stack[i]      = frame.regs.reg(static_cast<uint32_t>(vreg));
            osr.stack_tags[i] = frame.regs.reg_tag(static_cast<uint32_t>(vreg));
        } else {
            osr.stack[i]      = 0;
            osr.stack_tags[i] = static_cast<uint8_t>(ValueTag::Void);
        }
    }

    // Reconstruct locals from register file using the local map.
    osr.local_count = local_count > OsrState::kMaxLocals
                          ? OsrState::kMaxLocals
                          : local_count;
    for (uint32_t i = 0; i < osr.local_count; ++i) {
        int8_t vreg = stack_entry.local_regs[i];
        if (vreg >= 0) {
            osr.locals[i]      = frame.regs.reg(static_cast<uint32_t>(vreg));
            osr.local_tags[i]  = frame.regs.reg_tag(static_cast<uint32_t>(vreg));
        } else {
            osr.locals[i]      = 0;
            osr.local_tags[i]  = static_cast<uint8_t>(ValueTag::Void);
        }
    }

    // Transfer tracked object ownership.
    osr.tracked_cnt = frame.tracked_cnt;
    memcpy(osr.tracked_objs, frame.tracked_objs, sizeof(osr.tracked_objs));
    memcpy(osr.tracked_dtors, frame.tracked_dtors, sizeof(osr.tracked_dtors));
}

// ── RestoreOsrToRegisterFrame ─────────────────────────────────────────────
//
// Writes OsrState flat arrays into RegisterFrame register file using the
// RegStackMapEntry.  Used when promoting from FastExecute/VM to RegisterExecute.

void RestoreOsrToRegisterFrame(const OsrState& osr,
                               RegisterFrame& frame,
                               const RegStackMapEntry& stack_entry,
                               uint32_t arg_count,
                               uint32_t local_count) noexcept {
    frame.pc = osr.pc;

    // Restore evaluation stack into virtual registers.
    for (uint32_t i = 0; i < osr.sp && i < OsrState::kMaxStack; ++i) {
        int8_t vreg = stack_entry.slot_regs[i];
        if (vreg >= 0) {
            frame.regs.set_reg(static_cast<uint32_t>(vreg),
                               osr.stack[i], osr.stack_tags[i]);
        }
    }

    // Restore locals into their dedicated registers.
    uint32_t n = osr.local_count > OsrState::kMaxLocals ? OsrState::kMaxLocals : osr.local_count;
    for (uint32_t i = 0; i < n; ++i) {
        int8_t vreg = stack_entry.local_regs[i];
        if (vreg >= 0) {
            frame.regs.set_reg(static_cast<uint32_t>(vreg),
                               osr.locals[i], osr.local_tags[i]);
        }
    }

    // NOTE: tracked objects are NOT transferred here — they remain owned by
    // the OsrState.  RegisterExecute allocates its own tracked objects as
    // needed, and CleanupTracked on normal/fallback exit handles them.
}

// ── CaptureNativeFrame ────────────────────────────────────────────────────
//
// Reads the flat GPR/FPR register file snapshots from a native code frame
// and reconstructs the evaluation stack and locals using the RegStackMapEntry.
// Used when native code deoptimizes to RegisterExecute/FastExecute/VM.
//
// The native code frame stores all virtual registers in a fixed stack layout
// (see code_generator.cpp: GprOff/FprOff helpers).  At a safepoint or deopt
// point, the runtime snapshots the current register file values and passes
// them as flat arrays to this function.
//
// Phase 3c: Since the native code generator uses a spill-all frame layout
// (every virtual register is in a fixed stack slot), the restoration is
// straightforward — the gpr_file[] and fpr_file[] arrays contain the
// current values of all virtual registers.
//
// Future: When register caching is enabled (hot GPRs mapped to x64 regs),
// the snapshots must capture both register and spill-slot values.

void CaptureNativeFrame(OsrState& osr,
                        const uint64_t* gpr_file,
                        const double* fpr_file,
                        const RegStackMapEntry& stack_entry,
                        uint32_t arg_count,
                        uint32_t local_count,
                        const uint8_t* gpr_tags) noexcept {
    if (gpr_file == nullptr) return;

    // pc is set by the caller (from the deopt entry).
    osr.sp = stack_entry.stack_depth;

    // Reconstruct evaluation stack from GPR register file using the stack map.
    for (uint32_t i = 0; i < osr.sp && i < OsrState::kMaxStack; ++i) {
        int8_t vreg = stack_entry.slot_regs[i];
        if (vreg >= 0) {
            uint32_t ur = static_cast<uint32_t>(vreg);
            if (ur < 64) {
                osr.stack[i]      = gpr_file[ur];
                osr.stack_tags[i] = (gpr_tags != nullptr && gpr_tags[ur] != 0)
                    ? gpr_tags[ur]
                    : static_cast<uint8_t>(ValueTag::Int64);
            } else {
                // FPR: read from fpr_file as raw uint64 bits.
                uint32_t fpr_idx = ur - 64;
                if (fpr_idx < 32) {
                    double d = fpr_file[fpr_idx];
                    uint64_t bits;
                    memcpy(&bits, &d, sizeof(bits));
                    osr.stack[i] = bits;
                }
                osr.stack_tags[i] = static_cast<uint8_t>(ValueTag::Float64);
            }
        } else {
            osr.stack[i]      = 0;
            osr.stack_tags[i] = static_cast<uint8_t>(ValueTag::Void);
        }
    }

    // Reconstruct locals from GPR register file using the local map.
    osr.local_count = local_count > OsrState::kMaxLocals
                          ? OsrState::kMaxLocals
                          : local_count;
    for (uint32_t i = 0; i < osr.local_count; ++i) {
        int8_t vreg = stack_entry.local_regs[i];
        if (vreg >= 0) {
            uint32_t ur = static_cast<uint32_t>(vreg);
            if (ur < 64) {
                osr.locals[i]      = gpr_file[ur];
                osr.local_tags[i]  = (gpr_tags != nullptr && gpr_tags[ur] != 0)
                    ? gpr_tags[ur]
                    : static_cast<uint8_t>(ValueTag::Int64);
            } else {
                osr.locals[i]      = 0;
                osr.local_tags[i]  = static_cast<uint8_t>(ValueTag::Void);
            }
        } else {
            osr.locals[i]      = 0;
            osr.local_tags[i]  = static_cast<uint8_t>(ValueTag::Void);
        }
    }

    // No tracked objects to transfer — native code does not use the
    // FastFrame tracking system.  The interpreter tiers will manage
    // their own tracked objects as needed.
    osr.tracked_cnt = 0;
}

}  // namespace chaos::il2cpp::interpreter
