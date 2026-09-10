// jit_codegen_simd.cpp — SIMD operation dispatch for NativeCodeGenerator
// (T2.4 module split of jit_engine.cpp).
//
// Pure move: EmitSimd was relocated verbatim from jit_engine.cpp; no logic
// changed.  It emits SIMD (SSE/AVX/NEON) instructions for the Simd IR ops,
// reading the NativeCodeGenerator member state (buf_, enc_, fpr coloring) and
// calling the architecture encoder free functions in x64_encoder.h /
// arm64_encoder.h.

#include "jit_engine.h"
#include "ArchTraits.h"
#include "jit_helpers.h"
#include "code_buffer.h"
#include "IEncoder.h"
#include "jit_reg_alloc.h"
#include "../interpreter/ir_reg_alloc.h"
#include "x64_encoder.h"    // EmitMovdqaRR, ... (x64 SIMD free functions)
#include "arm64_encoder.h"  // EmitFadd4S, EmitPaddbRR, ... (SIMD free functions, both archs)
#if defined(__aarch64__)
#include "Arm64Encoder.h"
#else
#include "X64Encoder.h"
#endif

namespace chaos::il2cpp::jit {

// ── NativeCodeGenerator::EmitSimd — SIMD operation dispatch ───────────
//
// Dispatches on (simd_op, elem_type) to emit the correct SSE/SSE2/SSSE3/SSE4.1
// instruction sequence.  src1/src2 vregs are loaded from the stack frame into
// physical XMM registers, the operation is emitted, and the result is stored.
bool NativeCodeGenerator::EmitSimd(const interpreter::RegisterInstruction& instr, uint8_t simd_op, uint8_t elem_type,
                                   uint16_t simd_imm) noexcept {
    // Map virtual src/dst to physical XMM registers
    uint8_t xmm_dst = 0;  // XMM0 for SIMD operations
    uint8_t xmm_src1 = 1; // XMM1
    uint8_t xmm_src2 = 2; // XMM2 (for binary ops)

    // Load source operands from stack frame into XMM registers
    LoadFpr(xmm_src1, instr.src1_reg());
    if (instr.has_src2())
        LoadFpr(xmm_src2, instr.src2_reg());

    switch (simd_op) {
        // ── Arithmetic ──────────────────────────────────────────────────
        case 1: // kSimdAdd
            if (kUseVexEncoding) {
                switch (elem_type) {
                    case 0:
                        EmitVPaddbRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 1:
                        EmitVPaddwRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 2:
                        EmitVPadddRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 3:
                        EmitVPaddqRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 4:
                        EmitVAddpsRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 5:
                        EmitVAddpdRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    default:
                        return false;
                }
            } else {
                switch (elem_type) {
                    case 0:
                        enc_.EmitPaddbRR(xmm_src1, xmm_src2);
                        break;
                    case 1:
                        enc_.EmitPaddwRR(xmm_src1, xmm_src2);
                        break;
                    case 2:
                        enc_.EmitPadddRR(xmm_src1, xmm_src2);
                        break;
                    case 3:
                        enc_.EmitPaddqRR(xmm_src1, xmm_src2);
                        break;
                    case 4:
                        EmitFadd4S(buf_, xmm_src1, xmm_src1, xmm_src2);
                        break;
                    case 5:
                        EmitFadd2D(buf_, xmm_src1, xmm_src1, xmm_src2);
                        break;
                    default:
                        return false;
                }
            }
            break;

        case 2: // kSimdSub
            if (kUseVexEncoding) {
                switch (elem_type) {
                    case 0:
                        EmitVPsubbRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 1:
                        EmitVPsubwRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 2:
                        EmitVPsubdRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 3:
                        EmitVPsubqRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 4:
                        EmitVSubpsRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 5:
                        EmitVSubpdRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    default:
                        return false;
                }
            } else {
                switch (elem_type) {
                    case 0:
                        enc_.EmitPsubbRR(xmm_src1, xmm_src2);
                        break;
                    case 1:
                        enc_.EmitPsubwRR(xmm_src1, xmm_src2);
                        break;
                    case 2:
                        enc_.EmitPsubdRR(xmm_src1, xmm_src2);
                        break;
                    case 3:
                        enc_.EmitPsubqRR(xmm_src1, xmm_src2);
                        break;
                    case 4:
                        EmitFsub4S(buf_, xmm_src1, xmm_src1, xmm_src2);
                        break;
                    case 5:
                        EmitFsub2D(buf_, xmm_src1, xmm_src1, xmm_src2);
                        break;
                    default:
                        return false;
                }
            }
            break;

        case 3: // kSimdMul
            if (kUseVexEncoding) {
                switch (elem_type) {
                    case 1:
                        EmitVPmullwRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 2:
                        EmitVPmuludqRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 4:
                        EmitVMulpsRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 5:
                        EmitVMulpdRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    default:
                        return false;
                }
            } else {
                switch (elem_type) {
                    case 1:
                        enc_.EmitPmullwRR(xmm_src1, xmm_src2);
                        break;
                    case 2:
                        enc_.EmitPmuludqRR(xmm_src1, xmm_src2);
                        break;
                    case 4:
                        EmitFmul4S(buf_, xmm_src1, xmm_src1, xmm_src2);
                        break;
                    case 5:
                        EmitFmul2D(buf_, xmm_src1, xmm_src1, xmm_src2);
                        break;
                    default:
                        return false;
                }
            }
            break;

        // ── Bitwise ────────────────────────────────────────────────────
        case 4: // kSimdAnd
            if (kUseVexEncoding)
                EmitVPandRR(buf_, xmm_dst, xmm_src1, xmm_src2);
            else
                enc_.EmitPandRR(xmm_src1, xmm_src2);
            break;
        case 5: // kSimdOr
            if (kUseVexEncoding)
                EmitVPorRR(buf_, xmm_dst, xmm_src1, xmm_src2);
            else
                enc_.EmitPorRR(xmm_src1, xmm_src2);
            break;
        case 6: // kSimdXor
            if (kUseVexEncoding)
                EmitVPxorRR(buf_, xmm_dst, xmm_src1, xmm_src2);
            else
                enc_.EmitPxorRR(xmm_src1, xmm_src2);
            break;
        case 7: // kSimdAndNot
            if (kUseVexEncoding)
                EmitVPandnRR(buf_, xmm_dst, xmm_src1, xmm_src2);
            else {
#if defined(__aarch64__)
                // BIC = rn & ~rm.  PANDN(src1, src2) = ~src1 & src2 = BIC(src2, src1).
                // So BIC(xmm_src1, xmm_src2, xmm_src1) → xmm_src2 & ~xmm_src1.
                EmitBic16B(buf_, xmm_src1, xmm_src2, xmm_src1);
#else
                enc_.EmitPandnRR(xmm_src1, xmm_src2);
#endif
            }
            break;

        // ── Compare (integer + float/double) ─────────────────────────────
        case 8: // kSimdEq
            if (kUseVexEncoding) {
                switch (elem_type) {
                    case 0:
                        EmitVPcmpeqbRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 1:
                        EmitVPcmpeqwRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 2:
                        EmitVPcmpeqdRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 3:
                        EmitVPcmpeqqRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 4:
                        EmitVCmppsRR(buf_, xmm_dst, xmm_src1, xmm_src2, 0);
                        break; // EQ_OQ
                    case 5:
                        EmitVCmppdRR(buf_, xmm_dst, xmm_src1, xmm_src2, 0);
                        break; // EQ_OQ
                    default:
                        return false;
                }
            } else {
                switch (elem_type) {
                    case 0:
                        enc_.EmitPcmpeqbRR(xmm_src1, xmm_src2);
                        break;
                    case 1:
                        enc_.EmitPcmpeqwRR(xmm_src1, xmm_src2);
                        break;
                    case 2:
                        enc_.EmitPcmpeqdRR(xmm_src1, xmm_src2);
                        break;
                    case 3:
                        enc_.EmitPcmpeqqRR(xmm_src1, xmm_src2);
                        break;
                    case 4:
                        EmitFcmeq4S(buf_, xmm_src1, xmm_src1, xmm_src2);
                        break;
                    case 5:
                        EmitFcmeq2D(buf_, xmm_src1, xmm_src1, xmm_src2);
                        break;
                    default:
                        return false;
                }
            }
            break;

        case 9: // kSimdGt
            if (kUseVexEncoding) {
                switch (elem_type) {
                    case 0:
                        EmitVPcmpgtbRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 1:
                        EmitVPcmpgtwRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 2:
                        EmitVPcmpgtdRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 3:
                        EmitVPcmpgtqRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    // SSE compare has no direct GT predicate; use LT_OS with swapped operands:
                    // vcmpps dest, src2, src1, 1  =>  src2 < src1  =>  src1 > src2
                    case 4:
                        EmitVCmppsRR(buf_, xmm_dst, xmm_src2, xmm_src1, 1);
                        break;
                    case 5:
                        EmitVCmppdRR(buf_, xmm_dst, xmm_src2, xmm_src1, 1);
                        break;
                    default:
                        return false;
                }
            } else {
                switch (elem_type) {
                    case 0:
                        enc_.EmitPcmpgtbRR(xmm_src1, xmm_src2);
                        break;
                    case 1:
                        enc_.EmitPcmpgtwRR(xmm_src1, xmm_src2);
                        break;
                    case 2:
                        enc_.EmitPcmpgtdRR(xmm_src1, xmm_src2);
                        break;
                    case 3:
                        enc_.EmitPcmpgtqRR(xmm_src1, xmm_src2);
                        break;
                    case 4:
                        EmitFcmgt4S(buf_, xmm_src1, xmm_src1, xmm_src2);
                        break;
                    case 5:
                        EmitFcmgt2D(buf_, xmm_src1, xmm_src1, xmm_src2);
                        break;
                    default:
                        return false;
                }
            }
            break;

        // ── Shuffle ─────────────────────────────────────────────────────
        case 10: // kSimdShuffle (pshufd — dword shuffle with imm8)
            if (kUseVexEncoding)
                EmitVPshufdRR(buf_, xmm_dst, xmm_src1, static_cast<uint8_t>(simd_imm & 0xFF));
            else
                enc_.EmitPshufdRR(xmm_src1, xmm_src1, static_cast<uint8_t>(simd_imm & 0xFF));
            break;

        case 11: // kSimdShuffleB (pshufb — SSSE3 byte shuffle)
            if (elem_type != 0)
                return false;
            if (kUseVexEncoding)
                EmitVPshufbRR(buf_, xmm_dst, xmm_src1, xmm_src2);
            else
                EmitPshufbRR(buf_, xmm_src1, xmm_src2);
            break;

        // ── Unpack / Interleave ─────────────────────────────────────────
        case 12: // kSimdUnpackLo
            if (kUseVexEncoding) {
                switch (elem_type) {
                    case 0:
                        EmitVPunpcklbwRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 1:
                        EmitVPunpcklwdRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 2:
                        EmitVPunpckldqRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 3:
                        EmitVPunpcklqdqRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    default:
                        return false;
                }
            } else {
#if !defined(__aarch64__)
                switch (elem_type) {
                    case 0:
                        EmitPunpcklbwRR(buf_, xmm_src1, xmm_src2);
                        break;
                    case 1:
                        EmitPunpcklwdRR(buf_, xmm_src1, xmm_src2);
                        break;
                    case 2:
                        EmitPunpckldqRR(buf_, xmm_src1, xmm_src2);
                        break;
                    case 3:
                        EmitPunpcklqdqRR(buf_, xmm_src1, xmm_src2);
                        break;
                    default:
                        return false;
                }
#else
                return false;
#endif
            }
            break;

        case 13: // kSimdUnpackHi
            if (kUseVexEncoding) {
                switch (elem_type) {
                    case 0:
                        EmitVPunpckhbwRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 1:
                        EmitVPunpckhwdRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 2:
                        EmitVPunpckhdqRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 3:
                        EmitVPunpckhqdqRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    default:
                        return false;
                }
            } else {
#if !defined(__aarch64__)
                switch (elem_type) {
                    case 0:
                        EmitPunpckhbwRR(buf_, xmm_src1, xmm_src2);
                        break;
                    case 1:
                        EmitPunpckhwdRR(buf_, xmm_src1, xmm_src2);
                        break;
                    case 2:
                        EmitPunpckhdqRR(buf_, xmm_src1, xmm_src2);
                        break;
                    case 3:
                        EmitPunpckhqdqRR(buf_, xmm_src1, xmm_src2);
                        break;
                    default:
                        return false;
                }
#else
                return false;
#endif
            }
            break;

        // ── Pack with signed saturation ─────────────────────────────────
        case 14: // kSimdPackS
            if (kUseVexEncoding) {
                switch (elem_type) {
                    case 1:
                        EmitVPacksswbRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 2:
                        EmitVPackssdwRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    default:
                        return false;
                }
            } else {
#if !defined(__aarch64__)
                switch (elem_type) {
                    case 1:
                        EmitPacksswbRR(buf_, xmm_src1, xmm_src2);
                        break;
                    case 2:
                        EmitPackssdwRR(buf_, xmm_src1, xmm_src2);
                        break;
                    default:
                        return false;
                }
#else
                return false;
#endif
            }
            break;

        // ── Absolute value ──────────────────────────────────────────────
        case 15: // kSimdAbs
            if (kUseVexEncoding) {
                switch (elem_type) {
                    case 0:
                        EmitVPabsbRR(buf_, xmm_dst, xmm_src1);
                        break;
                    case 1:
                        EmitVPabswRR(buf_, xmm_dst, xmm_src1);
                        break;
                    case 2:
                        EmitVPabsdRR(buf_, xmm_dst, xmm_src1);
                        break;
                    default:
                        return false;
                }
            } else {
                switch (elem_type) {
                    case 0:
                        enc_.EmitPabsbRR(xmm_src1, xmm_src1);
                        break;
                    case 1:
                        enc_.EmitPabswRR(xmm_src1, xmm_src1);
                        break;
                    case 2:
                        enc_.EmitPabsdRR(xmm_src1, xmm_src1);
                        break;
#if defined(__aarch64__)
                    case 4:
                        EmitFabs4S(buf_, xmm_src1, xmm_src1);
                        break;
                    case 5:
                        EmitFabs2D(buf_, xmm_src1, xmm_src1);
                        break;
#endif
                    default:
                        return false;
                }
            }
            break;

        // ── Packed shift by XMM count ───────────────────────────────────
        case 16: // kSimdShl (shift left logical)
            if (kUseVexEncoding) {
                switch (elem_type) {
                    case 1:
                        EmitVPsllwRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 2:
                        EmitVPslldRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 3:
                        EmitVPsllqRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    default:
                        return false;
                }
            } else {
#if !defined(__aarch64__)
                switch (elem_type) {
                    case 1:
                        EmitPsllwRR(buf_, xmm_src1, xmm_src2);
                        break;
                    case 2:
                        EmitPslldRR(buf_, xmm_src1, xmm_src2);
                        break;
                    case 3:
                        EmitPsllqRR(buf_, xmm_src1, xmm_src2);
                        break;
                    default:
                        return false;
                }
#else
                return false;
#endif
            }
            break;

        case 17: // kSimdShr (shift right logical)
            if (kUseVexEncoding) {
                switch (elem_type) {
                    case 1:
                        EmitVPsrlwRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 2:
                        EmitVPsrldRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 3:
                        EmitVPsrlqRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    default:
                        return false;
                }
            } else {
#if !defined(__aarch64__)
                switch (elem_type) {
                    case 1:
                        EmitPsrlwRR(buf_, xmm_src1, xmm_src2);
                        break;
                    case 2:
                        EmitPsrldRR(buf_, xmm_src1, xmm_src2);
                        break;
                    case 3:
                        EmitPsrlqRR(buf_, xmm_src1, xmm_src2);
                        break;
                    default:
                        return false;
                }
#else
                return false;
#endif
            }
            break;

        case 18: // kSimdSar (shift right arithmetic)
            if (kUseVexEncoding) {
                switch (elem_type) {
                    case 1:
                        EmitVPsrawRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 2:
                        EmitVPsradRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    default:
                        return false;
                }
            } else {
#if !defined(__aarch64__)
                switch (elem_type) {
                    case 1:
                        EmitPsrawRR(buf_, xmm_src1, xmm_src2);
                        break;
                    case 2:
                        EmitPsradRR(buf_, xmm_src1, xmm_src2);
                        break;
                    default:
                        return false;
                }
#else
                return false;
#endif
            }
            break;

        // ── Extract element to GPR ──────────────────────────────────────
        case 19: { // kSimdExtract
            // Load XMM source (already in xmm_src1 from the top of this function).
            // Extract the element at index simd_imm into RAX, then store to reg file.
            switch (elem_type) {
                case 0:
                    if (kUseVexEncoding)
                        EmitVPextrbRR(buf_, AT::kScratchA, xmm_src1, static_cast<uint8_t>(simd_imm & 0xFF));
                    else
#if defined(__aarch64__)
                        EmitUmovB(buf_, AT::kScratchA, xmm_src1, static_cast<uint8_t>(simd_imm));
#else
                        EmitPextrbRR(buf_, AT::kScratchA, xmm_src1, static_cast<uint8_t>(simd_imm & 0xFF));
#endif
                    break;
                case 1:
                    if (kUseVexEncoding)
                        EmitVPextrwRR(buf_, AT::kScratchA, xmm_src1, static_cast<uint8_t>(simd_imm & 0xFF));
                    else
#if defined(__aarch64__)
                        EmitUmovH(buf_, AT::kScratchA, xmm_src1, static_cast<uint8_t>(simd_imm));
#else
                        EmitPextrwRR(buf_, AT::kScratchA, xmm_src1, static_cast<uint8_t>(simd_imm & 0xFF));
#endif
                    break;
                case 2:
                    if (kUseVexEncoding)
                        EmitVPextrdRR(buf_, AT::kScratchA, xmm_src1, static_cast<uint8_t>(simd_imm & 0xFF));
                    else
#if defined(__aarch64__)
                        EmitUmovS(buf_, AT::kScratchA, xmm_src1, static_cast<uint8_t>(simd_imm));
#else
                        EmitPextrdRR(buf_, AT::kScratchA, xmm_src1, static_cast<uint8_t>(simd_imm & 0xFF));
#endif
                    break;
#if defined(__aarch64__)
                case 3:
                    EmitUmovD(buf_, AT::kScratchA, xmm_src1, static_cast<uint8_t>(simd_imm));
                    break;
#endif
                default:
                    return false;
            }
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        // ── Insert scalar from GPR ──────────────────────────────────────
        case 20: { // kSimdInsert
            // Load destination XMM, load source GPR, insert, store back.
            LoadFpr(xmm_src1, instr.dst_reg()); // re-load dest as source
            uint32_t gpr_vreg = instr.src2_reg();
            LoadGpr(AT::kScratchA, gpr_vreg);
            switch (elem_type) {
                case 0:
                    if (kUseVexEncoding)
                        EmitVPinsrbRR(buf_, xmm_src1, xmm_src1, AT::kScratchA, static_cast<uint8_t>(simd_imm & 0xFF));
                    else
#if defined(__aarch64__)
                        EmitInsB(buf_, xmm_src1, AT::kScratchA, static_cast<uint8_t>(simd_imm));
#else
                        EmitPinsrbRR(buf_, xmm_src1, AT::kScratchA, static_cast<uint8_t>(simd_imm & 0xFF));
#endif
                    break;
                case 1:
                    if (kUseVexEncoding)
                        EmitVPinsrwRR(buf_, xmm_src1, xmm_src1, AT::kScratchA, static_cast<uint8_t>(simd_imm & 0xFF));
                    else
#if defined(__aarch64__)
                        EmitInsH(buf_, xmm_src1, AT::kScratchA, static_cast<uint8_t>(simd_imm));
#else
                        EmitPinsrwRR(buf_, xmm_src1, AT::kScratchA, static_cast<uint8_t>(simd_imm & 0xFF));
#endif
                    break;
                case 2:
                    if (kUseVexEncoding)
                        EmitVPinsrdRR(buf_, xmm_src1, xmm_src1, AT::kScratchA, static_cast<uint8_t>(simd_imm & 0xFF));
                    else
#if defined(__aarch64__)
                        EmitInsS(buf_, xmm_src1, AT::kScratchA, static_cast<uint8_t>(simd_imm));
#else
                        EmitPinsrdRR(buf_, xmm_src1, AT::kScratchA, static_cast<uint8_t>(simd_imm & 0xFF));
#endif
                    break;
#if defined(__aarch64__)
                case 3:
                    EmitInsD(buf_, xmm_src1, AT::kScratchA, static_cast<uint8_t>(simd_imm));
                    break;
#endif
                default:
                    return false;
            }
            StoreFpr(instr.dst_reg(), xmm_src1);
            return true;
        }

        // ── Move byte mask to GPR ───────────────────────────────────────
        case 21: { // kSimdMoveMask
            if (elem_type != 0)
                return false;
#if !defined(__aarch64__)
            if (kUseVexEncoding)
                EmitVPmovmskbRR(buf_, AT::kScratchA, xmm_src1);
            else
                EmitPmovmskbRR(buf_, AT::kScratchA, xmm_src1);
#else
            return false;
#endif
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        // ── Zero XMM ────────────────────────────────────────────────────
        case 24: // kSimdZero
            if (kUseVexEncoding)
                EmitVPxorRR(buf_, xmm_dst, xmm_dst, xmm_dst);
            else
                enc_.EmitPxorRR(xmm_dst, xmm_dst);
            StoreFpr(instr.dst_reg(), xmm_dst);
            return true;

        // ── Load/Store (movdqa) ─────────────────────────────────────────
        case 22: { // kSimdLoad — load from memory at address in src1
            uint32_t addr_vreg = instr.src1_reg();
            // Load address into RAX from the register file
            LoadGpr(AT::kScratchA, addr_vreg);
            if (kUseVexEncoding) {
                buf_.EmitVEX_66_0F(xmm_dst, 0, xmm_dst);
                buf_.EmitByte(0x6F); // vmovdqa
                buf_.EmitByte(ModRM(0, xmm_dst, AT::kScratchA));
            } else {
                enc_.EmitMovdqaRM(xmm_dst, AT::kScratchA, 0);
            }
            StoreFpr(instr.dst_reg(), xmm_dst);
            return true;
        }

        case 23: { // kSimdStore — store to memory at address in src1
            uint32_t addr_vreg = instr.src1_reg();
            LoadGpr(AT::kScratchA, addr_vreg);
            if (kUseVexEncoding) {
                buf_.EmitVEX_66_0F(xmm_src1, 0, xmm_src1);
                buf_.EmitByte(0x7F); // vmovdqa store
                buf_.EmitByte(ModRM(0, xmm_src1, AT::kScratchA));
            } else {
                enc_.EmitMovdqaMR(AT::kScratchA, 0, xmm_src1);
            }
            return true;
        }

        case 25: // kSimdMin
            if (kUseVexEncoding) {
                switch (elem_type) {
                    // VEX integer SIMD min/max encoder functions not yet available (x64 future work).
                    case 4:
                        EmitVMinpsRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 5:
                        EmitVMinpdRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    default:
                        return false;
                }
            } else {
                switch (elem_type) {
#if defined(__aarch64__)
                    case 0:
                        EmitSmin16B(buf_, xmm_src1, xmm_src1, xmm_src2);
                        break;
                    case 1:
                        EmitSmin8H(buf_, xmm_src1, xmm_src1, xmm_src2);
                        break;
                    case 2:
                        EmitSmin4S(buf_, xmm_src1, xmm_src1, xmm_src2);
                        break;
                    case 4:
                        EmitFmin4S(buf_, xmm_src1, xmm_src1, xmm_src2);
                        break;
                    case 5:
                        EmitFmin2D(buf_, xmm_src1, xmm_src1, xmm_src2);
                        break;
#endif
                    default:
                        return false;
                }
            }
            break;

        case 26: // kSimdMax
            if (kUseVexEncoding) {
                switch (elem_type) {
                    // VEX integer SIMD min/max encoder functions not yet available (x64 future work).
                    case 4:
                        EmitVMaxpsRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 5:
                        EmitVMaxpdRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    default:
                        return false;
                }
            } else {
                switch (elem_type) {
#if defined(__aarch64__)
                    case 0:
                        EmitSmax16B(buf_, xmm_src1, xmm_src1, xmm_src2);
                        break;
                    case 1:
                        EmitSmax8H(buf_, xmm_src1, xmm_src1, xmm_src2);
                        break;
                    case 2:
                        EmitSmax4S(buf_, xmm_src1, xmm_src1, xmm_src2);
                        break;
                    case 4:
                        EmitFmax4S(buf_, xmm_src1, xmm_src1, xmm_src2);
                        break;
                    case 5:
                        EmitFmax2D(buf_, xmm_src1, xmm_src1, xmm_src2);
                        break;
#endif
                    default:
                        return false;
                }
            }
            break;

        case 27: // kSimdDiv
            if (kUseVexEncoding) {
                switch (elem_type) {
                    case 4:
                        EmitVDivpsRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 5:
                        EmitVDivpdRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    default:
                        return false;
                }
            } else {
#if defined(__aarch64__)
                // ARM64 NEON has no packed FDIV.  Use Newton-Raphson reciprocal:
                //   xmm_dst = FRECPE(denom)              → 1/denom (initial estimate)
                //   xmm_src2 = FRECPS(denom, xmm_dst)     → 2 - denom * xmm_dst (refinement step)
                //   xmm_dst = FMUL(xmm_dst, xmm_src2)     → refined reciprocal
                //   xmm_src1 = FMUL(numer, xmm_dst)       → numer/denom in xmm_src1 (for StoreFpr)
                switch (elem_type) {
                    case 4: // float32x4
                        EmitFrecpe4S(buf_, xmm_dst, xmm_src2);
                        EmitFrecps4S(buf_, xmm_src2, xmm_src2, xmm_dst);
                        EmitFmul4S(buf_, xmm_dst, xmm_dst, xmm_src2);
                        EmitFmul4S(buf_, xmm_src1, xmm_src1, xmm_dst);
                        break;
                    case 5: // float64x2
                        EmitFrecpe2D(buf_, xmm_dst, xmm_src2);
                        EmitFrecps2D(buf_, xmm_src2, xmm_src2, xmm_dst);
                        EmitFmul2D(buf_, xmm_dst, xmm_dst, xmm_src2);
                        EmitFmul2D(buf_, xmm_src1, xmm_src1, xmm_dst);
                        break;
                    default:
                        return false;
                }
#else
                // ARM64 NEON has no packed float division
                return false;
#endif
            }
            break;

        default:
            return false; // unsupported SIMD operation
    }

    // Store result: VEX→XMM0, legacy→XMM1
    StoreFpr(instr.dst_reg(), kUseVexEncoding ? xmm_dst : xmm_src1);
    return true;
}
} // namespace chaos::il2cpp::jit
