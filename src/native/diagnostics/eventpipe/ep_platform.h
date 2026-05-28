// ep_platform.h — Platform abstraction for EventPipe transport
//
// Abstracts the IPC transport mechanism behind a common interface.
// Currently only Windows Named Pipe is implemented (the only platform with
// an active diagnostic receiver).
//
// ## Porting to a new platform
// 1. Add #elif block for the new platform (e.g., Linux -> Unix socket)
// 2. Implement EpPlatformPipe (create, connect, write, disconnect, close)
// 3. Keep the same EpTransportWrite abstraction in ep_transport.h

#ifndef CHAOS_IL2CPP_EP_PLATFORM_H_
#define CHAOS_IL2CPP_EP_PLATFORM_H_

#include "ep_config.h"

#if CHAOS_IL2CPP_EVENTPIPE == 1

#if defined(_WIN32) || defined(_WIN64)
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #ifndef _WIN32_WINNT
        #define _WIN32_WINNT 0x0601  // Windows 7+
    #endif
    #include <windows.h>
    #include <synchapi.h>

    namespace chaos::il2cpp::diagnostics {

    /// Platform-specific pipe handle type.
    using EpPlatformHandle = HANDLE;

    /// Invalid handle sentinel.
    #define kEpInvalidHandle INVALID_HANDLE_VALUE

    /// Create a named pipe server instance.
    /// @param pipe_name  Full pipe path (e.g., L"\\\\.\\pipe\\chaos-il2cpp-diag-1234")
    /// @return  Handle to the pipe, or kEpInvalidHandle on failure.
    inline EpPlatformHandle EpCreatePipe(const wchar_t* pipe_name) noexcept {
        return CreateNamedPipeW(
            pipe_name,
            PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED,
            PIPE_TYPE_BYTE | PIPE_WAIT,
            1,                          // max instances: 1 (single client)
            0,                          // out buffer (default)
            0,                          // in buffer (default)
            0,                          // default timeout
            nullptr                     // default security attributes
        );
    }

    /// Wait for a client to connect to the pipe.
    /// @param pipe  Handle to the pipe (from EpCreatePipe).
    /// @param overlapped  OVERLAPPED structure for async wait.
    /// @return true if connected, false on failure or timeout.
    inline bool EpWaitForClient(EpPlatformHandle pipe, OVERLAPPED* overlapped) noexcept {
        BOOL result = ConnectNamedPipe(pipe, overlapped);
        if (result == FALSE) {
            DWORD err = GetLastError();
            if (err == ERROR_IO_PENDING) {
                // Connection is pending (async).
                return true;  // caller will check GetOverlappedResult later
            }
            if (err == ERROR_PIPE_CONNECTED) {
                // Client already connected before we called ConnectNamedPipe.
                SetEvent(overlapped->hEvent);
                return true;
            }
            return false;
        }
        return true;  // synchronous connect succeeded
    }

    /// Write data to the pipe.
    /// @param pipe         Pipe handle.
    /// @param data         Data buffer to write.
    /// @param data_size    Size of data in bytes.
    /// @param overlapped   OVERLAPPED for async write.
    /// @return true if write was initiated (check overlapped for completion).
    inline bool EpWritePipe(EpPlatformHandle pipe, const void* data, DWORD data_size, OVERLAPPED* overlapped) noexcept {
        BOOL result = WriteFile(pipe, data, data_size, nullptr, overlapped);
        if (result == FALSE) {
            DWORD err = GetLastError();
            if (err == ERROR_IO_PENDING) {
                return true;  // async write in progress
            }
            return false;  // write failed
        }
        return true;  // synchronous write completed
    }

    /// Disconnect the pipe (waits for client to drain).
    /// @param pipe  Pipe handle.
    inline void EpDisconnectPipe(EpPlatformHandle pipe) noexcept {
        FlushFileBuffers(pipe);
        DisconnectNamedPipe(pipe);
    }

    /// Close the pipe handle.
    /// @param pipe  Pipe handle (may be kEpInvalidHandle, which is a no-op).
    inline void EpClosePipe(EpPlatformHandle pipe) noexcept {
        if (pipe != kEpInvalidHandle) {
            CloseHandle(pipe);
        }
    }

    }  // namespace chaos::il2cpp::diagnostics

#elif defined(__linux__)

    #include <cstddef>
    #include <cstdint>

    namespace chaos::il2cpp::diagnostics {

    /// Platform-specific pipe handle type (Linux stub: uses int fd).
    using EpPlatformHandle = int;

    /// Invalid handle sentinel.
    #define kEpInvalidHandle (-1)

    /// Create a named pipe server instance (Linux stub — no-op, always fails).
    inline EpPlatformHandle EpCreatePipe(const wchar_t* /*pipe_name*/) noexcept {
        return kEpInvalidHandle;
    }

    /// Wait for a client to connect to the pipe (Linux stub — no-op).
    inline bool EpWaitForClient(EpPlatformHandle /*pipe*/, void* /*overlapped*/) noexcept {
        return false;
    }

    /// Write data to the pipe (Linux stub — no-op).
    inline bool EpWritePipe(EpPlatformHandle /*pipe*/, const void* /*data*/, uint32_t /*data_size*/, void* /*overlapped*/) noexcept {
        return false;
    }

    /// Disconnect the pipe (Linux stub — no-op).
    inline void EpDisconnectPipe(EpPlatformHandle /*pipe*/) noexcept {}

    /// Close the pipe handle (Linux stub — no-op).
    inline void EpClosePipe(EpPlatformHandle /*pipe*/) noexcept {}

    }  // namespace chaos::il2cpp::diagnostics

#else
    #error "EventPipe: unsupported platform (port to Linux Unix socket or disable CHAOS_IL2CPP_EVENTPIPE)"
#endif  // _WIN32 || _WIN64

#endif  // CHAOS_IL2CPP_EVENTPIPE

#endif  // CHAOS_IL2CPP_EP_PLATFORM_H_
