#include "gc_low_mem.h"

#include <chaos/log.h>

#include "gc_helpers.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#endif

namespace chaos::il2cpp::runtime_core {

/// Minimum interval (ns) between consecutive low-memory-triggered GCs.
/// Prevents thrashing when the OS keeps the low-memory signal asserted
/// after a single GC is insufficient to relieve pressure.
static constexpr uint64_t kMinLowMemGcIntervalNs = 10ULL * 1000 * 1000 * 1000;  // 10 s

#if !defined(_WIN32)
// ── Linux PSI (Pressure Stall Information) implementation ──────────────
//
// Uses /proc/pressure/memory with poll(POLLPRI) to receive kernel-level
// memory pressure notifications (available since Linux 4.20).
//
// Shutdown signaling uses a self-pipe (eventfd could also work but pipe
// is more portable across POSIX systems).
//
// The PSI FD is monitored with POLLPRI; the shutdown-pipe read end is
// monitored with POLLIN.

/// Open the PSI memory pressure file and configure the notification threshold.
/// Returns a valid fd, or -1 on failure.
static int OpenPsiFd() noexcept {
    int fd = ::open("/proc/pressure/memory", O_RDWR | O_CLOEXEC);
    if (fd < 0) {
        CHAOS_IL2CPP_LOG_WARN_M("GcLowMem",
            "open(/proc/pressure/memory) failed: %s", std::strerror(errno));
        return -1;
    }
    // Write threshold: "some 50000 1000000" = notify after 50ms stall in 1s window.
    // Conservative enough to avoid spurious wakeups, aggressive enough to trigger
    // before the OOM killer.
    const char* threshold = "some 50000 1000000";
    ssize_t written = ::write(fd, threshold, std::strlen(threshold));
    if (written < 0) {
        CHAOS_IL2CPP_LOG_WARN_M("GcLowMem",
            "write PSI threshold failed: %s", std::strerror(errno));
        ::close(fd);
        return -1;
    }
    return fd;
}

/// Create a self-pipe for shutdown signaling.
/// Returns 0 on success, -1 on failure.
static int CreateShutdownPipe(int pipe_fds[2]) noexcept {
    if (::pipe2(pipe_fds, O_CLOEXEC) != 0) {
        CHAOS_IL2CPP_LOG_WARN_M("GcLowMem",
            "pipe2() failed: %s", std::strerror(errno));
        return -1;
    }
    return 0;
}
#endif  // !defined(_WIN32)

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
    // ── Linux PSI path ────────────────────────────────────────────────
    int psi_fd = OpenPsiFd();
    if (psi_fd < 0) {
        platform_supported_ = 0;
        return;
    }

    int pipe_fds[2];
    if (CreateShutdownPipe(pipe_fds) != 0) {
        ::close(psi_fd);
        platform_supported_ = 0;
        return;
    }

    // Pack psi_fd + shutdown pipe ends into a small array (owned by the
    // monitor thread; freed in Stop() via delete[]).
    int* fds = new int[3]{psi_fd, pipe_fds[0], pipe_fds[1]};
    platform_supported_ = fds;
    shutdown_.store(false, std::memory_order_relaxed);

    monitor_thread_ = std::thread([this]() noexcept { MonitorLoop(); });
    CHAOS_IL2CPP_LOG_INFO_M("GcLowMem", "low-memory monitor started (PSI fd=%d)", psi_fd);
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
#else
    int* fds = reinterpret_cast<int*>(platform_supported_);
    // Write a byte to the shutdown pipe to wake the monitor thread.
    char c = 0;
    ssize_t written = ::write(fds[2], &c, 1);
    (void)written;  // Best-effort: failure means the thread is already shutting down.
#endif

    if (monitor_thread_.joinable()) {
        monitor_thread_.join();
    }

#ifdef _WIN32
    HANDLE* handles_cleanup = reinterpret_cast<HANDLE*>(platform_supported_);
    CloseHandle(handles_cleanup[0]);  // Low-memory notification handle.
    CloseHandle(handles_cleanup[1]);  // Shutdown event handle.
    delete[] handles_cleanup;
#else
    int* fds_cleanup = reinterpret_cast<int*>(platform_supported_);
    ::close(fds_cleanup[0]);  // PSI fd
    ::close(fds_cleanup[1]);  // Shutdown pipe read end
    ::close(fds_cleanup[2]);  // Shutdown pipe write end
    delete[] fds_cleanup;
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
    // ── Linux PSI monitor loop ────────────────────────────────────────
    int* fds = reinterpret_cast<int*>(platform_supported_);
    int psi_fd = fds[0];
    int shutdown_fd = fds[1];

    uint64_t last_gc_time = 0;

    // Consume any initial POLLPRI event so the first poll() blocks normally.
    // The PSI file starts in a "signaled" state until the initial event is read.
    {
        char buf[256];
        ssize_t n = ::read(psi_fd, buf, sizeof(buf) - 1);
        (void)n;
    }

    while (!shutdown_.load(std::memory_order_acquire)) {
        struct pollfd pfds[2];
        pfds[0].fd = psi_fd;
        pfds[0].events = POLLPRI;
        pfds[0].revents = 0;
        pfds[1].fd = shutdown_fd;
        pfds[1].events = POLLIN;
        pfds[1].revents = 0;

        int ret = ::poll(pfds, 2, -1);  // Infinite timeout.

        if (ret < 0) {
            if (errno == EINTR) continue;  // Interrupted by signal — retry.
            CHAOS_IL2CPP_LOG_WARN_M("GcLowMem",
                "poll() failed: %s", std::strerror(errno));
            std::this_thread::sleep_for(std::chrono::seconds(5));
            continue;
        }

        if (pfds[1].revents & POLLIN) {
            // Shutdown pipe was written to.
            break;
        }

        if (pfds[0].revents & POLLPRI) {
            // ── Memory pressure notification ──────────────────────────
            CHAOS_IL2CPP_LOG_INFO_M("GcLowMem", "PSI memory pressure notification, triggering GC");

            // Drain the PSI event so poll() will block again.
            char buf[256];
            ssize_t n = ::read(psi_fd, buf, sizeof(buf) - 1);
            (void)n;

            // Rate-limit.
            auto now = static_cast<uint64_t>(
                std::chrono::steady_clock::now().time_since_epoch().count());
            if (now - last_gc_time >= kMinLowMemGcIntervalNs) {
                chaos_gc_collect();
                last_gc_time = now;
                CHAOS_IL2CPP_LOG_INFO_M("GcLowMem", "low-memory GC triggered");
            } else {
                CHAOS_IL2CPP_LOG_DEBUG_M("GcLowMem",
                    "low-memory GC skipped (rate-limited)");
            }

            // Brief cooldown.
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
#endif
}

GcLowMemoryMonitor g_low_memory_monitor;

}  // namespace chaos::il2cpp::runtime_core
