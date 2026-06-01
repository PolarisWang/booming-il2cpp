#ifndef CHAOS_IL2CPP_IENCODER_H_
#define CHAOS_IL2CPP_IENCODER_H_

// ── Abstract instruction encoder interface ──────────────────────────────────
//
// Pure virtual interface for native code generation.  Each architecture
// (x64, ARM64, etc.) provides its own implementation.
//
// All Emit* methods omit the CodeBuffer& parameter — the concrete encoder
// owns its own CodeBuffer internally.

#include <cstdint>
#include <cstddef>

#include "ArchTraits.h"
#include "code_buffer.h"
#include "interpreter_vm.h"

namespace chaos::il2cpp::jit {

// ── Architecture-neutral register constants ─────────────────────────────────

// GPR (x64 numbering; ARM64 will define its own mapping)
static constexpr uint8_t kRAX = 0;
static constexpr uint8_t kRCX = 1;
static constexpr uint8_t kRDX = 2;
static constexpr uint8_t kRBX = 3;
static constexpr uint8_t kRSP = 4;
static constexpr uint8_t kRBP = 5;
static constexpr uint8_t kRSI = 6;
static constexpr uint8_t kRDI = 7;
static constexpr uint8_t kR8  = 8;
static constexpr uint8_t kR9  = 9;
static constexpr uint8_t kR10 = 10;
static constexpr uint8_t kR11 = 11;
static constexpr uint8_t kR12 = 12;
static constexpr uint8_t kR13 = 13;
static constexpr uint8_t kR14 = 14;
static constexpr uint8_t kR15 = 15;

// ── Instruction size helpers ───────────────────────────────────────────────

/// Max bytes for a single instruction emitted by this encoder.
static constexpr uint32_t kMaxInstrSize = 16;

// ── Condition code constants (for jcc/setcc/cmovcc) ─────────────────────────

static constexpr uint8_t kCC_O  = 0;   // overflow
static constexpr uint8_t kCC_NO = 1;   // not overflow
static constexpr uint8_t kCC_B  = 2;   // below (unsigned <)
static constexpr uint8_t kCC_AE = 3;   // above or equal (unsigned >=)
static constexpr uint8_t kCC_E  = 4;   // equal
static constexpr uint8_t kCC_NE = 5;   // not equal
static constexpr uint8_t kCC_BE = 6;   // below or equal (unsigned <=)
static constexpr uint8_t kCC_A  = 7;   // above (unsigned >)
static constexpr uint8_t kCC_S  = 8;   // sign (negative)
static constexpr uint8_t kCC_NS = 9;   // not sign (non-negative)
static constexpr uint8_t kCC_L  = 12;  // less (signed <)
static constexpr uint8_t kCC_GE = 13;  // greater or equal (signed >=)
static constexpr uint8_t kCC_LE = 14;  // less or equal (signed <=)
static constexpr uint8_t kCC_G  = 15;  // greater (signed >)

/// Map comparison opcode to jcc condition code for signed compare.
using chaos::il2cpp::interpreter::IROpCode;
inline uint8_t CmpToJccSigned(IROpCode op) noexcept {
    switch (op) {
    default:
    case IROpCode::Beq:    return kCC_E;
    case IROpCode::BneUn:  return kCC_NE;
    case IROpCode::Blt:    return kCC_L;
    case IROpCode::Bgt:    return kCC_G;
    case IROpCode::Ble:    return kCC_LE;
    case IROpCode::Bge:    return kCC_GE;
    case IROpCode::BltUn:  return kCC_B;
    case IROpCode::BgtUn:  return kCC_A;
    case IROpCode::BleUn:  return kCC_BE;
    case IROpCode::BgeUn:  return kCC_AE;
    case IROpCode::Ceq:    return kCC_E;
    case IROpCode::Clt:    return kCC_L;
    case IROpCode::Cgt:    return kCC_G;
    }
}

// ── Pure virtual encoder interface ─────────────────────────────────────────

class IEncoder {
public:
    virtual ~IEncoder() = default;

    /// Architecture identification for dispatch.
    virtual Arch GetArch() const noexcept = 0;

    // ── MOV ──────────────────────────────────────────────────────────────
    virtual void EmitMovRR(uint8_t dst, uint8_t src) = 0;
    virtual void EmitMovRM(uint8_t dst, uint8_t base, int32_t disp) = 0;
    virtual void EmitMovMR(uint8_t base, int32_t disp, uint8_t src) = 0;
    virtual void EmitMovImm64(uint8_t dst, uint64_t imm) = 0;
    virtual void EmitMovRI32(uint8_t dst, int32_t imm) = 0;
    virtual void EmitMovRIImm32(uint8_t dst, uint32_t imm) = 0;
    virtual void EmitMovMI32(uint8_t base, int32_t disp, uint32_t imm) = 0;
    virtual void EmitLeaRM(uint8_t dst, uint8_t base, int32_t disp) = 0;
    virtual void EmitLeaRipRel(uint8_t dst, int32_t disp) = 0;

    // ── Addition ─────────────────────────────────────────────────────────
    virtual void EmitAddRR(uint8_t dst, uint8_t src) = 0;
    virtual void EmitAddRI(uint8_t dst, int32_t imm) = 0;
    virtual void EmitAddMR(uint8_t base, int32_t disp, uint8_t src) = 0;
    virtual void EmitAdd32RR(uint8_t dst, uint8_t src) = 0;

    // ── Subtraction ──────────────────────────────────────────────────────
    virtual void EmitSubRI(uint8_t dst, int32_t imm) = 0;
    virtual void EmitSubRR(uint8_t dst, uint8_t src) = 0;
    virtual void EmitSub32RR(uint8_t dst, uint8_t src) = 0;

    // ── Multiplication ───────────────────────────────────────────────────
    virtual void EmitImulRR(uint8_t dst, uint8_t src) = 0;
    virtual void EmitImul32RR(uint8_t dst, uint8_t src) = 0;

    // ── Bitwise AND ──────────────────────────────────────────────────────
    virtual void EmitAndRR(uint8_t dst, uint8_t src) = 0;
    virtual void EmitAnd32RR(uint8_t dst, uint8_t src) = 0;

    // ── Bitwise OR ───────────────────────────────────────────────────────
    virtual void EmitOrRR(uint8_t dst, uint8_t src) = 0;
    virtual void EmitOr32RR(uint8_t dst, uint8_t src) = 0;

    // ── Bitwise XOR ──────────────────────────────────────────────────────
    virtual void EmitXorRR(uint8_t dst, uint8_t src) = 0;
    virtual void EmitXor32RR(uint8_t dst, uint8_t src) = 0;
    virtual void EmitXor32ZR(uint8_t reg) = 0;
    virtual void EmitXorMR(uint8_t base, int32_t disp, uint8_t src) = 0;
    virtual void EmitXorRM(uint8_t dst, uint8_t base, int32_t disp) = 0;
    virtual void EmitXorZR(uint8_t reg) = 0;
    virtual void EmitXorpsRR(uint8_t dst, uint8_t src) = 0;

    // ── Negation / Not ──────────────────────────────────────────────────────
    virtual void EmitNeg32(uint8_t reg) = 0;
    virtual void EmitNeg(uint8_t reg) = 0;
    virtual void EmitNot32(uint8_t reg) = 0;
    virtual void EmitNot(uint8_t reg) = 0;

    // ── Shift ────────────────────────────────────────────────────────────
    virtual void EmitShlRI(uint8_t reg, uint8_t imm) = 0;
    virtual void EmitShrRI(uint8_t reg, uint8_t imm) = 0;
    virtual void EmitSarRI(uint8_t reg, uint8_t imm) = 0;
    virtual void EmitShlRCL(uint8_t reg) = 0;
    virtual void EmitShrRCL(uint8_t reg) = 0;
    virtual void EmitSarRCL(uint8_t reg) = 0;

    // ── Comparison ───────────────────────────────────────────────────────
    virtual void EmitCmpRR(uint8_t lhs, uint8_t rhs) = 0;
    virtual void EmitCmp32RR(uint8_t lhs, uint8_t rhs) = 0;
    virtual void EmitCmpRI(uint8_t reg, int32_t imm) = 0;
    virtual void EmitCmpMI(uint8_t base, int32_t disp, int32_t imm) = 0;
    virtual void EmitTestRR(uint8_t reg1, uint8_t reg2) = 0;
    virtual void EmitSetcc(uint8_t cc, uint8_t reg) = 0;
    virtual void EmitCmovcc(uint8_t cc, uint8_t dst, uint8_t src) = 0;

    // ── Jump / Call / Return / Stack ─────────────────────────────────────
    virtual void EmitJmpRel32(int32_t offset) = 0;
    virtual void EmitJmpRel8(int8_t offset) = 0;
    virtual void EmitJccRel32(uint8_t cc, int32_t offset) = 0;
    virtual void EmitJccRel8(uint8_t cc, int8_t offset) = 0;
    virtual void EmitJmpReg(uint8_t reg) = 0;
    virtual void EmitCallRel32(int32_t offset) = 0;
    virtual void EmitCallRipRel(int32_t disp) = 0;
    virtual void EmitCallReg(uint8_t reg) = 0;
    virtual void EmitRet() = 0;
    virtual void EmitPush(uint8_t reg) = 0;
    virtual void EmitPop(uint8_t reg) = 0;

    // ── Sign extension ───────────────────────────────────────────────────
    virtual void EmitMovsxd(uint8_t dst, uint8_t src) = 0;

    // ── Float / SSE (scalar double) ──────────────────────────────────────
    virtual void EmitMovSDRR(uint8_t dst, uint8_t src) = 0;
    virtual void EmitMovSSRR(uint8_t dst, uint8_t src) = 0;
    virtual void EmitMovSDMR(uint8_t base, int32_t disp, uint8_t src) = 0;
    virtual void EmitMovSDRM(uint8_t dst, uint8_t base, int32_t disp) = 0;
    virtual void EmitMovUPSMR(uint8_t base, int32_t disp, uint8_t src) = 0;
    virtual void EmitMovUPRM(uint8_t dst, uint8_t base, int32_t disp) = 0;
    virtual void EmitMovSSMR(uint8_t base, int32_t disp, uint8_t src) = 0;
    virtual void EmitMovSSRM(uint8_t dst, uint8_t base, int32_t disp) = 0;
    virtual void EmitAddSDRR(uint8_t dst, uint8_t src) = 0;
    virtual void EmitSubSDRR(uint8_t dst, uint8_t src) = 0;
    virtual void EmitMulSDRR(uint8_t dst, uint8_t src) = 0;
    virtual void EmitDivSDRR(uint8_t dst, uint8_t src) = 0;
    virtual void EmitAddSSRR(uint8_t dst, uint8_t src) = 0;
    virtual void EmitSubSSRR(uint8_t dst, uint8_t src) = 0;
    virtual void EmitMulSSRR(uint8_t dst, uint8_t src) = 0;
    virtual void EmitDivSSRR(uint8_t dst, uint8_t src) = 0;
    virtual void EmitCvtsi2sd(uint8_t xmm, uint8_t reg) = 0;
    virtual void EmitCvtsi2ss(uint8_t xmm, uint8_t reg) = 0;
    virtual void EmitCvttsd2si(uint8_t reg, uint8_t xmm) = 0;
    virtual void EmitCvttss2si(uint8_t reg, uint8_t xmm) = 0;
    virtual void EmitCvtsd2ss(uint8_t dst, uint8_t src) = 0;
    virtual void EmitCvtss2sd(uint8_t dst, uint8_t src) = 0;
    virtual void EmitUcomisd(uint8_t lhs, uint8_t rhs) = 0;
    virtual void EmitUcomiss(uint8_t lhs, uint8_t rhs) = 0;
    virtual void EmitComisd(uint8_t lhs, uint8_t rhs) = 0;
    virtual void EmitMovdXrm(uint8_t xmm, uint8_t reg) = 0;
    virtual void EmitMovdMrx(uint8_t reg, uint8_t xmm) = 0;
    virtual void EmitMovqXrm(uint8_t xmm, uint8_t reg) = 0;
    virtual void EmitMovqMrx(uint8_t reg, uint8_t xmm) = 0;
    virtual void EmitMovdqaRR(uint8_t dst, uint8_t src) = 0;
    virtual void EmitMovdqaMR(uint8_t base, int32_t disp, uint8_t src) = 0;
    virtual void EmitMovdqaRM(uint8_t dst, uint8_t base, int32_t disp) = 0;
    virtual void EmitPxorRR(uint8_t dst, uint8_t src) = 0;

    // SSE2 integer ALU
    virtual void EmitPaddbRR(uint8_t dst, uint8_t src) = 0;
    virtual void EmitPaddwRR(uint8_t dst, uint8_t src) = 0;
    virtual void EmitPadddRR(uint8_t dst, uint8_t src) = 0;
    virtual void EmitPaddqRR(uint8_t dst, uint8_t src) = 0;
    virtual void EmitPsubbRR(uint8_t dst, uint8_t src) = 0;
    virtual void EmitPsubwRR(uint8_t dst, uint8_t src) = 0;
    virtual void EmitPsubdRR(uint8_t dst, uint8_t src) = 0;
    virtual void EmitPsubqRR(uint8_t dst, uint8_t src) = 0;
    virtual void EmitPmullwRR(uint8_t dst, uint8_t src) = 0;
    virtual void EmitPmuludqRR(uint8_t dst, uint8_t src) = 0;
    virtual void EmitPandRR(uint8_t dst, uint8_t src) = 0;
    virtual void EmitPorRR(uint8_t dst, uint8_t src) = 0;
    virtual void EmitPandnRR(uint8_t dst, uint8_t src) = 0;
    virtual void EmitPcmpeqbRR(uint8_t dst, uint8_t src) = 0;
    virtual void EmitPcmpeqwRR(uint8_t dst, uint8_t src) = 0;
    virtual void EmitPcmpeqdRR(uint8_t dst, uint8_t src) = 0;
    virtual void EmitPcmpeqqRR(uint8_t dst, uint8_t src) = 0;
    virtual void EmitPcmpgtbRR(uint8_t dst, uint8_t src) = 0;
    virtual void EmitPcmpgtwRR(uint8_t dst, uint8_t src) = 0;
    virtual void EmitPcmpgtdRR(uint8_t dst, uint8_t src) = 0;
    virtual void EmitPcmpgtqRR(uint8_t dst, uint8_t src) = 0;
    virtual void EmitPshufdRR(uint8_t dst, uint8_t src, uint8_t imm) = 0;
    virtual void EmitPabsbRR(uint8_t dst, uint8_t src) = 0;
    virtual void EmitPabswRR(uint8_t dst, uint8_t src) = 0;
    virtual void EmitPabsdRR(uint8_t dst, uint8_t src) = 0;

    // Bit manipulation
    virtual void EmitPopcntRR(uint8_t dst, uint8_t src) = 0;
    virtual void EmitLzcntRR(uint8_t dst, uint8_t src) = 0;

    // ── FMA (VEX.66.0F38 3-operand fused multiply-add) ──────────────
    // Semantics (231 form): acc = src1 * src2 + acc
    virtual void EmitVfmadd231psRR(uint8_t acc, uint8_t src1, uint8_t src2) = 0;
    virtual void EmitVfmadd231pdRR(uint8_t acc, uint8_t src1, uint8_t src2) = 0;
    virtual void EmitVfmsub231psRR(uint8_t acc, uint8_t src1, uint8_t src2) = 0;
    virtual void EmitVfmsub231pdRR(uint8_t acc, uint8_t src1, uint8_t src2) = 0;
    virtual void EmitVfnmadd231psRR(uint8_t acc, uint8_t src1, uint8_t src2) = 0;
    virtual void EmitVfnmadd231pdRR(uint8_t acc, uint8_t src1, uint8_t src2) = 0;
    virtual void EmitVfnmsub231psRR(uint8_t acc, uint8_t src1, uint8_t src2) = 0;
    virtual void EmitVfnmsub231pdRR(uint8_t acc, uint8_t src1, uint8_t src2) = 0;
};

}  // namespace chaos::il2cpp::jit

#endif  // CHAOS_IL2CPP_IENCODER_H_
