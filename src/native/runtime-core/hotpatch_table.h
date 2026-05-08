#ifndef CHAOS_IL2CPP_HOTPATCH_TABLE_H_
#define CHAOS_IL2CPP_HOTPATCH_TABLE_H_

// ── Hotpatch Table + HotpatchNameRegistry ─────────────────────────────
//
// Runtime infrastructure for the hotpatch dispatch architecture.
// See wiki: 03-功能模块/06-il2cpp核心架构/04-历史决策/18-HotupdateDispatch-决策.md
//
// Two-level name-based method lookup for hot-patch dispatching.
// Holds pointers to codegen-emitted .rodata arrays — no dynamic allocation
// during normal operation. Thread-safe for reads after bootstrap (all module
// registration happens during single-threaded bootstrap).

#include <chaos/native_types.h>
#include "codegen_bridge.h"

#include <cstddef>
#include <cstdint>

namespace chaos::il2cpp::runtime_core {

// ── HotpatchNameRegistry ─────────────────────────────────────────────
//
// Registry for per-module hotpatch data emitted by codegen.  Supports:
//   1) type_name + method_name → AOT metadata token  (name index lookup)
//   2) AOT metadata token      → dispatch table slot  (Token→Slot lookup)
//   3) direct dispatch entry access for patch/unpatch
//
// All name matching uses byte-exact strcmp (UTF-8).  No case folding.
class HotpatchNameRegistry {
public:
    static constexpr size_t kMaxModules = 64;

    // ── Registration ────────────────────────────────────────────────
    void RegisterModule(const HotpatchModuleV0* module) noexcept;

    size_t ModuleCount() const noexcept { return module_count_; }

    // ── Lookup ──────────────────────────────────────────────────────
    uint32_t LookupMethod(const char* type_name, const char* method_name) const noexcept;
    uint32_t TokenToSlot(uint32_t token) const noexcept;

    // ── Dispatch entry access ──────────────────────────────────────
    HotpatchEntryV0* GetDispatchEntry(uint32_t token) const noexcept;
    HotpatchEntryV0* GetDispatchEntryBySlot(size_t module_index, uint32_t slot) const noexcept;

    // ── Patch management ────────────────────────────────────────────
    void SetPatched(uint32_t token, bool patched, void* method_key) noexcept;

private:
    const HotpatchModuleV0* modules_[kMaxModules] = {};
    size_t module_count_ = 0;

    // Single-entry token→module cache: avoids re-scanning all modules.
    mutable uint32_t token_cache_key_ = 0;
    mutable size_t   token_cache_value_ = ~static_cast<size_t>(0);

    size_t FindModuleForToken(uint32_t token) const noexcept;

    static int CompareTypeName(const void* key, const void* elem) noexcept;
    static int CompareTokenSlot(const void* key, const void* elem) noexcept;
};

// ── Global singleton ──────────────────────────────────────────────────
HotpatchNameRegistry& GetHotpatchNameRegistry() noexcept;

// ── Registration entry point (extern "C") ────────────────────────────
void RegisterHotpatchModule(const HotpatchModuleV0* module) noexcept;

void RegisterReversePInvokeWrappers(void* const* wrappers, uint32_t count) noexcept;

// ── Dispatch helpers ──────────────────────────────────────────────────

inline HotpatchEntryV0* HotpatchLookup(uint32_t token) noexcept {
    return GetHotpatchNameRegistry().GetDispatchEntry(token);
}

inline HotpatchEntryV0* HotpatchLookupBySlot(size_t module_index, uint32_t slot) noexcept {
    return GetHotpatchNameRegistry().GetDispatchEntryBySlot(module_index, slot);
}

void* HotpatchCallViaSlot(uint32_t module_index, uint32_t slot, void* args_buf, void* ret) noexcept;

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_HOTPATCH_TABLE_H_