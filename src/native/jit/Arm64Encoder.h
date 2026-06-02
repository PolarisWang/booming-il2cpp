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

    Arch GetArch() const noexcept override { return Arch::kARM64; }

private:
    // Decompose large immediate (>12 bits) for ADD/SUB/CMP by loading into
    // the intra-procedure-call scratch register X17 (IP1).  X17 is always
    // available as a temporary — it is caller-saved and not used by the
    // ARM64 register allocator.
    static constexpr uint8_t kImmScratch = 17;  // X17 (IP1)

    // ── Large immediate decomposition helpers ────────────────────────────
    void EmitAddRIDecomposed(uint8_t rd, uint8_t rn, int32_t imm) noexcept {
        if (static_cast<uint32_t>(imm) <= 4095) {
            EmitAdd64Imm(buf_, rd, rn, static_cast<uint16_t>(imm));
        } else if ((static_cast<uint32_t>(imm) & 0xFFF) == 0 &&
                   (static_cast<uint32_t>(imm) >> 12) <= 4095) {
            EmitAdd64ImmShift(buf_, rd, rn, static_cast<uint16_t>(static_cast<uint32_t>(imm) >> 12));
        } else {
            EmitLoadImm64(buf_, kImmScratch, static_cast<uint64_t>(static_cast<int64_t>(imm)));
            EmitAdd64(buf_, rd, rn, kImmScratch);
        }
    }
    void EmitSubRIDecomposed(uint8_t rd, uint8_t rn, int32_t imm) noexcept {
        if (static_cast<uint32_t>(imm) <= 4095) {
            EmitSub64Imm(buf_, rd, rn, static_cast<uint16_t>(imm));
        } else if ((static_cast<uint32_t>(imm) & 0xFFF) == 0 &&
                   (static_cast<uint32_t>(imm) >> 12) <= 4095) {
            EmitSub64ImmShift(buf_, rd, rn, static_cast<uint16_t>(static_cast<uint32_t>(imm) >> 12));
        } else {
            EmitLoadImm64(buf_, kImmScratch, static_cast<uint64_t>(static_cast<int64_t>(imm)));
            EmitSub64(buf_, rd, rn, kImmScratch);
        }
    }
    void EmitCmpRIDecomposed(uint8_t rn, int32_t imm) noexcept {
        if (static_cast<uint32_t>(imm) <= 4095) {
            EmitCmp64Imm(buf_, rn, static_cast<uint16_t>(imm));
        } else if ((static_cast<uint32_t>(imm) & 0xFFF) == 0 &&
                   (static_cast<uint32_t>(imm) >> 12) <= 4095) {
            EmitCmp64ImmShift(buf_, rn, static_cast<uint16_t>(static_cast<uint32_t>(imm) >> 12));
        } else {
            EmitLoadImm64(buf_, kImmScratch, static_cast<uint64_t>(static_cast<int64_t>(imm)));
            EmitCmpRR(rn, kImmScratch);
        }
    }
    void EmitLeaRMDecomposed(uint8_t dst, uint8_t base, int32_t disp) noexcept {
        if (static_cast<uint32_t>(disp) <= 4095) {
            EmitAdd64Imm(buf_, dst, base, static_cast<uint16_t>(disp));
        } else if ((static_cast<uint32_t>(disp) & 0xFFF) == 0 &&
                   (static_cast<uint32_t>(disp) >> 12) <= 4095) {
            EmitAdd64ImmShift(buf_, dst, base, static_cast<uint16_t>(static_cast<uint32_t>(disp) >> 12));
        } else {
            EmitLoadImm64(buf_, kImmScratch, static_cast<uint64_t>(static_cast<int64_t>(disp)));
            EmitAdd64(buf_, dst, base, kImmScratch);
        }
    }

public:

    // ── MOV ──────────────────────────────────────────────────────────────
    void EmitMovRR(uint8_t dst, uint8_t src) override {
        // MOV Xd, Xm = ORR Xd, XZR, Xm
        EmitOrr64(buf_, dst, 31, src);
    }
    void EmitMovRM(uint8_t dst, uint8_t base, int32_t disp) override {
        EmitLdr64(buf_, dst, base, static_cast<uint16_t>(disp / 8));
    }
    void EmitMovMR(uint8_t base, int32_t disp, uint8_t src) override {
        chaos::il2cpp::jit::EmitStr64(buf_, src, base, static_cast<uint16_t>(disp / 8));
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
        EmitLeaRMDecomposed(dst, base, disp);
    }
    void EmitLeaRipRel(uint8_t dst, int32_t disp) override {
        chaos::il2cpp::jit::EmitAdr(buf_, dst, disp);
    }

    // ── ARM64-specific overrides (stub on x64) ─────────────────────────
    void EmitAdr(uint8_t reg, int32_t imm) noexcept override {
        chaos::il2cpp::jit::EmitAdr(buf_, reg, imm);
    }
    void EmitBr(uint8_t reg) noexcept override {
        chaos::il2cpp::jit::EmitBr(buf_, reg);
    }
    void EmitAddRI(uint8_t dst, uint8_t src, int32_t imm) noexcept override {
        EmitAddRIDecomposed(dst, src, imm);
    }
    void EmitStr64(uint8_t base, uint16_t scaled_offset, uint8_t data) noexcept override {
        chaos::il2cpp::jit::EmitStr64(buf_, data, base, scaled_offset);
    }

    // ── Addition ─────────────────────────────────────────────────────────
    void EmitAddRR(uint8_t dst, uint8_t src) override {
        EmitAdd64(buf_, dst, dst, src);
    }
    void EmitAddRI(uint8_t dst, int32_t imm) override {
        EmitAddRIDecomposed(dst, dst, imm);
    }
    void EmitAddMR(uint8_t base, int32_t disp, uint8_t src) override {
        // ARM64 LDR + ADD + STR sequence
        // LDR Xtmp, [base, #disp]; ADD Xtmp, Xtmp, Xsrc; STR Xtmp, [base, #disp]
        // Use X0 as temp — caller must be aware.  For JIT use, register allocator
        // should handle this.
        uint8_t tmp = 0;
        EmitLdr64(buf_, tmp, base, static_cast<uint16_t>(disp / 8));
        EmitAdd64(buf_, tmp, tmp, src);
        chaos::il2cpp::jit::EmitStr64(buf_, tmp, base, static_cast<uint16_t>(disp / 8));
    }
    void EmitAdd32RR(uint8_t dst, uint8_t src) override {
        EmitAdd32(buf_, dst, dst, src);
    }

    // ── Subtraction ──────────────────────────────────────────────────────
    void EmitSubRI(uint8_t dst, int32_t imm) override {
        EmitSubRIDecomposed(dst, dst, imm);
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
        chaos::il2cpp::jit::EmitStr64(buf_, tmp, base, static_cast<uint16_t>(disp / 8));
    }
    void EmitXorRM(uint8_t dst, uint8_t base, int32_t disp) override {
        // dst = dst ^ [base+disp]; use X0 as temp
        uint8_t tmp = 0;
        EmitLdr64(buf_, tmp, base, static_cast<uint16_t>(disp / 8));
        EmitEor64(buf_, dst, dst, tmp);
    }
    void EmitXorZR(uint8_t reg) override {
        EmitEor64(buf_, reg, reg, reg);  // XOR Xd, Xd, Xd = zero
    }
    void EmitXorpsRR(uint8_t dst, uint8_t src) override {
        EmitXor16B(buf_, dst, dst, src);
    }

    // ── Negation / Not ──────────────────────────────────────────────────────
    void EmitNeg32(uint8_t reg) override {
        ::chaos::il2cpp::jit::EmitNeg32(buf_, reg, reg);
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
        EmitCmpRIDecomposed(reg, imm);
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
        chaos::il2cpp::jit::EmitBr(buf_, reg);
    }
    void EmitCallRel32(int32_t offset) override {
        EmitBl(buf_, offset);
    }
    void EmitCallRipRel(int32_t) override {
        // LDR X17, #0; BLR X17 — the imm19 is patched later by slot table
        // emission in NativeCodeGenerator (jit_engine.cpp slot_table loop).
        buf_.Emit32(0x58000011u);  // LDR X17, #0 (placeholder)
        buf_.Emit32(0xD63F0220u);  // BLR X17
    }
    void EmitCallReg(uint8_t reg) override {
        EmitBlr(buf_, reg);
    }
    void EmitRet() override {
        ::chaos::il2cpp::jit::EmitArm64(buf_, 0xD65F03C0u);  // RET X30 (LR)
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

    // ── 128-bit SIMD moves ─────────────────────────────────────────────────────
    void EmitMovdqaRR(uint8_t dst, uint8_t src) override {
        EmitOrr16B(buf_, dst, src, src);  // ORR Vd.16B, Vn.16B, Vn.16B = MOV
    }
    void EmitMovdqaMR(uint8_t base, int32_t disp, uint8_t src) override {
        EmitStrQ(buf_, src, base, static_cast<uint16_t>(disp / 16));
    }
    void EmitMovdqaRM(uint8_t dst, uint8_t base, int32_t disp) override {
        EmitLdrQ(buf_, dst, base, static_cast<uint16_t>(disp / 16));
    }

    // ── SSE2 integer ALU ───────────────────────────────────────────────────────
    void EmitPaddbRR(uint8_t dst, uint8_t src) override {
        EmitAdd16B(buf_, dst, dst, src);
    }
    void EmitPaddwRR(uint8_t dst, uint8_t src) override {
        EmitAdd8H(buf_, dst, dst, src);
    }
    void EmitPadddRR(uint8_t dst, uint8_t src) override {
        EmitAdd4S(buf_, dst, dst, src);
    }
    void EmitPaddqRR(uint8_t dst, uint8_t src) override {
        EmitAdd2D(buf_, dst, dst, src);
    }
    void EmitPsubbRR(uint8_t dst, uint8_t src) override {
        EmitSub16B(buf_, dst, dst, src);
    }
    void EmitPsubwRR(uint8_t dst, uint8_t src) override {
        EmitSub8H(buf_, dst, dst, src);
    }
    void EmitPsubdRR(uint8_t dst, uint8_t src) override {
        EmitSub4S(buf_, dst, dst, src);
    }
    void EmitPsubqRR(uint8_t dst, uint8_t src) override {
        EmitSub2D(buf_, dst, dst, src);
    }
    void EmitPmullwRR(uint8_t dst, uint8_t src) override {
        EmitMul8H(buf_, dst, dst, src);
    }
    void EmitPmuludqRR(uint8_t dst, uint8_t src) override {
        EmitUmull2D(buf_, dst, dst, src);
    }
    void EmitPandRR(uint8_t dst, uint8_t src) override {
        EmitAnd16B(buf_, dst, dst, src);
    }
    void EmitPorRR(uint8_t dst, uint8_t src) override {
        EmitOrr16B(buf_, dst, dst, src);
    }
    void EmitPandnRR(uint8_t dst, uint8_t src) override {
        EmitBic16B(buf_, dst, dst, src);
    }
    void EmitPcmpeqbRR(uint8_t dst, uint8_t src) override {
        EmitCmeq16B(buf_, dst, dst, src);
    }
    void EmitPcmpeqwRR(uint8_t dst, uint8_t src) override {
        EmitCmeq8H(buf_, dst, dst, src);
    }
    void EmitPcmpeqdRR(uint8_t dst, uint8_t src) override {
        EmitCmeq4S(buf_, dst, dst, src);
    }
    void EmitPcmpeqqRR(uint8_t dst, uint8_t src) override {
        EmitCmeq2D(buf_, dst, dst, src);
    }
    void EmitPcmpgtbRR(uint8_t dst, uint8_t src) override {
        EmitCmgt16B(buf_, dst, dst, src);
    }
    void EmitPcmpgtwRR(uint8_t dst, uint8_t src) override {
        EmitCmgt8H(buf_, dst, dst, src);
    }
    void EmitPcmpgtdRR(uint8_t dst, uint8_t src) override {
        EmitCmgt4S(buf_, dst, dst, src);
    }
    void EmitPcmpgtqRR(uint8_t dst, uint8_t src) override {
        EmitCmgt2D(buf_, dst, dst, src);
    }
    void EmitPshufdRR(uint8_t dst, uint8_t src, uint8_t imm) override {
        // PSHUFD: shuffle 4 dwords using 8-bit immediate (2 bits per dword).
        // ARM64 TBL Vd.16B, {Vn.16B}, Vm.16B does arbitrary byte permutation.
        //
        // Code layout: ADR X10, #12 (points to mask after ADR+LDR+TBL)
        //              LDR Q31, [X10]
        //              TBL Vd.16B, {Vsrc.16B}, V31.16B
        //              .byte mask[16]   (literal pool — NOT executed, data only)
        //
        // The 16 mask bytes follow immediately.  Since PSHUFD is always inside
        // a basic block whose next instruction is a branch (JIT engine invariant),
        // execution never reaches the literal pool data.
        uint8_t mask[16];
        for (int i = 0; i < 4; i++) {
            int sel = (imm >> (i * 2)) & 3;
            mask[i * 4 + 0] = static_cast<uint8_t>(sel * 4 + 0);
            mask[i * 4 + 1] = static_cast<uint8_t>(sel * 4 + 1);
            mask[i * 4 + 2] = static_cast<uint8_t>(sel * 4 + 2);
            mask[i * 4 + 3] = static_cast<uint8_t>(sel * 4 + 3);
        }
        uint8_t tmp_gpr = 10;   // X10 — temp addr register (caller-saved)
        uint8_t tmp_vec = 31;   // V31 — temp mask register
        chaos::il2cpp::jit::EmitAdr(buf_, tmp_gpr, 12);  // points to mask data after ADR(4)+LDR(4)+TBL(4)
        EmitLdrQ(buf_, tmp_vec, tmp_gpr, 0);
        EmitTbl1(buf_, dst, src, tmp_vec);
        // Emit 16-byte mask as 4 little-endian words
        for (int i = 0; i < 4; i++) {
            uint32_t w = (static_cast<uint32_t>(mask[i * 4 + 3]) << 24)
                       | (static_cast<uint32_t>(mask[i * 4 + 2]) << 16)
                       | (static_cast<uint32_t>(mask[i * 4 + 1]) << 8)
                       | (static_cast<uint32_t>(mask[i * 4 + 0]));
            buf_.Emit32(w);
        }
    }
    void EmitPabsbRR(uint8_t dst, uint8_t src) override {
        EmitAbs16B(buf_, dst, src);
    }
    void EmitPabswRR(uint8_t dst, uint8_t src) override {
        EmitAbs8H(buf_, dst, src);
    }
    void EmitPabsdRR(uint8_t dst, uint8_t src) override {
        EmitAbs4S(buf_, dst, src);
    }

    // ── Bit manipulation ───────────────────────────────────────────────────────
    void EmitPopcntRR(uint8_t dst, uint8_t src) override {
        // CNT Vsrc.8B + UADDLV Dsrc, Vsrc.8B + FMOV Xdst, Dsrc
        EmitFmov64Gpr(buf_, src, src);   // FMOV Dsrc, Xsrc (GPR→FP)
        EmitCnt8B(buf_, src, src);        // CNT Vsrc.8B, Vsrc.8B
        EmitUaddlv8B(buf_, src, src);     // UADDLV Dsrc, Vsrc.8B
        EmitFmov64Fpr(buf_, dst, src);    // FMOV Xdst, Dsrc (FP→GPR)
    }
    void EmitLzcntRR(uint8_t dst, uint8_t src) override {
        EmitClz64(buf_, dst, src);  // CLZ Xd, Xn
    }

    // ── FMA (fused multiply-add, 231 form: acc = src1 * src2 [+-] acc) ────────
    void EmitVfmadd231psRR(uint8_t acc, uint8_t src1, uint8_t src2) override {
        EmitFmla4S(buf_, acc, src1, src2);  // Vacc += Vsrc1 * Vsrc2
    }
    void EmitVfmadd231pdRR(uint8_t acc, uint8_t src1, uint8_t src2) override {
        EmitFmla2D(buf_, acc, src1, src2);  // Vacc += Vsrc1 * Vsrc2
    }
    void EmitVfmsub231psRR(uint8_t acc, uint8_t src1, uint8_t src2) override {
        EmitFmls4S(buf_, acc, src1, src2);  // Vacc -= Vsrc1 * Vsrc2
    }
    void EmitVfmsub231pdRR(uint8_t acc, uint8_t src1, uint8_t src2) override {
        EmitFmls2D(buf_, acc, src1, src2);  // Vacc -= Vsrc1 * Vsrc2
    }
    void EmitVfnmadd231psRR(uint8_t acc, uint8_t src1, uint8_t src2) override {
        // -(src1*src2) + acc = acc - src1*src2 = same as fmsub
        EmitFmls4S(buf_, acc, src1, src2);
    }
    void EmitVfnmadd231pdRR(uint8_t acc, uint8_t src1, uint8_t src2) override {
        EmitFmls2D(buf_, acc, src1, src2);
    }
    void EmitVfnmsub231psRR(uint8_t acc, uint8_t src1, uint8_t src2) override {
        // -(src1*src2) - acc = -(src1*src2 + acc) = FMLA then FNEG
        EmitFmla4S(buf_, acc, src1, src2);
        EmitFneg4S(buf_, acc, acc);
    }
    void EmitVfnmsub231pdRR(uint8_t acc, uint8_t src1, uint8_t src2) override {
        EmitFmla2D(buf_, acc, src1, src2);
        EmitFneg2D(buf_, acc, acc);
    }

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
