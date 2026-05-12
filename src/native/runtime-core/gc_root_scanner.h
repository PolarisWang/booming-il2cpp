#ifndef CHAOS_IL2CPP_GC_ROOT_SCANNER_H_
#define CHAOS_IL2CPP_GC_ROOT_SCANNER_H_

#include <chaos/native_types.h>
#include <chaos/log.h>

#include <cstddef>
#include <cstdint>

#include "codegen_bridge.h"

// ======================================================================
// CRAG precise root scanner — GcSlotMap-based stack scanning
//
// During GC, the scanner walks each thread's managed stack frames and uses
// codegen-emitted GcSlotMap entries to precisely identify object references.
// Frames without a GcSlotMap are scanned conservatively (treated as raw
// pointer memory).
//
// == Scanning strategy ==
// 1. Walk the managed frame chain (FastFrame / interpreter frames)
// 2. For each frame: look up its GcSlotMap by PC/function address
//    - Found → precise scan of frame slots (exact root reporting)
//    - Not found → conservative scan of entire frame (BDWGC fallback)
// 3. Non-managed frames (native/P/Invoke without managed caller context)
//    are skipped — they're in preemptive mode and don't hold GC roots.
// ======================================================================

namespace chaos::il2cpp::runtime_core {

// ── Root scanner callbacks ─────────────────────────────────────

/// Callback invoked for each precise GC root found during scanning.
/// @param root_addr  Address of the root slot (pointer to the reference).
/// @param is_interior  Whether this is an interior/byref pointer.
using GcRootCallback = void (*)(void* root_addr, bool is_interior, void* user_data);

/// Callback invoked for each conservative root candidate.
/// Conservative roots are potential pointers (may be false positives).
/// @param candidate  The value found in the slot (suspected object pointer).
using GcConservativeRootCallback = void (*)(void* candidate, void* user_data);

// ── Managed frame descriptor (runtime-built) ───────────────────

/// Describes a single managed stack frame for root scanning.
/// Populated by walking the managed frame chain (FastFrame / interpreter).
struct ManagedFrameInfo {
    void*   frame_ptr;       // Base address of the stack frame
    uint32_t frame_size;     // Size of the frame in bytes
    void*   return_address;  // Return PC (used for GcSlotMap lookup)
};

// ── GcSlotMap registry ─────────────────────────────────────────

/// Register a GcSlotMap for a method at @a code_address.
/// Called during module registration (generated code startup).
void GcRegisterSlotMap(const void* code_address, const GcSlotMapV0* slot_map);

/// Look up a GcSlotMap for a given code address.
/// Returns nullptr if no precise map is registered (fall back to conservative).
const GcSlotMapV0* GcLookupSlotMap(const void* code_address);

/// Register all GcSlotMaps from a contiguous section range.
/// The @a begin and @a end pointers are linker-provided bounds of
/// the .gc_slot_maps section.
void GcRegisterSlotMapsFromSection(
    const GcSlotMapV0* begin,
    const GcSlotMapV0* end);

// ── Root scanning entry points ─────────────────────────────────

/// Scan a single managed frame using its GcSlotMap.
/// Calls @a callback for each precise root found.
void GcScanPreciseFrame(
    const ManagedFrameInfo& frame,
    const GcSlotMapV0& slot_map,
    GcRootCallback callback,
    void* user_data);

/// Conservatively scan an unknown frame (all pointer-aligned slots).
void GcScanConservativeFrame(
    const ManagedFrameInfo& frame,
    GcConservativeRootCallback callback,
    void* user_data);

/// Hybrid scan: tries precise first, falls back to conservative.
/// Calls @a precise_callback for known frames, @a conservative_callback
/// for unknown frames and any slots the precise scanner can't classify.
void GcScanFrameHybrid(
    const ManagedFrameInfo& frame,
    GcRootCallback precise_callback,
    GcConservativeRootCallback conservative_callback,
    void* user_data);

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_ROOT_SCANNER_H_