// jit_tree_mutator.h — Tree traversal and transformation base class
//
// Mutator follows the Visitor pattern:
//   - Visit(node)   — pre-order (called before children)
//   - PostVisit(node) — post-order (called after children)
//
// Each subclass overrides one or both to implement tree transformations.
// The return value is the (possibly replaced) node.

#ifndef CHAOS_IL2CPP_JIT_TREE_MUTATOR_H_
#define CHAOS_IL2CPP_JIT_TREE_MUTATOR_H_

#include "tree/jit_tree_node.h"
#include "tree/jit_vn.h"

namespace chaos::il2cpp::jit::tree {

/// Base class for tree mutators (visitors).
/// Subclasses override Visit() and/or PostVisit().
class Mutator {
public:
    virtual ~Mutator() = default;

    /// Mutate a single root node, returning the (possibly replaced) node.
    /// Calls Visit() pre-order, recurses into children, then PostVisit().
    ExprNode* Mutate(ExprNode* node) noexcept {
        if (!node) return nullptr;

        node = Visit(node);
        if (!node) return nullptr;

        // Recurse into children
        if (node->child_count >= 1 && node->child0)
            node->child0 = Mutate(node->child0);
        if (node->child_count >= 2 && node->child1)
            node->child1 = Mutate(node->child1);

        return PostVisit(node);
    }

protected:
    /// Pre-order hook.  Return the (possibly replaced) node.
    /// Default: return node unchanged.
    virtual ExprNode* Visit(ExprNode* node) noexcept {
        (void)node;
        return node;
    }

    /// Post-order hook.  Return the (possibly replaced) node.
    /// Default: return node unchanged.
    virtual ExprNode* PostVisit(ExprNode* node) noexcept {
        (void)node;
        return node;
    }
};

// ── Constant folding mutator ──────────────────────────────────────────
// Post-order: if a binary/unary node has only constant children, fold it
// to a LdcI4/LdcI8 leaf.
class ConstFoldMutator : public Mutator {
public:
    explicit ConstFoldMutator(uint8_t*& arena_pos, uint8_t* arena_end) noexcept
        : arena_pos_(arena_pos), arena_end_(arena_end) {}

protected:
    ExprNode* PostVisit(ExprNode* node) noexcept override;

private:
    uint8_t*& arena_pos_;
    uint8_t*  arena_end_;
};

// ── CSE mutator ────────────────────────────────────────────────────────
// Post-order: if a node's VN is already computed (IsComputed), replace it
// with a Dup reference.  Otherwise, mark VN as computed.
class CSEMutator : public Mutator {
public:
    explicit CSEMutator(VNTable& vn) noexcept : vn_(vn) {}

protected:
    ExprNode* PostVisit(ExprNode* node) noexcept override;

private:
    VNTable& vn_;
};

}  // namespace chaos::il2cpp::jit::tree

#endif  // CHAOS_IL2CPP_JIT_TREE_MUTATOR_H_
