// jit_cfg.h — Control flow graph, dominator tree, and natural loop detection
//
// BuildCfg() converts basic-block ranges into a control flow graph with
// predecessor/successor edges, computes the immediate dominator tree
// (Lengauer-Tarjan), and identifies natural loops from back-edges.
//
// Usage:
//   auto bbs = FindBasicBlocks(instrs.data(), n);
//   auto cfg = BuildCfg(bbs, instrs.data());
//   if (cfg.has_loops) { /* process cfg.loops */ }

#ifndef CHAOS_IL2CPP_JIT_CFG_H_
#define CHAOS_IL2CPP_JIT_CFG_H_

#include "tree/jit_tree_builder.h"  // BBRange

#include <cstdint>
#include <vector>

namespace chaos::il2cpp::interpreter {
struct RegisterInstruction;
}

namespace chaos::il2cpp::jit::tree {

/// A single basic block node in the control flow graph.
struct CfgBlock {
    uint32_t lo = 0;            // instruction index range [lo, hi)
    uint32_t hi = 0;
    uint32_t id = 0;            // block index in the blocks_ array
    int32_t  idom = -1;         // immediate dominator (-1 = entry / no idom)
    uint32_t loop_depth = 0;    // nesting depth (0 = outside any loop)
    bool     visited = false;   // for DFS / loop detection

    std::vector<uint32_t> preds;  // predecessor block IDs
    std::vector<uint32_t> succs;  // successor block IDs
};

/// A natural loop identified from back-edge analysis.
struct NaturalLoop {
    uint32_t              header = 0;       // loop header block ID
    uint32_t              back_edge_from = 0; // source of the back-edge
    std::vector<uint32_t> blocks;            // all blocks belonging to this loop
    uint32_t              depth = 0;         // nesting depth (0 = outermost)
};

/// Result of CFG analysis: graph, dominator tree, and natural loops.
struct LoopAnalysis {
    std::vector<CfgBlock>   blocks;
    std::vector<NaturalLoop> loops;
    bool                    has_loops = false;

    /// True if block_id belongs to at least one natural loop.
    /// If so, out_loop_idx is set to the innermost loop index.
    bool IsInLoop(uint32_t block_id, uint32_t& out_loop_idx) const noexcept {
        for (uint32_t i = loops.size(); i > 0; --i) {
            uint32_t li = i - 1;
            const auto& loop = loops[li];
            for (uint32_t b : loop.blocks) {
                if (b == block_id) {
                    out_loop_idx = li;
                    return true;
                }
            }
        }
        return false;
    }

    /// True if vreg is loop-invariant with respect to the given loop:
    /// the vreg's defining instruction originates from a block outside
    /// the loop (dominated by the loop pre-header).  def_blocks maps
    /// vreg → defining block id (UINT32_MAX if unknown/variable).
    bool IsLoopInvariant(uint32_t vreg, uint32_t loop_idx,
                         const uint32_t* def_blocks,
                         uint32_t def_block_count) const noexcept {
        if (vreg >= def_block_count) return false;
        uint32_t def_block = def_blocks[vreg];
        if (def_block == UINT32_MAX) return false;

        const auto& loop = loops[loop_idx];
        for (uint32_t b : loop.blocks) {
            if (b == def_block) return false;  // defined inside the loop
        }
        return true;  // defined outside the loop → invariant
    }
};

/// Build CFG from basic block ranges.
///
/// Scans the terminator instruction of each BB to determine successor blocks:
///   - {Br, BrTrue, BrFalse, Beq, BneUn, ...} → branch_target as succ
///   - Conditional branches also have a fall-through succ (next BB)
///   - {Ret, Throw, Rethrow, EndFinally, EndFilter} → no succs
///   - Switch → reads switch target table from imm.ptr
///   - Non-terminator blocks (fall-through) → next BB as succ
///
/// After building the graph, computes the immediate dominator tree using
/// the Lengauer-Tarjan algorithm, then identifies natural loops from
/// back-edges in the dominator tree.
LoopAnalysis BuildCfg(const std::vector<BBRange>& bbs,
                      const interpreter::RegisterInstruction* instrs) noexcept;

}  // namespace chaos::il2cpp::jit::tree

#endif  // CHAOS_IL2CPP_JIT_CFG_H_
