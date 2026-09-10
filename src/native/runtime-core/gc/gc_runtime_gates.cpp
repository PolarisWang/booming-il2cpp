#include "gc_runtime_gates.h"

#include <chrono>
#include <chaos/log.h>

#include "gc_helpers.h"  // GetRuntimeMode / RuntimeMode
#include "../runtime_core.h"  // RuntimeMode{Aot,Mixed}
#include "thread_state.h"

// Forward-declare the runtime-global BGC and low-memory-enabled flags.
// These are defined in gc_bgc.cpp and gc_low_mem.cpp respectively, which are
// compiled on all target platforms (the low-memory monitor is a no-op on
// unsupported platforms; BGC compiles to a stub).  Using forward declarations
// here instead of #include "gc_bgc.h" / "gc_low_mem.h" avoids depending on
// those headers being compiled on every platform.
namespace chaos::il2cpp::runtime_core {
extern bool g_bgc_enabled;
extern bool g_low_mem_enabled;
}

namespace chaos::il2cpp::runtime_core {

void ApplyGcRuntimeGates(GcRuntimeGates gates) noexcept {
    // These globals default to true in gc_bgc.cpp / gc_low_mem.cpp.  This
    // function is the single mutation point an entrypoint uses to opt in/out
    // of GC background features, so the process-wide switches stay centralized
    // here rather than sprinkled across caller sites.
    g_bgc_enabled = gates.bgc_enabled;
    g_low_mem_enabled = gates.low_mem_monitor_enabled;

    CHAOS_IL2CPP_LOG_DEBUG_M("GcGates", "applied gates: bgc={0} low_mem={1}",
        gates.bgc_enabled ? 1 : 0, gates.low_mem_monitor_enabled ? 1 : 0);
}

bool GcStartupVitalityCheck(std::chrono::nanoseconds budget,
                            std::chrono::nanoseconds stall_threshold,
                            GcStartupVitality* out_vitality) noexcept {
    using clock = std::chrono::steady_clock;
    auto start = clock::now();

    if (out_vitality) *out_vitality = GcStartupVitality::kHealthy;

    // In pure AOT mode there is no other background thread that can acknowledge
    // the safepoint handshake (no JIT/interpreter worker threads, single-thread
    // startup).  Running the handshake here would always hit the internal 100ms
    // timeout and produce a false negative (spurious "startup coordination
    // regression" warning).  Skip the check in AOT-only mode; report healthy.
    if (GetRuntimeMode() == RuntimeMode::Aot) {
        return true;
    }

    // Perform a single safepoint round-trip with an overall wall-clock budget.
    // RequestGlobalSafepoint already self-bounds via its internal 100ms/500ms
    // timeouts (force-release + conservative scan), so this cannot hang forever;
    // the outer budget is a belt-and-suspenders guard and a way to *report*
    // whether the handshake was prompt or took a hard-timeout stall.
    uint32_t epoch = threading::RequestGlobalSafepoint();
    threading::ReleaseGlobalSafepoint(epoch);

    auto elapsed = clock::now() - start;

    GcStartupVitality vitality;
    if (elapsed <= budget) {
        vitality = GcStartupVitality::kHealthy;
    } else if (elapsed <= stall_threshold) {
        // Over budget but returned within the hard-timeout window — a transient
        // scheduling hiccup that converged.  Log a warning; keep running.
        vitality = GcStartupVitality::kSlowButConverged;
        CHAOS_IL2CPP_LOG_WARN_M("GcGates",
            "startup GC-vitality self-check slow: safepoint round-trip took {0}ms "
            "(budget {1}ms). BGC concurrency may be momentarily degraded; "
            "the hard-timeout conservative scan prevented a hang.",
            static_cast<long long>(std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count()),
            static_cast<long long>(std::chrono::duration_cast<std::chrono::milliseconds>(budget).count()));
    } else {
        // Exceeded the hard-timeout window (stall_threshold) — a genuine
        // coordination stall that forced a conservative-scan release.  Treat as
        // a real startup failure.
        vitality = GcStartupVitality::kStalled;
        CHAOS_IL2CPP_LOG_ERROR_M("GcGates",
            "startup GC-vitality self-check STALLED: safepoint round-trip took {0}ms "
            "(budget {1}ms, hard-timeout {2}ms). A background thread failed to ack; "
            "the conservative scan forced a release — this is a startup "
            "coordination regression, not a transient delay.",
            static_cast<long long>(std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count()),
            static_cast<long long>(std::chrono::duration_cast<std::chrono::milliseconds>(budget).count()),
            static_cast<long long>(std::chrono::duration_cast<std::chrono::milliseconds>(stall_threshold).count()));
    }

    if (out_vitality) *out_vitality = vitality;
    // Return false ONLY on a genuine stall (kStalled); kSlowButConverged is
    // still a "converged" outcome so callers can decide to proceed.
    return vitality != GcStartupVitality::kStalled;
}

}  // namespace chaos::il2cpp::runtime_core
