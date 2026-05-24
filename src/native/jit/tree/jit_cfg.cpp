// jit_cfg.cpp — Control flow graph construction and natural loop detection
//
// Implements Lengauer-Tarjan dominator tree (O(N log N)) and back-edge based
// natural loop identification.

#include "tree/jit_cfg.h"
#include "interpreter/ir_reg_alloc.h"        // RegisterInstruction
#include "interpreter/generated/ir_opcodes.h"

#include <cstdint>
#include <cstring>
#include <limits>
#include <vector>

namespace chaos::il2cpp::jit::tree {

// ── Helpers: instruction index → BB index map ──────────────────────────
// Builds a lookup table: instruction_index → bb_index.
static std::vector<uint32_t> BuildInstToBbMap(
    const std::vector<BBRange>& bbs, uint32_t total_instrs) noexcept
{
    std::vector<uint32_t> map(total_instrs, UINT32_MAX);
    for (uint32_t bi = 0; bi < bbs.size(); ++bi) {
        for (uint32_t i = bbs[bi].lo; i < bbs[bi].hi; ++i)
            map[i] = bi;
    }
    return map;
}

// ── Determine successors for a basic block ─────────────────────────────
// Scans the terminator of the BB and fills succs with successor block IDs.
static void FindBlockSuccessors(
    uint32_t bb_id, const BBRange& bb,
    const interpreter::RegisterInstruction* instrs,
    const std::vector<uint32_t>& inst_to_bb,
    uint32_t total_bbs,
    std::vector<uint32_t>& succs) noexcept
{
    succs.clear();

    // Last instruction in the BB is the terminator (or fall-through).
    uint32_t last_idx = bb.hi - 1;
    auto opc = instrs[last_idx].op_code();
    const auto& ri = instrs[last_idx];

    switch (opc) {
    case interpreter::IROpCode::Br:
    case interpreter::IROpCode::Leave: {
        // Unconditional branch to branch_target
        uint32_t target = ri.imm.branch_target;
        if (target < inst_to_bb.size()) {
            uint32_t target_bb = inst_to_bb[target];
            if (target_bb != UINT32_MAX)
                succs.push_back(target_bb);
        }
        break;
    }

    case interpreter::IROpCode::BrTrue:
    case interpreter::IROpCode::BrFalse:
    case interpreter::IROpCode::Beq:
    case interpreter::IROpCode::BneUn:
    case interpreter::IROpCode::Blt:
    case interpreter::IROpCode::BltUn:
    case interpreter::IROpCode::Bgt:
    case interpreter::IROpCode::BgtUn:
    case interpreter::IROpCode::Ble:
    case interpreter::IROpCode::BleUn:
    case interpreter::IROpCode::Bge:
    case interpreter::IROpCode::BgeUn: {
        // Conditional branch: branch_target + fall-through to next BB
        uint32_t target = ri.imm.branch_target;
        if (target < inst_to_bb.size()) {
            uint32_t target_bb = inst_to_bb[target];
            if (target_bb != UINT32_MAX)
                succs.push_back(target_bb);
        }
        // Fall-through to next BB
        if (bb_id + 1 < total_bbs)
            succs.push_back(bb_id + 1);
        break;
    }

    case interpreter::IROpCode::Switch: {
        // Switch: read target table from imm.ptr (uint32_t array)
        // + fall-through to next BB (for out-of-range / default)
        if (ri.imm.ptr != nullptr) {
            auto* targets = static_cast<const uint32_t*>(ri.imm.ptr);
            // Read target count from first element (or use known convention)
            // The standard encoding: ptr points to [count, target1, target2, ...]
            uint32_t count = targets[0];
            for (uint32_t ti = 0; ti < count; ++ti) {
                uint32_t t = targets[1 + ti];
                if (t < inst_to_bb.size()) {
                    uint32_t tb = inst_to_bb[t];
                    if (tb != UINT32_MAX)
                        succs.push_back(tb);
                }
            }
        }
        // Fall-through to next BB (default case or out-of-range)
        if (bb_id + 1 < total_bbs)
            succs.push_back(bb_id + 1);
        break;
    }

    case interpreter::IROpCode::Ret:
    case interpreter::IROpCode::Throw:
    case interpreter::IROpCode::Rethrow:
    case interpreter::IROpCode::EndFinally:
    case interpreter::IROpCode::EndFilter:
        // No successors (exit blocks)
        break;

    default:
        // Not a terminator: fall-through to next BB
        if (bb_id + 1 < total_bbs)
            succs.push_back(bb_id + 1);
        break;
    }
}

// ── Lengauer-Tarjan dominator tree ─────────────────────────────────────
// Standard algorithm O(N log N).

struct LtData {
    uint32_t n;                     // number of blocks
    std::vector<uint32_t> semi;     // semi-dominator number
    std::vector<uint32_t> parent;   // DFS tree parent
    std::vector<uint32_t> ancestor; // union-find ancestor
    std::vector<uint32_t> best;     // vertex with minimal semi on path
    std::vector<uint32_t> idom;     // immediate dominator
    std::vector<uint32_t> dfs_order; // vertex → DFS number
    std::vector<uint32_t> vertex;    // DFS number → vertex
    std::vector<std::vector<uint32_t>> bucket; // semi-dominator buckets
    uint32_t dfs_clock = 0;

    explicit LtData(uint32_t n) noexcept
        : n(n), semi(n, UINT32_MAX), parent(n, UINT32_MAX),
          ancestor(n, UINT32_MAX), best(n, UINT32_MAX),
          idom(n, UINT32_MAX),
          dfs_order(n, UINT32_MAX), vertex(n, UINT32_MAX),
          bucket(n) {}
};

static void LtDfs(uint32_t v, const std::vector<CfgBlock>& blocks,
                  LtData& lt) noexcept
{
    lt.semi[v] = lt.dfs_clock;
    lt.dfs_order[v] = lt.dfs_clock;
    lt.vertex[lt.dfs_clock] = v;
    lt.dfs_clock++;

    for (uint32_t w : blocks[v].succs) {
        if (lt.dfs_order[w] == UINT32_MAX) {
            lt.parent[w] = v;
            LtDfs(w, blocks, lt);
        }
    }
}

static void LtCompress(uint32_t v, LtData& lt) noexcept {
    if (lt.ancestor[lt.ancestor[v]] != UINT32_MAX) {
        LtCompress(lt.ancestor[v], lt);
        if (lt.semi[lt.best[lt.ancestor[v]]] < lt.semi[lt.best[v]])
            lt.best[v] = lt.best[lt.ancestor[v]];
        lt.ancestor[v] = lt.ancestor[lt.ancestor[v]];
    }
}

static uint32_t LtEval(uint32_t v, LtData& lt) noexcept {
    if (lt.ancestor[v] == UINT32_MAX)
        return lt.best[v];
    LtCompress(v, lt);
    if (lt.semi[lt.best[lt.ancestor[v]]] >= lt.semi[lt.best[v]])
        return lt.best[v];
    return lt.best[lt.ancestor[v]];
}

static void LtLink(uint32_t v, uint32_t w, LtData& lt) noexcept {
    lt.ancestor[w] = v;
    lt.best[w] = w;
}

static void ComputeDominators(std::vector<CfgBlock>& blocks) noexcept {
    uint32_t n = static_cast<uint32_t>(blocks.size());
    if (n == 0) return;
    if (n == 1) {
        blocks[0].idom = -1;  // entry has no idom
        return;
    }

    LtData lt(n);

    // Step 1: DFS from entry (block 0)
    for (uint32_t i = 0; i < n; ++i)
        lt.best[i] = i;
    LtDfs(0, blocks, lt);

    // Step 2: Process vertices in reverse DFS order
    for (uint32_t i = lt.dfs_clock; i > 1; --i) {
        uint32_t w = lt.vertex[i - 1];
        // Compute semi-dominator from predecessors
        for (uint32_t v : blocks[w].preds) {
            if (lt.dfs_order[v] == UINT32_MAX) continue;
            uint32_t u = LtEval(v, lt);
            if (lt.semi[u] < lt.semi[w])
                lt.semi[w] = lt.semi[u];
        }
        lt.bucket[lt.vertex[lt.semi[w]]].push_back(w);
        LtLink(lt.parent[w], w, lt);

        // Step 3: Process bucket of parent
        for (uint32_t v : lt.bucket[lt.parent[w]]) {
            uint32_t u = LtEval(v, lt);
            lt.idom[v] = (lt.semi[u] < lt.semi[v]) ? u : lt.parent[w];
        }
        lt.bucket[lt.parent[w]].clear();
    }

    // Step 4: Final pass for nodes where idom differs from semi
    for (uint32_t i = 1; i < lt.dfs_clock; ++i) {
        uint32_t w = lt.vertex[i];
        if (lt.idom[w] != lt.parent[w])
            lt.idom[w] = lt.idom[lt.idom[w]];
    }

    // Step 5: Write idom back to CfgBlock (idom[w] = vertex ID)
    blocks[0].idom = -1;  // entry
    for (uint32_t i = 1; i < n; ++i) {
        uint32_t w = lt.vertex[i];
        blocks[w].idom = static_cast<int32_t>(lt.idom[w]);
    }
}

// ── Natural loop detection ─────────────────────────────────────────────
// For each back-edge (a→b where b dominates a):
//   header = b, loop blocks = {a} ∪ path from a up to b in dom tree
static void DetectNaturalLoops(std::vector<CfgBlock>& blocks,
                               std::vector<NaturalLoop>& loops) noexcept
{
    uint32_t n = static_cast<uint32_t>(blocks.size());
    loops.clear();

    for (uint32_t a = 0; a < n; ++a) {
        for (uint32_t b : blocks[a].succs) {
            // back-edge: a→b where b dominates a
            if (b >= n) continue;
            if (blocks[a].idom < 0) continue;  // entry has no idom

            // Check if b dominates a by walking up idom chain from a
            bool dominates = false;
            int32_t cur = static_cast<int32_t>(a);
            while (cur >= 0) {
                if (static_cast<uint32_t>(cur) == b) {
                    dominates = true;
                    break;
                }
                cur = blocks[cur].idom;
            }
            if (!dominates) continue;

            // Natural loop: header = b, blocks = path from a up to b
            NaturalLoop loop;
            loop.header = b;
            loop.back_edge_from = a;
            loop.blocks.push_back(b);  // header
            loop.blocks.push_back(a);  // back-edge source

            // Walk idom chain from a up to b
            cur = static_cast<int32_t>(a);
            while (cur >= 0 && static_cast<uint32_t>(cur) != b) {
                bool found = false;
                for (uint32_t x : loop.blocks) {
                    if (x == static_cast<uint32_t>(cur)) {
                        found = true;
                        break;
                    }
                }
                if (!found)
                    loop.blocks.push_back(static_cast<uint32_t>(cur));
                cur = blocks[cur].idom;
            }

            loops.push_back(loop);
        }
    }
}

// ── Assign loop depths ─────────────────────────────────────────────────
// Outer loops first: depth 0 → 1 → ...  For nested loops, child depth = parent depth + 1.
static void AssignLoopDepths(std::vector<CfgBlock>& blocks,
                             std::vector<NaturalLoop>& loops) noexcept
{
    // Reset all depths
    for (auto& blk : blocks)
        blk.loop_depth = 0;

    // Sort loops by header id to process outermost first
    // (lower header = likely more outer in a linear layout).
    // For each loop header, check if it's contained in another loop.
    for (auto& loop : loops) {
        uint32_t depth = 0;
        for (const auto& other : loops) {
            if (&other == &loop) continue;
            // Check if this loop's header is inside another loop
            for (uint32_t b : other.blocks) {
                if (b == loop.header) {
                    depth = other.depth + 1;
                    break;
                }
            }
            if (depth > 0) break;
        }
        loop.depth = depth;
    }

    // Apply max depth to each block
    for (const auto& loop : loops) {
        for (uint32_t b : loop.blocks) {
            if (loop.depth + 1 > blocks[b].loop_depth)
                blocks[b].loop_depth = loop.depth + 1;
        }
    }
}

// ── BuildCfg entry point ───────────────────────────────────────────────

LoopAnalysis BuildCfg(const std::vector<BBRange>& bbs,
                      const interpreter::RegisterInstruction* instrs) noexcept
{
    LoopAnalysis result;
    uint32_t n = static_cast<uint32_t>(bbs.size());
    if (n == 0) return result;

    // Determine total instruction count for inst→bb map
    uint32_t total_instrs = 0;
    if (!bbs.empty())
        total_instrs = bbs.back().hi;
    auto inst_to_bb = BuildInstToBbMap(bbs, total_instrs);

    // Step 1: Create CfgBlocks
    result.blocks.resize(n);
    for (uint32_t i = 0; i < n; ++i) {
        auto& blk = result.blocks[i];
        blk.lo = bbs[i].lo;
        blk.hi = bbs[i].hi;
        blk.id = i;
    }

    // Step 2: Build predecessor/successor edges
    for (uint32_t i = 0; i < n; ++i) {
        std::vector<uint32_t> succs;
        FindBlockSuccessors(i, bbs[i], instrs, inst_to_bb, n, succs);
        result.blocks[i].succs = succs;
        for (uint32_t s : succs) {
            if (s < n)
                result.blocks[s].preds.push_back(i);
        }
    }

    // Step 3: Compute dominator tree (Lengauer-Tarjan)
    ComputeDominators(result.blocks);

    // Step 4: Detect natural loops
    DetectNaturalLoops(result.blocks, result.loops);
    result.has_loops = !result.loops.empty();

    // Step 5: Assign loop depths
    if (result.has_loops)
        AssignLoopDepths(result.blocks, result.loops);

    return result;
}

}  // namespace chaos::il2cpp::jit::tree
