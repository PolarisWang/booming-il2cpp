// ABI exports: extern "C" linkage for managed/NativeAOT callability.
#ifndef NOMINMAX
#define NOMINMAX 1
#endif
#include "jit_engine.h"
#include "x64_encoder.h"
#include "code_buffer.h"
#include "jit_helpers.h"
#include "jit_seh.h"
#include "jit_unwind.h"
#include "slot_map.h"
#include "tree/jit_optimizer.h"
#include "jit_inline.h" // g_inline_reverse_map
#include "IEncoder.h"
#include "ISehHandler.h"
#include "X64Encoder.h"
#include "arm64_encoder.h" // EmitFcmgt4S, EmitFcmeq4S, etc. — free-function API used on both x64 and ARM64
#if defined(__aarch64__)
#include "Arm64Encoder.h" // Arm64Encoder class wrapper (IEncoder impl, only needed on ARM64)
#endif

#include <gc_root_scanner.h>
#include <gc/gc_bgc.h>
#include <gc/gc_card_table.h>

#include "../interpreter/ir_reg_alloc.h"
#include "../interpreter/interpreter_vm.h"
#include "jit_reg_alloc.h"

#include <codegen_bridge.h>
#include <instantiation_engine.h>
#include <chaos/log.h>
#include <chaos/profile.h>

#include <cstdint>
#include <cstring>
#include <vector>

#if defined(_WIN64)
#include <windows.h>
#endif

// GCC unwinder .eh_frame registration — no header declares these on all GCC versions.
#if defined(__linux__)
extern "C" void __register_frame(const void*);
extern "C" void __deregister_frame(const void*);
#endif

namespace chaos::il2cpp::jit {

// Register convention constants (mirrors ir_reg_alloc.h convention)
// r0-r7   = argument registers (mapped from LdArg operand_index)
// r8-r15  = local variable registers (mapped from LdLoc/StLoc operand_index)
// r16+    = evaluation stack virtual registers
static constexpr uint32_t kArgRegCount = 8;
static constexpr uint32_t kLocalRegBase = 8;

// Global reverse slot map: callee token → (JitMethod*, slot_index).
// Used by the hotpatch callback to update RX slot tables when a method is patched.
ReverseSlotMap g_reverse_slot_map;

// ARM64: patch a forward B.cond instruction (local forward jump).
// On ARM64, B.cond has a 19-bit imm19 field (±1MB).  For forward jumps
// within the same basic block (always < 1MB), reconstruct the instruction
// with the correct imm19 encoding.
#if defined(__aarch64__)
inline void PatchArm64Bcond(CodeBuffer& buf, uint32_t patch_pos, uint32_t target_pos) noexcept {
    int32_t disp = static_cast<int32_t>(target_pos - (patch_pos + 4));
    uint32_t instr = buf.Load32(patch_pos);
    uint32_t imm19 = (static_cast<uint32_t>(disp) >> 2) & 0x7FFFF;
    buf.Patch32(patch_pos, (instr & 0xFF00001Fu) | (imm19 << 5));
}
/// ARM64: patch a forward B (unconditional) instruction.
/// B has a 26-bit imm26 field (±128MB), sufficient for any method-local jump.
inline void PatchArm64B(CodeBuffer& buf, uint32_t patch_pos, uint32_t target_pos) noexcept {
    int32_t disp = static_cast<int32_t>(target_pos - (patch_pos + 4));
    uint32_t instr = buf.Load32(patch_pos);
    uint32_t imm26 = (static_cast<uint32_t>(disp) >> 2) & 0x3FFFFFF;
    buf.Patch32(patch_pos, (instr & 0xFC000000u) | imm26);
}
#endif

// ── Codename glossary (expansion for the terse tags used below) ─────────
// V1 : the original register-CACHING allocator (fixed kCacheableRegs, freq-based
//      hit tracking, write-through to the GPR stack file on spill).
// V2 : the Chaitin-Briggs GRAPH-COLORING allocator (jit_reg_alloc.h) that
//      replaces V1 when enable_register_caching is active; unchanged on tiers.
// A1 : T2.1 increment that eliminated the caller-colored write-through for
//      scalar-only, call-free methods (has_caller_clobber_ gate).
// T2.x : roadmap milestone tags (T2.1 register-residency, T2.2 precise
//      per-safepoint GC root maps, T2.3 spill/deopt contract + hot-update).
// T4 : the fully-JIT-compiled native code tier (JIT Tier 4), as opposed to
//      the interpreter (T1-T3 lowering).  "T4 code" = the emitted native
//      machine code; "T4 frame" = a stack frame running it.
// kQuick : CompileTier::kQuick — the quick (no-optimizer) compile tier.

// Internal class that drives code generation.

bool NativeCodeGenerator::EmitInstruction(const interpreter::RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::EmitInstruction");
    using IROpCode = interpreter::IROpCode;
    auto opc = instr.op_code();
    auto& _buf = this->buf_;
    auto& _instr = instr;
    auto& _config = this->config_;
    auto& _rm = this->rm_;
    auto& _call_sites = this->call_sites_;
    auto& _branch_patches = this->branch_patches_;
    auto& _deopt_entries = this->deopt_entries_;
    auto& _deopt_values = this->deopt_values_;
    switch (opc) {
        case IROpCode::LdcI4: {
            if (!instr.has_dst())
                return false;
            // Load int32_t zero-extended to 64-bit, matching x64 mov r32,imm32
            // and interpreter WRITE_REG semantics.
#if defined(__aarch64__)
            enc_.EmitMovRIImm32(AT::kScratchA, static_cast<uint32_t>(instr.imm.i4));
#else
            enc_.EmitMovRIImm32(AT::kScratchA, static_cast<uint32_t>(instr.imm.i4));
#endif
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::LdcI8: {
            if (!instr.has_dst())
                return false;
            int64_t val = instr.imm.i8;
            if (val >= INT32_MIN && val <= INT32_MAX)
                enc_.EmitMovRI32(AT::kScratchA, static_cast<int32_t>(val));
            else
                enc_.EmitMovImm64(AT::kScratchA, static_cast<uint64_t>(val));
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::LdcR4: {
            if (!instr.has_dst())
                return false;
            // Load float constant, promote to double (matching RegisterExecute's
            // WriteFloat64(static_cast<double>(v), dst) semantics).
            float v;
            std::memcpy(&v, &instr.imm.i4, sizeof(v));
            uint32_t bits;
            std::memcpy(&bits, &v, sizeof(bits));
            enc_.EmitMovRIImm32(AT::kScratchA, bits);
            enc_.EmitMovdXrm(0, AT::kScratchA);
            enc_.EmitCvtss2sd(0, 0); // promote float→double
            StoreFpr(0, instr.dst_reg());
            return true;
        }

        case IROpCode::LdcR8: {
            if (!instr.has_dst())
                return false;
            uint64_t bits;
            std::memcpy(&bits, &instr.imm.r8, sizeof(bits));
            enc_.EmitMovImm64(AT::kScratchA, bits);
            enc_.EmitMovqXrm(0, AT::kScratchA);
            StoreFpr(0, instr.dst_reg());
            return true;
        }

        case IROpCode::LdNull: {
            if (!instr.has_dst())
                return false;
            enc_.EmitXorZR(AT::kScratchA);
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::LdArg: {
            if (!instr.has_dst())
                return false;
            uint32_t arg_idx = instr.imm.operand_index;
            enc_.EmitMovRM(AT::kScratchA, AT::kArgsBuf, static_cast<int32_t>(arg_idx * 8));
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::LdLoc: {
            if (!instr.has_dst())
                return false;
            uint32_t local_idx = instr.imm.operand_index;
            uint32_t vreg = kLocalRegBase + local_idx;
            if (vreg < kGprCount) {
                LoadGpr(AT::kScratchA, vreg);
                StoreGpr(AT::kScratchA, instr.dst_reg());
            }
            return true;
        }

        case IROpCode::StLoc: {
            if (!instr.has_src1())
                return false;
            uint32_t local_idx = instr.imm.operand_index;
            uint32_t vreg = kLocalRegBase + local_idx;
            if (vreg < kGprCount) {
                LoadGpr(AT::kScratchA, instr.src1_reg());
                StoreGpr(AT::kScratchA, vreg);
                if (instr.has_dst())
                    StoreGpr(AT::kScratchA, instr.dst_reg());
            }
            return true;
        }

        case IROpCode::StArg: {
            if (!instr.has_src1())
                return false;
            LoadGpr(AT::kScratchA, instr.src1_reg());
            enc_.EmitMovMR(AT::kArgsBuf, static_cast<int32_t>(instr.imm.operand_index * 8), AT::kScratchA);
            return true;
        }

        case IROpCode::Ret: {
            // Spill cached regs before reading return value
            if (config_.enable_register_caching && cached_slots_used_)
                SpillCachedRegs();
            if (instr.has_src1()) {
                LoadGpr(AT::kScratchA, instr.src1_reg());
#if defined(__aarch64__)
                // Load saved ABI ret_buf (X1) from [X29, #-16], saved right after
                // MOV X29, SP in the prologue.  Write the return value through it
                // (matching x64 saved-RSI reload in the Ret handler).
                EmitLdur64(buf_, AT::kScratchC, AT::kFrameReg, -16); // LDR XscratchC, [X29, #-16]
                EmitStr64(buf_, AT::kScratchA, AT::kScratchC, 0);    // STR Xretval, [XscratchC]
#else
                // kRetBuf (RSI) is caller-saved and may have been clobbered by EnterCooperativeMode
                enc_.EmitMovRM(AT::kScratchC, AT::kFrameReg, -16);
                enc_.EmitMovMR(AT::kScratchC, 0, AT::kScratchA);
#endif
            }
            // Restore callee-saved XMMs
            for (uint32_t si = 0; si < num_fpr_callee_; ++si) {
                int32_t off = static_cast<int32_t>(kFrameSize + frame_size_extra_ + frame_align_adj_ + si * 16);
                enc_.EmitMovUPRM(callee_xmm_regs_[si], AT::kStackReg, off);
            }
#if defined(__aarch64__)
            enc_.EmitAddRI(AT::kStackReg, static_cast<int32_t>(kFrameSize + frame_size_extra_ + frame_align_adj_ +
                                                               xmm_save_size_ + localloc_extra_));
            for (uint32_t slot = num_cache_regs_; slot > 0; --slot)
                EmitLdp64Post(buf_, callee_saved_regs_[slot - 1], 0, kARM64_SP,
                              16);                    // LDP Xreg, X0, [SP], #16 (X0=scratch, never XZR)
            EmitLdp64Post(buf_, 1, 0, kARM64_SP, 16); // LDP X1, X0, [SP], #16 (discard saved ABI ret_buf & scratch)
            EmitLdp64Post(buf_, AT::kFrameReg, 30, kARM64_SP, 16); // LDP X29, X30, [SP], #16
            enc_.EmitRet();
#else
            enc_.EmitAddRI(AT::kStackReg, static_cast<int32_t>(kFrameSize + frame_size_extra_ + frame_align_adj_ +
                                                               xmm_save_size_ + localloc_extra_));
            for (uint32_t slot = num_cache_regs_; slot > 0; --slot)
                enc_.EmitPop(callee_saved_regs_[slot - 1]);
            enc_.EmitPop(kRDI);
            enc_.EmitPop(AT::kRetBuf);
            enc_.EmitPop(AT::kArgsBuf);
            enc_.EmitPop(AT::kFrameReg);
            enc_.EmitRet();
#endif
            return true;
        }

        case IROpCode::Add:
        case IROpCode::Sub:
        case IROpCode::Mul:
        case IROpCode::Div:
        case IROpCode::Rem:
        case IROpCode::Neg:
        case IROpCode::DivUn:
        case IROpCode::RemUn: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            if (instr.src1_reg() < vreg_types_.size() && vreg_types_[instr.src1_reg()] >= kTypeFloat32)
                EmitFloatingArithmetic(opc, instr.dst_reg(), instr.src1_reg(),
                                       instr.has_src2() ? instr.src2_reg() : UINT32_MAX);
            else
                EmitIntegerArithmetic(opc, instr.dst_reg(), instr.src1_reg(),
                                      instr.has_src2() ? instr.src2_reg() : UINT32_MAX);
            return true;
        }

        // ── Intrinsic: Abs / Min / Max (int32) ─────────────────────────
        case IROpCode::Abs: {
            // int32 abs: cdq (sign-extend eax→edx:eax), xor eax, edx, sub eax, edx
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            LoadGpr(AT::kScratchA, instr.src1_reg());
#if defined(__aarch64__)
            // ARM64 abs(Wd): CMP Wsrc, #0 → CSNEG Wd, Wsrc, Wsrc, GE
            // Save original in scratchC, negate to scratchA, conditionally select
            enc_.EmitMovRR(AT::kScratchC, AT::kScratchA);          // copy val
            enc_.EmitNeg32(AT::kScratchA);                         // scratchA = -val
            enc_.EmitCmp32RR(AT::kScratchC, 31);                   // CMP WscratchC, WZR (31)
            enc_.EmitCmovcc(kCC_GE, AT::kScratchA, AT::kScratchC); // if ≥0, restore original
#else
            buf_.EmitByte(0x99);                            // cdq
            enc_.EmitXor32RR(AT::kScratchA, AT::kScratchC); // eax ^= edx
            enc_.EmitSub32RR(AT::kScratchA, AT::kScratchC); // eax -= edx
#endif
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::Min: {
            // int32 min: dst = (src1 > src2) ? src2 : src1 → cmovg
            if (!instr.has_src1() || !instr.has_src2() || !instr.has_dst())
                return false;
            LoadGpr(AT::kScratchA, instr.src1_reg());
            LoadGpr(AT::kScratchB, instr.src2_reg());
            enc_.EmitCmpRR(AT::kScratchA, AT::kScratchB);
            enc_.EmitCmovcc(kCC_G, AT::kScratchA, AT::kScratchB); // if a > b, a = b
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::Max: {
            // int32 max: dst = (src1 < src2) ? src2 : src1 → cmovl
            if (!instr.has_src1() || !instr.has_src2() || !instr.has_dst())
                return false;
            LoadGpr(AT::kScratchA, instr.src1_reg());
            LoadGpr(AT::kScratchB, instr.src2_reg());
            enc_.EmitCmpRR(AT::kScratchA, AT::kScratchB);
            enc_.EmitCmovcc(kCC_L, AT::kScratchA, AT::kScratchB); // if a < b, a = b
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::AddOvf:
        case IROpCode::SubOvf:
        case IROpCode::MulOvf: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            uint8_t op_reg = AT::kScratchA;
            uint8_t src2_reg = AT::kScratchB;
            if (has_graph_coloring_) {
                if (instr.dst_reg() < interpreter::kGPRegisters) {
                    uint8_t c = gcr_.gpr_color[instr.dst_reg()];
                    if (c != 0xFF)
                        op_reg = c;
                }
                if (instr.has_src2() && instr.src2_reg() < interpreter::kGPRegisters) {
                    uint8_t c = gcr_.gpr_color[instr.src2_reg()];
                    if (c != 0xFF)
                        src2_reg = c;
                }
            }
            LoadGpr(op_reg, instr.src1_reg());
            if (instr.has_src2())
                LoadGpr(src2_reg, instr.src2_reg());
            // Use 32-bit operations so x64 OF flag reflects 32-bit
            // signed overflow, matching IL add.ovf/sub.ovf/mul.ovf semantics.
#if defined(__aarch64__)
            if (opc == IROpCode::AddOvf) {
                // ADDS Wd, Wn, Wm — sets V flag on signed 32-bit overflow.
                EmitAdds32(buf_, op_reg, op_reg, src2_reg);
            } else if (opc == IROpCode::SubOvf) {
                // SUBS Wd, Wn, Wm — sets V flag on signed 32-bit overflow.
                EmitSubs32(buf_, op_reg, op_reg, src2_reg);
            } else {
                // MulOvf: SMULL 64-bit product + SXTW + CMP sets Z flag.
                // No overflow if lower 32 bits sign-extend to match full 64-bit result.
                EmitSmull(buf_, AT::kScratchC, op_reg, src2_reg);
                EmitSxtw(buf_, AT::kScratchB, AT::kScratchC);
                EmitCmp64(buf_, AT::kScratchC, AT::kScratchB);
            }
#else
            if (opc == IROpCode::AddOvf) {
                EmitREX(buf_, false, op_reg, src2_reg);
                buf_.EmitByte(0x03);
                buf_.EmitByte(ModRM(3, op_reg, src2_reg));
            } else if (opc == IROpCode::SubOvf) {
                EmitREX(buf_, false, op_reg, src2_reg);
                buf_.EmitByte(0x2B);
                buf_.EmitByte(ModRM(3, op_reg, src2_reg));
            } else {
                EmitREX(buf_, false, op_reg, src2_reg);
                buf_.EmitByte(0x0F);
                buf_.EmitByte(0xAF);
                buf_.EmitByte(ModRM(3, op_reg, src2_reg));
            }
#endif
            {
                uint32_t jno_pos = buf_.pos();
#if defined(__aarch64__)
                if (opc == IROpCode::MulOvf) {
                    // SMULL+SXTW+CMP sets Z flag — B.EQ skips deopt on no overflow.
                    EmitBCond(buf_, kARM64_EQ, 0);
                } else {
                    enc_.EmitJccRel32(kCC_NO, 0);
                }
#else
                enc_.EmitJccRel32(kCC_NO, 0);
#endif
                // Find the nearest backward branch target (loop header) for OSR
                // resume on overflow deoptimization.  Scanning backward from the
                // current instruction, the first backward branch found is the
                // nearest back-edge; its target is the loop header to resume at.
                uint32_t osr_pc = 0;
                for (uint32_t si = current_instr_index_; si > 0; --si) {
                    uint32_t scan_idx = si - 1;
                    const auto& scan_instr = rm_.instructions[scan_idx];
                    if (scan_instr.is_branch() && scan_instr.imm.branch_target < scan_idx) {
                        osr_pc = scan_instr.imm.branch_target;
                        break;
                    }
                }
                EmitDeoptSequence(current_instr_index_, osr_pc);
                uint32_t no_overflow = buf_.pos();
#if defined(__aarch64__)
                PatchArm64Bcond(buf_, jno_pos, no_overflow);
#else
                // JccRel32 is 6 bytes: 0F 8x + 4-byte offset at jno_pos+2.
                // Displacement is from end of instruction (jno_pos + 6).
                int32_t disp = static_cast<int32_t>(no_overflow - (jno_pos + 6));
                buf_.Patch32(jno_pos + 2, static_cast<uint32_t>(disp));
#endif
#if defined(__aarch64__)
                // ARM64 MulOvf: SMULL result is in scratchC — move low 32 bits to op_reg.
                if (opc == IROpCode::MulOvf) {
                    EmitOrr32(buf_, op_reg, 31, AT::kScratchC);
                }
#endif
            }
            StoreGpr(op_reg, instr.dst_reg());
            return true;
        }

        case IROpCode::And:
        case IROpCode::Or:
        case IROpCode::Xor:
        case IROpCode::Not: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            EmitBitwise(opc, instr.dst_reg(), instr.src1_reg(), instr.has_src2() ? instr.src2_reg() : UINT32_MAX);
            return true;
        }

        case IROpCode::Shl:
        case IROpCode::Shr:
        case IROpCode::ShrUn: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            EmitShift(opc, instr.dst_reg(), instr.src1_reg(), instr.has_src2() ? instr.src2_reg() : UINT32_MAX,
                      instr.has_imm() ? instr.imm.i4 : 0);
            return true;
        }

        case IROpCode::Br: {
            uint32_t target = instr.imm.branch_target;
            if (target < current_instr_index_)
                EmitSafepointPoll();
            uint32_t patch_off
#if defined(__aarch64__)
                = buf_.pos();
#else
                = buf_.pos() + 1;
#endif
            enc_.EmitJmpRel32(0);
            branch_patches_.push_back({patch_off, target});
            return true;
        }

        case IROpCode::Beq:
        case IROpCode::BneUn: {
            if (!instr.has_src1() || !instr.has_src2())
                return false;
            uint8_t cmp_a = AT::kScratchA, cmp_b = AT::kScratchB;
            if (has_graph_coloring_) {
                if (instr.src1_reg() < interpreter::kGPRegisters) {
                    uint8_t c = gcr_.gpr_color[instr.src1_reg()];
                    if (c != 0xFF)
                        cmp_a = c;
                }
                if (instr.src2_reg() < interpreter::kGPRegisters) {
                    uint8_t c = gcr_.gpr_color[instr.src2_reg()];
                    if (c != 0xFF)
                        cmp_b = c;
                }
                if (cmp_a == cmp_b)
                    cmp_b = (cmp_a == AT::kScratchA) ? AT::kScratchB : AT::kScratchA;
            }
            LoadGpr(cmp_a, instr.src1_reg());
            LoadGpr(cmp_b, instr.src2_reg());
            enc_.EmitCmpRR(cmp_a, cmp_b);
            uint8_t jcc = (instr.op_code() == IROpCode::Beq) ? kCC_E : kCC_NE;
            uint32_t patch_off
#if defined(__aarch64__)
                = buf_.pos();
#else
                = buf_.pos() + 2;
#endif
            enc_.EmitJccRel32(jcc, 0);
            branch_patches_.push_back({patch_off, instr.imm.branch_target});
            return true;
        }

        case IROpCode::Blt:
        case IROpCode::Bgt:
        case IROpCode::Ble:
        case IROpCode::Bge:
        case IROpCode::BltUn:
        case IROpCode::BgtUn:
        case IROpCode::BleUn:
        case IROpCode::BgeUn: {
            if (!instr.has_src1() || !instr.has_src2())
                return false;
            uint8_t cmp_a = AT::kScratchA, cmp_b = AT::kScratchB;
            if (has_graph_coloring_) {
                if (instr.src1_reg() < interpreter::kGPRegisters) {
                    uint8_t c = gcr_.gpr_color[instr.src1_reg()];
                    if (c != 0xFF)
                        cmp_a = c;
                }
                if (instr.src2_reg() < interpreter::kGPRegisters) {
                    uint8_t c = gcr_.gpr_color[instr.src2_reg()];
                    if (c != 0xFF)
                        cmp_b = c;
                }
                if (cmp_a == cmp_b)
                    cmp_b = (cmp_a == AT::kScratchA) ? AT::kScratchB : AT::kScratchA;
            }
            LoadGpr(cmp_a, instr.src1_reg());
            LoadGpr(cmp_b, instr.src2_reg());
            enc_.EmitCmp32RR(cmp_a, cmp_b);
            uint8_t jcc = CmpToJccSigned(instr.op_code());
            uint32_t patch_off
#if defined(__aarch64__)
                = buf_.pos();
#else
                = buf_.pos() + 2;
#endif
            enc_.EmitJccRel32(jcc, 0);
            branch_patches_.push_back({patch_off, instr.imm.branch_target});
            return true;
        }

        case IROpCode::BrTrue:
        case IROpCode::BrFalse: {
            if (!instr.has_src1())
                return false;
            uint32_t target = instr.imm.branch_target;
            if (target < current_instr_index_)
                EmitSafepointPoll();
            uint8_t test_reg = AT::kScratchA;
            if (has_graph_coloring_ && instr.src1_reg() < interpreter::kGPRegisters) {
                uint8_t c = gcr_.gpr_color[instr.src1_reg()];
                if (c != 0xFF)
                    test_reg = c;
            }
            LoadGpr(test_reg, instr.src1_reg());
            enc_.EmitTestRR(test_reg, test_reg);
            uint8_t jcc = (instr.op_code() == IROpCode::BrTrue) ? kCC_NE : kCC_E;
            uint32_t patch_off
#if defined(__aarch64__)
                = buf_.pos();
#else
                = buf_.pos() + 2;
#endif
            enc_.EmitJccRel32(jcc, 0);
            branch_patches_.push_back({patch_off, target});
            return true;
        }

        // ── Switch: jump table (>=4 cases) or linear chain (<4 cases) ────
        case IROpCode::Switch: {
            if (!instr.has_src1())
                return false;
            uint32_t target_count = static_cast<uint32_t>((instr.header >> 48) & 0x7FFF);
            auto* targets = static_cast<const uint32_t*>(instr.imm.ptr);
            if (targets == nullptr || target_count == 0)
                return false;
            LoadGpr(AT::kScratchA, instr.src1_reg());

            if (target_count >= 4) {
                // ── Jump table path ────────────────────────────────────────
                //   cmp   rax, target_count-1
                //   jae   default_branch
                //   lea   r10, [rip + table_off]   ; disp = 9 (fixed: past the 9 bytes after LEA)
                //   movsxd rax, [r10 + rax*4]       ; rax = table[value]
                //   add   r10, rax                  ; r10 = table_base + offset = target
                //   jmp   r10
                //   table: .int32 disp_to_target_i

                // Bounds check: if (value >= target_count) goto default
                enc_.EmitCmpRI(AT::kScratchA, static_cast<int32_t>(target_count));
                uint32_t default_patch_off
#if defined(__aarch64__)
                    = buf_.pos();
#else
                    = buf_.pos() + 2;
#endif
                enc_.EmitJccRel32(kCC_AE, 0);
                branch_patches_.push_back({default_patch_off, targets[target_count]});

                // LEA r10, [rip + 10] — points to table start (10 bytes from LEA end)
                // LEA is 7 bytes. movsxd(4) + add(3) + jmp(3) = 10 bytes after LEA.
                uint32_t lea_pos = buf_.pos();
#if defined(__aarch64__)
                // ARM64 jump table: ADR Xtmp, table; LDRSW Xidx, [Xtmp, Xidx, LSL #2]; ADD Xtmp, Xtmp, Xidx; BR Xtmp
                // ADR X10, #(offset to table) — points to table entries
                // We'll compute offset after emitting LDRSW+ADD+BR (12 bytes)
                enc_.EmitAdr(AT::kTmpReg, 12); // skip ADR(4)+LDR(4)+BR(4) → points to table
                buf_.Emit32(0xF860780Au | (AT::kTmpReg << 5) | AT::kScratchA); // LDRSW Xidx, [X10, Xidx, LSL #2]
                //           ^-- actually 0xF8607800 | (Xtmp << 5) | Xidx
                buf_.Emit32(0x8B0C000Au | (AT::kScratchA << 16) | (AT::kTmpReg << 5) |
                            AT::kTmpReg); // ADD X10, X10, Xidx
                enc_.EmitBr(AT::kTmpReg);
#else
                enc_.EmitLeaRipRel(AT::kTmpReg, 10);
                // MOVSXD rax, [r10 + rax*4] — load sign-extended table entry
                // REX: W=1, R=0(rax), X=0(rax), B=1(r10) → 0x49
                buf_.EmitByte(0x49);
                buf_.EmitByte(0x63);                               // MOVSXD opcode
                buf_.EmitByte(ModRM(0, AT::kScratchA, 4));         // rm=4 = SIB escape
                buf_.EmitByte(SIB(2, AT::kScratchA, AT::kTmpReg)); // scale=4, index=rax, base=r10
                // ADD r10, rax
                enc_.EmitAddRR(AT::kTmpReg, AT::kScratchA);
                // JMP r10
                enc_.EmitJmpReg(AT::kTmpReg);
#endif

                // Emit jump table entries (placeholder values, patched by ResolveBranches)
                uint32_t table_pos = buf_.pos();
                for (uint32_t i = 0; i < target_count; ++i) {
                    buf_.Emit32(0); // placeholder
                    jump_table_patches_.push_back({buf_.pos() - 4, table_pos, targets[i]});
                }
            } else {
                // ── Linear chain for small switches (< 4 cases) ──────────
                for (uint32_t i = 0; i < target_count; ++i) {
                    enc_.EmitCmpRI(AT::kScratchA, static_cast<int32_t>(i));
                    uint32_t patch_off
#if defined(__aarch64__)
                        = buf_.pos();
#else
                        = buf_.pos() + 2;
#endif
                    enc_.EmitJccRel32(kCC_E, 0);
                    branch_patches_.push_back({patch_off, targets[i]});
                }
                // No match: jump to default target at targets[target_count]
                uint32_t default_patch_off
#if defined(__aarch64__)
                    = buf_.pos();
#else
                    = buf_.pos() + 1;
#endif
                enc_.EmitJmpRel32(0);
                branch_patches_.push_back({default_patch_off, targets[target_count]});
            }
            return true;
        }

        // ── SEH opcodes ─────────────────────────────────────────────────
        case IROpCode::Leave: {
            uint32_t target = instr.imm.branch_target;

            // SEH V3: Check if any finally/fault clause covers the current
            // instruction.  If so, emit JitLeaveHelper call to set up pending_leave
            // and redirect to the innermost finally handler.
            bool has_finally_covering = false;
            for (const auto& clause : rm_.seh_clauses) {
                uint32_t flags = static_cast<uint32_t>(clause.flags);
                uint32_t try_start = static_cast<uint32_t>(clause.try_start_idx);
                uint32_t try_end = static_cast<uint32_t>(clause.try_end_idx);
                if ((flags == 2 || flags == 4) && current_instr_index_ >= try_start && current_instr_index_ < try_end) {
                    has_finally_covering = true;
                    break;
                }
            }

            if (has_finally_covering) {
                // Leave crossing a finally/fault boundary: call JitLeaveHelper
                // to resolve byte offsets at runtime and find the innermost handler.
                enc_.EmitMovRIImm32(AT::kScratchB, target);               // arg1: target_instr_idx
                enc_.EmitMovRIImm32(AT::kScratchC, current_instr_index_); // arg2: current_instr_idx
                EmitRuntimeHelperCall(JitLeaveHelper);                    // RAX = handler addr or 0
                enc_.EmitTestRR(AT::kScratchA, AT::kScratchA);            // test rax, rax
                uint32_t jz_pos = buf_.pos();
                enc_.EmitJccRel32(kCC_E, 0);    // jz .normal_jmp
                enc_.EmitJmpReg(AT::kScratchA); // jmp rax → handler
                // .normal_jmp: fall through to normal leave JMP
                uint32_t normal_pos = buf_.pos();
#if defined(__aarch64__)
                PatchArm64Bcond(buf_, jz_pos, normal_pos);
#else
                int32_t jz_disp = static_cast<int32_t>(normal_pos - (jz_pos + 6));
                buf_.Patch32(jz_pos + 2, static_cast<uint32_t>(jz_disp));
#endif
                // Normal JMP to leave target (JitLeaveHelper returned 0 or
                // finally chain completed and returned to leave path).
                uint32_t patch_off
#if defined(__aarch64__)
                    = buf_.pos();
#else
                    = buf_.pos() + 1;
#endif
                enc_.EmitJmpRel32(0);
                branch_patches_.push_back({patch_off, target});
            } else {
                // No finally/fault covering — normal JMP (current behavior).
                if (target < current_instr_index_)
                    EmitSafepointPoll();
                uint32_t patch_off
#if defined(__aarch64__)
                    = buf_.pos();
#else
                    = buf_.pos() + 1;
#endif
                enc_.EmitJmpRel32(0);
                branch_patches_.push_back({patch_off, target});
            }
            return true;
        }

        case IROpCode::EndFinally: {
            // SEH V3: call JitEndFinallyHelper to advance the finally/fault
            // unwind chain.  Returns next handler address (non-zero) or 0
            // (continue normally).
            EmitRuntimeHelperCall(JitEndFinallyHelper);
            enc_.EmitTestRR(AT::kScratchA, AT::kScratchA);
            uint32_t jz_pos = buf_.pos();
            enc_.EmitJccRel32(kCC_E, 0);    // jz .continue
            enc_.EmitJmpReg(AT::kScratchA); // jmp rax → next handler/leave target
            // .continue: normal fall-through
            uint32_t continue_pos = buf_.pos();
#if defined(__aarch64__)
            PatchArm64Bcond(buf_, jz_pos, continue_pos);
#else
            int32_t jz_disp = static_cast<int32_t>(continue_pos - (jz_pos + 6));
            buf_.Patch32(jz_pos + 2, static_cast<uint32_t>(jz_disp));
#endif
            return true;
        }

        case IROpCode::EndFilter: {
            // EndFilter: pop the filter result from src1_reg and decide.
            //   non-zero (accept) → fall through to the handler (which is emitted
            //                       sequentially right after the filter)
            //   zero    (reject)  → call JitEndFinallyHelper to continue exception
            //                       search outward; jmp rax to the next handler
            //
            // Per the SEH layout convention used by code_generator.cpp:
            // for filter clauses, handler_start_offset stores the filter function
            // offset, and the actual handler follows the EndFilter sequentially.
            // The fall-through path therefore lands on the handler.
            if (instr.has_src1()) {
                LoadGpr(AT::kScratchA, instr.src1_reg()); // EAX = filter result
            } else {
                enc_.EmitMovImm64(AT::kScratchA, 0); // defensive: treat as reject
            }
            enc_.EmitTestRR(AT::kScratchA, AT::kScratchA);
            // jne .accept  (filter result != 0 → fall through to handler)
            uint32_t jne_pos = buf_.pos();
            enc_.EmitJccRel32(kCC_NE, 0);
            // Reject path: dispatch via JitEndFinallyHelper.
            EmitRuntimeHelperCall(JitEndFinallyHelper);
            enc_.EmitTestRR(AT::kScratchA, AT::kScratchA);
            uint32_t jz_pos = buf_.pos();
            enc_.EmitJccRel32(kCC_E, 0);    // jz .accept (no further handler)
            enc_.EmitJmpReg(AT::kScratchA); // jmp to next handler
            // .accept: fall through to the handler block (sequentially after EndFilter).
            uint32_t accept_pos = buf_.pos();
#if defined(__aarch64__)
            PatchArm64Bcond(buf_, jne_pos, accept_pos);
            PatchArm64Bcond(buf_, jz_pos, accept_pos);
#else
            int32_t jne_disp = static_cast<int32_t>(accept_pos - (jne_pos + 6));
            buf_.Patch32(jne_pos + 2, static_cast<uint32_t>(jne_disp));
            int32_t jz_disp = static_cast<int32_t>(accept_pos - (jz_pos + 6));
            buf_.Patch32(jz_pos + 2, static_cast<uint32_t>(jz_disp));
#endif
            return true;
        }

        case IROpCode::Throw:
        case IROpCode::Rethrow: {
            // Deoptimize when register allocation is inconsistent
            // (should not happen — the interpreter verifier ensures correct
            //  src1 count for Throw/Rethrow; this is a safety check).
            if (!instr.has_src1())
                return false;
            LoadGpr(AT::kScratchB, instr.src1_reg()); // RCX = exception object
            // JMP to cold section (patched after epilogue emission).
            // Cold section contains ChaosJitRaiseException call + INT3 safety net.
            // This keeps hot-path code contiguous for better icache behavior.
            uint32_t jmp_off = buf_.pos();
            enc_.EmitJmpRel32(0);
            cold_patches_.push_back({jmp_off + 1});
            return true;
        }

        case IROpCode::Ceq:
        case IROpCode::Clt:
        case IROpCode::Cgt: {
            if (!instr.has_src1() || !instr.has_src2() || !instr.has_dst())
                return false;
            // Use fixed scratch registers RAX/RCX, NOT graph-colored registers,
            // because the setcc sequence (MovRIImm32 + Setcc) destroys the
            // register used as cmp_a.  Using src1's colored register would
            // silently corrupt src1's value for any subsequent read of src1
            // through LoadGpr (which returns the colored register, not the stack).
            uint8_t cmp_a = AT::kScratchA, cmp_b = AT::kScratchB;
            LoadGpr(cmp_a, instr.src1_reg());
            LoadGpr(cmp_b, instr.src2_reg());
            // Ceq: 64-bit compare (uint64_t equality, RegisterExecute uses == on uint64_t).
            // Clt/Cgt: 32-bit compare (RegisterExecute casts to int32_t first).
            if (opc == IROpCode::Ceq) {
                enc_.EmitCmpRR(cmp_a, cmp_b);
            } else {
                enc_.EmitCmp32RR(cmp_a, cmp_b);
            }
            uint8_t cc = (opc == IROpCode::Ceq) ? kCC_E : (opc == IROpCode::Clt) ? kCC_L : kCC_G;
            // IMPORTANT: use mov reg, 0 (NOT xor reg, reg) to preserve CMP flags.
            // xor reg,reg sets ZF=1 (result is zero), clobbering the flags sete reads.
            enc_.EmitMovRIImm32(cmp_a, 0);
            enc_.EmitSetcc(cc, cmp_a);
            StoreGpr(cmp_a, instr.dst_reg());
            return true;
        }

        case IROpCode::Conv_I4:
        case IROpCode::Conv_I8:
        case IROpCode::ConvI:
        case IROpCode::ConvU:
        case IROpCode::ConvOvfI:
        case IROpCode::ConvOvfI8: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            LoadGpr(AT::kScratchA, instr.src1_reg());
            // Int32-converting opcodes: truncate to 32-bit via mov eax,eax (zero-extends).
            // Int64 opcodes (Conv_I8, ConvOvfI8): keep full 64-bit value.
            bool is_int32_op = (instr.op_code() != IROpCode::Conv_I8 && instr.op_code() != IROpCode::ConvOvfI8);
            if (is_int32_op) {
#if !defined(__aarch64__)
                buf_.EmitByte(0x89);
                buf_.EmitByte(0xC0); // mov eax, eax (zero-extend 32→64)
#endif
            }
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::ConvOvfI4: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            LoadGpr(AT::kScratchA, instr.src1_reg());
            // Check overflow: sign-extend 32-bit truncation back to 64-bit and compare.
            // If original ≠ sign-extend(truncate(original)), the value doesn't fit int32.
            enc_.EmitMovRR(AT::kScratchB, AT::kScratchA); // rcx = rax (copy original)
#if !defined(__aarch64__)
            buf_.EmitByte(0x89);
            buf_.EmitByte(0xC1); // mov ecx, eax (truncate to 32-bit)
#endif
            enc_.EmitMovsxd(AT::kScratchB, AT::kScratchB); // movsxd rcx, ecx (sign-extend 32→64)
            enc_.EmitCmpRR(AT::kScratchA, AT::kScratchB);  // cmp rax, rcx
            {
                uint32_t jne_pos = buf_.pos();
                enc_.EmitJccRel32(kCC_NE, 0); // jne → deopt (overflow)
                EmitDeoptSequence(current_instr_index_);
                uint32_t no_overflow = buf_.pos();
#if defined(__aarch64__)
                PatchArm64Bcond(buf_, jne_pos, no_overflow);
#else
                int32_t disp = static_cast<int32_t>(no_overflow - (jne_pos + 6));
                buf_.Patch32(jne_pos + 2, static_cast<uint32_t>(disp));
#endif
            }
            // Result: truncated 32-bit value in EAX (zero-extended to 64-bit in RAX)
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::ConvOvfU:
        case IROpCode::ConvOvfU4:
        case IROpCode::ConvOvfU8: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            LoadGpr(AT::kScratchA, instr.src1_reg());
            if (instr.op_code() == IROpCode::ConvOvfU4) {
                // Check value fits in uint32: truncate + compare
                enc_.EmitMovRR(AT::kScratchB, AT::kScratchA);
#if !defined(__aarch64__)
                buf_.EmitByte(0x89);
                buf_.EmitByte(0xC1); // mov ecx, eax (32-bit truncate)
#endif
                enc_.EmitCmpRR(AT::kScratchA, AT::kScratchB);
                uint32_t jne_pos = buf_.pos();
                enc_.EmitJccRel32(kCC_NE, 0);
                EmitDeoptSequence(current_instr_index_);
                uint32_t no_overflow = buf_.pos();
#if defined(__aarch64__)
                PatchArm64Bcond(buf_, jne_pos, no_overflow);
#else
                int32_t disp = static_cast<int32_t>(no_overflow - (jne_pos + 6));
                buf_.Patch32(jne_pos + 2, static_cast<uint32_t>(disp));
#endif
            } else {
                // ConvOvfU / ConvOvfU8: check sign bit
                enc_.EmitTestRR(AT::kScratchA, AT::kScratchA);
                uint32_t js_pos = buf_.pos();
                enc_.EmitJccRel32(kCC_S, 0);
                EmitDeoptSequence(current_instr_index_);
                uint32_t non_neg = buf_.pos();
#if defined(__aarch64__)
                PatchArm64Bcond(buf_, js_pos, non_neg);
#else
                int32_t disp = static_cast<int32_t>(non_neg - (js_pos + 6));
                buf_.Patch32(js_pos + 2, static_cast<uint32_t>(disp));
#endif
            }
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::Conv_R4: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            // int32→float: read GPR[src1], truncate to int32, convert to float.
            // Matches RegisterExecute: float v = static_cast<float>(static_cast<int32_t>(gpr[src1]));
            LoadGpr(AT::kScratchA, instr.src1_reg());
            enc_.EmitMovsxd(AT::kScratchA, AT::kScratchA); // movsxd rax, eax (sign-extend 32→64)
            enc_.EmitCvtsi2sd(0, AT::kScratchA);           // cvtsi2sd xmm0, rax (int64→double, full 64-bit slot)
            StoreFpr(0, instr.dst_reg());
            return true;
        }

        case IROpCode::Conv_R8: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            // int32→double: read GPR[src1], truncate to int32, convert to double.
            // Matches RegisterExecute: double v = static_cast<double>(static_cast<int32_t>(gpr[src1]));
            LoadGpr(AT::kScratchA, instr.src1_reg());
            enc_.EmitMovsxd(AT::kScratchA, AT::kScratchA); // movsxd rax, eax (sign-extend 32→64)
            enc_.EmitCvtsi2sd(0, AT::kScratchA);           // cvtsi2sd xmm0, rax (int64→double)
            StoreFpr(0, instr.dst_reg());
            return true;
        }

        case IROpCode::ConvRUn: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            // uint32 → float64: load uint32 from GPR[src1], zero-extend to 64-bit,
            // convert to double, store to FPR[dst].  Matches interpreter VM:
            //   double result = static_cast<double>(static_cast<uint32_t>(gpr[src1]));
            LoadGpr(AT::kScratchA, instr.src1_reg());
#if !defined(__aarch64__)
            buf_.EmitByte(0x8B);
            buf_.EmitByte(0xC0); // mov eax, eax (zero-extend 32→64)
#endif
            enc_.EmitCvtsi2sd(0, AT::kScratchA); // cvtsi2sd xmm0, rax
            StoreFpr(0, instr.dst_reg());
            return true;
        }

        case IROpCode::Pop:
            return true;

        case IROpCode::Break:
            return true;

        case IROpCode::SizeOf: {
            if (!instr.has_dst())
                return false;
            enc_.EmitMovRIImm32(AT::kScratchA, static_cast<uint32_t>(instr.imm.i4));
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::LdToken: {
            if (!instr.has_dst())
                return false;
            enc_.EmitMovRIImm32(AT::kScratchA, static_cast<uint32_t>(instr.imm.i4));
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::LdFtn: {
            if (!instr.has_dst())
                return false;
            enc_.EmitMovImm64(AT::kScratchA, reinterpret_cast<uint64_t>(instr.imm.ptr));
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::LdStr: {
            if (!instr.has_dst())
                return false;
            enc_.EmitMovImm64(AT::kScratchA, reinterpret_cast<uint64_t>(instr.imm.ptr));
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::LdFld: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            LoadGpr(AT::kScratchB, instr.src1_reg());
            enc_.EmitMovRIImm32(AT::kScratchC, instr.imm.field_offset);
            uint32_t call_pos = EmitRuntimeHelperCall(::CodegenLdFld);
            {
                uint32_t call_token = 0, call_module = 0;
                if (config_.call_cache != nullptr && current_instr_index_ < config_.call_cache_count) {
                    const auto& cached = static_cast<const runtime_instantiation::CachedCallInfo*>(
                        config_.call_cache)[current_instr_index_];
                    call_token = cached.method_token;
                    call_module = cached.module_id;
                }
                call_sites_.push_back({current_instr_index_, call_pos, call_token, call_module});
            }
            RecordGcPoint(call_pos);
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::StFld: {
            if (!instr.has_src1() || !instr.has_src2())
                return false;
            // Simple path -- always call CodegenStFld (full SATB barrier).
            LoadGpr(AT::kScratchB, instr.src1_reg());
            enc_.EmitMovRIImm32(AT::kScratchC, instr.imm.field_offset);
            LoadGpr(AT::kExtraScratch0, instr.src2_reg());
            EmitRuntimeHelperCall(::CodegenStFld);
            return true;
            ;
        }

        case IROpCode::StFldBarrier: {
            // Inline g_bgc_is_marking check + conditional barrier.
            // Fast path (not marking): call CodegenStFldNoBarrier (store + card mark).
            // Slow path (marking): call CodegenStFld (full SATB pre-write barrier).
            if (!instr.has_src1() || !instr.has_src2())
                return false;
            LoadGpr(AT::kScratchB, instr.src1_reg());                   // obj
            enc_.EmitMovRIImm32(AT::kScratchC, instr.imm.field_offset); // field_idx
            LoadGpr(AT::kExtraScratch0, instr.src2_reg());              // value
            // Inline g_bgc_is_marking check (preserves RCX/RDX/R8)
            enc_.EmitMovImm64(AT::kScratchA,
                              reinterpret_cast<uint64_t>(&chaos::il2cpp::runtime_core::g_bgc_is_marking));
#if defined(__aarch64__)
            // ARM64: LDRB Wtmp, [Xaddr]; CBZ Wtmp, skip_marking
            buf_.Emit32(0x39400000u | (AT::kScratchA << 5) | AT::kScratchC); // LDRB WscratchC, [XscratchA]
            buf_.Emit32(0x3400000Cu | (AT::kScratchC << 5)); // CBZ WscratchC, #0 (placeholder, patched later)
#else
            buf_.EmitByte(0x80); // cmp r/m8, imm8
            buf_.EmitByte(0x38); // ModRM: mod=00, reg=7, rm=0 → [rax]
            buf_.EmitByte(0x00); // imm8 = 0
#endif
            uint32_t marking_jmp_pos = buf_.pos();
            enc_.EmitJccRel32(kCC_NE, 0); // jne .marking
            // Fast path: not marking → CodegenStFldNoBarrier
            EmitRuntimeHelperCall(::CodegenStFldNoBarrier);
            uint32_t done_jmp_pos = buf_.pos();
            enc_.EmitJmpRel32(0); // jmp .done
            // Slow path: marking → CodegenStFld (full SATB barrier)
            uint32_t marking_pos = buf_.pos();
#if defined(__aarch64__)
            PatchArm64Bcond(buf_, marking_jmp_pos, marking_pos);
#else
            buf_.Patch32(marking_jmp_pos + 2, marking_pos - (marking_jmp_pos + 6));
#endif
            // Reload args (spilled/colored regs may differ in slow path)
            LoadGpr(AT::kScratchB, instr.src1_reg());
            enc_.EmitMovRIImm32(AT::kScratchC, instr.imm.field_offset);
            LoadGpr(AT::kExtraScratch0, instr.src2_reg());
            EmitRuntimeHelperCall(::CodegenStFld);
            uint32_t done_pos = buf_.pos();
#if defined(__aarch64__)
            PatchArm64B(buf_, done_jmp_pos, done_pos);
#else
            buf_.Patch32(done_jmp_pos + 1, done_pos - (done_jmp_pos + 5));
#endif
            return true;
        }

        case IROpCode::NewObj: {
            if (!instr.has_dst())
                return false;
            uint32_t type_token = instr.imm.field_offset;
            uint32_t field_count = instr.imm.operand_index;
            if (field_count == 0)
                field_count = 1;
            // InterpreterObject size (64 bytes: SmallFieldArray 56 + type_token 4 + padding 4)
            static constexpr int32_t kObjSize = static_cast<int32_t>(sizeof(interpreter::InterpreterObject));

            if (false) {
                // ═══ TLAB inline allocation path ═══
                if (config_.enable_register_caching && cached_slots_used_)
                    SpillCachedRegs();
                EmitLoadTlsTlab(buf_); // rax = &tls_tlab

                enc_.EmitMovRM(AT::kScratchB, AT::kScratchA, 8);       // rcx = tls_tlab.current
                enc_.EmitLeaRM(AT::kArgsBuf, AT::kScratchB, kObjSize); // rbx = new current
                enc_.EmitMovRM(AT::kScratchC, AT::kScratchA, 16);      // rdx = tls_tlab.end
                enc_.EmitCmpRR(AT::kArgsBuf, AT::kScratchC);
                uint32_t newobj_ja_pos = buf_.pos();
                enc_.EmitJccRel32(kCC_A, 0); // ja slow_path (patched later)

                // TLAB HIT: bump, zero-init, init struct
                enc_.EmitMovMR(AT::kScratchA, 8, AT::kArgsBuf); // tls_tlab.current = new_ptr

                // Zero-init 64 bytes (4 × movups)
                enc_.EmitXorpsRR(0, 0);                  // xorps xmm0, xmm0
                enc_.EmitMovUPSMR(AT::kScratchB, 0, 0);  // [rcx+0]
                enc_.EmitMovUPSMR(AT::kScratchB, 16, 0); // [rcx+16]
                enc_.EmitMovUPSMR(AT::kScratchB, 32, 0); // [rcx+32]
                enc_.EmitMovUPSMR(AT::kScratchB, 48, 0); // [rcx+48]

                // Init SmallFieldArray: fields_ptr_ = &inline_[0] (at offset 24)
                enc_.EmitLeaRM(AT::kArgsBuf, AT::kScratchB, 24);
                enc_.EmitMovMR(AT::kScratchB, 0, AT::kArgsBuf);  // fields.fields_ptr_ = &inline_[0]
                enc_.EmitMovRI32(AT::kArgsBuf, 2);               // rbx = kInlineCapacity
                enc_.EmitMovMR(AT::kScratchB, 16, AT::kArgsBuf); // fields.field_capacity_ = 2
                // field_count_ at offset 8 is already 0 from zero-init

                // Set type_token at offset 56 (4 bytes, upper 4 zero from zero-init)
                enc_.EmitMovRIImm32(AT::kArgsBuf, type_token);
                enc_.EmitMovMR(AT::kScratchB, 56, AT::kArgsBuf); // obj->type_token = type_token

                StoreGpr(AT::kScratchB, instr.dst_reg()); // result = obj pointer

                uint32_t newobj_jmp_done_pos = buf_.pos();
                enc_.EmitJmpRel32(0); // skip slow path

                // ═══ Slow path (TLAB miss) ═══
                uint32_t newobj_slow_pos = buf_.pos();
#if defined(__aarch64__)
                PatchArm64Bcond(buf_, newobj_ja_pos, newobj_slow_pos);
#else
                buf_.Patch32(newobj_ja_pos + 2, newobj_slow_pos - (newobj_ja_pos + 6));
#endif

                {
                    enc_.EmitMovRIImm32(AT::kScratchB, type_token);
                    enc_.EmitMovRIImm32(AT::kScratchC, field_count);
                    uint32_t call_pos = EmitRuntimeHelperCall(::CodegenNewObj);
                    {
                        uint32_t call_token = 0, call_module = 0;
                        if (config_.call_cache != nullptr && current_instr_index_ < config_.call_cache_count) {
                            const auto& cached = static_cast<const runtime_instantiation::CachedCallInfo*>(
                                config_.call_cache)[current_instr_index_];
                            call_token = cached.method_token;
                            call_module = cached.module_id;
                        }
                        call_sites_.push_back({current_instr_index_, call_pos, call_token, call_module});
                    }
                    RecordGcPoint(call_pos);
                    StoreGpr(AT::kScratchA, instr.dst_reg());
                }

                uint32_t newobj_done_pos = buf_.pos();
#if defined(__aarch64__)
                PatchArm64B(buf_, newobj_jmp_done_pos, newobj_done_pos);
#else
                buf_.Patch32(newobj_jmp_done_pos + 1, newobj_done_pos - (newobj_jmp_done_pos + 5));
#endif
            } else {
                // Object too large for TLAB inline — direct GC allocation
                enc_.EmitMovRIImm32(AT::kScratchB, type_token);
                enc_.EmitMovRIImm32(AT::kScratchC, field_count);
                uint32_t call_pos = EmitRuntimeHelperCall(::CodegenNewObj);
                {
                    uint32_t call_token = 0, call_module = 0;
                    if (config_.call_cache != nullptr && current_instr_index_ < config_.call_cache_count) {
                        const auto& cached = static_cast<const runtime_instantiation::CachedCallInfo*>(
                            config_.call_cache)[current_instr_index_];
                        call_token = cached.method_token;
                        call_module = cached.module_id;
                    }
                    call_sites_.push_back({current_instr_index_, call_pos, call_token, call_module});
                }
                RecordGcPoint(call_pos);
                StoreGpr(AT::kScratchA, instr.dst_reg());
            }
            return true;
        }

        case IROpCode::Box: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            // BoxedValue = 16 bytes (single InterpreterValue, no heap-allocated fields)
            static constexpr int32_t kBoxSize = static_cast<int32_t>(sizeof(interpreter::BoxedValue));

            // ═══ TLAB inline allocation path (Windows TLS / ARM64 Linux TPIDR_EL0) ═══
#if defined(_WIN32) || defined(_WIN64) || (defined(__linux__) && defined(__aarch64__))
            if (config_.enable_register_caching && cached_slots_used_)
                SpillCachedRegs();
            EmitLoadTlsTlab(buf_); // rax/x0 = &tls_tlab

            enc_.EmitMovRM(AT::kScratchB, AT::kScratchA, 8);       // rcx/x1 = tls_tlab.current
            enc_.EmitLeaRM(AT::kArgsBuf, AT::kScratchB, kBoxSize); // rbx/x6 = new current
            enc_.EmitMovRM(AT::kScratchC, AT::kScratchA, 16);      // rdx/x2 = tls_tlab.end
            enc_.EmitCmpRR(AT::kArgsBuf, AT::kScratchC);
            uint32_t box_ja_pos = buf_.pos();
            enc_.EmitJccRel32(kCC_A, 0); // ja slow_path

            enc_.EmitMovMR(AT::kScratchA, 8, AT::kArgsBuf); // tls_tlab.current = new_ptr
            LoadGpr(AT::kExtraScratch0, instr.src1_reg());  // r8 = value
            enc_.EmitXorpsRR(0, 0);                         // xorps xmm0, xmm0
            enc_.EmitMovUPSMR(AT::kScratchB, 0, 0);         // [rcx+0..15] = 0
            enc_.EmitMovRIImm32(AT::kArgsBuf, static_cast<uint32_t>(interpreter::ValueTag::Int64));
            enc_.EmitMovMR(AT::kScratchB, 0, AT::kArgsBuf);       // BoxedValue::value.tag = Int64
            enc_.EmitMovMR(AT::kScratchB, 8, AT::kExtraScratch0); // BoxedValue::value.i64 = value
            StoreGpr(AT::kScratchB, instr.dst_reg());             // result = boxed pointer

            uint32_t box_jmp_done_pos = buf_.pos();
            enc_.EmitJmpRel32(0); // skip slow path

            uint32_t box_slow_pos = buf_.pos();
#if defined(__aarch64__)
            PatchArm64Bcond(buf_, box_ja_pos, box_slow_pos);
#else
            buf_.Patch32(box_ja_pos + 2, box_slow_pos - (box_ja_pos + 6));
#endif
#else
            // Linux: no inline TLAB — always use runtime helper
            if (config_.enable_register_caching && cached_slots_used_)
                SpillCachedRegs();
            uint32_t box_slow_pos = buf_.pos();
#endif

            {
                LoadGpr(AT::kScratchB, instr.src1_reg());
                enc_.EmitMovRIImm32(AT::kScratchC, static_cast<uint32_t>(interpreter::ValueTag::Int64));
                enc_.EmitMovRIImm32(AT::kExtraScratch0, instr.imm.field_offset);
                uint32_t call_pos = EmitRuntimeHelperCall(::CodegenBox);
                {
                    uint32_t call_token = 0, call_module = 0;
                    if (config_.call_cache != nullptr && current_instr_index_ < config_.call_cache_count) {
                        const auto& cached = static_cast<const runtime_instantiation::CachedCallInfo*>(
                            config_.call_cache)[current_instr_index_];
                        call_token = cached.method_token;
                        call_module = cached.module_id;
                    }
                    call_sites_.push_back({current_instr_index_, call_pos, call_token, call_module});
                }
                RecordGcPoint(call_pos);
                StoreGpr(AT::kScratchA, instr.dst_reg());
            }

#if defined(_WIN32) || defined(_WIN64) || (defined(__linux__) && defined(__aarch64__))
            uint32_t box_done_pos = buf_.pos();
#if defined(__aarch64__)
            PatchArm64B(buf_, box_jmp_done_pos, box_done_pos);
#else
            buf_.Patch32(box_jmp_done_pos + 1, box_done_pos - (box_jmp_done_pos + 5));
#endif
#endif
            return true;
        }

        case IROpCode::Unbox: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            LoadGpr(AT::kScratchB, instr.src1_reg());
            uint32_t call_pos = EmitRuntimeHelperCall(::CodegenUnbox);
            {
                uint32_t call_token = 0, call_module = 0;
                if (config_.call_cache != nullptr && current_instr_index_ < config_.call_cache_count) {
                    const auto& cached = static_cast<const runtime_instantiation::CachedCallInfo*>(
                        config_.call_cache)[current_instr_index_];
                    call_token = cached.method_token;
                    call_module = cached.module_id;
                }
                call_sites_.push_back({current_instr_index_, call_pos, call_token, call_module});
            }
            RecordGcPoint(call_pos);
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::LdLen: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            LoadGpr(AT::kScratchB, instr.src1_reg());
            uint32_t call_pos = EmitRuntimeHelperCall(::CodegenLdLen);
            RecordGcPoint(call_pos);
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::NewArr: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            static constexpr int32_t kArrSize = static_cast<int32_t>(sizeof(interpreter::ArrayStorage));

            // ═══ TLAB inline allocation path (Windows TLS / ARM64 Linux TPIDR_EL0) ═══
#if defined(_WIN32) || defined(_WIN64) || (defined(__linux__) && defined(__aarch64__))
            if (config_.enable_register_caching && cached_slots_used_)
                SpillCachedRegs();
            EmitLoadTlsTlab(buf_); // rax/x0 = &tls_tlab

            enc_.EmitMovRM(AT::kScratchB, AT::kScratchA, 8);       // rcx/x1 = tls_tlab.current
            enc_.EmitLeaRM(AT::kArgsBuf, AT::kScratchB, kArrSize); // rbx/x6 = new current
            enc_.EmitMovRM(AT::kScratchC, AT::kScratchA, 16);      // rdx/x2 = tls_tlab.end
            enc_.EmitCmpRR(AT::kArgsBuf, AT::kScratchC);
            uint32_t newarr_ja_pos = buf_.pos();
            enc_.EmitJccRel32(kCC_A, 0); // ja slow_path

            // TLAB HIT: bump
            enc_.EmitMovMR(AT::kScratchA, 8, AT::kArgsBuf); // tls_tlab.current = new_ptr

            // CodegenNewArrTlab(mem=rcx, length=rdx) — placement new + elements.resize
            LoadGpr(AT::kScratchC, instr.src1_reg()); // rdx = length
            EmitRuntimeHelperCall(::CodegenNewArrTlab);
            StoreGpr(AT::kScratchA, instr.dst_reg());

            uint32_t newarr_jmp_done_pos = buf_.pos();
            enc_.EmitJmpRel32(0); // skip slow path

            // ═══ Slow path (TLAB miss) ═══
            uint32_t newarr_slow_pos = buf_.pos();
#if defined(__aarch64__)
            PatchArm64Bcond(buf_, newarr_ja_pos, newarr_slow_pos);
#else
            buf_.Patch32(newarr_ja_pos + 2, newarr_slow_pos - (newarr_ja_pos + 6));
#endif
#else
            // Linux x64: no inline TLAB — always use runtime helper
            if (config_.enable_register_caching && cached_slots_used_)
                SpillCachedRegs();
            uint32_t newarr_slow_pos = buf_.pos();
#endif

            LoadGpr(AT::kScratchB, instr.src1_reg());
            uint32_t call_pos = EmitRuntimeHelperCall(::CodegenNewArr);
            {
                uint32_t call_token = 0, call_module = 0;
                if (config_.call_cache != nullptr && current_instr_index_ < config_.call_cache_count) {
                    const auto& cached = static_cast<const runtime_instantiation::CachedCallInfo*>(
                        config_.call_cache)[current_instr_index_];
                    call_token = cached.method_token;
                    call_module = cached.module_id;
                }
                call_sites_.push_back({current_instr_index_, call_pos, call_token, call_module});
            }
            RecordGcPoint(call_pos);
            StoreGpr(AT::kScratchA, instr.dst_reg());

#if defined(_WIN32) || defined(_WIN64) || (defined(__linux__) && defined(__aarch64__))
            uint32_t newarr_done_pos = buf_.pos();
#if defined(__aarch64__)
            PatchArm64B(buf_, newarr_jmp_done_pos, newarr_done_pos);
#else
            buf_.Patch32(newarr_jmp_done_pos + 1, newarr_done_pos - (newarr_jmp_done_pos + 5));
#endif
#endif
            return true;
        }

        case IROpCode::LdElem:
        case IROpCode::LdElemA: {
            if (!instr.has_src1() || !instr.has_src2() || !instr.has_dst())
                return false;
            LoadGpr(AT::kScratchB, instr.src1_reg());
            LoadGpr(AT::kScratchC, instr.src2_reg());
            uint32_t call_pos = EmitRuntimeHelperCall(::CodegenLdElem);
            {
                uint32_t call_token = 0, call_module = 0;
                if (config_.call_cache != nullptr && current_instr_index_ < config_.call_cache_count) {
                    const auto& cached = static_cast<const runtime_instantiation::CachedCallInfo*>(
                        config_.call_cache)[current_instr_index_];
                    call_token = cached.method_token;
                    call_module = cached.module_id;
                }
                call_sites_.push_back({current_instr_index_, call_pos, call_token, call_module});
            }
            RecordGcPoint(call_pos);
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::StElem: {
            if (!instr.has_src1() || !instr.has_src2())
                return false;
            LoadGpr(AT::kScratchB, instr.src1_reg());
            LoadGpr(AT::kScratchC, instr.src2_reg());
            if (instr.flags() & ::chaos::il2cpp::interpreter::kRegHasSrc3)
                LoadGpr(AT::kExtraScratch0, instr.src3_reg());
            EmitRuntimeHelperCall(::CodegenStElem);
            return true;
        }

        case IROpCode::LdElemNoChk:
        case IROpCode::LdElemANoChk: {
            if (!instr.has_src1() || !instr.has_src2() || !instr.has_dst())
                return false;
            LoadGpr(AT::kScratchB, instr.src1_reg());
            LoadGpr(AT::kScratchC, instr.src2_reg());
            uint32_t call_pos = EmitRuntimeHelperCall(::CodegenLdElemNoCheck);
            RecordGcPoint(call_pos);
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::StElemNoChk: {
            if (!instr.has_src1() || !instr.has_src2())
                return false;
            LoadGpr(AT::kScratchB, instr.src1_reg());
            LoadGpr(AT::kScratchC, instr.src2_reg());
            if (instr.flags() & ::chaos::il2cpp::interpreter::kRegHasSrc3)
                LoadGpr(AT::kExtraScratch0, instr.src3_reg());
            EmitRuntimeHelperCall(::CodegenStElemNoCheck);
            return true;
        }

        case IROpCode::LdSFld: {
            if (!instr.has_dst())
                return false;
            enc_.EmitMovRIImm32(AT::kScratchB, instr.imm.field_offset);
            uint32_t call_pos = EmitRuntimeHelperCall(::CodegenLdSFld);
            {
                uint32_t call_token = 0, call_module = 0;
                if (config_.call_cache != nullptr && current_instr_index_ < config_.call_cache_count) {
                    const auto& cached = static_cast<const runtime_instantiation::CachedCallInfo*>(
                        config_.call_cache)[current_instr_index_];
                    call_token = cached.method_token;
                    call_module = cached.module_id;
                }
                call_sites_.push_back({current_instr_index_, call_pos, call_token, call_module});
            }
            RecordGcPoint(call_pos);
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::StSFld: {
            if (!instr.has_src1())
                return false;
            enc_.EmitMovRIImm32(AT::kScratchB, instr.imm.field_offset);
            LoadGpr(AT::kScratchC, instr.src1_reg());
            EmitRuntimeHelperCall(::CodegenStSFld);
            return true;
        }

        case IROpCode::CastClass: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            LoadGpr(AT::kScratchB, instr.src1_reg());
            enc_.EmitMovRIImm32(AT::kScratchC, instr.imm.field_offset);
            EmitRuntimeHelperCall(::CodegenCastClass);
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::IsInst: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            LoadGpr(AT::kScratchB, instr.src1_reg());
            enc_.EmitMovRIImm32(AT::kScratchC, instr.imm.field_offset);
            EmitRuntimeHelperCall(::CodegenIsInst);
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::LdVirtFtn: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            LoadGpr(AT::kScratchB, instr.src1_reg());
            enc_.EmitMovRIImm32(AT::kScratchC, instr.imm.field_offset);
            EmitRuntimeHelperCall(::CodegenLdVirtFtn);
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::InitObj: {
            if (!instr.has_src1())
                return false;
            LoadGpr(AT::kScratchB, instr.src1_reg());
            EmitRuntimeHelperCall(::CodegenInitObj);
            return true;
        }

        case IROpCode::StObj: {
            if (!instr.has_src1() || !instr.has_src2())
                return false;
            LoadGpr(AT::kScratchB, instr.src1_reg());
            LoadGpr(AT::kScratchC, instr.src2_reg());
            EmitRuntimeHelperCall(::CodegenStObj);
            return true;
        }

        case IROpCode::StInd: {
            if (!instr.has_src1() || !instr.has_src2())
                return false;
            LoadGpr(AT::kScratchA, instr.src1_reg());
            LoadGpr(AT::kScratchB, instr.src2_reg());
            enc_.EmitMovMR(AT::kScratchA, 0, AT::kScratchB);
            return true;
        }

        case IROpCode::LdInd: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            LoadGpr(AT::kScratchB, instr.src1_reg());
            enc_.EmitMovRM(AT::kScratchA, AT::kScratchB, 0);
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::LdObj: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            LoadGpr(AT::kScratchB, instr.src1_reg());
            uint32_t call_pos = EmitRuntimeHelperCall(::CodegenLdObj);
            {
                uint32_t call_token = 0, call_module = 0;
                if (config_.call_cache != nullptr && current_instr_index_ < config_.call_cache_count) {
                    const auto& cached = static_cast<const runtime_instantiation::CachedCallInfo*>(
                        config_.call_cache)[current_instr_index_];
                    call_token = cached.method_token;
                    call_module = cached.module_id;
                }
                call_sites_.push_back({current_instr_index_, call_pos, call_token, call_module});
            }
            RecordGcPoint(call_pos);
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::Cpblk: {
            if (!instr.has_src1() || !instr.has_src2())
                return false;
            LoadGpr(AT::kScratchB, instr.src1_reg());
            LoadGpr(AT::kScratchC, instr.src2_reg());
            if (instr.flags() & ::chaos::il2cpp::interpreter::kRegHasSrc3)
                LoadGpr(AT::kExtraScratch0, instr.src3_reg());
            EmitRuntimeHelperCall(::CodegenCpblk);
            return true;
        }

        case IROpCode::InitBlk: {
            if (!instr.has_src1() || !instr.has_src2())
                return false;
            LoadGpr(AT::kScratchB, instr.src1_reg());
            LoadGpr(AT::kScratchC, instr.src2_reg());
            if (instr.flags() & ::chaos::il2cpp::interpreter::kRegHasSrc3)
                LoadGpr(AT::kExtraScratch0, instr.src3_reg());
            EmitRuntimeHelperCall(::CodegenInitBlk);
            return true;
        }

        case IROpCode::LocAlloc: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            LoadGpr(AT::kScratchB, instr.src1_reg()); // size
            if (localloc_extra_ > 0) {
                // Stack allocation from pre-allocated frame reserve.
                // RDX = base address of localloc reserve.
                enc_.EmitLeaRM(
                    AT::kScratchC, AT::kStackReg,
                    static_cast<int32_t>(kFrameSize + frame_size_extra_ + frame_align_adj_ + xmm_save_size_ + 8));
                // R8 = bump counter pointer (8 bytes, at kFrameSize + align_adj + xmm_save).
                enc_.EmitLeaRM(
                    AT::kExtraScratch0, AT::kStackReg,
                    static_cast<int32_t>(kFrameSize + frame_size_extra_ + frame_align_adj_ + xmm_save_size_));
                EmitRuntimeHelperCall(reinterpret_cast<void*>(::CodegenLocAlloc));
            } else {
                // Fallback: heap allocation (no stack reserve — rare edge case).
                EmitRuntimeHelperCall(reinterpret_cast<void*>(::CodegenLocAlloc));
            }
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::Dup: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            LoadGpr(AT::kScratchA, instr.src1_reg());
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::LdArgA: {
            if (!instr.has_dst())
                return false;
            uint32_t arg_idx = instr.imm.operand_index;
            enc_.EmitLeaRM(AT::kScratchA, AT::kArgsBuf, static_cast<int32_t>(arg_idx * 8));
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::LdLocA: {
            if (!instr.has_dst())
                return false;
            uint32_t local_idx = instr.imm.operand_index;
            uint32_t vreg = kLocalRegBase + local_idx;
            enc_.EmitLeaRM(AT::kScratchA, AT::kStackReg, static_cast<int32_t>(GprOff(vreg)));
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::Call:
        case IROpCode::CallBridge: {
            if (!instr.has_src1())
                return false;
            uint32_t first_arg_reg = instr.src1_reg();
            uint32_t arg_count = instr.call_arg_count();
            void* target_fn = instr.imm.ptr;
            if (target_fn == nullptr)
                return false;
            static constexpr uint8_t kArgRegs[] = {AT::kScratchB, AT::kScratchC, AT::kExtraScratch0,
                                                   AT::kExtraScratch1};
            uint32_t max_scratch = arg_count < 4 ? arg_count : 4;
            for (uint32_t i = 0; i < max_scratch; ++i)
                LoadGpr(kArgRegs[i], first_arg_reg + i);
            for (uint32_t i = 4; i < arg_count; ++i) {
                LoadGpr(AT::kScratchA, first_arg_reg + i);
                enc_.EmitMovMR(AT::kStackReg, static_cast<int32_t>((i - 4) * 8), AT::kScratchA);
            }
            // Spill cached/colored registers before call (same as EmitCallWithSpill preamble)
            if (config_.enable_register_caching && cached_slots_used_)
                SpillCachedRegs();
            if (has_graph_coloring_) {
                for (uint32_t vr = 0; vr < kGprCount; ++vr) {
                    uint8_t colored_x64 = gcr_.gpr_color[vr];
                    if (colored_x64 != 0xFF) {
                        if (caller_colored_mask_ & (1ULL << vr))
                            continue;
                        enc_.EmitMovMR(AT::kStackReg, static_cast<int32_t>(GprOff(vr)), colored_x64);
                    }
                }
                // Spill colored FPRs (same logic: StoreFpr skips stack write).
                for (uint32_t fi = 0; fi < 32; ++fi) {
                    uint8_t colored_xmm = gcr_.fpr_color[fi];
                    if (colored_xmm != 0xFF) {
                        if (caller_fpr_colored_mask_ & (1ULL << fi))
                            continue;
                        enc_.EmitMovdqaMR(AT::kStackReg, static_cast<int32_t>(FprOff(kGprCount + fi)), colored_xmm);
                    }
                }
            }
            // Slot-based call: emit call [rip+0] placeholder, record SlotPatch.
            // The slot table is emitted after all instructions, before buf_.Seal().
            uint32_t call_start = buf_.pos();
            enc_.EmitCallRipRel(0); // 6 bytes: FF 15 <disp32>
            uint32_t call_pos = call_start;
            uint32_t slot_patch_offset = call_start + kSlotPatchDispOff; // disp32 starts at byte 2 of call [rip+disp32]
            slot_patches_.push_back({slot_patch_offset, static_cast<uint32_t>(call_sites_.size()), target_fn});
            slot_count_used_++;
#if defined(__aarch64__)
            // ARM64 ABI returns values in X0, but StoreGpr below expects kScratchA (X9).
            enc_.EmitMovRR(AT::kScratchA, 0); // MOV X9, X0
#endif
            // Post-call reload (same as EmitCallWithSpill postamble)
            if (has_graph_coloring_ && caller_colored_mask_) {
                uint64_t mask = caller_colored_mask_;
                for (uint32_t vr = 0; mask; ++vr) {
                    if (mask & 1) {
                        uint8_t colored_x64 = gcr_.gpr_color[vr];
                        enc_.EmitMovRM(colored_x64, AT::kStackReg, static_cast<int32_t>(GprOff(vr)));
                    }
                    mask >>= 1;
                }
            }
            // Post-call reload: restore caller-saved colored FPRs.
            if (has_graph_coloring_ && caller_fpr_colored_mask_) {
                uint64_t mask = caller_fpr_colored_mask_;
                for (uint32_t fi = 0; mask; ++fi) {
                    if (mask & 1) {
                        uint8_t colored_xmm = gcr_.fpr_color[fi];
                        enc_.EmitMovdqaRM(colored_xmm, AT::kStackReg, static_cast<int32_t>(FprOff(kGprCount + fi)));
                    }
                    mask >>= 1;
                }
            }
            {
                uint32_t call_token = 0, call_module = 0;
                if (config_.call_cache != nullptr && current_instr_index_ < config_.call_cache_count) {
                    const auto& cached = static_cast<const runtime_instantiation::CachedCallInfo*>(
                        config_.call_cache)[current_instr_index_];
                    call_token = cached.method_token;
                    call_module = cached.module_id;
                }
                call_sites_.push_back({current_instr_index_, call_pos, call_token, call_module});
            }
            RecordGcPoint(call_pos);
            if (config_.enable_deopt) {
                uint32_t val_start = static_cast<uint32_t>(deopt_values_.size());
                for (uint32_t vr = 0; vr < kGprCount; ++vr) {
                    DeoptValue dv;
                    dv.reg_index = vr;
                    dv.value_tag = (vr < static_cast<uint32_t>(vreg_types_.size()))
                                     ? vreg_types_[vr]
                                     : static_cast<uint8_t>(interpreter::ValueTag::Int64);
                    dv.is_spilled = true;
                    dv.spill_offset = static_cast<int16_t>(GprOff(vr));
                    deopt_values_.push_back(dv);
                }
                for (uint32_t vr = kGprCount; vr < kGprCount + kFprCount; ++vr) {
                    DeoptValue dv;
                    dv.reg_index = vr;
                    dv.value_tag = static_cast<uint8_t>(interpreter::ValueTag::Float64);
                    dv.is_spilled = true;
                    dv.spill_offset = static_cast<int16_t>(FprOff(vr));
                    deopt_values_.push_back(dv);
                }
                uint32_t n_vals = static_cast<uint32_t>(deopt_values_.size()) - val_start;
                DeoptEntry entry;
                entry.native_offset = call_pos;
                entry.instr_pc = current_instr_index_;
                entry.num_values = n_vals;
                entry.values_offset = val_start;
                deopt_entries_.push_back(entry);
            }
            if (instr.has_dst())
                StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::Calli: {
            if (!instr.has_src1())
                return false;
            uint32_t first_arg_reg = instr.src1_reg();
            uint32_t arg_count = instr.call_arg_count();
            uint32_t func_ptr_vreg = instr.imm.operand_index;

            LoadGpr(AT::kScratchA, func_ptr_vreg);
            enc_.EmitTestRR(AT::kScratchA, AT::kScratchA);
            uint32_t non_null_pos = buf_.pos();
            enc_.EmitJccRel32(kCC_NE, 0);
            // Null function pointer → deopt (fall-through)
            if (config_.enable_deopt) {
                EmitDeoptSequence(current_instr_index_);
            } else {
                return false;
            }
            // Patch jnz to land here (non-null case)
            uint32_t after_deopt = buf_.pos();
            int32_t disp = static_cast<int32_t>(after_deopt - (non_null_pos + 6));
            buf_.Patch32(non_null_pos + 2, static_cast<uint32_t>(disp));

            // Load args into calling convention registers
            static constexpr uint8_t kArgRegs[] = {AT::kScratchB, AT::kScratchC, AT::kExtraScratch0,
                                                   AT::kExtraScratch1};
            uint32_t max_scratch = arg_count < 4 ? arg_count : 4;
            for (uint32_t i = 0; i < max_scratch; ++i)
                LoadGpr(kArgRegs[i], first_arg_reg + i);
            for (uint32_t i = 4; i < arg_count; ++i) {
                LoadGpr(AT::kScratchA, first_arg_reg + i);
                enc_.EmitMovMR(AT::kStackReg, static_cast<int32_t>((i - 4) * 8), AT::kScratchA);
            }

            LoadGpr(AT::kScratchA, func_ptr_vreg);
            uint32_t call_pos = buf_.pos();
            EmitCallWithSpill(AT::kScratchA);
            {
                uint32_t call_token = 0, call_module = 0;
                if (config_.call_cache != nullptr && current_instr_index_ < config_.call_cache_count) {
                    const auto& cached = static_cast<const runtime_instantiation::CachedCallInfo*>(
                        config_.call_cache)[current_instr_index_];
                    call_token = cached.method_token;
                    call_module = cached.module_id;
                }
                call_sites_.push_back({current_instr_index_, call_pos, call_token, call_module});
            }
            RecordGcPoint(call_pos);

            if (config_.enable_deopt) {
                uint32_t val_start = static_cast<uint32_t>(deopt_values_.size());
                for (uint32_t vr = 0; vr < kGprCount; ++vr) {
                    DeoptValue dv;
                    dv.reg_index = vr;
                    dv.value_tag = (vr < static_cast<uint32_t>(vreg_types_.size()))
                                     ? vreg_types_[vr]
                                     : static_cast<uint8_t>(interpreter::ValueTag::Int64);
                    dv.is_spilled = true;
                    dv.spill_offset = static_cast<int16_t>(GprOff(vr));
                    deopt_values_.push_back(dv);
                }
                for (uint32_t vr = kGprCount; vr < kGprCount + kFprCount; ++vr) {
                    DeoptValue dv;
                    dv.reg_index = vr;
                    dv.value_tag = static_cast<uint8_t>(interpreter::ValueTag::Float64);
                    dv.is_spilled = true;
                    dv.spill_offset = static_cast<int16_t>(FprOff(vr));
                    deopt_values_.push_back(dv);
                }
                uint32_t n_vals = static_cast<uint32_t>(deopt_values_.size()) - val_start;
                DeoptEntry entry;
                entry.native_offset = call_pos;
                entry.instr_pc = current_instr_index_;
                entry.num_values = n_vals;
                entry.values_offset = val_start;
                deopt_entries_.push_back(entry);
            }
            if (instr.has_dst())
                StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::CallVirt:
        case IROpCode::CallVirtConstrained: {
            if (!instr.has_src1())
                return false;
            uint32_t first_arg_reg = instr.src1_reg();
            uint32_t arg_count = instr.call_arg_count();

            // ── Inline PIC multi-slot fast path ─────────────────────────
            // Check for per-instruction PIC data from entry_direct.cpp.
            // When up to 3 PIC slots have resolved direct_fn entries and
            // arg_count <= 4, emit an inline type check chain + direct
            // calls instead of going through the full CodegenCallVirt
            // C helper.  RCX holds the object type_token (loaded once
            // from [obj+56]) and is reused across all slot checks.
            uint32_t pic_slot_count = 0;
            uint32_t inline_expected_types[3] = {};
            void* inline_direct_fns[3] = {};
            if (config_.per_instr_pic != nullptr && current_instr_index_ < config_.per_instr_pic_count) {
                const auto& pic = config_.per_instr_pic[current_instr_index_];
                if (pic.slot_count > 0 && arg_count <= 4) {
                    pic_slot_count = pic.slot_count;
                    for (uint32_t si = 0; si < pic_slot_count && si < 3; ++si) {
                        inline_expected_types[si] = pic.expected_type_tokens[si];
                        inline_direct_fns[si] = pic.direct_fns[si];
                    }
                }
            }
            std::vector<uint32_t> inline_miss_jumps; // jne positions (per slot)
            std::vector<uint32_t> inline_done_jumps; // jmp .done positions (per slot)
            uint32_t inline_null_jmp_pos = 0;

            if (pic_slot_count > 0) {
                // Load receiver from register file (via LoadGpr for correct cache/coloring)
                LoadGpr(AT::kScratchA, first_arg_reg);
                enc_.EmitTestRR(AT::kScratchA, AT::kScratchA);
                inline_null_jmp_pos = buf_.pos();
                enc_.EmitJccRel32(kCC_E, 0); // je .use_c_helper (null receiver)

                // Load type_token from object at offset 56 (= sizeof(SmallFieldArray))
                // RCX holds type_token across ALL slot checks — loaded once here.
                enc_.EmitMovRM(AT::kScratchB, AT::kScratchA, 56);

                // Generate type check chain for up to pic_slot_count slots
                for (uint32_t si = 0; si < pic_slot_count; ++si) {
                    // Compare RCX (type_token) with expected type for this slot
                    enc_.EmitMovRIImm32(AT::kScratchA, inline_expected_types[si]);
                    enc_.EmitCmpRR(AT::kScratchA, AT::kScratchB);
                    // Miss jump — patched after all slot code is emitted
                    inline_miss_jumps.push_back(buf_.pos());
                    enc_.EmitJccRel32(kCC_NE, 0); // jne .next (or .use_c_helper if last)

                    // ── Slot hit: load args and call ──
                    // Load Win64 calling convention registers from register file.
                    // RCX is overwritten with first arg (type_token no longer needed).
                    if (arg_count >= 1)
                        LoadGpr(AT::kScratchB, first_arg_reg);
                    if (arg_count >= 2)
                        LoadGpr(AT::kScratchC, first_arg_reg + 1);
                    if (arg_count >= 3)
                        LoadGpr(AT::kExtraScratch0, first_arg_reg + 2);
                    if (arg_count >= 4)
                        LoadGpr(AT::kExtraScratch1, first_arg_reg + 3);

                    // Direct call to pre-resolved AOT function pointer
                    uint32_t call_pos = buf_.pos();
                    EmitRuntimeHelperCall(inline_direct_fns[si]);

                    // Record call site for hotpatch tracking and GC point
                    {
                        uint32_t call_token = 0, call_module = 0;
                        if (config_.call_cache != nullptr && current_instr_index_ < config_.call_cache_count) {
                            const auto& cached = static_cast<const runtime_instantiation::CachedCallInfo*>(
                                config_.call_cache)[current_instr_index_];
                            call_token = cached.method_token;
                            call_module = cached.module_id;
                        }
                        call_sites_.push_back({current_instr_index_, call_pos, call_token, call_module});
                    }
                    RecordGcPoint(call_pos);

                    // Deoptimization metadata for inline path
                    if (config_.enable_deopt) {
                        uint32_t val_start = static_cast<uint32_t>(deopt_values_.size());
                        for (uint32_t vr = 0; vr < kGprCount; ++vr) {
                            DeoptValue dv;
                            dv.reg_index = vr;
                            dv.value_tag = (vr < static_cast<uint32_t>(vreg_types_.size()))
                                             ? vreg_types_[vr]
                                             : static_cast<uint8_t>(interpreter::ValueTag::Int64);
                            dv.is_spilled = true;
                            dv.spill_offset = static_cast<int16_t>(GprOff(vr));
                            deopt_values_.push_back(dv);
                        }
                        for (uint32_t vr = kGprCount; vr < kGprCount + kFprCount; ++vr) {
                            DeoptValue dv;
                            dv.reg_index = vr;
                            dv.value_tag = static_cast<uint8_t>(interpreter::ValueTag::Float64);
                            dv.is_spilled = true;
                            dv.spill_offset = static_cast<int16_t>(FprOff(vr));
                            deopt_values_.push_back(dv);
                        }
                        uint32_t n_vals = static_cast<uint32_t>(deopt_values_.size()) - val_start;
                        DeoptEntry entry;
                        entry.native_offset = call_pos;
                        entry.instr_pc = current_instr_index_;
                        entry.num_values = n_vals;
                        entry.values_offset = val_start;
                        deopt_entries_.push_back(entry);
                    }

                    // Store return value before deopt check (deopt clobbers RAX)
                    if (instr.has_dst())
                        StoreGpr(AT::kScratchA, instr.dst_reg());

                    // Check ret_buf[0] for kDeoptMagic
                    // kRetBuf may have been clobbered -- reload from stack save slot.
                    enc_.EmitMovRM(AT::kScratchC, AT::kFrameReg, -16);
                    enc_.EmitMovRM(AT::kScratchB, AT::kScratchC, 0);
                    enc_.EmitMovImm64(AT::kScratchA, kDeoptMagic);
                    enc_.EmitCmpRR(AT::kScratchA, AT::kScratchB);
                    uint32_t inline_jne_patch_off = buf_.pos() + 2;
                    enc_.EmitJccRel32(kCC_NE, 0); // jne .check_stale

                    // Deopt path: jump to common deopt trampoline
                    {
                        uint32_t inline_deopt_patch = buf_.pos() + 1;
                        enc_.EmitJmpRel32(0);
                        deopt_jump_patches_.push_back({inline_deopt_patch});
                    }

                    // .check_stale — also check HotUpdate stale flag
                    {
                        // Patch the jne from ret_buf check to land here (not stale → skip deopt)
                        uint32_t check_stale_off = buf_.pos();
                        int32_t disp = static_cast<int32_t>(check_stale_off - (inline_jne_patch_off + 4));
                        buf_.Patch32(inline_jne_patch_off, static_cast<uint32_t>(disp));

                        // Load stale flag (atomic<bool>, 0 or 1) from pre-computed pointer
                        enc_.EmitMovImm64(AT::kScratchA, reinterpret_cast<uint64_t>(stale_flag_ptr_));
                        enc_.EmitMovRM(AT::kScratchA, AT::kScratchA, 0);
                        enc_.EmitTestRR(AT::kScratchA, AT::kScratchA);
                        uint32_t stale_done_patch = buf_.pos() + 2;
                        enc_.EmitJccRel32(kCC_E, 0); // je .slot_done (not stale)

                        // Stale: jump to common deopt trampoline
                        uint32_t stale_deopt_patch = buf_.pos() + 1;
                        enc_.EmitJmpRel32(0);
                        deopt_jump_patches_.push_back({stale_deopt_patch});

                        // .slot_done (normal return — skip deopt path)
                        uint32_t slot_done_off = buf_.pos();
                        {
                            int32_t done_disp = static_cast<int32_t>(slot_done_off - (stale_done_patch + 4));
                            buf_.Patch32(stale_done_patch, static_cast<uint32_t>(done_disp));
                        }

                    } // end check_stale + slot_done block

                    // Jump past C helper path
                    inline_done_jumps.push_back(buf_.pos());
                    enc_.EmitJmpRel32(0); // jmp .done
                } // end for-loop body (slot si)

                // ── Patch all slot miss jumps ──
                // For slot si, the miss jump target is right after this slot's
                // jmp .done instruction (= the next slot's check, or .use_c_helper
                // for the last slot).
                uint32_t c_helper_start = buf_.pos();
                for (size_t si = 0; si < inline_miss_jumps.size(); ++si) {
                    uint32_t target;
                    if (si + 1 < inline_done_jumps.size()) {
                        // Miss goes to next slot's check (right after this slot's jmp .done)
                        target = inline_done_jumps[si] + 5; // jmp rel32 = 5 bytes
                    } else {
                        // Last slot miss → fall through to C helper
                        target = c_helper_start;
                    }
                    int32_t disp = static_cast<int32_t>(target - (inline_miss_jumps[si] + 6));
                    buf_.Patch32(inline_miss_jumps[si] + 2, static_cast<uint32_t>(disp));
                }
                // Patch null receiver jump to C helper as well
                {
                    int32_t null_disp = static_cast<int32_t>(c_helper_start - (inline_null_jmp_pos + 6));
                    buf_.Patch32(inline_null_jmp_pos + 2, static_cast<uint32_t>(null_disp));
                }
            }

            // ── Existing C helper path (CodegenCallVirt) ────────────────────
            // Build CodegenCallVirtArgs at [rsp + kCallVirtArgsOff]
            enc_.EmitLeaRM(AT::kScratchB, AT::kStackReg, static_cast<int32_t>(kCallVirtArgsOff));
            enc_.EmitLeaRM(AT::kScratchA, AT::kStackReg, static_cast<int32_t>(kGprFileOff));
            enc_.EmitMovMR(AT::kScratchB, 0, AT::kScratchA); // gpr_base
            enc_.EmitMovImm64(AT::kScratchA, reinterpret_cast<uint64_t>(config_.pic_dispatch_data));
            enc_.EmitMovMR(AT::kScratchB, 8, AT::kScratchA); // pic_data
            enc_.EmitMovImm64(AT::kScratchA, reinterpret_cast<uint64_t>(config_.dispatch_ctx));
            enc_.EmitMovMR(AT::kScratchB, 16, AT::kScratchA); // dispatch_ctx
            enc_.EmitMovRIImm32(AT::kScratchA, current_instr_index_);
            enc_.EmitMovMR(AT::kScratchB, 24, AT::kScratchA); // instruction_idx
            enc_.EmitMovRIImm32(AT::kScratchA, arg_count);
            enc_.EmitMovMR(AT::kScratchB, 28, AT::kScratchA); // arg_count
            enc_.EmitMovRIImm32(AT::kScratchA, first_arg_reg);
            enc_.EmitMovMR(AT::kScratchB, 32, AT::kScratchA); // first_arg_reg

            // method_token at offset 36 (from call_cache or 0)
            {
                uint32_t mt = 0;
                if (config_.call_cache != nullptr && current_instr_index_ < config_.call_cache_count) {
                    const auto& cached = static_cast<const runtime_instantiation::CachedCallInfo*>(
                        config_.call_cache)[current_instr_index_];
                    mt = cached.method_token;
                }
                enc_.EmitMovRIImm32(AT::kScratchA, mt);
                enc_.EmitMovMR(AT::kScratchB, 36, AT::kScratchA); // method_token
            }

            enc_.EmitMovImm64(AT::kScratchA, reinterpret_cast<uint64_t>(instr.imm.ptr));
            enc_.EmitMovMR(AT::kScratchB, 40, AT::kScratchA); // call_target
            enc_.EmitMovRIImm32(AT::kScratchA, instr.has_dst() ? 1 : 0);
            enc_.EmitMovMR(AT::kScratchB, 48, AT::kScratchA); // has_dst
            enc_.EmitMovRIImm32(AT::kScratchA, static_cast<uint32_t>((instr.header >> 63) & 1));
            enc_.EmitMovMR(AT::kScratchB, 52, AT::kScratchA);  // is_instance_call
            enc_.EmitMovRM(AT::kScratchC, AT::kFrameReg, -16); // reload kRetBuf from stack save
            enc_.EmitMovMR(AT::kScratchB, 56, AT::kScratchC);  // ret_buf = saved kRetBuf

            // call CodegenCallVirt
            uint32_t call_pos = buf_.pos();
            EmitRuntimeHelperCall(reinterpret_cast<void*>(::CodegenCallVirt));
            {
                uint32_t call_token = 0, call_module = 0;
                if (config_.call_cache != nullptr && current_instr_index_ < config_.call_cache_count) {
                    const auto& cached = static_cast<const runtime_instantiation::CachedCallInfo*>(
                        config_.call_cache)[current_instr_index_];
                    call_token = cached.method_token;
                    call_module = cached.module_id;
                }
                call_sites_.push_back({current_instr_index_, call_pos, call_token, call_module});
            }
            RecordGcPoint(call_pos);

            // Deoptimization metadata for CallVirt: if CodegenCallVirt takes the
            // PIC miss path, it needs DeoptEntry/DeoptValue to extract type tags
            // for the register file at this call site.
            if (config_.enable_deopt) {
                uint32_t val_start = static_cast<uint32_t>(deopt_values_.size());
                for (uint32_t vr = 0; vr < kGprCount; ++vr) {
                    DeoptValue dv;
                    dv.reg_index = vr;
                    dv.value_tag = (vr < static_cast<uint32_t>(vreg_types_.size()))
                                     ? vreg_types_[vr]
                                     : static_cast<uint8_t>(interpreter::ValueTag::Int64);
                    dv.is_spilled = true;
                    dv.spill_offset = static_cast<int16_t>(GprOff(vr));
                    deopt_values_.push_back(dv);
                }
                for (uint32_t vr = kGprCount; vr < kGprCount + kFprCount; ++vr) {
                    DeoptValue dv;
                    dv.reg_index = vr;
                    dv.value_tag = static_cast<uint8_t>(interpreter::ValueTag::Float64);
                    dv.is_spilled = true;
                    dv.spill_offset = static_cast<int16_t>(FprOff(vr));
                    deopt_values_.push_back(dv);
                }
                uint32_t n_vals = static_cast<uint32_t>(deopt_values_.size()) - val_start;
                DeoptEntry entry;
                entry.native_offset = call_pos;
                entry.instr_pc = current_instr_index_;
                entry.num_values = n_vals;
                entry.values_offset = val_start;
                deopt_entries_.push_back(entry);
            }
            // Check ret_buf[0] for kDeoptMagic.
            // kRetBuf may have been clobbered -- reload from stack save slot.
            enc_.EmitMovRM(AT::kScratchC, AT::kFrameReg, -16);
            enc_.EmitMovRM(AT::kScratchB, AT::kScratchC, 0);
            enc_.EmitMovImm64(AT::kScratchA, kDeoptMagic);
            enc_.EmitCmpRR(AT::kScratchA, AT::kScratchB);
            uint32_t jne_patch_off = buf_.pos() + 2;
            enc_.EmitJccRel32(kCC_NE, 0);

            // Deopt path
            uint32_t deopt_patch_off = buf_.pos() + 1;
            enc_.EmitJmpRel32(0);
            deopt_jump_patches_.push_back({deopt_patch_off});

            // .normal:
            uint32_t normal_offset = buf_.pos();
            int32_t jne_disp = static_cast<int32_t>(normal_offset - (jne_patch_off + 4));
            buf_.Patch32(jne_patch_off, static_cast<uint32_t>(jne_disp));

            if (instr.has_dst())
                StoreGpr(AT::kScratchA, instr.dst_reg());

            // .done (inline PIC fast path jumps here to skip C helper)
            {
                uint32_t done_pos = buf_.pos();
                for (size_t di = 0; di < inline_done_jumps.size(); ++di) {
                    int32_t done_disp = static_cast<int32_t>(done_pos - (inline_done_jumps[di] + 5));
                    buf_.Patch32(inline_done_jumps[di] + 1, static_cast<uint32_t>(done_disp));
                }
            }
            return true;
        }

        // ── SIMD operations ──────────────────────────────────────────────
        case IROpCode::Simd: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            int64_t meta = instr.imm.i8;
            uint8_t simd_op = static_cast<uint8_t>(meta & 0xFF);
            uint8_t elem_type = static_cast<uint8_t>((meta >> 8) & 0xFF);
            uint16_t simd_imm = static_cast<uint16_t>((meta >> 16) & 0xFFFF);
            return EmitSimd(instr, simd_op, elem_type, simd_imm);
        }

        // ── FMA (3-operand fused multiply-add) ──────────────────────────
        case IROpCode::SimdFma: {
            if (!instr.has_src1() || !instr.has_src2() || !instr.has_src3() || !instr.has_dst())
                return false;
            int64_t meta = instr.imm.i8;
            uint8_t fma_op = static_cast<uint8_t>(meta & 0xFF);
            uint8_t elem_type = static_cast<uint8_t>((meta >> 8) & 0xFF);

            uint8_t xmm_src1 = 1; // XMM1
            uint8_t xmm_src2 = 2; // XMM2
            uint8_t xmm_acc = 0;  // XMM0 (accumulator/dest)

            LoadFpr(xmm_src1, instr.src1_reg());
            LoadFpr(xmm_src2, instr.src2_reg());
            LoadFpr(xmm_acc, instr.src3_reg());

            // VEX 3-operand: acc = src1 * src2 + acc (231 form)
            switch (fma_op) {
                case 0:                 // kSimdFmaAdd
                    if (elem_type == 4) // kElemFloat32 → PS
                        enc_.EmitVfmadd231psRR(xmm_acc, xmm_src1, xmm_src2);
                    else // kElemFloat64 → PD
                        enc_.EmitVfmadd231pdRR(xmm_acc, xmm_src1, xmm_src2);
                    break;
                case 1: // kSimdFmaSub
                    if (elem_type == 4)
                        enc_.EmitVfmsub231psRR(xmm_acc, xmm_src1, xmm_src2);
                    else
                        enc_.EmitVfmsub231pdRR(xmm_acc, xmm_src1, xmm_src2);
                    break;
                case 2: // kSimdFmaNegAdd (negate multiply-add: a * b - c)
                    if (elem_type == 4)
                        enc_.EmitVfnmadd231psRR(xmm_acc, xmm_src1, xmm_src2);
                    else
                        enc_.EmitVfnmadd231pdRR(xmm_acc, xmm_src1, xmm_src2);
                    break;
                case 3: // kSimdFmaNegSub (negate multiply-sub: -(a * b) - c)
                    if (elem_type == 4)
                        enc_.EmitVfnmsub231psRR(xmm_acc, xmm_src1, xmm_src2);
                    else
                        enc_.EmitVfnmsub231pdRR(xmm_acc, xmm_src1, xmm_src2);
                    break;
                default:
                    return false;
            }

            StoreFpr(instr.dst_reg(), xmm_acc);
            return true;
        }

        // ── POPCNT / LZCNT (bit manipulation intrinsics) ────────────────
        case IROpCode::Popcnt: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            LoadGpr(AT::kScratchA, instr.src1_reg());
            enc_.EmitPopcntRR(AT::kScratchA, AT::kScratchA);
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::Lzcnt: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            LoadGpr(AT::kScratchA, instr.src1_reg());
            enc_.EmitLzcntRR(AT::kScratchA, AT::kScratchA);
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        default:
            if (config_.enable_deopt) {
                EmitDeoptSequence(current_instr_index_);
                return true;
            }
            return false;
    }
}

// ── OptimizeInstructions ──────────────────────────────────────────────────
// Performs constant folding + dead code elimination on register instructions.
// Two-pass: (1) use-count scan, (2) fold arithmetic + DCE + branch simplify.
// Does not change instruction count — marks removed entries to be skipped
// during emission via the returned removed_mask (empty = no optimizations).

} // namespace chaos::il2cpp::jit