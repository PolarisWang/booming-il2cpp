#include "jit_unwind.h"
#include "code_buffer.h"

#include <cstddef>
#include <cstdlib>

namespace chaos::il2cpp::jit {

// ── Cross-platform layout assertions ──────────────────────────────────
// These static_asserts verify that the compiler's struct layout matches
// the assumptions made by the manual byte-level emission below.  If the
// assertion fires on a new platform or toolchain, the emission logic must
// be adjusted for the target ABI.
static_assert(sizeof(void*) == 8, "x64 is required — only 64-bit targets are supported");

#if defined(_WIN64)
static_assert(offsetof(UnwindCode, code_offset) == 0,
              "UnwindCode.code_offset must be at offset 0 for Win64 UNWIND_CODE layout");

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

    bool alloc_small = (frame_sub_size >= 8) && (frame_sub_size <= 128) &&
                       (frame_sub_size % 8 == 0);
    uint32_t alloc_code_count = alloc_small ? 1 : 2;
    code_count += alloc_code_count;

    if (code_count > 255) code_count = 255;

    // ── Emit UNWIND_INFO header (4 bytes) ─────────────────────────────
    // SizeOfProlog = 255 (max uint8_t).  This is safe because:
    //   a) At crash time IP is always well past the prologue → the unwinder
    //      enters "after prologue" mode and applies all codes unconditionally
    //      without per-slot CodeOffset validation.
    //   b) ALLOC_LARGE's 2nd slot stores the allocation size as raw data in
    //      its "CodeOffset" byte.  A value < SizeOfProlog is required to pass
    //      ntdll's internal validation pass; 255 accommodates any reasonable
    //      single-byte allocation size value.
    uint8_t size_of_prolog = 255;

    // Version=1, Flags=UNW_FLAG_EHANDLER if has_seh
    uint8_t flags = has_seh ? 0x01u : 0x00u;  // UNW_FLAG_EHANDLER = 0x01
    buf.EmitByte(static_cast<uint8_t>(1 | (flags << 3)));
    buf.EmitByte(size_of_prolog);
    buf.EmitByte(static_cast<uint8_t>(code_count));
    // FrameRegister=5(RBP), FrameOffset=1 (CFA = RBP + 16)
    // After push rbp + mov rbp, rsp: RBP = caller_RSP - 16,
    // so CFA (caller_RSP) = RBP + 16 = RBP + FrameOffset*16 → FrameOffset=1
    buf.EmitByte(static_cast<uint8_t>((1 << 4) | 5));

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
    //   5. push rbp (PUSH_NONVOL — frame register)
    //   6. mov rbp, rsp (SET_FPREG — MUST follow PUSH_NONVOL for frame register)

    // 1. sub rsp, K
    if (alloc_small) {
        uint32_t scale = frame_sub_size / 8;
        if (scale < 1) scale = 1;
        uint8_t op_info = static_cast<uint8_t>(scale - 1);
        buf.EmitByte(static_cast<uint8_t>(sub_rsp_offset));
        buf.EmitByte(static_cast<uint8_t>((op_info << 4) | UWOP_ALLOC_SMALL));
    } else {
        uint32_t scaled = frame_sub_size / 8;
        buf.EmitByte(static_cast<uint8_t>(sub_rsp_offset));
        buf.EmitByte(static_cast<uint8_t>((1 << 4) | UWOP_ALLOC_LARGE));  // OpInfo=1: scaled by 8
        buf.EmitByte(static_cast<uint8_t>(scaled & 0xFF));
        buf.EmitByte(static_cast<uint8_t>((scaled >> 8) & 0xFF));
    }

    // 2. Push cache regs + rsi + rbx (reverse order, skipping rbp at index 0)
    // push_reg_nums[0] = rbp, [1] = rbx, [2] = rsi, [3+] = cached regs
    for (uint32_t i = num_push_regs - 1; i >= 1; --i) {
        buf.EmitByte(static_cast<uint8_t>(push_reg_offsets[i]));
        buf.EmitByte(static_cast<uint8_t>((push_reg_nums[i] << 4) | UWOP_PUSH_NONVOL));
    }

    // 3. push rbp (index 0) — frame register push
    buf.EmitByte(static_cast<uint8_t>(push_reg_offsets[0]));
    buf.EmitByte(static_cast<uint8_t>((push_reg_nums[0] << 4) | UWOP_PUSH_NONVOL));

    // 4. UWOP_SET_FPREG (mov rbp, rsp) — MUST immediately follow PUSH_NONVOL(rbp)
    // per Win64 ABI: "the corresponding unwind code must immediately follow
    //  the unwind code that establishes the frame pointer register push"
    buf.EmitByte(static_cast<uint8_t>(set_fpreg_offset));
    buf.EmitByte(static_cast<uint8_t>(UWOP_SET_FPREG));

    // ── Pad to 4-byte boundary ────────────────────────────────────────
    uint32_t code_bytes = code_count * 2;
    uint32_t pad = (4 - (code_bytes % 4)) % 4;
    for (uint32_t i = 0; i < pad; ++i) {
        buf.EmitByte(0);
    }

    // ── Emit personality routine JMP thunk (V2, UNW_FLAG_EHANDLER) ────
    // The ExceptionHandler RVA in UNWIND_INFO is relative to BaseAddress
    // (code_start). The personality routine JitPersonalityRoutine is in
    // chaos_codegen.dll which may be >2GB away from the dynamic code buffer,
    // too far for a 32-bit RVA.  Solution: embed a 12-byte absolute JMP thunk
    // in the code buffer and point the RVA to it.
    //
    // Thunk: mov rax, <8-byte addr>; jmp rax
    //   REX.W MOV RAX, imm64 = 48 B8 <8-byte address>
    //   JMP RAX               = FF E0
    if (has_seh) {
        uint64_t personality_addr = reinterpret_cast<uint64_t>(&JitPersonalityRoutine);
        buf.EmitByte(0x48);  // REX.W prefix
        buf.EmitByte(0xB8);  // MOV RAX, imm64
        buf.Emit64(personality_addr);
        buf.EmitByte(0xFF);  // JMP
        buf.EmitByte(0xE0);  // RAX
    }

    return unwind_start;
}

void DebugDumpUnwindInfo(const CodeBuffer& buf, uint32_t unwind_start, uint32_t code_size) noexcept {
    const uint8_t* d = buf.Data();
    if (!d) return;
    uint32_t pos = unwind_start;
    fprintf(stderr, "[unwind_dbg] UNWIND_INFO at offset=%u code_size=%u\n", unwind_start, code_size);
    fprintf(stderr, "[unwind_dbg]   header[0]=0x%02X (ver_flags)\n", d[pos]);
    fprintf(stderr, "[unwind_dbg]   header[1]=0x%02X (size_of_prolog)\n", d[pos+1]);
    fprintf(stderr, "[unwind_dbg]   header[2]=0x%02X (count_of_codes)\n", d[pos+2]);
    fprintf(stderr, "[unwind_dbg]   header[3]=0x%02X (fp_reg+offset)\n", d[pos+3]);
    uint32_t code_count = d[pos+2];
    for (uint32_t i = 0; i < code_count && i < 20; ++i) {
        uint32_t off = pos + 4 + i*2;
        fprintf(stderr, "[unwind_dbg]   code[%u] offset=%u op=0x%02X\n",
                i, (unsigned)d[off], (unsigned)d[off+1]);
    }
    uint32_t code_bytes = code_count * 2;
    uint32_t pad = (4 - (code_bytes % 4)) % 4;
    uint32_t seh_offset = pos + 4 + code_bytes + pad;
    fprintf(stderr, "[unwind_dbg]   code_bytes=%u pad=%u seh_at=%u\n", code_bytes, pad, seh_offset);
    uint32_t flags = (d[pos] >> 3) & 0x1F;
    if (flags & 0x01) {
        uint32_t handler_rva;
        memcpy(&handler_rva, d + seh_offset, sizeof(handler_rva));
        fprintf(stderr, "[unwind_dbg]   SEH handler RVA=0x%08X\n", handler_rva);
    }
    fflush(stderr);
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

// ── DWARF .eh_frame (Linux) ────────────────────────────────────────────────

#if defined(__linux__)

#if defined(__aarch64__)

// ── ARM64 DWARF .eh_frame ──────────────────────────────────────────────────
//
// ARM64 DWARF register numbering:
//   X0-X30  → DWARF 0-30
//   SP      → DWARF 31
//   LR(X30) → DWARF 30
//   FP(X29) → DWARF 29

uint32_t EmitDwarfCie(CodeBuffer& buf) noexcept {
    uint32_t start = buf.pos();
    uint32_t length_off = buf.pos();
    buf.Emit32(0);              // placeholder length
    buf.Emit32(0);              // cie_id = 0 (CIE marker)
    buf.EmitByte(1);            // version = 1
    buf.EmitByte('z');          // augmentation = "zR\0"
    buf.EmitByte('R');
    buf.EmitByte(0);
    buf.EmitByte(1);            // code_align = ULEB128(1)
    buf.EmitByte(0x78);         // data_align = SLEB128(-8)
    buf.EmitByte(30);           // ret_addr_reg = ULEB128(30) = LR
    buf.EmitByte(1);            // aug_len = ULEB128(1)
    buf.EmitByte(0x1B);         // fde_encoding = DW_EH_PE_pcrel | DW_EH_PE_sdata4

    // Initial: CFA = SP (caller's SP at entry, no pushed return address)
    buf.EmitByte(0x0C);         // DW_CFA_def_cfa
    buf.EmitByte(31);           // register 31 (SP)
    buf.EmitByte(0);            // offset 0

    // Pad to 4-byte boundary
    uint32_t content = buf.pos() - start - 4;
    uint32_t pad = (4 - (content % 4)) % 4;
    for (uint32_t i = 0; i < pad; ++i)
        buf.EmitByte(0);
    buf.Patch32(length_off, buf.pos() - start - 4);
    return start;
}

uint32_t EmitDwarfFde(CodeBuffer& buf, uint32_t cie_offset,
                      uint32_t code_body_size,
                      uint32_t num_push_regs,
                      const uint8_t* push_reg_nums) noexcept {
    uint32_t fde_start = buf.pos();
    uint32_t length_off = buf.pos();
    buf.Emit32(0);              // placeholder length
    uint32_t cie_ptr_val = cie_offset - (fde_start + 4);
    buf.Emit32(cie_ptr_val);

    uint32_t initial_loc_off = buf.pos() - fde_start;
    int32_t pcrel_val = 0 - static_cast<int32_t>(fde_start + initial_loc_off + 4);
    buf.Emit32(static_cast<uint32_t>(pcrel_val));
    buf.Emit32(code_body_size);

    // Post-prologue frame state (after STP X29, X30 + cache reg STPs):
    //   CFA = X29 + 16
    //   X29 at CFA-16  (factored offset 2)
    //   LR  at CFA-8   (factored offset 1)
    //   X19 at CFA-32  (factored offset 4) — first cache slot if any
    //   X20 at CFA-48  (factored offset 6) — etc.
    //
    // General formula for cache reg slot i: CFA - 16*(i+2), factored offset 2*(i+2)

    // DW_CFA_def_cfa(29, 16): CFA = X29 + 16
    buf.EmitByte(0x0C);
    buf.EmitByte(29);           // X29 (DWARF reg 29)
    buf.EmitByte(16);

    // DW_CFA_offset(29, 2): X29 at CFA-16
    buf.EmitByte(0x80 | 29);
    buf.EmitByte(2);

    // DW_CFA_offset(30, 1): LR at CFA-8
    buf.EmitByte(0x80 | 30);
    buf.EmitByte(1);

    // For each callee-saved GPR in prologue order:
    //   push_reg_nums[0] = X29 (already handled above)
    //   push_reg_nums[1..] = callee-saved regs (X19-X28)
    //   Each at CFA - 16*(slot + 2), factored offset = 2*(slot + 2)
    for (uint32_t i = 1; i < num_push_regs; ++i) {
        uint8_t dwarf_reg = push_reg_nums[i];  // ARM64: reg# == DWARF#
        uint8_t factored = static_cast<uint8_t>(2 * (i + 1));  // slot i → CFA - 16*(i+1) → offset 2*(i+1)
        buf.EmitByte(static_cast<uint8_t>(0x80 | dwarf_reg));
        buf.EmitByte(factored);
    }

    // Pad to 4-byte boundary
    uint32_t content = buf.pos() - fde_start - 4;
    uint32_t pad = (4 - (content % 4)) % 4;
    for (uint32_t i = 0; i < pad; ++i)
        buf.EmitByte(0);
    buf.Patch32(length_off, buf.pos() - fde_start - 4);
    return fde_start;
}

#else  // !__aarch64__ (x64)

// x64 register number → DWARF register number mapping.
// First 8 x64 regs have different DWARF numbers; R8-R15 map directly.
static constexpr uint8_t kX64ToDwarfReg[16] = {
    0,  // x64 RAX(0) → DWARF 0
    2,  // x64 RCX(1) → DWARF 2
    1,  // x64 RDX(2) → DWARF 1
    3,  // x64 RBX(3) → DWARF 3
    7,  // x64 RSP(4) → DWARF 7
    6,  // x64 RBP(5) → DWARF 6
    4,  // x64 RSI(6) → DWARF 4
    5,  // x64 RDI(7) → DWARF 5
    8, 9, 10, 11, 12, 13, 14, 15  // x64 R8-R15 → DWARF 8-15
};

uint32_t EmitDwarfCie(CodeBuffer& buf) noexcept {
    uint32_t start = buf.pos();
    uint32_t length_off = buf.pos();
    buf.Emit32(0);           // placeholder: total length (excluding this field)

    buf.Emit32(0);           // cie_id = 0 (CIE marker)
    buf.EmitByte(1);            // version = 1
    buf.EmitByte('z');          // augmentation = "zR\0"
    buf.EmitByte('R');
    buf.EmitByte(0);
    buf.EmitByte(1);            // code_align = ULEB128(1)
    buf.EmitByte(0x78);         // data_align = SLEB128(-8)
    buf.EmitByte(16);           // ret_addr_reg = ULEB128(16)
    buf.EmitByte(1);            // aug_len = ULEB128(1)
    buf.EmitByte(0x1B);         // fde_encoding = DW_EH_PE_pcrel | DW_EH_PE_sdata4

    // Initial instructions: CFA = RSP + 8 (after CALL, ret addr on stack)
    buf.EmitByte(0x0C);         // DW_CFA_def_cfa
    buf.EmitByte(7);            // ULEB128: register 7 (RSP in DWARF)
    buf.EmitByte(8);            // ULEB128: offset 8

    // DW_CFA_offset(16, 1): return address at CFA-8
    buf.EmitByte(0x90);         // opcode = 0x80 | 16
    buf.EmitByte(1);            // ULEB128: CFA + 1 * (-8) = CFA - 8

    // Pad to 4-byte boundary
    uint32_t content = buf.pos() - start - 4;
    uint32_t pad = (4 - (content % 4)) % 4;
    for (uint32_t i = 0; i < pad; ++i)
        buf.EmitByte(0);

    // Patch length
    buf.Patch32(length_off, buf.pos() - start - 4);

    return start;
}

uint32_t EmitDwarfFde(CodeBuffer& buf, uint32_t cie_offset,
                      uint32_t code_body_size,
                      uint32_t num_push_regs,
                      const uint8_t* push_reg_nums) noexcept {
    uint32_t fde_start = buf.pos();
    uint32_t length_off = buf.pos();
    buf.Emit32(0);           // placeholder length

    // CIE pointer: relative offset from this field back to CIE start.
    // cie_ptr = CIE_addr - (FDE_addr + 4)
    uint32_t cie_ptr_val = cie_offset - (fde_start + 4);
    buf.Emit32(cie_ptr_val);

    // initial_loc: pcrel|sdata4. Stored value = -(initial_loc_field_offset + 4).
    // code starts at buffer offset 0, cancellation with runtime base is implicit
    // since code and eh_frame are in the same allocation.
    uint32_t initial_loc_off = buf.pos() - fde_start;
    int32_t pcrel_val = 0 - static_cast<int32_t>(fde_start + initial_loc_off + 4);
    buf.Emit32(static_cast<uint32_t>(pcrel_val));

    // address_range: code body size (sdata4, absolute)
    buf.Emit32(code_body_size);

    // Post-prologue frame state:
    //   CFA = RBP + 16
    //   Return address at CFA-8
    //   Each pushed register at CFA-(16+8*i)
    //
    // Applied from function start (no DW_CFA_advance_loc), so unwinding during
    // the prologue itself is incorrect, but all T4 execution is post-prologue.

    // DW_CFA_def_cfa(6, 16): CFA = RBP + 16
    buf.EmitByte(0x0C);         // DW_CFA_def_cfa
    buf.EmitByte(kX64ToDwarfReg[5]);  // RBP → DWARF 6
    buf.EmitByte(16);

    // DW_CFA_offset(16, 1): return address at CFA-8
    buf.EmitByte(0x90);
    buf.EmitByte(1);

    // For each pushed register in prologue order:
    //   push_reg_nums[0]=RBP at CFA-16 → factored offset 2
    //   push_reg_nums[1]=RBX at CFA-24 → factored offset 3
    //   push_reg_nums[2]=RSI at CFA-32 → factored offset 4
    //   push_reg_nums[i]=cached[i-3] at CFA-(16+8*(i+1)) → factored offset 2+i
    for (uint32_t i = 0; i < num_push_regs; ++i) {
        uint8_t dwarf_reg = kX64ToDwarfReg[push_reg_nums[i] & 0x0F];
        uint8_t factored = static_cast<uint8_t>(2 + i);
        buf.EmitByte(static_cast<uint8_t>(0x80 | dwarf_reg));
        buf.EmitByte(factored);
    }

    // Pad to 4-byte boundary
    uint32_t content = buf.pos() - fde_start - 4;
    uint32_t pad = (4 - (content % 4)) % 4;
    for (uint32_t i = 0; i < pad; ++i)
        buf.EmitByte(0);

    // Patch length
    buf.Patch32(length_off, buf.pos() - fde_start - 4);

    return fde_start;
}

#endif  // !__aarch64__ (closes #else block)

#endif  // __linux__

}  // namespace chaos::il2cpp::jit
