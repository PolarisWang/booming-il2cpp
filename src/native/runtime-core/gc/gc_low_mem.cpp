#include "gc_low_mem.h"

#include <chaos/log.h>

#include "gc_helpers.h"

#ifdef _WIN32
#include <windows.h>
#endif

namespace chaos::il2cpp::runtime_core {

/// Minimum interval (ns) between consecutive low-memory-triggered GCs.
/// Prevents thrashing when the OS keeps the low-memory signal asserted
/// after a single GC is insufficient to relieve pressure.
static constexpr uint64_t kMinLowMemGcIntervalNs = 10ULL * 1000 * 1000 * 1000;  // 10 s

void GcLowMemoryMonitor::Start() noexcept {
#ifdef _WIN32
    HANDLE h = CreateMemoryResourceNotification(LowMemoryResourceNotification);
    if (!h || h == INVALID_HANDLE_VALUE) {
        CHAOS_IL2CPP_LOG_WARN_M("GcLowMem", "CreateMemoryResourceNotification failed (error=%lu)",
                                GetLastError());
        return;
    }

    // Create a manual-reset event for shutdown signaling.
    HANDLE hShutdown = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!hShutdown) {
        CHAOS_IL2CPP_LOG_WARN_M("GcLowMem", "CreateEvent for shutdown failed (error=%lu)",
                                GetLastError());
        CloseHandle(h);
        return;
    }

    // Store handles in the platform_supported_ pointer slots.
    // platform_supported_ != 0 signals that the monitor is active.
    // We pack both handles into a single allocation.
    HANDLE* handles = new HANDLE[2]{h, hShutdown};
    platform_supported_ = handles;
    shutdown_.store(false, std::memory_order_relaxed);

    monitor_thread_ = std::thread([this]() noexcept { MonitorLoop(); });
    CHAOS_IL2CPP_LOG_INFO_M("GcLowMem", "low-memory monitor started");
#else
    CHAOS_IL2CPP_LOG_DEBUG_M("GcLowMem", "low-memory monitor: unsupported platform, skipping");
    platform_supported_ = 0;
#endif
}

void GcLowMemoryMonitor::Stop() noexcept {
    if (!platform_supported_) {
        if (monitor_thread_.joinable()) {
            monitor_thread_.join();
        }
        return;
    }

    shutdown_.store(true, std::memory_order_release);

#ifdef _WIN32
    HANDLE* handles = reinterpret_cast<HANDLE*>(platform_supported_);
    // Signal the shutdown event to wake the monitor thread.
    SetEvent(handles[1]);
#endif

    if (monitor_thread_.joinable()) {
        monitor_thread_.join();
    }

#ifdef _WIN32
    HANDLE* handles_cleanup = reinterpret_cast<HANDLE*>(platform_supported_);
    CloseHandle(handles_cleanup[0]);  // Low-memory notification handle.
    CloseHandle(handles_cleanup[1]);  // Shutdown event handle.
    delete[] handles_cleanup;
#endif

    platform_supported_ = 0;
    CHAOS_IL2CPP_LOG_INFO_M("GcLowMem", "low-memory monitor stopped");
}

void GcLowMemoryMonitor::MonitorLoop() noexcept {
#ifdef _WIN32
    HANDLE* handles = reinterpret_cast<HANDLE*>(platform_supported_);
    HANDLE hLowMem = handles[0];
    HANDLE hShutdown = handles[1];

    uint64_t last_gc_time = 0;

    while (!shutdown_.load(std::memory_order_acquire)) {
        // Wait for either a low-memory notification or shutdown signal.
        DWORD wait_result = WaitForMultipleObjects(2, handles, FALSE, INFINITE);

        if (wait_result == WAIT_OBJECT_0 + 1) {
            // Shutdown event was signaled.
            break;
        }

        if (wait_result != WAIT_OBJECT_0) {
            // Unexpected wait failure.  Log and retry after a short delay.
            CHAOS_IL2CPP_LOG_WARN_M("GcLowMem",
                "WaitForMultipleObjects unexpected result=%lu (error=%lu)",
                wait_result, GetLastError());
            // Brief sleep to avoid spinning on a persistent error.
            std::this_thread::sleep_for(std::chrono::seconds(5));
            continue;
        }

        // ── Low-memory notification received ──────────────────────────
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

        // The low-memory notification is level-triggered: if the OS still
        // reports low memory, WaitForMultipleObjects will return immediately
        // again.  Without a brief cooldown this would busy-loop.  The
        // rate-limiter above handles the GC side, but we also add a small
        // sleep here to keep the thread responsive to shutdown.
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
#else
    // Stub: MonitorLoop should never run on unsupported platforms.
    // If it does (e.g., platform_supported_ set incorrectly), just exit.
#endif
}

GcLowMemoryMonitor g_low_memory_monitor;

}  // namespace chaos::il2cpp::runtime_core
