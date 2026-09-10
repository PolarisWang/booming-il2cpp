#ifndef CHAOS_IL2CPP_GC_LOW_MEM_H_
#define CHAOS_IL2CPP_GC_LOW_MEM_H_

#include <atomic>
#include <cstdint>
#include <thread>

namespace chaos::il2cpp::runtime_core {

/// Monitors OS low-memory notifications and triggers proactive GC.
///
/// Windows: uses CreateMemoryResourceNotification(LowMemoryResourceNotification)
/// to receive kernel-level low-memory alerts.  When triggered, calls
/// chaos_gc_collect() to reclaim memory before the OOM killer or pagefile
/// pressure becomes critical.
///
/// Linux: uses /proc/pressure/memory (PSI, Linux 4.20+) with poll(POLLPRI)
/// to receive memory pressure notifications.  Threshold configured at 50ms
/// stall in a 1-second monitoring window.
///
/// Unsupported platforms: Start() is a no-op (stub).
class GcLowMemoryMonitor {
public:
    GcLowMemoryMonitor() = default;
    ~GcLowMemoryMonitor() = default;

    GcLowMemoryMonitor(const GcLowMemoryMonitor&) = delete;
    GcLowMemoryMonitor& operator=(const GcLowMemoryMonitor&) = delete;

    /// Start the low-memory monitoring thread.
    /// No-op on unsupported platforms.
    void Start() noexcept;

    /// Signal the monitor thread to shut down and join it.
    void Stop() noexcept;

private:
    void MonitorLoop() noexcept;

    std::thread monitor_thread_;
    std::atomic<bool> shutdown_{false};
    void* platform_supported_{nullptr};  // Windows: HANDLE[2] (low-mem + shutdown).
                                         // Linux:   int[3] (psi_fd + pipe_read + pipe_write).
};

/// Process-wide low-memory monitor instance.
extern GcLowMemoryMonitor g_low_memory_monitor;

/// Gate for the low-memory monitor background thread — mirrors
/// gc_bgc.h `g_bgc_enabled`. Set to false before RuntimeInit() to disable the
/// OS low-memory notification monitor (used by short-lived app/benchmark
/// processes where a low-memory-triggered GC safepoint would hang startup).
extern bool g_low_mem_enabled;

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_LOW_MEM_H_
