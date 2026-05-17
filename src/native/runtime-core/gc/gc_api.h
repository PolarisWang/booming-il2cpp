#ifndef CHAOS_IL2CPP_GC_API_H_
#define CHAOS_IL2CPP_GC_API_H_

#include <chaos/native_types.h>
#include <runtime_abi.h>

namespace chaos::il2cpp::runtime_core {

// ── Managed GC API (System.GC) ─────────────────────────────────────

/// Returns the total number of bytes currently thought to be allocated
/// in the managed GC heap (old gen + LOH).  When @a force_full_collection
/// is non-zero, triggers a full blocking GC first.
extern "C" CHAOS_IL2CPP_INT64 CHAOS_RUNTIME_ABI_CALL chaos_gc_get_total_memory(
    CHAOS_IL2CPP_INT32 force_full_collection) noexcept;

/// Notifies the GC of external (unmanaged) memory pressure.
/// Accumulates in a process-wide counter; when the total external
/// memory exceeds a threshold, a GC is triggered at the next safepoint.
extern "C" void CHAOS_RUNTIME_ABI_CALL chaos_gc_add_memory_pressure(
    CHAOS_IL2CPP_INT64 bytes) noexcept;

/// Notifies the GC that external memory has been freed.
extern "C" void CHAOS_RUNTIME_ABI_CALL chaos_gc_remove_memory_pressure(
    CHAOS_IL2CPP_INT64 bytes) noexcept;

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_API_H_
