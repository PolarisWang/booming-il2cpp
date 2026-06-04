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
#include <chaos/unordered_dense.h>
#include "codegen_bridge.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include <atomic>

namespace chaos::il2cpp::runtime_core {

// ── HotpatchNameRegistry ─────────────────────────────────────────────
//
// Registry for per-module hotpatch data emitted by codegen.  Supports:
//   1) type_name + method_name → AOT metadata token  (name index lookup)
//   2) AOT metadata token      → dispatch table slot  (Token→Slot lookup)
//   3) direct dispatch entry access for patch/unpatch
//
// All name matching uses byte-exact strcmp (UTF-8).  No case folding.
//
// NOTE: Since the v0.3 refactor, this registry uses std::vector for the
// module list (no hard cap) and returns (module_id<<32 | token) composite
// keys from LookupMethod.  All token-scoped operations are module-qualified
// to eliminate token-space collisions between assemblies.
class HotpatchNameRegistry {
public:
    // ── Registration ────────────────────────────────────────────────
    void RegisterModule(const HotpatchModuleV0* module) noexcept;
    // Bulk registration for bootstrap — O(total methods) builds lookup_cache_.
    void RegisterAllModules(const HotpatchModuleV0* const* modules, uint32_t count) noexcept;

    size_t ModuleCount() const noexcept { return modules_.size(); }

    // ── Lookup ──────────────────────────────────────────────────────
    // Returns composite key: (module_index << 32) | token, or 0 if not found.
    uint64_t LookupMethod(const char* ns, const char* type_name, const char* method_name) const noexcept;

    // Direct access to registered modules (for multi-module dependency checks).
    const HotpatchModuleV0* GetModuleByIndex(size_t index) const noexcept {
        return (index < modules_.size()) ? modules_[index] : nullptr;
    }

    // Token→Slot within a specific module (no cross-module collision).
    uint32_t TokenToSlot(uint32_t module_id, uint32_t token) const noexcept;

    // Token→(module_index<<32 | token) across all modules (bsearch).
    // Returns composite key usable with ExtractModuleId/ExtractToken,
    // or 0 if token not found in any module.
    uint64_t FindToken(CHAOS_IL2CPP_UINT32 method_token) const noexcept;

    // ── Dispatch entry access ──────────────────────────────────────
    // Module-scoped lookup (preferred — no token collision).
    HotpatchEntryV0* GetDispatchEntry(uint32_t module_id, uint32_t token) const noexcept;
    HotpatchEntryV0* GetDispatchEntryBySlot(size_t module_index, uint32_t slot) const noexcept;

    // ── Method name resolution (for debugger/SOS) ─────────────────
    // Given (module_id, method_token), scan method_entries and return
    // the UTF-8 method name from codegen-emitted .rodata.
    // Returns nullptr if not found (e.g. AOT-only method without hotpatch entry).
    const char* GetMethodName(uint32_t module_id, uint32_t method_token) const noexcept;

    // ── Patch management ────────────────────────────────────────────
    // Module-scoped patch: set/unset patch on (module_id, slot) — O(1).
    // @param domain_id  Domain owning this patch (0 = core domain, never unloaded).
    //                    Only meaningful when patched=true; ignored when patched=false.
    void SetPatchedBySlot(uint32_t module_id, uint32_t slot, bool patched, void* method_key,
                           uint32_t domain_id = 0) noexcept;

    /// Clear all dispatch entries belonging to the given domain.
    /// Called during domain unload (STW phase) to prevent use-after-free
    /// of PatchMethod pointers in the unloaded domain's heap.
    /// @param domain_id  The domain whose dispatch entries should be cleared.
    /// @return Number of entries cleared.
    uint32_t ClearDomainDispatchEntries(uint32_t domain_id) noexcept;

    // Reverse lookup: slot index → metadata token. Linear scan, only
    // called during hotpatch (non-critical path).
    uint32_t SlotToToken(uint32_t module_id, uint32_t slot) const noexcept;

private:
    std::vector<const HotpatchModuleV0*> modules_;

    // ── Fast lookup cache ─────────────────────────────────────────────
    // Maps "ns\0type\0method" → (module_index << 32) | token.
    // Populated during RegisterModule / RegisterAllModules.
    // Avoids O(modules × log(types)) per LookupMethod call.
    CHAOS_IL2CPP_UNORDERED_DENSE_MAP(CHAOS_IL2CPP_STRING, CHAOS_IL2CPP_UINT64) lookup_cache_;

    // ── Per-domain patch tracking ────────────────────────────────────
    // Flat vector of (domain_id, module_id, slot) triples for all patched
    // dispatch entries. Populated when SetPatchedBySlot(patched=true) is
    // called with domain_id > 0. On unpatch (patched=false), the matching
    // entry is removed. On domain unload, ClearDomainDispatchEntries
    // iterates and clears all entries belonging to the unloaded domain.
    // Linear scan with O(n) per operation; n is typically <1000.
    struct DomainPatchRecord {
        uint32_t domain_id;
        uint32_t module_id;
        uint32_t slot;
    };
    std::vector<DomainPatchRecord> domain_patches_;

    // Build cache entries for one module (called from RegisterModule).
    void BuildLookupCacheForModule(const HotpatchModuleV0* mod, size_t module_index) noexcept;

    static int CompareTypeName(const void* key, const void* elem) noexcept;
    static int CompareTokenSlot(const void* key, const void* elem) noexcept;
};

// ── Global singleton ──────────────────────────────────────────────────
HotpatchNameRegistry& GetHotpatchNameRegistry() noexcept;

// ── Registration entry point (extern "C") ────────────────────────────
void RegisterHotpatchModule(const HotpatchModuleV0* module) noexcept;

// ── Module registration callback ─────────────────────────────────────
// Called after each module registration. Used by PatchLoader to retry
// deferred patches whose dependencies may now be satisfied.
// Set to nullptr when no callback is needed (default).
typedef void (*ModuleRegisteredCallback)() noexcept;
void SetModuleRegisteredCallback(ModuleRegisteredCallback cb) noexcept;

void RegisterReversePInvokeWrappers(void* const* wrappers, uint32_t count) noexcept;

// ── Module enumeration helpers ──────────────────────────────────────────
// Used by verification dispatch (ChaosDispatchMethodAllModules) to iterate
// all registered modules without exposing the internal vector directly.
inline size_t HotpatchModuleCount() noexcept {
    return GetHotpatchNameRegistry().ModuleCount();
}
inline const HotpatchModuleV0* GetHotpatchModuleByIndex(size_t index) noexcept {
    return GetHotpatchNameRegistry().GetModuleByIndex(index);
}

// ── Dispatch helpers ──────────────────────────────────────────────────

// Callback type for slot update notifications during hotpatch.
// Invoked by SetPatchedBySlot when a method is patched/unpatched.
// Parameters: callee_token, new_direct_ptr, callee_HotpatchEntryV0_ptr.
// The callee entry pointer enables inline version-staleness checks.
typedef void (*SlotUpdateCallback)(uint32_t, void*, HotpatchEntryV0*);

// Register a global callback fired from SetPatchedBySlot on version bump.
void RegisterSlotUpdateCallback(SlotUpdateCallback cb) noexcept;

// Reverse lookup: given module_id + token, find the slot index.
// Returns ~0u if not found.
uint32_t SlotToToken(uint32_t module_id, uint32_t method_token) noexcept;

// ── Original AOT direct_ptr resolution (JIT mode) ──────────────────────────
// In JIT mode, RegisterJitEntryMethods replaces HotpatchEntryV0::direct_ptr
// with a JIT trampoline, saving the original AOT function pointer in JitPrecode.
// ResolveDirectFn (used during patch IR lowering) needs the real AOT code pointer,
// not the trampoline.  This callback returns the original AOT pointer given a
// dispatch entry, or nullptr in AOT mode (where direct_ptr IS the real code).
typedef void* (*OriginalAotPtrCallback)(HotpatchEntryV0* entry) noexcept;
void SetOriginalAotPtrCallback(OriginalAotPtrCallback cb) noexcept;
OriginalAotPtrCallback GetOriginalAotPtrCallback() noexcept;

inline HotpatchEntryV0* HotpatchLookup(uint32_t module_id, uint32_t token) noexcept {
    return GetHotpatchNameRegistry().GetDispatchEntry(module_id, token);
}

inline HotpatchEntryV0* HotpatchLookupBySlot(size_t module_index, uint32_t slot) noexcept {
    return GetHotpatchNameRegistry().GetDispatchEntryBySlot(module_index, slot);
}

// ── Composite key helpers ─────────────────────────────────────────────
//
// LookupMethod returns a composite key: (module_index << 32) | token.
// These helpers extract the components safely and consistently.
inline uint32_t ExtractModuleId(uint64_t composite_key) noexcept {
    return static_cast<uint32_t>(composite_key >> 32);
}

inline uint32_t ExtractToken(uint64_t composite_key) noexcept {
    return static_cast<uint32_t>(composite_key & 0xFFFFFFFFu);
}

// ── Hotpatch dispatch condition helpers ──────────────────────────────
//
// These helpers provide acquire-order reads of the dispatch entry flags
// field using reinterpret_cast to std::atomic<uint32_t>. The acquire load
// pairs with the release fetch_or in SetPatchedBySlot (hotpatch_table.cpp).
//
// Using reinterpret_cast + std::atomic here instead of C++20 std::atomic_ref
// because the project builds with C++17. std::atomic<uint32_t> and uint32_t
// have identical layout and alignment on all supported platforms (x64/ARM64),
// making this cast safe in practice.
//
// Without the acquire fence, a reader on ARM64 could see kHotpatchActive=1
// but method_key=0 (stale value) — the data race described in S1 of the
// architecture risk audit.
//
// Usage (emitted by codegen at every dispatch point):
//   if (HotpatchIsActive(entry) && !HotpatchShouldKeepNative(entry))
//       InterpreterEntryDirect(entry.method_key, ...);
//   else
//       entry.direct_ptr(...);
inline bool HotpatchIsActive(const HotpatchEntryV0& entry) noexcept {
    return (reinterpret_cast<const std::atomic<uint32_t>*>(&entry.flags)->load(std::memory_order_acquire) & kHotpatchActive) != 0;
}

inline bool HotpatchShouldKeepNative(const HotpatchEntryV0& entry) noexcept {
    return (reinterpret_cast<const std::atomic<uint32_t>*>(&entry.flags)->load(std::memory_order_relaxed) & kHotpatchKeepNative) != 0;
}

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_HOTPATCH_TABLE_H_