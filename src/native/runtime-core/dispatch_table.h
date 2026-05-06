#ifndef CHAOS_IL2CPP_DISPATCH_TABLE_H_
#define CHAOS_IL2CPP_DISPATCH_TABLE_H_

// ── Dispatch Table + NameIndexRegistry ─────────────────────────────────
//
// Runtime infrastructure for the D3 Dual-Layer Dispatch architecture.
// See wiki: 04-历史决策/18-HotUpdateDispatchD3-决策.md
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

// ── NameIndexRegistry ─────────────────────────────────────────────────
//
// Registry for per-module NameIndex data emitted by codegen.  Supports:
//   1) type_name + method_name → AOT metadata token  (NameIndex lookup)
//   2) AOT metadata token      → dispatch table slot  (Token→Slot lookup)
//   3) direct dispatch entry access for patch/unpatch
//
// All name matching uses byte-exact strcmp (UTF-8).  No case folding.
class NameIndexRegistry {
public:
    static constexpr size_t kMaxModules = 64;

    // ── Registration ────────────────────────────────────────────────
    // Register a module's name index and dispatch table.
    // `module` points into .rodata and must remain valid for process lifetime.
    void RegisterModule(const NameIndexModuleV0* module) noexcept;

    size_t ModuleCount() const noexcept { return module_count_; }

    // ── Lookup ──────────────────────────────────────────────────────
    // Lookup a method's AOT metadata token by fully qualified type and method name.
    // Returns 0 if not found.
    uint32_t LookupMethod(const char* type_name, const char* method_name) const noexcept;

    // Reverse lookup: AOT metadata token → dispatch table slot index.
    // Returns ~0u if not found in any registered module.
    uint32_t TokenToSlot(uint32_t token) const noexcept;

    // ── Dispatch entry access ──────────────────────────────────────
    // Get dispatch entry pointer for a given AOT token.
    // Returns nullptr if token is not found.
    DispatchEntryV0* GetDispatchEntry(uint32_t token) const noexcept;

    // Get dispatch entry by (module_index, slot).
    DispatchEntryV0* GetDispatchEntryBySlot(size_t module_index, uint32_t slot) const noexcept;

    // ── Patch management ────────────────────────────────────────────
    // Mark a dispatch entry as patched or reverted.  Sets interrupt_ptr
    // and method_key accordingly.
    void SetPatched(uint32_t token, bool patched, void* method_key) noexcept;

private:
    const NameIndexModuleV0* modules_[kMaxModules] = {};
    size_t module_count_ = 0;

    // Cached token → module_index mapping: populated lazily on first
    // GetDispatchEntry miss.  Avoids re-scanning all modules on every call.
    mutable uint32_t token_cache_key_ = 0;
    mutable size_t   token_cache_value_ = ~static_cast<size_t>(0);

    // Find which registered module contains a given token (linear scan).
    size_t FindModuleForToken(uint32_t token) const noexcept;

    // Binary search comparators (qsort-style).
    static int CompareTypeName(const void* key, const void* elem) noexcept;
    static int CompareTokenSlot(const void* key, const void* elem) noexcept;
};

// ── Global singleton ──────────────────────────────────────────────────
// Initialised during bootstrap.  Thread-safe for reads after init.
NameIndexRegistry& GetNameIndexRegistry() noexcept;

// ── Registration entry point (extern "C") ────────────────────────────
// Called by codegen-generated static initializers or PatchLoader to
// register a module's name index and dispatch table.
void RegisterModuleNameIndex(const NameIndexModuleV0* module) noexcept;

/// Register an array of function pointers for <see cref="System.Runtime.InteropServices.UnmanagedCallersOnlyAttribute"/>
/// methods. Each entry is a C-ABI function pointer that native code can call directly.
/// Called by codegen-generated static initializers during module load.
/// `wrappers` must remain valid for process lifetime (points into .rodata).
void RegisterReversePInvokeWrappers(void* const* wrappers, uint32_t count) noexcept;

// ── Dispatch helpers ──────────────────────────────────────────────────

// RuntimeDispatchLookup: given an AOT metadata token, return the
// corresponding dispatch entry (or nullptr if not found).
inline DispatchEntryV0* RuntimeDispatchLookup(uint32_t token) noexcept {
    return GetNameIndexRegistry().GetDispatchEntry(token);
}

// RuntimeDispatchLookupBySlot: lookup dispatch entry by module index and slot.
// Used by the test harness to verify D3 dispatch state without knowing the
// AOT token (slot indices are deterministic per MethodN ordering).
inline DispatchEntryV0* RuntimeDispatchLookupBySlot(size_t module_index, uint32_t slot) noexcept {
    return GetNameIndexRegistry().GetDispatchEntryBySlot(module_index, slot);
}

// CallViaSlot: invoke a method through the dispatch table.  If the entry
// is patched, routes through interrupt_ptr → InterpreterEntryDirect.
// Returns the direct_ptr for AOT calls.
// Used by the test harness for unified dispatch.
void* CallViaSlot(uint32_t module_index, uint32_t slot, void* args_buf, void* ret) noexcept;

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_DISPATCH_TABLE_H_
