#ifndef CHAOS_IL2CPP_ARM64_ENCODER_H_
#define CHAOS_IL2CPP_ARM64_ENCODER_H_

// ── ARM64 instruction encoding helpers ────────────────────────────────────
//
// Free-function API for emitting ARM64 (AArch64) instruction sequences into
// a CodeBuffer.  All instructions are exactly 4 bytes (fixed length).
//
// ARM64 register mapping (our convention):
//   X0..X28   — 64-bit general-purpose registers
//   X29 (FP)  — frame pointer
//   X30 (LR)  — link register
//   SP        — stack pointer (separate encoding, not X31)
//   V0..V31   — 128-bit SIMD/FP registers
//   D0..D31   — 64-bit scalar FP (lower 64 bits of V registers)
//   S0..S31   — 32-bit scalar FP (lower 32 bits of V registers)
//
// Our virtual register numbering in the encoder matches:
//   GPR: 0..30 (X0..X30), 31 = SP
//   FPR: 0..31 (V0..V31)

#include <cstdint>
#include <cstddef>

#include "code_buffer.h"

namespace chaos::il2cpp::jit {

// ── ARM64 register aliases ───────────────────────────────────────────────
static constexpr uint8_t kARM64_SP  = 31;  // SP (stack pointer)
static constexpr uint8_t kARM64_LR  = 30;  // X30 (link register)
static constexpr uint8_t kARM64_FP  = 29;  // X29 (frame pointer)

// ── ARM64 condition codes ────────────────────────────────────────────────
// These map to the 4-bit cond field in B.cond / CSEL / CSET / etc.
// Bits: [3:1] = condition, [0] = invert
static constexpr uint8_t kARM64_EQ = 0;   // Equal (Z set)
static constexpr uint8_t kARM64_NE = 1;   // Not equal (Z clear)
static constexpr uint8_t kARM64_CS = 2;   // Carry set / unsigned >= (C set)
static constexpr uint8_t kARM64_CC = 3;   // Carry clear / unsigned < (C clear)
static constexpr uint8_t kARM64_MI = 4;   // Minus / negative (N set)
static constexpr uint8_t kARM64_PL = 5;   // Plus / non-negative (N clear)
static constexpr uint8_t kARM64_VS = 6;   // Overflow (V set)
static constexpr uint8_t kARM64_VC = 7;   // No overflow (V clear)
static constexpr uint8_t kARM64_HI = 8;   // Unsigned > (C set, Z clear)
static constexpr uint8_t kARM64_LS = 9;   // Unsigned <= (C clear, Z set)
static constexpr uint8_t kARM64_GE = 10;  // Signed >= (N == V)
static constexpr uint8_t kARM64_LT = 11;  // Signed < (N != V)
static constexpr uint8_t kARM64_GT = 12;  // Signed > (Z clear, N == V)
static constexpr uint8_t kARM64_LE = 13;  // Signed <= (Z set, N != V)
static constexpr uint8_t kARM64_AL = 14;  // Always (unconditional)

// ── ARM64 instruction encoding ───────────────────────────────────────────

/// Emit a raw 32-bit instruction word.
inline void EmitArm64(CodeBuffer& buf, uint32_t instr) noexcept {
    buf.Emit32(instr);
}

// ═══════════════════════════════════════════════════════════════════════════
// Data processing — Register (shifted register)
// ═══════════════════════════════════════════════════════════════════════════

/// 32-bit ADD Rd, Rn, Rm {shift #0}
inline void EmitAdd32(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x0B000000u | (rm << 16) | (rn << 5) | rd);
}

/// 64-bit ADD Rd, Rn, Rm
inline void EmitAdd64(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x8B000000u | (rm << 16) | (rn << 5) | rd);
}

/// 32-bit SUB Rd, Rn, Rm
inline void EmitSub32(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4B000000u | (rm << 16) | (rn << 5) | rd);
}

/// 64-bit SUB Rd, Rn, Rm
inline void EmitSub64(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0xCB000000u | (rm << 16) | (rn << 5) | rd);
}

/// 32-bit AND Rd, Rn, Rm
inline void EmitAnd32(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x0A000000u | (rm << 16) | (rn << 5) | rd);
}

/// 64-bit AND Rd, Rn, Rm
inline void EmitAnd64(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x8A000000u | (rm << 16) | (rn << 5) | rd);
}

/// 32-bit ORR Rd, Rn, Rm
inline void EmitOrr32(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x2A000000u | (rm << 16) | (rn << 5) | rd);
}

/// 64-bit ORR Rd, Rn, Rm
inline void EmitOrr64(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0xAA000000u | (rm << 16) | (rn << 5) | rd);
}

/// 32-bit EOR Rd, Rn, Rm
inline void EmitEor32(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4A000000u | (rm << 16) | (rn << 5) | rd);
}

/// 64-bit EOR Rd, Rn, Rm
inline void EmitEor64(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0xCA000000u | (rm << 16) | (rn << 5) | rd);
}

/// 64-bit MUL Rd, Rn, Rm
inline void EmitMul64(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x9B007C00u | (rm << 16) | (rn << 5) | rd);
}

/// 32-bit MUL Wd, Wn, Wm
inline void EmitMul32(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x1B007C00u | (rm << 16) | (rn << 5) | rd);
}

/// 64-bit SDIV Rd, Rn, Rm
inline void EmitSdiv64(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x9AC00C00u | (rm << 16) | (rn << 5) | rd);
}

/// 32-bit SDIV Wd, Wn, Wm
inline void EmitSdiv32(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x1AC00C00u | (rm << 16) | (rn << 5) | rd);
}

/// 64-bit UDIV Rd, Rn, Rm
inline void EmitUdiv64(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x9AC00800u | (rm << 16) | (rn << 5) | rd);
}

/// 32-bit UDIV Wd, Wn, Wm
inline void EmitUdiv32(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x1AC00800u | (rm << 16) | (rn << 5) | rd);
}

/// 64-bit NEG Rd, Rm (alias for SUB Rd, XZR, Rm)
inline void EmitNeg64(CodeBuffer& buf, uint8_t rd, uint8_t rm) noexcept {
    EmitSub64(buf, rd, 31, rm);
}

/// 32-bit NEG Wd, Wm
inline void EmitNeg32(CodeBuffer& buf, uint8_t rd, uint8_t rm) noexcept {
    EmitSub32(buf, rd, 31, rm);
}

/// 64-bit MVN Rd, Rm (alias for ORN Rd, XZR, Rm = bitwise NOT)
inline void EmitMvn64(CodeBuffer& buf, uint8_t rd, uint8_t rm) noexcept {
    // ORN Xd, XZR, Xm = ~Xm
    EmitArm64(buf, 0xAA2003E0u | (rm << 16) | rd);
}

/// 32-bit MVN Wd, Wm
inline void EmitMvn32(CodeBuffer& buf, uint8_t rd, uint8_t rm) noexcept {
    EmitArm64(buf, 0x2A2003E0u | (rm << 16) | rd);
}

// ═══════════════════════════════════════════════════════════════════════════
// Data processing — Immediate
// ═══════════════════════════════════════════════════════════════════════════

/// 64-bit ADD Rd, Rn, #imm12 (shift=0)
inline void EmitAdd64Imm(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint16_t imm12) noexcept {
    EmitArm64(buf, 0x91000000u | (static_cast<uint32_t>(imm12) << 10) | (rn << 5) | rd);
}

/// 32-bit ADD Wd, Wn, #imm12
inline void EmitAdd32Imm(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint16_t imm12) noexcept {
    EmitArm64(buf, 0x11000000u | (static_cast<uint32_t>(imm12) << 10) | (rn << 5) | rd);
}

/// 64-bit SUB Rd, Rn, #imm12
inline void EmitSub64Imm(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint16_t imm12) noexcept {
    EmitArm64(buf, 0xD1000000u | (static_cast<uint32_t>(imm12) << 10) | (rn << 5) | rd);
}

/// 32-bit SUB Wd, Wn, #imm12
inline void EmitSub32Imm(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint16_t imm12) noexcept {
    EmitArm64(buf, 0x51000000u | (static_cast<uint32_t>(imm12) << 10) | (rn << 5) | rd);
}

/// 64-bit CMP Rn, #imm12 (alias for SUBS XZR, Rn, #imm12)
inline void EmitCmp64Imm(CodeBuffer& buf, uint8_t rn, uint16_t imm12) noexcept {
    EmitArm64(buf, 0xF1000000u | (static_cast<uint32_t>(imm12) << 10) | (rn << 5));
}

/// 32-bit CMP Wn, #imm12
inline void EmitCmp32Imm(CodeBuffer& buf, uint8_t rn, uint16_t imm12) noexcept {
    EmitArm64(buf, 0x71000000u | (static_cast<uint32_t>(imm12) << 10) | (rn << 5));
}

// ═══════════════════════════════════════════════════════════════════════════
// Move (immediate, wide)
// ═══════════════════════════════════════════════════════════════════════════

/// 64-bit MOVZ Rd, #imm16, lsl #shift (shift = 0, 16, 32, 48)
inline void EmitMovz64(CodeBuffer& buf, uint8_t rd, uint16_t imm16, uint8_t shift = 0) noexcept {
    uint32_t hw = (static_cast<uint32_t>(shift) / 16) & 3;
    EmitArm64(buf, 0xD2800000u | (hw << 21) | (static_cast<uint32_t>(imm16) << 5) | rd);
}

/// 32-bit MOVZ Wd, #imm16, lsl #shift (shift = 0, 16)
inline void EmitMovz32(CodeBuffer& buf, uint8_t rd, uint16_t imm16, uint8_t shift = 0) noexcept {
    uint32_t hw = (static_cast<uint32_t>(shift) / 16) & 1;
    EmitArm64(buf, 0x52800000u | (hw << 21) | (static_cast<uint32_t>(imm16) << 5) | rd);
}

/// 64-bit MOVK Rd, #imm16, lsl #shift (shift = 0, 16, 32, 48)
inline void EmitMovk64(CodeBuffer& buf, uint8_t rd, uint16_t imm16, uint8_t shift = 0) noexcept {
    uint32_t hw = (static_cast<uint32_t>(shift) / 16) & 3;
    EmitArm64(buf, 0xF2800000u | (hw << 21) | (static_cast<uint32_t>(imm16) << 5) | rd);
}

/// 64-bit MOVN Rd, #imm16, lsl #shift (shift = 0, 16, 32, 48)
inline void EmitMovn64(CodeBuffer& buf, uint8_t rd, uint16_t imm16, uint8_t shift = 0) noexcept {
    uint32_t hw = (static_cast<uint32_t>(shift) / 16) & 3;
    EmitArm64(buf, 0x92800000u | (hw << 21) | (static_cast<uint32_t>(imm16) << 5) | rd);
}

/// Load 64-bit immediate into a register using MOVZ + MOVK sequence (up to 4 instructions).
inline void EmitLoadImm64(CodeBuffer& buf, uint8_t rd, uint64_t imm) noexcept {
    uint16_t chunks[4] = {
        static_cast<uint16_t>(imm & 0xFFFF),
        static_cast<uint16_t>((imm >> 16) & 0xFFFF),
        static_cast<uint16_t>((imm >> 32) & 0xFFFF),
        static_cast<uint16_t>((imm >> 48) & 0xFFFF)
    };
    int first = 3;
    while (first >= 0 && chunks[first] == 0) --first;
    if (first < 0) {
        EmitMovz64(buf, rd, 0, 0);
        return;
    }
    EmitMovz64(buf, rd, chunks[first], static_cast<uint8_t>(first * 16));
    for (int i = first - 1; i >= 0; --i) {
        if (chunks[i] != 0) {
            EmitMovk64(buf, rd, chunks[i], static_cast<uint8_t>(i * 16));
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// Load / Store (register offset)
// ═══════════════════════════════════════════════════════════════════════════

/// 64-bit LDR Xt, [Xn, Rm, LSL #0]
inline void EmitLdr64Reg(CodeBuffer& buf, uint8_t rt, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0xF8600800u | (rm << 16) | (rn << 5) | rt);
}

/// 32-bit LDR Wt, [Xn, Rm, LSL #0]
inline void EmitLdr32Reg(CodeBuffer& buf, uint8_t rt, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0xB8600800u | (rm << 16) | (rn << 5) | rt);
}

/// 64-bit STR Xt, [Xn, Rm, LSL #0]
inline void EmitStr64Reg(CodeBuffer& buf, uint8_t rt, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0xF8200800u | (rm << 16) | (rn << 5) | rt);
}

/// 32-bit STR Wt, [Xn, Rm, LSL #0]
inline void EmitStr32Reg(CodeBuffer& buf, uint8_t rt, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0xB8200800u | (rm << 16) | (rn << 5) | rt);
}

// ═══════════════════════════════════════════════════════════════════════════
// Load / Store (unsigned immediate — 12-bit scaled offset)
// ═══════════════════════════════════════════════════════════════════════════

/// 64-bit LDR Xt, [Xn, #imm12 * 8]
inline void EmitLdr64(CodeBuffer& buf, uint8_t rt, uint8_t rn, uint16_t imm12) noexcept {
    EmitArm64(buf, 0xF9400000u | (static_cast<uint32_t>(imm12 & 0xFFF) << 10) | (rn << 5) | rt);
}

/// 32-bit LDR Wt, [Xn, #imm12 * 4]
inline void EmitLdr32(CodeBuffer& buf, uint8_t rt, uint8_t rn, uint16_t imm12) noexcept {
    EmitArm64(buf, 0xB9400000u | (static_cast<uint32_t>(imm12 & 0xFFF) << 10) | (rn << 5) | rt);
}

/// 64-bit STR Xt, [Xn, #imm12 * 8]
inline void EmitStr64(CodeBuffer& buf, uint8_t rt, uint8_t rn, uint16_t imm12) noexcept {
    EmitArm64(buf, 0xF9000000u | (static_cast<uint32_t>(imm12 & 0xFFF) << 10) | (rn << 5) | rt);
}

/// 32-bit STR Wt, [Xn, #imm12 * 4]
inline void EmitStr32(CodeBuffer& buf, uint8_t rt, uint8_t rn, uint16_t imm12) noexcept {
    EmitArm64(buf, 0xB9000000u | (static_cast<uint32_t>(imm12 & 0xFFF) << 10) | (rn << 5) | rt);
}

// ═══════════════════════════════════════════════════════════════════════════
// Load / Store (9-bit signed offset — unscaled)
// ═══════════════════════════════════════════════════════════════════════════

/// 64-bit LDUR Xt, [Xn, #simm9]
inline void EmitLdur64(CodeBuffer& buf, uint8_t rt, uint8_t rn, int16_t simm9) noexcept {
    uint32_t imm9 = static_cast<uint32_t>(simm9) & 0x1FF;
    EmitArm64(buf, 0xF8400000u | (imm9 << 12) | (rn << 5) | rt);
}

/// 32-bit LDUR Wt, [Xn, #simm9]
inline void EmitLdur32(CodeBuffer& buf, uint8_t rt, uint8_t rn, int16_t simm9) noexcept {
    uint32_t imm9 = static_cast<uint32_t>(simm9) & 0x1FF;
    EmitArm64(buf, 0xB8400000u | (imm9 << 12) | (rn << 5) | rt);
}

/// 64-bit STUR Xt, [Xn, #simm9]
inline void EmitStur64(CodeBuffer& buf, uint8_t rt, uint8_t rn, int16_t simm9) noexcept {
    uint32_t imm9 = static_cast<uint32_t>(simm9) & 0x1FF;
    EmitArm64(buf, 0xF8000000u | (imm9 << 12) | (rn << 5) | rt);
}

/// 32-bit STUR Wt, [Xn, #simm9]
inline void EmitStur32(CodeBuffer& buf, uint8_t rt, uint8_t rn, int16_t simm9) noexcept {
    uint32_t imm9 = static_cast<uint32_t>(simm9) & 0x1FF;
    EmitArm64(buf, 0xB8000000u | (imm9 << 12) | (rn << 5) | rt);
}

// ═══════════════════════════════════════════════════════════════════════════
// Load / Store pair (pre-indexed / post-indexed / signed offset)
// ═══════════════════════════════════════════════════════════════════════════

/// 64-bit STP Xt1, Xt2, [Xn, #simm7 * 8] (signed offset, no index)
inline void EmitStp64(CodeBuffer& buf, uint8_t rt1, uint8_t rt2, uint8_t rn, int32_t simm7) noexcept {
    uint32_t imm7 = (static_cast<uint32_t>(simm7) / 8) & 0x7F;
    EmitArm64(buf, 0xA9000000u | (imm7 << 15) | (rt2 << 10) | (rn << 5) | rt1);
}

/// 64-bit LDP Xt1, Xt2, [Xn, #simm7 * 8] (signed offset, no index)
inline void EmitLdp64(CodeBuffer& buf, uint8_t rt1, uint8_t rt2, uint8_t rn, int32_t simm7) noexcept {
    uint32_t imm7 = (static_cast<uint32_t>(simm7) / 8) & 0x7F;
    EmitArm64(buf, 0xA9400000u | (imm7 << 15) | (rt2 << 10) | (rn << 5) | rt1);
}

/// 64-bit STP Xt1, Xt2, [Xn, #simm7 * 8]! (pre-indexed)
inline void EmitStp64Pre(CodeBuffer& buf, uint8_t rt1, uint8_t rt2, uint8_t rn, int32_t simm7) noexcept {
    uint32_t imm7 = (static_cast<uint32_t>(simm7) / 8) & 0x7F;
    EmitArm64(buf, 0xA9800000u | (imm7 << 15) | (rt2 << 10) | (rn << 5) | rt1);
}

/// 64-bit LDP Xt1, Xt2, [Xn, #simm7 * 8]! (pre-indexed)
inline void EmitLdp64Pre(CodeBuffer& buf, uint8_t rt1, uint8_t rt2, uint8_t rn, int32_t simm7) noexcept {
    uint32_t imm7 = (static_cast<uint32_t>(simm7) / 8) & 0x7F;
    EmitArm64(buf, 0xA9C00000u | (imm7 << 15) | (rt2 << 10) | (rn << 5) | rt1);
}

/// 64-bit STP Xt1, Xt2, [Xn], #simm7 * 8 (post-indexed)
inline void EmitStp64Post(CodeBuffer& buf, uint8_t rt1, uint8_t rt2, uint8_t rn, int32_t simm7) noexcept {
    uint32_t imm7 = (static_cast<uint32_t>(simm7) / 8) & 0x7F;
    EmitArm64(buf, 0xA8800000u | (imm7 << 15) | (rt2 << 10) | (rn << 5) | rt1);
}

/// 64-bit LDP Xt1, Xt2, [Xn], #simm7 * 8 (post-indexed)
inline void EmitLdp64Post(CodeBuffer& buf, uint8_t rt1, uint8_t rt2, uint8_t rn, int32_t simm7) noexcept {
    uint32_t imm7 = (static_cast<uint32_t>(simm7) / 8) & 0x7F;
    EmitArm64(buf, 0xA8C00000u | (imm7 << 15) | (rt2 << 10) | (rn << 5) | rt1);
}

// ═══════════════════════════════════════════════════════════════════════════
// Load address (PC-relative)
// ═══════════════════════════════════════════════════════════════════════════

/// ADR Rd, #offset (PC-relative, ±1 MB)
inline void EmitAdr(CodeBuffer& buf, uint8_t rd, int32_t offset) noexcept {
    uint32_t imm_low = static_cast<uint32_t>(offset) & 3;
    uint32_t imm_high = (static_cast<uint32_t>(offset) >> 2) & 0x7FFFF;
    EmitArm64(buf, 0x10000000u | (imm_high << 5) | (imm_low << 29) | rd);
}

/// ADRP Rd, #page_offset (page-aligned PC-relative, ±4 GB)
inline void EmitAdrp(CodeBuffer& buf, uint8_t rd, int32_t page_offset) noexcept {
    uint32_t imm_low = (static_cast<uint32_t>(page_offset) >> 12) & 3;
    uint32_t imm_high = (static_cast<uint32_t>(page_offset) >> 14) & 0x7FFFF;
    EmitArm64(buf, 0x90000000u | (imm_high << 5) | (imm_low << 29) | rd);
}

// ═══════════════════════════════════════════════════════════════════════════
// Branch
// ═══════════════════════════════════════════════════════════════════════════

/// B #offset (unconditional, ±128 MB, PC-relative)
inline void EmitB(CodeBuffer& buf, int32_t offset) noexcept {
    uint32_t imm26 = (static_cast<uint32_t>(offset) >> 2) & 0x3FFFFFF;
    EmitArm64(buf, 0x14000000u | imm26);
}

/// BL #offset (call, ±128 MB, PC-relative)
inline void EmitBl(CodeBuffer& buf, int32_t offset) noexcept {
    uint32_t imm26 = (static_cast<uint32_t>(offset) >> 2) & 0x3FFFFFF;
    EmitArm64(buf, 0x94000000u | imm26);
}

/// BR Xn (jump to register)
inline void EmitBr(CodeBuffer& buf, uint8_t rn) noexcept {
    EmitArm64(buf, 0xD61F0000u | (rn << 5));
}

/// BLR Xn (call to register)
inline void EmitBlr(CodeBuffer& buf, uint8_t rn) noexcept {
    EmitArm64(buf, 0xD63F0000u | (rn << 5));
}

/// RET Xn (return)
inline void EmitRet(CodeBuffer& buf, uint8_t rn = kARM64_LR) noexcept {
    EmitArm64(buf, 0xD65F0000u | (rn << 5));
}

// ═══════════════════════════════════════════════════════════════════════════
// Conditional branch
// ═══════════════════════════════════════════════════════════════════════════

/// B.cond #offset (conditional branch, ±1 MB, PC-relative)
inline void EmitBCond(CodeBuffer& buf, uint8_t cond, int32_t offset) noexcept {
    uint32_t imm19 = (static_cast<uint32_t>(offset) >> 2) & 0x7FFFF;
    EmitArm64(buf, 0x54000000u | (imm19 << 5) | cond);
}

/// CBZ Xt, #offset (compare and branch if zero, ±1 MB)
inline void EmitCbz64(CodeBuffer& buf, uint8_t rt, int32_t offset) noexcept {
    uint32_t imm19 = (static_cast<uint32_t>(offset) >> 2) & 0x7FFFF;
    EmitArm64(buf, 0xB4000000u | (imm19 << 5) | rt);
}

/// CBNZ Xt, #offset (compare and branch if non-zero, ±1 MB)
inline void EmitCbnz64(CodeBuffer& buf, uint8_t rt, int32_t offset) noexcept {
    uint32_t imm19 = (static_cast<uint32_t>(offset) >> 2) & 0x7FFFF;
    EmitArm64(buf, 0xB5000000u | (imm19 << 5) | rt);
}

/// CBZ Wt, #offset (32-bit, ±1 MB)
inline void EmitCbz32(CodeBuffer& buf, uint8_t rt, int32_t offset) noexcept {
    uint32_t imm19 = (static_cast<uint32_t>(offset) >> 2) & 0x7FFFF;
    EmitArm64(buf, 0x34000000u | (imm19 << 5) | rt);
}

/// CBNZ Wt, #offset (32-bit, ±1 MB)
inline void EmitCbnz32(CodeBuffer& buf, uint8_t rt, int32_t offset) noexcept {
    uint32_t imm19 = (static_cast<uint32_t>(offset) >> 2) & 0x7FFFF;
    EmitArm64(buf, 0x35000000u | (imm19 << 5) | rt);
}

// ═══════════════════════════════════════════════════════════════════════════
// Conditional select / set
// ═══════════════════════════════════════════════════════════════════════════

/// CSEL Xd, Xn, Xm, cond
inline void EmitCsel64(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm, uint8_t cond) noexcept {
    EmitArm64(buf, 0x9A800000u | (rm << 16) | (cond << 12) | (rn << 5) | rd);
}

/// CSEL Wd, Wn, Wm, cond
inline void EmitCsel32(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm, uint8_t cond) noexcept {
    EmitArm64(buf, 0x1A800000u | (rm << 16) | (cond << 12) | (rn << 5) | rd);
}

/// CSET Xd, cond (alias for CSEL Xd, XZR, XZR, inv(cond))
inline void EmitCset64(CodeBuffer& buf, uint8_t rd, uint8_t cond) noexcept {
    EmitCsel64(buf, rd, 31, 31, cond ^ 1);
}

// ═══════════════════════════════════════════════════════════════════════════
// Shift (register)
// ═══════════════════════════════════════════════════════════════════════════

/// 64-bit LSL Xd, Xn, Xm (logical shift left by variable)
inline void EmitLsl64Var(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x9A400000u | (rm << 16) | (rn << 5) | rd);  // LSLV
}

/// 32-bit LSL Wd, Wn, Wm
inline void EmitLsl32Var(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x1A400000u | (rm << 16) | (rn << 5) | rd);  // LSLV
}

/// 64-bit LSR Xd, Xn, Xm (logical shift right by variable)
inline void EmitLsr64Var(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x9A440000u | (rm << 16) | (rn << 5) | rd);  // LSRV
}

/// 32-bit LSR Wd, Wn, Wm
inline void EmitLsr32Var(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x1A440000u | (rm << 16) | (rn << 5) | rd);  // LSRV
}

/// 64-bit ASR Xd, Xn, Xm (arithmetic shift right by variable)
inline void EmitAsr64Var(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x9A480000u | (rm << 16) | (rn << 5) | rd);  // ASRV
}

/// 32-bit ASR Wd, Wn, Wm
inline void EmitAsr32Var(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x1A480000u | (rm << 16) | (rn << 5) | rd);  // ASRV
}

// ═══════════════════════════════════════════════════════════════════════════
// Shift (immediate) — via UBFM/SBFM
// ═══════════════════════════════════════════════════════════════════════════
//
// ARM64 encodes immediate shifts via bitfield instructions:
//   LSL #shift → UBFM Xd, Xn, #((-shift) mod 64), #(63-shift)
//   LSR #shift → UBFM Xd, Xn, #shift, #63
//   ASR #shift → SBFM Xd, Xn, #shift, #63
//
// UBFM base (64-bit): sf=1, N=1, opc=10 → 0xD3400000
// SBFM base (64-bit): sf=1, N=1, opc=00 → 0x93400000
// UBFM base (32-bit): sf=0, N=0, opc=10 → 0x53000000
// SBFM base (32-bit): sf=0, N=0, opc=00 → 0x13000000

/// 64-bit LSL Xd, Xn, #shift (0-63)
inline void EmitLsl64Imm(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t shift) noexcept {
    uint32_t immr = (64u - shift) & 0x3F;
    uint32_t imms = (63u - shift) & 0x3F;
    EmitArm64(buf, 0xD3400000u | (immr << 16) | (imms << 10) | (rn << 5) | rd);
}

/// 64-bit LSR Xd, Xn, #shift (0-63)
inline void EmitLsr64Imm(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t shift) noexcept {
    EmitArm64(buf, 0xD3400000u | (static_cast<uint32_t>(shift) << 16) | (0x3Fu << 10) | (rn << 5) | rd);
}

/// 64-bit ASR Xd, Xn, #shift (0-63)
inline void EmitAsr64Imm(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t shift) noexcept {
    EmitArm64(buf, 0x93400000u | (static_cast<uint32_t>(shift) << 16) | (0x3Fu << 10) | (rn << 5) | rd);
}

// ═══════════════════════════════════════════════════════════════════════════
// Sign extension
// ═══════════════════════════════════════════════════════════════════════════

/// SXTW Xd, Wn (sign-extend 32→64)
inline void EmitSxtw(CodeBuffer& buf, uint8_t rd, uint8_t rn) noexcept {
    // SBFM Xd, Xn, #0, #31
    EmitArm64(buf, 0x93407C00u | (rn << 5) | rd);
}

/// SXTB Xd, Wn (sign-extend 8→64)
inline void EmitSxtb(CodeBuffer& buf, uint8_t rd, uint8_t rn) noexcept {
    // SBFM Xd, Xn, #0, #7
    EmitArm64(buf, 0x93401C00u | (rn << 5) | rd);
}

/// SXTH Xd, Wn (sign-extend 16→64)
inline void EmitSxth(CodeBuffer& buf, uint8_t rd, uint8_t rn) noexcept {
    // SBFM Xd, Xn, #0, #15
    EmitArm64(buf, 0x93403C00u | (rn << 5) | rd);
}

// ═══════════════════════════════════════════════════════════════════════════
// Scalar FP (SIMD & Floating-point)
// ═══════════════════════════════════════════════════════════════════════════
//
// Scalar 3-register FP group encoding:
//   bits [31:24] = 0x1E  (FP group)
//   bit  [23]    = 0     (scalar)
//   bit  [22]    = sz    (0 = S/32-bit, 1 = D/64-bit)
//   bits [21:20] = opc   (FADD=10, FSUB=11, FMUL=00, FDIV=01) -- via bit 20
//   bits [15:10] = fixed for each op (see individual encodings)

/// 64-bit scalar FADD Dd, Dn, Dm
inline void EmitFadd64(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x1E603800u | (rm << 16) | (rn << 5) | rd);
}

/// 32-bit scalar FADD Sd, Sn, Sm
inline void EmitFadd32(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x1E203800u | (rm << 16) | (rn << 5) | rd);
}

/// 64-bit FSUB Dd, Dn, Dm
inline void EmitFsub64(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    // FSUB = FADD with bit 20 set
    EmitArm64(buf, 0x1E703800u | (rm << 16) | (rn << 5) | rd);
}

/// 32-bit FSUB Sd, Sn, Sm
inline void EmitFsub32(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x1E303800u | (rm << 16) | (rn << 5) | rd);
}

/// 64-bit FMUL Dd, Dn, Dm
inline void EmitFmul64(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x1E600800u | (rm << 16) | (rn << 5) | rd);
}

/// 64-bit FDIV Dd, Dn, Dm
inline void EmitFdiv64(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x1E601800u | (rm << 16) | (rn << 5) | rd);
}

/// FCVT Dd, Sn (single → double)
inline void EmitFcvtS2D(CodeBuffer& buf, uint8_t rd, uint8_t rn) noexcept {
    EmitArm64(buf, 0x1E22C000u | (rn << 5) | rd);
}

/// FCVT Sd, Dn (double → single)
inline void EmitFcvtD2S(CodeBuffer& buf, uint8_t rd, uint8_t rn) noexcept {
    EmitArm64(buf, 0x1E624000u | (rn << 5) | rd);
}

/// SCVTF Dd, Xn (integer → double)
inline void EmitScvtfD64(CodeBuffer& buf, uint8_t rd, uint8_t rn) noexcept {
    EmitArm64(buf, 0x9E620000u | (rn << 5) | rd);
}

/// SCVTF Sd, Wn (32-bit integer → float)
inline void EmitScvtfS32(CodeBuffer& buf, uint8_t rd, uint8_t rn) noexcept {
    EmitArm64(buf, 0x1E220000u | (rn << 5) | rd);
}

/// FCVTZS Xd, Dn (double → signed integer, truncate toward zero)
inline void EmitFcvtzsD64(CodeBuffer& buf, uint8_t rd, uint8_t rn) noexcept {
    EmitArm64(buf, 0x9E780000u | (rn << 5) | rd);
}

/// FCVTZS Wd, Sn (float → 32-bit signed integer)
inline void EmitFcvtzsS32(CodeBuffer& buf, uint8_t rd, uint8_t rn) noexcept {
    EmitArm64(buf, 0x1E380000u | (rn << 5) | rd);
}

/// FCMP Dn, Dm (compare double, set NZCV)
inline void EmitFcmp64(CodeBuffer& buf, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x1E602000u | (rm << 16) | (rn << 5));
}

/// FCMP Sn, Sm (compare float, set NZCV)
inline void EmitFcmp32(CodeBuffer& buf, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x1E202000u | (rm << 16) | (rn << 5));
}

// ═══════════════════════════════════════════════════════════════════════════
// FP load/store (scalar)
// ═══════════════════════════════════════════════════════════════════════════

/// 64-bit LDR Dt, [Xn, #imm12 * 8]
inline void EmitLdrD(CodeBuffer& buf, uint8_t rt, uint8_t rn, uint16_t imm12) noexcept {
    EmitArm64(buf, 0xFD400000u | (static_cast<uint32_t>(imm12 & 0xFFF) << 10) | (rn << 5) | rt);
}

/// 32-bit LDR St, [Xn, #imm12 * 4]
inline void EmitLdrS(CodeBuffer& buf, uint8_t rt, uint8_t rn, uint16_t imm12) noexcept {
    EmitArm64(buf, 0xBD400000u | (static_cast<uint32_t>(imm12 & 0xFFF) << 10) | (rn << 5) | rt);
}

/// 64-bit STR Dt, [Xn, #imm12 * 8]
inline void EmitStrD(CodeBuffer& buf, uint8_t rt, uint8_t rn, uint16_t imm12) noexcept {
    EmitArm64(buf, 0xFD000000u | (static_cast<uint32_t>(imm12 & 0xFFF) << 10) | (rn << 5) | rt);
}

/// 32-bit STR St, [Xn, #imm12 * 4]
inline void EmitStrS(CodeBuffer& buf, uint8_t rt, uint8_t rn, uint16_t imm12) noexcept {
    EmitArm64(buf, 0xBD000000u | (static_cast<uint32_t>(imm12 & 0xFFF) << 10) | (rn << 5) | rt);
}

/// 128-bit LDR Qt, [Xn, #imm12 * 16]
inline void EmitLdrQ(CodeBuffer& buf, uint8_t rt, uint8_t rn, uint16_t imm12) noexcept {
    EmitArm64(buf, 0x3DC00000u | (static_cast<uint32_t>(imm12 & 0xFFF) << 10) | (rn << 5) | rt);
}

/// 128-bit STR Qt, [Xn, #imm12 * 16]
inline void EmitStrQ(CodeBuffer& buf, uint8_t rt, uint8_t rn, uint16_t imm12) noexcept {
    EmitArm64(buf, 0x3D800000u | (static_cast<uint32_t>(imm12 & 0xFFF) << 10) | (rn << 5) | rt);
}

// ═══════════════════════════════════════════════════════════════════════════
// SIMD (Advanced SIMD)
// ═══════════════════════════════════════════════════════════════════════════

/// XOR Vd.16B, Vn.16B, Vm.16B (bitwise XOR, 16 bytes)
inline void EmitXor16B(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x6E201C00u | (rm << 16) | (rn << 5) | rd);
}

/// ORR Vd.16B, Vn.16B, Vm.16B (bitwise OR / register move, 16 bytes)
inline void EmitOrr16B(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x6E231C00u | (rm << 16) | (rn << 5) | rd);
}

/// AND Vd.16B, Vn.16B, Vm.16B (bitwise AND, 16 bytes)
inline void EmitAnd16B(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4E231C00u | (rm << 16) | (rn << 5) | rd);
}

/// BIC Vd.16B, Vn.16B, Vm.16B (bitwise AND NOT, 16 bytes)
inline void EmitBic16B(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4E2B1C00u | (rm << 16) | (rn << 5) | rd);
}

// ── Integer ALU (3-register same, Q=1 for 128-bit) ──────────────────────────

/// ADD Vd.16B, Vn.16B, Vm.16B
inline void EmitAdd16B(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4E20C400u | (rm << 16) | (rn << 5) | rd);
}

/// ADD Vd.8H, Vn.8H, Vm.8H
inline void EmitAdd8H(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4E60C400u | (rm << 16) | (rn << 5) | rd);
}

/// ADD Vd.4S, Vn.4S, Vm.4S
inline void EmitAdd4S(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4EA0C400u | (rm << 16) | (rn << 5) | rd);
}

/// ADD Vd.2D, Vn.2D, Vm.2D
inline void EmitAdd2D(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4EE0C400u | (rm << 16) | (rn << 5) | rd);
}

/// SUB Vd.16B, Vn.16B, Vm.16B
inline void EmitSub16B(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4E30C400u | (rm << 16) | (rn << 5) | rd);
}

/// SUB Vd.8H, Vn.8H, Vm.8H
inline void EmitSub8H(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4E70C400u | (rm << 16) | (rn << 5) | rd);
}

/// SUB Vd.4S, Vn.4S, Vm.4S
inline void EmitSub4S(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4EB0C400u | (rm << 16) | (rn << 5) | rd);
}

/// SUB Vd.2D, Vn.2D, Vm.2D
inline void EmitSub2D(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4EF0C400u | (rm << 16) | (rn << 5) | rd);
}

/// MUL Vd.8H, Vn.8H, Vm.8H
inline void EmitMul8H(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4E73C400u | (rm << 16) | (rn << 5) | rd);
}

/// MUL Vd.4S, Vn.4S, Vm.4S
inline void EmitMul4S(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4EB3C400u | (rm << 16) | (rn << 5) | rd);
}

/// CMEQ Vd.16B, Vn.16B, Vm.16B (compare equal)
inline void EmitCmeq16B(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x6E31C400u | (rm << 16) | (rn << 5) | rd);
}

/// CMEQ Vd.8H, Vn.8H, Vm.8H
inline void EmitCmeq8H(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x6E71C400u | (rm << 16) | (rn << 5) | rd);
}

/// CMEQ Vd.4S, Vn.4S, Vm.4S
inline void EmitCmeq4S(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x6EB1C400u | (rm << 16) | (rn << 5) | rd);
}

/// CMEQ Vd.2D, Vn.2D, Vm.2D
inline void EmitCmeq2D(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x6EF1C400u | (rm << 16) | (rn << 5) | rd);
}

/// CMGT Vd.16B, Vn.16B, Vm.16B (signed greater than)
inline void EmitCmgt16B(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4E25C400u | (rm << 16) | (rn << 5) | rd);
}

/// CMGT Vd.8H, Vn.8H, Vm.8H
inline void EmitCmgt8H(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4E65C400u | (rm << 16) | (rn << 5) | rd);
}

/// CMGT Vd.4S, Vn.4S, Vm.4S
inline void EmitCmgt4S(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4EA5C400u | (rm << 16) | (rn << 5) | rd);
}

/// CMGT Vd.2D, Vn.2D, Vm.2D
inline void EmitCmgt2D(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4EE5C400u | (rm << 16) | (rn << 5) | rd);
}

/// UMULL Vd.2D, Vn.2S, Vm.2S (unsigned multiply long, 32→64)
inline void EmitUmull2D(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x6E80C400u | (rm << 16) | (rn << 5) | rd);
}

// ── 2-register misc (Q=1 for 128-bit) ──────────────────────────────────────

/// CNT Vd.8B, Vn.8B (count bits in each byte, 8-byte)
inline void EmitCnt8B(CodeBuffer& buf, uint8_t rd, uint8_t rn) noexcept {
    EmitArm64(buf, 0x0E205800u | (rn << 5) | rd);
}

/// UADDLV Dd, Vn.8B (unsigned add across vector, 8 bytes → scalar)
inline void EmitUaddlv8B(CodeBuffer& buf, uint8_t rd, uint8_t rn) noexcept {
    EmitArm64(buf, 0x4E303800u | (rn << 5) | rd);
}

/// ABS Vd.16B, Vn.16B (absolute value, 16 bytes)
inline void EmitAbs16B(CodeBuffer& buf, uint8_t rd, uint8_t rn) noexcept {
    EmitArm64(buf, 0x4E20B800u | (rn << 5) | rd);
}

/// ABS Vd.8H, Vn.8H
inline void EmitAbs8H(CodeBuffer& buf, uint8_t rd, uint8_t rn) noexcept {
    EmitArm64(buf, 0x4E60B800u | (rn << 5) | rd);
}

/// ABS Vd.4S, Vn.4S
inline void EmitAbs4S(CodeBuffer& buf, uint8_t rd, uint8_t rn) noexcept {
    EmitArm64(buf, 0x4EA0B800u | (rn << 5) | rd);
}

// ── Add across vector (2-register misc) ─────────────────────────────────────

/// ADDV Dd, Vn.16B (add across 16 bytes → scalar byte in Dd[0])
inline void EmitAddv16B(CodeBuffer& buf, uint8_t rd, uint8_t rn) noexcept {
    EmitArm64(buf, 0x4E30B800u | (rn << 5) | rd);
}

/// ADDV Dd, Vn.8H (add across 8 halfwords → scalar halfword in Dd[0])
inline void EmitAddv8H(CodeBuffer& buf, uint8_t rd, uint8_t rn) noexcept {
    EmitArm64(buf, 0x4E70B800u | (rn << 5) | rd);
}

/// ADDV Dd, Vn.4S (add across 4 words → scalar word in Dd[0])
inline void EmitAddv4S(CodeBuffer& buf, uint8_t rd, uint8_t rn) noexcept {
    EmitArm64(buf, 0x4EB0B800u | (rn << 5) | rd);
}

// ── Table lookup ───────────────────────────────────────────────────────────

/// TBL Vd.16B, {Vn.16B}, Vm.16B (1-register table, byte permute)
inline void EmitTbl1(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4E000000u | (rm << 16) | (rn << 5) | rd);
}

// ── DUP (general register) ─────────────────────────────────────────────────

/// DUP Vd.16B, Wn (broadcast byte from GPR, 16 bytes)
inline void EmitDup16B(CodeBuffer& buf, uint8_t rd, uint8_t rn) noexcept {
    EmitArm64(buf, 0x4E000400u | (rn << 5) | rd);
}

/// DUP Vd.8H, Wn (broadcast halfword from GPR, 8 × 16-bit)
inline void EmitDup8H(CodeBuffer& buf, uint8_t rd, uint8_t rn) noexcept {
    EmitArm64(buf, 0x4E010400u | (rn << 5) | rd);
}

/// DUP Vd.4S, Wn (broadcast word from GPR, 4 × 32-bit)
inline void EmitDup4S(CodeBuffer& buf, uint8_t rd, uint8_t rn) noexcept {
    EmitArm64(buf, 0x4E020400u | (rn << 5) | rd);
}

/// DUP Vd.2D, Xn (broadcast doubleword from GPR, 2 × 64-bit)
inline void EmitDup2D(CodeBuffer& buf, uint8_t rd, uint8_t rn) noexcept {
    EmitArm64(buf, 0x4E030400u | (rn << 5) | rd);
}

// ── Permute: ZIP / UZP / TRN ───────────────────────────────────────────────

/// ZIP1 Vd.16B, Vn.16B, Vm.16B (interleave low halves)
inline void EmitZip1_16B(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4E003800u | (rm << 16) | (rn << 5) | rd);
}

/// ZIP2 Vd.16B, Vn.16B, Vm.16B (interleave high halves)
inline void EmitZip2_16B(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4E007800u | (rm << 16) | (rn << 5) | rd);
}

/// ZIP1 Vd.8H, Vn.8H, Vm.8H
inline void EmitZip1_8H(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4E403800u | (rm << 16) | (rn << 5) | rd);
}

/// ZIP2 Vd.8H, Vn.8H, Vm.8H
inline void EmitZip2_8H(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4E407800u | (rm << 16) | (rn << 5) | rd);
}

/// ZIP1 Vd.4S, Vn.4S, Vm.4S
inline void EmitZip1_4S(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4E803800u | (rm << 16) | (rn << 5) | rd);
}

/// ZIP2 Vd.4S, Vn.4S, Vm.4S
inline void EmitZip2_4S(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4E807800u | (rm << 16) | (rn << 5) | rd);
}

/// ZIP1 Vd.2D, Vn.2D, Vm.2D
inline void EmitZip1_2D(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4EC03800u | (rm << 16) | (rn << 5) | rd);
}

/// ZIP2 Vd.2D, Vn.2D, Vm.2D
inline void EmitZip2_2D(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4EC07800u | (rm << 16) | (rn << 5) | rd);
}

/// UZP1 Vd.16B, Vn.16B, Vm.16B (de-interleave even elements)
inline void EmitUzp1_16B(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4E001800u | (rm << 16) | (rn << 5) | rd);
}

/// UZP2 Vd.16B, Vn.16B, Vm.16B (de-interleave odd elements)
inline void EmitUzp2_16B(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4E005800u | (rm << 16) | (rn << 5) | rd);
}

/// UZP1 Vd.8H, Vn.8H, Vm.8H
inline void EmitUzp1_8H(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4E401800u | (rm << 16) | (rn << 5) | rd);
}

/// UZP1 Vd.4S, Vn.4S, Vm.4S
inline void EmitUzp1_4S(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4E801800u | (rm << 16) | (rn << 5) | rd);
}

/// UZP1 Vd.2D, Vn.2D, Vm.2D
inline void EmitUzp1_2D(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4EC01800u | (rm << 16) | (rn << 5) | rd);
}

/// TRN1 Vd.16B, Vn.16B, Vm.16B (transpose low halves)
inline void EmitTrn1_16B(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4E002800u | (rm << 16) | (rn << 5) | rd);
}

/// TRN2 Vd.16B, Vn.16B, Vm.16B (transpose high halves)
inline void EmitTrn2_16B(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4E006800u | (rm << 16) | (rn << 5) | rd);
}

/// TRN1 Vd.8H, Vn.8H, Vm.8H
inline void EmitTrn1_8H(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4E402800u | (rm << 16) | (rn << 5) | rd);
}

/// TRN1 Vd.4S, Vn.4S, Vm.4S
inline void EmitTrn1_4S(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4E802800u | (rm << 16) | (rn << 5) | rd);
}

/// TRN1 Vd.2D, Vn.2D, Vm.2D
inline void EmitTrn1_2D(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4EC02800u | (rm << 16) | (rn << 5) | rd);
}

// ── Variable shift ─────────────────────────────────────────────────────────

/// SSHL Vd.16B, Vn.16B, Vm.16B (signed shift left by signed variable)
inline void EmitSshl16B(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4E24C400u | (rm << 16) | (rn << 5) | rd);
}

/// USHL Vd.16B, Vn.16B, Vm.16B (unsigned shift left by signed variable)
inline void EmitUshl16B(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x6E24C400u | (rm << 16) | (rn << 5) | rd);
}

/// USHL Vd.8H, Vn.8H, Vm.8H (unsigned shift left by signed variable, 8×int16)
inline void EmitUshl8H(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x6E64C400u | (rm << 16) | (rn << 5) | rd);
}

/// USHL Vd.4S, Vn.4S, Vm.4S (unsigned shift left by signed variable, 4×int32)
inline void EmitUshl4S(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x6EA4C400u | (rm << 16) | (rn << 5) | rd);
}

/// USHL Vd.2D, Vn.2D, Vm.2D (unsigned shift left by signed variable, 2×int64)
inline void EmitUshl2D(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x6EE4C400u | (rm << 16) | (rn << 5) | rd);
}

/// SSHL Vd.8H, Vn.8H, Vm.8H
inline void EmitSshl8H(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4E64C400u | (rm << 16) | (rn << 5) | rd);
}

/// SSHL Vd.4S, Vn.4S, Vm.4S
inline void EmitSshl4S(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4EA4C400u | (rm << 16) | (rn << 5) | rd);
}

/// SSHL Vd.2D, Vn.2D, Vm.2D
inline void EmitSshl2D(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4EE4C400u | (rm << 16) | (rn << 5) | rd);
}

// ── EXT (extract) ──────────────────────────────────────────────────────────

/// EXT Vd.16B, Vn.16B, Vm.16B, #imm4 (extract bytes from concatenation)
inline void EmitExt16B(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm, uint8_t imm4) noexcept {
    EmitArm64(buf, 0x6E000000u | (rm << 16) | (static_cast<uint32_t>(imm4 & 0xF) << 11) | (rn << 5) | rd);
}

// ── Integer scalar ──────────────────────────────────────────────────────────

/// CLZ Xd, Xn (count leading zeros, 64-bit)
inline void EmitClz64(CodeBuffer& buf, uint8_t rd, uint8_t rn) noexcept {
    EmitArm64(buf, 0xDAC01000u | (rn << 5) | rd);
}

/// CLZ Wd, Wn (count leading zeros, 32-bit)
inline void EmitClz32(CodeBuffer& buf, uint8_t rd, uint8_t rn) noexcept {
    EmitArm64(buf, 0x5AC01000u | (rn << 5) | rd);
}

// ── FP vector (Advanced SIMD 3-same FP) ────────────────────────────────────

/// FADD Vd.4S, Vn.4S, Vm.4S (float add, 4×float32)
inline void EmitFadd4S(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4E20D400u | (rm << 16) | (rn << 5) | rd);
}

/// FADD Vd.2D, Vn.2D, Vm.2D (float add, 2×float64)
inline void EmitFadd2D(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4E60D400u | (rm << 16) | (rn << 5) | rd);
}

/// FSUB Vd.4S, Vn.4S, Vm.4S (float subtract, 4×float32)
inline void EmitFsub4S(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4EA0D400u | (rm << 16) | (rn << 5) | rd);
}

/// FSUB Vd.2D, Vn.2D, Vm.2D (float subtract, 2×float64)
inline void EmitFsub2D(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4EE0D400u | (rm << 16) | (rn << 5) | rd);
}

/// FMUL Vd.4S, Vn.4S, Vm.4S (float multiply, 4×float32)
inline void EmitFmul4S(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x6E20DC00u | (rm << 16) | (rn << 5) | rd);
}

/// FMUL Vd.2D, Vn.2D, Vm.2D (float multiply, 2×float64)
inline void EmitFmul2D(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x6E60DC00u | (rm << 16) | (rn << 5) | rd);
}

/// FCMEQ Vd.4S, Vn.4S, Vm.4S (float compare equal, 4×float32)
inline void EmitFcmeq4S(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4E20E400u | (rm << 16) | (rn << 5) | rd);
}

/// FCMEQ Vd.2D, Vn.2D, Vm.2D (float compare equal, 2×float64)
inline void EmitFcmeq2D(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4E60E400u | (rm << 16) | (rn << 5) | rd);
}

/// FCMGT Vd.4S, Vn.4S, Vm.4S (float greater than, 4×float32)
inline void EmitFcmgt4S(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x6EA0E400u | (rm << 16) | (rn << 5) | rd);
}

/// FCMGT Vd.2D, Vn.2D, Vm.2D (float greater than, 2×float64)
inline void EmitFcmgt2D(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x6EE0E400u | (rm << 16) | (rn << 5) | rd);
}

/// FCMGE Vd.4S, Vn.4S, Vm.4S (float greater or equal, 4×float32)
inline void EmitFcmge4S(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x6E20E400u | (rm << 16) | (rn << 5) | rd);
}

/// FCMGE Vd.2D, Vn.2D, Vm.2D (float greater or equal, 2×float64)
inline void EmitFcmge2D(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x6E60E400u | (rm << 16) | (rn << 5) | rd);
}

/// SCVTF Vd.4S, Vn.4S (signed integer → float32, 4×32)
inline void EmitScvtf4S(CodeBuffer& buf, uint8_t rd, uint8_t rn) noexcept {
    EmitArm64(buf, 0x4E21D800u | (rn << 5) | rd);
}

/// SCVTF Vd.2D, Vn.2D (signed integer → float64, 2×64)
inline void EmitScvtf2D(CodeBuffer& buf, uint8_t rd, uint8_t rn) noexcept {
    EmitArm64(buf, 0x4E61D800u | (rn << 5) | rd);
}

/// FCVTZS Vd.4S, Vn.4S (float32 → signed int32, truncate, 4×32)
inline void EmitFcvtzs4S(CodeBuffer& buf, uint8_t rd, uint8_t rn) noexcept {
    EmitArm64(buf, 0x4EA1B800u | (rn << 5) | rd);
}

/// FCVTZS Vd.2D, Vn.2D (float64 → signed int64, truncate, 2×64)
inline void EmitFcvtzs2D(CodeBuffer& buf, uint8_t rd, uint8_t rn) noexcept {
    EmitArm64(buf, 0x4EE1B800u | (rn << 5) | rd);
}

/// FMLA Vd.4S, Vn.4S, Vm.4S (fused multiply-accumulate, 4×float32)
inline void EmitFmla4S(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4E20CC00u | (rm << 16) | (rn << 5) | rd);
}

/// FMLA Vd.2D, Vn.2D, Vm.2D (fused multiply-accumulate, 2×float64)
inline void EmitFmla2D(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4E60CC00u | (rm << 16) | (rn << 5) | rd);
}

/// FMLS Vd.4S, Vn.4S, Vm.4S (fused multiply-subtract, 4×float32)
inline void EmitFmls4S(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4EA0CC00u | (rm << 16) | (rn << 5) | rd);
}

/// FMLS Vd.2D, Vn.2D, Vm.2D (fused multiply-subtract, 2×float64)
inline void EmitFmls2D(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
    EmitArm64(buf, 0x4EE0CC00u | (rm << 16) | (rn << 5) | rd);
}

/// FNEG Vd.4S, Vn.4S (negate, 4×float32)
inline void EmitFneg4S(CodeBuffer& buf, uint8_t rd, uint8_t rn) noexcept {
    EmitArm64(buf, 0x6EA0F800u | (rn << 5) | rd);
}

/// FNEG Vd.2D, Vn.2D (negate, 2×float64)
inline void EmitFneg2D(CodeBuffer& buf, uint8_t rd, uint8_t rn) noexcept {
    EmitArm64(buf, 0x6EE0F800u | (rn << 5) | rd);
}

// ═══════════════════════════════════════════════════════════════════════════
// System
// ═══════════════════════════════════════════════════════════════════════════

/// NOP
inline void EmitNop(CodeBuffer& buf) noexcept {
    EmitArm64(buf, 0xD503201Fu);
}

/// DMB (data memory barrier)
inline void EmitDmb(CodeBuffer& buf) noexcept {
    EmitArm64(buf, 0xD50330BFu);
}

}  // namespace chaos::il2cpp::jit

#endif  // CHAOS_IL2CPP_ARM64_ENCODER_H_
