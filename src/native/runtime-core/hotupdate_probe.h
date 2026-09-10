// hotupdate_probe.h — Hot-update semantic observability probe.
//
// AOT-only builds (no JIT) execute patched methods through the interpreter.
// PatchDataExtractor rewrites every patch-subject IL body to a Tiny-format
// sentinel ("ldc.i4 0xBEEF0000|idx; ret"), so a correctly-dispatched patch
// produces a return value that differs from the baseline AOT value.  That is
// the primary semantic signal.
//
// This probe is the independent second signal: it records which subjects
// actually executed patched code, so verification can distinguish "the patch
// ran and produced a different value" from "the patch never ran but the
// values happened to differ".  A subject that is marked has demonstrably
// executed through the interpreter.
//
// Contract:
//   - Probe indices are assigned by the patch pipeline, one per subject.
//   - Marking is idempotent and cumulative for the process lifetime.
//   - The harness resets between phases and reads the bitset afterwards.
//
// Threading: mark/reset use relaxed atomics on a word array.  Marking is
// lock-free — it sits on the patched-method entry path.
#pragma once

#include <chaos/native_types.h>

namespace chaos::il2cpp::runtime_core {

/// Maximum number of probe subjects tracked in the bitset.
/// Sized to cover foundation-dll chunks with headroom; indices beyond this
/// are ignored (the mark is dropped, not treated as a failure).
constexpr uint32_t kHotupdateProbeCapacity = 4096;

/// Record that patch subject `index` executed patched code.
/// Out-of-range indices are ignored.
void HotupdateProbeMark(uint32_t index) noexcept;

/// True if `index` has been marked since the last HotupdateProbeReset.
bool HotupdateProbeIsMarked(uint32_t index) noexcept;

/// Number of distinct subjects marked since the last reset.
uint32_t HotupdateProbeMarkedCount() noexcept;

/// Copy the bitset into caller storage (one bit per index, word 0 bit 0 =
/// index 0).  Returns the number of words written.
uint32_t HotupdateProbeSnapshot(uint64_t* out_words, uint32_t word_capacity) noexcept;

/// Clear all marks.  Called between baseline and patched runs.
void HotupdateProbeReset() noexcept;

}  // namespace chaos::il2cpp::runtime_core

extern "C" {

/// C-linkage entry point emitted into patch code.
/// Must remain importable by the external-runtime symbol table.
void chaos_hotupdate_probe_mark(CHAOS_IL2CPP_UINT32 subject_index) noexcept;

/// C-linkage reset for the harness to call before each run.
void chaos_hotupdate_probe_reset(void) noexcept;

/// C-linkage readout of the marked-subject count for the harness.
CHAOS_IL2CPP_UINT32 chaos_hotupdate_probe_marked_count(void) noexcept;

}  // extern "C"
