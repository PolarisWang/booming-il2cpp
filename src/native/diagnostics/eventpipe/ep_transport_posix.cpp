// ep_transport_posix.cpp — EventPipe IPC transport (POSIX Unix Domain Socket)
//
// Linux Unix Domain Socket server with blocking I/O.
// Single-client (one receiver at a time).  Uses poll() for interruptible
// accept and disconnect detection.

#include "ep_transport.h"

#if CHAOS_IL2CPP_EVENTPIPE == 1

#include "ep_protocol.h"

#include <atomic>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <thread>

#include <poll.h>
#include <unistd.h>

namespace chaos::il2cpp::diagnostics {

namespace {

// ── State ───────────────────────────────────────────────────────────────

EpPlatformHandle g_server_fd = kEpInvalidHandle;
EpPlatformHandle g_client_fd = kEpInvalidHandle;
std::atomic<bool> g_running{false};
std::atomic<bool> g_connected{false};
std::mutex g_write_mutex;

// Socket path buffer.
char g_pipe_path[128] = {};

// Background listener thread.
std::thread g_listener_thread;

// ── Listener thread ─────────────────────────────────────────────────────
//
// Creates a Unix domain socket, waits for client connection, then monitors
// for disconnection.  Uses poll() with a short timeout so shutdown is
// responsive (at most 200 ms delay).

void ListenerThread() noexcept {
    // Create server socket.
    g_server_fd = EpCreatePipe(g_pipe_path);
    if (g_server_fd == kEpInvalidHandle) {
        return;
    }

    while (g_running.load(std::memory_order_acquire)) {
        // Wait for client (poll with timeout for interruptible accept).
        while (g_running.load(std::memory_order_acquire)) {
            struct pollfd pfd;
            pfd.fd = g_server_fd;
            pfd.events = POLLIN;
            int ret = ::poll(&pfd, 1, 200);
            if (ret > 0 && (pfd.revents & POLLIN)) {
                g_client_fd = ::accept(g_server_fd, nullptr, nullptr);
                if (g_client_fd >= 0) {
                    g_connected.store(true, std::memory_order_release);
                    break;
                }
            }
        }

        if (!g_running.load(std::memory_order_acquire)) {
            break;
        }

        // Client connected — wait for disconnect.
        while (g_running.load(std::memory_order_acquire)) {
            struct pollfd pfd;
            pfd.fd = g_client_fd;
            pfd.events = POLLIN;
            int ret = ::poll(&pfd, 1, 200);
            if (ret > 0) {
                // Client sent data or disconnected.
                uint8_t buf;
                ssize_t n = ::read(g_client_fd, &buf, 1);
                if (n <= 0) break;
            }
        }

        g_connected.store(false, std::memory_order_release);

        // Cleanup client.
        EpDisconnectPipe(g_client_fd);
        EpClosePipe(g_client_fd);
        g_client_fd = kEpInvalidHandle;
    }

    EpClosePipe(g_server_fd);
    g_server_fd = kEpInvalidHandle;
}

}  // anonymous namespace

// ── Public API ──────────────────────────────────────────────────────────

bool EpTransportInitialize(uint32_t pid) noexcept {
    if (g_running.load(std::memory_order_relaxed)) {
        return true;
    }

    // Build socket path.
    ::snprintf(g_pipe_path, sizeof(g_pipe_path),
               CHAOS_IL2CPP_EP_PIPE_NAME_TEMPLATE, pid);

    g_running.store(true, std::memory_order_release);
    g_listener_thread = std::thread(ListenerThread);

    return true;
}

void EpTransportShutdown() noexcept {
    g_running.store(false, std::memory_order_release);

    if (g_listener_thread.joinable()) {
        g_listener_thread.join();
    }

    // Cleanup client (listener thread already exited).
    {
        std::lock_guard<std::mutex> lock(g_write_mutex);
        EpDisconnectPipe(g_client_fd);
        EpClosePipe(g_client_fd);
        g_client_fd = kEpInvalidHandle;
    }

    // Server fd was closed by listener thread.
    g_server_fd = kEpInvalidHandle;
    g_connected.store(false, std::memory_order_release);

    // Remove socket file.
    ::unlink(g_pipe_path);
}

void EpTransportWrite(const void* data, uint32_t data_size) noexcept {
    if (!g_connected.load(std::memory_order_acquire)) {
        return;
    }

    std::lock_guard<std::mutex> lock(g_write_mutex);

    if (g_client_fd == kEpInvalidHandle) {
        return;
    }

    bool ok = EpWritePipe(g_client_fd, data, data_size);
    if (!ok) {
        g_connected.store(false, std::memory_order_release);
    }
}

bool EpTransportIsConnected() noexcept {
    return g_connected.load(std::memory_order_acquire);
}

}  // namespace chaos::il2cpp::diagnostics

#endif  // CHAOS_IL2CPP_EVENTPIPE
