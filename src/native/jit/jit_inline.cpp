// jit_inline.cpp — Inlining infrastructure implementation
//
// Provides TokenToPrecodeMap, EvaluateInline, Inliner, and InlineReverseMap.

#include "jit_inline.h"
#include "jit_method.h"
#include "jit_precode.h"

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
    uint32_t /*callee_token*/,
    uint32_t /*caller_depth*/,
    bool     /*return_value_used*/,
    const InlineConfig& /*cfg*/) noexcept
{
    // Minimal stub: always reject inlining.  Full heuristics (callee size,
    // loop depth, return value analysis) are added when inlining is enabled.
    return InlineDecision{};
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

bool Inliner::TryInline(InlineCandidate& /*candidate*/,
                         tree::ExprNode** /*roots*/,
                         uint32_t& /*root_count*/,
                         uint32_t /*max_roots*/) noexcept
{
    // Minimal stub: no inline grafting performed.
    // Full implementation builds callee tree via TreeBuilder, replaces the
    // kCall node with the callee's return expression, appends side-effect
    // roots, and remaps vregs to avoid caller/callee conflicts.
    return false;
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
        candidate.callee_token = node->method_token;
        candidate.first_arg_vreg = 0;
        candidate.arg_count    = 0;
        candidate.decision     = EvaluateInline(candidate.callee_token,
                                                inline_depth_,
                                                false, cfg_);

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
    map_->entries.push_back({callee_token, caller});
}

void InlineReverseMap::RemoveAll(JitMethod* caller) noexcept {
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
