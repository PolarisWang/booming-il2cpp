// jit_codegen_arith.cpp — integer/floating arithmetic, bitwise, and shift
// emission for NativeCodeGenerator (T2.4 module split of jit_engine.cpp).
//
// Pure move: these method definitions were relocated verbatim from
// jit_engine.cpp; no logic changed.  They read the NativeCodeGenerator
// member state (buf_, enc_, gcr_, has_graph_coloring_) declared in
// jit_engine.h.

#include "jit_engine.h"
#include "ArchTraits.h"    // AT::{kScratchA,kScratchB,kScratchC}
#include "jit_reg_alloc.h" // gcr_ (GraphColoringResult)
#include "x64_encoder.h"
#include "IEncoder.h"
#if defined(__aarch64__)
#include "arm64_encoder.h"
#include "Arm64Encoder.h"
#else
#include "X64Encoder.h"
#endif

namespace chaos::il2cpp::jit {

void NativeCodeGenerator::EmitIntegerArithmetic(IROpCode opc, uint32_t dst, uint32_t src1, uint32_t src2) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::EmitIntegerArithmetic");
    // Div/Rem have implicit eax/edx/ecx register requirements.
    bool has_implicit =
        (opc == IROpCode::Div || opc == IROpCode::Rem || opc == IROpCode::DivUn || opc == IROpCode::RemUn);
    uint8_t op_reg = AT::kScratchA;
    uint8_t src2_reg = AT::kScratchB;
    if (has_graph_coloring_ && !has_implicit) {
        if (dst < interpreter::kGPRegisters) {
            uint8_t c = gcr_.gpr_color[dst];
            if (c != 0xFF)
                op_reg = c;
        }
        if (src2 != UINT32_MAX && src2 < interpreter::kGPRegisters) {
            uint8_t c = gcr_.gpr_color[src2];
            if (c != 0xFF)
                src2_reg = c;
        }
    }
    // When op_reg and src2_reg share the same colored register (dst and src2
    // assigned the same x64 register by graph coloring) and src1 != src2,
    // loading src1 into op_reg destroys src2's value.  Load src2 into a
    // scratch register FIRST, then load src1, so the computation reads the
    // correct src2 value even after op_reg overwrites the shared register.
    bool src2_loaded = false;
    if (has_graph_coloring_ && !has_implicit && op_reg == src2_reg && src1 != src2) {
        src2_reg = (op_reg == AT::kScratchB) ? AT::kScratchA : AT::kScratchB;
        LoadGpr(src2_reg, src2);
        src2_loaded = true;
    }
    LoadGpr(op_reg, src1);
    if (src2 != UINT32_MAX && !src2_loaded)
        LoadGpr(src2_reg, src2);
    if (opc == IROpCode::Add) {
        enc_.EmitAdd32RR(op_reg, src2_reg);
    } else if (opc == IROpCode::Sub) {
        enc_.EmitSub32RR(op_reg, src2_reg);
    } else if (opc == IROpCode::Mul) {
        enc_.EmitImul32RR(op_reg, src2_reg);
    } else if (opc == IROpCode::Neg) {
#if defined(__aarch64__)
        // ARM64: 32-bit NEG (NEG Wd,Wm) doesn't zero-extend upper 32 bits of Xd,
        // unlike x64 where 32-bit register destination always zero-extends to 64 bits.
        // Use 64-bit NEG so the full register holds the sign-extended result.
        enc_.EmitNeg(op_reg);
#else
        enc_.EmitNeg32(op_reg);
#endif
    } else if (opc == IROpCode::Div || opc == IROpCode::Rem) {
#if defined(__aarch64__)
        // ARM64: SDIV 32-bit signed.  WscratchA = dividend, WscratchB = divisor (pre-loaded).
        enc_.EmitMovRR(AT::kScratchC, AT::kScratchA); // save dividend
        ::chaos::il2cpp::jit::EmitSdiv32(buf_, AT::kScratchA, AT::kScratchA,
                                         AT::kScratchB); // quotient = dividend / divisor
        if (opc == IROpCode::Rem) {
            ::chaos::il2cpp::jit::EmitMul32(buf_, AT::kScratchB, AT::kScratchA,
                                            AT::kScratchB); // WscratchB = quotient * divisor
            enc_.EmitSub32RR(AT::kScratchC, AT::kScratchB); // remainder = original_dividend - quotient*divisor
            enc_.EmitMovRR(op_reg, AT::kScratchC);
        }
#else
        EmitREXB(buf_, false, 0);
        buf_.EmitByte(0x99); // cdq: sign-extend eax→edx:eax
        EmitREX(buf_, false, 7, AT::kScratchB);
        buf_.EmitByte(0xF7);
        buf_.EmitByte(ModRM(3, 7, AT::kScratchB)); // idiv ecx
        if (opc == IROpCode::Rem)
            enc_.EmitMovRR(op_reg, AT::kScratchC);
#endif
    } else if (opc == IROpCode::DivUn || opc == IROpCode::RemUn) {
#if defined(__aarch64__)
        // ARM64: UDIV 32-bit unsigned
        enc_.EmitMovRR(AT::kScratchC, AT::kScratchA); // save dividend
        ::chaos::il2cpp::jit::EmitUdiv32(buf_, AT::kScratchA, AT::kScratchA,
                                         AT::kScratchB); // quotient = dividend / divisor
        if (opc == IROpCode::RemUn) {
            ::chaos::il2cpp::jit::EmitMul32(buf_, AT::kScratchB, AT::kScratchA,
                                            AT::kScratchB); // WscratchB = quotient * divisor
            enc_.EmitSub32RR(AT::kScratchC, AT::kScratchB); // remainder = original_dividend - quotient*divisor
            enc_.EmitMovRR(op_reg, AT::kScratchC);
        }
#else
        enc_.EmitXor32ZR(AT::kScratchC); // xor edx, edx
        EmitREX(buf_, false, 6, AT::kScratchB);
        buf_.EmitByte(0xF7);
        buf_.EmitByte(ModRM(3, 6, AT::kScratchB)); // div ecx
        if (opc == IROpCode::RemUn)
            enc_.EmitMovRR(op_reg, AT::kScratchC);
#endif
    }
    StoreGpr(op_reg, dst);
}

void NativeCodeGenerator::EmitFloatingArithmetic(IROpCode opc, uint32_t dst, uint32_t src1, uint32_t src2) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::EmitFloatingArithmetic");
    // Pick the working XMM register: prefer src1's colored reg, else XMM0.
    uint8_t op_xmm = 0;
    if (has_graph_coloring_ && src1 >= kGprCount) {
        uint32_t fi = src1 - kGprCount;
        if (fi < 32 && gcr_.fpr_color[fi] != 0xFF)
            op_xmm = gcr_.fpr_color[fi];
    }
    LoadFpr(op_xmm, src1);
    uint8_t src2_xmm = 1;
    if (src2 != UINT32_MAX && src2 >= kGprCount) {
        if (has_graph_coloring_) {
            uint32_t fi = src2 - kGprCount;
            if (fi < 32 && gcr_.fpr_color[fi] != 0xFF)
                src2_xmm = gcr_.fpr_color[fi];
        }
        LoadFpr(src2_xmm, src2);
    }
    if (opc == IROpCode::Add)
        enc_.EmitAddSDRR(op_xmm, src2_xmm);
    else if (opc == IROpCode::Sub)
        enc_.EmitSubSDRR(op_xmm, src2_xmm);
    else if (opc == IROpCode::Mul)
        enc_.EmitMulSDRR(op_xmm, src2_xmm);
    else if (opc == IROpCode::Div)
        enc_.EmitDivSDRR(op_xmm, src2_xmm);
    StoreFpr(op_xmm, dst);
}

void NativeCodeGenerator::EmitBitwise(IROpCode opc, uint32_t dst, uint32_t src1, uint32_t src2) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::EmitBitwise");
    uint8_t op_reg = AT::kScratchA;
    uint8_t src2_reg = AT::kScratchB;
    if (has_graph_coloring_) {
        if (dst < interpreter::kGPRegisters) {
            uint8_t c = gcr_.gpr_color[dst];
            if (c != 0xFF)
                op_reg = c;
        }
        if (src2 != UINT32_MAX && src2 < interpreter::kGPRegisters) {
            uint8_t c = gcr_.gpr_color[src2];
            if (c != 0xFF)
                src2_reg = c;
        }
    }
    // Same collision guard as EmitIntegerArithmetic: when op_reg and src2_reg
    // share a color and src1 != src2, load src2 into a scratch register first.
    bool src2_loaded = false;
    if (has_graph_coloring_ && op_reg == src2_reg && src1 != src2) {
        src2_reg = (op_reg == AT::kScratchB) ? AT::kScratchA : AT::kScratchB;
        LoadGpr(src2_reg, src2);
        src2_loaded = true;
    }
    LoadGpr(op_reg, src1);
    if (opc == IROpCode::Not)
        enc_.EmitNot32(op_reg);
    else if (opc == IROpCode::And) {
        if (src2 != UINT32_MAX && !src2_loaded) {
            LoadGpr(src2_reg, src2);
        }
        enc_.EmitAnd32RR(op_reg, src2_reg);
    } else if (opc == IROpCode::Or) {
        if (src2 != UINT32_MAX && !src2_loaded) {
            LoadGpr(src2_reg, src2);
        }
        enc_.EmitOr32RR(op_reg, src2_reg);
    } else if (opc == IROpCode::Xor) {
        if (src2 != UINT32_MAX && !src2_loaded) {
            LoadGpr(src2_reg, src2);
        }
        enc_.EmitXor32RR(op_reg, src2_reg);
    }
    StoreGpr(op_reg, dst);
}

// ── Shift with proper 32-bit semantics ─────────────────────────────────
// RegisterExecute uses int32_t/uint32_t for shift operations, which means:
//   Shr (signed):   (int32_t)RAX >> CL  → sign-extend to 64-bit
//   ShrUn (unsigned): (uint32_t)RAX >> CL → zero-extend to 64-bit
// x64 32-bit ops automatically zero-extend to 64 bits.
void NativeCodeGenerator::EmitShift(IROpCode opc, uint32_t dst, uint32_t src1, uint32_t src2, int32_t imm) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::EmitShift");
    // For variable shifts (src2 present), shift count must be in CL (AT::kScratchB),
    // but the destination can be any register. For immediate shifts, any GPR works.
    uint8_t op_reg = AT::kScratchA;
    if (has_graph_coloring_ && dst < interpreter::kGPRegisters) {
        uint8_t c = gcr_.gpr_color[dst];
        if (c != 0xFF)
            op_reg = c;
    }
    // Collision guard (mirrors EmitIntegerArithmetic): a variable shift computes
    // "dst = src1 << src2", where src2 is the shift-count vreg that must land
    // in CL (AT::kScratchB).  When src2's graph color equals op_reg
    // (e.g. in-place "dst = src1 << dst", where dst and src2 are the same vreg
    // and therefore share a color), loading src1 into op_reg first would
    // clobber src2's value before the shift count is captured into CL.  Load
    // the shift count into CL FIRST, then load src1 into op_reg, so the count
    // is read from the true src2 value, not from the just-loaded src1.
    bool load_src1_first = true;
    if (has_graph_coloring_ && src2 != UINT32_MAX && src2 < interpreter::kGPRegisters) {
        uint8_t src2_color = gcr_.gpr_color[src2];
        if (src2_color != 0xFF && src2_color == op_reg)
            load_src1_first = false;
    }
#if defined(__aarch64__)
    if (src2 != UINT32_MAX) {
        if (!load_src1_first)
            LoadGpr(AT::kScratchB, src2);
        LoadGpr(op_reg, src1);
        if (load_src1_first)
            LoadGpr(AT::kScratchB, src2);
        if (opc == IROpCode::Shl) {
            enc_.EmitShlRCL(op_reg);
        } else if (opc == IROpCode::Shr) {
            enc_.EmitSarRCL(op_reg); // signed → arithmetic
        } else if (opc == IROpCode::ShrUn) {
            enc_.EmitShrRCL(op_reg); // unsigned → logical
        }
    } else {
        uint8_t shift = static_cast<uint8_t>(imm & 0x1F);
        if (opc == IROpCode::Shl) {
            enc_.EmitShlRI(op_reg, shift);
        } else if (opc == IROpCode::Shr) {
            enc_.EmitSarRI(op_reg, shift);
        } else if (opc == IROpCode::ShrUn) {
            enc_.EmitShrRI(op_reg, shift);
        }
    }
#else
    if (src2 != UINT32_MAX) {
        if (!load_src1_first)
            LoadGpr(AT::kScratchB, src2);
        LoadGpr(op_reg, src1);
        if (load_src1_first)
            LoadGpr(AT::kScratchB, src2);
        EmitREXB(buf_, false, op_reg); // REX.B for extended destination register
        if (opc == IROpCode::Shl) {
            buf_.EmitByte(0xD3);
            buf_.EmitByte(ModRM(3, 4, op_reg));
        } else if (opc == IROpCode::Shr) {
            buf_.EmitByte(0xD3);
            buf_.EmitByte(ModRM(3, 7, op_reg));
        } else if (opc == IROpCode::ShrUn) {
            buf_.EmitByte(0xD3);
            buf_.EmitByte(ModRM(3, 5, op_reg));
        }
    } else {
        uint8_t shift = static_cast<uint8_t>(imm & 0x1F);
        if (opc == IROpCode::Shl) {
            EmitREX(buf_, false, 4, op_reg);
            buf_.EmitByte(0xC1);
            buf_.EmitByte(ModRM(3, 4, op_reg));
            buf_.EmitByte(shift);
        } else if (opc == IROpCode::Shr) {
            EmitREX(buf_, false, 7, op_reg);
            buf_.EmitByte(0xC1);
            buf_.EmitByte(ModRM(3, 7, op_reg));
            buf_.EmitByte(shift);
        } else if (opc == IROpCode::ShrUn) {
            EmitREX(buf_, false, 5, op_reg);
            buf_.EmitByte(0xC1);
            buf_.EmitByte(ModRM(3, 5, op_reg));
            buf_.EmitByte(shift);
        }
    }
#endif
    StoreGpr(op_reg, dst);
}
} // namespace chaos::il2cpp::jit
