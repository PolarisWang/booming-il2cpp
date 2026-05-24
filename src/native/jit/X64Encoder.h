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
    void EmitPxorRR(uint8_t dst, uint8_t src) override {
        ::chaos::il2cpp::jit::EmitPxorRR(buf_, dst, src);
    }

private:
    CodeBuffer& buf_;
};

}  // namespace chaos::il2cpp::jit

#endif  // CHAOS_IL2CPP_X64ENCODER_H_
