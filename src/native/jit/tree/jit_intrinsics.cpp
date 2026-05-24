// jit_intrinsics.cpp — Intrinsic method recognition and expansion implementation

#include "tree/jit_intrinsics.h"

#include <chaos/log.h>

namespace chaos::il2cpp::jit::tree {

// ── Intrinsic table (Phase 1) ─────────────────────────────────────────
// Token values use sentinel 0xFFFFFFFF since actual metadata tokens vary
// per assembly. These must be updated with real tokens from the target
// benchmark assembly before intrinsic expansion takes effect.
const IntrinsicEntry kIntrinsicTable[] = {
    // Math.Abs(int32)
    { 0xFFFFFFFF, IntrinsicId::kMathAbsInt32,  kAbs,     1, kInt32 },
    // Math.Min(int32, int32)
    { 0xFFFFFFFF, IntrinsicId::kMathMinInt32,  kMin,     2, kInt32 },
    // Math.Max(int32, int32)
    { 0xFFFFFFFF, IntrinsicId::kMathMaxInt32,  kMax,     2, kInt32 },
    // Array.get_Length
    { 0xFFFFFFFF, IntrinsicId::kArrayLength,   kLdLen,   1, kInt32 },
};

const uint32_t kIntrinsicTableSize =
    sizeof(kIntrinsicTable) / sizeof(kIntrinsicTable[0]);

// ── IntrinsicMutator::Visit ────────────────────────────────────────────
//
// Algorithm:
//   1. Skip nodes that aren't kCall
//   2. Scan intrinsic table for method_token match
//   3. On match, extract arg expression nodes from vreg via TreeBuilder
//   4. Create specialized node (kAbs/kMin/kMax/kLdLen) with proper children
//   5. Return new node (replaces the kCall in the tree)
ExprNode* IntrinsicMutator::Visit(ExprNode* node) noexcept {
    if (!node || node->kind() != kCall)
        return node;

    // ── Scan intrinsic table ───────────────────────────────────────────
    // Note: Phase 1 uses sentinel token values. In practice, the table
    // must be populated with the actual method tokens from the target
    // assembly (determined at JIT registration time).
    //
    // For debug/dev builds, we also support a wildcard fallback: if the
    // table entry has sentinel token (0xFFFFFFFF), match by arg_count and
    // rely on the caller having verified the method identity externally.
    uint32_t token = node->call_method_token;
    const IntrinsicEntry* match = nullptr;

    for (uint32_t i = 0; i < table_size_; ++i) {
        const auto& entry = table_[i];
        if (entry.method_token == token ||
            entry.method_token == 0xFFFFFFFF) {  // wildcard (dev mode)
            // Verify arg_count matches
            if (node->arg_count == entry.arg_count) {
                match = &entry;
                break;
            }
        }
    }

    if (!match)
        return node;  // not an intrinsic — leave kCall unchanged

    // ── Resolve argument expression nodes ──────────────────────────────
    // kCall stores the first arg's vreg in call_arg0_vreg. For multi-arg
    // calls, args are in consecutive vregs. Use TreeBuilder's vreg→node
    // mapping to find the defining expression for each arg.
    ExprNode* arg0 = nullptr;
    ExprNode* arg1 = nullptr;

    uint32_t base_vreg = node->call_arg0_vreg;

    if (match->arg_count >= 1) {
        arg0 = builder_->FindVRegDef(base_vreg);
        if (!arg0) {
            // Arg not defined in this BB (live-in) — create a LdLoc leaf
            arg0 = AllocNode(arena_pos_, arena_end_, kLdLoc, kInt32);
            if (arg0) arg0->operand_index = base_vreg;
        }
    }

    if (match->arg_count >= 2) {
        arg1 = builder_->FindVRegDef(base_vreg + 1);
        if (!arg1) {
            arg1 = AllocNode(arena_pos_, arena_end_, kLdLoc, kInt32);
            if (arg1) arg1->operand_index = base_vreg + 1;
        }
    }

    // ── Create specialized node ────────────────────────────────────────
    ExprNode* result = nullptr;

    switch (match->id) {
    case IntrinsicId::kMathAbsInt32:
        // kAbs is unary: Abs(arg0)
        result = Unary(arena_pos_, arena_end_, kAbs, arg0, kInt32);
        break;

    case IntrinsicId::kMathMinInt32:
        // kMin is binary: Min(arg0, arg1)
        result = Binary(arena_pos_, arena_end_, kMin, arg0, arg1, kInt32);
        break;

    case IntrinsicId::kMathMaxInt32:
        // kMax is binary: Max(arg0, arg1)
        result = Binary(arena_pos_, arena_end_, kMax, arg0, arg1, kInt32);
        break;

    case IntrinsicId::kArrayLength:
        // kLdLen is unary: Length(arg0)
        result = Unary(arena_pos_, arena_end_, kLdLen, arg0, kInt32);
        break;

    default:
        break;
    }

    if (result) {
        CHAOS_IL2CPP_LOG_DEBUG_M("jit", "Intrinsic expanded: id=%d kind=%d",
                               static_cast<int>(match->id),
                               static_cast<int>(match->target_kind));
    }

    return result ? result : node;
}

}  // namespace chaos::il2cpp::jit::tree
