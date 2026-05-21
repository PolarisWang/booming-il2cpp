#ifndef CHAOS_IL2CPP_GC_API_H_
#define CHAOS_IL2CPP_GC_API_H_

#include <chaos/native_types.h>
#include <runtime_abi.h>

#include "gc_card_table.h"

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
/// @param obj  Managed GCMemoryInfoData object reference (pointer with MethodTable* header).
///             The native function computes the interior pointer past the MethodTable*
///             to write the GcMemoryInfoNative fields directly into the object.
/// @param kind GCKind value (Any=0, Gen=1, Full=2, Background=3).  Currently unused
///             (all requests return the same snapshot).
extern "C" void CHAOS_RUNTIME_ABI_CALL chaos_gc_get_memory_info(
    CHAOS_IL2CPP_INTPTR obj, CHAOS_IL2CPP_INT32 kind) noexcept;

/// Get GC collection count for a specific generation (GC.CollectionCount).
/// @param generation  0=young (gen0), 1=gen1 (survivor), 2+=full (gen2/old).
extern "C" CHAOS_IL2CPP_INT32 CHAOS_RUNTIME_ABI_CALL chaos_gc_get_collection_count(
    CHAOS_IL2CPP_INT32 generation) noexcept;

/// Check if a pointer resides in the GC-managed heap.
/// Inline definition for zero-overhead calls from C++ codegen.
/// Stack-allocated value types return false, so callers can skip
/// the write barrier for nested value-type fields.
static inline bool chaos_is_gc_pointer(const void* ptr) noexcept {
    uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
    // Fast path: g_heap_base is the minimum address registered with the
    // card table, covering old-gen and nursery.  Stack-allocated value
    // types live far below this address.
    if (addr >= g_heap_base) [[likely]] return true;
    // POH regions are independently VirtualAlloc'd and may be below
    // g_heap_base (the card table does not cover POH).  Check via the
    // lock-free POH slot array.
    if (RegionManager::Instance().IsPohPointer(ptr)) return true;
    return false;
}

// ── NO_GC_REGION API ─────────────────────────────────────────────────────

/// Enter a NO_GC_REGION: nested counter prevents any GC from occurring
/// on this thread.  Must be paired with GcLeaveNoGcRegion().
/// Safe to nest: each Enter increments a TLS counter, each Leave decrements.
extern "C" void CHAOS_RUNTIME_ABI_CALL chaos_gc_enter_no_gc_region() noexcept;

/// Leave a NO_GC_REGION.  When the counter reaches zero, any deferred GC
/// that was requested during the region will be executed.
extern "C" void CHAOS_RUNTIME_ABI_CALL chaos_gc_leave_no_gc_region() noexcept;

/// Try to start a NO_GC_REGION with a pre-allocated budget.
/// Returns 1 (true) if the region was started successfully — the calling
/// thread may allocate up to @a total_size bytes without triggering a
/// blocking GC.  Returns 0 (false) if insufficient capacity is available.
/// When @a disallow_full_blocking_gc is non-zero, the region will fail
/// if a full (blocking) GC would be required to satisfy the budget.
/// Corresponds to System.GC.TryStartNoGCRegion(totalSize, disallowFullBlockingGC).
extern "C" CHAOS_IL2CPP_INT32 CHAOS_RUNTIME_ABI_CALL chaos_gc_try_start_no_gc_region(
    CHAOS_IL2CPP_INT64 total_size, CHAOS_IL2CPP_INT32 disallow_full_blocking_gc) noexcept;

/// End the current NO_GC_REGION.  Returns 0 (Success) if no GC was
/// triggered during the region, or 1 (GCTriggered) if a GC was deferred
/// and executed on exit.
extern "C" CHAOS_IL2CPP_INT32 CHAOS_RUNTIME_ABI_CALL chaos_gc_end_no_gc_region() noexcept;

/// Check whether the current thread is inside a NO_GC_REGION.
/// Returns true when the TLS nesting counter > 0.
bool GcIsInNoGcRegion() noexcept;

/// Returns the total accumulated pause time across all GC collections
/// (young + gen1 + full) in nanoseconds.  Corresponds to
/// GC.GetTotalPauseDuration() in the BCL.
extern "C" CHAOS_IL2CPP_INT64 CHAOS_RUNTIME_ABI_CALL chaos_gc_get_total_pause_duration() noexcept;

/// Returns the total number of bytes allocated by the current thread
/// across all generations (nursery, old gen, POH).  Corresponds to
/// GC.GetAllocatedBytesForCurrentThread() in the BCL.
/// This is a monotonically increasing per-thread counter that is never reset.
extern "C" CHAOS_IL2CPP_INT64 CHAOS_RUNTIME_ABI_CALL chaos_gc_get_allocated_bytes_for_current_thread() noexcept;

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_API_H_
