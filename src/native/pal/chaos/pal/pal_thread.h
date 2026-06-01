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

}  // namespace chaos::il2cpp::pal
