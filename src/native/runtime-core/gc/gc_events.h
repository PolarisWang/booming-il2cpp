#ifndef CHAOS_IL2CPP_GC_EVENTS_H_
#define CHAOS_IL2CPP_GC_EVENTS_H_

#include <chaos/native_types.h>

#include <atomic>
#include <cstdint>
#include <cstring>
#include <utility>
#include <vector>

namespace chaos::il2cpp::runtime_core {

// ======================================================================
// GC Event Callbacks — online GC lifecycle notifications
//
// == Design ==
// CoreCLR-aligned event system: registers up to 8 callbacks that fire at
// key GC phase boundaries.  Used by the engine integration layer for
// metrics, logging, and adaptive tuning.
//
// == Events ==
// GC_START:       Full GC has begun (safepoint acquired, roots being marked)
// MARK_DONE:      Transitive closure marking complete (parallel or sequential)
// SWEEP_DONE:     All pages swept (free lists rebuilt)
// COMPACT_DONE:   Compaction complete (if applicable, otherwise same as SWEEP_DONE)
// GC_END:         Full GC complete (threads resumed)
// ======================================================================

/// GC event types.
enum class GcEvent : uint8_t {
    GC_START       = 0,    // Full GC began (kept for compatibility)
    MARK_DONE      = 1,    // Mark phase complete
    SWEEP_DONE     = 2,    // Sweep phase complete (free lists rebuilt)
    COMPACT_DONE   = 3,    // Compaction complete (if applicable)
    GC_END         = 4,    // Full GC complete (kept for compatibility)
    GC_YOUNG_START = 5,    // Young GC began
    GC_YOUNG_DONE  = 6,    // Young GC completed (with stats)
    GC_FULL_START  = 7,    // Full GC began
    GC_FULL_DONE   = 8,    // Full GC completed (with stats)
    GC_OOM         = 9,    // Out-of-memory condition (allocation failed after GC)
};

/// Maximum number of registered callbacks.
static constexpr int kMaxGcEventCallbacks = 8;

/// Event callback signature.
/// @param event     The GC event that fired.
/// @param user_data  Opaque user data registered with the callback.
using GcEventCallback = void (*)(GcEvent event, void* user_data);

/// A slot in the global event callback table.
struct GcEventSlot {
    std::atomic<GcEventCallback> callback{nullptr};
    void* user_data = nullptr;
};

/// Global event callback table (defined in gc_events.cpp).
extern GcEventSlot g_gc_event_slots[kMaxGcEventCallbacks];

/// Register a callback for GC events.
/// Returns true on success, false if the callback table is full.
/// Thread-safe (uses atomic CAS on the callback table slot).
/// @param callback   Function pointer to invoke on each event.
/// @param user_data  Opaque data passed back to the callback.
bool GcRegisterEventCallback(GcEventCallback callback, void* user_data);

/// Fire a GC event to all registered callbacks.
/// Called from within the GC safepoint (STW).
inline void GcFireEvent(GcEvent event) noexcept {
    for (int i = 0; i < kMaxGcEventCallbacks; i++) {
        auto cb = g_gc_event_slots[i].callback.load(std::memory_order_acquire);
        if (cb != nullptr) {
            cb(event, g_gc_event_slots[i].user_data);
        }
    }
}

/// Iterate all entries in the GCHandle table (defined in engine_lifecycle.cpp).
/// Calls @a callback for each non-null object instance.
void GcIterateHandleTable(void (*callback)(void* object, void* user_data),
                           void* user_data) noexcept;

/// Iterate only handles pointing to tenured (old-gen / LOH) objects.
/// Skips nursery pointers for full GC mark-phase efficiency.
void GcIterateTenuredHandles(void (*callback)(void* object, void* user_data),
                              void* user_data) noexcept;

/// Iterate only handles pointing to nursery objects.
/// Useful for young GC post-processing (e.g., weak handle nulling).
void GcIterateNurseryHandles(void (*callback)(void* object, void* user_data),
                              void* user_data) noexcept;

/// Iterate only strong (non-weak, non-pinned) handles.
void GcIterateStrongHandles(void (*callback)(void* object, void* user_data),
                             void* user_data) noexcept;

/// Iterate only weak handles.
void GcIterateWeakHandles(void (*callback)(void* object, void* user_data),
                           void* user_data) noexcept;

/// Iterate only pinned handles.
void GcIteratePinnedHandles(void (*callback)(void* object, void* user_data),
                             void* user_data) noexcept;

/// Process weak GCHandles after a young GC:
///   - Nursery objects that were promoted → update handle to forwarding address
///   - Nursery objects that were NOT promoted → null the handle
/// Called between young GC Phase 3 (BFS done) and Phase 4 (nursery sweep).
void GcProcessWeakHandlesAfterYoungGC() noexcept;

// ── DependentHandle API (for ConditionalWeakTable / Ephemeron) ─────────

/// Allocate a new dependent handle (primary → secondary).
CHAOS_IL2CPP_UINT64 GcCreateDependentHandle(void* primary, void* secondary) noexcept;

/// Get the primary object of a dependent handle.
void* GcGetDependentHandlePrimary(CHAOS_IL2CPP_UINT64 handle_id) noexcept;

/// Get the secondary object of a dependent handle.
void* GcGetDependentHandleSecondary(CHAOS_IL2CPP_UINT64 handle_id) noexcept;

/// Set the secondary object of a dependent handle.
void GcSetDependentHandleSecondary(CHAOS_IL2CPP_UINT64 handle_id, void* secondary) noexcept;

/// Free a dependent handle.
void GcFreeDependentHandle(CHAOS_IL2CPP_UINT64 handle_id) noexcept;

/// Process dependent handles after a young GC (promote secondary with primary).
void GcProcessDependentHandlesAfterYoungGC() noexcept;

/// Process dependent handles after a full GC (fixed-point iteration up to 3 rounds).
int GcProcessDependentHandlesAfterFullGC() noexcept;

/// Collect weak handle entries whose objects died during BGC marking.
/// Must be called during/after BgcSweep while the mark bitmap is still
/// valid (before StwCompact clears it).  Returns handle IDs that need
/// nulling.  The actual nulling happens after finalization so that
/// WeakTrackResurrection semantics are preserved.
/// @param out_dead  Output vector receiving (handle_id, old_object) pairs.
void GcCollectDeadWeakHandles(
    std::vector<std::pair<uint64_t, void*>>& out_dead) noexcept;

/// Null weak handles from a list collected by GcCollectDeadWeakHandles.
/// Checks that the handle still points to the original dead object
/// (respecting resurrection via WeakTrackResurrection).
/// @param dead_handles  List of (handle_id, old_object) from collection.
void GcProcessCollectedWeakHandles(
    const std::vector<std::pair<uint64_t, void*>>& dead_handles) noexcept;

/// Process dependent handles after a BGC cycle (fixed-point iteration).
/// Called after GcProcessWeakHandlesAfterBgc().
int GcProcessDependentHandlesAfterBgc() noexcept;

/// Relocate GCHandles that point to moved objects (LOH compaction).
/// Walks the handle table and updates any entry whose object_instance
/// matches old_addr → new_addr in @a relocations.
void GcRelocateHandles(
    const std::vector<std::pair<void*, void*>>& relocations) noexcept;

// ── Pinned object set (for GCHandleType.Pinned / POH) ──────────────────

/// Register a pinned object (prevented from being moved by young GC).
/// Called when GCHandleType.Pinned is created.
void GcAddPinnedObject(void* obj) noexcept;

/// Unregister a pinned object (allow normal GC behavior again).
/// Called when a pinned GCHandle is freed.
void GcRemovePinnedObject(void* obj) noexcept;

/// Check if an object is currently pinned (preventing young GC copy).
bool GcIsPinnedObject(void* obj) noexcept;

/// Allocate an object directly in the Pinned Object Heap (POH).
/// Returns zeroed memory.  POH objects never participate in young GC
/// copying — they are allocated in separate REGION_POH regions and use
/// mark-sweep collection (same as old gen).
/// This is the preferred path for allocations that the caller knows
/// will be pinned (e.g., fixed buffers, GCHandleType.Pinned targets).
void* GcAllocatePinned(CHAOS_IL2CPP_SIZE size) noexcept;

/// Check if @a ptr falls within a POH region.
bool GcIsPohPointer(const void* ptr) noexcept;

/// Read the target object of any GCHandle (strong, weak, pinned).
/// Returns nullptr if handle_id is invalid or the target was collected.
void* GcGetHandleTarget(CHAOS_IL2CPP_UINT64 handle_id) noexcept;

/// Overwrite the target object of any GCHandle.
void GcSetHandleTarget(CHAOS_IL2CPP_UINT64 handle_id, void* new_target) noexcept;

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_EVENTS_H_