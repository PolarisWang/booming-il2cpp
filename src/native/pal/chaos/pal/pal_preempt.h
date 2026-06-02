// pal_preempt.h — Preemptive thread suspend abstraction
#pragma once

#include <cstdint>

namespace chaos::il2cpp::pal {

/// Callback invoked when a preemptive suspend request is delivered to
/// the target thread.
///
/// On Windows: called from APC context with the epoch passed via
/// QueueUserAPC's ULONG_PTR parameter.
///
/// On POSIX (Linux): called from SIGUSR2 signal handler context with
/// epoch=0.  The callback reads the actual epoch from TLS (suspend_seq).
///
/// The callback runs in the target thread's context and must be
/// async-signal-safe on POSIX.
typedef void (*PalPreemptCallback)(uint64_t epoch) noexcept;

/// Initialize the preemptive suspend subsystem.
///
/// On Windows: stores the callback for APC forwarding.
/// On POSIX (Linux): stores the callback and installs the SIGUSR2
/// signal handler (one-time, internally synchronized).
/// On Apple/Android: stores the callback (no-op delivery).
///
/// Must be called at least once before any PalPreemptRequest call.
/// Calling multiple times is safe — only the first call installs
/// the signal handler.
void PalPreemptInit(PalPreemptCallback callback) noexcept;

/// Request preemptive suspend of a target thread.
///
/// On Windows: queues an APC via QueueUserAPC using os_handle.
/// On POSIX (Linux): sends SIGUSR2 via pthread_kill using os_thread_id.
/// On Apple/Android: no-op, returns false.
///
/// @param os_handle     Windows thread handle (ignored on POSIX).
/// @param os_thread_id  POSIX thread ID (ignored on Windows).
/// @param epoch         Safepoint epoch (passed to callback on Windows;
///                      callback reads from TLS on POSIX).
/// @returns true if the request was delivered successfully.
bool PalPreemptRequest(void* os_handle, uint64_t os_thread_id, uint64_t epoch) noexcept;

}  // namespace chaos::il2cpp::pal
