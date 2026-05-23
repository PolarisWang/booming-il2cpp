// ── Codegen unwind info unit tests ──────────────────────────────────────────
//
// Verifies the byte-level output of EmitUnwindInfo (Win64), EmitDwarfCie,
// and EmitDwarfFde (Linux x64) by emitting into a CodeBuffer and checking
// the resulting bytes against expected layouts.
//
// This is the only test file that validates unwind info emission correctness
// at the byte level — no other test in the codebase does this.

#include <gtest/gtest.h>

#include <cstdint>
#include <cstring>

#include "code_buffer.h"
#include "unwind_info.h"

using chaos::il2cpp::codegen::CodeBuffer;
using chaos::il2cpp::codegen::EmitUnwindInfo;
using chaos::il2cpp::codegen::AllocRuntimeFunction;
using chaos::il2cpp::codegen::RuntimeFunction;
using chaos::il2cpp::codegen::EmitDwarfCie;
using chaos::il2cpp::codegen::EmitDwarfFde;

namespace {

// ═══════════════════════════════════════════════════════════════════════════
// Win64 .xdata / .pdata tests
// ═══════════════════════════════════════════════════════════════════════════

#if defined(_WIN64)

TEST(CodegenUnwind, EmitUnwindInfoSmallAlloc) {
    // Emit UNWIND_INFO for a small frame (sub rsp <= 128KB = ALLOC_SMALL path).
    //
    // Prologue:
    //   1. push rbp        (offset 0)
    //   2. mov rbp, rsp    (offset 7)
    //   3. push rbx        (offset 14)
    //   4. sub rsp, 0x40   (offset 21)
    //
    // Expected UNWIND_INFO header:
    //   version_flags = 0x01 (Version=1, Flags=0)
    //   size_of_prolog = ceil((21+15)/16)=3? Actually 21 bytes → ceil(21/16)=2
    //   count_of_codes = 4 (UWOP_PUSH_NONVOL*2 + UWOP_SET_FPREG + UWOP_ALLOC_SMALL)
    //   frame_reg_offset = 0x05 (FrameRegister=RBP=5, FrameOffset=0)
    //
    // UNWIND_CODE entries (reverse prologue order):
    //   1. offset=21, UWOP_ALLOC_SMALL(2), op_info=(0x40/8-1)=7
    //   2. offset=14, UWOP_PUSH_NONVOL(0), reg=RBX(3)
    //   3. offset=7,  UWOP_SET_FPREG(3), op_info=0
    //   4. offset=0,  UWOP_PUSH_NONVOL(0), reg=RBP(5)

    uint8_t reg_nums[]   = {5, 3};  // RBP, RBX
    uint32_t reg_offsets[] = {0, 14};  // offsets of push instructions
    uint32_t sub_rsp_offset = 21;
    uint32_t set_fpreg_offset = 7;

    CodeBuffer buf;
    uint32_t unwind_start = EmitUnwindInfo(
        buf, /*prologue_size=*/24, /*frame_sub_size=*/0x40,
        /*num_push_regs=*/2, reg_nums, reg_offsets,
        sub_rsp_offset, set_fpreg_offset,
        /*has_seh=*/false);

    // ── Verify header ──────────────────────────────────────────────────
    // Offset 0: version_flags (Version=1, Flags=0)
    EXPECT_EQ(buf.Peek(unwind_start + 0), 0x01);
    // Offset 1: size_of_prolog (24/16=1.5, rounded up to 2)
    EXPECT_EQ(buf.Peek(unwind_start + 1), 2);
    // Offset 2: count_of_codes = 4
    EXPECT_EQ(buf.Peek(unwind_start + 2), 4);
    // Offset 3: frame_reg (5=RBP, 0=offset) — stored as raw register number
    EXPECT_EQ(buf.Peek(unwind_start + 3), 5);

    // ── Verify UNWIND_CODE entries (4 entries × 2 bytes = 8 bytes) ─────

    // Entry 1: ALLOC_SMALL at offset 21, op_info = (0x40/8 - 1) = 7
    EXPECT_EQ(buf.Peek(unwind_start + 4), 21);      // code_offset
    EXPECT_EQ(buf.Peek(unwind_start + 5), 0x27);    // (UWOP_ALLOC_SMALL=2)<<4 | 7

    // Entry 2: PUSH_NONVOL RBX at offset 14
    EXPECT_EQ(buf.Peek(unwind_start + 6), 14);      // code_offset
    EXPECT_EQ(buf.Peek(unwind_start + 7), 0x03);    // (UWOP_PUSH_NONVOL=0)<<4 | 3

    // Entry 3: SET_FPREG at offset 7
    EXPECT_EQ(buf.Peek(unwind_start + 8), 7);       // code_offset
    EXPECT_EQ(buf.Peek(unwind_start + 9), 0x30);    // (UWOP_SET_FPREG=3)<<4 | 0

    // Entry 4: PUSH_NONVOL RBP at offset 0
    EXPECT_EQ(buf.Peek(unwind_start + 10), 0);      // code_offset
    EXPECT_EQ(buf.Peek(unwind_start + 11), 0x05);   // (UWOP_PUSH_NONVOL=0)<<4 | 5

    // ── Verify padding to 4-byte boundary ─────────────────────────────
    // 4 codes × 2 bytes = 8 bytes, already aligned. No padding needed.
    // With no SEH flag, next is... actually verify the expected total size.
    // Header(4) + UNWIND_CODE(8) = 12 bytes, already 4-byte aligned.
    uint32_t expected_total = unwind_start + 12;
    EXPECT_EQ(buf.pos(), expected_total);
}

TEST(CodegenUnwind, EmitUnwindInfoSehThunk) {
    // Emit UNWIND_INFO with has_seh=true — should emit a JMP thunk after
    // the unwind codes. The thunk is:
    //   48 B8 <8-byte addr>  FF E0
    // (REX.W MOV RAX, imm64; JMP RAX)

    uint8_t reg_nums[]   = {5};  // just RBP
    uint32_t reg_offsets[] = {0};
    uint32_t sub_rsp_offset = 14;
    uint32_t set_fpreg_offset = 7;

    CodeBuffer buf;
    uint32_t unwind_start = EmitUnwindInfo(
        buf, /*prologue_size=*/16, /*frame_sub_size=*/0x20,
        /*num_push_regs=*/1, reg_nums, reg_offsets,
        sub_rsp_offset, set_fpreg_offset,
        /*has_seh=*/true);

    // ── Verify header — UNW_FLAG_EHANDLER should be set ────────────────
    // version_flags = 0x01 | (0x01 << 3) = 0x09
    EXPECT_EQ(buf.Peek(unwind_start + 0), 0x09);

    // ── Verify JMP thunk exists ──────────────────────────────────────
    // Header(4) + UNWIND_CODE(3 codes × 2 = 6 bytes) + padding(2) = 12 bytes
    // Then the thunk: 48 B8 <8 bytes> FF E0
    uint32_t thunk_start = unwind_start + 12;
    EXPECT_EQ(buf.Peek(thunk_start + 0), 0x48);  // REX.W
    EXPECT_EQ(buf.Peek(thunk_start + 1), 0xB8);  // MOV RAX, imm64
    EXPECT_EQ(buf.Peek(thunk_start + 10), 0xFF); // JMP
    EXPECT_EQ(buf.Peek(thunk_start + 11), 0xE0); // RAX
    // Total: header(4) + codes+pad(8) + thunk(12) = 24
    EXPECT_EQ(buf.pos(), unwind_start + 24);
}

TEST(CodegenUnwind, EmitUnwindInfoAllocLarge) {
    // Emit UNWIND_INFO for a large frame (sub rsp > 128KB = ALLOC_LARGE path).
    // Use 0x20100 (131328 > 128*1024) to ensure ALLOC_LARGE path.
    // alloc_large with OpInfo=0: 2 extra bytes for the scaled frame size.
    uint8_t reg_nums[]   = {5};  // just RBP
    uint32_t reg_offsets[] = {0};
    uint32_t sub_rsp_offset = 7;

    CodeBuffer buf;
    uint32_t unwind_start = EmitUnwindInfo(
        buf, /*prologue_size=*/16, /*frame_sub_size=*/0x20100,
        /*num_push_regs=*/1, reg_nums, reg_offsets,
        sub_rsp_offset, /*set_fpreg_offset=*/0,
        /*has_seh=*/false);

    // ── Verify header ──────────────────────────────────────────────────
    EXPECT_EQ(buf.Peek(unwind_start + 0), 0x01);  // Version=1, no flags
    // num_push_regs=1 + 1(SET_FPREG) + 2(ALLOC_LARGE) = 4 codes
    EXPECT_EQ(buf.Peek(unwind_start + 2), 4);

    // Entry 1: ALLOC_LARGE at offset 7, OpInfo=0
    //   ALLOC_LARGE with OpInfo=0: 2 more bytes for scaled size (0x20100/8 = 0x4020)
    EXPECT_EQ(buf.Peek(unwind_start + 4), 7);      // code_offset
    EXPECT_EQ(buf.Peek(unwind_start + 5), 0x10);   // (UWOP_ALLOC_LARGE=1)<<4 | 0
    EXPECT_EQ(buf.Peek(unwind_start + 6), 0x20);   // scaled & 0xFF = 0x20
    EXPECT_EQ(buf.Peek(unwind_start + 7), 0x40);   // (scaled >> 8) & 0xFF = 0x40

    // Entry 2: SET_FPREG at offset 0
    EXPECT_EQ(buf.Peek(unwind_start + 8), 0);      // code_offset
    EXPECT_EQ(buf.Peek(unwind_start + 9), 0x30);   // (UWOP_SET_FPREG=3)<<4 | 0

    // Entry 3: PUSH_NONVOL RBP at offset 0
    EXPECT_EQ(buf.Peek(unwind_start + 10), 0);      // code_offset
    EXPECT_EQ(buf.Peek(unwind_start + 11), 0x05);   // (UWOP_PUSH_NONVOL=0)<<4 | 5

    // Total: header(4) + codes(4×2+2 extra=10) padded to 4 bytes = 12
    // buf.pos() is 12, not 16, because the ALLOC_LARGE extra 2 bytes are
    // part of the code entries counted by code_count (code_count=4 → 8 bytes).
    // Header(4) + 8 code bytes = 12, already 4-byte aligned.
    EXPECT_EQ(buf.pos(), unwind_start + 12);
}

TEST(CodegenUnwind, AllocRuntimeFunctionBasic) {
    RuntimeFunction* rf = AllocRuntimeFunction(/*unwind_info_offset=*/0x100,
                                                /*code_size=*/0x200);
    ASSERT_NE(rf, nullptr);
    EXPECT_EQ(rf->begin_address, 0u);
    EXPECT_EQ(rf->end_address, 0x200u);
    EXPECT_EQ(rf->unwind_info_address, 0x100u);
    std::free(rf);
}

TEST(CodegenUnwind, EmitUnwindInfoCodeCountCap) {
    // Emit with 300 push registers (capped to 255 codes internally).
    uint8_t reg_nums[300] = {};
    uint32_t reg_offsets[300] = {};
    for (uint32_t i = 0; i < 300; ++i) {
        reg_nums[i] = static_cast<uint8_t>(i % 16);
        reg_offsets[i] = i * 8;
    }

    CodeBuffer buf;
    uint32_t unwind_start = EmitUnwindInfo(
        buf, /*prologue_size=*/300*8+10, /*frame_sub_size=*/0x40,
        300, reg_nums, reg_offsets, 300*8, /*set_fpreg_offset=*/7,
        /*has_seh=*/false);

    // count_of_codes should be capped at 255 (254 push + 1 alloc_small).
    EXPECT_EQ(buf.Peek(unwind_start + 2), 255);
}

#endif  // _WIN64

// ═══════════════════════════════════════════════════════════════════════════
// DWARF .eh_frame tests (Linux x64)
// ═══════════════════════════════════════════════════════════════════════════

#if defined(__linux__)

TEST(CodegenUnwind, EmitDwarfCieBasic) {
    CodeBuffer buf;
    uint32_t cie_start = EmitDwarfCie(buf);

    // CIE header:
    //   length (4 bytes, placeholder → patched)
    //   cie_id = 0 (4 bytes)
    //   version = 1 (1 byte)
    //   augmentation = "zR\0" (3 bytes)
    //   code_align = 1 (ULEB128 1 byte)
    //   data_align = -8 (SLEB128 1 byte: 0x78)
    //   ret_addr_reg = 16 (ULEB128 1 byte: 0x10)
    //   aug_len = 1 (ULEB128 1 byte)
    //   fde_encoding = 0x1B (1 byte)
    // Initial instructions:
    //   DW_CFA_def_cfa(7, 8) → 0x0C 07 08
    //   DW_CFA_offset(16, 1) → 0x90 01
    uint32_t hdr = cie_start;

    // cie_id
    EXPECT_EQ(buf.Peek(hdr + 4), 0);
    EXPECT_EQ(buf.Peek(hdr + 5), 0);
    EXPECT_EQ(buf.Peek(hdr + 6), 0);
    EXPECT_EQ(buf.Peek(hdr + 7), 0);
    // version
    EXPECT_EQ(buf.Peek(hdr + 8), 1);
    // augmentation "zR\0"
    EXPECT_EQ(buf.Peek(hdr + 9), 'z');
    EXPECT_EQ(buf.Peek(hdr + 10), 'R');
    EXPECT_EQ(buf.Peek(hdr + 11), 0);
    // code_align
    EXPECT_EQ(buf.Peek(hdr + 12), 1);
    // data_align
    EXPECT_EQ(buf.Peek(hdr + 13), 0x78);
    // ret_addr_reg
    EXPECT_EQ(buf.Peek(hdr + 14), 16);
    // aug_len
    EXPECT_EQ(buf.Peek(hdr + 15), 1);
    // fde_encoding
    EXPECT_EQ(buf.Peek(hdr + 16), 0x1B);
    // Initial: DW_CFA_def_cfa
    EXPECT_EQ(buf.Peek(hdr + 17), 0x0C);
    EXPECT_EQ(buf.Peek(hdr + 18), 7);   // register 7 (RSP)
    EXPECT_EQ(buf.Peek(hdr + 19), 8);   // offset 8
    // DW_CFA_offset(16, 1)
    EXPECT_EQ(buf.Peek(hdr + 20), 0x90 | 16);
    EXPECT_EQ(buf.Peek(hdr + 21), 1);

    // Verify the length field was patched.
    uint32_t content_len =
        static_cast<uint32_t>(buf.Peek(hdr + 0)) |
        (static_cast<uint32_t>(buf.Peek(hdr + 1)) << 8) |
        (static_cast<uint32_t>(buf.Peek(hdr + 2)) << 16) |
        (static_cast<uint32_t>(buf.Peek(hdr + 3)) << 24);
    EXPECT_EQ(content_len, buf.pos() - hdr - 4);
}

TEST(CodegenUnwind, EmitDwarfFdeBasic) {
    // Emit a CIE first (needed for FDE reference), then an FDE.
    CodeBuffer buf;
    uint32_t cie_start = EmitDwarfCie(buf);

    uint8_t reg_nums[] = {5, 3};  // RBP, RBX
    uint32_t fde_start = EmitDwarfFde(buf, cie_start,
                                       /*code_body_size=*/128,
                                       /*num_push_regs=*/2,
                                       reg_nums);

    // FDE header:
    //   length (4 bytes)
    //   cie_pointer (4 bytes) = cie_start - (fde_start + 4)
    //   initial_loc (4 bytes, pcrel sdata4)
    //   address_range (4 bytes)
    // Then CIE instructions...
    uint32_t hdr = fde_start;

    // CIE pointer
    int32_t expected_cie_ptr = static_cast<int32_t>(cie_start) -
                               static_cast<int32_t>(fde_start + 4);
    int32_t cie_ptr_val =
        static_cast<int32_t>(buf.Peek(hdr + 4)) |
        (static_cast<int32_t>(buf.Peek(hdr + 5)) << 8) |
        (static_cast<int32_t>(buf.Peek(hdr + 6)) << 16) |
        (static_cast<int32_t>(buf.Peek(hdr + 7)) << 24);
    EXPECT_EQ(cie_ptr_val, expected_cie_ptr);

    // Address range
    uint32_t addr_range =
        static_cast<uint32_t>(buf.Peek(hdr + 12)) |
        (static_cast<uint32_t>(buf.Peek(hdr + 13)) << 8) |
        (static_cast<uint32_t>(buf.Peek(hdr + 14)) << 16) |
        (static_cast<uint32_t>(buf.Peek(hdr + 15)) << 24);
    EXPECT_EQ(addr_range, 128u);

    // Verify length was patched.
    uint32_t content_len =
        static_cast<uint32_t>(buf.Peek(hdr + 0)) |
        (static_cast<uint32_t>(buf.Peek(hdr + 1)) << 8) |
        (static_cast<uint32_t>(buf.Peek(hdr + 2)) << 16) |
        (static_cast<uint32_t>(buf.Peek(hdr + 3)) << 24);
    EXPECT_EQ(content_len, buf.pos() - fde_start - 4);
}

#endif  // __linux__

}  // namespace
