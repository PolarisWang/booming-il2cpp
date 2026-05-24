// jit_optimizer.h — Optimization orchestration for tree IR
//
// Entry point: OptimizeWithTreeIR() replaces the existing linear
// OptimizeInstructions() for non-SEH methods.
//
// Pipeline per BB:
//   1. TreeBuilder::Build() — RegisterMethod → expression DAG
//   2. ConstFoldMutator — fold constant arithmetic
//   3. CSEMutator — eliminate redundant sub-expressions
//   4. Linearizer — optimized DAG → RegisterInstruction sequence
//
// For SEH methods, the optimizer falls back to the existing linear
// OptimizeInstructions (preserving SEH clause correctness).

#ifndef CHAOS_IL2CPP_JIT_OPTIMIZER_H_
#define CHAOS_IL2CPP_JIT_OPTIMIZER_H_

#include <cstdint>
#include <vector>

// Forward declarations
namespace chaos::il2cpp::interpreter {
struct RegisterInstruction;
}  // namespace chaos::il2cpp::interpreter

namespace chaos::il2cpp::jit::tree {

/// Optimize a RegisterInstruction sequence using tree IR passes.
/// For non-SEH methods: per-BB tree IR (ConstFold + CSE).
/// For SEH methods: result is empty (caller should use linear optimizer).
///
/// @param instrs     Input instruction sequence.
/// @param out_instrs Output: optimized instruction sequence.
/// @param has_seh    True if method contains SEH clauses.
/// @returns true if tree IR optimization was applied, false if caller
///          should fall back to linear optimizer.
bool OptimizeWithTreeIR(
    const std::vector<interpreter::RegisterInstruction>& instrs,
    std::vector<interpreter::RegisterInstruction>& out_instrs,
    bool has_seh) noexcept;

}  // namespace chaos::il2cpp::jit::tree

#endif  // CHAOS_IL2CPP_JIT_OPTIMIZER_H_
