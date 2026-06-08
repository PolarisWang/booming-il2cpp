// pal_crash.h — Crash/signal handling (VEH / signal handlers)
#pragma once

#include <cstdint>

namespace chaos::il2cpp::pal {

/// Context information passed to the crash callback.
/// Platform-specific details are opaque; use PalFormatCrashContext for
/// human-readable output if available.
struct CrashContext {
    int signal;              ///< Signal number (POSIX) or exception code (Windows)
    void* fault_address;     ///< Instruction pointer at fault (may be nullptr)
};

/// Crash handler callback. Called from a signal handler or VEH callback.
/// Must be async-signal-safe on POSIX; must not call most runtime functions.
typedef void (*PalCrashCallback)(const CrashContext& ctx) noexcept;

/// Register a crash handler. Replaces any previously registered handler.
/// On POSIX: registers handlers for SIGSEGV, SIGABRT, SIGBUS, SIGILL, SIGFPE.
/// On Windows: registers a VectoredExceptionHandler.
/// Returns true on success.
bool PalRegisterCrashHandler(PalCrashCallback callback) noexcept;

/// Remove the crash handler and restore default handling.
void PalRemoveCrashHandler() noexcept;

/// Trigger a debug break (__debugbreak / raise(SIGTRAP) / trap instruction).
void PalDebugBreak() noexcept;

}  // namespace chaos::il2cpp::pal
