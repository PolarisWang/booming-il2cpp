// jit_optimizer.h — Optimization orchestration for tree IR
//
// Entry point: OptimizeWithTreeIR() replaces the existing linear
// OptimizeInstructions() for non-SEH methods.
//
// Pipeline per BB:
//   1. TreeBuilder::Build() — RegisterMethod → expression DAG
//   2. Inliner::InlineRoots — inline eligible kCall nodes (optional)
//   3. ConstFoldMutator — fold constant arithmetic
//   4. CSEMutator — eliminate redundant sub-expressions
//   5. Linearizer — optimized DAG → RegisterInstruction sequence

#ifndef CHAOS_IL2CPP_JIT_OPTIMIZER_H_
#define CHAOS_IL2CPP_JIT_OPTIMIZER_H_

#include <cstdint>
#include <vector>

namespace chaos::il2cpp::interpreter {
struct RegisterInstruction;
}

namespace chaos::il2cpp::jit {

// Forward declaration for inlined callee info
struct InlineDecision;

/// Accumulated inline results from OptimizeWithTreeIR.
/// Holds a fixed-size snapshot of all inlined decisions across all BBs.
struct InlineResultBuffer {
    static constexpr uint32_t kMaxInlines = 32;

    uint32_t count = 0;
    uint32_t callee_tokens[kMaxInlines];
    uint32_t snapshot_versions[kMaxInlines];

    void Add(uint32_t token, uint32_t version) noexcept {
        if (count < kMaxInlines) {
            callee_tokens[count] = token;
            snapshot_versions[count] = version;
            count++;
        }
    }
};

namespace tree {

bool OptimizeWithTreeIR(const std::vector<interpreter::RegisterInstruction>& instrs,
                        std::vector<interpreter::RegisterInstruction>& out_instrs, bool has_seh, uint32_t max_vreg = 40,
                        bool enable_inlining = false, InlineResultBuffer* inline_results = nullptr) noexcept;

}

} // namespace chaos::il2cpp::jit

#endif
