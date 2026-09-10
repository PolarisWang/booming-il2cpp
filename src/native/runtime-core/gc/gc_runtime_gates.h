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

/// Outcome of the startup GC-vitality self-check, distinguishing a brief
/// scheduling hiccup from a genuine coordination stall so the caller can choose
/// an appropriate action (WARNING vs FATAL).
enum class GcStartupVitality : uint8_t {
    /// Safepoint round-trip completed within the budget — healthy.
    kHealthy = 0,
    /// Round-trip exceeded the budget but still returned (the internal 100ms
    /// alertable-pause then hard-timeout conservative scan let it converge).
    /// A transient scheduling delay — log a warning, keep running.
    kSlowButConverged = 1,
    /// Round-trip hit the hard-timeout window (>500ms, i.e. the conservative
    /// scan had to force-release a non-acking thread).  This is a real
    /// coordination stall indicator — treat as a startup failure unless the
    /// caller has a documented reason to proceed degraded.
    kStalled = 2,
};

/// Perform a bounded startup GC-vitality self-check.
///
/// After RuntimeInit() has started the BGC thread (when enabled), this performs
/// a single global-safepoint round-trip with a wall-clock budget and reports
/// the outcome via @a out_vitality (see GcStartupVitality).  In AOT-only mode
/// there is no other managed thread to ack, so the check is skipped and reports
/// kHealthy (return true).
///
/// @param budget     Wall-clock budget for a "healthy" round-trip.
/// @param threshold  Elapsed time above which a returned-but-slow handshake is
///                   classified as kStalled rather than kSlowButConverged.
///                   Typical value: 500ms (matches kSafepointHardTimeoutNs).
/// @param out_vitality  Receives the outcome; may be nullptr.
/// @return true      If a safepoint was acquired+released (kHealthy or
///                   kSlowButConverged); false only on kStalled (a genuine
///                   coordination stall).
///
/// Rationale: the historical app_main hardcode disabled BGC entirely to dodge a
/// "safepoint hang".  That masked the root cause.  Removing the hardcode and
/// enabling BGC by default is only sound if a stall is (a) bounded and (b)
/// surfaced, not silently dead.  This check makes the health of the startup
/// safepoint an observable, runtime-verifiable fact rather than an assumption.
bool GcStartupVitalityCheck(std::chrono::nanoseconds budget,
                            std::chrono::nanoseconds stall_threshold,
                            GcStartupVitality* out_vitality = nullptr) noexcept;

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_RUNTIME_GATES_H_
