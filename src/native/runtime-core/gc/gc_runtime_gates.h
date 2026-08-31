#ifndef CHAOS_IL2CPP_GC_RUNTIME_GATES_H_
#define CHAOS_IL2CPP_GC_RUNTIME_GATES_H_

#include <chrono>
#include <cstdint>

namespace chaos::il2cpp::runtime_core {

// ── GC runtime gates (unified control surface) ──────────────────────────────
//
// Problem being solved: entrypoints (generated app_main.cpp, verification
// entry.cpp) used to disable GC background features by directly writing two
// internal process-wide globals (g_bgc_enabled, g_low_mem_enabled).  That
// (a) scattered control of runtime internals across caller sites, (b) made
// "disable for short-lived process" an implicit silent global mutation with no
// declared reason, and (c) — worst — used BGC/low-mem *disablement as a way to
// side-step a startup safepoint hang, masking the root cause instead of fixing
// it.
//
// This header provides the single semantic control surface an entrypoint
// declares *itself* with, and the runtime uses to decide which GC background
// features apply.  Callers never touch the internal booleans directly; they
// call ApplyGcRuntimeGates(GcRuntimeGates::For(profile)) BEFORE RuntimeInit().
//
// Default (kDefault): everything on — production apps get the full GC feature
// set (concurrent BGC + OS low-memory OOM protection).  Only an entry that
// genuinely needs deterministic single-thread measurement opts into
// kDeterministicMeasurement.  This inverts the old "default-off, occasionally
// on" bias into "default-on, explicitly opt-out with a declared reason".

/// Declared intent of a process entrypoint, used to select GC runtime features.
/// These are SEMANTIC categories — an entry states what it needs, not which
/// internal flags to flip.
enum class GcRuntimeProfile : uint8_t {
    /// Production / general application.  BGC concurrent collection + the OS
    /// low-memory monitor are both enabled (full OOM protection).
    kDefault = 0,
    /// Deterministic measurement / benchmark harness.  BGC concurrency and the
    /// low-memory monitor are disabled to keep allocations/GCs reproducible on a
    /// single thread.  Only select this for measurement entrypoints, not for
    /// real applications.
    kDeterministicMeasurement = 1,
};

/// The concrete feature gates an entrypoint wants.  Immutable snapshot; use
/// GcRuntimeGates::For(profile) to obtain one.
struct GcRuntimeGates {
    bool bgc_enabled = true;
    bool low_mem_monitor_enabled = true;

    static GcRuntimeGates For(GcRuntimeProfile profile) noexcept {
        if (profile == GcRuntimeProfile::kDeterministicMeasurement) {
            return {false, false};
        }
        return {true, true};  // kDefault — full protection.
    }
};

/// Apply the declared gates to the runtime-global GC feature switches.
/// MUST be called before RuntimeInit() (runtime_init.cpp reads these flags to
/// decide whether to start the BGC thread and the low-memory monitor).  Safe to
/// call more than once; the last call wins (idempotent to a single profile).
void ApplyGcRuntimeGates(GcRuntimeGates gates) noexcept;

/// Perform a bounded startup GC-vitality self-check.
///
/// After RuntimeInit() has started the BGC thread (when enabled), this performs
/// a single global-safepoint round-trip with a wall-clock budget and reports
/// whether the collector reached a quiescent point within budget.  If it stalls
/// (e.g. a background thread fails to ack, forcing a hard-timeout conservative
/// scan), it logs a clear diagnostic and — when @a out_healthy is non-null —
/// reports false so the caller can decide whether to fall back.
///
/// Rationale: the historical app_main hardcode disabled BGC entirely to dodge a
/// "safepoint hang".  That masked the root cause.  Removing the hardcode and
/// enabling BGC by default is only sound if a stall is (a) bounded and (b)
/// surfaced, not silently dead.  This check makes the health of the startup
/// safepoint an observable, runtime-verifiable fact rather than an assumption.
/// Returns true if a safepoint was acquired+released within budget.
bool GcStartupVitalityCheck(std::chrono::nanoseconds budget, bool* out_healthy = nullptr) noexcept;

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_RUNTIME_GATES_H_
