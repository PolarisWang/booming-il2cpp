#ifndef CHAOS_IL2CPP_X64ENCODER_H_
#define CHAOS_IL2CPP_X64ENCODER_H_

// ── X64 encoder (IEncoder implementation) ───────────────────────────────────
//
// Wraps the x64_encoder.h free-function API behind the IEncoder interface.
// Every pure virtual method forwards to the corresponding EmitXxx(buf_, ...)
// call in x64_encoder.h.

#include "IEncoder.h"
#include "x64_encoder.h"
#include "code_buffer.h"

namespace chaos::il2cpp::jit {

class X64Encoder : public IEncoder {
public:
    explicit X64Encoder(CodeBuffer& buf) : buf_(buf) {}

    Arch GetArch() const noexcept override { return Arch::kX64; }

    // ── MOV ──────────────────────────────────────────────────────────────
    void EmitMovRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitMovRR(buf_, dst, src);
    }
    void EmitMovRM(uint8_t dst, uint8_t base, int32_t disp) override {
        ::chaos::il2cpp::jit::EmitMovRM(buf_, dst, base, disp);
    }
    void EmitMovMR(uint8_t base, int32_t disp, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitMovMR(buf_, base, disp, src);
    }
    void EmitMovImm64(uint8_t dst, uint64_t imm) override {
        ::chaos::il2cpp::jit::EmitMovImm64(buf_, dst, imm);
    }
    void EmitMovRI32(uint8_t dst, int32_t imm) override {
        ::chaos::il2cpp::jit::EmitMovRI32(buf_, dst, imm);
    }
    void EmitMovRIImm32(uint8_t dst, uint32_t imm) override {
        ::chaos::il2cpp::jit::EmitMovRIImm32(buf_, dst, imm);
    }
    void EmitMovMI32(uint8_t base, int32_t disp, uint32_t imm) override {
        ::chaos::il2cpp::jit::EmitMovMI32(buf_, base, disp, imm);
    }
    void EmitLeaRM(uint8_t dst, uint8_t base, int32_t disp) override {
        ::chaos::il2cpp::jit::EmitLeaRM(buf_, dst, base, disp);
    }
    void EmitLeaRipRel(uint8_t dst, int32_t disp) override {
        ::chaos::il2cpp::jit::EmitLeaRipRel(buf_, dst, disp);
    }

    // ── Addition ─────────────────────────────────────────────────────────
    void EmitAddRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitAddRR(buf_, dst, src);
    }
    void EmitAddRI(uint8_t dst, int32_t imm) override {
        ::chaos::il2cpp::jit::EmitAddRI(buf_, dst, imm);
    }
    // 3-operand ADD: dst = src + imm. On x64 this is LEA (the IEncoder default
    // is a no-op, but jit_engine.cpp calls this for frame pointer setup on all arches).
    void EmitAddRI(uint8_t dst, uint8_t src, int32_t imm) noexcept override {
        if (imm == 0) {
            ::chaos::il2cpp::jit::EmitMovRR(buf_, dst, src);
        } else {
            ::chaos::il2cpp::jit::EmitLeaRM(buf_, dst, src, imm);
        }
    }
    void EmitAddMR(uint8_t base, int32_t disp, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitAddMR(buf_, base, disp, src);
    }
    void EmitAdd32RR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitAdd32RR(buf_, dst, src);
    }

    // ── Subtraction ──────────────────────────────────────────────────────
    void EmitSubRI(uint8_t dst, int32_t imm) override {
        ::chaos::il2cpp::jit::EmitSubRI(buf_, dst, imm);
    }
    void EmitSubRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitSubRR(buf_, dst, src);
    }
    void EmitSub32RR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitSub32RR(buf_, dst, src);
    }

    // ── Multiplication ───────────────────────────────────────────────────
    void EmitImulRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitImulRR(buf_, dst, src);
    }
    void EmitImul32RR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitImul32RR(buf_, dst, src);
    }

    // ── Bitwise AND ──────────────────────────────────────────────────────
    void EmitAndRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitAndRR(buf_, dst, src);
    }
    void EmitAnd32RR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitAnd32RR(buf_, dst, src);
    }

    // ── Bitwise OR ───────────────────────────────────────────────────────
    void EmitOrRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitOrRR(buf_, dst, src);
    }
    void EmitOr32RR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitOr32RR(buf_, dst, src);
    }

    // ── Bitwise XOR ──────────────────────────────────────────────────────
    void EmitXorRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitXorRR(buf_, dst, src);
    }
    void EmitXor32RR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitXor32RR(buf_, dst, src);
    }
    void EmitXor32ZR(uint8_t reg) override {
        ::chaos::il2cpp::jit::EmitXor32ZR(buf_, reg);
    }
    void EmitXorMR(uint8_t base, int32_t disp, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitXorMR(buf_, base, disp, src);
    }
    void EmitXorRM(uint8_t dst, uint8_t base, int32_t disp) override {
        ::chaos::il2cpp::jit::EmitXorRM(buf_, dst, base, disp);
    }
    void EmitXorZR(uint8_t reg) override {
        ::chaos::il2cpp::jit::EmitXorZR(buf_, reg);
    }
    void EmitXorpsRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitXorpsRR(buf_, dst, src);
    }

    // ── Negation / Not ───────────────────────────────────────────────────
    void EmitNeg32(uint8_t reg) override {
        ::chaos::il2cpp::jit::EmitNeg32(buf_, reg);
    }
    void EmitNeg(uint8_t reg) override {
        ::chaos::il2cpp::jit::EmitNeg(buf_, reg);
    }
    void EmitNot32(uint8_t reg) override {
        ::chaos::il2cpp::jit::EmitNot32(buf_, reg);
    }
    void EmitNot(uint8_t reg) override {
        ::chaos::il2cpp::jit::EmitNot(buf_, reg);
    }

    // ── Shift ────────────────────────────────────────────────────────────
    void EmitShlRI(uint8_t reg, uint8_t imm) override {
        ::chaos::il2cpp::jit::EmitShlRI(buf_, reg, imm);
    }
    void EmitShrRI(uint8_t reg, uint8_t imm) override {
        ::chaos::il2cpp::jit::EmitShrRI(buf_, reg, imm);
    }
    void EmitSarRI(uint8_t reg, uint8_t imm) override {
        ::chaos::il2cpp::jit::EmitSarRI(buf_, reg, imm);
    }
    void EmitShlRCL(uint8_t reg) override {
        ::chaos::il2cpp::jit::EmitShlRCL(buf_, reg);
    }
    void EmitShrRCL(uint8_t reg) override {
        ::chaos::il2cpp::jit::EmitShrRCL(buf_, reg);
    }
    void EmitSarRCL(uint8_t reg) override {
        ::chaos::il2cpp::jit::EmitSarRCL(buf_, reg);
    }

    // ── Comparison ───────────────────────────────────────────────────────
    void EmitCmpRR(uint8_t lhs, uint8_t rhs) override {
        ::chaos::il2cpp::jit::EmitCmpRR(buf_, lhs, rhs);
    }
    void EmitCmp32RR(uint8_t lhs, uint8_t rhs) override {
        ::chaos::il2cpp::jit::EmitCmp32RR(buf_, lhs, rhs);
    }
    void EmitCmpRI(uint8_t reg, int32_t imm) override {
        ::chaos::il2cpp::jit::EmitCmpRI(buf_, reg, imm);
    }
    void EmitCmpMI(uint8_t base, int32_t disp, int32_t imm) override {
        ::chaos::il2cpp::jit::EmitCmpMI(buf_, base, disp, imm);
    }
    void EmitTestRR(uint8_t reg1, uint8_t reg2) override {
        ::chaos::il2cpp::jit::EmitTestRR(buf_, reg1, reg2);
    }
    void EmitSetcc(uint8_t cc, uint8_t reg) override {
        ::chaos::il2cpp::jit::EmitSetcc(buf_, cc, reg);
    }
    void EmitCmovcc(uint8_t cc, uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitCmovcc(buf_, cc, dst, src);
    }

    // ── Jump / Call / Return / Stack ─────────────────────────────────────
    void EmitJmpRel32(int32_t offset) override {
        ::chaos::il2cpp::jit::EmitJmpRel32(buf_, offset);
    }
    void EmitJmpRel8(int8_t offset) override {
        ::chaos::il2cpp::jit::EmitJmpRel8(buf_, offset);
    }
    void EmitJccRel32(uint8_t cc, int32_t offset) override {
        ::chaos::il2cpp::jit::EmitJccRel32(buf_, cc, offset);
    }
    void EmitJccRel8(uint8_t cc, int8_t offset) override {
        ::chaos::il2cpp::jit::EmitJccRel8(buf_, cc, offset);
    }
    void EmitJmpReg(uint8_t reg) override {
        ::chaos::il2cpp::jit::EmitJmpReg(buf_, reg);
    }
    void EmitCallRel32(int32_t offset) override {
        ::chaos::il2cpp::jit::EmitCallRel32(buf_, offset);
    }
    void EmitCallRipRel(int32_t disp) override {
        ::chaos::il2cpp::jit::EmitCallRipRel(buf_, disp);
    }
    void EmitCallReg(uint8_t reg) override {
        ::chaos::il2cpp::jit::EmitCallReg(buf_, reg);
    }
    void EmitRet() override {
        ::chaos::il2cpp::jit::EmitRet(buf_);
    }
    void EmitPush(uint8_t reg) override {
        ::chaos::il2cpp::jit::EmitPush(buf_, reg);
    }
    void EmitPop(uint8_t reg) override {
        ::chaos::il2cpp::jit::EmitPop(buf_, reg);
    }

    // ── Sign extension ───────────────────────────────────────────────────
    void EmitMovsxd(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitMovsxd(buf_, dst, src);
    }

    // ── Float / SSE (scalar double) ──────────────────────────────────────
    void EmitMovSDRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitMovSDRR(buf_, dst, src);
    }
    void EmitMovSSRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitMovSSRR(buf_, dst, src);
    }
    void EmitMovSDMR(uint8_t base, int32_t disp, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitMovSDMR(buf_, base, disp, src);
    }
    void EmitMovSDRM(uint8_t dst, uint8_t base, int32_t disp) override {
        ::chaos::il2cpp::jit::EmitMovSDRM(buf_, dst, base, disp);
    }
    void EmitMovUPSMR(uint8_t base, int32_t disp, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitMovUPSMR(buf_, base, disp, src);
    }
    void EmitMovUPRM(uint8_t dst, uint8_t base, int32_t disp) override {
        ::chaos::il2cpp::jit::EmitMovUPRM(buf_, dst, base, disp);
    }
    void EmitMovSSMR(uint8_t base, int32_t disp, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitMovSSMR(buf_, base, disp, src);
    }
    void EmitMovSSRM(uint8_t dst, uint8_t base, int32_t disp) override {
        ::chaos::il2cpp::jit::EmitMovSSRM(buf_, dst, base, disp);
    }
    void EmitAddSDRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitAddSDRR(buf_, dst, src);
    }
    void EmitSubSDRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitSubSDRR(buf_, dst, src);
    }
    void EmitMulSDRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitMulSDRR(buf_, dst, src);
    }
    void EmitDivSDRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitDivSDRR(buf_, dst, src);
    }
    void EmitAddSSRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitAddSSRR(buf_, dst, src);
    }
    void EmitSubSSRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitSubSSRR(buf_, dst, src);
    }
    void EmitMulSSRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitMulSSRR(buf_, dst, src);
    }
    void EmitDivSSRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitDivSSRR(buf_, dst, src);
    }
    void EmitCvtsi2sd(uint8_t xmm, uint8_t reg) override {
        ::chaos::il2cpp::jit::EmitCvtsi2sd(buf_, xmm, reg);
    }
    void EmitCvtsi2ss(uint8_t xmm, uint8_t reg) override {
        ::chaos::il2cpp::jit::EmitCvtsi2ss(buf_, xmm, reg);
    }
    void EmitCvttsd2si(uint8_t reg, uint8_t xmm) override {
        ::chaos::il2cpp::jit::EmitCvttsd2si(buf_, reg, xmm);
    }
    void EmitCvttss2si(uint8_t reg, uint8_t xmm) override {
        ::chaos::il2cpp::jit::EmitCvttss2si(buf_, reg, xmm);
    }
    void EmitCvtsd2ss(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitCvtsd2ss(buf_, dst, src);
    }
    void EmitCvtss2sd(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitCvtss2sd(buf_, dst, src);
    }
    void EmitUcomisd(uint8_t lhs, uint8_t rhs) override {
        ::chaos::il2cpp::jit::EmitUcomisd(buf_, lhs, rhs);
    }
    void EmitUcomiss(uint8_t lhs, uint8_t rhs) override {
        ::chaos::il2cpp::jit::EmitUcomiss(buf_, lhs, rhs);
    }
    void EmitComisd(uint8_t lhs, uint8_t rhs) override {
        ::chaos::il2cpp::jit::EmitComisd(buf_, lhs, rhs);
    }
    void EmitMovdXrm(uint8_t xmm, uint8_t reg) override {
        ::chaos::il2cpp::jit::EmitMovdXrm(buf_, xmm, reg);
    }
    void EmitMovdMrx(uint8_t reg, uint8_t xmm) override {
        ::chaos::il2cpp::jit::EmitMovdMrx(buf_, reg, xmm);
    }
    void EmitMovqXrm(uint8_t xmm, uint8_t reg) override {
        ::chaos::il2cpp::jit::EmitMovqXrm(buf_, xmm, reg);
    }
    void EmitMovqMrx(uint8_t reg, uint8_t xmm) override {
        ::chaos::il2cpp::jit::EmitMovqMrx(buf_, reg, xmm);
    }
    void EmitMovdqaRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitMovdqaRR(buf_, dst, src);
    }
    void EmitMovdqaMR(uint8_t base, int32_t disp, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitMovdqaMR(buf_, base, disp, src);
    }
    void EmitMovdqaRM(uint8_t dst, uint8_t base, int32_t disp) override {
        ::chaos::il2cpp::jit::EmitMovdqaRM(buf_, dst, base, disp);
    }
    void EmitPxorRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitPxorRR(buf_, dst, src);
    }

    // SSE2 integer ALU
    void EmitPaddbRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitPaddbRR(buf_, dst, src);
    }
    void EmitPaddwRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitPaddwRR(buf_, dst, src);
    }
    void EmitPadddRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitPadddRR(buf_, dst, src);
    }
    void EmitPaddqRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitPaddqRR(buf_, dst, src);
    }
    void EmitPsubbRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitPsubbRR(buf_, dst, src);
    }
    void EmitPsubwRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitPsubwRR(buf_, dst, src);
    }
    void EmitPsubdRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitPsubdRR(buf_, dst, src);
    }
    void EmitPsubqRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitPsubqRR(buf_, dst, src);
    }
    void EmitPmullwRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitPmullwRR(buf_, dst, src);
    }
    void EmitPmuludqRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitPmuludqRR(buf_, dst, src);
    }
    void EmitPandRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitPandRR(buf_, dst, src);
    }
    void EmitPorRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitPorRR(buf_, dst, src);
    }
    void EmitPandnRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitPandnRR(buf_, dst, src);
    }
    void EmitPcmpeqbRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitPcmpeqbRR(buf_, dst, src);
    }
    void EmitPcmpeqwRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitPcmpeqwRR(buf_, dst, src);
    }
    void EmitPcmpeqdRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitPcmpeqdRR(buf_, dst, src);
    }
    void EmitPcmpeqqRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitPcmpeqqRR(buf_, dst, src);
    }
    void EmitPcmpgtbRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitPcmpgtbRR(buf_, dst, src);
    }
    void EmitPcmpgtwRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitPcmpgtwRR(buf_, dst, src);
    }
    void EmitPcmpgtdRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitPcmpgtdRR(buf_, dst, src);
    }
    void EmitPcmpgtqRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitPcmpgtqRR(buf_, dst, src);
    }
    void EmitPshufdRR(uint8_t dst, uint8_t src, uint8_t imm) override {
        ::chaos::il2cpp::jit::EmitPshufdRR(buf_, dst, src, imm);
    }
    void EmitPabsbRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitPabsbRR(buf_, dst, src);
    }
    void EmitPabswRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitPabswRR(buf_, dst, src);
    }
    void EmitPabsdRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitPabsdRR(buf_, dst, src);
    }

    // Bit manipulation
    void EmitPopcntRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitPopcntRR(buf_, dst, src);
    }
    void EmitLzcntRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitLzcntRR(buf_, dst, src);
    }

    // ── FMA ──────────────────────────────────────────────────────────
    void EmitVfmadd231psRR(uint8_t acc, uint8_t src1, uint8_t src2) override {
        ::chaos::il2cpp::jit::EmitVfmadd231psRR(buf_, acc, src1, src2);
    }
    void EmitVfmadd231pdRR(uint8_t acc, uint8_t src1, uint8_t src2) override {
        ::chaos::il2cpp::jit::EmitVfmadd231pdRR(buf_, acc, src1, src2);
    }
    void EmitVfmsub231psRR(uint8_t acc, uint8_t src1, uint8_t src2) override {
        ::chaos::il2cpp::jit::EmitVfmsub231psRR(buf_, acc, src1, src2);
    }
    void EmitVfmsub231pdRR(uint8_t acc, uint8_t src1, uint8_t src2) override {
        ::chaos::il2cpp::jit::EmitVfmsub231pdRR(buf_, acc, src1, src2);
    }
    void EmitVfnmadd231psRR(uint8_t acc, uint8_t src1, uint8_t src2) override {
        ::chaos::il2cpp::jit::EmitVfnmadd231psRR(buf_, acc, src1, src2);
    }
    void EmitVfnmadd231pdRR(uint8_t acc, uint8_t src1, uint8_t src2) override {
        ::chaos::il2cpp::jit::EmitVfnmadd231pdRR(buf_, acc, src1, src2);
    }
    void EmitVfnmsub231psRR(uint8_t acc, uint8_t src1, uint8_t src2) override {
        ::chaos::il2cpp::jit::EmitVfnmsub231psRR(buf_, acc, src1, src2);
    }
    void EmitVfnmsub231pdRR(uint8_t acc, uint8_t src1, uint8_t src2) override {
        ::chaos::il2cpp::jit::EmitVfnmsub231pdRR(buf_, acc, src1, src2);
    }

private:
    CodeBuffer& buf_;
};

}  // namespace chaos::il2cpp::jit

#endif  // CHAOS_IL2CPP_X64ENCODER_H_
