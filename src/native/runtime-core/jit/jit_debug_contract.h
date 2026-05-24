#ifndef CHAOS_IL2CPP_JIT_DEBUG_CONTRACT_H_
#define CHAOS_IL2CPP_JIT_DEBUG_CONTRACT_H_

/// jit_debug_contract.h — JIT subsystem debug data contract for SOS extension
///
/// Defines extern "C" globals that the chaos_jit_sos WinDbg extension finds
/// via IDebugSymbols::GetOffsetByName().  These provide stable C-linkage
/// entry points to locate JIT runtime data structures in the target process.
///
/// The SOS extension reads these pointer values, then calls ReadVirtual to
/// reconstruct JIT subsystem state using the struct layouts defined here.
///
/// Pattern: gc_debug_contract.h (GC SOS contract)

#include <cstdint>

extern "C" {

/// Pointer to the JIT debug entries array (JitDebugEntry[]).
/// Each entry corresponds to one T4-compiled method.
extern void* g_chaos_jit_debug_entries_ptr;

/// Pointer to g_chaos_jit_debug_entry_count (uint32_t).
extern void* g_chaos_jit_debug_entry_count_ptr;

/// Maximum number of debug entries (capacity).
extern int32_t g_chaos_jit_debug_max_entries;

/// Pointer to UnifiedMetadataRegistryV0 for method name resolution.
extern void* g_chaos_metadata_registry_ptr;

}  // extern "C"

// ── Public API for runtime components ──────────────────────────────────
// Called by jit_seh.cpp to sync the debug mirror when T4 code is registered.
void JitDebugContractAddEntry(void* code_start, uint32_t code_size,
                              const void* nm, uint32_t patch_method_token) noexcept;

// Called during bootstrap to set the UnifiedMetadataRegistryV0 pointer.
void JitDebugContractInitMetadataRegistry(const void* unified_registry) noexcept;

#endif  // CHAOS_IL2CPP_JIT_DEBUG_CONTRACT_H_
