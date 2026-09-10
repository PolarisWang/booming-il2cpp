// jit_linearizer.h — Optimized tree → RegisterInstruction sequence

#ifndef CHAOS_IL2CPP_JIT_LINEARIZER_H_
#define CHAOS_IL2CPP_JIT_LINEARIZER_H_

#include "tree/jit_tree_node.h"

#include <cstdint>
#include <cstring>
#include <vector>

// Forward declarations
namespace chaos::il2cpp::interpreter {
struct RegisterInstruction;
}  // namespace chaos::il2cpp::interpreter

namespace chaos::il2cpp::jit::tree {

/// Linearize an expression tree back into RegisterInstruction sequence.
///
/// Algorithm: DFS post-order traversal.
/// - Each expression node produces one RegisterInstruction that computes
///   its value into a freshly allocated vreg.
/// - Leaf nodes (LdcI4, LdLoc, LdArg) emit directly.
/// - Binary/unary nodes emit children first, then the operation.
/// - Store/root nodes (StLoc, StFld, Return) emit value child, then store.
/// - CSE duplicates: when a node has the same VN as a previously emitted
///   computation, emit a Dup instruction instead of recomputing.
///
/// Vreg allocation: uses high vreg numbers (starting at kBaseVReg) to avoid
/// conflicts with the original BB's vreg definitions (0..kBaseVReg-1).
class Linearizer {
public:
    static constexpr uint32_t kBaseVReg = 64;  // first vreg for tree intermediates

    Linearizer() noexcept;

    /// Linearize a tree root into `out_instrs`.
    /// Returns the vreg holding the root's value (0 if root is void/Store/Return).
    uint32_t Linearize(const ExprNode* root,
                       std::vector<interpreter::RegisterInstruction>& out_instrs) noexcept;

    /// Linearize multiple roots (side-effect root set from TreeBuilder).
    /// Roots are processed in order; each root's value (if any) is discarded.
    void LinearizeRoots(const ExprNode* const* roots, uint32_t root_count,
                        std::vector<interpreter::RegisterInstruction>& out_instrs) noexcept;

private:
    /// Internal recursive linearization.
    uint32_t LinearizeNode(const ExprNode* node,
                           std::vector<interpreter::RegisterInstruction>& out) noexcept;

    /// Emit a single RegisterInstruction helper.
    uint32_t EmitInstr(uint64_t header, int64_t imm,
                       std::vector<interpreter::RegisterInstruction>& out) noexcept;
    uint32_t EmitInstr(uint64_t header, int32_t imm,
                       std::vector<interpreter::RegisterInstruction>& out) noexcept;

    /// Check if a node is a CSE duplicate (computed VN already emitted).
    bool IsCseDuplicate(const ExprNode* node) const noexcept;

    /// Allocate next vreg.
    uint32_t NextVReg() noexcept { return next_vreg_++; }

    uint32_t next_vreg_ = kBaseVReg;

    /// Track emitted VNs for CSE duplicate detection.
    /// Bit i set = VN i has been emitted.
    static constexpr uint32_t kMaxVN = 4096;
    bool emitted_vn_[kMaxVN] = {};
};

}  // namespace chaos::il2cpp::jit::tree

#endif  // CHAOS_IL2CPP_JIT_LINEARIZER_H_
