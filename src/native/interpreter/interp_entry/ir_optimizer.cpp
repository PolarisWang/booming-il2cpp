// ir_optimizer.cpp — Tier 2 (Warm) IR re-optimization engine
//
// Called from InterpreterEntryDirect when a PatchMethod's call_count exceeds
// kT1HotThreshold (100).  Transforms the standard IR into an optimized variant
// with deep inlining, instruction fusion, and graph-coloring register allocation.
//
// The optimized RegisterMethod is stored in PatchMethod::cached_optimized_reg_method
// and used by RegisterExecute when tier_state == T2_ready.
//
// Optimization pipeline:
//   1. Clone the original IRMethod (preserve original for FastExecute fallback)
//   2. Deep inline (2 levels, 32-instr budget per callee)
//   3. Instruction fusion (peephole optimizations)
//   4. Graph-coloring register allocation
//   5. Store in PatchMethod::cached_optimized_reg_method

#include "ir_reg_alloc.h"
#include "patch_loader.h"

#include "interpreter_vm.h"

#include <chaos/log.h>
#include <vector>

namespace chaos::il2cpp::runtime_core {

// Forward declaration: defined in inlining.cpp.
bool DeepInlineCallees(
    interpreter::IRMethod& ir,
    PatchMethod& patch_method,
    uint32_t max_levels,
    uint32_t max_instructions) noexcept;

// ── Helpers ───────────────────────────────────────────────────────────────

/// Clone an IRMethod for independent optimization.
/// The clone has its own copy of instructions and SEH clauses.
static interpreter::IRMethod CloneIRMethod(const interpreter::IRMethod& src) noexcept {
    interpreter::IRMethod dst;
    dst.instructions = src.instructions;  // vector copy
    dst.seh_clauses  = src.seh_clauses;   // vector copy
    return dst;
}

// ── Instruction fusion pass ───────────────────────────────────────────────
// Peephole optimizations at the IRInstruction level:
//
//   Pattern 1: LdNull + StLoc n  →  StLoc n (with null value marker, or just StLoc)
//              For safety in Phase 1: remove the LdNull (the StLoc already pops).
//              Since StLoc pops and IR's StLoc does a typed store, we can simplify
//              LdNull; StLoc n → just StLocNull n.
//
//   Pattern 2: LdLoc m + StLoc n  →  MovLoc n, m
//              Copy propagation.  For Phase 1: only when m == n (redundant store),
//              both instructions can be removed.
//
//   Pattern 3: Pop; Pop  →  Pop2 (combined stack pop)
//              When two consecutive Pop instructions exist, replace with one Pop2.
//              For Phase 1: remove dead Pop pairs (they cancel each other's sp effect
//              but we can't easily verify sp balance without full dataflow).
//
//   Pattern 4: Dup; Pop  →  no-op (both eliminated)
//              These cancel — remove both.
//
//   For Phase 1, the most impactful fusion is:
//     - Dead Pop elimination (trailing Pops before Ret)
//     - Dup+Pop cancellation
//     - Redundant LdLoc+StLoc (same local) elimination
//
// Returns the number of instructions removed.

struct FusionStats {
    uint32_t dead_pops_removed    = 0;
    uint32_t dup_pop_cancelled    = 0;
    uint32_t redundant_locals     = 0;
    uint32_t ldnull_stloc_fused   = 0;
    uint32_t ldc_add_fused        = 0;
};

static FusionStats FusePass(interpreter::IRMethod& ir) noexcept {
    FusionStats stats;
    auto& instrs = ir.instructions;
    if (instrs.size() < 2) return stats;

    std::vector<interpreter::IRInstruction> fused;
    fused.reserve(instrs.size());

    for (size_t i = 0; i < instrs.size(); ++i) {
        const auto& op = instrs[i];

        // ── Pattern: Delete trailing Pop(s) before Ret ─────────────────
        // These are dead stack cleanup that serve no purpose after inlining.
        if (op.op_code == interpreter::IROpCode::Pop) {
            // Look ahead: if this Pop is followed by Ret (possibly with more
            // Pops in between), skip all consecutive Pops.
            size_t j = i;
            while (j < instrs.size() &&
                   instrs[j].op_code == interpreter::IROpCode::Pop) {
                ++j;
            }
            if (j < instrs.size() &&
                instrs[j].op_code == interpreter::IROpCode::Ret) {
                // These Pops are dead — skip them all.
                uint32_t skipped = static_cast<uint32_t>(j - i);
                stats.dead_pops_removed += skipped;
                i = j - 1;  // loop will advance past Ret too; Ret will be copied below
                continue;
            }
            // Single Pop — not obviously dead, keep it.
            fused.push_back(op);
            continue;
        }

        // ── Pattern: Dup + Pop → remove both ─────────────────────────
        if (op.op_code == interpreter::IROpCode::Dup &&
            i + 1 < instrs.size() &&
            instrs[i + 1].op_code == interpreter::IROpCode::Pop) {
            ++stats.dup_pop_cancelled;
            ++i;  // skip both Dup and Pop
            continue;
        }

        // ── Pattern: LdLoc m + StLoc n where m == n → remove pair ────
        // Redundant: load from local, store back to same local.
        if (op.op_code == interpreter::IROpCode::LdLoc &&
            i + 1 < instrs.size() &&
            instrs[i + 1].op_code == interpreter::IROpCode::StLoc &&
            op.operand_index == instrs[i + 1].operand_index) {
            ++stats.redundant_locals;
            ++i;  // skip both
            continue;
        }

        // ── Pattern: LdNull + StLoc n → fused StLocNull ─────────────
        // Replace with StLoc that has a special flag marking it as null store.
        // For Phase 1: just keep both (they work correctly as-is).
        // The fusion benefit here is small since LdNull+StLoc is already fast.

        // ── Default: keep instruction ─────────────────────────────────−
        fused.push_back(op);
    }

    // Replace instruction list if fusion reduced size.
    if (fused.size() < instrs.size()) {
        instrs.swap(fused);
    }

    return stats;
}

/// Run multi-pass instruction fusion until stable.
static void FuseInstructions(interpreter::IRMethod& ir) noexcept {
    FusionStats total;
    for (int pass = 0; pass < 4; ++pass) {
        auto stats = FusePass(ir);
        total.dead_pops_removed  += stats.dead_pops_removed;
        total.dup_pop_cancelled  += stats.dup_pop_cancelled;
        total.redundant_locals   += stats.redundant_locals;
        total.ldnull_stloc_fused += stats.ldnull_stloc_fused;

        if (stats.dead_pops_removed == 0 &&
            stats.dup_pop_cancelled == 0 &&
            stats.redundant_locals == 0 &&
            stats.ldnull_stloc_fused == 0) {
            break;  // Stable
        }
    }

    CHAOS_IL2CPP_LOG_DEBUG_M("tier", "FuseInstructions: removed {} dead Pops, "
                           "{} Dup+Pop pairs, {} redundant LdLoc+StLoc",
                           total.dead_pops_removed,
                           total.dup_pop_cancelled,
                           total.redundant_locals);
}

// ── OptimizeToTier2: main entry point ─────────────────────────────────────

bool OptimizeToTier2(PatchMethod* pm) noexcept {
    if (pm == nullptr) return false;

    auto* orig_ir = static_cast<interpreter::IRMethod*>(pm->cached_ir);
    if (orig_ir == nullptr) {
        CHAOS_IL2CPP_LOG_DEBUG_M("tier", "OptimizeToTier2: no cached IR for method_token={}",
                               pm->token);
        return false;
    }

    // Skip optimization for methods with SEH (InterpreterVM-only).
    if (!orig_ir->seh_clauses.empty()) {
        CHAOS_IL2CPP_LOG_DEBUG_M("tier", "OptimizeToTier2: SEH method, skip (token={})",
                               pm->token);
        return false;
    }

    // Skip very small methods (≤2 instructions — already handled by ultra-fast-path).
    if (orig_ir->instructions.size() <= 2) {
        return false;
    }

    // ── Stage 1: Clone ──────────────────────────────────────────────────
    auto cloned_ir = CloneIRMethod(*orig_ir);

    // ── Stage 2: Deep inline (2 levels, 32-instr budget) ───────────────
    DeepInlineCallees(cloned_ir, *pm, 2, 32);

    // ── Stage 3: Instruction fusion ─────────────────────────────────────
    FuseInstructions(cloned_ir);

    // ── Stage 4: Graph-coloring register allocation ─────────────────────
    auto optimized_rm = interpreter::AllocateRegistersGraphColoring(cloned_ir);

    // ── Stage 5: Validate and store ─────────────────────────────────────
    if (optimized_rm.instructions.empty()) {
        CHAOS_IL2CPP_LOG_DEBUG_M("tier", "OptimizeToTier2: empty RegisterMethod (token={})",
                               pm->token);
        return false;
    }

    // Heap-allocate the RegisterMethod (must outlive the stack frame).
    auto* storage = static_cast<interpreter::RegisterMethod*>(
        CHAOS_IL2CPP_MALLOC(sizeof(interpreter::RegisterMethod)));
    if (storage == nullptr) return false;

    ::new (storage) interpreter::RegisterMethod(std::move(optimized_rm));

    // Atomically store (release — paired with acquire load in entry_direct).
    pm->cached_optimized_reg_method = storage;

    CHAOS_IL2CPP_LOG_DEBUG_M("tier", "OptimizeToTier2: token={}, orig_instrs={}, "
                           "opt_instrs={}, opt_regs={}",
                           pm->token,
                           orig_ir->instructions.size(),
                           cloned_ir.instructions.size(),
                           storage->max_regs);

    return true;
}

}  // namespace chaos::il2cpp::runtime_core
