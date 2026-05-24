// jit_optimizer.cpp — Tree IR optimization orchestration

#include "tree/jit_optimizer.h"
#include "tree/jit_tree_builder.h"
#include "tree/jit_tree_mutator.h"
#include "tree/jit_linearizer.h"
#include "tree/jit_intrinsics.h"
#include "jit_inline.h"

#include "interpreter/ir_reg_alloc.h"
#include "interpreter/generated/ir_opcodes.h"

#include <chaos/log.h>

#include <cstdint>
#include <vector>

namespace chaos::il2cpp::jit::tree {

bool OptimizeWithTreeIR(
    const std::vector<interpreter::RegisterInstruction>& instrs,
    std::vector<interpreter::RegisterInstruction>& out_instrs,
    bool has_seh,
    uint32_t max_vreg,
    bool enable_inlining,
    InlineResultBuffer* inline_results) noexcept
{
    uint32_t n = static_cast<uint32_t>(instrs.size());
    if (n == 0) return false;

    if (has_seh) return false;

    if (max_vreg == 0) {
        for (const auto& ri : instrs) {
            if (ri.has_dst() && ri.dst_reg() > max_vreg)
                max_vreg = ri.dst_reg();
        }
        max_vreg += 1;
    }

    auto bbs = FindBasicBlocks(instrs.data(), n);
    if (bbs.empty()) return false;

    bool any_optimized = false;

    for (const auto& bb : bbs) {
        uint32_t bb_len = bb.hi - bb.lo;

        if (bb_len <= 2) {
            for (uint32_t i = bb.lo; i < bb.hi; ++i)
                out_instrs.push_back(instrs[i]);
            continue;
        }

        TreeBuilder builder;
        auto result = builder.Build(instrs.data(), bb.lo, bb.hi);

        if (!result.first_node || result.root_count == 0) {
            for (uint32_t i = bb.lo; i < bb.hi; ++i)
                out_instrs.push_back(instrs[i]);
            continue;
        }

        // Inline eligible kCall nodes
        if (enable_inlining) {
            Inliner inliner(InlineConfig{}, 0, max_vreg);
            inliner.InlineRoots(result.roots, result.root_count, 128u);
            if (inliner.new_max_vreg() > max_vreg)
                max_vreg = inliner.new_max_vreg();

            // Accumulate inlined callee info for JitMethod population
            if (inline_results) {
                for (uint32_t ri = 0; ri < inliner.inlined_count(); ++ri) {
                    const auto& d = inliner.inlined_decisions()[ri];
                    inline_results->Add(d.callee_token, d.snapshot_version);
                }
            }
        }

        // Constant folding
        uint8_t* arena_pos = reinterpret_cast<uint8_t*>(result.first_node);
        uint8_t* arena_end = arena_pos + builder.kArenaSize;

        // Intrinsic expansion (after inlining, before const-folding)
        IntrinsicMutator intrinsic_mut(arena_pos, arena_end,
                                        kIntrinsicTable, kIntrinsicTableSize,
                                        &builder);
        for (uint32_t ri = 0; ri < result.root_count; ++ri)
            result.roots[ri] = intrinsic_mut.Mutate(result.roots[ri]);

        ConstFoldMutator fold_mut(arena_pos, arena_end);
        for (uint32_t ri = 0; ri < result.root_count; ++ri)
            result.roots[ri] = fold_mut.Mutate(result.roots[ri]);

        // CSE
        CSEMutator cse_mut(builder.VN());
        for (uint32_t ri = 0; ri < result.root_count; ++ri)
            result.roots[ri] = cse_mut.Mutate(result.roots[ri]);

        // Linearize
        Linearizer linearizer;
        linearizer.LinearizeRoots(result.roots, result.root_count, out_instrs);

        any_optimized = true;
    }

    return any_optimized;
}

}  // namespace chaos::il2cpp::jit::tree
