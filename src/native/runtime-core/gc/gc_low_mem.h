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
/// Other platforms: Start() is a no-op (stub).
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
    void* platform_supported_{nullptr};  // Stores the HANDLE* array (2 handles: low-memory notification + shutdown event).
};

/// Process-wide low-memory monitor instance.
extern GcLowMemoryMonitor g_low_memory_monitor;

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_LOW_MEM_H_
