// jit_codegen_deopt.cpp — deoptimization sequence emission for
// NativeCodeGenerator (T2.4 module split of jit_engine.cpp).
//
// Pure move: EmitDeoptSequence was relocated verbatim from jit_engine.cpp; no
// logic changed.  It reads the NativeCodeGenerator member state (buf_, enc_,
// config_, deopt_values_, deopt_entries_) declared in jit_engine.h, and calls
// the extern DeoptSaveFrameState helper.

#include "jit_engine.h"
#include "ArchTraits.h"
#include "jit_helpers.h" // kDeoptMagic
#include "code_buffer.h"
#include "IEncoder.h"
#if defined(__aarch64__)
#include "arm64_encoder.h"
#include "Arm64Encoder.h"
#else
#include "X64Encoder.h"
#endif

namespace chaos::il2cpp::jit {

void NativeCodeGenerator::EmitDeoptSequence(uint32_t instr_pc, uint32_t osr_resume_pc) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::EmitDeoptSequence");
    uint32_t deopt_pos = buf_.pos();
    if (config_.enable_deopt) {
        // Spill any dirty cached regs so the stack frame is consistent
        if (config_.enable_register_caching && cached_slots_used_)
            SpillCachedRegs();

        uint32_t val_start = static_cast<uint32_t>(deopt_values_.size());
        uint32_t n_vals = RecordDeoptValues(instr_pc, osr_resume_pc);
        DeoptEntry entry;
        entry.native_offset = deopt_pos;
        entry.instr_pc = instr_pc;
        entry.osr_resume_pc = osr_resume_pc;
        entry.num_values = n_vals;
        entry.values_offset = val_start;
        deopt_entries_.push_back(entry);

        // Emit CALL to DeoptSaveFrameState(RSP) — saves all register values
        // and type tags from the stack frame to t_deopt_state before we
        // write kDeoptMagic and return to InterpreterEntryDirect.
        enc_.EmitMovRR(AT::kScratchB, AT::kStackReg); // RCX = codegen_rsp
        enc_.EmitSubRI(AT::kStackReg, 32);            // shadow space for Win64 callee
        EmitRuntimeHelperCall(::DeoptSaveFrameState);
        enc_.EmitAddRI(AT::kStackReg, 32); // restore shadow space
    }
    enc_.EmitMovImm64(AT::kScratchA, kDeoptMagic);
    // kRetBuf (RSI/X4) was set to the local GPR file in the prologue, not the
    // original ABI ret_buf from the caller.  Load the saved ret_buf from the
    // stack frame ([kFrameReg - 16] = saved RSI on x64, saved X1 on ARM64)
    // and write kDeoptMagic through it so ExecuteNative can read it.
#if defined(__aarch64__)
    // ARM64: saved ABI ret_buf (X1) at [X29 - 16].
    EmitLdur64(buf_, AT::kScratchC, AT::kFrameReg, -16);
    EmitStr64(buf_, AT::kScratchA, AT::kScratchC, 0);
#else
    enc_.EmitMovRM(AT::kScratchC, AT::kFrameReg, -16);
    enc_.EmitMovMR(AT::kScratchC, 0, AT::kScratchA);
#endif
    uint32_t patch_off = buf_.pos() + 1;
    enc_.EmitJmpRel32(0);
    deopt_jump_patches_.push_back({patch_off});
}

uint32_t NativeCodeGenerator::RecordDeoptValues(uint32_t instr_pc, uint32_t osr_resume_pc) noexcept {
    uint32_t val_start = static_cast<uint32_t>(deopt_values_.size());
    // 精确 spill (T2.3, cross-platform-unify §8): only append DeoptValues for the
    // GPR vregs LIVE at the deopt point (instr_pc) and, for an OSR continuation,
    // the resume loop header (osr_resume_pc) whose live set can differ from the
    // deopt point's.  Dead vregs are never read by the interpreter continuation,
    // so omitting their DeoptValue entry (and their stack slot from
    // reconstruction) is safe — it shrinks the serialized deopt table and avoids
    // recording values that can never be consumed.  FPR vregs (>=64) cannot be
    // filtered: liveness is a 64-bit mask and vreg >=64 is unrepresentable, so
    // they stay conservative (always recorded), matching the existing FPR policy.
    uint64_t keep = ~0ULL;
    if (liveness_computed_) {
        keep = 0;
        if (instr_pc < live_in_.size())
            keep |= live_in_[instr_pc];
        if (osr_resume_pc != 0 && osr_resume_pc < live_in_.size())
            keep |= live_in_[osr_resume_pc];
    }
    for (uint32_t vr = 0; vr < kGprCount; ++vr) {
        if (keep != ~0ULL && !(keep & (1ULL << vr)))
            continue; // dead at both deopt and OSR-resume: no DeoptValue needed
        DeoptValue dv;
        dv.reg_index = vr;
        // Use inferred type from vreg_types_ (kType* constants match ValueTag exactly)
        dv.value_tag = (vr < static_cast<uint32_t>(vreg_types_.size()))
                         ? vreg_types_[vr]
                         : static_cast<uint8_t>(interpreter::ValueTag::Int64);
        dv.is_spilled = true;
        dv.spill_offset = static_cast<int16_t>(GprOff(vr));
        deopt_values_.push_back(dv);
    }
    for (uint32_t vr = kGprCount; vr < kGprCount + kFprCount; ++vr) {
        DeoptValue dv;
        dv.reg_index = vr;
        dv.value_tag = static_cast<uint8_t>(interpreter::ValueTag::Float64);
        dv.is_spilled = true;
        dv.spill_offset = static_cast<int16_t>(FprOff(vr));
        deopt_values_.push_back(dv);
    }
    return static_cast<uint32_t>(deopt_values_.size()) - val_start;
}
} // namespace chaos::il2cpp::jit
