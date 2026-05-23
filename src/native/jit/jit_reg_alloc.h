#ifndef CHAOS_IL2CPP_CODEGEN_REG_ALLOC_GRAPH_COLORING_H_
#define CHAOS_IL2CPP_CODEGEN_REG_ALLOC_GRAPH_COLORING_H_

// ── Graph-Coloring Register Allocator ──────────────────────────────────────
//
// Takes a RegisterMethod (register-allocated IR with virtual registers) and
// assigns x64 physical registers via Chaitin-Briggs graph coloring.
//
// The allocator:
//   1. Computes liveness intervals via backward dataflow
//   2. Builds an interference graph (compact uint64_t[64] adjacency matrix)
//   3. Runs simplify+select (Kempe's algorithm) to assign 5 callee-saved
//      x64 GPRs (RDI, R12-R15)
//   4. Separately allocates 16 XMM registers for FPRs
//
// V1 only uses callee-saved registers to avoid stale-color issues after
// call instructions.  V2 can add caller-saved regs with post-call reload.
//
// Result: GraphColoringResult maps each virtual register to a physical
// register index (0xFF = spill to stack slot).

#include "../interpreter/ir_reg_alloc.h"

#include <cstdint>
#include <cstring>

namespace chaos::il2cpp::jit {

// ── Portable bit manipulation helpers (MSVC/GCC/Clang) ─────────────────────
namespace detail {

#if defined(_MSC_VER)
#include <intrin.h>
#pragma intrinsic(_BitScanForward64)
#pragma intrinsic(_BitScanForward)
#endif

/// Count trailing zeros in a 64-bit value.  Returns 64 if x == 0.
inline uint32_t Ctz64(uint64_t x) noexcept {
#if defined(__GNUC__) || defined(__clang__)
    return static_cast<uint32_t>(__builtin_ctzll(x));
#elif defined(_MSC_VER)
    unsigned long idx = 0;
    _BitScanForward64(&idx, x);
    return static_cast<uint32_t>(idx);
#else
    // Portable fallback (rarely used).
    if (x == 0) return 64;
    uint32_t n = 0;
    for (; (x & 1) == 0; x >>= 1) ++n;
    return n;
#endif
}

/// Count trailing zeros in a 32-bit value.  Returns 32 if x == 0.
inline uint32_t Ctz32(uint32_t x) noexcept {
#if defined(__GNUC__) || defined(__clang__)
    return static_cast<uint32_t>(__builtin_ctz(x));
#elif defined(_MSC_VER)
    unsigned long idx = 0;
    _BitScanForward(&idx, x);
    return static_cast<uint32_t>(idx);
#else
    if (x == 0) return 32;
    uint32_t n = 0;
    for (; (x & 1) == 0; x >>= 1) ++n;
    return n;
#endif
}

/// Population count (number of set bits) in a 64-bit value.
inline uint32_t Popcount64(uint64_t x) noexcept {
#if defined(__GNUC__) || defined(__clang__)
    return static_cast<uint32_t>(__builtin_popcountll(x));
#elif defined(_MSC_VER)
    return static_cast<uint32_t>(__popcnt64(x));
#else
    // SWAR popcount (portable).
    x = x - ((x >> 1) & 0x5555555555555555ULL);
    x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
    x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
    return static_cast<uint32_t>((x * 0x0101010101010101ULL) >> 56);
#endif
}

}  // namespace detail

// ── Result ─────────────────────────────────────────────────────────────────
// Maps virtual registers to x64 physical registers.
// gpr_color[vreg] = x64 GPR index (0-15), or 0xFF for spill.
// fpr_color[idx]  = XMM register index (0-15), or 0xFF for spill (idx = vreg - 64).
struct GraphColoringResult {
    uint8_t gpr_color[64] = {};
    uint8_t fpr_color[32] = {};
    uint32_t used_gpr_count = 0;
    uint32_t used_fpr_count = 0;
};

// ── Allocator Entry Point ──────────────────────────────────────────────────
GraphColoringResult AllocateRegistersGraphColoring(
    const interpreter::RegisterMethod& rm) noexcept;

// ── Internal helpers ───────────────────────────────────────────────────────
namespace detail {

// Available x64 GPRs for allocation (5 callee-saved + 4 caller-saved = 9 total).
// On Win64: RDI, R12-R15 are callee-saved (preserved across calls).
// V2 adds R8-R11 (caller-saved) with post-call reload in code_generator.cpp.
// RBX(3)=args_buf, RBP(5)=frame, RSI(6)=ret_buf are reserved.
// RAX(0)/RCX(1)/RDX(2) are excluded — implicitly clobbered by Div/Mul/Shift.
//
// Color-to-physical mapping:
//   Color 0 → RDI  (callee-saved)
//   Color 1 → R8   (caller-saved)
//   Color 2 → R9   (caller-saved)
//   Color 3 → R10  (caller-saved)
//   Color 4 → R11  (caller-saved)
//   Color 5 → R12  (callee-saved)
//   Color 6 → R13  (callee-saved)
//   Color 7 → R14  (callee-saved)
//   Color 8 → R15  (callee-saved)
//
// Callee-saved color set: {0, 5, 6, 7, 8}  (5 colors)
// Caller-saved color set: {1, 2, 3, 4}      (4 colors)
static constexpr uint8_t kPhysicalGprs[] = {
    7,   // RDI — callee-saved
    8,   // R8  — caller-saved
    9,   // R9  — caller-saved
    10,  // R10 — caller-saved
    11,  // R11 — caller-saved
    12,  // R12 — callee-saved
    13,  // R13 — callee-saved
    14,  // R14 — callee-saved
    15,  // R15 — callee-saved
};
static constexpr uint32_t kNumColors = 9;
// Color 0 and colors 5-8 are callee-saved (RDI + R12-R15).
// Colors 1-4 are caller-saved (R8-R11).
static constexpr uint32_t kFirstCallerSavedColor = 1;
static constexpr uint32_t kCallerSavedColorCount = 4;
static constexpr uint8_t kSpilled  = 0xFF;

// Returns true if the color index maps to a caller-saved physical register.
inline bool IsCallerSavedColor(uint32_t color_idx) noexcept {
    return color_idx >= kFirstCallerSavedColor &&
           color_idx < kFirstCallerSavedColor + kCallerSavedColorCount;
}

// Returns true if the color index maps to a callee-saved physical register.
inline bool IsCalleeSavedColor(uint32_t color_idx) noexcept {
    return color_idx == 0 ||
           (color_idx >= kFirstCallerSavedColor + kCallerSavedColorCount &&
            color_idx < kNumColors);
}

// Available XMM registers for FPR allocation (16 registers).
static constexpr uint8_t kPhysicalXmms[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
static constexpr uint32_t kNumXmmColors = 16;

// Max instructions for stack-allocated liveness arrays.
static constexpr uint32_t kMaxStackInstrs = 2048;

// Returns true for unconditional branches (no fall-through).
inline bool IsUnconditionalBranch(interpreter::IROpCode opc) noexcept {
    return opc == interpreter::IROpCode::Br ||
           opc == interpreter::IROpCode::Leave;
}

// Returns true for conditional branches (fall-through + branch target).
inline bool IsConditionalBranch(interpreter::IROpCode opc) noexcept {
    return opc == interpreter::IROpCode::BrTrue ||
           opc == interpreter::IROpCode::BrFalse ||
           opc == interpreter::IROpCode::Beq ||
           opc == interpreter::IROpCode::BneUn ||
           opc == interpreter::IROpCode::Blt ||
           opc == interpreter::IROpCode::Bgt ||
           opc == interpreter::IROpCode::Ble ||
           opc == interpreter::IROpCode::Bge ||
           opc == interpreter::IROpCode::BltUn ||
           opc == interpreter::IROpCode::BgtUn ||
           opc == interpreter::IROpCode::BleUn ||
           opc == interpreter::IROpCode::BgeUn;
}

// Returns true for terminators (no successors, don't propagate liveness).
inline bool IsTerminator(interpreter::IROpCode opc) noexcept {
    return opc == interpreter::IROpCode::Ret ||
           opc == interpreter::IROpCode::Throw ||
           opc == interpreter::IROpCode::Rethrow ||
           opc == interpreter::IROpCode::EndFinally ||
           opc == interpreter::IROpCode::EndFilter;
}

inline uint32_t Popcount(uint64_t x) noexcept {
    return Popcount64(x);
}

}  // namespace detail

// ── Public API ─────────────────────────────────────────────────────────────

inline GraphColoringResult AllocateRegistersGraphColoring(
    const interpreter::RegisterMethod& rm) noexcept
{
    using namespace detail;

    GraphColoringResult result;
    std::memset(result.gpr_color, 0xFF, sizeof(result.gpr_color));
    std::memset(result.fpr_color, 0xFF, sizeof(result.fpr_color));

    uint32_t n_instrs = static_cast<uint32_t>(rm.instructions.size());
    if (n_instrs == 0 || n_instrs > kMaxStackInstrs) return result;

    const auto* instrs = rm.instructions.data();

    // ── Phase 1-2: Liveness + Interference Graph (GPRs) ───────────────────
    // Allocate per-instruction arrays on the caller's stack frame.
    uint64_t live_in[2048] = {};
    uint64_t live_out[2048] = {};
    uint64_t def[2048] = {};
    uint64_t use[2048] = {};

    // Pass 1: compute def/use for each instruction.
    for (uint32_t i = 0; i < n_instrs; ++i) {
        const auto& inst = instrs[i];
        if (inst.has_dst() && inst.dst_reg() < interpreter::kGPRegisters)
            def[i] |= (1ULL << inst.dst_reg());
        if (inst.has_src1() && inst.src1_reg() < interpreter::kGPRegisters)
            use[i] |= (1ULL << inst.src1_reg());
        if (inst.has_src2() && inst.src2_reg() < interpreter::kGPRegisters)
            use[i] |= (1ULL << inst.src2_reg());
        // Third source operand (StElem, StObj, Cpblk, InitBlk)
        if (inst.flags() & interpreter::kRegHasSrc3) {
            uint8_t src3 = inst.src3_reg();
            if (src3 < interpreter::kGPRegisters)
                use[i] |= (1ULL << src3);
        }
        // Calli: func_ptr vreg in imm.operand_index is an implicit source
        if (inst.op_code() == interpreter::IROpCode::Calli &&
            inst.imm.operand_index < interpreter::kGPRegisters) {
            use[i] |= (1ULL << inst.imm.operand_index);
        }
    }

    // Pass 2: backward dataflow to fixed point.
    // live_out[i] = UNION over successors of live_in[succ]
    // live_in[i]  = use[i] | (live_out[i] & ~def[i])
    bool changed = true;
    while (changed) {
        changed = false;
        for (int32_t i = static_cast<int32_t>(n_instrs) - 1; i >= 0; --i) {
            const auto& inst = instrs[i];
            uint64_t new_live_out = 0;
            auto opc = inst.op_code();

            if (opc == interpreter::IROpCode::Switch) {
                // Switch: all case targets are successors (no fall-through).
                uint32_t n_targets = inst.imm.i4;
                if (n_targets > 0 && inst.imm.ptr) {
                    const uint32_t* targets = static_cast<const uint32_t*>(inst.imm.ptr);
                    uint32_t limit = n_targets < 256 ? n_targets : 256;
                    for (uint32_t ti = 0; ti < limit; ++ti) {
                        if (targets[ti] < n_instrs)
                            new_live_out |= live_in[targets[ti]];
                    }
                }
            } else if (IsUnconditionalBranch(opc)) {
                // Branch target inherits live_in from target. No fall-through.
                uint32_t target = inst.imm.branch_target;
                if (target < n_instrs)
                    new_live_out |= live_in[target];
            } else if (IsConditionalBranch(opc)) {
                // Conditional branch: both target and fall-through are successors.
                uint32_t target = inst.imm.branch_target;
                if (target < n_instrs)
                    new_live_out |= live_in[target];
                if (static_cast<uint32_t>(i) + 1 < n_instrs)
                    new_live_out |= live_in[i + 1];
            } else if (IsTerminator(opc)) {
                // Terminator: no successors.
            } else {
                // Default: next instruction is the only successor.
                if (static_cast<uint32_t>(i) + 1 < n_instrs)
                    new_live_out |= live_in[i + 1];
            }

            uint64_t new_live_in = use[i] | (new_live_out & ~def[i]);

            if (new_live_in != live_in[i] || new_live_out != live_out[i]) {
                live_in[i] = new_live_in;
                live_out[i] = new_live_out;
                changed = true;
            }
        }
    }

    // ── Compute first_def / last_use for conservative live range ──────────
    // Standard liveness uses def-kill semantics: when a vreg is redefined,
    // the old definition is killed and doesn't propagate backwards. This
    // means a vreg with multiple definitions has its earlier live range
    // truncated, causing the interference graph to miss true overlaps.
    //
    // Conservative live range: each vreg is live from its FIRST definition
    // to its LAST use, ignoring intermediate redefinitions. This ensures
    // overlapping conservative ranges are correctly marked as interfering.
    uint32_t first_def_idx[64];
    uint32_t last_use_idx[64];
    std::memset(first_def_idx, 0xFF, sizeof(first_def_idx));
    std::memset(last_use_idx, 0xFF, sizeof(last_use_idx));

    for (uint32_t i = 0; i < n_instrs; ++i) {
        uint64_t dbits = def[i];
        while (dbits) {
            uint32_t d = static_cast<uint32_t>(Ctz64(dbits));
            dbits &= dbits - 1;
            if (d < interpreter::kGPRegisters && first_def_idx[d] == UINT32_MAX)
                first_def_idx[d] = i;
        }
        uint64_t ubits = use[i];
        while (ubits) {
            uint32_t u = static_cast<uint32_t>(Ctz64(ubits));
            ubits &= ubits - 1;
            if (u < interpreter::kGPRegisters)
                last_use_idx[u] = i;
        }
    }

    // ── Build GPR interference graph ──────────────────────────────────────
    // adj[v] = bitmask of vregs that interfere with v.
    uint64_t adj[64] = {};

    for (uint32_t i = 0; i < n_instrs; ++i) {
        // All vregs in live_in[i] interfere with each other (transitive).
        uint64_t bits = live_in[i];
        while (bits) {
            uint32_t v = static_cast<uint32_t>(Ctz64(bits));
            bits &= bits - 1;
            if (v < interpreter::kGPRegisters) {
                adj[v] |= live_in[i];
                adj[v] &= ~(1ULL << v);  // no self-loop
            }
        }
        // A vreg defined at i interferes with all vregs live across i.
        // Without this, the allocator can assign `def[i]` to the same
        // physical register as a live-out vreg — the definition writes to
        // the shared register, clobbering the live value.
        // Must add edges in BOTH directions: d→live-out (for d's coloring)
        // AND live-out→d (for live-out vregs' coloring).
        uint64_t dbits = def[i];
        while (dbits) {
            uint32_t d = static_cast<uint32_t>(Ctz64(dbits));
            dbits &= dbits - 1;
            if (d < interpreter::kGPRegisters && live_out[i] != 0) {
                adj[d] |= live_out[i];
                adj[d] &= ~(1ULL << d);
                // Reverse edges: each live-out vreg interferes with d
                uint64_t lobits = live_out[i];
                while (lobits) {
                    uint32_t lo = static_cast<uint32_t>(Ctz64(lobits));
                    lobits &= lobits - 1;
                    if (lo < interpreter::kGPRegisters && lo != d) {
                        adj[lo] |= (1ULL << d);
                    }
                }
            }
        }
    }

    // ── Conservative live-range interference ─────────────────────────────
    // Standard liveness uses def-kill: when vreg r4 is redefined at instr 19,
    // r4's earlier value (from instr 13) is killed and doesn't propagate
    // backwards through instr 14. This means def[14] = r2 doesn't see r4
    // as live, allowing r2 and r4 to share a register — but r2's redefinition
    // at instr 14 clobbers r4's live value.
    //
    // Fix: add def→conservative_live interference (bidirectional), where
    // conservative_live tracks vregs from their FIRST definition to their
    // LAST use, ignoring intermediate redefinitions.
    //
    // Source operands of the current instruction are excluded from the
    // interference set (three-address code: src is read before dst is
    // written, so they can safely share a register with the dest).
    {
        uint64_t cons_live = 0;
        for (uint32_t i = 0; i < n_instrs; ++i) {
            // Add vregs first defined at this instruction.
            uint64_t dbits = def[i];
            uint64_t new_defs = 0;
            while (dbits) {
                uint32_t d = static_cast<uint32_t>(Ctz64(dbits));
                dbits &= dbits - 1;
                if (d < interpreter::kGPRegisters && first_def_idx[d] == i)
                    new_defs |= (1ULL << d);
            }
            cons_live |= new_defs;

            // Add bidirectional interference between def[i] and cons_live.
            // Exclude source operands (three-address code: src read before dst write).
            dbits = def[i];
            while (dbits) {
                uint32_t d = static_cast<uint32_t>(Ctz64(dbits));
                dbits &= dbits - 1;
                if (d < interpreter::kGPRegisters) {
                    uint64_t exclude = (1ULL << d) | use[i];
                    uint64_t interfere = cons_live & ~exclude;
                    if (interfere) {
                        adj[d] |= interfere;
                        uint64_t ibits = interfere;
                        while (ibits) {
                            uint32_t iv = static_cast<uint32_t>(Ctz64(ibits));
                            ibits &= ibits - 1;
                            adj[iv] |= (1ULL << d);
                        }
                    }
                }
            }

            // Remove vregs that had their last use at this instruction.
            uint64_t ubits = use[i];
            while (ubits) {
                uint32_t u = static_cast<uint32_t>(Ctz64(ubits));
                ubits &= ubits - 1;
                if (u < interpreter::kGPRegisters && last_use_idx[u] == i)
                    cons_live &= ~(1ULL << u);
            }
        }
    }

    // ── Spill cost ─────────────────────────────────────────────────────────
    // Count uses + defs per vreg; unused vregs get cost 0 (not colored).
    uint64_t cost[64] = {};
    for (uint32_t i = 0; i < n_instrs; ++i) {
        const auto& inst = instrs[i];
        if (inst.has_dst()  && inst.dst_reg()  < interpreter::kGPRegisters) cost[inst.dst_reg()]++;
        if (inst.has_src1() && inst.src1_reg()  < interpreter::kGPRegisters) cost[inst.src1_reg()]++;
        if (inst.has_src2() && inst.src2_reg()  < interpreter::kGPRegisters) cost[inst.src2_reg()]++;
        if ((inst.flags() & interpreter::kRegHasSrc3)) {
            uint8_t src3 = inst.src3_reg();
            if (src3 < interpreter::kGPRegisters) cost[src3]++;
        }
        // Calli: func_ptr vreg in imm.operand_index counts as a use
        if (inst.op_code() == interpreter::IROpCode::Calli &&
            inst.imm.operand_index < interpreter::kGPRegisters) {
            cost[inst.imm.operand_index]++;
        }
    }

    // ── Simplify (Kempe) ───────────────────────────────────────────────────
    // Build active mask (vregs with nonzero cost).
    uint64_t active = 0;
    for (uint32_t i = 0; i < 64; ++i) {
        if (cost[i] > 0) active |= (1ULL << i);
    }

    uint32_t simplify_stack[64];
    uint32_t sp = 0;
    bool potential_spill[64] = {};

    while (active) {
        int32_t best_v = -1;
        uint32_t best_deg = UINT32_MAX;
        uint64_t bits = active;
        while (bits) {
            uint32_t v = static_cast<uint32_t>(Ctz64(bits));
            bits &= bits - 1;
            uint32_t deg = Popcount64(adj[v] & active);
            if (deg < best_deg) {
                best_deg = deg;
                best_v = static_cast<int32_t>(v);
            }
        }
        if (best_v < 0) break;
        uint32_t v = static_cast<uint32_t>(best_v);

        if (best_deg < kNumColors) {
            // Simplify: remove from graph, push to stack.
            simplify_stack[sp++] = v;
            active &= ~(1ULL << v);
        } else {
            // Potential spill: mark and push.
            potential_spill[v] = true;
            simplify_stack[sp++] = v;
            active &= ~(1ULL << v);
        }
    }

    // ── Select colors ──────────────────────────────────────────────────────
    uint8_t color[64];
    std::memset(color, 0xFF, sizeof(color));

    while (sp > 0) {
        --sp;
        uint32_t v = simplify_stack[sp];
        uint32_t available = (1U << kNumColors) - 1;

        uint64_t bits = adj[v];
        while (bits) {
            uint32_t u = static_cast<uint32_t>(Ctz64(bits));
            bits &= bits - 1;
            if (color[u] != 0xFF && color[u] < kNumColors) {
                available &= ~(1U << color[u]);
            }
        }

        if (available != 0) {
            color[v] = static_cast<uint8_t>(Ctz64(available));
        } else if (potential_spill[v]) {
            color[v] = kSpilled;
        } else {
            color[v] = kSpilled;
        }
    }

    // ── Map to physical x64 registers ──────────────────────────────────────
    uint8_t color_used[12] = {};
    for (uint32_t v = 0; v < 64; ++v) {
        if (color[v] != kSpilled && color[v] < kNumColors) {
            result.gpr_color[v] = kPhysicalGprs[color[v]];
            color_used[color[v]] = 1;
        }
    }
    for (uint32_t c = 0; c < kNumColors; ++c) {
        if (color_used[c]) result.used_gpr_count++;
    }

    // ── FPR Allocation (XMM0-XMM15) ────────────────────────────────────────
    // Map FPR vregs (64-95) to indices 0-31.
    uint64_t fpr_adj[32] = {};

    for (uint32_t i = 0; i < n_instrs; ++i) {
        // Extract FPR vregs from the live_in bitmask.
        uint64_t fpr_live = 0;
        uint64_t bits = live_in[i];
        while (bits) {
            uint32_t r = static_cast<uint32_t>(Ctz64(bits));
            bits &= bits - 1;
            if (r >= interpreter::kGPRegisters && r < interpreter::kTotalRegisters) {
                uint32_t fi = r - interpreter::kGPRegisters;
                if (fi < 32) fpr_live |= (1ULL << fi);
            }
        }

        // Transitive interference among live FPRs.
        uint64_t fb = fpr_live;
        while (fb) {
            uint32_t fi = static_cast<uint32_t>(Ctz64(fb));
            fb &= fb - 1;
            if (fi < 32) {
                fpr_adj[fi] |= fpr_live;
                fpr_adj[fi] &= ~(1ULL << fi);
            }
        }

        // FPR def → live-out interference (same bug fix as GPR above).
        // A defined FPR vreg interferes with all FPR vregs that are live across i.
        const auto& inst = instrs[i];
        uint32_t fdst = (inst.has_dst() && inst.dst_reg() >= interpreter::kGPRegisters &&
                         inst.dst_reg() < interpreter::kTotalRegisters)
                            ? inst.dst_reg() - interpreter::kGPRegisters
                            : UINT32_MAX;
        if (fdst < 32) {
            uint64_t fpr_live_out = 0;
            uint64_t obits = live_out[i];
            while (obits) {
                uint32_t r = static_cast<uint32_t>(Ctz64(obits));
                obits &= obits - 1;
                if (r >= interpreter::kGPRegisters && r < interpreter::kTotalRegisters) {
                    uint32_t fi_lo = r - interpreter::kGPRegisters;
                    if (fi_lo < 32) fpr_live_out |= (1ULL << fi_lo);
                }
            }
            fpr_adj[fdst] |= fpr_live_out;
            fpr_adj[fdst] &= ~(1ULL << fdst);
        }
    }

    // FPR cost (unused vregs get cost 0 — not colored).
    uint64_t fpr_cost[32] = {};
    for (uint32_t i = 0; i < n_instrs; ++i) {
        const auto& inst = instrs[i];
        auto fpr_idx = [](uint32_t r) -> uint32_t {
            if (r >= interpreter::kGPRegisters && r < interpreter::kTotalRegisters)
                return r - interpreter::kGPRegisters;
            return UINT32_MAX;
        };
        uint32_t fdst = fpr_idx(inst.dst_reg());
        uint32_t fs1  = fpr_idx(inst.src1_reg());
        uint32_t fs2  = fpr_idx(inst.src2_reg());
        if (fdst != UINT32_MAX && fdst < 32) fpr_cost[fdst]++;
        if (fs1  != UINT32_MAX && fs1  < 32) fpr_cost[fs1]++;
        if (fs2  != UINT32_MAX && fs2  < 32) fpr_cost[fs2]++;
    }

    // FPR simplify.
    uint64_t fpr_active = 0;
    for (uint32_t fi = 0; fi < 32; ++fi) {
        if (fpr_cost[fi] > 0) fpr_active |= (1ULL << fi);
    }
    uint32_t fpr_stack[32];
    uint32_t fsp = 0;
    while (fpr_active) {
        int32_t best_fv = -1;
        uint32_t best_deg = UINT32_MAX;
        uint64_t fbits = fpr_active;
        while (fbits) {
            uint32_t fv = static_cast<uint32_t>(Ctz64(fbits));
            fbits &= fbits - 1;
            uint32_t deg = Popcount64(fpr_adj[fv] & fpr_active);
            if (deg < best_deg) { best_deg = deg; best_fv = static_cast<int32_t>(fv); }
        }
        if (best_fv < 0) break;
        fpr_stack[fsp++] = static_cast<uint32_t>(best_fv);
        fpr_active &= ~(1ULL << static_cast<uint32_t>(best_fv));
    }

    // FPR select.
    uint8_t fpr_color[32];
    std::memset(fpr_color, 0xFF, sizeof(fpr_color));
    while (fsp > 0) {
        --fsp;
        uint32_t fv = fpr_stack[fsp];
        uint32_t available = (1U << kNumXmmColors) - 1;
        uint64_t nbits = fpr_adj[fv];
        while (nbits) {
            uint32_t nu = static_cast<uint32_t>(Ctz64(nbits));
            nbits &= nbits - 1;
            if (fpr_color[nu] != 0xFF && fpr_color[nu] < kNumXmmColors) {
                available &= ~(1U << fpr_color[nu]);
            }
        }
        if (available != 0) {
            fpr_color[fv] = static_cast<uint8_t>(Ctz64(available));
        } else {
            fpr_color[fv] = kSpilled;
        }
    }

    // Map to physical XMM registers.
    uint8_t xmm_used[16] = {};
    for (uint32_t fv = 0; fv < 32; ++fv) {
        if (fpr_color[fv] != kSpilled && fpr_color[fv] < kNumXmmColors) {
            result.fpr_color[fv] = kPhysicalXmms[fpr_color[fv]];
            xmm_used[fpr_color[fv]] = 1;
        }
    }
    for (uint32_t c = 0; c < kNumXmmColors; ++c) {
        if (xmm_used[c]) result.used_fpr_count++;
    }

    return result;
}

}  // namespace chaos::il2cpp::jit

#endif  // CHAOS_IL2CPP_CODEGEN_REG_ALLOC_GRAPH_COLORING_H_
