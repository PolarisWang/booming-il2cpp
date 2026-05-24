#ifndef CHAOS_IL2CPP_ARM64ENCODER_H_
#define CHAOS_IL2CPP_ARM64ENCODER_H_

// ── ARM64 encoder (IEncoder implementation) ─────────────────────────────────
//
// Wraps the arm64_encoder.h free-function API behind the IEncoder interface.
// Every pure virtual method forwards to the corresponding EmitXxx(buf_, ...)
// call in arm64_encoder.h.
//
// ARM64 lacks several x64 instruction categories (memory-destination ALU ops,
// PUSH/POP, etc.).  These are emulated as multi-instruction sequences.

#include "IEncoder.h"
#include "arm64_encoder.h"
#include "code_buffer.h"

namespace chaos::il2cpp::jit {

class Arm64Encoder : public IEncoder {
public:
    explicit Arm64Encoder(CodeBuffer& buf) : buf_(buf) {}

    // ── MOV ──────────────────────────────────────────────────────────────
    void EmitMovRR(uint8_t dst, uint8_t src) override {
        // MOV Xd, Xm = ORR Xd, XZR, Xm
        EmitOrr64(buf_, dst, 31, src);
    }
    void EmitMovRM(uint8_t dst, uint8_t base, int32_t disp) override {
        EmitLdr64(buf_, dst, base, static_cast<uint16_t>(disp / 8));
    }
    void EmitMovMR(uint8_t base, int32_t disp, uint8_t src) override {
        EmitStr64(buf_, src, base, static_cast<uint16_t>(disp / 8));
    }
    void EmitMovImm64(uint8_t dst, uint64_t imm) override {
        EmitLoadImm64(buf_, dst, imm);
    }
    void EmitMovRI32(uint8_t dst, int32_t imm) override {
        // Sign-extend to 64-bit via MOVZ, or use ORR for small immediates.
        EmitLoadImm64(buf_, dst, static_cast<uint64_t>(static_cast<int64_t>(imm)));
    }
    void EmitMovRIImm32(uint8_t dst, uint32_t imm) override {
        EmitMovz64(buf_, dst, static_cast<uint16_t>(imm & 0xFFFF), 0);
        if ((imm >> 16) & 0xFFFF) {
            EmitMovk64(buf_, dst, static_cast<uint16_t>((imm >> 16) & 0xFFFF), 16);
        }
    }
    void EmitMovMI32(uint8_t base, int32_t disp, uint32_t imm) override {
        // ARM64 has no memory-imm store.  Load imm into X0 then STR.
        EmitMovz64(buf_, 0, static_cast<uint16_t>(imm & 0xFFFF), 0);
        if ((imm >> 16) & 0xFFFF) {
            EmitMovk64(buf_, 0, static_cast<uint16_t>((imm >> 16) & 0xFFFF), 16);
        }
        EmitStr32(buf_, 0, base, static_cast<uint16_t>(disp / 4));
    }
    void EmitLeaRM(uint8_t dst, uint8_t base, int32_t disp) override {
        EmitAdd64Imm(buf_, dst, base, static_cast<uint16_t>(disp));
    }
    void EmitLeaRipRel(uint8_t dst, int32_t disp) override {
        EmitAdr(buf_, dst, disp);
    }

    // ── Addition ─────────────────────────────────────────────────────────
    void EmitAddRR(uint8_t dst, uint8_t src) override {
        EmitAdd64(buf_, dst, dst, src);
    }
    void EmitAddRI(uint8_t dst, int32_t imm) override {
        EmitAdd64Imm(buf_, dst, dst, static_cast<uint16_t>(imm));
    }
    void EmitAddMR(uint8_t base, int32_t disp, uint8_t src) override {
        // ARM64 LDR + ADD + STR sequence
        // LDR Xtmp, [base, #disp]; ADD Xtmp, Xtmp, Xsrc; STR Xtmp, [base, #disp]
        // Use X0 as temp — caller must be aware.  For JIT use, register allocator
        // should handle this.
        uint8_t tmp = 0;
        EmitLdr64(buf_, tmp, base, static_cast<uint16_t>(disp / 8));
        EmitAdd64(buf_, tmp, tmp, src);
        EmitStr64(buf_, tmp, base, static_cast<uint16_t>(disp / 8));
    }
    void EmitAdd32RR(uint8_t dst, uint8_t src) override {
        EmitAdd32(buf_, dst, dst, src);
    }

    // ── Subtraction ──────────────────────────────────────────────────────
    void EmitSubRI(uint8_t dst, int32_t imm) override {
        EmitSub64Imm(buf_, dst, dst, static_cast<uint16_t>(imm));
    }
    void EmitSubRR(uint8_t dst, uint8_t src) override {
        EmitSub64(buf_, dst, dst, src);
    }
    void EmitSub32RR(uint8_t dst, uint8_t src) override {
        EmitSub32(buf_, dst, dst, src);
    }

    // ── Multiplication ───────────────────────────────────────────────────
    void EmitImulRR(uint8_t dst, uint8_t src) override {
        EmitMul64(buf_, dst, dst, src);
    }
    void EmitImul32RR(uint8_t dst, uint8_t src) override {
        EmitMul32(buf_, dst, dst, src);
    }

    // ── Bitwise AND ──────────────────────────────────────────────────────
    void EmitAndRR(uint8_t dst, uint8_t src) override {
        EmitAnd64(buf_, dst, dst, src);
    }
    void EmitAnd32RR(uint8_t dst, uint8_t src) override {
        EmitAnd32(buf_, dst, dst, src);
    }

    // ── Bitwise OR ───────────────────────────────────────────────────────
    void EmitOrRR(uint8_t dst, uint8_t src) override {
        EmitOrr64(buf_, dst, dst, src);
    }
    void EmitOr32RR(uint8_t dst, uint8_t src) override {
        EmitOrr32(buf_, dst, dst, src);
    }

    // ── Bitwise XOR ──────────────────────────────────────────────────────
    void EmitXorRR(uint8_t dst, uint8_t src) override {
        EmitEor64(buf_, dst, dst, src);
    }
    void EmitXor32RR(uint8_t dst, uint8_t src) override {
        EmitEor32(buf_, dst, dst, src);
    }
    void EmitXor32ZR(uint8_t reg) override {
        EmitEor32(buf_, reg, reg, reg);  // XOR Wd, Wd, Wd = zero
    }
    void EmitXorMR(uint8_t base, int32_t disp, uint8_t src) override {
        // LDR + EOR + STR sequence
        uint8_t tmp = 0;
        EmitLdr64(buf_, tmp, base, static_cast<uint16_t>(disp / 8));
        EmitEor64(buf_, tmp, tmp, src);
        EmitStr64(buf_, tmp, base, static_cast<uint16_t>(disp / 8));
    }
    void EmitXorRM(uint8_t dst, uint8_t base, int32_t disp) override {
        // LDR + EOR
        EmitLdr64(buf_, dst, base, static_cast<uint16_t>(disp / 8));
        EmitEor64(buf_, dst, dst, dst);
    }
    void EmitXorZR(uint8_t reg) override {
        EmitEor64(buf_, reg, reg, reg);  // XOR Xd, Xd, Xd = zero
    }
    void EmitXorpsRR(uint8_t dst, uint8_t src) override {
        EmitXor16B(buf_, dst, dst, src);
    }

    // ── Negation / Not ──────────────────────────────────────────────────────
    void EmitNeg32(uint8_t reg) override {
        EmitNeg32(buf_, reg, reg);
    }
    void EmitNeg(uint8_t reg) override {
        EmitNeg64(buf_, reg, reg);
    }
    void EmitNot32(uint8_t reg) override {
        EmitMvn32(buf_, reg, reg);
    }
    void EmitNot(uint8_t reg) override {
        EmitMvn64(buf_, reg, reg);
    }

    // ── Shift ────────────────────────────────────────────────────────────
    void EmitShlRI(uint8_t reg, uint8_t imm) override {
        EmitLsl64Imm(buf_, reg, reg, imm);
    }
    void EmitShrRI(uint8_t reg, uint8_t imm) override {
        EmitLsr64Imm(buf_, reg, reg, imm);
    }
    void EmitSarRI(uint8_t reg, uint8_t imm) override {
        EmitAsr64Imm(buf_, reg, reg, imm);
    }
    void EmitShlRCL(uint8_t reg) override {
        EmitLsl64Var(buf_, reg, reg, reg);
    }
    void EmitShrRCL(uint8_t reg) override {
        EmitLsr64Var(buf_, reg, reg, reg);
    }
    void EmitSarRCL(uint8_t reg) override {
        EmitAsr64Var(buf_, reg, reg, reg);
    }

    // ── Comparison ───────────────────────────────────────────────────────
    void EmitCmpRR(uint8_t lhs, uint8_t rhs) override {
        // CMP Xn, Xm = SUBS XZR, Xn, Xm
        EmitArm64(buf_, 0xEB000000u | (rhs << 16) | (lhs << 5) | 31);
    }
    void EmitCmp32RR(uint8_t lhs, uint8_t rhs) override {
        // CMP Wn, Wm = SUBS WZR, Wn, Wm
        EmitArm64(buf_, 0x6B000000u | (rhs << 16) | (lhs << 5) | 31);
    }
    void EmitCmpRI(uint8_t reg, int32_t imm) override {
        // CMP Xn, #imm = SUBS XZR, Xn, #imm
        EmitArm64(buf_, 0xF1000000u | (static_cast<uint32_t>(imm) << 10) | (reg << 5));
    }
    void EmitCmpMI(uint8_t base, int32_t disp, int32_t imm) override {
        // LDR + CMP sequence
        uint8_t tmp = 0;
        EmitLdr64(buf_, tmp, base, static_cast<uint16_t>(disp / 8));
        EmitCmpRI(tmp, imm);
    }
    void EmitTestRR(uint8_t reg1, uint8_t reg2) override {
        // TST Xn, Xm = ANDS XZR, Xn, Xm
        EmitArm64(buf_, 0xEA000000u | (reg2 << 16) | (reg1 << 5) | 31);
    }
    void EmitSetcc(uint8_t cc, uint8_t reg) override {
        // Map x64 condition code → ARM64 condition code, then CSET
        uint8_t arm_cond = X64ToArm64Cond(cc);
        EmitCset64(buf_, reg, arm_cond);
    }
    void EmitCmovcc(uint8_t cc, uint8_t dst, uint8_t src) override {
        uint8_t arm_cond = X64ToArm64Cond(cc);
        EmitCsel64(buf_, dst, dst, src, arm_cond);
    }

    // ── Jump / Call / Return / Stack ─────────────────────────────────────
    void EmitJmpRel32(int32_t offset) override {
        EmitB(buf_, offset);
    }
    void EmitJmpRel8(int8_t offset) override {
        EmitB(buf_, offset);
    }
    void EmitJccRel32(uint8_t cc, int32_t offset) override {
        uint8_t arm_cond = X64ToArm64Cond(cc);
        EmitBCond(buf_, arm_cond, offset);
    }
    void EmitJccRel8(uint8_t cc, int8_t offset) override {
        uint8_t arm_cond = X64ToArm64Cond(cc);
        EmitBCond(buf_, arm_cond, offset);
    }
    void EmitJmpReg(uint8_t reg) override {
        EmitBr(buf_, reg);
    }
    void EmitCallRel32(int32_t offset) override {
        EmitBl(buf_, offset);
    }
    void EmitCallRipRel(int32_t disp) override {
        CHAOS_IL2CPP_FAIL(FATAL);
    }
    void EmitCallReg(uint8_t reg) override {
        EmitBlr(buf_, reg);
    }
    void EmitRet() override {
        ::chaos::il2cpp::jit::EmitRet(buf_);
    }
    void EmitPush(uint8_t reg) override {
        // STP Xt, XZR, [SP, #-16]!  (pre-indexed store pair, dummy slot)
        EmitStp64Pre(buf_, reg, 31, kARM64_SP, -16);
    }
    void EmitPop(uint8_t reg) override {
        // LDP Xt, XZR, [SP], #16   (post-indexed load pair, discard dummy slot)
        EmitLdp64Post(buf_, reg, 31, kARM64_SP, 16);
    }

    // ── Sign extension ───────────────────────────────────────────────────
    void EmitMovsxd(uint8_t dst, uint8_t src) override {
        EmitSxtw(buf_, dst, src);
    }

    // ── Float / SSE (scalar double) ──────────────────────────────────────
    void EmitMovSDRR(uint8_t dst, uint8_t src) override {
        EmitFmov64(buf_, dst, src);
    }
    void EmitMovSSRR(uint8_t dst, uint8_t src) override {
        EmitFmov32(buf_, dst, src);
    }
    void EmitMovSDMR(uint8_t base, int32_t disp, uint8_t src) override {
        EmitStrD(buf_, src, base, static_cast<uint16_t>(disp / 8));
    }
    void EmitMovSDRM(uint8_t dst, uint8_t base, int32_t disp) override {
        EmitLdrD(buf_, dst, base, static_cast<uint16_t>(disp / 8));
    }
    void EmitMovUPSMR(uint8_t base, int32_t disp, uint8_t src) override {
        EmitStrQ(buf_, src, base, static_cast<uint16_t>(disp / 16));
    }
    void EmitMovUPRM(uint8_t dst, uint8_t base, int32_t disp) override {
        EmitLdrQ(buf_, dst, base, static_cast<uint16_t>(disp / 16));
    }
    void EmitMovSSMR(uint8_t base, int32_t disp, uint8_t src) override {
        EmitStrS(buf_, src, base, static_cast<uint16_t>(disp / 4));
    }
    void EmitMovSSRM(uint8_t dst, uint8_t base, int32_t disp) override {
        EmitLdrS(buf_, dst, base, static_cast<uint16_t>(disp / 4));
    }
    void EmitAddSDRR(uint8_t dst, uint8_t src) override {
        EmitFadd64(buf_, dst, dst, src);
    }
    void EmitSubSDRR(uint8_t dst, uint8_t src) override {
        EmitFsub64(buf_, dst, dst, src);
    }
    void EmitMulSDRR(uint8_t dst, uint8_t src) override {
        EmitFmul64(buf_, dst, dst, src);
    }
    void EmitDivSDRR(uint8_t dst, uint8_t src) override {
        EmitFdiv64(buf_, dst, dst, src);
    }
    void EmitAddSSRR(uint8_t dst, uint8_t src) override {
        EmitFadd32(buf_, dst, dst, src);
    }
    void EmitSubSSRR(uint8_t dst, uint8_t src) override {
        EmitFsub32(buf_, dst, dst, src);
    }
    void EmitMulSSRR(uint8_t dst, uint8_t src) override {
        EmitFmul32(buf_, dst, dst, src);
    }
    void EmitDivSSRR(uint8_t dst, uint8_t src) override {
        EmitFdiv32(buf_, dst, dst, src);
    }
    void EmitCvtsi2sd(uint8_t xmm, uint8_t reg) override {
        EmitScvtfD64(buf_, xmm, reg);
    }
    void EmitCvtsi2ss(uint8_t xmm, uint8_t reg) override {
        EmitScvtfS32(buf_, xmm, reg);
    }
    void EmitCvttsd2si(uint8_t reg, uint8_t xmm) override {
        EmitFcvtzsD64(buf_, reg, xmm);
    }
    void EmitCvttss2si(uint8_t reg, uint8_t xmm) override {
        EmitFcvtzsS32(buf_, reg, xmm);
    }
    void EmitCvtsd2ss(uint8_t dst, uint8_t src) override {
        EmitFcvtD2S(buf_, dst, src);
    }
    void EmitCvtss2sd(uint8_t dst, uint8_t src) override {
        EmitFcvtS2D(buf_, dst, src);
    }
    void EmitUcomisd(uint8_t lhs, uint8_t rhs) override {
        EmitFcmp64(buf_, lhs, rhs);
    }
    void EmitUcomiss(uint8_t lhs, uint8_t rhs) override {
        EmitFcmp32(buf_, lhs, rhs);
    }
    void EmitComisd(uint8_t lhs, uint8_t rhs) override {
        EmitFcmp64(buf_, lhs, rhs);
    }
    void EmitMovdXrm(uint8_t xmm, uint8_t reg) override {
        // FMOV Sd, Wn   (64-bit: FMOV Dd, Xn)
        EmitFmov64Gpr(buf_, xmm, reg);
    }
    void EmitMovdMrx(uint8_t reg, uint8_t xmm) override {
        // FMOV Xd, Dn
        EmitFmov64Fpr(buf_, reg, xmm);
    }
    void EmitMovqXrm(uint8_t xmm, uint8_t reg) override {
        EmitFmov64Gpr(buf_, xmm, reg);
    }
    void EmitMovqMrx(uint8_t reg, uint8_t xmm) override {
        EmitFmov64Fpr(buf_, reg, xmm);
    }
    void EmitPxorRR(uint8_t dst, uint8_t src) override {
        EmitXor16B(buf_, dst, dst, src);
    }

    // SSE2 integer ALU — ARM64 NEON stubs (not implemented; x64-only codegen)
    void EmitPaddbRR(uint8_t, uint8_t) override { CHAOS_IL2CPP_FAIL(FATAL); }
    void EmitPaddwRR(uint8_t, uint8_t) override { CHAOS_IL2CPP_FAIL(FATAL); }
    void EmitPadddRR(uint8_t, uint8_t) override { CHAOS_IL2CPP_FAIL(FATAL); }
    void EmitPaddqRR(uint8_t, uint8_t) override { CHAOS_IL2CPP_FAIL(FATAL); }
    void EmitPsubbRR(uint8_t, uint8_t) override { CHAOS_IL2CPP_FAIL(FATAL); }
    void EmitPsubwRR(uint8_t, uint8_t) override { CHAOS_IL2CPP_FAIL(FATAL); }
    void EmitPsubdRR(uint8_t, uint8_t) override { CHAOS_IL2CPP_FAIL(FATAL); }
    void EmitPsubqRR(uint8_t, uint8_t) override { CHAOS_IL2CPP_FAIL(FATAL); }
    void EmitPmullwRR(uint8_t, uint8_t) override { CHAOS_IL2CPP_FAIL(FATAL); }
    void EmitPmuludqRR(uint8_t, uint8_t) override { CHAOS_IL2CPP_FAIL(FATAL); }
    void EmitPandRR(uint8_t, uint8_t) override { CHAOS_IL2CPP_FAIL(FATAL); }
    void EmitPorRR(uint8_t, uint8_t) override { CHAOS_IL2CPP_FAIL(FATAL); }
    void EmitPandnRR(uint8_t, uint8_t) override { CHAOS_IL2CPP_FAIL(FATAL); }
    void EmitPcmpeqbRR(uint8_t, uint8_t) override { CHAOS_IL2CPP_FAIL(FATAL); }
    void EmitPcmpeqwRR(uint8_t, uint8_t) override { CHAOS_IL2CPP_FAIL(FATAL); }
    void EmitPcmpeqdRR(uint8_t, uint8_t) override { CHAOS_IL2CPP_FAIL(FATAL); }
    void EmitPcmpeqqRR(uint8_t, uint8_t) override { CHAOS_IL2CPP_FAIL(FATAL); }
    void EmitPcmpgtbRR(uint8_t, uint8_t) override { CHAOS_IL2CPP_FAIL(FATAL); }
    void EmitPcmpgtwRR(uint8_t, uint8_t) override { CHAOS_IL2CPP_FAIL(FATAL); }
    void EmitPcmpgtdRR(uint8_t, uint8_t) override { CHAOS_IL2CPP_FAIL(FATAL); }
    void EmitPcmpgtqRR(uint8_t, uint8_t) override { CHAOS_IL2CPP_FAIL(FATAL); }
    void EmitPshufdRR(uint8_t, uint8_t, uint8_t) override { CHAOS_IL2CPP_FAIL(FATAL); }
    void EmitPabsbRR(uint8_t, uint8_t) override { CHAOS_IL2CPP_FAIL(FATAL); }
    void EmitPabswRR(uint8_t, uint8_t) override { CHAOS_IL2CPP_FAIL(FATAL); }
    void EmitPabsdRR(uint8_t, uint8_t) override { CHAOS_IL2CPP_FAIL(FATAL); }
    void EmitPopcntRR(uint8_t, uint8_t) override { CHAOS_IL2CPP_FAIL(FATAL); }
    void EmitLzcntRR(uint8_t, uint8_t) override { CHAOS_IL2CPP_FAIL(FATAL); }

private:
    // ── Helper: map x64 condition code → ARM64 condition code ────────────
    static uint8_t X64ToArm64Cond(uint8_t xcc) noexcept {
        // x64 kCC values: O=0, NO=1, B=2, AE=3, E=4, NE=5, BE=6, A=7,
        //                 S=8, NS=9, P=10, NP=11, L=12, GE=13, LE=14, G=15
        // ARM64: EQ=0..AL=14
        switch (xcc) {
        case 0:  return kARM64_VS;  // O  → VS (overflow set)
        case 1:  return kARM64_VC;  // NO → VC
        case 2:  return kARM64_CC;  // B  → CC (unsigned below, carry clear)
        case 3:  return kARM64_CS;  // AE → CS (unsigned >=, carry set)
        case 4:  return kARM64_EQ;  // E  → EQ
        case 5:  return kARM64_NE;  // NE → NE
        case 6:  return kARM64_LS;  // BE → LS (unsigned <=)
        case 7:  return kARM64_HI;  // A  → HI (unsigned >)
        case 8:  return kARM64_MI;  // S  → MI (minus/negative)
        case 9:  return kARM64_PL;  // NS → PL (plus)
        case 10: return kARM64_AL;  // P  → AL (always, parity not mappable)
        case 11: return kARM64_AL;  // NP → AL
        case 12: return kARM64_LT;  // L  → LT (signed <)
        case 13: return kARM64_GE;  // GE → GE (signed >=)
        case 14: return kARM64_LE;  // LE → LE (signed <=)
        case 15: return kARM64_GT;  // G  → GT (signed >)
        default: return kARM64_AL;
        }
    }

    // ── Missing ARM64 encodings (inline helpers) ─────────────────────────

    /// FMOV Dd, Dn (scalar double move register)
    static void EmitFmov64(CodeBuffer& buf, uint8_t rd, uint8_t rn) noexcept {
        EmitArm64(buf, 0x1E604000u | (rn << 5) | rd);
    }

    /// FMOV Sd, Sn (scalar float move register)
    static void EmitFmov32(CodeBuffer& buf, uint8_t rd, uint8_t rn) noexcept {
        EmitArm64(buf, 0x1E204000u | (rn << 5) | rd);
    }

    /// FMOV Dd, Xn (GPR → FP, 64-bit)
    static void EmitFmov64Gpr(CodeBuffer& buf, uint8_t rd, uint8_t rn) noexcept {
        EmitArm64(buf, 0x9E670000u | (rn << 5) | rd);
    }

    /// FMOV Xd, Dn (FP → GPR, 64-bit)
    static void EmitFmov64Fpr(CodeBuffer& buf, uint8_t rd, uint8_t rn) noexcept {
        EmitArm64(buf, 0x9E660000u | (rn << 5) | rd);
    }

    /// 32-bit FMUL Sd, Sn, Sm
    static void EmitFmul32(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
        EmitArm64(buf, 0x1E200800u | (rm << 16) | (rn << 5) | rd);
    }

    /// 32-bit FDIV Sd, Sn, Sm
    static void EmitFdiv32(CodeBuffer& buf, uint8_t rd, uint8_t rn, uint8_t rm) noexcept {
        EmitArm64(buf, 0x1E201800u | (rm << 16) | (rn << 5) | rd);
    }

    CodeBuffer& buf_;
};

}  // namespace chaos::il2cpp::jit

#endif  // CHAOS_IL2CPP_ARM64ENCODER_H_
