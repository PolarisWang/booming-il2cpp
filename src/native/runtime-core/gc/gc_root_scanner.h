#ifndef CHAOS_IL2CPP_GC_ROOT_SCANNER_H_
#define CHAOS_IL2CPP_GC_ROOT_SCANNER_H_

#include <chaos/native_types.h>
#include <chaos/log.h>

#include <atomic>
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

/// Unregister a GcSlotMap by code address.
/// Called during T4 demotion to remove stale slot map entries.
void GcUnregisterSlotMap(const void* code_address) noexcept;

/// Look up a GcSlotMap for a given code address.
/// Returns nullptr if no precise map is registered (fall back to conservative).
const GcSlotMapV0* GcLookupSlotMap(const void* code_address);

/// Register all GcSlotMaps from a contiguous section range.
/// Each entry in the section is a GcSlotMapSectionEntryHdrV0 followed by
/// the GcSlotMapV0 body (frame_size + num_gc_slots + slots[]).
/// The @a begin and @a end pointers are the linker-provided bounds of
/// the .gc_slot_maps section (or registered by AOT module load).
void GcRegisterSlotMapsFromSection(
    const void* begin,
    const void* end);

// ── Root scanning entry points ─────────────────────────────────

/// Scan a single managed frame using its GcSlotMap.
/// Calls @a callback for each precise root found.
void GcScanPreciseFrame(
    const ManagedFrameInfo& frame,
    const GcSlotMapV0& slot_map,
    GcRootCallback callback,
    void* user_data);

/// Scan a single managed frame using its per-safepoint precise root map
/// (GcPointMapV0, T2.2).  Locates the GC safepoint covering the frame's
/// return address (binary search on native offset) and reports ONLY the roots
/// live at that safepoint — stack slots at @a frame.frame_ptr + offset, and
/// (Task B) volatile registers.  When the return address falls in a gap
/// between safepoints, the nearest-prior safepoint is used (frames are only
/// interruptible at recorded points, so a GC stop can only happen there).
/// @a gpr_values (may be null) is an optional [num_gprs] physical-GPR value
/// file captured at GC suspension; when non-null, the safepoint's live
/// volatile-register roots are also reported (Phase 2).  Stack-slot reporting
/// is always the primary path (never under-retains); register roots are added
/// on top when the window is available.
void GcScanPreciseSafepoint(
    const ManagedFrameInfo& frame,
    const GcPointMapV0& point_map,
    const void* code_start,
    const void* const* gpr_values,   // optional [num_gprs]; null = no register roots
    uint32_t num_gprs,
    GcRootCallback callback,
    void* user_data);

/// Scan the volatile (caller-saved) register roots of a safepoint from an
/// explicit register-value file.  @a gpr_values is an array indexed by
/// physical x64 GPR number holding each register's value at the safepoint;
/// @a saX64 register file covers RAX(0)..R15(15).  Only registers set in the
/// safepoint's live volatile-reg mask are reported, and only when @a
/// save_volatile is nonzero (indicating the caller has captured the volatile
/// regs).  This decouples the register-window capture (GC-suspension
/// policy) from the root decoding, which is unit-testable in isolation.
void GcScanSafepointRegisterRoots(
    const GcSafepointV0& safepoint,
    const void* const* gpr_values,   // [num_gprs] physical GPR values
    uint32_t num_gprs,
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

// ── Interpreter frame scanner (cross-module callback) ─────────────

/// Callback type for interpreter frame chain scanning.
/// root_frame: the head of the interpreter frame chain (FastFrame or RegisterFrame).
/// callback: GcRootCallback to report each precise root.
/// user_data: opaque pointer passed through to callback.
using GcScanInterpFramesFn = void (*)(void* root_frame,
                                      GcRootCallback callback,
                                      void* user_data);

/// Register the interpreter frame scanner callback.
/// Called during interpreter module initialization.  Only one scanner
/// can be registered at a time.  Passing nullptr clears the registration.
void GcSetInterpFrameScanner(GcScanInterpFramesFn fn) noexcept;

/// Get the registered interpreter frame scanner, or nullptr.
GcScanInterpFramesFn GcGetInterpFrameScanner() noexcept;

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_ROOT_SCANNER_H_