// jit_tree_mutator.cpp — Constant folding + CSE mutators

#include "tree/jit_tree_mutator.h"
#include "tree/jit_tree_node.h"

namespace chaos::il2cpp::jit::tree {

// ── Constant folding helper ───────────────────────────────────────────
// Returns true if `node` is a LdcI4/LdcI8 leaf (compile-time constant).
static bool IsConstInt(const ExprNode* node, int64_t& out_val) noexcept {
    if (!node) return false;
    if (node->kind() == kLdcI4) {
        out_val = node->i4;
        return true;
    }
    if (node->kind() == kLdcI8) {
        out_val = node->i8;
        return true;
    }
    return false;
}

static int64_t FoldBinary(NodeKind kind, int64_t a, int64_t b) noexcept {
    switch (kind) {
        case kAdd:   return a + b;
        case kSub:   return a - b;
        case kMul:   return a * b;
        case kDiv:   return a / b;
        case kRem:   return a % b;
        case kAnd:   return a & b;
        case kOr:    return a | b;
        case kXor:   return a ^ b;
        case kShl:   return static_cast<int64_t>(
            static_cast<uint64_t>(a) << (static_cast<uint64_t>(b) & 0x3F));
        case kShr:   return a >> (static_cast<uint64_t>(b) & 0x3F);
        case kShrUn: return static_cast<int64_t>(
            static_cast<uint64_t>(a) >> (static_cast<uint64_t>(b) & 0x3F));
        case kCeq:   return (a == b) ? 1 : 0;
        case kClt:   return (a < b) ? 1 : 0;
        case kCgt:   return (a > b) ? 1 : 0;
        case kCltUn: return (static_cast<uint64_t>(a) < static_cast<uint64_t>(b)) ? 1 : 0;
        case kCgtUn: return (static_cast<uint64_t>(a) > static_cast<uint64_t>(b)) ? 1 : 0;
        default:     return 0;
    }
}

static int64_t FoldUnary(NodeKind kind, int64_t a) noexcept {
    switch (kind) {
        case kNeg:    return -a;
        case kNot:    return ~a;
        case kConvI4: return static_cast<int64_t>(static_cast<int32_t>(a));
        case kConvI8: return a;
        case kConvU4: return static_cast<int64_t>(static_cast<uint32_t>(a));
        case kConvU8: return a;
        default:      return 0;
    }
}

// ── ConstFoldMutator ───────────────────────────────────────────────────

ExprNode* ConstFoldMutator::PostVisit(ExprNode* node) noexcept {
    if (!node) return nullptr;

    // Only interested in binary and unary arithmetic nodes
    NodeKind k = node->kind();
    bool is_binary = (k >= kAdd && k <= kCgtUn);
    bool is_unary  = (k >= kNeg && k <= kConvU8);

    if (!is_binary && !is_unary) return node;

    if (is_binary) {
        int64_t v1, v2;
        if (IsConstInt(node->child0, v1) && IsConstInt(node->child1, v2)) {
            int64_t result = FoldBinary(k, v1, v2);

            // Replace this node with a LdcI4/LdcI8 leaf
            // (reuse the current node storage, just change kind/payload)
            uint32_t old_vn = node->vn_id();  // preserve for state tracking
            (void)old_vn;
            node->set_kind((k == kCeq || k == kClt || k == kCgt ||
                           k == kCltUn || k == kCgtUn) ? kLdcI4 : kLdcI8);
            node->type_tag = (k == kCeq || k == kClt || k == kCgt ||
                             k == kCltUn || k == kCgtUn) ? kInt32 : kInt64;
            node->child_count = 0;
            node->child0 = nullptr;
            node->child1 = nullptr;
            if (node->kind() == kLdcI4)
                node->i4 = static_cast<int32_t>(result);
            else
                node->i8 = result;
        }
    }

    if (is_unary) {
        int64_t v;
        if (IsConstInt(node->child0, v)) {
            int64_t result = FoldUnary(k, v);
            // kNot on a 32-bit input must truncate to 32 bits:
            // e.g. ~0 = 0xFFFFFFFF, not 0xFFFFFFFFFFFFFFFF.
            // The source node is still valid here (IsConstInt already
            // read it), so check if the input is a 32-bit constant.
            bool is_32bit_not = (k == kNot && node->child0 &&
                                 node->child0->kind() == kLdcI4);
            node->set_kind(is_32bit_not ? kLdcI4 : kLdcI8);
            node->type_tag = is_32bit_not ? kInt32 : kInt64;
            node->child_count = 0;
            node->child0 = nullptr;  // same union as i8/i4 — clear FIRST
            node->child1 = nullptr;
            if (is_32bit_not)
                node->i4 = static_cast<int32_t>(result);
            else
                node->i8 = result;
        }
    }

    return node;
}

// ── CSEMutator ─────────────────────────────────────────────────────────

ExprNode* CSEMutator::PostVisit(ExprNode* node) noexcept {
    if (!node) return nullptr;

    uint32_t vn = node->vn_id();
    if (vn == 0) return node;  // unnumbered — skip

    // Leaf nodes are always "available" — don't CSE them away
    // Only CSE internal computation nodes (unary/binary)
    NodeKind k = node->kind();
    bool is_computation = (k >= kNeg && k <= kCgtUn);
    if (!is_computation) {
        vn_.SetComputed(vn);
        return node;
    }

    if (vn_.IsComputed(vn)) {
        // This expression was already computed earlier.
        // Replace with a null reference (caller will handle by Dup).
        // We can't replace in-place here — the Linearizer will check
        // IsComputed and emit a copy instead of recomputing.
        return node;
    }

    vn_.SetComputed(vn);
    return node;
}

}  // namespace chaos::il2cpp::jit::tree
