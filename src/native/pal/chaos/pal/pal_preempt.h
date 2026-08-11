// pal_preempt.h — Preemptive thread suspend abstraction
#pragma once

#include <atomic>
#include <cstdint>

#include <chaos/pal/pal_sync.h>

namespace chaos::il2cpp::pal {

/// Callback invoked when a preemptive suspend request is delivered to
/// the target thread.
///
/// On Windows: called from APC context with the epoch passed via
/// QueueUserAPC's ULONG_PTR parameter.
///
/// On POSIX (Linux): called from SIGUSR2 signal handler context with
/// epoch=0.  The callback reads the actual epoch from TLS (suspend_seq).
/// The ucontext from the signal handler is available via
/// PalPreemptGetUcontext().
///
/// The callback runs in the target thread's context and must be
/// async-signal-safe on POSIX.
typedef void (*PalPreemptCallback)(uint64_t epoch) noexcept;

/// Initialize the preemptive suspend subsystem.
///
/// On Windows: stores the callback for APC forwarding.
/// On POSIX (Linux): stores the callback and installs the SIGUSR2
/// signal handler (one-time, internally synchronized) using SA_SIGINFO
/// to capture the interrupted thread's register state via ucontext_t.
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

/// Called by the preemptive suspend handler (target thread context) after
/// acknowledging the suspend request.
///
/// On Windows: stores epoch to *suspend_ack, then blocks on suspend_event
/// (PalEventWait).  APC context permits event wait.
///
/// On POSIX: stores suspend_seq's current value to *suspend_ack, then
/// spin-waits with PalYield until *suspend_seq becomes 0.  Signal context
/// forbids synchronization primitives, so a spin-loop is the only option.
void PalPreemptiveSuspendAck(uint64_t epoch, PalEvent* suspend_event,
                              std::atomic<uint32_t>* suspend_seq,
                              std::atomic<uint32_t>* suspend_ack) noexcept;

/// Retrieve the ucontext_t pointer captured by the signal handler during
/// the current preemptive suspend callback invocation.
///
/// On POSIX (Linux): returns the ucontext_t* from SA_SIGINFO's third
/// argument, providing access to the interrupted thread's register state
/// (RIP, RSP, RBP on x64).  Only valid during a PalPreemptCallback call.
///
/// On non-POSIX platforms: always returns nullptr.
///
/// The returned pointer is thread-local and valid only within the scope
/// of the current callback invocation.
const void* PalPreemptGetUcontext() noexcept;

/// ── Phase 2 (C): cross-platform register-window capture ────────────────
/// Pure PAL ownership of per-thread captured register state, so the GC can read
/// a suspended thread's physical GPRs without reaching into runtime-core state.
///
/// Every thread is allocated a lazy, permanent capture slot (index into a PAL
/// slot pool) via PalGetCaptureSlot().  The suspend callback stores the captured
/// ucontext under the thread's OWN slot (PalSetPreemptContext); the GC reads it
/// cross-thread via the SAME slot index (which the runtime persists on
/// ManagedThread::gc_slot), then PalCaptureThreadContext fills the 16 GPRs.
///
/// Reliability gate: a platform/thread without a reliable capture returns
/// false / -1, and the GC must then keep gc_num_gprs==0 (skip register-root
/// scanning, keep the stack-slot floor).  The window is ALWAYS additive to stack
/// slots — never the sole retention mechanism (never under-retains).

/// Lazy per-thread capture-slot index; -1 when this platform/thread doesn't
/// support reliable register capture (e.g. Windows APC-park architecture).
int  PalGetCaptureSlot() noexcept;

/// Store (non-null) or clear (null) the captured context for a thread's slot.
/// Called by the target thread's suspend/clear path.  Uses release ordering so
/// the cross-thread GC read (acquire) sees the stored pointer.
void PalSetPreemptContext(int slot, const void* ucontext) noexcept;

/// Fill @a gpr_values[16] (RAX=0..R15=15, x64) from @a slot's captured context.
/// Returns true only when a reliable capture is present; sets *out_num_gprs=0
/// and returns false when the platform/thread has no reliable window.
bool PalCaptureThreadContext(int slot, uint64_t gpr_values[16], uint32_t* out_num_gprs) noexcept;

}  // namespace chaos::il2cpp::pal
