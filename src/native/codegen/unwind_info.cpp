#include "unwind_info.h"
#include "code_buffer.h"

#include <cstdlib>

namespace chaos::il2cpp::codegen {

#if defined(_WIN64)

uint32_t EmitUnwindInfo(
    CodeBuffer& buf,
    uint32_t prologue_size,
    uint32_t frame_sub_size,
    uint32_t num_push_regs,
    const uint8_t* push_reg_nums,
    const uint32_t* push_reg_offsets,
    uint32_t sub_rsp_offset,
    uint32_t set_fpreg_offset,
    bool has_seh) noexcept {

    uint32_t unwind_start = buf.pos();

    // ── Count UNWIND_CODE entries ─────────────────────────────────────
    // 1 UWOP_PUSH_NONVOL per push-register (includes rbp + rbx + rsi + cached)
    // 1 UWOP_SET_FPREG for mov rbp, rsp
    // 1 or 2 UWOP_ALLOC_* for sub rsp, K
    uint32_t code_count = num_push_regs + 1;

    bool alloc_small = (frame_sub_size <= (128u * 1024u)) &&
                       (frame_sub_size % 8 == 0);
    uint32_t alloc_code_count = alloc_small ? 1 : 2;
    code_count += alloc_code_count;

    if (code_count > 255) code_count = 255;

    // ── Emit UNWIND_INFO header (4 bytes) ─────────────────────────────
    uint8_t size_of_prolog = static_cast<uint8_t>((prologue_size + 15) / 16);
    if (size_of_prolog == 0) size_of_prolog = 1;

    // Version=1, Flags=UNW_FLAG_EHANDLER if has_seh
    uint8_t flags = has_seh ? 0x01u : 0x00u;  // UNW_FLAG_EHANDLER = 0x01
    buf.EmitByte(static_cast<uint8_t>(1 | (flags << 3)));
    buf.EmitByte(size_of_prolog);
    buf.EmitByte(static_cast<uint8_t>(code_count));
    // FrameRegister=5(RBP), FrameOffset=0
    buf.EmitByte(static_cast<uint8_t>(5));

    // ── Emit UNWIND_CODE entries (reverse prologue order) ─────────────
    // Prologue order (stack operations only):
    //   1. push rbp (offset push_reg_offsets[0])
    //   2. mov rbp, rsp (offset set_fpreg_offset) — SET_FPREG
    //   3. push rbx (offset push_reg_offsets[1])
    //   4. push rsi (offset push_reg_offsets[2])
    //   5..N push cached regs (offset push_reg_offsets[3..num_push_regs-1])
    //   N+1. sub rsp, K (offset sub_rsp_offset) — ALLOC
    //
    // Reverse order (emit order):
    //   1. sub rsp, K (ALLOC_SMALL / ALLOC_LARGE)
    //   2. push cached[N-1] ... push cached[0]
    //   3. push rsi
    //   4. push rbx
    //   5. mov rbp, rsp (SET_FPREG)
    //   6. push rbp

    // 1. sub rsp, K
    if (alloc_small) {
        uint32_t scale = frame_sub_size / 8;
        if (scale < 1) scale = 1;
        uint8_t op_info = static_cast<uint8_t>(scale - 1);
        buf.EmitByte(static_cast<uint8_t>(sub_rsp_offset));
        buf.EmitByte(static_cast<uint8_t>((UWOP_ALLOC_SMALL << 4) | (op_info & 0x0F)));
    } else {
        uint32_t scaled = frame_sub_size / 8;
        buf.EmitByte(static_cast<uint8_t>(sub_rsp_offset));
        buf.EmitByte(static_cast<uint8_t>((UWOP_ALLOC_LARGE << 4) | 0));
        buf.EmitByte(static_cast<uint8_t>(scaled & 0xFF));
        buf.EmitByte(static_cast<uint8_t>((scaled >> 8) & 0xFF));
    }

    // 2. Push cache regs + rsi + rbx (reverse order, skipping rbp at index 0)
    // push_reg_nums[0] = rbp, [1] = rbx, [2] = rsi, [3+] = cached regs
    for (uint32_t i = num_push_regs - 1; i >= 1; --i) {
        buf.EmitByte(static_cast<uint8_t>(push_reg_offsets[i]));
        buf.EmitByte(static_cast<uint8_t>((UWOP_PUSH_NONVOL << 4) | (push_reg_nums[i] & 0x0F)));
    }

    // 3. UWOP_SET_FPREG (mov rbp, rsp)
    buf.EmitByte(static_cast<uint8_t>(set_fpreg_offset));
    buf.EmitByte(static_cast<uint8_t>((UWOP_SET_FPREG << 4) | 0));

    // 4. push rbp (index 0)
    buf.EmitByte(static_cast<uint8_t>(push_reg_offsets[0]));
    buf.EmitByte(static_cast<uint8_t>((UWOP_PUSH_NONVOL << 4) | (push_reg_nums[0] & 0x0F)));

    // ── Pad to 4-byte boundary ────────────────────────────────────────
    uint32_t code_bytes = code_count * 2;
    uint32_t pad = (4 - (code_bytes % 4)) % 4;
    for (uint32_t i = 0; i < pad; ++i) {
        buf.EmitByte(0);
    }

    // ── Emit personality routine JMP thunk (V2, UNW_FLAG_EHANDLER) ────
    // The ExceptionHandler RVA in UNWIND_INFO is relative to BaseAddress
    // (code_start). The personality routine T4PersonalityRoutine is in
    // chaos_codegen.dll which may be >2GB away from the dynamic code buffer,
    // too far for a 32-bit RVA.  Solution: embed a 12-byte absolute JMP thunk
    // in the code buffer and point the RVA to it.
    //
    // Thunk: mov rax, <8-byte addr>; jmp rax
    //   REX.W MOV RAX, imm64 = 48 B8 <8-byte address>
    //   JMP RAX               = FF E0
    if (has_seh) {
        uint64_t personality_addr = reinterpret_cast<uint64_t>(&T4PersonalityRoutine);
        buf.EmitByte(0x48);  // REX.W prefix
        buf.EmitByte(0xB8);  // MOV RAX, imm64
        buf.Emit64(personality_addr);
        buf.EmitByte(0xFF);  // JMP
        buf.EmitByte(0xE0);  // RAX
    }

    return unwind_start;
}

RuntimeFunction* AllocRuntimeFunction(uint32_t unwind_info_offset,
                                      uint32_t code_size) noexcept {
    auto* rf = static_cast<RuntimeFunction*>(std::malloc(sizeof(RuntimeFunction)));
    if (rf == nullptr) return nullptr;
    rf->begin_address = 0;
    rf->end_address = code_size;
    rf->unwind_info_address = unwind_info_offset;
    return rf;
}

#endif  // _WIN64

}  // namespace chaos::il2cpp::codegen
