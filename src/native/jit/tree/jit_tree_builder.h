// jit_tree_builder.h — Build expression tree DAG from RegisterInstruction BB
//
// Algorithm:
//   1. Forward scan: process each RegisterInstruction.  For definers (dst_reg),
//      create ExprNode with child pointers resolved from the vreg→node map.
//      For uses without a prior def, create LdLoc leaf nodes (BB parameters /
//      live-in values).
//   2. VN assignment pass: walk nodes in creation order, assign VN IDs via
//      VNTable (GetOrCreate).  Duplicate expressions receive the same VN.
//   3. Collect roots: side-effect nodes (stores, calls) and branch sources.
//
// Memory: all nodes allocated from a bump arena (16KB default, auto-grow).

#ifndef CHAOS_IL2CPP_JIT_TREE_BUILDER_H_
#define CHAOS_IL2CPP_JIT_TREE_BUILDER_H_

#include "tree/jit_tree_node.h"
#include "tree/jit_vn.h"

#include <cstdint>
#include <cstddef>
#include <vector>

#include <chaos/unordered_dense.h>

// Forward declaration of RegisterInstruction from interpreter module.
namespace chaos::il2cpp::interpreter {
struct RegisterInstruction;
struct RegisterMethod;
}  // namespace chaos::il2cpp::interpreter

namespace chaos::il2cpp::jit::tree {

/// Result of building a tree for a basic block.
struct TreeBuildResult {
    ExprNode** roots;        // array of root nodes (side-effects + branch sources)
    uint32_t   root_count;   // number of roots
    ExprNode*  first_node;   // first allocated node (for arena reset)
    uint8_t*   arena;        // arena pointer (for freeing)
};

/// Basic block range: [lo, hi) instruction indices.
struct BBRange {
    uint32_t lo;
    uint32_t hi;
};

/// Tree builder for a single basic block.
class TreeBuilder {
public:
    /// Maximum node count per BB (safety limit).
    static constexpr uint32_t kMaxNodes = 4096;

    /// Default arena size per BB.
    static constexpr uint32_t kArenaSize = 16 * 1024;  // 16KB

    TreeBuilder() noexcept;
    ~TreeBuilder() noexcept;

    TreeBuilder(const TreeBuilder&) = delete;
    TreeBuilder& operator=(const TreeBuilder&) = delete;

    /// Build an expression DAG from instruction range [lo, hi).
    /// Roots are side-effect nodes (calls, stores) and the branch source
    /// (if the BB ends with a conditional branch).
    /// Returns empty result on allocation failure.
    TreeBuildResult Build(const interpreter::RegisterInstruction* instrs,
                          uint32_t lo, uint32_t hi) noexcept;

    /// Convenience: build for a RegisterMethod's full instruction span.
    TreeBuildResult Build(const interpreter::RegisterMethod& rm,
                          uint32_t lo, uint32_t hi) noexcept;

    /// Access the VNTable (for CSE mutator to query computed flags).
    VNTable& VN() noexcept { return vn_; }

    /// Find the defining ExprNode for a vreg (nullptr if unknown/not defined in BB).
    ExprNode* FindVRegDef(uint32_t vreg) const noexcept {
        return (vreg < 64) ? vreg_to_node_[vreg] : nullptr;
    }

    /// Get the constant size of a NewArr-defined vreg (0 = unknown/variable-size).
    uint32_t GetNewArrConstantSize(uint32_t vreg) const noexcept {
        return (vreg < 64) ? newarr_constant_size_[vreg] : 0;
    }

private:
    /// Ensure arena has at least `bytes` free.
    bool EnsureArena(uint32_t bytes) noexcept;

    /// Allocate a single ExprNode from the arena.
    ExprNode* AllocNode(NodeKind kind, TypeTag tag,
                         ExprNode* c0 = nullptr,
                         ExprNode* c1 = nullptr) noexcept;

    /// Look up or create a leaf node for a vreg.
    /// If the vreg has a defining node in the current BB, returns it.
    /// Otherwise creates a LdLoc leaf (BB parameter / live-in).
    ExprNode* ResolveVReg(uint32_t vreg, uint8_t type_tag) noexcept;

    uint8_t* arena_ = nullptr;
    uint8_t* arena_pos_ = nullptr;
    uint8_t* arena_end_ = nullptr;
    uint32_t arena_size_ = kArenaSize;

    // vreg → defining ExprNode (kGPRegisters = 64 entries)
    ExprNode* vreg_to_node_[64] = {};

    // NewArr constant-size tracking: vreg → known array size (0 = unknown)
    uint32_t newarr_constant_size_[64] = {};

    // VN table for the current BB
    VNTable vn_;

    // Roots collected during building
    static constexpr uint32_t kMaxRoots = 128;
    ExprNode* roots_[kMaxRoots];
    uint32_t  root_count_ = 0;

    // When true, Build() returns an empty result (triggers linear fallback).
    // Set when the instruction stream contains operations that the tree IR
    // builder doesn't yet support (StFld, StSFld, etc.).
    bool has_unsupported_ = false;
};

/// Analyze instruction sequence to find basic block boundaries.
/// Returns a vector of [lo, hi) ranges.
/// A BB ends at: Br, BrTrue, BrFalse, Ret, Throw, Rethrow, Switch, Leave.
/// Call does NOT end a BB (single-BB may contain calls).
std::vector<BBRange> FindBasicBlocks(
    const interpreter::RegisterInstruction* instrs,
    uint32_t count) noexcept;

}  // namespace chaos::il2cpp::jit::tree

#endif  // CHAOS_IL2CPP_JIT_TREE_BUILDER_H_
