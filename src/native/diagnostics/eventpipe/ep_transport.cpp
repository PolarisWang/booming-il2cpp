// ep_transport.cpp — EventPipe IPC transport implementation
//
// Windows Named Pipe server with OVERLAPPED I/O.
// Single-client (one receiver at a time).  If the client disconnects,
// the server resets and waits for a new connection.
//
// Linux: stubbed out — EventPipe transport is a no-op.

#include "ep_transport.h"

#if CHAOS_IL2CPP_EVENTPIPE == 1

#include "ep_protocol.h"

#include <atomic>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <thread>

namespace chaos::il2cpp::diagnostics {

#if defined(_WIN32) || defined(_WIN64)

namespace {

// ── State ───────────────────────────────────────────────────────────────

EpPlatformHandle g_pipe = kEpInvalidHandle;
std::atomic<bool> g_running{false};
std::atomic<bool> g_connected{false};
std::mutex g_write_mutex;

// Overlapped I/O structures (one for connect, one for write).
OVERLAPPED g_connect_overlapped = {};
OVERLAPPED g_write_overlapped = {};

// Background listener thread (accepts connections).
std::thread g_listener_thread;

// Pipe name buffer (wchar_t).
wchar_t g_pipe_name[128] = {};

// ── Listener thread ─────────────────────────────────────────────────────
//
// Runs in a background thread: waits for client connections, then exits.
// A new thread is spawned on each connect/disconnect cycle.

void ListenerThread() noexcept {
    // Create connect event.
    g_connect_overlapped.hEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (g_connect_overlapped.hEvent == nullptr) {
        return;
    }

    while (g_running.load(std::memory_order_acquire)) {
        // Create pipe instance.
        g_pipe = EpCreatePipe(g_pipe_name);
        if (g_pipe == kEpInvalidHandle) {
            break;
        }

        // Reset connect event.
        ResetEvent(g_connect_overlapped.hEvent);

        // Wait for client.
        bool wait_ok = EpWaitForClient(g_pipe, &g_connect_overlapped);
        if (!wait_ok) {
            EpClosePipe(g_pipe);
            g_pipe = kEpInvalidHandle;
            break;
        }

        // Check if we need to wait for async completion.
        DWORD wait_result = WaitForSingleObject(g_connect_overlapped.hEvent, INFINITE);
        if (wait_result == WAIT_OBJECT_0) {
            g_connected.store(true, std::memory_order_release);
        } else {
            EpClosePipe(g_pipe);
            g_pipe = kEpInvalidHandle;
            continue;
        }

        // Wait for client to disconnect (ReadFile returning 0 or error
        // signals client gone).  We poll g_running so shutdown is prompt.
        while (g_running.load(std::memory_order_acquire)) {
            // We can't read from an outbound-only pipe, so we poll
            // g_running + check pipe state via PeekNamedPipe.
            DWORD bytes_avail = 0;
            BOOL peek_ok = PeekNamedPipe(g_pipe, nullptr, 0, nullptr, &bytes_avail, nullptr);
            if (!peek_ok) {
                // Client disconnected or pipe error.
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        g_connected.store(false, std::memory_order_release);

        // Disconnect and prepare for next client.
        EpDisconnectPipe(g_pipe);
        EpClosePipe(g_pipe);
        g_pipe = kEpInvalidHandle;
    }

    CloseHandle(g_connect_overlapped.hEvent);
    g_connect_overlapped.hEvent = nullptr;
}

}  // anonymous namespace

// ── Public API ──────────────────────────────────────────────────────────

bool EpTransportInitialize(uint32_t pid) noexcept {
    if (g_running.load(std::memory_order_relaxed)) {
        return true;  // already initialized
    }

    // Build pipe name.
    swprintf_s(g_pipe_name, CHAOS_IL2CPP_EP_PIPE_NAME_TEMPLATE, pid);

    // Create write event (manual-reset for overlapped I/O).
    g_write_overlapped.hEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (g_write_overlapped.hEvent == nullptr) {
        return false;
    }

    g_running.store(true, std::memory_order_release);

    // Start listener thread.
    g_listener_thread = std::thread(ListenerThread);

    return true;
}

void EpTransportShutdown() noexcept {
    g_running.store(false, std::memory_order_release);

    // Signal connect event so listener wakes up.
    if (g_connect_overlapped.hEvent != nullptr) {
        SetEvent(g_connect_overlapped.hEvent);
    }

    if (g_listener_thread.joinable()) {
        g_listener_thread.join();
    }

    {
        std::lock_guard<std::mutex> lock(g_write_mutex);
        EpDisconnectPipe(g_pipe);
        EpClosePipe(g_pipe);
        g_pipe = kEpInvalidHandle;
    }

    g_connected.store(false, std::memory_order_release);

    if (g_write_overlapped.hEvent != nullptr) {
        CloseHandle(g_write_overlapped.hEvent);
        g_write_overlapped.hEvent = nullptr;
    }
}

void EpTransportWrite(const void* data, uint32_t data_size) noexcept {
    if (!g_connected.load(std::memory_order_acquire)) {
        return;  // no client connected — drop
    }

    std::lock_guard<std::mutex> lock(g_write_mutex);

    if (g_pipe == kEpInvalidHandle) {
        return;
    }

    ResetEvent(g_write_overlapped.hEvent);

    bool write_ok = EpWritePipe(g_pipe, data, data_size, &g_write_overlapped);
    if (!write_ok) {
        // Write failed entirely (not pending).
        g_connected.store(false, std::memory_order_release);
        return;
    }

    // Wait for async write with timeout.
    DWORD wait_result = WaitForSingleObject(g_write_overlapped.hEvent, kEpWriteTimeoutMs);
    if (wait_result == WAIT_TIMEOUT) {
        // Write timed out — cancel and disconnect.
        CancelIoEx(g_pipe, &g_write_overlapped);
        g_connected.store(false, std::memory_order_release);
    } else if (wait_result == WAIT_OBJECT_0) {
        DWORD bytes_written = 0;
        if (!GetOverlappedResult(g_pipe, &g_write_overlapped, &bytes_written, FALSE)) {
            // Async write failed (e.g. client disconnected mid-write).
            g_connected.store(false, std::memory_order_release);
        }
    } else {
        // WAIT_FAILED or WAIT_ABANDONED — unexpected.
        g_connected.store(false, std::memory_order_release);
    }
}

bool EpTransportIsConnected() noexcept {
    return g_connected.load(std::memory_order_acquire);
}

#else  // Linux stub

bool EpTransportInitialize(uint32_t /*pid*/) noexcept { return false; }
void EpTransportShutdown() noexcept {}
void EpTransportWrite(const void* /*data*/, uint32_t /*data_size*/) noexcept {}
bool EpTransportIsConnected() noexcept { return false; }

#endif  // _WIN32 || _WIN64

}  // namespace chaos::il2cpp::diagnostics

#endif  // CHAOS_IL2CPP_EVENTPIPE
