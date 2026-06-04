// jit_inline.h — Inlining infrastructure for tree IR optimizer
//
// Provides:
//   TokenToPrecodeMap — global method_token → JitPrecode lookup (inline target resolution)
//   InlineConfig     — heuristics configuration
//   InlineDecision   — result of inline candidate evaluation
//   EvaluateInline   — heuristics evaluation function
//   Inliner          — tree grafting: replaces kCall with callee expression tree
//   InlineReverseMap — token → JitMethod* for hot-update invalidation

#ifndef CHAOS_IL2CPP_JIT_INLINE_H_
#define CHAOS_IL2CPP_JIT_INLINE_H_

#include "jit_method.h"
#include "tree/jit_tree_builder.h"
#include "tree/jit_tree_node.h"
#include "ir_reg_alloc.h"

#include <cstdint>
#include <memory>
#include <mutex>
#include <vector>

// Forward declarations
struct HotpatchEntryV0;

namespace chaos::il2cpp::jit {

struct JitPrecode;
class InlineReverseMap;

// ── TokenToPrecodeMap ───────────────────────────────────────────────────────
// Global mapping: metadata token → JitPrecode*.
// Populated during RegisterJitEntryMethods().
// Enables the inliner to find a callee's RegisterMethod by method_token.
class TokenToPrecodeMap {
public:
    void Register(uint32_t token, JitPrecode* precode) noexcept;
    void Unregister(uint32_t token) noexcept;
    JitPrecode* Lookup(uint32_t token) const noexcept;

private:
    struct Map;
    Map* map_ = nullptr;
};

extern TokenToPrecodeMap g_token_to_precode;

// ── InlineConfig ────────────────────────────────────────────────────────────
// Heuristics thresholds for inline decision-making.
struct InlineConfig {
    uint32_t max_callee_nodes  = 25;   // max ExprNode count in callee
    uint32_t max_inline_depth  = 2;    // max nesting depth of inlined calls
    int32_t  loop_bonus        = 3;    // subtract from node count per loop level
    int32_t  return_used_bonus = 5;    // add to node count if return value used
};

// ── InlineDecision ─────────────────────────────────────────────────────────
struct InlineDecision {
    bool     should_inline       = false;
    uint32_t callee_token        = 0;
    uint32_t callee_module_id    = 0;
    uint32_t snapshot_version    = 0;  // HotpatchEntryV0.version at decision time
    uint32_t callee_max_vreg     = 0;  // for vreg remap range
    uint32_t callee_instr_count  = 0;  // for diagnostics
    const interpreter::RegisterMethod* callee_rm = nullptr;
};

// ── EvaluateInline ─────────────────────────────────────────────────────────
// Evaluate heuristics for a potential inline of `call_node` targeting `callee_token`.
InlineDecision EvaluateInline(
    uint32_t callee_token,
    uint32_t caller_depth,
    bool     return_value_used,
    const InlineConfig& cfg) noexcept;

// ── Inliner ────────────────────────────────────────────────────────────────
// Per-BB inliner: grafts callee expression trees into caller's root set.
// Must stay alive during Linearization (owns callee TreeBuilder arenas).
//
// Usage:
//   Inliner inliner(cfg);
//   inliner.InlineRoots(roots, root_count, max_roots);
//   // ... ConstFold, CSE, Linearizer ...
//   // (Inliner dtor after Linearizer — callee arenas stay alive)
class Inliner {
public:
    explicit Inliner(const InlineConfig& cfg,
                     uint32_t inline_depth = 0,
                     uint32_t caller_max_vreg = 0) noexcept;

    /// Try to inline eligible kCall nodes in the given root set.
    /// Replaces kCall nodes IN-PLACE with callee return expression.
    /// Appends callee side-effect roots to the root array.
    ///
    /// @param roots        Root node array (modified in-place)
    /// @param root_count   In/Out: number of roots
    /// @param max_roots    Capacity of roots array
    /// @returns Number of successful inlines
    uint32_t InlineRoots(tree::ExprNode** roots,
                         uint32_t& root_count,
                         uint32_t max_roots) noexcept;

    /// Updated max_vreg after accounting for callee vreg shifts.
    /// Must be passed to Linearizer after inlining.
    uint32_t new_max_vreg() const noexcept { return new_max_vreg_; }

    /// Number of inlined decisions (for JitMethod population).
    uint32_t inlined_count() const noexcept { return inlined_count_; }

    /// Access inlined callee info array (size = inlined_count()).
    const InlineDecision* inlined_decisions() const noexcept {
        return inlined_decisions_;
    }

private:
    struct CalleeArena {
        std::unique_ptr<tree::TreeBuilder> builder;
        tree::TreeBuildResult result;
    };

    struct InlineCandidate {
        tree::ExprNode* call_node;
        uint32_t        callee_token;
        uint32_t        first_arg_vreg;
        uint32_t        arg_count;
        InlineDecision  decision;
    };

    bool TryInline(InlineCandidate& candidate,
                   tree::ExprNode** roots,
                   uint32_t& root_count,
                   uint32_t max_roots) noexcept;

    InlineConfig cfg_;
    uint32_t     inline_depth_;
    uint32_t     new_max_vreg_ = 0;
    uint32_t     inlined_count_ = 0;
    InlineDecision inlined_decisions_[8];  // max inlines per BB

    // Keep callee TreeBuilders alive during BB processing
    std::vector<CalleeArena> callee_arenas_;
};

// ── InlineReverseMap ───────────────────────────────────────────────────────
// Tracks which JitMethods have inlined a given callee.
// Used during hotpatch to invalidate callers of patched methods.
class InlineReverseMap {
public:
    InlineReverseMap() noexcept;
    ~InlineReverseMap() noexcept;
    InlineReverseMap(const InlineReverseMap&) = delete;
    InlineReverseMap& operator=(const InlineReverseMap&) = delete;

    void Add(uint32_t callee_token, JitMethod* caller) noexcept;
    void RemoveAll(JitMethod* caller) noexcept;
    uint32_t InvalidateCallers(uint32_t callee_token, HotpatchEntryV0* callee_entry) noexcept;

private:
    struct Map;
    Map* map_ = nullptr;
    std::mutex mtx_;
};

extern InlineReverseMap g_inline_reverse_map;

}  // namespace chaos::il2cpp::jit

#endif  // CHAOS_IL2CPP_JIT_INLINE_H_
