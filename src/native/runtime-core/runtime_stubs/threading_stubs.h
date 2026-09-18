// ── Threading stub declarations ────────────────────────────────
#pragma once

#include <chaos/thread.h>
#include "thread_state.h"

/// Get the current managed thread object (fast path: TLS read, may be called from
/// hot benchmark loops where inlining matters).  Implemented here as inline so the
/// compiler can see through the TLS read at call sites in the same translation unit.
inline CHAOS_IL2CPP_INTPTR chaos_thread_get_current(void) noexcept
{
    // Fast path: codegen mode — current_thread_object is always set during
    // runtime_init (via s_main_thread_sentinel). Single TLS read, no fallback.
    auto result = chaos::il2cpp::common::current_thread_object;
    if (result != 0) return result;

    // Slow path: interpreter mode or uninitialized thread — check tls_this_thread.
    auto* thread = chaos::il2cpp::runtime_core::threading::tls_this_thread;
    if (thread == nullptr) return 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(thread->managed_object);
}
// ── Linkage: these MUST all be extern "C" ──────────────────────────────
//
// threading_stubs.cpp opens a single file-scope `extern "C" {` and defines
// every one of these symbols with C linkage (they export undecorated from
// chaos_runtime_core.lib: `chaos_thread_yield`, not `?chaos_thread_yield@@YAHXZ`).
// This header must therefore declare ALL of them inside one extern "C" block.
//
// When only a subset was inside the block (historically just
// chaos_monitor_enter/exit), the rest were declared with C++ linkage while
// defined with C linkage.  The mismatch is invisible until link time, and it
// only fires for symbols whose definitions the linker has to pull by name:
// LNK2019 "unresolved external symbol ?chaos_thread_yield@@YAHXZ" for
// Thread.Yield / Thread.Sleep, while chaos_monitor_enter linked fine from the
// same object file.  Keep this block closed only at the end of the exported
// declarations.
extern "C" {

void chaos_monitor_enter(CHAOS_IL2CPP_INTPTR obj, CHAOS_IL2CPP_INTPTR lockTaken) noexcept;
void chaos_monitor_exit(CHAOS_IL2CPP_INTPTR obj) noexcept;
CHAOS_IL2CPP_INT32 ChaosMonitorTryEnter(CHAOS_IL2CPP_INTPTR obj, CHAOS_IL2CPP_INT32 timeout) noexcept;

// Thread lifecycle: .ctor stores the delegate, Start spawns a native thread,
// Join waits for completion.  All take the managed Thread object as first arg.
void chaos_thread_ctor(CHAOS_IL2CPP_INTPTR thread_obj, CHAOS_IL2CPP_INTPTR thread_start_delegate) noexcept;
void chaos_thread_start(CHAOS_IL2CPP_INTPTR thread_obj) noexcept;
void chaos_thread_join(CHAOS_IL2CPP_INTPTR thread_obj) noexcept;

// Thread.Interrupt: signal a thread to throw ThreadInterruptedException
// at the next safepoint poll.
void chaos_thread_interrupt(CHAOS_IL2CPP_INTPTR thread_obj) noexcept;

// Thread.Abort: signal a thread to throw ThreadAbortException
// at the next safepoint poll.  state_obj comes from Thread.Abort(object state)
// and would be passed to the ThreadAbortException constructor in a full
// implementation; for now it is accepted and ignored (the abort signal itself
// is what matters for testing).
void chaos_thread_abort(CHAOS_IL2CPP_INTPTR thread_obj, CHAOS_IL2CPP_INTPTR state_obj) noexcept;

// Thread.ResetAbort: cancel a pending Thread.Abort for the calling thread.
// Returns nonzero if an abort was pending and was cancelled, 0 if no abort
// was pending (no-op).
CHAOS_IL2CPP_INT32 chaos_thread_reset_abort(void) noexcept;

// Thread.Yield: yield the current thread's time slice.
// Returns nonzero (true) on success to match .NET bool return.
//
// NOTE: the managed `Thread.Yield()` return value is NOT deterministic — it
// alternates true/false across runs depending on whether another thread was
// ready to run (measured against .NET 8 on one machine: 3 false, 2 true over
// five runs).  Callers must not assert a fixed value on it.
CHAOS_IL2CPP_INT32 chaos_thread_yield(void) noexcept;

// Thread.GetDomainID: id of the current AppDomain.
//
// .NET Core has exactly one AppDomain, so this is 1 by construction (verified
// against .NET 8).  The .NET Framework "increasing per-domain id" semantics do
// not apply because this runtime does not implement multiple domains.
CHAOS_IL2CPP_INT32 chaos_thread_get_domain_id(void) noexcept;

// Thread.Sleep: block the current thread for the specified timeout in milliseconds.
void chaos_thread_sleep(CHAOS_IL2CPP_INT32 timeout_ms) noexcept;

// Thread.IsBackground: get/set background status.
// Background threads do not prevent process exit.
CHAOS_IL2CPP_INT32 chaos_thread_is_background(CHAOS_IL2CPP_INTPTR thread_obj) noexcept;
void chaos_thread_set_background(CHAOS_IL2CPP_INTPTR thread_obj, CHAOS_IL2CPP_INT32 value) noexcept;

// Thread.ThreadState: get the current managed thread state.
CHAOS_IL2CPP_INT32 chaos_thread_get_state(CHAOS_IL2CPP_INTPTR thread_obj) noexcept;

// Thread.Priority: get/set thread priority.
CHAOS_IL2CPP_INT32 chaos_thread_get_priority(CHAOS_IL2CPP_INTPTR thread_obj) noexcept;
void chaos_thread_set_priority(CHAOS_IL2CPP_INTPTR thread_obj, CHAOS_IL2CPP_INT32 value) noexcept;

// Thread.IsThreadPoolThread: query whether thread is a ThreadPool worker.
CHAOS_IL2CPP_INT32 chaos_thread_is_threadpool(CHAOS_IL2CPP_INTPTR thread_obj) noexcept;

}  // extern "C"