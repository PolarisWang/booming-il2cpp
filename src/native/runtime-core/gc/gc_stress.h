#ifndef CHAOS_IL2CPP_GC_STRESS_H_
#define CHAOS_IL2CPP_GC_STRESS_H_

#include <chaos/native_types.h>

#include <atomic>
#include <cstdint>

namespace chaos::il2cpp::runtime_core {

// ======================================================================
// GCStress Mode — Debug-only aggressive GC triggering
//
// == Design ==
// In DEBUG builds, GCStress can be enabled via the CHAOS_GC_STRESS_MODE
// environment variable:
//   0 = disabled (default for release)
//   1 = stress: trigger GC after every allocation
//   2 = verify: verify heap consistency after every GC
//   3 = stress + verify: both modes simultaneously
//
// In SHIP builds, all GcStress functions compile to no-ops.
// ======================================================================

/// Current GCStress mode (0 = disabled).
/// Initialized once at startup from the environment variable.
extern std::atomic<int> g_gc_stress_mode;

/// Initialize GCStress mode from environment (called during Init).
void GcStressInit() noexcept;

/// Called by allocation paths to trigger stress GC.
/// In GCStress mode 1 or 3, triggers a full collection.
void GcStressCheckAlloc() noexcept;

/// Called by GC collection at various phase boundaries to verify heap.
/// In GCStress mode 2 or 3, calls GcVerifyHeap().
void GcStressCheckVerify() noexcept;

// ======================================================================
// Heap Verification
// ======================================================================

/// Verify the entire old-gen heap for consistency.
/// Checks:
///   - Free list integrity (no out-of-bounds blocks, no double-frees)
///   - Mark bitmap consistency (no mark bits set after sweep)
///   - Card table consistency (all cross-generation refs have dirty cards)
///   - Finalizer table integrity (all entries point to valid old-gen pages)
///
/// @return true if the heap is consistent, false if corruption found.
///         In DEBUG builds, asserts on corruption.
bool GcVerifyHeap() noexcept;

/// Verify a single page's internal consistency.
bool GcVerifyPage(const void* page) noexcept;

/// Verify card table entries are consistent with old-gen pages.
bool GcVerifyCardTable() noexcept;

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_STRESS_H_
