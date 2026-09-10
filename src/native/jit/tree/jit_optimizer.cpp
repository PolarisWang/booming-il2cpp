// jit_optimizer.cpp — Tree IR optimization orchestration
//
// Pipeline per BB:
//   1. TreeBuilder::Build() — RegisterMethod → expression DAG
//   2. Inliner::InlineRoots — inline eligible kCall nodes (optional)
//   3. IntrinsicMutator — replace known BCL calls with specialized nodes
//   4. ConstFoldMutator — fold constant arithmetic
//   5. BoundCheckEliminator — eliminate redundant array bounds checks
//   6. CSEMutator — eliminate redundant sub-expressions
//   7. Linearizer — optimized DAG → RegisterInstruction sequence
//
// P5 extension: when multiple basic blocks form natural loops, additional
// post-passes run on the linearized instruction stream:
//   a. Constant propagation — identify vregs with known constant values
//   b. LICM — hoist loop-invariant arithmetic out of loops
//   c. IV strength reduction — replace induction-variable mul with add
//   d. Loop unrolling — replicate loop body for constant-trip-count loops

#include "tree/jit_optimizer.h"
#include "tree/jit_optimizer_internal.h"
#include "tree/jit_tree_builder.h"
#include "tree/jit_tree_mutator.h"
#include "tree/jit_linearizer.h"
#include "tree/jit_intrinsics.h"
#include "tree/jit_bounds_check.h"
#include "tree/jit_cfg.h"
#include "jit_inline.h"

#include "interpreter/ir_reg_alloc.h"
#include "interpreter/generated/ir_opcodes.h"

#include <chaos/log.h>

#include <cstdint>
#include <vector>

namespace chaos::il2cpp::jit::tree {
using namespace internal;

// ── BB range helpers ───────────────────────────────────────────────────
// Build bb_starts array from per-BB linearization progress.
static void UpdateBbRanges(const std::vector<interpreter::RegisterInstruction>& out_instrs,
                           std::vector<uint32_t>& bb_starts, uint32_t bb_count) noexcept {
    // bb_starts is populated during the per-BB loop; compute ends here
    // by taking next BB's start (or out_instrs size for the last BB).
    // No action needed if bb_starts is already populated by the caller.
    (void)out_instrs;
    (void)bb_count;
}

// ── Constant propagation pass ──────────────────────────────────────────
//
// Identifies vregs that hold known constant values (LdcI4/LdcI8) and
// propagates them forward.  This enables more LICM hoisting (constant
// vregs are trivially loop-invariant) and more folding in downstream
// codegen passes.
//
// Phase 1: build const table — scan all instructions, record known
// constants for each dst vreg.
//
// Phase 2: forward propagate — replace LdLoc with LdcI4/I8 when the
// src vreg is known-constant; fold pure arithmetic with constant operands.
// Repeats until no changes (typically 1-2 iterations).
static bool ConstPropagate(std::vector<interpreter::RegisterInstruction>& out_instrs, uint32_t max_vreg) noexcept {
    if (out_instrs.empty() || max_vreg == 0)
        return false;

    enum ConstKind : uint8_t { kUnknown, kInt32, kInt64 };
    struct ConstVal {
        ConstKind kind = ConstKind::kUnknown;
        int64_t value = 0;
    };

    std::vector<ConstVal> const_vals(max_vreg);
    bool any_propagated = false;

    // Iterate until stable (typically 1-2 iterations)
    for (uint32_t iter = 0; iter < 4; ++iter) {
        // Reset const table each iteration (can't trust stale values
        // after propagation changes instructions)
        for (auto& cv : const_vals)
            cv = ConstVal {};

        // Phase 1: scan all instructions, record constants
        for (uint32_t i = 0; i < out_instrs.size(); ++i) {
            const auto& ri = out_instrs[i];
            if (!ri.has_dst())
                continue;
            uint32_t dst = ri.dst_reg();
            if (dst >= max_vreg)
                continue;

            switch (ri.op_code()) {
                case interpreter::IROpCode::LdcI4:
                    const_vals[dst] = {kInt32, ri.imm.i4};
                    break;
                case interpreter::IROpCode::LdcI8:
                    const_vals[dst] = {kInt64, ri.imm.i8};
                    break;
                default:
                    // Any other defining instruction → not a known constant
                    const_vals[dst] = {};
                    break;
            }
        }

        // Phase 2: propagate constants forward
        bool changed = false;
        for (uint32_t i = 0; i < out_instrs.size(); ++i) {
            auto& ri = out_instrs[i];

            // Case A: LdLoc → LdcI4/I8 when src vreg is known constant
            if (ri.op_code() == interpreter::IROpCode::LdLoc && ri.has_src1()) {
                uint32_t src = ri.src1_reg();
                if (src >= max_vreg)
                    continue;
                const auto& cv = const_vals[src];
                if (cv.kind == kInt32) {
                    ri.header = (ri.header & ~(0xFFFFull)) | static_cast<uint64_t>(interpreter::IROpCode::LdcI4);
                    ri.imm.i4 = static_cast<int32_t>(cv.value);
                    const_vals[ri.dst_reg()] = cv;
                    changed = true;
                    any_propagated = true;
                } else if (cv.kind == kInt64) {
                    ri.header = (ri.header & ~(0xFFFFull)) | static_cast<uint64_t>(interpreter::IROpCode::LdcI8);
                    ri.imm.i8 = cv.value;
                    const_vals[ri.dst_reg()] = cv;
                    changed = true;
                    any_propagated = true;
                }
                continue;
            }

            // Case B: fold pure arithmetic with constant operands
            if (!IsPureArithmetic(ri.op_code()))
                continue;
            if (!ri.has_dst())
                continue;
            uint32_t dst = ri.dst_reg();
            if (dst >= max_vreg)
                continue;

            // Collect src values
            bool has_src1 = ri.has_src1();
            bool has_src2 = ri.has_src2();
            uint32_t s1 = has_src1 ? ri.src1_reg() : 0;
            uint32_t s2 = has_src2 ? ri.src2_reg() : 0;

            int64_t v1 = 0, v2 = 0;
            bool c1_known = false, c2_known = false;
            ConstKind ck1 = kUnknown, ck2 = kUnknown;

            if (has_src1 && s1 < max_vreg && const_vals[s1].kind != kUnknown) {
                v1 = const_vals[s1].value;
                c1_known = true;
                ck1 = const_vals[s1].kind;
            }
            if (has_src2 && s2 < max_vreg && const_vals[s2].kind != kUnknown) {
                v2 = const_vals[s2].value;
                c2_known = true;
                ck2 = const_vals[s2].kind;
            }

            // For binary ops: both srcs must be known
            if (has_src1 && has_src2 && !(c1_known && c2_known))
                continue;
            // For unary ops (Neg, Not, Conv*): src1 must be known
            if (has_src1 && !has_src2 && !c1_known)
                continue;

            // Determine result type: i64 if either operand is i64
            bool use_i64 = (ck1 == kInt64) || (ck2 == kInt64);

            int64_t result = 0;
            bool foldable = true;

            switch (ri.op_code()) {
                // Binary arithmetic
                case interpreter::IROpCode::Add:
                    result = v1 + v2;
                    break;
                case interpreter::IROpCode::Sub:
                    result = v1 - v2;
                    break;
                case interpreter::IROpCode::Mul:
                    result = v1 * v2;
                    break;
                case interpreter::IROpCode::And:
                    result = v1 & v2;
                    break;
                case interpreter::IROpCode::Or:
                    result = v1 | v2;
                    break;
                case interpreter::IROpCode::Xor:
                    result = v1 ^ v2;
                    break;
                case interpreter::IROpCode::Shl:
                    result = use_i64 ? (v1 << (v2 & 0x3F)) : (static_cast<int32_t>(v1) << (v2 & 0x1F));
                    break;
                case interpreter::IROpCode::Shr:
                    result = use_i64 ? (v1 >> (v2 & 0x3F)) : (static_cast<int32_t>(v1) >> (v2 & 0x1F));
                    break;
                case interpreter::IROpCode::ShrUn:
                    result = use_i64 ? (static_cast<uint64_t>(v1) >> (v2 & 0x3F))
                                     : (static_cast<uint32_t>(static_cast<int32_t>(v1)) >> (v2 & 0x1F));
                    break;
                case interpreter::IROpCode::Ceq:
                    result = (v1 == v2) ? 1 : 0;
                    use_i64 = false;
                    break;
                case interpreter::IROpCode::Clt:
                    result = (v1 < v2) ? 1 : 0;
                    use_i64 = false;
                    break;
                case interpreter::IROpCode::Cgt:
                    result = (v1 > v2) ? 1 : 0;
                    use_i64 = false;
                    break;

                // Unary
                case interpreter::IROpCode::Neg:
                    result = use_i64 ? -v1 : -static_cast<int32_t>(v1);
                    break;
                case interpreter::IROpCode::Not:
                    result = use_i64 ? ~v1 : ~static_cast<int32_t>(v1);
                    break;

                // Conversions
                case interpreter::IROpCode::Conv_I4:
                    result = static_cast<int32_t>(v1);
                    use_i64 = false;
                    break;
                case interpreter::IROpCode::Conv_I8:
                    result = v1;
                    use_i64 = true;
                    break;
                case interpreter::IROpCode::ConvI:
                    result = static_cast<int32_t>(v1);
                    use_i64 = false;
                    break;
                case interpreter::IROpCode::ConvU:
                    result = static_cast<uint32_t>(v1);
                    use_i64 = false;
                    break;
                case interpreter::IROpCode::ConvRUn:
                    // Fold to LdcI4 (the truncated int result)
                    result = static_cast<int32_t>(v1);
                    use_i64 = false;
                    break;

                // LdLen — can't fold at linear level (runtime value)
                case interpreter::IROpCode::LdLen:
                    foldable = false;
                    break;

                default:
                    foldable = false;
                    break;
            }

            if (!foldable)
                continue;

            // Replace with LdcI4 or LdcI8
            if (use_i64) {
                ri.header = (ri.header & ~(0xFFFFull)) | static_cast<uint64_t>(interpreter::IROpCode::LdcI8);
                ri.imm.i8 = result;
                const_vals[dst] = {kInt64, result};
            } else {
                ri.header = (ri.header & ~(0xFFFFull)) | static_cast<uint64_t>(interpreter::IROpCode::LdcI4);
                ri.imm.i4 = static_cast<int32_t>(result);
                const_vals[dst] = {kInt32, result};
            }
            // Clear src flags (LdcI4/I8 has no srcs)
            ri.header &= ~(static_cast<uint64_t>(0xFF) << 24); // clear src1
            ri.header &= ~(static_cast<uint64_t>(0xFF) << 32); // clear src2
            ri.header &= ~(static_cast<uint64_t>(interpreter::kRegHasSrc1 | interpreter::kRegHasSrc2) << 40);

            changed = true;
            any_propagated = true;
        }

        if (!changed)
            break;
    }

    return any_propagated;
}

// ── OptimizeWithTreeIR ─────────────────────────────────────────────────

bool OptimizeWithTreeIR(const std::vector<interpreter::RegisterInstruction>& instrs,
                        std::vector<interpreter::RegisterInstruction>& out_instrs, bool has_seh, uint32_t max_vreg,
                        bool enable_inlining, InlineResultBuffer* inline_results) noexcept {
    uint32_t n = static_cast<uint32_t>(instrs.size());
    if (n == 0)
        return false;

    if (has_seh)
        return false;

    if (max_vreg == 0) {
        for (const auto& ri : instrs) {
            if (ri.has_dst() && ri.dst_reg() > max_vreg)
                max_vreg = ri.dst_reg();
        }
        max_vreg += 1;
    }

    auto bbs = FindBasicBlocks(instrs.data(), n);
    if (bbs.empty())
        return false;

    // [P5] Phase 1: Build CFG for multi-BB methods
    LoopAnalysis loop_analysis = {};
    if (bbs.size() > 1) {
        loop_analysis = BuildCfg(bbs, instrs.data());
    }

    // Track per-BB output ranges in out_instrs
    std::vector<uint32_t> bb_starts(bbs.size(), UINT32_MAX);

    bool any_optimized = false;

    for (uint32_t bi = 0; bi < bbs.size(); ++bi) {
        const auto& bb = bbs[bi];
        uint32_t bb_len = bb.hi - bb.lo;

        // Record start of this BB's output
        bb_starts[bi] = static_cast<uint32_t>(out_instrs.size());

        if (bb_len <= 2) {
            for (uint32_t i = bb.lo; i < bb.hi; ++i)
                out_instrs.push_back(instrs[i]);
            continue;
        }

        TreeBuilder builder;
        auto result = builder.Build(instrs.data(), bb.lo, bb.hi);

        if (!result.first_node || result.root_count == 0) {
            for (uint32_t i = bb.lo; i < bb.hi; ++i)
                out_instrs.push_back(instrs[i]);
            continue;
        }

        // Inline eligible kCall nodes
        if (enable_inlining) {
            Inliner inliner(InlineConfig {}, 0, max_vreg);
            // Set loop nesting depth for loop-aware inline cost model
            uint32_t bb_loop_depth = loop_analysis.blocks.empty() ? 0 : loop_analysis.blocks[bi].loop_depth;
            inliner.set_bb_loop_depth(bb_loop_depth);
            inliner.InlineRoots(result.roots, result.root_count, 128u);
            if (inliner.new_max_vreg() > max_vreg)
                max_vreg = inliner.new_max_vreg();

            if (inline_results) {
                for (uint32_t ri = 0; ri < inliner.inlined_count(); ++ri) {
                    const auto& d = inliner.inlined_decisions()[ri];
                    inline_results->Add(d.callee_token, d.snapshot_version);
                }
            }
        }

        // Constant folding
        uint8_t* arena_pos = reinterpret_cast<uint8_t*>(result.first_node);
        uint8_t* arena_end = arena_pos + builder.kArenaSize;

        // Intrinsic expansion (after inlining, before const-folding)
        IntrinsicMutator intrinsic_mut(arena_pos, arena_end, kIntrinsicTable, kIntrinsicTableSize, &builder);
        for (uint32_t ri = 0; ri < result.root_count; ++ri)
            result.roots[ri] = intrinsic_mut.Mutate(result.roots[ri]);

        ConstFoldMutator fold_mut(arena_pos, arena_end);
        for (uint32_t ri = 0; ri < result.root_count; ++ri)
            result.roots[ri] = fold_mut.Mutate(result.roots[ri]);

        // Bounds check elimination (after const-fold, before CSE)
        BoundCheckEliminator bce_mut(arena_pos, arena_end, &builder);
        for (uint32_t ri = 0; ri < result.root_count; ++ri)
            result.roots[ri] = bce_mut.Mutate(result.roots[ri]);

        // CSE
        CSEMutator cse_mut(builder.VN());
        for (uint32_t ri = 0; ri < result.root_count; ++ri)
            result.roots[ri] = cse_mut.Mutate(result.roots[ri]);

        // Linearize
        Linearizer linearizer;
        linearizer.LinearizeRoots(result.roots, result.root_count, out_instrs);

        any_optimized = true;
    }

    // [P5] Phase 2: Post-passes on linearized instruction stream
    if (loop_analysis.has_loops) {
        // Build bb_ends from bb_starts
        std::vector<uint32_t> bb_ends(bbs.size());
        for (uint32_t bi = 0; bi < bbs.size(); ++bi) {
            if (bi + 1 < bbs.size())
                bb_ends[bi] = bb_starts[bi + 1];
            else
                bb_ends[bi] = static_cast<uint32_t>(out_instrs.size());
        }

        // Build vreg → defining block map (from original instrs, before
        // const-prop and LICM modify out_instrs)
        std::vector<uint32_t> vreg_def_blocks;
        BuildVRegDefBlocks(instrs, bbs, max_vreg, vreg_def_blocks);

        // 0. Constant propagation (before LICM — enables more hoisting)
        ConstPropagate(out_instrs, max_vreg);

        // 2a. LICM: hoist loop-invariant arithmetic out of loops
        LicmHoist(out_instrs, bb_starts, bb_ends, loop_analysis, vreg_def_blocks, max_vreg);

        // 2b. IV strength reduction
        IvStrengthReduce(out_instrs, bb_starts, bb_ends, loop_analysis, max_vreg);

        // 2c. Loop unrolling (factor 4, or 8 for very small bodies)
        {
            uint32_t factor = 4;
            // Estimate body size: scan loop headers for small bodies
            for (const auto& l : loop_analysis.loops) {
                if (l.blocks.size() == 1) {
                    uint32_t est = bb_ends[l.header] - bb_starts[l.header];
                    if (est < 8) {
                        factor = 8;
                        break;
                    }
                }
            }
            UnrollLoops(out_instrs, bb_starts, bb_ends, loop_analysis, max_vreg, factor);
        }
    }

    return any_optimized;
}

} // namespace chaos::il2cpp::jit::tree
