#ifndef CHAOS_IL2CPP_PAL_ERROR_H_
#define CHAOS_IL2CPP_PAL_ERROR_H_

// ── OS error/process/thread info abstraction ─────────────────────────
// Abstracts Win32 GetLastError/SetLastError, GetCurrentProcessId/ThreadId,
// and handle retrieval so that consumers do not need #if defined(_WIN32)
// for basic OS introspection.
//
// On Win32: delegates to the real Windows API.
// On POSIX: returns stubs (0/nullptr) where the concept doesn't exist.

#include <cstdint>

namespace chaos::il2cpp::pal {

/// Return the last OS error code for the calling thread.
int32_t PalGetLastError() noexcept;

/// Set the last OS error code for the calling thread.
void PalSetLastError(int32_t error) noexcept;

/// Return the current process ID.
int32_t PalGetCurrentProcessId() noexcept;

/// Return the current thread ID (uint64_t to match pal_thread.h).
uint64_t PalGetCurrentThreadId() noexcept;

/// Return the current process handle (opaque).
intptr_t PalGetCurrentProcess() noexcept;

/// Return the current thread handle (opaque).
intptr_t PalGetCurrentThread() noexcept;

}  // namespace chaos::il2cpp::pal

#endif  // CHAOS_IL2CPP_PAL_ERROR_H_
