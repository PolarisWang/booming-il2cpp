// jit_optimizer.cpp — Tree IR optimization orchestration

#include "tree/jit_optimizer.h"
#include "tree/jit_tree_builder.h"
#include "tree/jit_tree_mutator.h"
#include "tree/jit_linearizer.h"

#include "interpreter/ir_reg_alloc.h"
#include "interpreter/generated/ir_opcodes.h"

#include <chaos/log.h>

#include <cstdint>
#include <vector>

namespace chaos::il2cpp::jit::tree {

// ── Arena allocation for per-BB optimization ──────────────────────────
// Each BB gets a fresh TreeBuilder with its own arena and VNTable.
// The arena is reset per-BB (TreeBuilder dtor frees memory).

bool OptimizeWithTreeIR(
    const std::vector<interpreter::RegisterInstruction>& instrs,
    std::vector<interpreter::RegisterInstruction>& out_instrs,
    bool has_seh) noexcept
{
    uint32_t n = static_cast<uint32_t>(instrs.size());
    if (n == 0) return false;

    // SEH methods: tree IR can't handle non-contiguous BB layout yet
    if (has_seh) return false;

    // Find basic block boundaries
    auto bbs = FindBasicBlocks(instrs.data(), n);
    if (bbs.empty()) return false;

    // Process each BB independently
    bool any_optimized = false;

    for (const auto& bb : bbs) {
        uint32_t bb_len = bb.hi - bb.lo;

        // Trivial BB (0-2 instructions): copy as-is, no tree overhead
        if (bb_len <= 2) {
            for (uint32_t i = bb.lo; i < bb.hi; ++i)
                out_instrs.push_back(instrs[i]);
            continue;
        }

        // Build tree for this BB
        TreeBuilder builder;
        auto result = builder.Build(instrs.data(), bb.lo, bb.hi);

        if (!result.first_node || result.root_count == 0) {
            // Tree build failed — fall back to original instructions
            for (uint32_t i = bb.lo; i < bb.hi; ++i)
                out_instrs.push_back(instrs[i]);
            continue;
        }

        // Apply constant folding
        uint8_t* arena_pos = reinterpret_cast<uint8_t*>(result.first_node);
        uint8_t* arena_end = arena_pos + builder.kArenaSize;  // approximate
        ConstFoldMutator fold_mut(arena_pos, arena_end);
        for (uint32_t ri = 0; ri < result.root_count; ++ri) {
            result.roots[ri] = fold_mut.Mutate(result.roots[ri]);
        }

        // Apply CSE
        CSEMutator cse_mut(builder.VN());
        for (uint32_t ri = 0; ri < result.root_count; ++ri) {
            result.roots[ri] = cse_mut.Mutate(result.roots[ri]);
        }

        // Linearize optimized tree back to RegisterInstructions
        Linearizer linearizer;
        linearizer.LinearizeRoots(result.roots, result.root_count, out_instrs);

        any_optimized = true;
    }

    // If optimization didn't actually change anything, we still use the
    // optimized output (it should be semantically equivalent).
    return any_optimized;
}

}  // namespace chaos::il2cpp::jit::tree
