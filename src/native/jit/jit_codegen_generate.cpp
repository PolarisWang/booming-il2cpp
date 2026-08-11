// jit_codegen_generate.cpp — method generation (Generate), IR optimizer pass
// (OptimizeInstructions), and the Compile/CanCompile entry points for
// NativeCodeGenerator (T2.4 module split of jit_engine.cpp).
//
// Pure move: OptimizeInstructions, Generate, Compile, CanCompile were
// relocated verbatim from jit_engine.cpp; no logic changed.

#ifndef NOMINMAX
#define NOMINMAX 1  // avoid Windows min/max macro clashing with std::min/std::max
#endif
#include "jit_engine.h"
#include "ArchTraits.h"
#include "jit_helpers.h"
#include "jit_seh.h"    // RegisterNativeCodeSection
#include "jit_unwind.h" // AllocRuntimeFunction, EmitDwarfCie, EmitDwarfFde
#include "slot_map.h"
#include "tree/jit_optimizer.h"
#include "code_buffer.h"
#include "IEncoder.h"
#include "codegen_bridge.h" // GcPointMapV0, GcSafepointV0, CHAOS_GC_SLOT_*
#include "jit_inline.h"     // g_inline_reverse_map
#include "../interpreter/ir_reg_alloc.h"
#include <gc_root_scanner.h> // GcRegisterSlotMap
#include <instantiation_engine.h>
#include <chaos/profile.h>
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h> // PRUNTIME_FUNCTION (Win64 .pdata/.xdata)
#endif
#if defined(__aarch64__)
#include "arm64_encoder.h"
#include "Arm64Encoder.h"
#else
#include "x64_encoder.h"
#include "X64Encoder.h"
#endif

#include <algorithm> // std::min/std::max

namespace chaos::il2cpp::jit {

static void OptimizeInstructions(CHAOS_IL2CPP_VECTOR(::chaos::il2cpp::interpreter::RegisterInstruction) & instrs,
                                 std::vector<uint8_t>& removed_mask, bool has_seh) noexcept {
    using IROpCode = ::chaos::il2cpp::interpreter::IROpCode;
    uint32_t n = static_cast<uint32_t>(instrs.size());
    if (n == 0)
        return;

    // ── Pass 1: Use count ────────────────────────────────────────────
    uint32_t use_count[64] = {};
    for (uint32_t i = 0; i < n; ++i) {
        const auto& ri = instrs[i];
        if (ri.has_src1() && ri.src1_reg() < 64)
            ++use_count[ri.src1_reg()];
        if (ri.has_src2() && ri.src2_reg() < 64)
            ++use_count[ri.src2_reg()];
        if ((ri.flags() & ::chaos::il2cpp::interpreter::kRegHasSrc3) && ri.src3_reg() < 64)
            ++use_count[ri.src3_reg()];
    }

    // ── Header builder for folded instructions ───────────────────────
    auto MakeHdr = [](IROpCode opc, uint8_t dst, uint8_t src1, uint8_t src2, uint8_t flags) -> uint64_t {
        return static_cast<uint64_t>(opc) | (static_cast<uint64_t>(dst) << 16) | (static_cast<uint64_t>(src1) << 24) |
               (static_cast<uint64_t>(src2) << 32) | (static_cast<uint64_t>(flags) << 40);
    };

    // Foldable pure-arithmetic opcodes (no side effects, no overflow).
    auto IsFoldable = [](IROpCode opc) -> bool {
        return opc == IROpCode::Add || opc == IROpCode::Sub || opc == IROpCode::Mul || opc == IROpCode::And ||
               opc == IROpCode::Or || opc == IROpCode::Xor || opc == IROpCode::Shl || opc == IROpCode::Shr;
    };

    auto FoldBinary = [](IROpCode opc, int32_t a, int32_t b) -> int32_t {
        switch (opc) {
            case IROpCode::Add:
                return a + b;
            case IROpCode::Sub:
                return a - b;
            case IROpCode::Mul:
                return a * b;
            case IROpCode::And:
                return a & b;
            case IROpCode::Or:
                return a | b;
            case IROpCode::Xor:
                return a ^ b;
            case IROpCode::Shl:
                return static_cast<int32_t>(static_cast<uint32_t>(a) << (static_cast<uint32_t>(b) & 0x1F));
            case IROpCode::Shr:
                return a >> (static_cast<uint32_t>(b) & 0x1F);
            default:
                return 0;
        }
    };

    // Find the most recent definition of `reg` as LdcI4 (scan backwards).
    // Returns nullptr if a backward branch targets an instruction between the def
    // and current_idx — loop back-edges can redefine the register on re-entry,
    // making the "constant" stale.
    auto FindDefLdcI4 = [&](uint32_t current_idx, uint32_t reg) -> const int32_t* {
        for (uint32_t j = current_idx; j > 0; --j) {
            const auto& prev = instrs[j - 1];
            if (prev.has_dst() && prev.dst_reg() == reg) {
                if (prev.op_code() == IROpCode::LdcI4) {
                    // Check for backward branches from current_idx onward that
                    // target an instruction between the def (j-1) and current_idx.
                    // Such back-edges skip the def on loop re-entry, making the
                    // "constant" stale.
                    for (uint32_t k = current_idx; k < instrs.size(); ++k) {
                        const auto& bi = instrs[k];
                        auto bopc = bi.op_code();
                        if (bi.has_imm() &&
                            (bopc == IROpCode::Br || bopc == IROpCode::BrTrue || bopc == IROpCode::BrFalse ||
                             bopc == IROpCode::Beq || bopc == IROpCode::BneUn || bopc == IROpCode::Blt ||
                             bopc == IROpCode::BltUn || bopc == IROpCode::Bgt || bopc == IROpCode::BgtUn ||
                             bopc == IROpCode::Ble || bopc == IROpCode::BleUn)) {
                            uint32_t target = bi.imm.branch_target;
                            // target < k = backward branch; target between def and
                            // current_idx means loop re-entry skips the LdcI4 def.
                            if (target < k && target >= j - 1 && target <= current_idx) {
                                return nullptr; // killed by loop back-edge
                            }
                        }
                    }
                    return &prev.imm.i4;
                }
                return nullptr; // non-LdcI4 def found
            }
        }
        return nullptr;
    };

    // ── Pass 2: Fold + DCE + branch simplify ─────────────────────────
    removed_mask.assign(n, 0);
    bool did_opt = false;

    for (uint32_t i = 0; i < n; ++i) {
        auto& ri = instrs[i];
        auto opc = ri.op_code();

        // (a) Constant folding — binary pure arithmetic with both srcs constant.
        // Skip when SEH exists: handler code placed linearly between branches
        // and join points can define registers that don't reach the use through
        // normal control flow, causing incorrect constant propagation.
        if (!has_seh && IsFoldable(opc) && ri.has_dst() && ri.has_src1() && ri.has_src2()) {
            const int32_t* v1 = FindDefLdcI4(i, ri.src1_reg());
            const int32_t* v2 = FindDefLdcI4(i, ri.src2_reg());
            if (v1 && v2) {
                int32_t result = FoldBinary(opc, *v1, *v2);
                ri.header =
                    MakeHdr(IROpCode::LdcI4, ri.dst_reg(), 0, 0,
                            ::chaos::il2cpp::interpreter::kRegHasDst | ::chaos::il2cpp::interpreter::kRegHasImm);
                ri.imm.i4 = result;
                did_opt = true;
                continue;
            }
        }

        // (a2) Constant folding — unary Neg/Not with constant src
        if (opc == IROpCode::Neg && ri.has_dst() && ri.has_src1()) {
            const int32_t* v = FindDefLdcI4(i, ri.src1_reg());
            if (v) {
                ri.header =
                    MakeHdr(IROpCode::LdcI4, ri.dst_reg(), 0, 0,
                            ::chaos::il2cpp::interpreter::kRegHasDst | ::chaos::il2cpp::interpreter::kRegHasImm);
                ri.imm.i4 = -*v;
                did_opt = true;
                continue;
            }
        }
        if (opc == IROpCode::Not && ri.has_dst() && ri.has_src1()) {
            const int32_t* v = FindDefLdcI4(i, ri.src1_reg());
            if (v) {
                ri.header =
                    MakeHdr(IROpCode::LdcI4, ri.dst_reg(), 0, 0,
                            ::chaos::il2cpp::interpreter::kRegHasDst | ::chaos::il2cpp::interpreter::kRegHasImm);
                ri.imm.i4 = ~(*v);
                did_opt = true;
                continue;
            }
        }

        // (b) DCE — unused LdcI4
        if (opc == IROpCode::LdcI4 && ri.has_dst()) {
            uint8_t dst = ri.dst_reg();
            if (dst < 64 && use_count[dst] == 0) {
                removed_mask[i] = 1;
                did_opt = true;
                continue;
            }
        }

        // (c) BrFalse simplification
        if (opc == IROpCode::BrFalse && ri.has_src1()) {
            const int32_t* v = FindDefLdcI4(i, ri.src1_reg());
            if (v) {
                uint32_t target = ri.imm.branch_target;
                if (*v == 0) {
                    // src == 0 → branch IS taken → unconditional Br
                    ri.header =
                        MakeHdr(IROpCode::Br, 0, 0, 0,
                                ::chaos::il2cpp::interpreter::kRegIsBranch | ::chaos::il2cpp::interpreter::kRegHasImm);
                    ri.imm.branch_target = target;
                } else {
                    // src != 0 → branch NOT taken → remove (fall through)
                    removed_mask[i] = 1;
                }
                did_opt = true;
            }
        }

        // (d) Unbox elimination: Box(dst_b, src) + Unbox(dst_u, dst_b) → Dup(dst_u, src)
        if (opc == IROpCode::Unbox && ri.has_src1()) {
            uint8_t unbox_src = ri.src1_reg();
            uint8_t unbox_dst = ri.has_dst() ? ri.dst_reg() : 0;
            for (uint32_t j = i; j > 0; --j) {
                auto& prev = instrs[j - 1];
                if (prev.has_dst() && prev.dst_reg() == unbox_src) {
                    if (prev.op_code() == IROpCode::Box && prev.has_src1() && unbox_src < 64 &&
                        use_count[unbox_src] == 1) // Box dst used only by this Unbox
                    {
                        uint8_t box_src1 = prev.src1_reg();
                        prev.header = MakeHdr(
                            IROpCode::Dup, unbox_dst, box_src1, 0,
                            ::chaos::il2cpp::interpreter::kRegHasDst | ::chaos::il2cpp::interpreter::kRegHasSrc1);
                        prev.imm.i4 = 0;
                        removed_mask[i] = 1;
                        did_opt = true;
                    }
                    break;
                }
            }
        }

        // (e) Dead store elimination: consecutive StLoc to same local
        if (opc == IROpCode::StLoc && ri.has_src1() && i > 0) {
            auto& prev = instrs[i - 1];
            if (prev.op_code() == IROpCode::StLoc && prev.imm.operand_index == ri.imm.operand_index &&
                !removed_mask[i - 1]) {
                // Previous StLoc writes to the same local vreg and is immediately
                // overwritten — safe to remove regardless of use_count.
                removed_mask[i - 1] = 1;
                did_opt = true;
            }
        }

        // (e2) Extended dead store: non-adjacent StLoc to same local (EBB-safe scan)
        if (opc == IROpCode::StLoc && ri.has_src1() && i > 0) {
            uint32_t local_idx = ri.imm.operand_index;
            const uint32_t kMaxScan = 50;
            for (uint32_t j = i; j > 0 && (i - j) < kMaxScan; --j) {
                uint32_t idx = j - 1;
                if (removed_mask[idx])
                    continue;
                auto& prev = instrs[idx];
                auto prev_opc = prev.op_code();
                // Stop at branches, calls, or terminators (different execution path)
                if (prev_opc == IROpCode::Br || prev_opc == IROpCode::BrTrue || prev_opc == IROpCode::BrFalse ||
                    prev_opc == IROpCode::Beq || prev_opc == IROpCode::Blt || prev_opc == IROpCode::Bgt ||
                    prev_opc == IROpCode::Ble || prev_opc == IROpCode::Bge || prev_opc == IROpCode::BneUn ||
                    prev_opc == IROpCode::BgeUn || prev_opc == IROpCode::BgtUn || prev_opc == IROpCode::BleUn ||
                    prev_opc == IROpCode::BltUn || prev_opc == IROpCode::Switch || prev_opc == IROpCode::Call ||
                    prev_opc == IROpCode::CallVirt)
                    break;
                if (prev_opc == IROpCode::StLoc && prev.imm.operand_index == local_idx) {
                    removed_mask[idx] = 1; // previous StLoc is dead
                    did_opt = true;
                    break;
                }
                if (prev_opc == IROpCode::LdLoc && prev.src1_reg() == local_idx) {
                    break; // LdLoc reads this local — not dead
                }
            }
        }

        // (f) Copy propagation: Dup elimination + forwarding
        if (opc == IROpCode::Dup && ri.has_dst() && ri.has_src1()) {
            uint8_t dup_dst = ri.dst_reg();
            uint8_t dup_src = ri.src1_reg();
            // (f1) Dead Dup: dst never read
            if (dup_dst < 64 && use_count[dup_dst] == 0) {
                removed_mask[i] = 1;
                did_opt = true;
                continue;
            }
            // (f2) Single-use: propagate to immediately next instruction
            if (dup_dst < 64 && use_count[dup_dst] == 1 && i + 1 < n) {
                auto& next = instrs[i + 1];
                uint64_t nh = next.header;
                bool changed = false;
                if (next.has_src1() && next.src1_reg() == dup_dst) {
                    nh = (nh & ~(0xFFULL << 24)) | (static_cast<uint64_t>(dup_src) << 24);
                    changed = true;
                }
                if (next.has_src2() && next.src2_reg() == dup_dst) {
                    nh = (nh & ~(0xFFULL << 32)) | (static_cast<uint64_t>(dup_src) << 32);
                    changed = true;
                }
                if (changed) {
                    next.header = nh;
                    removed_mask[i] = 1;
                    did_opt = true;
                }
            }
            // (f3) Multi-hop Dup forwarding: find first non-removed user past i+1
            if (!removed_mask[i] && dup_dst < 64 && use_count[dup_dst] == 1) {
                for (uint32_t k = i + 1; k < n; ++k) {
                    if (removed_mask[k])
                        continue;
                    auto& later = instrs[k];
                    uint64_t nh = later.header;
                    bool changed = false;
                    if (later.has_src1() && later.src1_reg() == dup_dst) {
                        nh = (nh & ~(0xFFULL << 24)) | (static_cast<uint64_t>(dup_src) << 24);
                        changed = true;
                    }
                    if (later.has_src2() && later.src2_reg() == dup_dst) {
                        nh = (nh & ~(0xFFULL << 32)) | (static_cast<uint64_t>(dup_src) << 32);
                        changed = true;
                    }
                    if (changed) {
                        later.header = nh;
                        removed_mask[i] = 1;
                        did_opt = true;
                    }
                    break; // only forward to the first non-removed user
                }
            }
        }

        // (g) Redundant LdLoc elimination
        if (opc == IROpCode::LdLoc && ri.has_dst() && ri.has_src1()) {
            uint8_t ldst = ri.dst_reg();
            // (g1) Dead LdLoc: dst never read
            if (ldst < 64 && use_count[ldst] == 0) {
                removed_mask[i] = 1;
                did_opt = true;
                continue;
            }
            // (g2) Single-use StLoc→LdLoc forwarding (adjacent)
            if (ldst < 64 && use_count[ldst] == 1 && i > 0 && i + 1 < n) {
                auto& prev = instrs[i - 1];
                uint8_t local_vreg = ri.src1_reg();
                if (prev.op_code() == IROpCode::StLoc && !removed_mask[i - 1] && prev.has_dst() &&
                    prev.dst_reg() == local_vreg && prev.has_src1()) {
                    uint8_t store_src = prev.src1_reg();
                    auto& next = instrs[i + 1];
                    uint64_t nh = next.header;
                    bool changed = false;
                    if (next.has_src1() && next.src1_reg() == ldst) {
                        nh = (nh & ~(0xFFULL << 24)) | (static_cast<uint64_t>(store_src) << 24);
                        changed = true;
                    }
                    if (next.has_src2() && next.src2_reg() == ldst) {
                        nh = (nh & ~(0xFFULL << 32)) | (static_cast<uint64_t>(store_src) << 32);
                        changed = true;
                    }
                    if (changed) {
                        next.header = nh;
                        removed_mask[i] = 1;
                        did_opt = true;
                    }
                }
            }
            // (g3) Non-adjacent StLoc→LdLoc forwarding (single-use, EBB-safe)
            if (!removed_mask[i] && ldst < 64 && use_count[ldst] == 1 && i > 0) {
                uint8_t local_vreg = ri.src1_reg();
                const uint32_t kMaxScan = 50;
                // Scan backward for the defining StLoc
                for (uint32_t j = i; j > 0 && (i - j) < kMaxScan; --j) {
                    uint32_t idx = j - 1;
                    if (removed_mask[idx])
                        continue;
                    auto& prev = instrs[idx];
                    auto prev_opc = prev.op_code();
                    if (prev_opc == IROpCode::StLoc && prev.imm.operand_index == local_vreg && prev.has_src1()) {
                        uint8_t store_src = prev.src1_reg();
                        // Scan forward from LdLoc for the single user
                        for (uint32_t k = i + 1; k < n; ++k) {
                            if (removed_mask[k])
                                continue;
                            auto& later = instrs[k];
                            uint64_t nh = later.header;
                            bool changed = false;
                            if (later.has_src1() && later.src1_reg() == ldst) {
                                nh = (nh & ~(0xFFULL << 24)) | (static_cast<uint64_t>(store_src) << 24);
                                changed = true;
                            }
                            if (later.has_src2() && later.src2_reg() == ldst) {
                                nh = (nh & ~(0xFFULL << 32)) | (static_cast<uint64_t>(store_src) << 32);
                                changed = true;
                            }
                            if (changed) {
                                later.header = nh;
                                removed_mask[i] = 1;
                                did_opt = true;
                            }
                            break;
                        }
                        break;
                    }
                    // Stop at branches/calls
                    if (prev_opc == IROpCode::Br || prev_opc == IROpCode::BrTrue || prev_opc == IROpCode::BrFalse ||
                        prev_opc == IROpCode::Beq || prev_opc == IROpCode::Blt || prev_opc == IROpCode::Bgt ||
                        prev_opc == IROpCode::Ble || prev_opc == IROpCode::Bge || prev_opc == IROpCode::BneUn ||
                        prev_opc == IROpCode::BgeUn || prev_opc == IROpCode::BgtUn || prev_opc == IROpCode::BleUn ||
                        prev_opc == IROpCode::BltUn || prev_opc == IROpCode::Switch || prev_opc == IROpCode::Call ||
                        prev_opc == IROpCode::CallVirt)
                        break;
                }
            }
        }

        // (h) Branch-to-branch forwarding + dead Br elimination
        if ((opc == IROpCode::Br || opc == IROpCode::BrFalse || opc == IROpCode::BrTrue) && ri.has_imm()) {
            uint32_t target = ri.imm.branch_target;

            // (h1) Dead unconditional Br: branch to next instruction → fall-through
            if (opc == IROpCode::Br && target == i + 1) {
                removed_mask[i] = 1;
                did_opt = true;
                continue;
            }

            // (h2) Br/BrFalse/BrTrue forwarding: resolve Br → Br chains
            uint32_t resolved = target;
            uint32_t max_hop = 16;
            while (resolved < n && max_hop > 0) {
                auto& target_instr = instrs[resolved];
                if (target_instr.op_code() == IROpCode::Br && target_instr.has_imm() && !removed_mask[resolved]) {
                    uint32_t next_target = target_instr.imm.branch_target;
                    if (next_target == resolved)
                        break; // self-loop guard
                    resolved = next_target;
                    --max_hop;
                } else {
                    break;
                }
            }

            if (resolved != target) {
                ri.imm.branch_target = resolved;
                did_opt = true;
                // If unconditional Br now points to next instruction, remove it
                if (opc == IROpCode::Br && resolved == i + 1) {
                    removed_mask[i] = 1;
                }
            }
        }

        // (i) Arithmetic identity intrinsics: eliminate redundant operations
        if (ri.has_dst() && ri.has_src1()) {
            bool identity_opt = false;

            if (opc == IROpCode::Add || opc == IROpCode::Sub || opc == IROpCode::Or || opc == IROpCode::Xor ||
                opc == IROpCode::Shl || opc == IROpCode::Shr || opc == IROpCode::ShrUn) {
                // x op 0 → x (src2 is constant 0)
                if (ri.has_src2()) {
                    const int32_t* v2 = FindDefLdcI4(i, ri.src2_reg());
                    if (v2 && *v2 == 0) {
                        ri.header = MakeHdr(
                            IROpCode::Dup, ri.dst_reg(), ri.src1_reg(), 0,
                            ::chaos::il2cpp::interpreter::kRegHasDst | ::chaos::il2cpp::interpreter::kRegHasSrc1);
                        ri.imm.i4 = 0;
                        identity_opt = true;
                    }
                }
            }

            if (!identity_opt && opc == IROpCode::Mul && ri.has_src2()) {
                const int32_t* v2 = FindDefLdcI4(i, ri.src2_reg());
                if (v2) {
                    if (*v2 == 1) {
                        // x * 1 → x
                        ri.header = MakeHdr(
                            IROpCode::Dup, ri.dst_reg(), ri.src1_reg(), 0,
                            ::chaos::il2cpp::interpreter::kRegHasDst | ::chaos::il2cpp::interpreter::kRegHasSrc1);
                        ri.imm.i4 = 0;
                        identity_opt = true;
                    } else if (*v2 == 0) {
                        // x * 0 → 0
                        ri.header = MakeHdr(
                            IROpCode::LdcI4, ri.dst_reg(), 0, 0,
                            ::chaos::il2cpp::interpreter::kRegHasDst | ::chaos::il2cpp::interpreter::kRegHasImm);
                        ri.imm.i4 = 0;
                        identity_opt = true;
                    }
                }
            }

            if (!identity_opt && opc == IROpCode::And && ri.has_src2()) {
                if (ri.src1_reg() == ri.src2_reg()) {
                    // x & x → x
                    ri.header =
                        MakeHdr(IROpCode::Dup, ri.dst_reg(), ri.src1_reg(), 0,
                                ::chaos::il2cpp::interpreter::kRegHasDst | ::chaos::il2cpp::interpreter::kRegHasSrc1);
                    ri.imm.i4 = 0;
                    identity_opt = true;
                } else {
                    // x & 0 → 0 (src2 is constant 0)
                    const int32_t* v2 = FindDefLdcI4(i, ri.src2_reg());
                    if (v2 && *v2 == 0) {
                        ri.header = MakeHdr(
                            IROpCode::LdcI4, ri.dst_reg(), 0, 0,
                            ::chaos::il2cpp::interpreter::kRegHasDst | ::chaos::il2cpp::interpreter::kRegHasImm);
                        ri.imm.i4 = 0;
                        identity_opt = true;
                    }
                }
            }

            if (!identity_opt && opc == IROpCode::Or && ri.has_src2()) {
                if (ri.src1_reg() == ri.src2_reg()) {
                    // x | x → x
                    ri.header =
                        MakeHdr(IROpCode::Dup, ri.dst_reg(), ri.src1_reg(), 0,
                                ::chaos::il2cpp::interpreter::kRegHasDst | ::chaos::il2cpp::interpreter::kRegHasSrc1);
                    ri.imm.i4 = 0;
                    identity_opt = true;
                }
            }

            if (!identity_opt && opc == IROpCode::Xor && ri.has_src2()) {
                if (ri.src1_reg() == ri.src2_reg()) {
                    // x ^ x → 0
                    ri.header =
                        MakeHdr(IROpCode::LdcI4, ri.dst_reg(), 0, 0,
                                ::chaos::il2cpp::interpreter::kRegHasDst | ::chaos::il2cpp::interpreter::kRegHasImm);
                    ri.imm.i4 = 0;
                    identity_opt = true;
                }
            }

            if (!identity_opt && opc == IROpCode::Sub) {
                if (ri.has_src2() && ri.src1_reg() == ri.src2_reg()) {
                    // x - x → 0
                    ri.header =
                        MakeHdr(IROpCode::LdcI4, ri.dst_reg(), 0, 0,
                                ::chaos::il2cpp::interpreter::kRegHasDst | ::chaos::il2cpp::interpreter::kRegHasImm);
                    ri.imm.i4 = 0;
                    identity_opt = true;
                }
            }

            if (identity_opt) {
                did_opt = true;
                continue;
            }
        }

        // (j) DivUn/RemUn by power of 2 → shift/and
        // NOTE: only apply when use_count[src2_reg] == 1 — the optimization
        // modifies the defining LdcI4's immediate value in-place.  If other
        // instructions also read that LdcI4, they'd see the wrong value.
        if (ri.has_dst() && ri.has_src1() && ri.has_src2()) {
            if (opc == IROpCode::DivUn || opc == IROpCode::RemUn) {
                uint8_t src2_reg = ri.src2_reg();
                if (src2_reg < 64 && use_count[src2_reg] == 1) {
                    const int32_t* v2 = FindDefLdcI4(i, src2_reg);
                    if (v2 && *v2 > 0) {
                        uint32_t shift = 0;
                        uint32_t uv = static_cast<uint32_t>(*v2);
                        if (uv && (uv & (uv - 1)) == 0) {
                            while ((uv >> shift) > 1)
                                ++shift;
                            if (opc == IROpCode::DivUn) {
                                // DivUn by 2^k → ShrUn by k (logical shift, unsigned)
                                ri.header = MakeHdr(IROpCode::ShrUn, ri.dst_reg(), ri.src1_reg(), src2_reg,
                                                    ::chaos::il2cpp::interpreter::kRegHasDst |
                                                        ::chaos::il2cpp::interpreter::kRegHasSrc1 |
                                                        ::chaos::il2cpp::interpreter::kRegHasSrc2);
                                for (uint32_t j = i; j > 0; --j) {
                                    auto& prev = instrs[j - 1];
                                    if (prev.has_dst() && prev.dst_reg() == src2_reg &&
                                        prev.op_code() == IROpCode::LdcI4) {
                                        prev.imm.i4 = static_cast<int32_t>(shift);
                                        break;
                                    }
                                }
                                did_opt = true;
                                continue;
                            } else {
                                // RemUn by 2^k → And with (2^k - 1)
                                uint32_t mask = (1u << shift) - 1u;
                                ri.header = MakeHdr(IROpCode::And, ri.dst_reg(), ri.src1_reg(), src2_reg,
                                                    ::chaos::il2cpp::interpreter::kRegHasDst |
                                                        ::chaos::il2cpp::interpreter::kRegHasSrc1 |
                                                        ::chaos::il2cpp::interpreter::kRegHasSrc2);
                                for (uint32_t j = i; j > 0; --j) {
                                    auto& prev = instrs[j - 1];
                                    if (prev.has_dst() && prev.dst_reg() == src2_reg &&
                                        prev.op_code() == IROpCode::LdcI4) {
                                        prev.imm.i4 = static_cast<int32_t>(mask);
                                        break;
                                    }
                                }
                                did_opt = true;
                                continue;
                            }
                        }
                    }
                }
            }
        }

        // (k) Ceq/Clt/Cgt self-comparison → constant
        if (ri.has_dst() && ri.has_src1() && ri.has_src2() && ri.src1_reg() == ri.src2_reg()) {
            if (opc == IROpCode::Ceq) {
                // x == x → 1
                ri.header =
                    MakeHdr(IROpCode::LdcI4, ri.dst_reg(), 0, 0,
                            ::chaos::il2cpp::interpreter::kRegHasDst | ::chaos::il2cpp::interpreter::kRegHasImm);
                ri.imm.i4 = 1;
                did_opt = true;
                continue;
            }
            if (opc == IROpCode::Clt || opc == IROpCode::Cgt) {
                // x < x → 0,  x > x → 0
                ri.header =
                    MakeHdr(IROpCode::LdcI4, ri.dst_reg(), 0, 0,
                            ::chaos::il2cpp::interpreter::kRegHasDst | ::chaos::il2cpp::interpreter::kRegHasImm);
                ri.imm.i4 = 0;
                did_opt = true;
                continue;
            }
        }

        // (l) Branch-to-switch conversion: consecutive Beq (same src1 vs different LdcI4) → Switch
        if (opc == IROpCode::Beq && ri.has_src1() && ri.has_src2() && !removed_mask[i]) {
            uint8_t switch_reg = ri.src1_reg();
            uint32_t max_cases = (n - i) / 2; // each case = Beq (+ optional LdcI4)
            if (max_cases > 64)
                max_cases = 64;

            // Collect cases: (value_k, target_k) pairs
            struct BeqCase {
                int32_t value;
                uint32_t target;
                uint32_t beq_idx;
            };
            BeqCase cases[64];
            uint32_t case_count = 0;
            bool valid_chain = true;
            uint32_t default_target = n; // fall-through

            for (uint32_t j = i; j < n && case_count < max_cases;) {
                if (j > i) {
                    // Skip any LdcI4 that defines the Beq's src2
                    if (instrs[j].op_code() == IROpCode::LdcI4 && instrs[j].has_dst() && !removed_mask[j]) {
                        ++j;
                        continue;
                    }
                }
                auto& cur = instrs[j];
                if (cur.op_code() != IROpCode::Beq || !cur.has_src1() || !cur.has_src2() ||
                    cur.src1_reg() != switch_reg || removed_mask[j]) {
                    // Check for trailing Br (default target)
                    if (cur.op_code() == IROpCode::Br && cur.has_imm() && case_count >= 3) {
                        default_target = cur.imm.branch_target;
                        removed_mask[j] = 1; // remove Br, Switch handles default
                        did_opt = true;
                    }
                    if (case_count < 3)
                        valid_chain = false;
                    break;
                }
                // Find the LdcI4 defining this Beq's src2 (scan backward from j)
                uint8_t src2 = cur.src2_reg();
                const int32_t* val = FindDefLdcI4(j, src2);
                if (val == nullptr) {
                    valid_chain = false;
                    break;
                }
                cases[case_count].value = *val;
                cases[case_count].target = cur.imm.branch_target;
                cases[case_count].beq_idx = j;
                ++case_count;
                ++j;
            }

            if (valid_chain && case_count >= 3) {
                // Build targets array: [case_0, case_1, ..., case_n-1, default]
                static uint32_t s_switch_targets[256];
                uint32_t tc = case_count;
                for (uint32_t c = 0; c < tc; ++c) {
                    s_switch_targets[c] = cases[c].target;
                }
                s_switch_targets[tc] = default_target;

                // Replace first Beq with Switch
                uint64_t switch_header =
                    static_cast<uint64_t>(IROpCode::Switch) | (static_cast<uint64_t>(0) << 16) |   // no dst
                    (static_cast<uint64_t>(switch_reg) << 24) | (static_cast<uint64_t>(0) << 32) | // no src2
                    (static_cast<uint64_t>(::chaos::il2cpp::interpreter::kRegHasSrc1 |
                                           ::chaos::il2cpp::interpreter::kRegHasImm |
                                           ::chaos::il2cpp::interpreter::kRegIsBranch)
                     << 40) |
                    (static_cast<uint64_t>(tc & 0x7FFF) << 48);
                ri.header = switch_header;
                ri.imm.ptr = s_switch_targets;

                // Mark subsequent Beqs as removed
                for (uint32_t c = 1; c < tc; ++c) {
                    removed_mask[cases[c].beq_idx] = 1;
                }
                did_opt = true;
                continue;
            }
        }
    }

    if (!did_opt)
        removed_mask.clear();
}

JitMethod* NativeCodeGenerator::Generate() noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::Generate");
    uint32_t n_instrs = static_cast<uint32_t>(rm_.instructions.size());
    if (n_instrs == 0)
        return nullptr;
    instr_offsets_.resize(n_instrs, 0);

    // Cache the diagnostics gate once.  When off (production), every accessor
    // sees the same false bool → a single never-taken branch, no env lookup.
    collect_stats_ = CodegenStatsEnabled();
    if (collect_stats_)
        current_opc_ = 0;

    // Initialize type inference state for all GPR vregs
    vreg_types_.assign(kGprCount, kTypeVoid);

    // Scan for LocAlloc: if used, enlarge frame to include stack reserve.
    // This avoids heap allocation (no leak, freed on method return).
    localloc_extra_ = 0;
    for (const auto& instr : rm_.instructions) {
        if (instr.op_code() == IROpCode::LocAlloc) {
            localloc_extra_ = kLocAllocBumpAndReserve;
            break;
        }
    }

    // Pre-scan: count managed call instructions to reserve slot table entries.
    // slot_count_ = number of Call/CallBridge instructions (not Calli, not CallVirt).
    slot_count_ = 0;
    // T2.1 A1: determine whether any instruction can clobber caller-saved
    // registers (and thus require the caller-colored write-through in
    // StoreGpr/StoreFpr).  A managed/runtime helper call, an enabled safepoint
    // poll, an overflow-checked op (deopt), or ANY object/heap/memory op
    // (allocation, field/element access, box/cast, write barrier — all of
    // which either emit a runtime helper call, can trigger a GC that scans the
    // frame, or write through a write barrier) can clobber/require caller-saved
    // values on the stack.  Only a strictly scalar, call-free method (pure
    // arithmetic/bitwise/shift/compare/convert on Int32/Int64/Float) is safe
    // to keep purely register-resident.
    bool can_clobber_caller = config_.enable_safepoint_polls && config_.safepoint_fn != nullptr;
    for (const auto& instr : rm_.instructions) {
        auto opc = instr.op_code();
        if (opc == IROpCode::Call || opc == IROpCode::CallBridge || opc == IROpCode::CallVirt ||
            opc == IROpCode::Calli) {
            slot_count_ += (opc == IROpCode::Call || opc == IROpCode::CallBridge);
            can_clobber_caller = true;
            continue;
        }
        // Whitelist of ops that are pure scalar (Int/FP) register-resident
        // safe: no heap, no GC, no call, no overflow-deopt, no memory RMW.
        switch (opc) {
            case IROpCode::LdcI4:
            case IROpCode::LdcI8:
            case IROpCode::LdcR4:
            case IROpCode::LdcR8:
            case IROpCode::Add:
            case IROpCode::Sub:
            case IROpCode::Mul:
            case IROpCode::Div:
            case IROpCode::Rem:
            case IROpCode::DivUn:
            case IROpCode::RemUn:
            case IROpCode::Neg:
            case IROpCode::Not:
            case IROpCode::And:
            case IROpCode::Or:
            case IROpCode::Xor:
            case IROpCode::Shl:
            case IROpCode::Shr:
            case IROpCode::ShrUn:
            case IROpCode::Ceq:
            case IROpCode::Clt:
            case IROpCode::Cgt:
            case IROpCode::Conv_I4:
            case IROpCode::Conv_I8:
            case IROpCode::ConvI:
            case IROpCode::ConvU:
            case IROpCode::Conv_R4:
            case IROpCode::Conv_R8:
            case IROpCode::ConvRUn:
            case IROpCode::Abs:
            case IROpCode::Min:
            case IROpCode::Max:
            case IROpCode::Dup:
            case IROpCode::Pop:
            case IROpCode::Ret:
                break; // safe; keep resident
            default:
                can_clobber_caller = true; // object/heap/call/memory/br — not resident-safe
                break;
        }
    }
    has_caller_clobber_ = can_clobber_caller;
    slot_patches_.reserve(slot_count_);
    slot_count_used_ = 0;

    // Initialize cached TLS info for inline TLAB access
    InitTlsTlabInfo();


    // ── Register allocation: Tier 0 skips entirely (stack-only) ───────────
    if (is_tier0_) {
        num_cache_regs_ = 0;
        has_graph_coloring_ = false;
        num_fpr_callee_ = 0;
        xmm_save_size_ = 0;
    } else if (config_.enable_register_caching) {
        gcr_ = AllocateRegistersGraphColoring(rm_);
        has_graph_coloring_ = false;
        std::memset(phys_to_colored_vreg_, 0xFF, sizeof(phys_to_colored_vreg_));
        for (uint32_t vr = 0; vr < kGprCount; ++vr) {
            uint8_t x64r = gcr_.gpr_color[vr];
            if (x64r != 0xFF) {
                has_graph_coloring_ = true;
                phys_to_colored_vreg_[x64r] = static_cast<uint8_t>(vr);
            }
        }
        // has_graph_coloring_ was set true above only if a GPR got a color.
        // A pure-float method has all-GPR-spilled but valid FPR colors, which
        // still need the filter + prologue/epilogue machinery and must set the
        // accessor gate.  Re-derive from ANY surviving color now.
        for (uint32_t fc2 = 0; fc2 < kFprCount; ++fc2) {
            if (gcr_.fpr_color[fc2] != 0xFF) {
                has_graph_coloring_ = true;
                break;
            }
        }
        if (has_graph_coloring_) {
            // Print final coloring after filter for diagnostics
            std::printf("    [GC] %u GPR callee colors (%u instrs):", num_cache_regs_, n_instrs);
            for (uint32_t dv = 0; dv < kGprCount; ++dv) {
                if (gcr_.gpr_color[dv] != 0xFF)
                    std::printf(" r%u→x%u", dv, gcr_.gpr_color[dv]);
            }
            std::printf(" | caller_mask=0x%llx filtered_mask=0x%llx\n", (unsigned long long)caller_colored_mask_,
                        (unsigned long long)filtered_vreg_mask_);

            // V3: keep all colored registers, build caller_colored_mask_ for
            // caller-saved regs.  EmitCallWithSpill reloads these after each
            // runtime helper call.  Only callee-saved regs are saved/restored
            // in prologue/epilogue (PUSH/POP on x64, STP/LDP on ARM64).
            bool seen[32] = {};
            num_cache_regs_ = 0;
            filtered_vreg_mask_ = 0;
            caller_colored_mask_ = 0;
            caller_fpr_colored_mask_ = 0;
            for (uint32_t vr = 0; vr < kGprCount; ++vr) {
                uint8_t x64r = gcr_.gpr_color[vr];
                if (x64r != 0xFF) {
#if defined(__aarch64__)
                    // ARM64 callee-saved: X19-X28
                    bool is_callee = (x64r >= 19 && x64r <= 28);
#else
                    // x64 callee-saved: R12-R15 for the coloring pool.  RDI is
                    // ABI-callee-saved on Win64 but is operationally RESERVED as
                    // the frame-init scratch (REP STOSQ in the prologue), so it
                    // must remain caller-colored to avoid clobbering vreg values.
                    bool is_callee = (x64r >= 12 && x64r <= 15);
#endif
                    if (is_callee) {
                        if (!seen[x64r] && num_cache_regs_ < kMaxCacheRegs) {
                            seen[x64r] = true;
                            callee_gpr_regs_[num_cache_regs_++] = x64r;
                        } else {
                            // Another vreg already claimed this callee-saved
                            // register.  The caller-filter already handles
                            // caller-saved duplicates, but graph coloring
                            // can still assign interfering vregs to the same
                            // callee-saved register (select-phase ordering:
                            // neighbor not yet colored when this vreg is
                            // selected).  Clear the color → stack spill.
                            gcr_.gpr_color[vr] = 0xFF;
                        }
                    } else {
                        // Caller-saved register.
                        // Check for duplicate physical register (same issue as
                        // callee-saved): the post-call reload iterates through
                        // caller_colored_mask_ and reloads each vreg from its
                        // stack slot — when two vregs share the same register,
                        // the second reload overwrites the first, corrupting
                        // the value for subsequent instructions that read it.
                        if (!seen[x64r]) {
                            seen[x64r] = true;
                            caller_colored_mask_ |= (1ULL << vr);
                        } else {
                            gcr_.gpr_color[vr] = 0xFF; // duplicate → stack spill
                        }
                    }
                }
            }
            cached_slots_used_ = 0;
            cached_dirty_mask_ = 0;
            // Clear V1 cache mappings so spilled vregs fall through to stack
            std::memset(cached_x64_for_vreg_, 0xFF, sizeof(cached_x64_for_vreg_));
            callee_saved_regs_ = callee_gpr_regs_;

            // ── FPR callee-saved filter ────────────────────────────────
            // x64 Win64: XMM6-XMM15 are callee-saved.
            // ARM64: V8-V15 are callee-saved.
            num_fpr_callee_ = 0;
            bool xmm_seen[16] = {};
            for (uint32_t fi = 0; fi < 32; ++fi) {
                uint8_t xmm = gcr_.fpr_color[fi];
                if (xmm != 0xFF) {
#if defined(__aarch64__)
                    // ARM64 NEON: V8-V15 callee-saved
                    bool fpr_callee = (xmm >= 8 && xmm <= 15);
#else
                    // x64: XMM6-XMM15 callee-saved
                    bool fpr_callee = (xmm >= 6 && xmm <= 15);
#endif
                    if (fpr_callee) {
                        if (!xmm_seen[xmm] && num_fpr_callee_ < 10) {
                            xmm_seen[xmm] = true;
                            callee_xmm_regs_[num_fpr_callee_] = xmm;
                            callee_xmm_fi_[num_fpr_callee_] = static_cast<uint8_t>(fi);
                            ++num_fpr_callee_;
                        } else {
                            gcr_.fpr_color[fi] = 0xFF; // already claimed → spill
                        }
                    } else {
                        // Caller-saved FPR: keep the color assignment.
                        // EmitCallWithSpill will reload from stack after each call.
                        uint32_t fpr_vreg = fi;
                        caller_fpr_colored_mask_ |= (1ULL << fpr_vreg);
                    }
                }
            }
            xmm_save_size_ = static_cast<int32_t>(num_fpr_callee_) * 16;
        } else {
            SelectCacheableRegs(); // V1 fallback
        }
    } else {
        SelectCacheableRegs();
    }
#if defined(__aarch64__)
    // ARM64: STP is always 16 bytes — frame_align_adj_ not needed.
    frame_align_adj_ = 0;
#else
    // x64: after CALL (RSP -= 8), each PUSH/N pushes by 8.
    // Alignment: RSP_pre_call - 8 - 8*num_push_regs ≡ 0 (mod 16)
    // → num_push_regs must be ODD for alignment.
    // With 4 base pushes (rbp, rbx, rsi, rdi), num_push_regs = 4 + num_cache_regs_.
    // So pad 8 when num_push_regs is EVEN → pad when num_cache_regs_ is even.
    frame_align_adj_ = ((num_cache_regs_ + 4) % 2) * 8;
#endif

    // Prologue — push/STP callee-saved regs, establish frame pointer
#if defined(__aarch64__)
    // ARM64 prologue: STP X29, X30 (FP+LR), MOV X29, SP, save ABI ret_buf
    // (X1) below FP/LR, then cacheable regs, then SUB SP, #N.
    prologue_push_offsets_[0] = buf_.pos();
    EmitStp64Pre(buf_, AT::kFrameReg, 30, kARM64_SP, -16); // STP X29, X30, [SP, #-16]!
    prologue_set_fpreg_offset_ = buf_.pos();
    enc_.EmitAddRI(AT::kFrameReg, AT::kStackReg, 0); // MOV X29, SP
    // Save ABI ret_buf (X1) at [X29-16] so Ret handler can write return value
    // through it (matching x64 saved-RSI approach).  X29 stays at the saved
    // X29/X30 pair (from the STP above); X1 sits one slot below at [X29-16].
    EmitStp64Pre(buf_, 1, 0, kARM64_SP, -16); // STP X1, X0, [SP, #-16]! (X0=scratch, never XZR)
    // Save additional callee-saved regs used for register caching (Phase 3+)
    for (uint32_t slot = 0; slot < num_cache_regs_; ++slot) {
        prologue_push_offsets_[4 + slot] = buf_.pos();
        EmitStp64Pre(buf_, callee_saved_regs_[slot], 0, kARM64_SP,
                     -16); // STP Xreg, X0, [SP, #-16]! (X0=scratch, never XZR)
    }
    prologue_sub_rsp_offset_ = buf_.pos();
    prologue_sub_rsp_size_ =
        static_cast<uint32_t>(kFrameSize + frame_size_extra_ + frame_align_adj_ + xmm_save_size_ + localloc_extra_);
    enc_.EmitSubRI(AT::kStackReg, static_cast<int32_t>(prologue_sub_rsp_size_));
    enc_.EmitAddRI(AT::kArgsBuf, AT::kStackReg, static_cast<int32_t>(kGprFileOff)); // X3 = SP + GPR file
    enc_.EmitAddRI(AT::kRetBuf, AT::kStackReg, static_cast<int32_t>(kGprFileOff));  // X4 = SP + GPR file
    prologue_total_bytes_ = buf_.pos() - prologue_push_offsets_[0];
    // Build push register list (ARM64 unwind — Phase 4)
    push_reg_nums_[0] = AT::kFrameReg;
    for (uint32_t slot = 0; slot < num_cache_regs_; ++slot)
        push_reg_nums_[1 + slot] = callee_saved_regs_[slot];
    num_push_regs_ = 1 + num_cache_regs_;
#else
    prologue_push_offsets_[0] = buf_.pos();
    enc_.EmitPush(AT::kFrameReg);
    prologue_set_fpreg_offset_ = buf_.pos();
    enc_.EmitAddRI(AT::kFrameReg, AT::kStackReg, 0); // frame pointer chain for GC stack walking
    prologue_push_offsets_[1] = buf_.pos();
    enc_.EmitPush(AT::kArgsBuf);
    prologue_push_offsets_[2] = buf_.pos();
    enc_.EmitPush(AT::kRetBuf);
    prologue_push_offsets_[3] = buf_.pos();
    enc_.EmitPush(kRDI); // x64 ABI: RDI is callee-saved; used by REP STOSQ below
    // Push additional callee-saved regs used for register caching
    for (uint32_t slot = 0; slot < num_cache_regs_; ++slot)
        prologue_push_offsets_[4 + slot] = buf_.pos(), enc_.EmitPush(callee_saved_regs_[slot]);
    prologue_sub_rsp_offset_ = buf_.pos();
    prologue_sub_rsp_size_ =
        static_cast<uint32_t>(kFrameSize + frame_size_extra_ + frame_align_adj_ + xmm_save_size_ + localloc_extra_);
    enc_.EmitSubRI(AT::kStackReg, static_cast<int32_t>(prologue_sub_rsp_size_));
    // Set up register convention: RBX = args_buf, RSI = ret_buf.
    // Both point to the GPR file in the local frame (not from caller RCX/RDX),
    // since JIT mode calls these functions as void() with no arguments.
    enc_.EmitLeaRM(AT::kArgsBuf, AT::kStackReg, static_cast<int32_t>(kGprFileOff));
    enc_.EmitLeaRM(AT::kRetBuf, AT::kStackReg, static_cast<int32_t>(kGprFileOff));
    prologue_total_bytes_ = buf_.pos() - prologue_push_offsets_[0];
    // Build push register list for unwind info: rbp, rbx, rsi, rdi, cached regs
    push_reg_nums_[0] = AT::kFrameReg;
    push_reg_nums_[1] = AT::kArgsBuf;
    push_reg_nums_[2] = AT::kRetBuf;
    push_reg_nums_[3] = kRDI;
    for (uint32_t slot = 0; slot < num_cache_regs_; ++slot)
        push_reg_nums_[4 + slot] = callee_saved_regs_[slot];
    num_push_regs_ = 4 + num_cache_regs_;
#endif

    // Save callee-saved XMM registers (used by graph coloring)
    // Stored in the area just below the regular frame (at RSP + kFrameSize + frame_size_extra_ + frame_align_adj_).
    for (uint32_t si = 0; si < num_fpr_callee_; ++si) {
        int32_t off = static_cast<int32_t>(kFrameSize + frame_size_extra_ + frame_align_adj_ + si * 16);
        enc_.EmitMovUPSMR(AT::kStackReg, off, callee_xmm_regs_[si]);
    }

    // Zero-initialize all GPR stack slots — reading an uninitialized slot
    // produces garbage.  Done BEFORE colored-reg zeroing to use RDI as scratch.
    enc_.EmitXorZR(AT::kScratchA);
#if defined(__aarch64__)
    // ARM64: zero GPR stack slots using STP XZR, XZR pairs (halves instruction
    // count vs. STR-per-slot).  Each STP zeroes 2 × 8 = 16 bytes.
    // kGprFileOff is always 0, so pairs cover [0, kGprCount * 8) in stride 16.
    // Register 31 = XZR (zero register) for STP source; kARM64_SP = 31 = SP base.
    for (uint32_t i = 0; i < kGprCount; i += 2) {
        EmitStp64(buf_, 31, 31, kARM64_SP, static_cast<int32_t>((kGprFileOff + i * 8) / 16));
    }
#else
    enc_.EmitLeaRM(kRDI, AT::kStackReg, static_cast<int32_t>(kGprFileOff));
    enc_.EmitMovRIImm32(AT::kScratchB, kGprCount);
    buf_.EmitByte(0xF3);
    buf_.EmitByte(0x48);
    buf_.EmitByte(0xAB);
#endif

    // Zero the localloc bump pointer (RAX is still 0 from xor above)
    if (localloc_extra_ > 0) {
        enc_.EmitMovMR(AT::kStackReg,
                       static_cast<int32_t>(kFrameSize + frame_size_extra_ + frame_align_adj_ + xmm_save_size_),
                       AT::kScratchA);
    }

    // Initialize colored callee-saved regs to 0 (re-zeros RDI if colored,
    // since REP STOSQ above left RDI pointing past the GPR file).
    if (has_graph_coloring_) {
        for (uint32_t slot = 0; slot < num_cache_regs_; ++slot)
            enc_.EmitXorRR(callee_saved_regs_[slot], callee_saved_regs_[slot]);
    }

    // Zero caller-colored registers — LoadGpr reads them directly
    // for vregs colored to caller-saved regs, bypassing the zeroed stack.
    // Without this, a read-before-write vreg in a caller-saved reg returns
    // garbage from the caller, while RegisterExecute returns 0.
    if (caller_colored_mask_) {
#if defined(__aarch64__)
        // ARM64: zero all caller-saved GPRs in the coloring pool (X5-X17)
        for (uint32_t x64r = 5; x64r <= 17; ++x64r)
            enc_.EmitXorRR(x64r, x64r);
#else
        // x64: zero R8-R11
        for (uint32_t x64r = AT::kExtraScratch0; x64r <= AT::kExtraScratch2; ++x64r)
            enc_.EmitXorRR(x64r, x64r);
#endif
    }

    // ── GC mode switch: EnterCooperativeMode ──────────────────────────
    // Switch the thread to cooperative GC mode before any instruction
    // that allocates or accesses the managed heap.  This must happen
    // after the frame is set up (stack walking works) but before the
    // first managed object access.
    if (config_.cooperative_fn != nullptr) {
#if !defined(__aarch64__)
        enc_.EmitSubRI(AT::kStackReg, 32); // shadow space for Win64 ABI
#endif
        uint32_t call_start = buf_.pos();
        enc_.EmitCallRipRel(0);
        slot_patches_.push_back(
            {call_start + kSlotPatchDispOff, UINT32_MAX, reinterpret_cast<void*>(config_.cooperative_fn)});
        slot_count_used_++;
        uint32_t call_pos = call_start;
#if !defined(__aarch64__)
        enc_.EmitAddRI(AT::kStackReg, 32);
#endif
        // Record as GC point so stack is walkable during mode switch
        call_sites_.push_back({UINT32_MAX, call_pos});
        RecordGcPoint(call_pos);
    }

    // Bail out early if any emit above hit an OOM
    if (CheckFailed())
        return nullptr;

    // ── Optimize instructions (tree IR pipeline + linear fallback) ──────
    // Creates a mutable copy of rm_.instructions for the optimizer.
    // Non-SEH methods: tree IR pipeline (Inliner → ConstFold → CSE → linearize).
    // SEH methods: existing linear OptimizeInstructions fallback.
    auto opt_instrs = rm_.instructions;
    std::vector<uint8_t> removed_mask;
    InlineResultBuffer inline_results;
    if (!is_tier0_ && config_.enable_optimizer) {
        if (!rm_.seh_clauses.empty()) {
            // SEH methods: use the existing linear optimizer
            OptimizeInstructions(opt_instrs, removed_mask, true);
        } else {
            // Non-SEH methods: tree IR pipeline with optional inlining.
            // Produces a clean instruction sequence (no removed_mask needed).
            std::vector<interpreter::RegisterInstruction> tree_opt;
            if (tree::OptimizeWithTreeIR(opt_instrs, tree_opt, false, rm_.max_regs, config_.enable_inlining,
                                         &inline_results)) {
                opt_instrs = std::move(tree_opt);
                n_instrs = static_cast<uint32_t>(opt_instrs.size());

                // Resize vreg_types_ to cover tree-optimizer-created vregs
                // (Linearizer starts at tree::Linearizer::kBaseVReg = 64, which
                //  is >= kGprCount, so the original vreg_types_[kGprCount] sizing
                //  would silently discard type info for these new vregs).
                {
                    uint32_t opt_max = kGprCount;
                    for (const auto& ri : opt_instrs) {
                        if (ri.has_dst() && ri.dst_reg() > opt_max)
                            opt_max = ri.dst_reg();
                        if (ri.has_src1() && ri.src1_reg() > opt_max)
                            opt_max = ri.src1_reg();
                        if (ri.has_src2() && ri.src2_reg() > opt_max)
                            opt_max = ri.src2_reg();
                    }
                    opt_max = std::max(opt_max + 1, static_cast<uint32_t>(kGprCount));
                    if (opt_max > vreg_types_.size())
                        vreg_types_.resize(opt_max, kTypeVoid);
                }

                // Extend the stack frame if tree-created vregs exceed kFrameSize.
                // Without this, StoreGpr(vreg>=136) writes past the frame boundary,
                // overwriting the saved ABI ret_buf pointer at [SP+kFrameSize].
                {
                    uint32_t max_vreg = kGprCount - 1;
                    for (const auto& ri : opt_instrs) {
                        if (ri.has_dst() && ri.dst_reg() > max_vreg)
                            max_vreg = ri.dst_reg();
                        if (ri.has_src1() && ri.src1_reg() > max_vreg)
                            max_vreg = ri.src1_reg();
                        if (ri.has_src2() && ri.src2_reg() > max_vreg)
                            max_vreg = ri.src2_reg();
                    }
                    uint32_t gpr_bytes = static_cast<uint32_t>(max_vreg + 1) * 8;
                    if (gpr_bytes > kFrameSize)
                        frame_size_extra_ = gpr_bytes - kFrameSize;
                }

                // Re-count call slots: inlining may have removed call instructions.
                // Save prologue/runtime helper slot patches that were added before
                // the optimizer (e.g. EnterCooperativeMode call) — they'll be
                // re-appended after the clear.
                auto saved_prologue_patches = std::move(slot_patches_);
                slot_count_ = 0;
                for (const auto& instr : opt_instrs) {
                    auto opc = instr.op_code();
                    if (opc == IROpCode::Call || opc == IROpCode::CallBridge) {
                        slot_count_++;
                    }
                }
                slot_patches_.clear();
                slot_patches_.reserve(slot_count_ + saved_prologue_patches.size());
                // Re-add prologue/runtime helper patches (cooperative_fn, etc.)
                for (auto& sp : saved_prologue_patches) {
                    slot_patches_.push_back(std::move(sp));
                }
                slot_count_used_ = static_cast<uint32_t>(saved_prologue_patches.size());
            } else {
                // Fallback: linear optimizer (tree IR build failed or empty BBs)
                OptimizeInstructions(opt_instrs, removed_mask, false);
            }
        }
    }

    // ── Liveness analysis for precise GC slot maps ─────────────────────────
    // Computes per-instruction live-in bitmasks used by RecordGcPoint() to
    // report only ObjectRef vregs that are both typed-as-ref AND live at the
    // current instruction point.  Uses the optimized instruction stream so
    // liveness reflects the actual emitted code.
    // Skipped entirely in Tier 0 — GC uses conservative scanning.
    if (!is_tier0_ && config_.enable_liveness && n_instrs > 0) {
        live_in_.assign(n_instrs, 0);
        std::vector<uint64_t> live_out(n_instrs, 0);
        std::vector<uint64_t> def(n_instrs, 0);
        std::vector<uint64_t> use(n_instrs, 0);

        // Pass 1: Compute def and use for each instruction
        for (uint32_t i = 0; i < n_instrs; ++i) {
            const auto& inst = opt_instrs[i];
            if (inst.has_dst() && inst.dst_reg() < kGprCount)
                def[i] |= (1ULL << inst.dst_reg());
            if (inst.has_src1() && inst.src1_reg() < kGprCount)
                use[i] |= (1ULL << inst.src1_reg());
            if (inst.has_src2() && inst.src2_reg() < kGprCount)
                use[i] |= (1ULL << inst.src2_reg());
            if (inst.flags() & ::chaos::il2cpp::interpreter::kRegHasSrc3) {
                uint8_t src3 = inst.src3_reg();
                if (src3 < kGprCount)
                    use[i] |= (1ULL << src3);
            }
            // Calli: func_ptr vreg in imm.operand_index is an implicit source
            if (inst.op_code() == IROpCode::Calli && inst.imm.operand_index < kGprCount) {
                use[i] |= (1ULL << inst.imm.operand_index);
            }
        }

        // Pass 2: Iterative backward dataflow to fixed point
        bool changed = true;
        while (changed) {
            changed = false;
            for (int32_t i = static_cast<int32_t>(n_instrs) - 1; i >= 0; --i) {
                const auto& inst = opt_instrs[i];
                uint64_t new_live_out = 0;
                auto opc = inst.op_code();

                if (opc == IROpCode::Switch) {
                    uint32_t n_targets = inst.imm.i4;
                    if (n_targets > 0 && inst.imm.ptr) {
                        const uint32_t* targets = static_cast<const uint32_t*>(inst.imm.ptr);
                        uint32_t limit = n_targets < 256 ? n_targets : 256;
                        for (uint32_t ti = 0; ti < limit; ++ti) {
                            if (targets[ti] < n_instrs)
                                new_live_out |= live_in_[targets[ti]];
                        }
                    }
                } else if (opc == IROpCode::Br || opc == IROpCode::Leave) {
                    uint32_t target = inst.imm.branch_target;
                    if (target < n_instrs)
                        new_live_out |= live_in_[target];
                } else if (opc == IROpCode::BrTrue || opc == IROpCode::BrFalse || opc == IROpCode::Beq ||
                           opc == IROpCode::BneUn || opc == IROpCode::Blt || opc == IROpCode::Bgt ||
                           opc == IROpCode::Ble || opc == IROpCode::Bge || opc == IROpCode::BltUn ||
                           opc == IROpCode::BgtUn || opc == IROpCode::BleUn || opc == IROpCode::BgeUn) {
                    uint32_t target = inst.imm.branch_target;
                    if (target < n_instrs)
                        new_live_out |= live_in_[target];
                    if (static_cast<uint32_t>(i) + 1 < n_instrs)
                        new_live_out |= live_in_[i + 1];
                } else if (opc == IROpCode::Ret || opc == IROpCode::Throw || opc == IROpCode::Rethrow ||
                           opc == IROpCode::EndFinally || opc == IROpCode::EndFilter) {
                    // Terminator: no successors
                } else {
                    if (static_cast<uint32_t>(i) + 1 < n_instrs)
                        new_live_out |= live_in_[i + 1];
                }

                uint64_t new_live_in = use[i] | (new_live_out & ~def[i]);

                if (new_live_in != live_in_[i] || new_live_out != live_out[i]) {
                    live_in_[i] = new_live_in;
                    live_out[i] = new_live_out;
                    changed = true;
                }
            }
        }

        // Activate liveness filtering in RecordGcPoint
        use_liveness_ = true;
        liveness_computed_ = true;
        CHAOS_IL2CPP_LOG_DEBUG_M("codegen", "Liveness computed for %u instructions, use_liveness=%d", n_instrs,
                                 (int)use_liveness_);
    }

    // ── cross_call_mask_: vregs live across any call / safepoint ──────────
    // A caller-colored vreg only needs its stack write-through (and the post-call
    // reload) if its live range crosses a call/safepoint.  A call/safepoint is any
    // managed/runtime call (Call/CallBridge/CallVirt/Calli) or an allocation/poll
    // op that can trigger a GC or clobber caller-saved regs (NewObj/Box/NewArr/
    // LdFld/StFld/LdSFld/StSFld/StElem/StObj/Cpblk/CastClass/IsInst/Throw/Rethrow
    // — all either emit a runtime helper call or can hit a safepoint).  Values
    // live-before such an instruction are live across it.  This is the per-vreg
    // refinement of the coarse method-wide has_caller_clobber_.
    cross_call_mask_ = 0;
    if (liveness_computed_) {
        for (uint32_t i = 0; i < n_instrs && i < live_in_.size(); ++i) {
            const auto& inst = opt_instrs[i];
            auto opc = inst.op_code();
            const bool is_branch = (opc == IROpCode::Br || opc == IROpCode::BrTrue || opc == IROpCode::BrFalse ||
                                    opc == IROpCode::Beq || opc == IROpCode::BneUn || opc == IROpCode::Blt ||
                                    opc == IROpCode::BltUn || opc == IROpCode::Bgt || opc == IROpCode::BgtUn ||
                                    opc == IROpCode::Ble || opc == IROpCode::BleUn || opc == IROpCode::Bge ||
                                    opc == IROpCode::BgeUn || opc == IROpCode::Switch || opc == IROpCode::Leave);
            const bool is_callish = (opc == IROpCode::Call || opc == IROpCode::CallBridge ||
                                     opc == IROpCode::CallVirt || opc == IROpCode::Calli ||
                                     opc == IROpCode::NewObj || opc == IROpCode::NewArr ||
                                     opc == IROpCode::Box || opc == IROpCode::Unbox ||
                                     opc == IROpCode::LdFld || opc == IROpCode::StFld ||
                                     opc == IROpCode::LdSFld || opc == IROpCode::StSFld ||
                                     opc == IROpCode::LdElem || opc == IROpCode::StElem ||
                                     opc == IROpCode::LdElemA || opc == IROpCode::StObj ||
                                     opc == IROpCode::CastClass || opc == IROpCode::IsInst ||
                                     opc == IROpCode::Throw || opc == IROpCode::Rethrow ||
                                     // Overflow-checked ops: their deopt path
                                     // reconstructs the register file from vreg
                                     // stack slots, so live vregs must be current.
                                     opc == IROpCode::AddOvf || opc == IROpCode::SubOvf ||
                                     opc == IROpCode::MulOvf || opc == IROpCode::ConvOvfI ||
                                     opc == IROpCode::ConvOvfI4 || opc == IROpCode::ConvOvfI8 ||
                                     opc == IROpCode::ConvOvfU || opc == IROpCode::ConvOvfU4 ||
                                     opc == IROpCode::ConvOvfU8);
            // Branches host safepoint polls (EmitSafepointPoll at Br/BrTrue/
            // BrFalse), so a GC can scan the frame there too.
            if (is_callish || (is_branch && config_.enable_safepoint_polls))
                cross_call_mask_ |= live_in_[i];
        }
    }
    // Fallback (liveness not computed, e.g. Tier 0): keep the coarse gate so
    // correctness is preserved for any caller-colored GC-ref the GC may scan.
    if (!liveness_computed_)
        cross_call_mask_ = ~0ULL;

    // Pre-allocate JitMethod early so instruction emission can embed
    // the address of its stale flag for HotUpdate inline PIC checking.
    auto* nm = static_cast<JitMethod*>(CHAOS_IL2CPP_MALLOC(sizeof(JitMethod)));
    if (nm == nullptr)
        return nullptr;
    std::memset(nm, 0, sizeof(*nm));
    ::new (nm) JitMethod();
    stale_flag_ptr_ = &nm->stale;

    // Emit instructions
    for (uint32_t i = 0; i < n_instrs; ++i) {
        instr_offsets_[i] = buf_.pos();
        current_instr_index_ = i;
        const auto& instr = opt_instrs[i];
        if (!removed_mask.empty() && removed_mask[i])
            continue;
        if (!EmitInstruction(instr)) {
            CHAOS_IL2CPP_LOG_DEBUG_M("codegen", "Compile: unsupported opcode {} at pc={}, emitting deopt",
                                     static_cast<int>(instr.op_code()), i);
            if (!is_tier0_ && config_.enable_deopt) {
                EmitDeoptSequence(i);
                continue;
            }
            return nullptr;
        }
        PropagateTypes(instr);
    }

    // Bail out if any instruction emit failed (OOM).
    if (CheckFailed())
        return nullptr;

    // deopt_return: shared deoptimization return point + stack frame epilogue.
    // All deopt paths (unsupported opcode, throw/rethrow, CallVirt PIC miss)
    // jump here.  The epilogue restores the stack frame and returns so that
    // InterpreterEntryDirect can check t_deopt_state.deopt_happened.
    deopt_return_pos_ = buf_.pos();

    // ── GC mode switch: EnterPreemptiveMode ───────────────────────────
    // Switch to preemptive mode before returning to native code.
    // In preemptive mode, safepoint requests don't spin — the thread
    // acknowledges and returns immediately, so the GC won't wait for it.
    if (config_.preemptive_fn != nullptr) {
#if !defined(__aarch64__)
        enc_.EmitSubRI(AT::kStackReg, 32); // shadow space for Win64 ABI
#endif
        uint32_t call_start = buf_.pos();
        enc_.EmitCallRipRel(0);
        slot_patches_.push_back(
            {call_start + kSlotPatchDispOff, UINT32_MAX, reinterpret_cast<void*>(config_.preemptive_fn)});
        slot_count_used_++;
        uint32_t call_pos = call_start;
#if !defined(__aarch64__)
        enc_.EmitAddRI(AT::kStackReg, 32);
#endif
        call_sites_.push_back({UINT32_MAX, call_pos});
        RecordGcPoint(call_pos);
    }

    // Restore callee-saved XMMs before deallocating frame
    for (uint32_t si = 0; si < num_fpr_callee_; ++si) {
        int32_t off = static_cast<int32_t>(kFrameSize + frame_size_extra_ + frame_align_adj_ + si * 16);
        enc_.EmitMovUPRM(callee_xmm_regs_[si], AT::kStackReg, off);
    }
#if defined(__aarch64__)
    enc_.EmitAddRI(AT::kStackReg, static_cast<int32_t>(kFrameSize + frame_size_extra_ + frame_align_adj_ +
                                                       xmm_save_size_ + localloc_extra_));
    for (uint32_t slot = num_cache_regs_; slot > 0; --slot)
        EmitLdp64Post(buf_, callee_saved_regs_[slot - 1], 0, kARM64_SP,
                      16);                                 // LDP Xreg, X0, [SP], #16 (X0=scratch, never XZR)
    EmitLdp64Post(buf_, 1, 0, kARM64_SP, 16);              // LDP X1, X0, [SP], #16 (pop saved ret_buf)
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

    // Sentinel entry for instr_offsets_ — SEH clause end indices may point
    // one past the last instruction (exclusive-end convention).
    instr_offsets_.push_back(buf_.pos());

    // ── Cold section (Throw/Rethrow handlers) ────────────────────────────
    // Emitted after the epilogue so hot-path instructions stay contiguous
    // (better icache utilization).  Each cold patch emits a short call to
    // ChaosJitRaiseException + INT3.  The VEH handler redirects RIP on
    // success, so we don't need register spill/reload (EmitCallReg is safe).
    //
    // NOTE: Do NOT use EmitCallWithSpill here — the cold path never returns
    // normally (VEH redirects RIP), so register state preservation is not
    // needed and would introduce unnecessary code in the hot section.
    for (auto& cp : cold_patches_) {
        uint32_t cold_target = buf_.pos();
        uint32_t call_start = buf_.pos();
        enc_.EmitCallRipRel(0);
        slot_patches_.push_back(
            {call_start + kSlotPatchDispOff, UINT32_MAX, reinterpret_cast<void*>(ChaosJitRaiseException)});
        slot_count_used_++;
        uint32_t call_pos = call_start;
#if defined(__aarch64__)
        buf_.Emit32(0xD4200000); // BRK #0 (ARM64 breakpoint)
#else
        buf_.EmitByte(0xCC); // INT3 safety net
#endif
        int32_t disp = static_cast<int32_t>(cold_target - (cp.patch_offset + 4));
        buf_.Patch32(cp.patch_offset, static_cast<uint32_t>(disp));
    }

    if (CheckFailed())
        return nullptr;
    if (buf_.pos() == 0 || instr_offsets_.empty())
        return nullptr;

    // Resolve branches (including deopt jump patches)
    ResolveBranches();
    if (CheckFailed())
        return nullptr;

    // ── Emit SEH clause table ──────────────────────────────────────────
    // Appended after code body; VEH handler reads from nm->code + seh_table_offset.
    //
    // Filter clauses (flags == 0x1) follow a special convention to keep the
    // table fixed-size: the handler_start_offset slot stores the FILTER
    // FUNCTION offset, not the handler offset.  The actual handler is
    // emitted immediately after the filter (sequential layout).  The T4 VEH
    // dispatch path runs the filter, and if it returns non-zero, control
    // falls through into the handler; if zero, the search continues outward.
    uint32_t seh_offset = 0;
    if (!is_tier0_ && !rm_.seh_clauses.empty()) {
        seh_offset = buf_.pos();
        uint32_t count = static_cast<uint32_t>(rm_.seh_clauses.size());
        uint32_t max_idx = n_instrs; // instr_offsets_ has n_instrs + 1 entries (sentinel at end)
        uint32_t emitted_count = 0;
        buf_.Emit32(count);
        for (const auto& clause : rm_.seh_clauses) {
            uint32_t try_start = static_cast<uint32_t>(clause.try_start_idx);
            uint32_t try_end = static_cast<uint32_t>(clause.try_end_idx);
            uint32_t handler_start = static_cast<uint32_t>(clause.handler_start_idx);
            const auto cflags_u32 = static_cast<uint32_t>(clause.flags);
            const bool is_filter = (cflags_u32 == static_cast<uint32_t>(interpreter::SEHFlags::Filter));
            // For filter clauses, the dispatched offset is the filter function;
            // the actual handler follows the filter sequentially.
            uint32_t dispatch_idx = handler_start;
            // Validate indices: skip malformed clauses (defensive, not a crash).
            if (try_start >= max_idx || try_end > max_idx || dispatch_idx >= max_idx || handler_start >= max_idx) {
                CHAOS_IL2CPP_LOG_WARN_M(
                    "codegen",
                    "Compile: SEH clause has out-of-range indices "
                    "(try_start={} try_end={} handler_start={} dispatch_idx={} max_idx={}), skipping",
                    try_start, try_end, handler_start, dispatch_idx, max_idx);
                continue;
            }
            buf_.Emit32(cflags_u32);
            buf_.Emit32(instr_offsets_[try_start]);
            buf_.Emit32(instr_offsets_[try_end]);
            buf_.Emit32(instr_offsets_[dispatch_idx]);
            buf_.Emit32(clause.class_token);
            ++emitted_count;
        }
        // Overwrite count with actual emitted count (skipped malformed clauses)
        buf_.Patch32(seh_offset, emitted_count);
    }

    // ── Emit .pdata/.xdata unwind info ──────────────────────────────────
    // Win64 RUNTIME_FUNCTION for OS stack unwinding (debugger, backtrace).
    // V1: no UNW_FLAG_EHANDLER (exception dispatch still via VEH handler).
    // OSR entry stub gets a separate RUNTIME_FUNCTION (V2).
    uint32_t unwind_data_offset = 0;
    uint32_t code_body_size = buf_.pos(); // Function body ends before metadata
    bool has_seh = !rm_.seh_clauses.empty();
#if defined(_WIN64)
    // Pad to 4-byte alignment: Win64 requires UNWIND_INFO to be DWORD-aligned.
    // The code body may end at any byte alignment; emit NOP padding to the
    // next 4-byte boundary so the UNWIND_INFO starts aligned.
    while (buf_.pos() % 4 != 0) {
        buf_.EmitByte(0x90); // NOP padding
    }
    if (!is_tier0_ && prologue_total_bytes_ > 0 && num_push_regs_ > 0) {
        unwind_data_offset =
            EmitUnwindInfo(buf_, prologue_total_bytes_, prologue_sub_rsp_size_, num_push_regs_, push_reg_nums_,
                           prologue_push_offsets_, prologue_sub_rsp_offset_, prologue_set_fpreg_offset_, has_seh);
        if (unwind_data_offset > 0) {
        }
    }
#endif

    // ── Emit .eh_frame DWARF CFI (Linux) ─────────────────────────────────
#if defined(__linux__)
    if (!is_tier0_ && prologue_total_bytes_ > 0 && num_push_regs_ > 0) {
        uint32_t cie_off = EmitDwarfCie(buf_);
        EmitDwarfFde(buf_, cie_off, code_body_size, num_push_regs_, push_reg_nums_);
        eh_frame_offset_ = cie_off;
        buf_.Emit32(0); // .eh_frame zero-length end marker
    }
#endif

    // ── Emit OSR entry stub ─────────────────────────────────────────────
    // Enables true mid-execution OSR: copies the current RegisterFile to
    // the stack frame and jumps to instruction 0. The hot loop then
    // continues in native code from the already-populated register state.
    // Convention: RCX=RegisterFile*, RDX=ret_buf.
    uint32_t osr_entry = 0;
    {
        bool has_loop = false;
        for (uint32_t i = 0; i < n_instrs && !has_loop; ++i) {
            const auto& instr = rm_.instructions[i];
            if (instr.is_branch() && instr.imm.branch_target < i) {
                has_loop = true;
            }
        }
        if (has_loop) {
            osr_entry = buf_.pos();
#if defined(__aarch64__)
            EmitStp64Pre(buf_, AT::kFrameReg, 30, kARM64_SP, -16); // STP X29, X30, [SP, #-16]!
            enc_.EmitAddRI(AT::kFrameReg, AT::kStackReg, 0);
            for (uint32_t slot = 0; slot < num_cache_regs_; ++slot)
                EmitStp64Pre(buf_, callee_saved_regs_[slot], 0, kARM64_SP,
                             -16); // STP Xreg, X0, [SP, #-16]! (X0=scratch, never XZR)
#else
            enc_.EmitPush(AT::kFrameReg);
            enc_.EmitMovRR(AT::kFrameReg, AT::kStackReg); // frame pointer chain
            enc_.EmitPush(AT::kArgsBuf);
            enc_.EmitPush(AT::kRetBuf);
            enc_.EmitPush(kRDI); // x64 ABI: RDI is callee-saved; matches main prologue
            for (uint32_t slot = 0; slot < num_cache_regs_; ++slot)
                enc_.EmitPush(callee_saved_regs_[slot]);
#endif
            enc_.EmitSubRI(AT::kStackReg, static_cast<int32_t>(kFrameSize + frame_size_extra_ + frame_align_adj_ +
                                                               xmm_save_size_ + localloc_extra_));
            // Zero the localloc bump pointer for OSR entry
            if (localloc_extra_ > 0) {
                enc_.EmitXorRR(AT::kScratchA, AT::kScratchA);
                enc_.EmitMovMR(AT::kStackReg,
                               static_cast<int32_t>(kFrameSize + frame_size_extra_ + frame_align_adj_ + xmm_save_size_),
                               AT::kScratchA);
            }
            // using rep movsq (RSI=source, RDI=dest, RCX=count)
#if defined(__aarch64__)
            // ARM64: copy RegisterFile to stack frame (96 quadwords)
            // kScratchB (X1/RCX) = RegisterFile* (source, preserved)
            // dest = SP + kGprFileOff
            for (uint32_t i = 0; i < kGprCount + kFprCount; ++i) {
                ::chaos::il2cpp::jit::EmitLdr64(buf_, AT::kScratchA, AT::kScratchB, static_cast<uint16_t>(i));
                ::chaos::il2cpp::jit::EmitStr64(buf_, AT::kScratchA, AT::kStackReg,
                                                static_cast<uint16_t>((kGprFileOff + i * 8) / 8));
            }
#else
            enc_.EmitLeaRM(kRDI, AT::kStackReg, static_cast<int32_t>(kGprFileOff));
            enc_.EmitMovRR(AT::kRetBuf, AT::kScratchB);
            enc_.EmitMovRIImm32(AT::kScratchB, kGprCount + kFprCount); // 96
            buf_.EmitByte(0xF3);                                       // REP prefix
            buf_.EmitByte(0x48);                                       // REX.W
            buf_.EmitByte(0xA5);                                       // MOVSQ
#endif

            // Set up register convention: RBX=args_buf, RSI=ret_buf
            enc_.EmitLeaRM(AT::kArgsBuf, AT::kStackReg, static_cast<int32_t>(kGprFileOff));
            enc_.EmitMovRR(AT::kRetBuf, AT::kScratchC);

            // Initialize cached/colored registers from the stack frame
            if (config_.enable_register_caching) {
                for (uint32_t slot = 0; slot < num_cache_regs_; ++slot) {
                    uint8_t x64_reg = callee_saved_regs_[slot];
                    if (has_graph_coloring_) {
                        uint32_t vreg = phys_to_colored_vreg_[x64_reg];
                        if (vreg != 0xFF) {
                            enc_.EmitMovRM(x64_reg, AT::kArgsBuf, static_cast<int32_t>(vreg * 8));
                        }
                    } else {
                        uint32_t vreg = phys_to_cached_vreg_[x64_reg];
                        if (vreg != kNotCached) {
                            enc_.EmitMovRM(x64_reg, AT::kArgsBuf, static_cast<int32_t>(vreg * 8));
                        }
                    }
                }
                cached_dirty_mask_ = 0; // clean after initialization
            }

            // Zero caller-colored registers — matches regular prologue.
            // OSR entry only loads callee-saved regs from the stack (above);
            // caller-colored regs have no stack slot, so without this they
            // contain garbage from the caller, while RegisterExecute returns 0.
            if (caller_colored_mask_) {
#if defined(__aarch64__)
                for (uint32_t x64r = 5; x64r <= 17; ++x64r)
                    enc_.EmitXorRR(x64r, x64r);
#else
                for (uint32_t x64r = AT::kExtraScratch0; x64r <= AT::kExtraScratch2; ++x64r)
                    enc_.EmitXorRR(x64r, x64r);
#endif
            }

            // Initialize callee-saved XMM registers from RegisterFile FPR copy.
            // Must also save to the XMM save area so the epilogue (Ret/deopt_return)
            // can restore them correctly.
            for (uint32_t si = 0; si < num_fpr_callee_; ++si) {
                uint8_t xmm_reg = callee_xmm_regs_[si];
                uint32_t fi = callee_xmm_fi_[si];
                int32_t fpr_off = static_cast<int32_t>(kFprFileOff + fi * kFprSlotSize);
                enc_.EmitMovSDRM(xmm_reg, AT::kStackReg, fpr_off);
                // Duplicate to XMM save area so Ret/deopt_return epilogue can restore
                int32_t save_off = static_cast<int32_t>(kFrameSize + frame_size_extra_ + frame_align_adj_ + si * 16);
                enc_.EmitMovUPSMR(AT::kStackReg, save_off, xmm_reg);
            }

            // ── GC mode switch: EnterCooperativeMode (OSR entry) ───────
            // OSR resumes mid-execution in a hot loop.  The thread must be
            // in cooperative mode before the first managed object access.
            if (config_.cooperative_fn != nullptr) {
#if !defined(__aarch64__)
                enc_.EmitSubRI(AT::kStackReg, 32);
#endif
                uint32_t call_start = buf_.pos();
                enc_.EmitCallRipRel(0);
                slot_patches_.push_back(
                    {call_start + kSlotPatchDispOff, UINT32_MAX, reinterpret_cast<void*>(config_.cooperative_fn)});
                slot_count_used_++;
                uint32_t call_pos = call_start;
#if !defined(__aarch64__)
                enc_.EmitAddRI(AT::kStackReg, 32);
#endif
                call_sites_.push_back({UINT32_MAX, call_pos});
                RecordGcPoint(call_pos);
            }

            // Resolve loop header target and jump there.
            // Since OSR always restarts at the backward branch target
            // (loop header), we call OsrResolveLoopHeader() which reads
            // t_deopt_state.osr_resume_pc and resolves it through the
            // persisted instr_offsets table to an absolute native address.
#if !defined(__aarch64__)
            enc_.EmitSubRI(AT::kStackReg, 32); // shadow space for Win64
#endif
            EmitRuntimeHelperCall(reinterpret_cast<void*>(::OsrResolveLoopHeader));
#if !defined(__aarch64__)
            enc_.EmitAddRI(AT::kStackReg, 32); // restore shadow space
#endif

            // RAX now holds the target native address (or nullptr if resolution failed).
            // Null-check RAX before jumping: if null, return to caller (fall back to
            // interpreter).  The caller initialized ret_buf[0]=0, so a bare ret here
            // signals "OSR failed — continue interpreting".
#if defined(__aarch64__)
            // ARM64: CBNZ XscratchA, #4 (skip RET if non-null)
            buf_.Emit32(0xB5000020u | AT::kScratchA);
            enc_.EmitRet();
#else
            EmitTestRR(buf_, AT::kScratchA, AT::kScratchA);
            buf_.EmitByte(0x75); // JNE rel8
            buf_.EmitByte(0x01); // skip 1 byte (the RET)
            buf_.EmitByte(0xC3); // RET
#endif
            // RAX non-null: jump to resolved loop header address.
            // Re-zero caller-colored regs clobbered by the call.
            if (caller_colored_mask_) {
#if defined(__aarch64__)
                for (uint32_t x64r = 5; x64r <= 17; ++x64r)
                    enc_.EmitXorRR(x64r, x64r);
#else
                for (uint32_t x64r = AT::kExtraScratch0; x64r <= AT::kExtraScratch2; ++x64r)
                    enc_.EmitXorRR(x64r, x64r);
#endif
            }
            enc_.EmitJmpReg(AT::kScratchA);
        }
    }

    if (CheckFailed())
        return nullptr;

    // ── Emit call-site slot table ─────────────────────────────────────────
    // Slot table embedded in the RX code buffer for call [rip+off] indirection.
    // Each entry is a void* pointer to the target function.  Patch all call
    // instruction RIP-relative displacements now that we know the slot table
    // position.  The table is in RX memory; ReverseSlotMap::UpdateAll uses
    // VirtualProtect to write to it during hotpatch.
    // Sync slot_count_ to actual total (managed + runtime helper slots).
    slot_count_ = static_cast<uint32_t>(slot_patches_.size());
    slot_count_used_ = slot_count_;

    uint32_t slot_table_offset = 0;
    if (slot_count_used_ > 0) {
        slot_table_offset = buf_.pos();
        for (uint32_t si = 0; si < slot_patches_.size(); ++si) {
            auto& sp = slot_patches_[si];
            buf_.Emit64(reinterpret_cast<uint64_t>(sp.target_fn));
#if defined(__aarch64__)
            // ARM64: LDR X17, #imm19 — encode the PC-relative offset.
            // LDR loads from PC + imm19*4, where PC = address of the LDR instruction.
            int64_t ldr_addr = static_cast<int64_t>(sp.patch_offset);
            int64_t slot_addr = static_cast<int64_t>(slot_table_offset + si * 8);
            int64_t disp_bytes = slot_addr - ldr_addr;
            int32_t imm19 = static_cast<int32_t>(disp_bytes / 4);
            uint32_t instr = buf_.Load32(sp.patch_offset);
            instr = (instr & 0xFF00001Fu) | ((static_cast<uint32_t>(imm19) & 0x7FFFFu) << 5);
            buf_.Patch32(sp.patch_offset, instr);
#else
            // Patch the RIP-relative displacement: slot_entry - (call_next_addr)
            uint32_t call_next = sp.patch_offset + 4; // FF 15 <disp32> = 6 bytes
            int32_t disp = static_cast<int32_t>((slot_table_offset + si * 8) - call_next);
            buf_.Patch32(sp.patch_offset, static_cast<uint32_t>(disp));
#endif
        }
    }

    // ── Emit JitDebugInfo footer (before Seal, while buffer is RW) ──────
    uint32_t debug_info_offset = 0;
    if (!is_tier0_) {
        debug_info_offset = buf_.pos();

        // 1. Emit instr_offsets array (uint32_t[])
        uint32_t offsets_off = debug_info_offset;
        for (auto off : instr_offsets_) {
            buf_.Emit32(off);
        }

        // 2. Emit method name placeholder: "T4_{module_id}_{token}"
        uint32_t name_off = buf_.pos();
        char name_buf[128];
        int name_len =
            std::snprintf(name_buf, sizeof(name_buf), "T4_%08X_%08X", config_.method_module_id, config_.method_token);
        buf_.EmitBytes(name_buf, static_cast<uint32_t>(name_len + 1));

        // 3. Emit JitDebugInfo header
        JitDebugInfo di;
        debug_info_offset = buf_.pos(); // Record offset of the struct itself
        di.magic = JitDebugInfo::kMagic;
        di.version = JitDebugInfo::kVersion;
        di.code_size = buf_.pos();
        di.instr_offset_count = static_cast<uint32_t>(instr_offsets_.size());
        di.instr_offsets_off = offsets_off;
        di.method_name_off = name_off;
        di.method_name_len = static_cast<uint32_t>(name_len);
        buf_.EmitBytes(&di, sizeof(di));
    }

    // Seal code buffer — returns nullptr on OOM or failure
    if (CheckFailed())
        return nullptr;
    uint32_t code_bytes = buf_.pos();
    void* code = buf_.Seal();
    if (code == nullptr)
        return nullptr;

    CHAOS_IL2CPP_LOG_DEBUG_M("codegen", "Compile: {} instrs, {} bytes code, {} call sites", n_instrs, code_bytes,
                             call_sites_.size());

    // Build JitMethod (pre-allocated as nm_ before instruction emission)
    nm->code = code;
    nm->code_size = code_bytes;
    nm->instr_count = n_instrs;
    nm->seh_table_offset = seh_offset;
    nm->osr_entry_offset = osr_entry;
    nm->debug_info_offset = debug_info_offset;

    // Wire up call-site slot table (embedded in the RX code buffer).
    // call_site_slots points into the sealed buffer after buf_.Seal().
    // Slot entries are written with VirtualProtect during hotpatch.
    if (slot_count_used_ > 0) {
        nm->call_site_slots = reinterpret_cast<void**>(static_cast<uint8_t*>(code) + slot_table_offset);
        nm->call_site_slot_count = slot_count_used_;
        nm->call_site_capacity = slot_count_;

        // Register each slot in the reverse map for hotpatch updates.
        for (uint32_t si = 0; si < slot_patches_.size(); ++si) {
            auto& sp = slot_patches_[si];
            uint32_t callee_token = 0;
            if (sp.call_site_index < call_sites_.size()) {
                callee_token = call_sites_[sp.call_site_index].method_token;
            }
            if (callee_token != 0) {
                g_reverse_slot_map.Add(callee_token, nm, si);
            }
        }
    }

    // ── Populate inlined callee info ──────────────────────────────────
    // Records callee tokens and version snapshots for hot-update staleness
    // detection.  If a callee is later hotpatched, InlineReverseMap sets
    // nm->stale = true, triggering recompilation on next dispatch.
    if (inline_results.count > 0) {
        nm->inlined_callees = static_cast<JitMethod::InlinedCallee*>(
            CHAOS_IL2CPP_MALLOC(inline_results.count * sizeof(JitMethod::InlinedCallee)));
        if (nm->inlined_callees) {
            for (uint32_t ri = 0; ri < inline_results.count; ++ri) {
                nm->inlined_callees[ri].callee_token = inline_results.callee_tokens[ri];
                nm->inlined_callees[ri].snapshot_version = inline_results.snapshot_versions[ri];
                g_inline_reverse_map.Add(inline_results.callee_tokens[ri], nm);
            }
            nm->inlined_callee_count = inline_results.count;
        }
    }

    // Persist instruction offset table for OSR loop header resolution.
    if (!instr_offsets_.empty()) {
        nm->instr_offset_count = static_cast<uint32_t>(instr_offsets_.size());
        nm->instr_offsets = static_cast<uint32_t*>(CHAOS_IL2CPP_MALLOC(nm->instr_offset_count * sizeof(uint32_t)));
        if (nm->instr_offsets) {
            std::memcpy(nm->instr_offsets, instr_offsets_.data(), nm->instr_offset_count * sizeof(uint32_t));
        }
    }

    if (!call_sites_.empty()) {
        nm->call_site_count = static_cast<uint32_t>(call_sites_.size());
        nm->call_sites = static_cast<CallSiteInfo*>(CHAOS_IL2CPP_MALLOC(nm->call_site_count * sizeof(CallSiteInfo)));
        if (nm->call_sites)
            std::memcpy(nm->call_sites, call_sites_.data(), nm->call_site_count * sizeof(CallSiteInfo));
    }
    if (!deopt_entries_.empty()) {
        nm->deopt_entry_count = static_cast<uint32_t>(deopt_entries_.size());
        nm->deopt_entries = static_cast<DeoptEntry*>(CHAOS_IL2CPP_MALLOC(nm->deopt_entry_count * sizeof(DeoptEntry)));
        if (nm->deopt_entries)
            std::memcpy(nm->deopt_entries, deopt_entries_.data(), nm->deopt_entry_count * sizeof(DeoptEntry));
    }
    if (!deopt_values_.empty()) {
        uint32_t n = static_cast<uint32_t>(deopt_values_.size());
        nm->deopt_values = static_cast<DeoptValue*>(CHAOS_IL2CPP_MALLOC(n * sizeof(DeoptValue)));
        if (nm->deopt_values)
            std::memcpy(nm->deopt_values, deopt_values_.data(), n * sizeof(DeoptValue));
    }
    if (!gc_points_.empty()) {
        nm->gc_point_count = static_cast<uint32_t>(gc_points_.size());
        nm->gc_points = static_cast<GcPoint*>(CHAOS_IL2CPP_MALLOC(nm->gc_point_count * sizeof(GcPoint)));
        if (nm->gc_points)
            std::memcpy(nm->gc_points, gc_points_.data(), nm->gc_point_count * sizeof(GcPoint));
    }

    // Serialize GcSlotMapV0
    if (!slot_map_entries_.empty()) {
        uint32_t num_slots = static_cast<uint32_t>(slot_map_entries_.size());
        uint32_t map_size = sizeof(GcSlotMapV0) + num_slots * sizeof(uint32_t);
        auto* map_data = static_cast<uint8_t*>(CHAOS_IL2CPP_MALLOC(map_size));
        if (map_data) {
            auto* sm = reinterpret_cast<GcSlotMapV0*>(map_data);
            sm->frame_size = static_cast<uint32_t>(kFrameSize + frame_size_extra_ + frame_align_adj_);
            sm->num_gc_slots = num_slots;
            std::memcpy(sm->slots, slot_map_entries_.data(), num_slots * sizeof(uint32_t));
            nm->slot_map_data = map_data;
            nm->slot_map_size = map_size;
            nm->gc_slot_map = reinterpret_cast<GcSlotMapV0*>(map_data);
        }
    }

    // T2.2-A: Serialize per-safepoint precise root map (GcPointMapV0).
    // For each GC point (already liveness-filtered ObjectRef vregs at that
    // point), emit a GcSafepointV0 with its exact live stack slots so the
    // scanner can report only the roots live at the return offset — replacing
    // the whole-method union above with per-safepoint precision.
    if (!gc_points_.empty() && nm->code_size > 0) {
        // Compute total size: header + per-safepoint (GcSafepointV0 fixed part
        // + num_gc_slots stack-slot encodings + num_live_regs register encodings).
        auto popcount = [](uint32_t m) noexcept {
            uint32_t n = 0;
            while (m) {
                n += (m & 1u);
                m >>= 1;
            }
            return n;
        };
        uint32_t total = sizeof(GcPointMapV0);
        for (const auto& gp : gc_points_) {
            uint32_t nregs = popcount(gp.live_reg_mask);
            total += sizeof(GcSafepointV0) + (gp.slot_count + nregs) * sizeof(uint32_t);
        }
        auto* pm = static_cast<uint8_t*>(CHAOS_IL2CPP_MALLOC(total));
        if (pm) {
            auto* gm = reinterpret_cast<GcPointMapV0*>(pm);
            gm->code_size = nm->code_size;
            gm->num_gprs = static_cast<uint32_t>(kPhysRegCount);
            uint32_t count = static_cast<uint32_t>(gc_points_.size());
            gm->num_safepoints = count;
            uint8_t* cursor = pm + sizeof(GcPointMapV0);
            for (uint32_t i = 0; i < count; ++i) {
                const GcPoint& gp = gc_points_[i];
                auto* sp = reinterpret_cast<GcSafepointV0*>(cursor);
                sp->native_offset = gp.native_offset;
                // Precise stack slots for this safepoint (Task A), plus live
                // volatile-register roots (Task B) as register encodings.
                sp->num_gc_slots = gp.slot_count;
                sp->num_live_regs = popcount(gp.live_reg_mask);
                uint32_t* slit = sp->slots;
                for (uint32_t si = 0; si < gp.slot_count; ++si) {
                    // GcSlot may represent a stack spill slot (offset/8) or a
                    // register.  Reconstruct the GcSlotMap-style offset-only
                    // encoding for stack roots.
                    if (gp.slots[si].kind == GcSlotKind::Stack) {
                        uint32_t byte_off = static_cast<uint32_t>(gp.slots[si].index) * 8u;
                        *slit++ = CHAOS_GC_SLOT_ENCODE(byte_off, CHAOS_GC_SLOT_KIND_OBJECT);
                    }
                }
                // T2.2-B: serialize the live volatile-register root mask as
                // register encodings (physical reg index | object kind).
                for (uint32_t phys = 0; phys < 12; ++phys) {
                    if (gp.live_reg_mask & (1u << phys)) {
                        *slit++ = CHAOS_GC_REG_ENCODE(phys, CHAOS_GC_SLOT_KIND_OBJECT);
                    }
                }
                cursor += sizeof(GcSafepointV0) + (gp.slot_count + sp->num_live_regs) * sizeof(uint32_t);
            }
            (void)gm;
            nm->gc_point_map_data = pm;
            nm->gc_point_map_size = total;
        }
    }

    nm->rbp_to_rsp_offset =
        16 + num_cache_regs_ * 8 +
        static_cast<uint32_t>(kFrameSize + frame_size_extra_ + frame_align_adj_ + xmm_save_size_ + localloc_extra_);

    // ── Allocate Win64 RUNTIME_FUNCTION for .pdata registration ─────────
#if defined(_WIN64)
    if (unwind_data_offset > 0) {
        nm->runtime_function = AllocRuntimeFunction(unwind_data_offset, code_body_size);
    }
#elif defined(__linux__)
    // Store .eh_frame DWARF CFI offset for __register_frame in RegisterNativeCodeSection.
    nm->eh_frame_offset = eh_frame_offset_;
#endif

    CHAOS_IL2CPP_LOG_INFO_M("codegen", "Generate: method compiled, code_size={}, code={:p}, slots={}", nm->code_size,
                            static_cast<void*>(nm->code), slot_count_used_);

    if (collect_stats_) {
        // Per-method allocation-quality record.  vregs not colored are 0xFF
        // (spilled through) — approximate "spilled" as = the 0xFF count.
        CodegenMethodStats ms;
        ms.n_instrs = n_instrs;
        ms.gpr_vreg_total = kGprCount;
        for (uint32_t vr = 0; vr < kGprCount; ++vr) {
            if (gcr_.gpr_color[vr] == 0xFF)
                ++ms.gpr_vreg_spilled;
            else
                ++ms.gpr_vreg_colored;
        }
        for (uint32_t fi = 0; fi < kFprCount; ++fi) {
            if (gcr_.fpr_color[fi] == 0xFF)
                ++ms.fpr_vreg_spilled;
            else
                ++ms.fpr_vreg_colored;
        }
        RecordMethodStats(ms);
        // Write the aggregated report once (idempotent — regenerates the file).
        DumpCodegenStatsJson("jit_allocation_stats.json");
    }
    return nm;
}

JitMethod* Compile(const ::chaos::il2cpp::interpreter::RegisterMethod& rm, const CompileConfig& config) noexcept {
    if (rm.instructions.empty())
        return nullptr;
    ISehHandler& seh = GetSehHandler();
    NativeCodeGenerator gen(rm, config, seh);
    return gen.Generate();
}

bool CanCompile(const ::chaos::il2cpp::interpreter::RegisterMethod& rm) noexcept {
    using namespace chaos::il2cpp::interpreter;
    if (rm.instructions.empty())
        return false;
    // Validate SEH clause indices — any out-of-range clause means the
    // IR is malformed.  Generate() skips them, but early rejection here
    // prevents partial codegen.
    uint32_t n_instrs = static_cast<uint32_t>(rm.instructions.size());
    for (const auto& clause : rm.seh_clauses) {
        if (static_cast<uint32_t>(clause.try_start_idx) >= n_instrs ||
            static_cast<uint32_t>(clause.try_end_idx) > n_instrs ||
            static_cast<uint32_t>(clause.handler_start_idx) >= n_instrs) {
            return false;
        }
    }
    return true; // All opcodes accepted — unsupported ones deopt at runtime.
}

JitMethod::~JitMethod() noexcept {
    CHAOS_IL2CPP_FREE(call_sites);

    // Remove this method's slots from the reverse map (so hotpatch updates
    // don't try to write to freed JitMethod pointers).
    g_reverse_slot_map.RemoveAll(this);
    CHAOS_IL2CPP_FREE(deopt_entries);
    CHAOS_IL2CPP_FREE(deopt_values);
    CHAOS_IL2CPP_FREE(gc_points);
    // Unregister GC slot map before freeing the backing data.
    if (code != nullptr && gc_slot_map != nullptr) {
        chaos::il2cpp::runtime_core::GcUnregisterSlotMap(code);
    }
    CHAOS_IL2CPP_FREE(slot_map_data);
    CHAOS_IL2CPP_FREE(gc_point_map_data);
    CHAOS_IL2CPP_FREE(instr_offsets);
    // Free GcPoint.slots arrays (each allocated independently by RecordGcPoint)
    for (uint32_t i = 0; i < gc_point_count; ++i) {
        CHAOS_IL2CPP_FREE(gc_points[i].slots);
    }
#if defined(_WIN64)
    if (runtime_function != nullptr) {
        RtlDeleteFunctionTable(static_cast<PRUNTIME_FUNCTION>(runtime_function));
        CHAOS_IL2CPP_FREE(runtime_function);
        runtime_function = nullptr;
    }
#elif defined(__linux__)
    if (eh_frame_registered && code != nullptr) {
        const void* eh_frame = static_cast<const uint8_t*>(code) + eh_frame_offset;
        __deregister_frame(eh_frame);
    }
#endif
    if (code != nullptr && !code_managed_externally) {
        chaos::il2cpp::pal::PalVirtualFree(code, code_size);
    }
    code = nullptr;
}

JitMethod::JitMethod(JitMethod&& other) noexcept {
    std::memcpy(this, &other, sizeof(*this));
    std::memset(&other, 0, sizeof(other));
}

JitMethod& JitMethod::operator=(JitMethod&& other) noexcept {
    if (this != &other) {
        this->~JitMethod();
        std::memcpy(this, &other, sizeof(*this));
        std::memset(&other, 0, sizeof(other));
    }
    return *this;
}


} // namespace chaos::il2cpp::jit
