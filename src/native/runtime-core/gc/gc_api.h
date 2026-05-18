#ifndef CHAOS_IL2CPP_GC_API_H_
#define CHAOS_IL2CPP_GC_API_H_

#include <chaos/native_types.h>
#include <runtime_abi.h>

namespace chaos::il2cpp::runtime_core {

// ── Platform memory status (abstracted from GlobalMemoryStatusEx) ──

/// Platform-agnostic memory status data.
/// Populated by GetPlatformMemoryStatus().
struct MemoryStatusData {
    int64_t total_phys;   ///< Total physical memory in bytes
    int64_t avail_phys;   ///< Available physical memory in bytes
};

/// Query the OS for current memory status.
/// Win32: calls GlobalMemoryStatusEx.  Other platforms: returns zero.
void GetPlatformMemoryStatus(MemoryStatusData& out) noexcept;

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

/// Trigger GC.Collect with generation and mode.
/// @param generation  Max generation to collect (0=young, 1=full, 2=all).
/// @param mode        GCCollectionMode: 0=Default, 1=Forced, 2=Optimized, 3=Aggressive.
extern "C" void CHAOS_RUNTIME_ABI_CALL chaos_gc_collect_with_mode(
    CHAOS_IL2CPP_INT32 generation, CHAOS_IL2CPP_INT32 mode) noexcept;

/// Get the current GCLatencyMode.
/// Returns: 0=Batch, 1=Interactive, 2=LowLatency, 3=SustainedLowLatency, 4=NoGCRegion.
extern "C" CHAOS_IL2CPP_INT32 CHAOS_RUNTIME_ABI_CALL chaos_gc_get_latency_mode() noexcept;

/// Set the GCLatencyMode.
/// @param mode  GCLatencyMode: 0=Batch, 1=Interactive, 2=LowLatency, 3=SustainedLowLatency.
extern "C" void CHAOS_RUNTIME_ABI_CALL chaos_gc_set_latency_mode(
    CHAOS_IL2CPP_INT32 mode) noexcept;

/// Get GC memory info snapshot (for GC.GetGCMemoryInfo()).
/// Returns heap size bytes (total allocated in old gen + LOH).
/// The full GcMemoryInfoNative struct is obtainable via chaos_gc_get_memory_info().
extern "C" CHAOS_IL2CPP_INT64 CHAOS_RUNTIME_ABI_CALL chaos_gc_get_heap_size() noexcept;

/// Populate a GcMemoryInfoNative struct with the current GC memory snapshot.
/// @param out  Pointer to a GcMemoryInfoNative struct to fill.
extern "C" void CHAOS_RUNTIME_ABI_CALL chaos_gc_get_memory_info(void* out) noexcept;

/// Check if a pointer resides in the GC-managed heap.
/// Returns true when @a ptr points at or above the managed heap base
/// (i.e., within the card-table-covered address range).  Stack-allocated
/// value types return false, allowing callers to skip the write barrier.
extern "C" bool CHAOS_RUNTIME_ABI_CALL chaos_is_gc_pointer(const void* ptr) noexcept;

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_API_H_
