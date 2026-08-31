// pal_suspend.h — Hard suspension (SuspendThread) abstraction for A3 Hybrid safepoint
#pragma once

#include <cstdint>

namespace chaos::il2cpp::pal {

/// Results of PalSuspendThread / PalGetThreadContext.
enum class SuspendResult {
    kSuccess,     // operation completed
    kNotFound,    // thread handle invalid / thread unstarted-or-dead
    kSuspended,   // thread was already suspended
    kForbidden,   // target thread forbids suspension (m_dwForbidSuspendThread != 0)
    kUnsupported, // platform does not support hard suspension
    kFailure,     // other OS-level failure
};

/// Query whether this platform supports hard suspension
/// (SuspendThread + GetThreadContext + ResumeThread).
///
/// - Windows: true — Win32 SuspendThread/ResumeThread/GetThreadContext.
/// - Linux/POSIX: false — no equivalent hard-suspend; A3 falls back to the
///   SIGUSR2 trampoline drive (PalPreemptRequest), matching CoreCLR's
///   `#ifndef DISABLE_THREADSUSPEND` platform gate.
/// - Apple/Android: false — no reliable suspension; pure soft rendezvous.
bool PalHardSuspendSupported() noexcept;

/// Suspend a target thread by its OS handle (Windows) before inspecting /
/// redirecting its execution.
///
/// IMPORTANT (CoreCLR semantics, threadsuspend.cpp:~230): hard suspension is
/// NOT a "suspend-and-hold-to-scan" primitive.  The caller must:
///   1. SuspendThread
///   2. GetThreadContext to read the IP / registers
///   3. Redirect (rewrite EIP/return-address) if needed
///   4. ResumeThread IMMEDIATELY
///   5. The thread then walks itself to a rendezvous (safepoint event wait)
/// Never scan GC roots from a thread while it stays physically suspended —
/// OS GetThreadContext does not guarantee a consistent register window.
///
/// @param os_handle   Windows thread handle (from PalDuplicateCurrentThreadHandle).
/// @returns kSuccess if suspended, kForbidden if the target forbids suspension.
SuspendResult PalSuspendThread(void* os_handle) noexcept;

/// Resume a thread previously suspended by PalSuspendThread.
SuspendResult PalResumeThread(void* os_handle) noexcept;

/// Read the thread's execution context (IP, stack, registers).
///
/// @param os_handle  Windows thread handle.
/// @param ip_out     [optional] receives the instruction pointer (whether the
///                   thread is at a safe point / polling location).
/// @param sp_out     [optional] receives the stack pointer.
/// @param gpr_out    [optional] 16-entry array (x64: RAX=0..R15=15).  May be null.
/// @returns kSuccess on a reliable context read; kUnsupported/kFailure otherwise.
SuspendResult PalGetThreadContext(void* os_handle, uint64_t* ip_out,
                                  uint64_t* sp_out, uint64_t* gpr_out) noexcept;

}  // namespace chaos::il2cpp::pal
