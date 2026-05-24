// jit_intrinsics.cpp — Intrinsic method recognition and expansion implementation

#include "tree/jit_intrinsics.h"

#include <chaos/log.h>

namespace chaos::il2cpp::jit::tree {

// ── Helper: map SIMD IntrinsicId to SimdSubOperation ──────────────────
static uint8_t IntrinsicToSimdOp(IntrinsicId id) noexcept {
    switch (id) {
    case IntrinsicId::kVector128Add:     return kSimdAdd;
    case IntrinsicId::kVector128Sub:     return kSimdSub;
    case IntrinsicId::kVector128Mul:     return kSimdMul;
    case IntrinsicId::kVector128And:     return kSimdAnd;
    case IntrinsicId::kVector128Or:      return kSimdOr;
    case IntrinsicId::kVector128Xor:     return kSimdXor;
    case IntrinsicId::kVector128AndNot:  return kSimdAndNot;
    case IntrinsicId::kVector128Equals:  return kSimdEq;
    case IntrinsicId::kVector128Abs:     return kSimdAbs;
    case IntrinsicId::kVector128Shuffle: return kSimdShuffle;
    case IntrinsicId::kVector128Zero:    return kSimdZero;
    case IntrinsicId::kVector128Extract: return kSimdExtract;
    case IntrinsicId::kVector128Insert:  return kSimdInsert;
    default: return kSimdInvalid;
    }
}

/// Default element type for SIMD intrinsics.  In Phase 1 we infer from the
/// IntrinsicId context; a more precise implementation would read the generic
/// instantiation from metadata.
static uint8_t DefaultSimdElemType(IntrinsicId id) noexcept {
    (void)id;
    return kElemInt32;  // default for Phase 1
}

// ── Intrinsic table (Phase 1) ─────────────────────────────────────────
// Token values use sentinel 0xFFFFFFFF since actual metadata tokens vary
// per assembly. These must be updated with real tokens from the target
// benchmark assembly before intrinsic expansion takes effect.
//
// Phase 1 entries: Math.*, Array.Length, BitOperations.PopCount/LeadingZeroCount
const IntrinsicEntry kIntrinsicTable[] = {
    // Math.Abs(int32)
    { 0xFFFFFFFF, IntrinsicId::kMathAbsInt32,  kAbs,     1, kInt32 },
    // Math.Min(int32, int32)
    { 0xFFFFFFFF, IntrinsicId::kMathMinInt32,  kMin,     2, kInt32 },
    // Math.Max(int32, int32)
    { 0xFFFFFFFF, IntrinsicId::kMathMaxInt32,  kMax,     2, kInt32 },
    // Array.get_Length
    { 0xFFFFFFFF, IntrinsicId::kArrayLength,   kLdLen,   1, kInt32 },
    // BitOperations.PopCount (unary, GPR)
    { 0xFFFFFFFF, IntrinsicId::kBitOpsPopCount, kPopcnt, 1, kInt32 },
    // BitOperations.LeadingZeroCount (unary, GPR)
    { 0xFFFFFFFF, IntrinsicId::kBitOpsLeadingZeroCount, kLzcnt, 1, kInt32 },
    // Vector128<T>.Zero (static, no args → zero XMM)
    { 0xFFFFFFFF, IntrinsicId::kVector128Zero,  kSimd,   0, kInt32 },
};

const uint32_t kIntrinsicTableSize =
    sizeof(kIntrinsicTable) / sizeof(kIntrinsicTable[0]);

// ── IntrinsicMutator::Visit ────────────────────────────────────────────
//
// Algorithm:
//   1. Skip nodes that aren't kCall
//   2. Scan intrinsic table for method_token match
//   3. On match, extract arg expression nodes from vreg via TreeBuilder
//   4. Create specialized node (kAbs/kMin/kMax/kLdLen/kSimd/kPopcnt/kLzcnt)
//   5. Return new node (replaces the kCall in the tree)
ExprNode* IntrinsicMutator::Visit(ExprNode* node) noexcept {
    if (!node || node->kind() != kCall)
        return node;

    // ── Scan intrinsic table ───────────────────────────────────────────
    uint32_t token = node->call_method_token;
    const IntrinsicEntry* match = nullptr;

    for (uint32_t i = 0; i < table_size_; ++i) {
        const auto& entry = table_[i];
        if (entry.method_token == token ||
            entry.method_token == 0xFFFFFFFF) {  // wildcard (dev mode)
            if (node->arg_count == entry.arg_count) {
                match = &entry;
                break;
            }
        }
    }

    if (!match)
        return node;

    // ── Resolve argument expression nodes ──────────────────────────────
    ExprNode* arg0 = nullptr;
    ExprNode* arg1 = nullptr;

    uint32_t base_vreg = node->call_arg0_vreg;

    if (match->arg_count >= 1) {
        arg0 = builder_->FindVRegDef(base_vreg);
        if (!arg0) {
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
        result = Unary(arena_pos_, arena_end_, kAbs, arg0, kInt32);
        break;

    case IntrinsicId::kMathMinInt32:
        result = Binary(arena_pos_, arena_end_, kMin, arg0, arg1, kInt32);
        break;

    case IntrinsicId::kMathMaxInt32:
        result = Binary(arena_pos_, arena_end_, kMax, arg0, arg1, kInt32);
        break;

    case IntrinsicId::kArrayLength:
        result = Unary(arena_pos_, arena_end_, kLdLen, arg0, kInt32);
        break;

    case IntrinsicId::kBitOpsPopCount:
        result = Unary(arena_pos_, arena_end_, kPopcnt, arg0, kInt32);
        break;

    case IntrinsicId::kBitOpsLeadingZeroCount:
        result = Unary(arena_pos_, arena_end_, kLzcnt, arg0, kInt32);
        break;

    case IntrinsicId::kVector128Zero:
        // kSimdZero: no children, just the node with metadata
        result = AllocNode(arena_pos_, arena_end_, kSimd, kInt32);
        if (result)
            result->set_simd_meta(kSimdZero, DefaultSimdElemType(match->id), 0);
        break;

    case IntrinsicId::kVector128Add:
    case IntrinsicId::kVector128Sub:
    case IntrinsicId::kVector128Mul:
    case IntrinsicId::kVector128And:
    case IntrinsicId::kVector128Or:
    case IntrinsicId::kVector128Xor:
    case IntrinsicId::kVector128AndNot:
    case IntrinsicId::kVector128Equals:
    case IntrinsicId::kVector128Abs:
        // Binary/unary SIMD operation
        {
            uint8_t simd_op = IntrinsicToSimdOp(match->id);
            TypeTag result_tag = kInt32;  // most SIMD ops produce vector → stored as int
            if (match->arg_count >= 2) {
                result = Binary(arena_pos_, arena_end_, kSimd, arg0, arg1, result_tag);
            } else {
                result = Unary(arena_pos_, arena_end_, kSimd, arg0, result_tag);
            }
            if (result)
                result->set_simd_meta(simd_op, DefaultSimdElemType(match->id), 0);
        }
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
