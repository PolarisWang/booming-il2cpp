#ifndef CHAOS_IL2CPP_CODEGEN_X64_ENCODER_H_
#define CHAOS_IL2CPP_CODEGEN_X64_ENCODER_H_

// ── x64 instruction encoder ────────────────────────────────────────────────
//
// Minimal x64 instruction encoding primitives for the self-hosted code
// generator.  Supports the subset of x64 instructions needed to translate
// RegisterInstruction[] to native code.
//
// Intel SDM register numbering (for ModRM.reg and ModRM.rm fields):
//   0=RAX  1=RCX  2=RDX  3=RBX  4=RSP  5=RBP  6=RSI  7=RDI
//   8=R8   9=R9   10=R10 11=R11 12=R12 13=R13 14=R14 15=R15
// Note: RSP/RBP have special ModRM encoding requirements.

#include <cstdint>
#include <cstddef>

#include "code_buffer.h"
#include "interpreter_vm.h"
#include "IEncoder.h"

namespace chaos::il2cpp::jit {

// ── x64 register constants (defined in IEncoder.h; included above) ─────────
// ── REX prefix ─────────────────────────────────────────────────────────────
// Table 2-4: REX {0100wrb}
//   w=1 → 64-bit operand size, w=0 → 32-bit (or default)
//   r   → extends ModRM.reg by 1 bit (bit 3 → bit 4 of register)
//   x   → extends SIB.index by 1 bit
//   b   → extends ModRM.rm or SIB.base by 1 bit

inline uint8_t REX(bool w, uint8_t r, uint8_t x, uint8_t b) noexcept {
    return static_cast<uint8_t>(0x40 | (w ? 8 : 0) | ((r & 8) ? 4 : 0) |
                                ((x & 8) ? 2 : 0) | ((b & 8) ? 1 : 0));
}

/// Emit REX prefix for a reg, rm operation.
inline void EmitREX(CodeBuffer& buf, bool w, uint8_t reg, uint8_t rm) noexcept {
    uint8_t rex = REX(w, reg, 0, rm);
    if (rex != 0x40) buf.EmitByte(rex);
}

/// Emit REX prefix with b-only (for opcodes that encode reg in the opcode byte).
inline void EmitREXB(CodeBuffer& buf, bool w, uint8_t rm) noexcept {
    uint8_t rex = REX(w, 0, 0, rm);
    if (rex != 0x40) buf.EmitByte(rex);
}

// ── ModRM byte ─────────────────────────────────────────────────────────────
// mod: 00=register/[base], 01=[base+disp8], 10=[base+disp32], 11=register
// reg: opcode extension or register operand
// rm:  register or memory operand

inline uint8_t ModRM(uint8_t mod, uint8_t reg, uint8_t rm) noexcept {
    return static_cast<uint8_t>(((mod & 3) << 6) | ((reg & 7) << 3) | (rm & 7));
}

// ── SIB byte ───────────────────────────────────────────────────────────────
// scale: 00=1, 01=2, 10=4, 11=8
// index: register for scaled index
// base:  base register

inline uint8_t SIB(uint8_t scale, uint8_t index, uint8_t base) noexcept {
    return static_cast<uint8_t>(((scale & 3) << 6) | ((index & 7) << 3) | (base & 7));
}

// ── Emit ModRM + optional SIB + displacement ───────────────────────────────
// Handles the special encoding for RSP (needs SIB) and RBP/R13 (needs disp8/32).

inline void EmitModRM(CodeBuffer& buf, uint8_t mod, uint8_t reg, uint8_t rm) noexcept {
    buf.EmitByte(ModRM(mod, reg, rm));
}

/// Encode reg, [base + disp] addressing.
inline void EmitMR(CodeBuffer& buf, uint8_t reg, uint8_t base, int32_t disp) noexcept {
    if (disp == 0 && base != kRBP && base != kR13) {
        // [base] — no displacement (but RBP/R13 with mod=00 encodes RIP-relative)
        if (base == kRSP || base == kR12) {
            // RSP/R12 always need SIB
            buf.EmitByte(ModRM(0, reg, 4));
            buf.EmitByte(SIB(0, 4, base));
        } else {
            buf.EmitByte(ModRM(0, reg, base));
        }
    } else if (disp >= -128 && disp <= 127) {
        // [base + disp8]
        if (base == kRSP || base == kR12) {
            buf.EmitByte(ModRM(1, reg, 4));
            buf.EmitByte(SIB(0, 4, base));
        } else {
            buf.EmitByte(ModRM(1, reg, base));
        }
        buf.EmitByte(static_cast<uint8_t>(disp & 0xFF));
    } else {
        // [base + disp32]
        if (base == kRSP || base == kR12) {
            buf.EmitByte(ModRM(2, reg, 4));
            buf.EmitByte(SIB(0, 4, base));
        } else {
            buf.EmitByte(ModRM(2, reg, base));
        }
        buf.Emit32(static_cast<uint32_t>(disp));
    }
}

// ── Primary opcode group helper ────────────────────────────────────────────

// ── MOV instructions ───────────────────────────────────────────────────────

/// mov r64, imm64 (10 bytes, uses REX.W)
inline void EmitMovImm64(CodeBuffer& buf, uint8_t dst, uint64_t imm) noexcept {
    // REX.W + B8+reg + imm64
    EmitREXB(buf, true, dst);
    buf.EmitByte(static_cast<uint8_t>(0xB8 + (dst & 7)));
    buf.Emit64(imm);
}

// ── MOV r/m64, r64 ──── mov [mem], reg ─────────────────────────────────────
inline void EmitMovMR(CodeBuffer& buf, uint8_t base, int32_t disp, uint8_t src) noexcept {
    // REX.W + 89 /r
    EmitREX(buf, true, src, base);
    buf.EmitByte(0x89);
    EmitMR(buf, src, base, disp);
}

/// mov dword [base+disp], imm32  (32-bit immediate-to-memory store)
inline void EmitMovMI32(CodeBuffer& buf, uint8_t base, int32_t disp, uint32_t imm) noexcept {
    // C7 /0 id: C7 + ModRM(reg=0, rm=base) + disp + imm32
    // No REX.W = 32-bit operand; REX.B for extended base regs.
    uint8_t rex = REX(false, 0, 0, base);
    if (rex != 0x40) buf.EmitByte(rex);  // REX.B if base > 7
    buf.EmitByte(0xC7);
    EmitMR(buf, 0, base, disp);
    buf.Emit32(imm);
}

/// mov r64, r/m64  ──── mov reg, [mem]
inline void EmitMovRM(CodeBuffer& buf, uint8_t dst, uint8_t base, int32_t disp) noexcept {
    EmitREX(buf, true, dst, base);
    buf.EmitByte(0x8B);
    EmitMR(buf, dst, base, disp);
}

/// lea r64, [base + disp]  ──── load effective address
inline void EmitLeaRM(CodeBuffer& buf, uint8_t dst, uint8_t base, int32_t disp) noexcept {
    EmitREX(buf, true, dst, base);
    buf.EmitByte(0x8D);
    EmitMR(buf, dst, base, disp);
}

/// lea r64, [rip + disp32]  ──── load RIP-relative effective address
/// ModRM.mod=00 with ModRM.rm=101 encodes RIP-relative addressing in x64.
inline void EmitLeaRipRel(CodeBuffer& buf, uint8_t dst, int32_t disp) noexcept {
    EmitREX(buf, true, dst, 0);
    buf.EmitByte(0x8D);
    buf.EmitByte(ModRM(0, dst, 5));  // rm=5 with mod=00 = RIP-relative
    buf.Emit32(static_cast<uint32_t>(disp));
}

/// mov r64, r64       (register-to-register)
inline void EmitMovRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    if (dst == src) return;  // nop
    // Opcode 89 = MOV r/m64, r64: reg field = SOURCE (r64), r/m field = DESTINATION (r/m64).
    EmitREX(buf, true, src, dst);
    buf.EmitByte(0x89);
    buf.EmitByte(ModRM(3, src, dst));
}

/// mov r64, imm32 sign-extended (C7 /0)
inline void EmitMovRI32(CodeBuffer& buf, uint8_t dst, int32_t imm) noexcept {
    EmitREX(buf, true, 0, dst);
    buf.EmitByte(0xC7);
    buf.EmitByte(ModRM(3, 0, dst));
    buf.Emit32(static_cast<uint32_t>(imm));
}

/// mov eax, imm32 (32-bit, zero-extends to 64-bit, 5 bytes for RAX special)
/// Actually just mov r32, imm32 — B8+rd id (32-bit)
inline void EmitMovRIImm32(CodeBuffer& buf, uint8_t dst, uint32_t imm) noexcept {
    if (dst < 8) {
        buf.EmitByte(static_cast<uint8_t>(0xB8 + dst));
    } else {
        buf.EmitByte(REX(false, 0, 0, dst));
        buf.EmitByte(static_cast<uint8_t>(0xB8 + (dst & 7)));
    }
    buf.Emit32(imm);
}

/// mov al, imm8
inline void EmitMovRI8(CodeBuffer& buf, uint8_t dst, uint8_t imm) noexcept {
    // B0+rd ib (for low 8 registers) or REX + B0+rd ib
    // For simplicity: always emit REX prefix if dst >= 4 (SIL/DIL/BPL/SPL)
    // and for r8-r15
    if (dst >= 4 && dst <= 7) {
        // Need REX for SIL/DIL/BPL/SPL
        buf.EmitByte(0x40);
    } else if (dst >= 8) {
        buf.EmitByte(REX(false, 0, 0, dst));
    }
    buf.EmitByte(static_cast<uint8_t>(0xB0 + (dst & 7)));
    buf.EmitByte(imm);
}

// ── Arithmetic ───────────────────────────────────────────────────────────

/// add r/m64, r64 — [base+disp] += src
inline void EmitAddMR(CodeBuffer& buf, uint8_t base, int32_t disp, uint8_t src) noexcept {
    EmitREX(buf, true, src, base);
    buf.EmitByte(0x01);
    EmitMR(buf, src, base, disp);
}

/// add r64, r/m64 — dst += src
inline void EmitAddRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    EmitREX(buf, true, dst, src);
    buf.EmitByte(0x03);
    buf.EmitByte(ModRM(3, dst, src));
}

/// add r/m64, imm32
inline void EmitAddRI(CodeBuffer& buf, uint8_t dst, int32_t imm) noexcept {
    if (imm == 0) return;
    if (imm >= -128 && imm <= 127) {
        // 83 /0 id8
        EmitREX(buf, true, 0, dst);
        buf.EmitByte(0x83);
        buf.EmitByte(ModRM(3, 0, dst));
        buf.EmitByte(static_cast<uint8_t>(imm));
    } else {
        // 81 /0 id32
        EmitREX(buf, true, 0, dst);
        buf.EmitByte(0x81);
        buf.EmitByte(ModRM(3, 0, dst));
        buf.Emit32(static_cast<uint32_t>(imm));
    }
}

/// sub r/m64, imm32
inline void EmitSubRI(CodeBuffer& buf, uint8_t dst, int32_t imm) noexcept {
    if (imm == 0) return;
    if (imm >= -128 && imm <= 127) {
        EmitREX(buf, true, 5, dst);  // /5 = sub opcode extension
        buf.EmitByte(0x83);
        buf.EmitByte(ModRM(3, 5, dst));
        buf.EmitByte(static_cast<uint8_t>(imm));
    } else {
        EmitREX(buf, true, 5, dst);
        buf.EmitByte(0x81);
        buf.EmitByte(ModRM(3, 5, dst));
        buf.Emit32(static_cast<uint32_t>(imm));
    }
}

/// sub r64, r/m64 — dst -= src
inline void EmitSubRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    EmitREX(buf, true, dst, src);
    buf.EmitByte(0x2B);
    buf.EmitByte(ModRM(3, dst, src));
}

/// imul r64, r/m64 — dst *= src (signed)
inline void EmitImulRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    EmitREX(buf, true, dst, src);
    buf.EmitByte(0x0F);
    buf.EmitByte(0xAF);
    buf.EmitByte(ModRM(3, dst, src));
}

/// and r64, r/m64
inline void EmitAndRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    EmitREX(buf, true, dst, src);
    buf.EmitByte(0x23);
    buf.EmitByte(ModRM(3, dst, src));
}

/// or r64, r/m64
inline void EmitOrRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    EmitREX(buf, true, dst, src);
    buf.EmitByte(0x0B);
    buf.EmitByte(ModRM(3, dst, src));
}

/// xor r64, r/m64
inline void EmitXorRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    EmitREX(buf, true, dst, src);
    buf.EmitByte(0x33);
    buf.EmitByte(ModRM(3, dst, src));
}

// ── 32-bit GPR arithmetic (zero-extend to 64-bit, matching x86 natural behavior) ──

/// add r32, r/m32
inline void EmitAdd32RR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    EmitREX(buf, false, dst, src);
    buf.EmitByte(0x03);
    buf.EmitByte(ModRM(3, dst, src));
}

/// sub r32, r/m32
inline void EmitSub32RR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    EmitREX(buf, false, dst, src);
    buf.EmitByte(0x2B);
    buf.EmitByte(ModRM(3, dst, src));
}

/// imul r32, r/m32
inline void EmitImul32RR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F);
    buf.EmitByte(0xAF);
    buf.EmitByte(ModRM(3, dst, src));
}

/// and r32, r/m32
inline void EmitAnd32RR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    EmitREX(buf, false, dst, src);
    buf.EmitByte(0x23);
    buf.EmitByte(ModRM(3, dst, src));
}

/// or r32, r/m32
inline void EmitOr32RR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0B);
    buf.EmitByte(ModRM(3, dst, src));
}

/// xor r32, r/m32
inline void EmitXor32RR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    EmitREX(buf, false, dst, src);
    buf.EmitByte(0x33);
    buf.EmitByte(ModRM(3, dst, src));
}

/// neg r/m32
inline void EmitNeg32(CodeBuffer& buf, uint8_t reg) noexcept {
    EmitREX(buf, false, 3, reg);
    buf.EmitByte(0xF7);
    buf.EmitByte(ModRM(3, 3, reg));
}

/// not r/m32
inline void EmitNot32(CodeBuffer& buf, uint8_t reg) noexcept {
    EmitREX(buf, false, 2, reg);
    buf.EmitByte(0xF7);
    buf.EmitByte(ModRM(3, 2, reg));
}

/// xor r32, r/m32 (zero a register: xor eax, eax is shorter than xor rax, rax)
inline void EmitXor32ZR(CodeBuffer& buf, uint8_t reg) noexcept {
    EmitREX(buf, false, reg, reg);
    buf.EmitByte(0x33);
    buf.EmitByte(ModRM(3, reg, reg));
}

/// movsxd r64, r/m32 — sign-extend dword to qword (already exists, reference below)
// inline void EmitMovsxd(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept;

/// xor r/m64, r64 — [base+disp] ^= src
inline void EmitXorMR(CodeBuffer& buf, uint8_t base, int32_t disp, uint8_t src) noexcept {
    EmitREX(buf, true, src, base);
    buf.EmitByte(0x31);
    EmitMR(buf, src, base, disp);
}

/// xor r64, r/m64 — dst ^= [base+disp]
inline void EmitXorRM(CodeBuffer& buf, uint8_t dst, uint8_t base, int32_t disp) noexcept {
    EmitREX(buf, true, dst, base);
    buf.EmitByte(0x33);
    EmitMR(buf, dst, base, disp);
}

/// xor eax, eax / xor rax, rax (zero register, 3 bytes with REX or 2 bytes without)
inline void EmitXorZR(CodeBuffer& buf, uint8_t reg) noexcept {
    EmitREX(buf, true, reg, reg);
    buf.EmitByte(0x33);
    buf.EmitByte(ModRM(3, reg, reg));
}

// ── Neg / Not ────────────────────────────────────────────────────────────

/// neg r/m64
inline void EmitNeg(CodeBuffer& buf, uint8_t reg) noexcept {
    EmitREX(buf, true, 3, reg);  // /3 = neg
    buf.EmitByte(0xF7);
    buf.EmitByte(ModRM(3, 3, reg));
}

/// not r/m64
inline void EmitNot(CodeBuffer& buf, uint8_t reg) noexcept {
    EmitREX(buf, true, 2, reg);  // /2 = not
    buf.EmitByte(0xF7);
    buf.EmitByte(ModRM(3, 2, reg));
}

// ── Shift ─────────────────────────────────────────────────────────────────

/// shl r/m64, imm8
inline void EmitShlRI(CodeBuffer& buf, uint8_t reg, uint8_t imm) noexcept {
    EmitREX(buf, true, 4, reg);  // /4 = shl
    buf.EmitByte(0xC1);
    buf.EmitByte(ModRM(3, 4, reg));
    buf.EmitByte(imm);
}

/// shr r/m64, imm8
inline void EmitShrRI(CodeBuffer& buf, uint8_t reg, uint8_t imm) noexcept {
    EmitREX(buf, true, 5, reg);  // /5 = shr
    buf.EmitByte(0xC1);
    buf.EmitByte(ModRM(3, 5, reg));
    buf.EmitByte(imm);
}

/// sar r/m64, imm8 (arithmetic)
inline void EmitSarRI(CodeBuffer& buf, uint8_t reg, uint8_t imm) noexcept {
    EmitREX(buf, true, 7, reg);  // /7 = sar
    buf.EmitByte(0xC1);
    buf.EmitByte(ModRM(3, 7, reg));
    buf.EmitByte(imm);
}

/// shl r/m64, %cl
inline void EmitShlRCL(CodeBuffer& buf, uint8_t reg) noexcept {
    EmitREX(buf, true, 4, reg);
    buf.EmitByte(0xD3);
    buf.EmitByte(ModRM(3, 4, reg));
}

/// shr r/m64, %cl
inline void EmitShrRCL(CodeBuffer& buf, uint8_t reg) noexcept {
    EmitREX(buf, true, 5, reg);
    buf.EmitByte(0xD3);
    buf.EmitByte(ModRM(3, 5, reg));
}

/// sar r/m64, %cl
inline void EmitSarRCL(CodeBuffer& buf, uint8_t reg) noexcept {
    EmitREX(buf, true, 7, reg);
    buf.EmitByte(0xD3);
    buf.EmitByte(ModRM(3, 7, reg));
}

// ── Sign extension ───────────────────────────────────────────────────────

/// movsxd r64, r/m32 (sign-extend 32-bit to 64-bit)
inline void EmitMovsxd(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    EmitREX(buf, true, dst, src);
    buf.EmitByte(0x63);
    buf.EmitByte(ModRM(3, dst, src));
}

// ── Compare ──────────────────────────────────────────────────────────────

/// cmp r32, r32 (32-bit comparison, no REX.W)
inline void EmitCmp32RR(CodeBuffer& buf, uint8_t lhs, uint8_t rhs) noexcept {
    EmitREX(buf, false, lhs, rhs);
    buf.EmitByte(0x3B);
    buf.EmitByte(ModRM(3, lhs, rhs));
}

/// cmp r64, r/m64
inline void EmitCmpRR(CodeBuffer& buf, uint8_t lhs, uint8_t rhs) noexcept {
    EmitREX(buf, true, lhs, rhs);
    buf.EmitByte(0x3B);
    buf.EmitByte(ModRM(3, lhs, rhs));
}

/// cmp r/m64, imm32
inline void EmitCmpRI(CodeBuffer& buf, uint8_t reg, int32_t imm) noexcept {
    if (imm >= -128 && imm <= 127) {
        EmitREX(buf, true, 7, reg);
        buf.EmitByte(0x83);
        buf.EmitByte(ModRM(3, 7, reg));
        buf.EmitByte(static_cast<uint8_t>(imm));
    } else {
        EmitREX(buf, true, 7, reg);
        buf.EmitByte(0x81);
        buf.EmitByte(ModRM(3, 7, reg));
        buf.Emit32(static_cast<uint32_t>(imm));
    }
}

/// cmp r/m64, imm32 — memory form: [base+disp] vs imm32
inline void EmitCmpMI(CodeBuffer& buf, uint8_t base, int32_t disp, int32_t imm) noexcept {
    if (imm >= -128 && imm <= 127) {
        EmitREX(buf, true, 7, base);
        buf.EmitByte(0x83);
        EmitMR(buf, 7, base, disp);
        buf.EmitByte(static_cast<uint8_t>(imm));
    } else {
        EmitREX(buf, true, 7, base);
        buf.EmitByte(0x81);
        EmitMR(buf, 7, base, disp);
        buf.Emit32(static_cast<uint32_t>(imm));
    }
}

/// test r/m64, r64 (set flags)
inline void EmitTestRR(CodeBuffer& buf, uint8_t reg1, uint8_t reg2) noexcept {
    EmitREX(buf, true, reg1, reg2);
    buf.EmitByte(0x85);
    buf.EmitByte(ModRM(3, reg1, reg2));
}

// ── Conditional set byte ─────────────────────────────────────────────────

/// setcc r/m8 — table: 0=o,1=no,2=b/nae,3=nb/ae,4=e/z,5=ne/nz, ...
/// Use EmitSetcc(buf, 0, reg) for seto, 4 for sete, 5 for setne, etc.
inline void EmitSetcc(CodeBuffer& buf, uint8_t cc, uint8_t reg) noexcept {
    // Need REX for 8-bit reg access to SIL/DIL/BPL/SPL and r8-r15
    if (reg >= 4) {
        buf.EmitByte(REX(false, 0, 0, reg));
    }
    buf.EmitByte(0x0F);
    buf.EmitByte(static_cast<uint8_t>(0x90 + cc));
    buf.EmitByte(ModRM(3, 0, reg));
}

// ── Conditional move ─────────────────────────────────────────────────────

/// cmovcc r64, r/m64 — cc: 0=o,1=no,2=b,3=ae,4=e,5=ne,6=be,7=a,8=s,9=ns
///                             10=p,11=np,12=l,13=ge,14=le,15=g
inline void EmitCmovcc(CodeBuffer& buf, uint8_t cc, uint8_t dst, uint8_t src) noexcept {
    EmitREX(buf, true, dst, src);
    buf.EmitByte(0x0F);
    buf.EmitByte(static_cast<uint8_t>(0x40 + cc));
    buf.EmitByte(ModRM(3, dst, src));
}

// ── Jump instructions ─────────────────────────────────────────────────────

/// jmp rel32 (near jump, 5 bytes)
inline void EmitJmpRel32(CodeBuffer& buf, int32_t offset) noexcept {
    buf.EmitByte(0xE9);
    buf.Emit32(static_cast<uint32_t>(offset));
}

/// jmp rel8 (short jump, 2 bytes)
inline void EmitJmpRel8(CodeBuffer& buf, int8_t offset) noexcept {
    buf.EmitByte(0xEB);
    buf.EmitByte(static_cast<uint8_t>(offset));
}

/// jcc rel32 near (6 bytes: 0F 8x id32)
inline void EmitJccRel32(CodeBuffer& buf, uint8_t cc, int32_t offset) noexcept {
    buf.EmitByte(0x0F);
    buf.EmitByte(static_cast<uint8_t>(0x80 | cc));
    buf.Emit32(static_cast<uint32_t>(offset));
}

/// jcc rel8 short (2 bytes: 7x ib)
inline void EmitJccRel8(CodeBuffer& buf, uint8_t cc, int8_t offset) noexcept {
    buf.EmitByte(static_cast<uint8_t>(0x70 + cc));
    buf.EmitByte(static_cast<uint8_t>(offset));
}


// ── Call / Return ─────────────────────────────────────────────────────────

/// call rel32 (direct call)
inline void EmitCallRel32(CodeBuffer& buf, int32_t offset) noexcept {
    buf.EmitByte(0xE8);
    buf.Emit32(static_cast<uint32_t>(offset));
}

/// call r/m64 (indirect call via register)
inline void EmitCallReg(CodeBuffer& buf, uint8_t reg) noexcept {
    EmitREXB(buf, false, reg);
    buf.EmitByte(0xFF);
    buf.EmitByte(ModRM(3, 2, reg));
}

/// call [rip+disp32] — indirect call via slot table entry (6 bytes)
/// ModRM.mod=00 with ModRM.rm=101 encodes RIP-relative addressing in x64.
/// reg field = 2 (opcode extension for call).
inline void EmitCallRipRel(CodeBuffer& buf, int32_t disp) noexcept {
    buf.EmitByte(0xFF);               // opcode for call r/m64
    buf.EmitByte(ModRM(0, 2, 5));     // mod=00, reg=2(call), rm=101(RIP-rel)
    buf.Emit32(static_cast<uint32_t>(disp));
}

/// ret
inline void EmitRet(CodeBuffer& buf) noexcept {
    buf.EmitByte(0xC3);
}

/// jmp r/m64 — indirect jump via register (opcode FF /4)
inline void EmitJmpReg(CodeBuffer& buf, uint8_t reg) noexcept {
    EmitREXB(buf, false, reg);
    buf.EmitByte(0xFF);
    buf.EmitByte(ModRM(3, 4, reg));
}

/// push r/m64
inline void EmitPush(CodeBuffer& buf, uint8_t reg) noexcept {
    EmitREXB(buf, false, reg);
    buf.EmitByte(static_cast<uint8_t>(0x50 + (reg & 7)));
}

/// pop r/m64
inline void EmitPop(CodeBuffer& buf, uint8_t reg) noexcept {
    EmitREXB(buf, false, reg);
    buf.EmitByte(static_cast<uint8_t>(0x58 + (reg & 7)));
}

// ── Float / XMM ──────────────────────────────────────────────────────────

/// movsd xmm1, xmm2 (scalar double move, reg-to-reg)
inline void EmitMovSDRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0xF2);
    EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F);
    buf.EmitByte(0x10);
    buf.EmitByte(ModRM(3, dst, src));
}

/// movss xmm1, xmm2 (scalar float move)
inline void EmitMovSSRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0xF3);
    EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F);
    buf.EmitByte(0x10);
    buf.EmitByte(ModRM(3, dst, src));
}

/// movsd [mem], xmm (store double to memory)
inline void EmitMovSDMR(CodeBuffer& buf, uint8_t base, int32_t disp, uint8_t src) noexcept {
    buf.EmitByte(0xF2);
    EmitREX(buf, false, src, base);
    buf.EmitByte(0x0F);
    buf.EmitByte(0x11);
    EmitMR(buf, src, base, disp);
}

/// movsd xmm, [mem] (load double from memory)
inline void EmitMovSDRM(CodeBuffer& buf, uint8_t dst, uint8_t base, int32_t disp) noexcept {
    buf.EmitByte(0xF2);
    EmitREX(buf, false, dst, base);
    buf.EmitByte(0x0F);
    buf.EmitByte(0x10);
    EmitMR(buf, dst, base, disp);
}

/// movups [mem], xmm (store 16 bytes, unaligned)
inline void EmitMovUPSMR(CodeBuffer& buf, uint8_t base, int32_t disp, uint8_t src) noexcept {
    EmitREX(buf, false, src, base);
    buf.EmitByte(0x0F);
    buf.EmitByte(0x11);
    EmitMR(buf, src, base, disp);
}

/// movups xmm, [mem] (load 16 bytes, unaligned)
inline void EmitMovUPRM(CodeBuffer& buf, uint8_t dst, uint8_t base, int32_t disp) noexcept {
    EmitREX(buf, false, dst, base);
    buf.EmitByte(0x0F);
    buf.EmitByte(0x10);
    EmitMR(buf, dst, base, disp);
}

/// movss [mem], xmm (store float to memory)
inline void EmitMovSSMR(CodeBuffer& buf, uint8_t base, int32_t disp, uint8_t src) noexcept {
    buf.EmitByte(0xF3);
    EmitREX(buf, false, src, base);
    buf.EmitByte(0x0F);
    buf.EmitByte(0x11);
    EmitMR(buf, src, base, disp);
}

/// movss xmm, [mem] (load float from memory)
inline void EmitMovSSRM(CodeBuffer& buf, uint8_t dst, uint8_t base, int32_t disp) noexcept {
    buf.EmitByte(0xF3);
    EmitREX(buf, false, dst, base);
    buf.EmitByte(0x0F);
    buf.EmitByte(0x10);
    EmitMR(buf, dst, base, disp);
}

/// addsd xmm1, xmm2
inline void EmitAddSDRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0xF2);
    EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F);
    buf.EmitByte(0x58);
    buf.EmitByte(ModRM(3, dst, src));
}

/// subsd xmm1, xmm2
inline void EmitSubSDRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0xF2);
    EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F);
    buf.EmitByte(0x5C);
    buf.EmitByte(ModRM(3, dst, src));
}

/// mulsd xmm1, xmm2
inline void EmitMulSDRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0xF2);
    EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F);
    buf.EmitByte(0x59);
    buf.EmitByte(ModRM(3, dst, src));
}

/// divsd xmm1, xmm2
inline void EmitDivSDRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0xF2);
    EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F);
    buf.EmitByte(0x5E);
    buf.EmitByte(ModRM(3, dst, src));
}

/// addss xmm1, xmm2
inline void EmitAddSSRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0xF3);
    EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F);
    buf.EmitByte(0x58);
    buf.EmitByte(ModRM(3, dst, src));
}

/// subss xmm1, xmm2
inline void EmitSubSSRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0xF3);
    EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F);
    buf.EmitByte(0x5C);
    buf.EmitByte(ModRM(3, dst, src));
}

/// mulss xmm1, xmm2
inline void EmitMulSSRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0xF3);
    EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F);
    buf.EmitByte(0x59);
    buf.EmitByte(ModRM(3, dst, src));
}

/// divss xmm1, xmm2
inline void EmitDivSSRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0xF3);
    EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F);
    buf.EmitByte(0x5E);
    buf.EmitByte(ModRM(3, dst, src));
}

/// cvtsi2sd xmm, r/m64 (convert int64 → double)
inline void EmitCvtsi2sd(CodeBuffer& buf, uint8_t xmm, uint8_t reg) noexcept {
    buf.EmitByte(0xF2);
    EmitREX(buf, true, xmm, reg);
    buf.EmitByte(0x0F);
    buf.EmitByte(0x2A);
    buf.EmitByte(ModRM(3, xmm, reg));
}

/// cvtsi2ss xmm, r/m64 (convert int64 → float)
inline void EmitCvtsi2ss(CodeBuffer& buf, uint8_t xmm, uint8_t reg) noexcept {
    buf.EmitByte(0xF3);
    EmitREX(buf, true, xmm, reg);
    buf.EmitByte(0x0F);
    buf.EmitByte(0x2A);
    buf.EmitByte(ModRM(3, xmm, reg));
}

/// cvttsd2si r64, xmm (truncate double → int64)
inline void EmitCvttsd2si(CodeBuffer& buf, uint8_t reg, uint8_t xmm) noexcept {
    buf.EmitByte(0xF2);
    EmitREX(buf, true, reg, xmm);
    buf.EmitByte(0x0F);
    buf.EmitByte(0x2C);
    buf.EmitByte(ModRM(3, reg, xmm));
}

/// cvttss2si r64, xmm (truncate float → int64)
inline void EmitCvttss2si(CodeBuffer& buf, uint8_t reg, uint8_t xmm) noexcept {
    buf.EmitByte(0xF3);
    EmitREX(buf, true, reg, xmm);
    buf.EmitByte(0x0F);
    buf.EmitByte(0x2C);
    buf.EmitByte(ModRM(3, reg, xmm));
}

/// cvtsd2ss xmm, xmm (convert double → float)
inline void EmitCvtsd2ss(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0xF2);
    EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F);
    buf.EmitByte(0x5A);
    buf.EmitByte(ModRM(3, dst, src));
}

/// cvtss2sd xmm, xmm (convert float → double)
inline void EmitCvtss2sd(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0xF3);
    EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F);
    buf.EmitByte(0x5A);
    buf.EmitByte(ModRM(3, dst, src));
}

/// ucomisd xmm1, xmm2 (unordered compare double, set flags)
inline void EmitUcomisd(CodeBuffer& buf, uint8_t lhs, uint8_t rhs) noexcept {
    buf.EmitByte(0x66);
    EmitREX(buf, false, lhs, rhs);
    buf.EmitByte(0x0F);
    buf.EmitByte(0x2E);
    buf.EmitByte(ModRM(3, lhs, rhs));
}

/// ucomiss xmm1, xmm2 (unordered compare float, set flags)
inline void EmitUcomiss(CodeBuffer& buf, uint8_t lhs, uint8_t rhs) noexcept {
    EmitREX(buf, false, lhs, rhs);
    buf.EmitByte(0x0F);
    buf.EmitByte(0x2E);
    buf.EmitByte(ModRM(3, lhs, rhs));
}

/// comisd xmm1, xmm2 (ordered compare double, set flags — signals QNaN)
inline void EmitComisd(CodeBuffer& buf, uint8_t lhs, uint8_t rhs) noexcept {
    buf.EmitByte(0x66);
    EmitREX(buf, false, lhs, rhs);
    buf.EmitByte(0x0F);
    buf.EmitByte(0x2F);
    buf.EmitByte(ModRM(3, lhs, rhs));
}

/// movd xmm, r/m32 (move 32-bit GPR to XMM lower)
inline void EmitMovdXrm(CodeBuffer& buf, uint8_t xmm, uint8_t reg) noexcept {
    buf.EmitByte(0x66);
    EmitREX(buf, false, xmm, reg);
    buf.EmitByte(0x0F);
    buf.EmitByte(0x6E);
    buf.EmitByte(ModRM(3, xmm, reg));
}

/// movd r/m32, xmm (move XMM lower to 32-bit GPR)
inline void EmitMovdMrx(CodeBuffer& buf, uint8_t reg, uint8_t xmm) noexcept {
    buf.EmitByte(0x66);
    EmitREX(buf, false, reg, xmm);
    buf.EmitByte(0x0F);
    buf.EmitByte(0x7E);
    buf.EmitByte(ModRM(3, reg, xmm));
}

/// movq xmm, r/m64 (move 64-bit GPR to XMM)
inline void EmitMovqXrm(CodeBuffer& buf, uint8_t xmm, uint8_t reg) noexcept {
    buf.EmitByte(0xF3);
    EmitREX(buf, false, xmm, reg);
    buf.EmitByte(0x0F);
    buf.EmitByte(0x7E);
    buf.EmitByte(ModRM(3, xmm, reg));
}

/// movq r/m64, xmm (move XMM to 64-bit GPR)
inline void EmitMovqMrx(CodeBuffer& buf, uint8_t reg, uint8_t xmm) noexcept {
    buf.EmitByte(0x66);
    EmitREX(buf, false, reg, xmm);
    buf.EmitByte(0x0F);
    buf.EmitByte(0x7E);
    buf.EmitByte(ModRM(3, reg, xmm));
}

/// pxor xmm1, xmm2 (XOR — zero XMM register)
inline void EmitPxorRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66);
    EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F);
    buf.EmitByte(0xEF);
    buf.EmitByte(ModRM(3, dst, src));
}

/// xorps xmm1, xmm2 (another way to zero XMM)
inline void EmitXorpsRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F);
    buf.EmitByte(0x57);
    buf.EmitByte(ModRM(3, dst, src));
}

// ═══════════════════════════════════════════════════════════════════════════
// SSE2 packed integer arithmetic (Padd*, Psub*, Pmul*)
// ═══════════════════════════════════════════════════════════════════════════

/// paddb xmm1, xmm2 (packed byte add)
inline void EmitPaddbRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0xFC);
    buf.EmitByte(ModRM(3, dst, src));
}

/// paddw xmm1, xmm2 (packed 16-bit add)
inline void EmitPaddwRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0xFD);
    buf.EmitByte(ModRM(3, dst, src));
}

/// paddd xmm1, xmm2 (packed 32-bit add)
inline void EmitPadddRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0xFE);
    buf.EmitByte(ModRM(3, dst, src));
}

/// paddq xmm1, xmm2 (packed 64-bit add)
inline void EmitPaddqRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0xD4);
    buf.EmitByte(ModRM(3, dst, src));
}

/// psubb xmm1, xmm2 (packed byte subtract)
inline void EmitPsubbRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0xF8);
    buf.EmitByte(ModRM(3, dst, src));
}

/// psubw xmm1, xmm2 (packed 16-bit subtract)
inline void EmitPsubwRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0xF9);
    buf.EmitByte(ModRM(3, dst, src));
}

/// psubd xmm1, xmm2 (packed 32-bit subtract)
inline void EmitPsubdRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0xFA);
    buf.EmitByte(ModRM(3, dst, src));
}

/// psubq xmm1, xmm2 (packed 64-bit subtract)
inline void EmitPsubqRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0xFB);
    buf.EmitByte(ModRM(3, dst, src));
}

/// pmullw xmm1, xmm2 (packed 16-bit multiply, low)
inline void EmitPmullwRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0xD5);
    buf.EmitByte(ModRM(3, dst, src));
}

/// pmuludq xmm1, xmm2 (packed 32-bit → 64-bit unsigned multiply)
inline void EmitPmuludqRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0xF4);
    buf.EmitByte(ModRM(3, dst, src));
}

/// pmaddwd xmm1, xmm2 (packed multiply-add, word→dword)
inline void EmitPmaddwdRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0xF5);
    buf.EmitByte(ModRM(3, dst, src));
}

// ═══════════════════════════════════════════════════════════════════════════
// SSE2 packed bitwise (Pand, Por, Pandn)
// ═══════════════════════════════════════════════════════════════════════════

/// pand xmm1, xmm2 (packed bitwise AND)
inline void EmitPandRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0xDB);
    buf.EmitByte(ModRM(3, dst, src));
}

/// por xmm1, xmm2 (packed bitwise OR)
inline void EmitPorRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0xEB);
    buf.EmitByte(ModRM(3, dst, src));
}

/// pandn xmm1, xmm2 (packed AND NOT: dst = ~dst & src)
inline void EmitPandnRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0xDF);
    buf.EmitByte(ModRM(3, dst, src));
}

// ═══════════════════════════════════════════════════════════════════════════
// SSE2/SSE4.1 packed compare (Pcmpeq*, Pcmpgt*)
// ═══════════════════════════════════════════════════════════════════════════

/// pcmpeqb xmm1, xmm2 (packed byte compare equal)
inline void EmitPcmpeqbRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0x74);
    buf.EmitByte(ModRM(3, dst, src));
}

/// pcmpeqw xmm1, xmm2 (packed 16-bit compare equal)
inline void EmitPcmpeqwRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0x75);
    buf.EmitByte(ModRM(3, dst, src));
}

/// pcmpeqd xmm1, xmm2 (packed 32-bit compare equal)
inline void EmitPcmpeqdRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0x76);
    buf.EmitByte(ModRM(3, dst, src));
}

/// pcmpeqq xmm1, xmm2 (packed 64-bit compare equal, SSE4.1)
inline void EmitPcmpeqqRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0x38); buf.EmitByte(0x29);
    buf.EmitByte(ModRM(3, dst, src));
}

/// pcmpgtb xmm1, xmm2 (packed byte signed compare greater)
inline void EmitPcmpgtbRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0x64);
    buf.EmitByte(ModRM(3, dst, src));
}

/// pcmpgtw xmm1, xmm2 (packed 16-bit signed compare greater)
inline void EmitPcmpgtwRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0x65);
    buf.EmitByte(ModRM(3, dst, src));
}

/// pcmpgtd xmm1, xmm2 (packed 32-bit signed compare greater)
inline void EmitPcmpgtdRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0x66);
    buf.EmitByte(ModRM(3, dst, src));
}

/// pcmpgtq xmm1, xmm2 (packed 64-bit signed compare greater, SSE4.1)
inline void EmitPcmpgtqRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0x38); buf.EmitByte(0x37);
    buf.EmitByte(ModRM(3, dst, src));
}

// ═══════════════════════════════════════════════════════════════════════════
// SSE2/SSSE3 packed shift
// ═══════════════════════════════════════════════════════════════════════════

/// psllw xmm1, xmm2 (packed 16-bit shift left logical, by count in xmm2)
inline void EmitPsllwRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0xF1);
    buf.EmitByte(ModRM(3, dst, src));
}

/// pslld xmm1, xmm2 (packed 32-bit shift left logical)
inline void EmitPslldRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0xF2);
    buf.EmitByte(ModRM(3, dst, src));
}

/// psllq xmm1, xmm2 (packed 64-bit shift left logical)
inline void EmitPsllqRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0xF3);
    buf.EmitByte(ModRM(3, dst, src));
}

/// psrlw xmm1, xmm2 (packed 16-bit shift right logical)
inline void EmitPsrlwRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0xD1);
    buf.EmitByte(ModRM(3, dst, src));
}

/// psrld xmm1, xmm2 (packed 32-bit shift right logical)
inline void EmitPsrldRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0xD2);
    buf.EmitByte(ModRM(3, dst, src));
}

/// psrlq xmm1, xmm2 (packed 64-bit shift right logical)
inline void EmitPsrlqRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0xD3);
    buf.EmitByte(ModRM(3, dst, src));
}

/// psraw xmm1, xmm2 (packed 16-bit shift right arithmetic)
inline void EmitPsrawRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0xE1);
    buf.EmitByte(ModRM(3, dst, src));
}

/// psrad xmm1, xmm2 (packed 32-bit shift right arithmetic)
inline void EmitPsradRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0xE2);
    buf.EmitByte(ModRM(3, dst, src));
}

// ═══════════════════════════════════════════════════════════════════════════
// SSE2/SSSE3/SSE4.1 unpack & pack
// ═══════════════════════════════════════════════════════════════════════════

/// punpcklbw xmm1, xmm2 (unpack low bytes)
inline void EmitPunpcklbwRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0x60);
    buf.EmitByte(ModRM(3, dst, src));
}

/// punpcklwd xmm1, xmm2 (unpack low 16-bit)
inline void EmitPunpcklwdRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0x61);
    buf.EmitByte(ModRM(3, dst, src));
}

/// punpckldq xmm1, xmm2 (unpack low 32-bit)
inline void EmitPunpckldqRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0x62);
    buf.EmitByte(ModRM(3, dst, src));
}

/// punpcklqdq xmm1, xmm2 (unpack low 64-bit)
inline void EmitPunpcklqdqRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0x6C);
    buf.EmitByte(ModRM(3, dst, src));
}

/// punpckhbw xmm1, xmm2 (unpack high bytes)
inline void EmitPunpckhbwRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0x68);
    buf.EmitByte(ModRM(3, dst, src));
}

/// punpckhwd xmm1, xmm2 (unpack high 16-bit)
inline void EmitPunpckhwdRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0x69);
    buf.EmitByte(ModRM(3, dst, src));
}

/// punpckhdq xmm1, xmm2 (unpack high 32-bit)
inline void EmitPunpckhdqRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0x6A);
    buf.EmitByte(ModRM(3, dst, src));
}

/// punpckhqdq xmm1, xmm2 (unpack high 64-bit)
inline void EmitPunpckhqdqRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0x6D);
    buf.EmitByte(ModRM(3, dst, src));
}

/// packsswb xmm1, xmm2 (pack signed 16-bit → byte with signed saturation)
inline void EmitPacksswbRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0x63);
    buf.EmitByte(ModRM(3, dst, src));
}

/// packssdw xmm1, xmm2 (pack signed 32-bit → 16-bit with signed saturation)
inline void EmitPackssdwRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0x6B);
    buf.EmitByte(ModRM(3, dst, src));
}

/// packuswb xmm1, xmm2 (pack signed 16-bit → byte with unsigned saturation)
inline void EmitPackuswbRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0x67);
    buf.EmitByte(ModRM(3, dst, src));
}

// ═══════════════════════════════════════════════════════════════════════════
// SSE2/SSSE3/SSE4.1 shuffle & absolute value
// ═══════════════════════════════════════════════════════════════════════════

/// pshufd xmm1, xmm2, imm8 (packed 32-bit shuffle)
inline void EmitPshufdRR(CodeBuffer& buf, uint8_t dst, uint8_t src, uint8_t imm) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0x70);
    buf.EmitByte(ModRM(3, dst, src));
    buf.EmitByte(imm);
}

/// pshufb xmm1, xmm2 (packed shuffle bytes, SSSE3)
inline void EmitPshufbRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0x38); buf.EmitByte(0x00);
    buf.EmitByte(ModRM(3, dst, src));
}

/// pabsb xmm1, xmm2 (packed byte absolute value, SSSE3)
inline void EmitPabsbRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0x38); buf.EmitByte(0x1C);
    buf.EmitByte(ModRM(3, dst, src));
}

/// pabsw xmm1, xmm2 (packed 16-bit absolute value, SSSE3)
inline void EmitPabswRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0x38); buf.EmitByte(0x1D);
    buf.EmitByte(ModRM(3, dst, src));
}

/// pabsd xmm1, xmm2 (packed 32-bit absolute value, SSSE3)
inline void EmitPabsdRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0x38); buf.EmitByte(0x1E);
    buf.EmitByte(ModRM(3, dst, src));
}

// ═══════════════════════════════════════════════════════════════════════════
// SSE2/SSE4.1 insert & extract (GPR ↔ XMM)
// ═══════════════════════════════════════════════════════════════════════════

/// pinsrw xmm, r32, imm8 (insert 16-bit from GPR, SSE2)
inline void EmitPinsrwRR(CodeBuffer& buf, uint8_t xmm, uint8_t reg, uint8_t imm) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, xmm, reg);
    buf.EmitByte(0x0F); buf.EmitByte(0xC4);
    buf.EmitByte(ModRM(3, xmm, reg));
    buf.EmitByte(imm);
}

/// pextrw r32, xmm, imm8 (extract 16-bit to GPR, SSE2)
inline void EmitPextrwRR(CodeBuffer& buf, uint8_t reg, uint8_t xmm, uint8_t imm) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, reg, xmm);
    buf.EmitByte(0x0F); buf.EmitByte(0xC5);
    buf.EmitByte(ModRM(3, reg, xmm));
    buf.EmitByte(imm);
}

/// pinsrb xmm, r32, imm8 (insert byte from GPR, SSE4.1)
inline void EmitPinsrbRR(CodeBuffer& buf, uint8_t xmm, uint8_t reg, uint8_t imm) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, xmm, reg);
    buf.EmitByte(0x0F); buf.EmitByte(0x3A); buf.EmitByte(0x20);
    buf.EmitByte(ModRM(3, xmm, reg));
    buf.EmitByte(imm);
}

/// pinsrd xmm, r32, imm8 (insert 32-bit from GPR, SSE4.1)
inline void EmitPinsrdRR(CodeBuffer& buf, uint8_t xmm, uint8_t reg, uint8_t imm) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, xmm, reg);
    buf.EmitByte(0x0F); buf.EmitByte(0x3A); buf.EmitByte(0x22);
    buf.EmitByte(ModRM(3, xmm, reg));
    buf.EmitByte(imm);
}

/// pextrb r32, xmm, imm8 (extract byte to GPR, SSE4.1)
inline void EmitPextrbRR(CodeBuffer& buf, uint8_t reg, uint8_t xmm, uint8_t imm) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, reg, xmm);
    buf.EmitByte(0x0F); buf.EmitByte(0x3A); buf.EmitByte(0x14);
    buf.EmitByte(ModRM(3, reg, xmm));
    buf.EmitByte(imm);
}

/// pextrd r32, xmm, imm8 (extract 32-bit to GPR, SSE4.1)
inline void EmitPextrdRR(CodeBuffer& buf, uint8_t reg, uint8_t xmm, uint8_t imm) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, reg, xmm);
    buf.EmitByte(0x0F); buf.EmitByte(0x3A); buf.EmitByte(0x16);
    buf.EmitByte(ModRM(3, reg, xmm));
    buf.EmitByte(imm);
}

/// pmovmskb r32, xmm (move byte mask to GPR, SSE2)
inline void EmitPmovmskbRR(CodeBuffer& buf, uint8_t reg, uint8_t xmm) noexcept {
    EmitREX(buf, false, reg, xmm);
    buf.EmitByte(0x0F); buf.EmitByte(0xD7);
    buf.EmitByte(ModRM(3, reg, xmm));
}

// ═══════════════════════════════════════════════════════════════════════════
// SSE2 aligned 128-bit moves (Movdqa) — for explicit alignment scenarios
// ═══════════════════════════════════════════════════════════════════════════

/// movdqa xmm1, xmm2 (aligned 128-bit register-to-register)
inline void EmitMovdqaRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0x6F);
    buf.EmitByte(ModRM(3, dst, src));
}

/// movdqa [mem], xmm (aligned 128-bit store)
inline void EmitMovdqaMR(CodeBuffer& buf, uint8_t base, int32_t disp, uint8_t src) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, src, base);
    buf.EmitByte(0x0F); buf.EmitByte(0x7F);
    EmitMR(buf, src, base, disp);
}

/// movdqa xmm, [mem] (aligned 128-bit load)
inline void EmitMovdqaRM(CodeBuffer& buf, uint8_t dst, uint8_t base, int32_t disp) noexcept {
    buf.EmitByte(0x66); EmitREX(buf, false, dst, base);
    buf.EmitByte(0x0F); buf.EmitByte(0x6F);
    EmitMR(buf, dst, base, disp);
}

// ═══════════════════════════════════════════════════════════════════════════
// POPCNT / LZCNT (bit manipulation)
// ═══════════════════════════════════════════════════════════════════════════

/// popcnt r64, r/m64 (population count)
inline void EmitPopcntRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0xF3); EmitREX(buf, true, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0xB8);
    buf.EmitByte(ModRM(3, dst, src));
}

/// lzcnt r64, r/m64 (leading zero count)
inline void EmitLzcntRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0xF3); EmitREX(buf, true, dst, src);
    buf.EmitByte(0x0F); buf.EmitByte(0xBD);
    buf.EmitByte(ModRM(3, dst, src));
}

}  // namespace chaos::il2cpp::jit

#endif  // CHAOS_IL2CPP_CODEGEN_X64_ENCODER_H_
