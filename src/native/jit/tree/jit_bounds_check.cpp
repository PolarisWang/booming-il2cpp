// jit_bounds_check.cpp — Bounds check elimination pass implementation
//
// BoundCheckEliminator::PostVisit examines array access nodes (kLdElem,
// kLdElemA, kStElem) and marks them with kFlagNoBoundsCheck when the
// index is a compile-time constant within known array bounds.

#include "tree/jit_bounds_check.h"

#include <cstdint>

namespace chaos::il2cpp::jit::tree {

ExprNode* BoundCheckEliminator::PostVisit(ExprNode* node) noexcept {
    if (!node) return nullptr;

    NodeKind k = node->kind();
    if (k != kLdElem && k != kLdElemA && k != kStElem)
        return node;

    // ── Extract the index expression node ───────────────────────────────
    // For LdElem/LdElemA: child1 is the index expression (tree node).
    // For StElem:        index is stored as a vreg number in operand_index;
    //                    look up its defining node via the builder.
    ExprNode* idx_node = nullptr;
    if (k == kStElem) {
        idx_node = builder_->FindVRegDef(node->operand_index);
    } else {
        idx_node = node->child1;
    }

    if (!idx_node) return node;

    // ── Constant index elimination ──────────────────────────────────────
    // If the index is LdcI4 and the array was allocated by NewArr with a
    // known constant size, check const_index < known_size.
    // The LdLen → LdcI4 folding happens at TreeBuilder::Build() time via
    // NewArr constant-size propagation, so array.Length on a known-size
    // NewArr is already a constant node.
    if (idx_node->kind() == kLdcI4) {
        int32_t const_index = idx_node->i4;
        if (const_index >= 0) {
            // Extract array vreg from child0 (which should be a LdLoc leaf
            // referencing the array variable).
            ExprNode* arr_node = node->child0;
            uint32_t arr_vreg = UINT32_MAX;
            if (arr_node && arr_node->kind() == kLdLoc) {
                arr_vreg = arr_node->operand_index;
            }

            if (arr_vreg < 64) {
                uint32_t known_size = builder_->GetNewArrConstantSize(arr_vreg);
                if (known_size > 0 && static_cast<uint32_t>(const_index) < known_size) {
                    node->flags |= kFlagNoBoundsCheck;
                }
            }
        }
    }

    // ── NULL coalescing ─────────────────────────────────────────────────
    // Track array vregs that have been proven non-null within this BB.
    // When the same array vreg appears in multiple accesses, subsequent
    // accesses can skip the null check.
    //
    // In P4 this is informational (tracked_nonnull_vregs_ tracking).
    // The kFlagNonNullArray flag is set for future use; the NoCheck
    // variants already skip null checks when kFlagNoBoundsCheck is set.
    ExprNode* arr_node = node->child0;
    if (arr_node && arr_node->kind() == kLdLoc) {
        uint32_t arr_vreg = arr_node->operand_index;
        if (arr_vreg < 64) {
            if (tracked_nonnull_vregs_[arr_vreg]) {
                node->flags |= kFlagNonNullArray;
            }
            tracked_nonnull_vregs_[arr_vreg] = true;
        }
    }

    return node;
}

}  // namespace chaos::il2cpp::jit::tree
