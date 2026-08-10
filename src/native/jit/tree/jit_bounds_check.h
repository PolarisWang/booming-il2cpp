// jit_bounds_check.h — Bounds check elimination pass for tree IR
//
// BoundCheckEliminator is a Mutator pass that runs after constant folding
// and before CSE.  It identifies array accesses where the index is a
// compile-time constant within the known array bounds (from NewArr constant
// size propagation), and marks them with kFlagNoBoundsCheck so the
// Linearizer can emit NoChk variant opcodes that skip bounds/null checks.
//
// Algorithm:
//   1. Constant index: if the index node is LdcI4 and the array vreg was
//      defined by NewArr with known constant size, compare index < size.
//   2. NULL coalescing: track array vregs that have been proven non-null
//      within the current BB (for future use; P4 is informational only).

#ifndef CHAOS_IL2CPP_JIT_BOUNDS_CHECK_H_
#define CHAOS_IL2CPP_JIT_BOUNDS_CHECK_H_

#include "tree/jit_tree_node.h"
#include "tree/jit_tree_mutator.h"
#include "tree/jit_tree_builder.h"

#include <cstdint>

namespace chaos::il2cpp::jit::tree {

/// Bounds check elimination mutator.
///
/// Pipeline position: after ConstFoldMutator, before CSEMutator.
/// This ordering ensures constant indices have already been folded to
/// LdcI4 nodes before BCE examines them.
class BoundCheckEliminator : public Mutator {
public:
    BoundCheckEliminator(uint8_t*& arena_pos, uint8_t* arena_end, const TreeBuilder* builder) noexcept
        : arena_pos_(arena_pos), arena_end_(arena_end), builder_(builder) {}

protected:
    /// Post-order: examine array access nodes for eliminatable bounds checks.
    ExprNode* PostVisit(ExprNode* node) noexcept override;

private:
    uint8_t*& arena_pos_;
    uint8_t* arena_end_;
    const TreeBuilder* builder_;

    /// Per-BB tracked non-null array vregs (persists across root traversals).
    bool tracked_nonnull_vregs_[64] = {};
};

} // namespace chaos::il2cpp::jit::tree

#endif // CHAOS_IL2CPP_JIT_BOUNDS_CHECK_H_
