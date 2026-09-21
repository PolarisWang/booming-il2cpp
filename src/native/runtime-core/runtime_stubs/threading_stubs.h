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

// Thread.GetCurrentProcessorId: processor the calling thread is currently on.
//
// A per-call HOST property, not the machine's processor count and not stable
// across calls (the OS may migrate the thread).  .NET 8 documents it as
// diagnostic-only with no consistency guarantee.
//
// Expected values are environment-snapshots captured from the ATG probe's own
// machine, so the fact layer classifies these subjects envSensitive rather than
// comparing against a fixed value (see stages/fact_chunk.py,
// _get_env_sensitive_subject_ids).  This entry exists so the method is actually
// implemented instead of falling through the zero-argument catch-all.
CHAOS_IL2CPP_INT32 chaos_thread_get_current_processor_id(void) noexcept;

// ── LazyInitializer.EnsureInitialized<T> ────────────────────────────────────
//
// Managed contract (verified against .NET 8):
//
//   T EnsureInitialized(ref T target, ref bool initialized, ref object syncLock)
//       If *initialized is false: store default(T) into *target, set
//       *initialized = true, assign a non-null sentinel to *syncLock (the real
//       BCL uses a lock object it constructs), and return the stored value.
//       If *initialized is true: leave everything alone and return *target.
//
//   T EnsureInitialized(ref T target, ref bool initialized, ref object syncLock,
//                       Func<T> valueFactory)
//       Same, except the stored/returned value comes from invoking valueFactory.
//       The factory is NOT invoked when *initialized is already true.
//
// The four `ref` slots arrive as pointers: target carries the T value on its
// by-ref slot, `initialized` is a 1-byte bool at that address, and `syncLock`
// is a reference slot.  `carrier` says how wide T is so the value can be
// written and read back through the caller's storage.
//
// The factory overload takes the delegate as an opaque handle and invokes it
// via chaos_delegate_object_invoke, reading its return into the target.

CHAOS_IL2CPP_INTPTR chaos_lazy_initializer_ensure_initialized(
    CHAOS_IL2CPP_INTPTR target_ref,
    CHAOS_IL2CPP_INTPTR initialized_ref,
    CHAOS_IL2CPP_INTPTR sync_lock_ref,
    CHAOS_IL2CPP_INT32 carrier_width) noexcept;

CHAOS_IL2CPP_INTPTR chaos_lazy_initializer_ensure_initialized_factory(
    CHAOS_IL2CPP_INTPTR target_ref,
    CHAOS_IL2CPP_INTPTR initialized_ref,
    CHAOS_IL2CPP_INTPTR sync_lock_ref,
    CHAOS_IL2CPP_INTPTR value_factory,
    CHAOS_IL2CPP_INT32 carrier_width) noexcept;


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

// ── WaitHandle static overloads: argument validation ──────────────────────
//
// Managed contract (measured against .NET 8 AND net10 — identical on both):
//
//   WaitHandle.WaitAll (WaitHandle[])              → null arr: ArgumentNullException
//   WaitHandle.WaitAll (WaitHandle[], int|TimeSpan [, bool])
//   WaitHandle.WaitAny (WaitHandle[], ...)         → null arr: ArgumentNullException
//                                                  → empty arr: ArgumentException
//   WaitHandle.SignalAndWait (WaitHandle, WaitHandle [, ...])
//
// The validation runs BEFORE any wait, and it is IDENTICAL across every
// overload — the timeout operand does not participate.  That is what makes a
// single shared entry point sufficient: each registered shim keeps the callee's
// own arity (see the arity note in CoreStubs.Part1.S16.cs) but forwards only
// the array handle here.
//
// A null ELEMENT (not just a null array) is also an ArgumentNullException —
// measured with `WaitAll(new WaitHandle[]{ ev, null! }, 0)`.
//
// Ordering matters and is asserted by the contract test: null-check precedes
// empty-check, because `WaitAll(null!, …)` must raise ArgumentNullException and
// not ArgumentException.
CHAOS_IL2CPP_INT32 chaos_wait_handle_validate(CHAOS_IL2CPP_INTPTR wait_handles) noexcept;

// Same validation for the two-handle SignalAndWait form, where each operand is
// a single WaitHandle rather than an array.  Null either one → ArgumentNullException.
CHAOS_IL2CPP_INT32 chaos_wait_handle_validate_pair(
    CHAOS_IL2CPP_INTPTR to_signal,
    CHAOS_IL2CPP_INTPTR to_wait_on) noexcept;

}  // extern "C"