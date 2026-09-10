// pal_eh.h — Structured exception handling abstraction
#pragma once

#include <cstdint>

namespace chaos::il2cpp::pal {

/// Call a native function pointer with SEH protection.
/// On Windows: wraps the call in __try/__except to catch hardware
/// exceptions (access violation, stack overflow, etc.).
/// On POSIX: uses sigsetjmp/siglongjmp + SIGSEGV/SIGBUS handler to
/// catch hardware exceptions (segmentation fault, bus error).
///
/// @returns true if a hardware exception was caught.
/// @param out_result  Receives the function's return value on success.
///                    Undefined if the function threw an SEH exception.
bool PalTryCallNoExcept(uint64_t (*fn)(uint64_t, uint64_t, uint64_t, uint64_t,
                                        uint64_t, uint64_t, uint64_t, uint64_t),
                         uint64_t a0, uint64_t a1, uint64_t a2, uint64_t a3,
                         uint64_t a4, uint64_t a5, uint64_t a6, uint64_t a7,
                         uint64_t& out_result) noexcept;

/// Reset PAL EH thread-local state to safe defaults.
/// Safe to call from any context (including inside a signal handler).
/// Use after a longjmp that bypassed PalTryCallNoExcept's normal cleanup,
/// to prevent stale sigsetjmp contexts from causing cascading crashes.
///
/// Defined as inline (accesses TLS state declared extern below) so that
/// callers in test/foundation-dll entry.exe don't need to link against
/// a runtime library with the definition.  The TLS storage is in the
/// platform .cpp file.
extern thread_local bool g_pal_try_active;
extern thread_local int g_pal_try_reentry;

inline void PalTryResetState() noexcept {
    g_pal_try_active = false;
    g_pal_try_reentry = 0;
}

}  // namespace chaos::il2cpp::pal
