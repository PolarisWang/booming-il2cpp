#include "gc_runtime_gates.h"

#include <chrono>
#include <chaos/log.h>

#include "gc_bgc.h"      // g_bgc_enabled
#include "gc_low_mem.h"  // g_low_mem_enabled
#include "thread_state.h"

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

bool GcStartupVitalityCheck(std::chrono::nanoseconds budget, bool* out_healthy) noexcept {
    using clock = std::chrono::steady_clock;
    auto start = clock::now();

    if (out_healthy) *out_healthy = true;

    // Perform a single safepoint round-trip with an overall wall-clock budget.
    // RequestGlobalSafepoint already self-bounds via its internal 100ms/500ms
    // timeouts (force-release + conservative scan), so this cannot hang forever;
    // the outer budget is a belt-and-suspenders guard and a way to *report*
    // whether the handshake was prompt or took a hard-timeout stall.
    uint32_t epoch = threading::RequestGlobalSafepoint();
    threading::ReleaseGlobalSafepoint(epoch);

    auto elapsed = clock::now() - start;
    bool healthy = (elapsed <= budget);
    if (out_healthy) *out_healthy = healthy;

    if (!healthy) {
        CHAOS_IL2CPP_LOG_ERROR_M("GcGates",
            "startup GC-vitality self-check SLOW/stalled: safepoint round-trip took {0}ms "
            "(budget {1}ms). Indicates a background thread failed to ack promptly; "
            "the hard-timeout conservative scan prevented a hang but signals a "
            "startup coordination regression.",
            static_cast<long long>(std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count()),
            static_cast<long long>(std::chrono::duration_cast<std::chrono::milliseconds>(budget).count()));
    }
    return healthy;
}

}  // namespace chaos::il2cpp::runtime_core
