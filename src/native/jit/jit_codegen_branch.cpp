// jit_codegen_branch.cpp — branch resolution / patching for
// NativeCodeGenerator (T2.4 module split of jit_engine.cpp).
//
// Pure move: ResolveBranches was relocated verbatim from jit_engine.cpp; no
// logic changed.  It reads the NativeCodeGenerator member state (buf_,
// branch_patches_, deopt_jump_patches_, jump_table_patches_, instr_offsets_)
// declared in jit_engine.h.

#include "jit_engine.h"
#include "ArchTraits.h"
#include "code_buffer.h"
#include "IEncoder.h"
#if defined(__aarch64__)
#include "arm64_encoder.h"
#include "Arm64Encoder.h"
#else
#include "X64Encoder.h"
#endif

namespace chaos::il2cpp::jit {

void NativeCodeGenerator::ResolveBranches() noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::ResolveBranches");
#if defined(__aarch64__)
    for (auto& bp : branch_patches_) {
        if (bp.target_instr >= instr_offsets_.size()) {
            // Out of bounds target — patch to B #4 (skip next instruction, i.e. no-op).
            buf_.Patch32(bp.patch_offset, 0x14000001u); // B #4
            continue;
        }
        uint32_t target_off = instr_offsets_[bp.target_instr];
        uint32_t current_off = bp.patch_offset;
        int64_t disp = static_cast<int64_t>(target_off) - static_cast<int64_t>(current_off);
        uint32_t instr = buf_.Load32(bp.patch_offset);
        uint8_t opcode = static_cast<uint8_t>(instr >> 24);
        if (opcode == 0x54) {
            // B.cond (condition code at bits [0:4], imm19 at bits [5:23])
            int64_t max_disp = 1048576; // ±1MB
            if (disp < -max_disp || disp > max_disp) {
                // Out of range — emit trampoline B at end of buffer
                uint32_t tramp_off = buf_.pos();
                int64_t tramp_disp = static_cast<int64_t>(target_off) - static_cast<int64_t>(tramp_off + 4);
                uint32_t tramp_imm26 = (static_cast<uint32_t>(tramp_disp) >> 2) & 0x3FFFFFF;
                buf_.Emit32(0x14000000u | tramp_imm26); // B target
                // Redirect original B.cond to trampoline
                disp = static_cast<int64_t>(tramp_off) - current_off;
            }
            uint32_t imm19 = (static_cast<uint32_t>(disp) >> 2) & 0x7FFFF;
            uint32_t new_instr = (instr & 0xFF00001Fu) | (imm19 << 5);
            buf_.Patch32(bp.patch_offset, new_instr);
        } else {
            // B (unconditional, 26-bit imm26, ±128MB range)
            uint32_t imm26 = (static_cast<uint32_t>(disp) >> 2) & 0x3FFFFFF;
            uint32_t new_instr = (instr & 0xFC000000u) | imm26;
            buf_.Patch32(bp.patch_offset, new_instr);
        }
    }
#else
    for (auto& bp : branch_patches_) {
        if (bp.target_instr >= instr_offsets_.size()) {
            buf_.Patch32(bp.patch_offset, 0);
            continue;
        }
        uint32_t target_off = instr_offsets_[bp.target_instr];
        uint32_t current_off = bp.patch_offset + 4;
        int32_t disp = static_cast<int32_t>(target_off - current_off);
        buf_.Patch32(bp.patch_offset, static_cast<uint32_t>(disp));
    }
#endif
    uint32_t deopt_ret_off = deopt_return_pos_;
    for (auto& djp : deopt_jump_patches_) {
        int32_t disp = static_cast<int32_t>(deopt_ret_off - (djp.patch_offset + 4));
        buf_.Patch32(djp.patch_offset, static_cast<uint32_t>(disp));
    }
    // Patch jump table entries: each stores target_off - table_base
    for (auto& jtp : jump_table_patches_) {
        uint32_t target_off = jtp.target_instr < instr_offsets_.size() ? instr_offsets_[jtp.target_instr] : 0;
        int32_t disp = static_cast<int32_t>(target_off - jtp.table_base);
        buf_.Patch32(jtp.table_entry_offset, static_cast<uint32_t>(disp));
    }
}
} // namespace chaos::il2cpp::jit
