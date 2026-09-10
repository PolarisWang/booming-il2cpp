// jit_intrinsics.h — Intrinsic method recognition and expansion
//
// IntrinsicMutator recognizes known BCL method calls (Math.Abs/Min/Max,
// Array.Length) and replaces them with specialized tree nodes that the
// Linearizer can expand to inline x64 instruction sequences.
//
// Pipeline position: after Inliner, before ConstFold (in OptimizeWithTreeIR).

#ifndef CHAOS_IL2CPP_JIT_INTRINSICS_H_
#define CHAOS_IL2CPP_JIT_INTRINSICS_H_

#include "tree/jit_tree_node.h"
#include "tree/jit_tree_mutator.h"
#include "tree/jit_tree_builder.h"

#include <cstdint>

namespace chaos::il2cpp::jit::tree {

// ── Intrinsic identifiers ──────────────────────────────────────────────
enum class IntrinsicId : uint8_t {
    kNone = 0,
    kMathAbsInt32,
    kMathMinInt32,
    kMathMaxInt32,
    kArrayLength,
    // Vector128<T> operations
    kVector128Add,
    kVector128Sub,
    kVector128Mul,
    kVector128And,
    kVector128Or,
    kVector128Xor,
    kVector128AndNot,
    kVector128Equals,
    kVector128Shuffle,
    kVector128Abs,
    kVector128Negate,
    kVector128Zero,
    kVector128Create,
    kVector128CreateScalar,
    kVector128Extract,
    kVector128Insert,
    kVector128Load,
    kVector128Store,
    // BitOperations
    kBitOpsPopCount,
    kBitOpsLeadingZeroCount,
};

// ── Intrinsic table entry ──────────────────────────────────────────────
struct IntrinsicEntry {
    uint32_t     method_token;    // callee metadata token to match
    IntrinsicId  id;              // unique intrinsic identifier
    NodeKind     target_kind;     // target NodeKind after replacement
    uint8_t      arg_count;       // expected argument count
    TypeTag      result_tag;      // result type tag
    uint8_t      simd_elem_type;  // SIMD element type (SimdElementType), for kSimd targets
};

// ── IntrinsicMutator — Tree IR pass ────────────────────────────────────
//
// Replaces kCall nodes matching known intrinsic patterns with specialized
// tree nodes (kAbs, kMin, kMax, or kLdLen for Array.Length).
//
// The mutator accesses the TreeBuilder's vreg-to-node mapping to
// correctly wire argument expressions as children of the new nodes.
class IntrinsicMutator : public Mutator {
public:
    IntrinsicMutator(uint8_t*& arena_pos, uint8_t* arena_end,
                     const IntrinsicEntry* table, uint32_t table_size,
                     const TreeBuilder* builder) noexcept
        : table_(table), table_size_(table_size),
          arena_pos_(arena_pos), arena_end_(arena_end),
          builder_(builder) {}

protected:
    /// Pre-order: intercept kCall nodes matching intrinsic patterns.
    ExprNode* Visit(ExprNode* node) noexcept override;

private:
    const IntrinsicEntry* table_;
    uint32_t              table_size_;
    uint8_t*&             arena_pos_;
    uint8_t*              arena_end_;
    const TreeBuilder*    builder_;
};

// ── Built-in intrinsic table ───────────────────────────────────────────
// Phase 1: Math.Abs(int32), Math.Min(int32, int32), Math.Max(int32, int32),
//          and Array.Length.
//
// Token values are filled at JIT registration time since metadata tokens
// vary per assembly. The default table uses sentinel values (0xFFFFFFFF)
// which won't match any real token; callers must populate with actual
// tokens from the target assembly.
extern const IntrinsicEntry kIntrinsicTable[];
extern const uint32_t kIntrinsicTableSize;

}  // namespace chaos::il2cpp::jit::tree

#endif  // CHAOS_IL2CPP_JIT_INTRINSICS_H_
