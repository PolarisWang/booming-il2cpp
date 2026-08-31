#include "gc_low_mem.h"

#include <chaos/log.h>
#include <chaos/pal/pal_low_mem.h>

#include "gc_helpers.h"

namespace chaos::il2cpp::runtime_core {

using chaos::il2cpp::pal::PalLowMemCreate;
using chaos::il2cpp::pal::PalLowMemWait;
using chaos::il2cpp::pal::PalLowMemMonitor;
using chaos::il2cpp::pal::PalLowMemSignalShutdown;
using chaos::il2cpp::pal::PalLowMemDestroy;

/// Minimum interval (ns) between consecutive low-memory-triggered GCs.
/// Prevents thrashing when the OS keeps the low-memory signal asserted
/// after a single GC is insufficient to relieve pressure.
static constexpr uint64_t kMinLowMemGcIntervalNs = 10ULL * 1000 * 1000 * 1000;  // 10 s

/// Opt-out gate: app/benchmark processes set this false to disable the monitor.
bool g_low_mem_enabled = true;

void GcLowMemoryMonitor::Start() noexcept {
    if (!g_low_mem_enabled) {
        CHAOS_IL2CPP_LOG_INFO_M("GcLowMem", "low-memory monitor disabled (g_low_mem_enabled=false)");
        return;
    }
    auto* monitor = PalLowMemCreate();
    if (monitor == nullptr) {
        CHAOS_IL2CPP_LOG_WARN_M("GcLowMem", "PalLowMemCreate failed — low-memory monitoring disabled");
        platform_supported_ = 0;
        return;
    }

    platform_supported_ = monitor;
    shutdown_.store(false, std::memory_order_relaxed);

    monitor_thread_ = std::thread([this]() noexcept { MonitorLoop(); });
    CHAOS_IL2CPP_LOG_INFO_M("GcLowMem", "low-memory monitor started");
}

void GcLowMemoryMonitor::Stop() noexcept {
    auto* monitor = static_cast<PalLowMemMonitor*>(platform_supported_);
    if (!monitor) {
        if (monitor_thread_.joinable()) {
            monitor_thread_.join();
        }
        return;
    }

    shutdown_.store(true, std::memory_order_release);
    PalLowMemSignalShutdown(monitor);

    if (monitor_thread_.joinable()) {
        monitor_thread_.join();
    }

    PalLowMemDestroy(monitor);
    platform_supported_ = 0;
    CHAOS_IL2CPP_LOG_INFO_M("GcLowMem", "low-memory monitor stopped");
}

void GcLowMemoryMonitor::MonitorLoop() noexcept {
    auto* monitor = static_cast<PalLowMemMonitor*>(platform_supported_);
    uint64_t last_gc_time = 0;

    while (!shutdown_.load(std::memory_order_acquire)) {
        int ret = PalLowMemWait(monitor, -1);  // Infinite timeout.

        if (ret < 0) {
            // Error from underlying wait — retry after a short delay.
            CHAOS_IL2CPP_LOG_WARN_M("GcLowMem", "PalLowMemWait returned error, retrying");
            std::this_thread::sleep_for(std::chrono::seconds(5));
            continue;
        }

        if (ret == 0) {
            // Shutdown was signaled or spurious wakeup — recheck shutdown flag.
            continue;
        }

        // ── Low-memory notification received ──────────────────────────
        // NOTE: PalLowMemWait returns 1 for BOTH a real low-memory notification
        // AND a shutdown-signal wake (it waits on {low_mem_handle, shutdown_event}).
        // If shutdown was requested, the wake is a teardown signal, NOT a real
        // low-memory event — triggering chaos_gc_collect() here would try to run
        // a GC during shutdown (BGC/other threads already stopping), hanging the
        // process.  Re-check the shutdown flag before doing any GC work.
        if (shutdown_.load(std::memory_order_acquire)) {
            return;  // teardown signal — exit the loop, do not GC
        }
        CHAOS_IL2CPP_LOG_INFO_M("GcLowMem", "low-memory notification received, triggering GC");

        // Rate-limit: don't trigger GC more than once per kMinLowMemGcIntervalNs.
        auto now = static_cast<uint64_t>(
            std::chrono::steady_clock::now().time_since_epoch().count());
        if (now - last_gc_time >= kMinLowMemGcIntervalNs) {
            chaos_gc_collect();
            last_gc_time = now;
            CHAOS_IL2CPP_LOG_INFO_M("GcLowMem", "low-memory GC triggered");
        } else {
            CHAOS_IL2CPP_LOG_DEBUG_M("GcLowMem",
                "low-memory GC skipped (rate-limited, %lluns since last)",
                static_cast<unsigned long long>(now - last_gc_time));
        }

        // Brief cooldown to avoid busy-looping on level-triggered notifications.
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

GcLowMemoryMonitor g_low_memory_monitor;

}  // namespace chaos::il2cpp::runtime_core
