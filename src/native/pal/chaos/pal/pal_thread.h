// pal_thread.h — Threading primitives
#pragma once

#include <cstddef>
#include <cstdint>

namespace chaos::il2cpp::pal {

/// Opaque handle for a native thread.
struct PalThread;

/// Thread entry point signature.
typedef void* (*PalThreadProc)(void* arg);

/// Create a new thread. Returns nullptr on failure.
PalThread* PalThreadCreate(PalThreadProc proc, void* arg) noexcept;

/// Wait for a thread to exit. Returns the thread's exit value.
void* PalThreadJoin(PalThread* thread) noexcept;

/// Get the current thread's native OS identifier.
/// On POSIX: returns pthread_t cast to uint64_t.
/// On Windows: returns GetCurrentThreadId().
uint64_t PalGetCurrentThreadId() noexcept;

/// Sleep for the specified number of milliseconds.
void PalSleepMs(uint64_t ms) noexcept;

/// Yield the current thread's remaining CPU slice.
void PalYield() noexcept;

// ── Stack bounds ────────────────────────────────────────────────────

/// Get the stack bounds of the calling thread.
/// On Windows: reads NT_TIB via __readgsqword(0x30).
/// On POSIX: uses pthread_getattr_np / pthread_attr_getstack.
/// @param out_base   Output: high address of the stack (highest valid address).
/// @param out_limit  Output: low address of the stack (lowest valid address).
void PalGetStackBounds(void*& out_base, void*& out_limit) noexcept;

/// Duplicate the current thread's OS handle for cross-thread operations.
/// On Windows: returns a duplicated HANDLE via DuplicateHandle with
///             THREAD_SET_CONTEXT access (for QueueUserAPC).
/// On POSIX: returns nullptr (signal-based preemption uses os_thread_id).
/// The returned handle must be closed via PalCloseThreadHandle.
void* PalDuplicateCurrentThreadHandle() noexcept;

/// Close a thread handle returned by PalDuplicateCurrentThreadHandle.
/// On Windows: calls CloseHandle.  On POSIX: no-op.
void PalCloseThreadHandle(void* handle) noexcept;

/// Set the OS thread priority from a managed priority level (0=Lowest..4=Highest).
/// On Windows: maps to THREAD_PRIORITY_LOWEST..THREAD_PRIORITY_HIGHEST.
/// On POSIX: uses setpriority with nice value 19..-20.
/// Returns true on success, false on failure.
bool PalSetThreadPriority(int level) noexcept;

}  // namespace chaos::il2cpp::pal
