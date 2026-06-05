// jit_inline.cpp — Inlining infrastructure implementation
//
// Provides TokenToPrecodeMap, EvaluateInline, Inliner, and InlineReverseMap.

#include "jit_inline.h"
#include "jit_method.h"
#include "jit_precode.h"

#include "tree/jit_tree_builder.h"

#include <cstdint>
#include <vector>

namespace chaos::il2cpp::jit {

// ── TokenToPrecodeMap ───────────────────────────────────────────────────────
// Maps method_token → JitPrecode* for inline target resolution.

struct TokenToPrecodeMap::Map {
    // Key-value pairs; linear scan is fine for small map sizes.
    std::vector<std::pair<uint32_t, JitPrecode*>> entries;
};

void TokenToPrecodeMap::Register(uint32_t token, JitPrecode* precode) noexcept {
    if (!map_) map_ = new Map();
    // Replace if already registered (hot-update re-registration)
    for (auto& entry : map_->entries) {
        if (entry.first == token) {
            entry.second = precode;
            return;
        }
    }
    map_->entries.push_back({token, precode});
}

void TokenToPrecodeMap::Unregister(uint32_t token) noexcept {
    if (!map_) return;
    auto& entries = map_->entries;
    for (size_t i = 0; i < entries.size(); ++i) {
        if (entries[i].first == token) {
            entries[i] = entries.back();
            entries.pop_back();
            return;
        }
    }
}

JitPrecode* TokenToPrecodeMap::Lookup(uint32_t token) const noexcept {
    if (!map_) return nullptr;
    for (const auto& entry : map_->entries) {
        if (entry.first == token) return entry.second;
    }
    return nullptr;
}

TokenToPrecodeMap g_token_to_precode;

// ── EvaluateInline ──────────────────────────────────────────────────────────
// Heuristics evaluation: decides whether a call node should be inlined.

InlineDecision EvaluateInline(
    uint32_t callee_token,
    uint32_t /*caller_depth*/,
    bool     return_value_used,
    uint32_t loop_depth,
    const InlineConfig& cfg) noexcept
{
    // Look up callee via global token-to-precode map.
    JitPrecode* precode = g_token_to_precode.Lookup(callee_token);
    if (precode == nullptr) {
        InlineDecision d;
        d.callee_token = callee_token;
        return d;  // should_inline = false (callee not found)
    }

    const auto& callee_rm = precode->ir;

    // Reject methods with SEH clauses (cannot inline try/catch/fault/finally).
    if (!callee_rm.seh_clauses.empty()) {
        InlineDecision d;
        d.callee_token = callee_token;
        return d;
    }

    // Reject empty methods.
    uint32_t instr_count = static_cast<uint32_t>(callee_rm.instructions.size());
    if (instr_count == 0) {
        InlineDecision d;
        d.callee_token = callee_token;
        return d;
    }

    // Cost model: base = instruction count.
    // Loop bonus: inlining inside loops is more valuable (reduces effective cost).
    // Return-value bonus: eliminating call/ret overhead when result is consumed.
    int32_t cost = static_cast<int32_t>(instr_count);
    cost -= static_cast<int32_t>(loop_depth) * cfg.loop_bonus;
    if (return_value_used) {
        cost -= cfg.return_used_bonus;
    }
    if (cost < 1) cost = 1;

    // Reject if cost exceeds threshold.
    if (static_cast<uint32_t>(cost) > cfg.max_callee_nodes) {
        InlineDecision d;
        d.callee_token = callee_token;
        return d;
    }

    // Accept: populate decision with callee metadata for TryInline.
    InlineDecision d;
    d.should_inline      = true;
    d.callee_token       = callee_token;
    d.callee_max_vreg    = callee_rm.max_regs;
    d.callee_instr_count = instr_count;
    d.callee_rm          = &callee_rm;
    return d;
}

// ── Inliner ─────────────────────────────────────────────────────────────────
// Per-BB inliner: grafts callee expression trees into caller's root set.

Inliner::Inliner(const InlineConfig& cfg,
                 uint32_t inline_depth,
                 uint32_t caller_max_vreg) noexcept
    : cfg_(cfg)
    , inline_depth_(inline_depth)
    , new_max_vreg_(caller_max_vreg)
{}

// ── TryInline ──────────────────────────────────────────────────────────────────
// Builds callee expression tree, remaps vregs, and grafts into caller's root set.

// Recursive helper: visit all nodes in the callee tree and remap vregs.
static void RemapCalleeVRegs(tree::ExprNode* node,
                              uint32_t callee_max_vreg,
                              uint32_t first_arg_vreg,
                              uint32_t vreg_shift) noexcept {
    if (!node) return;

    auto k = node->kind();

    // LdArg(i) → LdLoc(first_arg_vreg + i): callee argument loads become
    // caller local loads from the corresponding argument vregs.
    if (k == tree::kLdArg) {
        node->set_kind(tree::kLdLoc);
        node->operand_index = first_arg_vreg + node->operand_index;
        return;  // leaf: no children to visit
    }

    // LdLoc(v) where v < callee_max_vreg: shift internal vregs.
    if (k == tree::kLdLoc && node->operand_index < callee_max_vreg) {
        node->operand_index += vreg_shift;
        // fall through to visit children (though LdLoc is a leaf in practice)
    }

    // StLoc(l) where l < callee_max_vreg: shift store-target vregs.
    // StLoc stores the local vreg in arg_count (child1 union slot).
    if (k == tree::kStLoc && node->arg_count < callee_max_vreg) {
        node->arg_count += vreg_shift;
    }

    // Recurse into children
    if (node->child_count >= 1 && node->child0)
        RemapCalleeVRegs(node->child0, callee_max_vreg, first_arg_vreg, vreg_shift);
    if (node->child_count >= 2 && node->child1)
        RemapCalleeVRegs(node->child1, callee_max_vreg, first_arg_vreg, vreg_shift);
}

bool Inliner::TryInline(InlineCandidate& candidate,
                         tree::ExprNode** roots,
                         uint32_t& root_count,
                         uint32_t max_roots) noexcept
{
    const InlineDecision& decision = candidate.decision;
    if (!decision.should_inline) return false;
    if (!decision.callee_rm) return false;

    const auto& callee_rm = *decision.callee_rm;
    uint32_t callee_instr_count = static_cast<uint32_t>(callee_rm.instructions.size());
    if (callee_instr_count == 0) return false;

    // Reject multi-BB callees (tree builder only handles single BBs).
    auto bbs = tree::FindBasicBlocks(callee_rm.instructions.data(), callee_instr_count);
    if (bbs.size() > 1) return false;

    // Step 1: Build callee tree in a separate arena.
    CalleeArena arena;
    arena.builder = std::make_unique<tree::TreeBuilder>();
    arena.result = arena.builder->Build(callee_rm, 0, callee_instr_count);
    if (!arena.result.first_node || arena.result.root_count == 0)
        return false;

    // Step 2: VReg safety — must not collide with Linearizer kBaseVReg (64).
    uint32_t callee_max_vreg = callee_rm.max_regs;
    if (callee_max_vreg == 0) return false;
    uint32_t vreg_shift = new_max_vreg_;
    if (vreg_shift + callee_max_vreg > 64) return false;

    // Step 3: Remap callee vregs. Walk all callee roots recursively.
    for (uint32_t ri = 0; ri < arena.result.root_count; ++ri) {
        RemapCalleeVRegs(arena.result.roots[ri],
                          callee_max_vreg,
                          candidate.first_arg_vreg,
                          vreg_shift);
    }

    // Step 4: Identify return expression and side-effect roots.
    // Return: first kReturn node with a child provides the replacement value.
    // Side-effects: all non-kReturn roots are appended to the caller.
    tree::ExprNode* return_expr = nullptr;
    uint32_t side_effect_count = 0;

    for (uint32_t ri = 0; ri < arena.result.root_count; ++ri) {
        tree::ExprNode* root = arena.result.roots[ri];
        if (!root) continue;
        if (root->kind() == tree::kReturn) {
            if (root->child0 && !return_expr)
                return_expr = root->child0;
            continue;
        }
        side_effect_count++;
    }

    // Step 5: Find the call node in roots[] and replace/remove it.
    int32_t call_root_index = -1;
    for (uint32_t ri = 0; ri < root_count; ++ri) {
        if (roots[ri] == candidate.call_node) {
            call_root_index = static_cast<int32_t>(ri);
            break;
        }
    }
    if (call_root_index < 0) return false;

    if (return_expr) {
        // Non-void call: replace kCall root with callee return expression.
        roots[call_root_index] = return_expr;
    } else {
        // Void call: remove kCall root by shifting remaining roots left.
        for (uint32_t ri = static_cast<uint32_t>(call_root_index); ri + 1 < root_count; ++ri) {
            roots[ri] = roots[ri + 1];
        }
        root_count--;
    }

    // Step 6: Append callee side-effect roots to the caller's root set.
    if (root_count + side_effect_count > max_roots) {
        // Not enough room — revert the replacement.
        roots[call_root_index] = candidate.call_node;
        if (!return_expr) root_count++;
        return false;
    }

    for (uint32_t ri = 0; ri < arena.result.root_count; ++ri) {
        tree::ExprNode* root = arena.result.roots[ri];
        if (!root) continue;
        if (root->kind() == tree::kReturn) continue;
        roots[root_count++] = root;
    }

    // Step 7: Update vreg high-water mark and preserve callee arena.
    new_max_vreg_ = vreg_shift + callee_max_vreg;
    callee_arenas_.push_back(std::move(arena));

    return true;
}

uint32_t Inliner::InlineRoots(tree::ExprNode** roots,
                               uint32_t& root_count,
                               uint32_t max_roots) noexcept
{
    if (!roots || root_count == 0 || max_roots == 0) return 0;
    if (inline_depth_ >= cfg_.max_inline_depth) return 0;

    uint32_t inlined = 0;

    for (uint32_t ri = 0; ri < root_count; ++ri) {
        tree::ExprNode* node = roots[ri];
        if (!node || node->kind() != tree::kCall) continue;

        InlineCandidate candidate;
        candidate.call_node   = node;
        candidate.callee_token = node->call_method_token;
        candidate.first_arg_vreg = node->call_arg0_vreg;
        candidate.arg_count    = node->arg_count;
        candidate.decision     = EvaluateInline(candidate.callee_token,
                                                inline_depth_,
                                                candidate.arg_count > 0,
                                                bb_loop_depth_, cfg_);

        if (TryInline(candidate, roots, root_count, max_roots)) {
            ++inlined;
            if (inlined_count_ < 8) {
                inlined_decisions_[inlined_count_++] = candidate.decision;
            }
            if (candidate.decision.callee_max_vreg > new_max_vreg_)
                new_max_vreg_ = candidate.decision.callee_max_vreg;
        }
    }

    return inlined;
}

// ── InlineReverseMap ────────────────────────────────────────────────────────
// Maps callee_token → set of JitMethod* callers that inlined that callee.
// When a method is hotpatched, InvalidateCallers marks all callers as stale
// so their next dispatch triggers recompilation.

struct InlineReverseMap::Map {
    std::vector<std::pair<uint32_t, JitMethod*>> entries;
};

InlineReverseMap::InlineReverseMap() noexcept
    : map_(new Map()) {}

InlineReverseMap::~InlineReverseMap() noexcept {
    delete map_;
}

void InlineReverseMap::Add(uint32_t callee_token, JitMethod* caller) noexcept {
    std::lock_guard<std::mutex> lock(mtx_);
    map_->entries.push_back({callee_token, caller});
}

void InlineReverseMap::RemoveAll(JitMethod* caller) noexcept {
    std::lock_guard<std::mutex> lock(mtx_);
    auto& entries = map_->entries;
    for (size_t i = entries.size(); i > 0; --i) {
        if (entries[i - 1].second == caller) {
            entries[i - 1] = entries.back();
            entries.pop_back();
        }
    }
}

uint32_t InlineReverseMap::InvalidateCallers(uint32_t callee_token,
                                               HotpatchEntryV0* /*callee_entry*/) noexcept {
    std::lock_guard<std::mutex> lock(mtx_);
    uint32_t count = 0;
    auto& entries = map_->entries;
    for (size_t i = entries.size(); i > 0; --i) {
        if (entries[i - 1].first == callee_token) {
            JitMethod* caller = entries[i - 1].second;
            if (caller) {
                caller->stale.store(true, std::memory_order_release);
                ++count;
            }
            entries[i - 1] = entries.back();
            entries.pop_back();
        }
    }
    return count;
}

InlineReverseMap g_inline_reverse_map;

}  // namespace chaos::il2cpp::jit
