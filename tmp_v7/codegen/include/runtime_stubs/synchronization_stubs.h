// ── Synchronization primitive ABI exports (T2.0) ───────────────────────
//
// THE PROBLEM THIS SOLVES
// -----------------------
// The native bodies for these primitives already existed — SemaphoreSlim,
// ReaderWriterLockSlim (incl. upgradeable), Barrier and CountdownEvent live in
// synchronization.cpp; ManualResetEvent/AutoResetEvent in wait_handle.cpp —
// but they are C++ NAMESPACE functions
// (chaos::il2cpp::runtime_core::threading::SemaphoreSlimCreate etc.).
//
// Generated C++ cannot call those.  The ShapeRegistry's SimpleForward kind
// emits a DIRECT call to the symbol named in the registry entry:
//
//     return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(<NativeFnSymbol>(args...));
//
// That emission targets an unqualified identifier and is compiled into a
// separate translation unit, so the name it can reach is a C-linkage symbol
// declared in a header the generated TU includes.  A namespaced C++ function is
// both unreachable by that name and unnecessary — the managed side has no
// concept of a namespace.
//
// So this header is the ABI boundary, not a reimplementation: every function
// below is a thin extern "C" shim forwarding to the namespace implementation.
// Adding a managed-visible primitive is therefore a two-part change — a shim
// here, and a registry entry in the Generator — and the part that is easy to
// forget is the DECLARATION, because omitting it fails late and confusingly.
//
// ── Linkage: every declaration here MUST be inside one extern "C" block ──
//
// This is the documented failure mode from threading_stubs.h (see its comment)
// and it is worth restating because it is silent until link time: if a
// declaration sits OUTSIDE the block while its definition is inside a
// file-scope `extern "C"`, the compiler gives the declaration C++ linkage and
// the symbol mangles (`?SemaphoreSlimCreate@@...`) against an undecorated
// definition.  The mismatch is legal C++ and produces no diagnostic; it
// surfaces only as LNK2019 for callers whose definition the linker must pull by
// name.  Keep this block closed only at the very end.
//
// ── Naming ──
//
// `Chaos` + PascalCase words, matching the Interlocked precedent that the
// ShapeRegistry already references (ChaosInterlockedMemoryBarrier,
// ChaosInterlockedReadInt64 — see RuntimeHelperShapeRegistry.CoreStubs.Part2.S22.cs).
// The older `chaos_*` snake_case family (chaos_monitor_enter, chaos_thread_*)
// predates this and is left alone rather than churned; both casings are valid C
// symbols, and the registry entry name and the implementation name must simply
// agree.  All NEW threading exports use the Chaos* form.

#pragma once

#include <chaos/native_types.h>

extern "C" {

// ══════════════════════════════════════════════════════════════════════
// SemaphoreSlim
// ══════════════════════════════════════════════════════════════════════
//
// Handles are 32-bit slot ids, NOT pointers.  The managed object holds the id;
// the native side owns the slot table.  0 is the invalid id (creation failure),
// which is why the create functions return INTPTR rather than a boolean.

/// Create a semaphore.  Returns a nonzero handle, or 0 on failure.
CHAOS_IL2CPP_INTPTR ChaosSemaphoreSlimCreate(CHAOS_IL2CPP_INT32 initial_count,
                                             CHAOS_IL2CPP_INT32 max_count) noexcept;

/// Destroy a semaphore.  Returns nonzero if the handle was found.
CHAOS_IL2CPP_INT32 ChaosSemaphoreSlimDestroy(CHAOS_IL2CPP_INTPTR sem_id) noexcept;

/// Wait/decrement.  timeout_ms: -1 = infinite, 0 = poll, >0 = bounded.
/// Returns 1 = acquired, 0 = timeout, -1 = error.
CHAOS_IL2CPP_INT32 ChaosSemaphoreSlimWait(CHAOS_IL2CPP_INTPTR sem_id,
                                          CHAOS_IL2CPP_INT32 timeout_ms) noexcept;

/// Release/increment by `count`.  Returns 0 = success, -1 = error
/// (unknown handle, or the release would exceed max_count).
CHAOS_IL2CPP_INT32 ChaosSemaphoreSlimRelease(CHAOS_IL2CPP_INTPTR sem_id,
                                             CHAOS_IL2CPP_INT32 count) noexcept;

// ══════════════════════════════════════════════════════════════════════
// ReaderWriterLockSlim
// ══════════════════════════════════════════════════════════════════════
//
// The upgradeable-read mode is the reason this lock cannot be modelled as a
// plain mutex: at most one thread may hold it, it coexists with ordinary
// readers, and it can be promoted to a writer without releasing first (which is
// what makes the upgrade deadlock-free for the single-upgrader case).

CHAOS_IL2CPP_INTPTR ChaosReaderWriterLockSlimCreate() noexcept;
CHAOS_IL2CPP_INT32 ChaosReaderWriterLockSlimDestroy(CHAOS_IL2CPP_INTPTR rw_id) noexcept;

/// Enter/exit shared (reader) mode.  Enter returns 1 = acquired, 0 = timeout.
CHAOS_IL2CPP_INT32 ChaosReaderWriterLockSlimEnterRead(CHAOS_IL2CPP_INTPTR rw_id,
                                                      CHAOS_IL2CPP_INT32 timeout_ms) noexcept;
CHAOS_IL2CPP_INT32 ChaosReaderWriterLockSlimExitRead(CHAOS_IL2CPP_INTPTR rw_id) noexcept;

/// Enter/exit exclusive (writer) mode.
CHAOS_IL2CPP_INT32 ChaosReaderWriterLockSlimEnterWrite(CHAOS_IL2CPP_INTPTR rw_id,
                                                       CHAOS_IL2CPP_INT32 timeout_ms) noexcept;
CHAOS_IL2CPP_INT32 ChaosReaderWriterLockSlimExitWrite(CHAOS_IL2CPP_INTPTR rw_id) noexcept;

/// Enter/exit upgradeable-read mode (at most one holder).
CHAOS_IL2CPP_INT32 ChaosReaderWriterLockSlimEnterUpgradeableRead(
    CHAOS_IL2CPP_INTPTR rw_id, CHAOS_IL2CPP_INT32 timeout_ms) noexcept;
CHAOS_IL2CPP_INT32 ChaosReaderWriterLockSlimExitUpgradeableRead(
    CHAOS_IL2CPP_INTPTR rw_id) noexcept;

/// Promote an upgradeable-read holder to writer (blocks until readers drain)
/// and demote back.  Returns 1 = acquired, 0 = timeout, -1 = error.
CHAOS_IL2CPP_INT32 ChaosReaderWriterLockSlimUpgradeToWrite(
    CHAOS_IL2CPP_INTPTR rw_id, CHAOS_IL2CPP_INT32 timeout_ms) noexcept;
CHAOS_IL2CPP_INT32 ChaosReaderWriterLockSlimDowngradeFromWrite(
    CHAOS_IL2CPP_INTPTR rw_id) noexcept;

// ══════════════════════════════════════════════════════════════════════
// Barrier
// ══════════════════════════════════════════════════════════════════════

/// Create a barrier for `participant_count` threads.  Returns nonzero handle.
CHAOS_IL2CPP_INTPTR ChaosBarrierCreate(CHAOS_IL2CPP_INT32 participant_count) noexcept;
CHAOS_IL2CPP_INT32 ChaosBarrierDestroy(CHAOS_IL2CPP_INTPTR barrier_id) noexcept;

/// Arrive and wait.  Returns 1 = phase completed, 0 = timeout, -1 = error.
CHAOS_IL2CPP_INT32 ChaosBarrierSignalAndWait(CHAOS_IL2CPP_INTPTR barrier_id,
                                             CHAOS_IL2CPP_INT32 timeout_ms) noexcept;

/// Participants still unaccounted for in the current phase (-1 on error).
CHAOS_IL2CPP_INT32 ChaosBarrierGetRemainingParticipants(CHAOS_IL2CPP_INTPTR barrier_id) noexcept;

/// Current phase number, incremented after each complete phase (-1 on error).
/// INT64 because the managed property is `long`.
CHAOS_IL2CPP_INT64 ChaosBarrierGetCurrentPhaseNumber(CHAOS_IL2CPP_INTPTR barrier_id) noexcept;

// ══════════════════════════════════════════════════════════════════════
// CountdownEvent
// ══════════════════════════════════════════════════════════════════════

CHAOS_IL2CPP_INTPTR ChaosCountdownEventCreate(CHAOS_IL2CPP_INT32 initial_count) noexcept;
CHAOS_IL2CPP_INT32 ChaosCountdownEventDestroy(CHAOS_IL2CPP_INTPTR ce_id) noexcept;

/// Decrement by `count`.  Returns 1 = reached zero (signalled), 0 = not yet,
/// -1 = error (unknown handle or non-positive count).
CHAOS_IL2CPP_INT32 ChaosCountdownEventSignal(CHAOS_IL2CPP_INTPTR ce_id,
                                             CHAOS_IL2CPP_INT32 count) noexcept;

/// Wait for the count to reach zero.  Returns 1 = signalled, 0 = timeout.
CHAOS_IL2CPP_INT32 ChaosCountdownEventWait(CHAOS_IL2CPP_INTPTR ce_id,
                                           CHAOS_IL2CPP_INT32 timeout_ms) noexcept;

/// Remaining count (-1 on error).
CHAOS_IL2CPP_INT32 ChaosCountdownEventGetCurrentCount(CHAOS_IL2CPP_INTPTR ce_id) noexcept;

/// Re-arm with a new count.  Returns nonzero on success.
CHAOS_IL2CPP_INT32 ChaosCountdownEventReset(CHAOS_IL2CPP_INTPTR ce_id,
                                            CHAOS_IL2CPP_INT32 count) noexcept;

// ══════════════════════════════════════════════════════════════════════
// ManualResetEvent / AutoResetEvent (via the shared WaitHandle table)
// ══════════════════════════════════════════════════════════════════════
//
// One table with a type discriminant rather than two families, because the two
// differ ONLY in wake policy: Manual wakes every waiter on Set, Auto wakes one
// and self-resets.

/// type: 0 = ManualResetEvent, 1 = AutoResetEvent.
/// Any other value is rejected (returns 0) rather than being coerced to a
/// default — a wrong type would silently change the wake policy.
CHAOS_IL2CPP_INTPTR ChaosWaitHandleCreate(CHAOS_IL2CPP_INT32 initial_state,
                                          CHAOS_IL2CPP_INT32 type) noexcept;
CHAOS_IL2CPP_INT32 ChaosWaitHandleClose(CHAOS_IL2CPP_INTPTR handle_id) noexcept;

/// Set to signalled.  Returns nonzero if the handle was found.
CHAOS_IL2CPP_INT32 ChaosWaitHandleSet(CHAOS_IL2CPP_INTPTR handle_id) noexcept;
/// Reset to non-signalled.  Returns nonzero if the handle was found.
CHAOS_IL2CPP_INT32 ChaosWaitHandleReset(CHAOS_IL2CPP_INTPTR handle_id) noexcept;

/// Wait for one handle.  Returns 1 = signalled, 0 = timeout, -1 = error.
CHAOS_IL2CPP_INT32 ChaosWaitHandleWaitOne(CHAOS_IL2CPP_INTPTR handle_id,
                                          CHAOS_IL2CPP_INT32 timeout_ms) noexcept;

/// Wait for ALL of the handles in `handle_ids` (a 32-bit id array).
///
/// RETURNS 0 ON SUCCESS — note this is INVERTED relative to WaitOne above
/// (which returns 1 on success) and relative to WaitAny (which returns an
/// index).  It follows the underlying PalEventWaitAll convention, where 0 is
/// "all signalled" (the C `WaitForMultipleObjects(fWaitAll)` shape) and -1 is
/// timeout/error.  Both others use "0 = did not happen"; this one uses
/// "0 = it happened".  Callers must NOT treat the return as a boolean.
CHAOS_IL2CPP_INT32 ChaosWaitHandleWaitAll(CHAOS_IL2CPP_INTPTR handle_ids,
                                          CHAOS_IL2CPP_INT32 count,
                                          CHAOS_IL2CPP_INT32 timeout_ms) noexcept;

/// Wait for ANY of the handles.  Returns the INDEX of the signalled handle
/// (0-based), or -1 on timeout/error — so callers must NOT treat the return as
/// a boolean, and index 0 is a real success.  Also note count <= 0 is rejected
/// rather than treated as a vacuous success.
CHAOS_IL2CPP_INT32 ChaosWaitHandleWaitAny(CHAOS_IL2CPP_INTPTR handle_ids,
                                          CHAOS_IL2CPP_INT32 count,
                                          CHAOS_IL2CPP_INT32 timeout_ms) noexcept;

// ══════════════════════════════════════════════════════════════════════
// Timer (System.Threading.Timer)
// ══════════════════════════════════════════════════════════════════════
//
// Backed by timer_queue.cpp.  The callback is a native function pointer plus an
// opaque state — the same shape the queue itself takes — because a managed
// TimerCallback is already a delegate whose method_ptr is what gets called.

/// due_time_ms: 0 = fire immediately, else delay before the first tick.
/// period_ms: 0 = one-shot, else the repeat interval.
/// Returns a nonzero timer id, or 0 on failure.
CHAOS_IL2CPP_INTPTR ChaosTimerCreate(CHAOS_IL2CPP_INTPTR callback,
                                     CHAOS_IL2CPP_INTPTR state,
                                     CHAOS_IL2CPP_INT32 due_time_ms,
                                     CHAOS_IL2CPP_INT32 period_ms) noexcept;

/// Change an existing timer's due time and period.  Returns nonzero on success.
CHAOS_IL2CPP_INT32 ChaosTimerChange(CHAOS_IL2CPP_INTPTR timer_id,
                                    CHAOS_IL2CPP_INT32 due_time_ms,
                                    CHAOS_IL2CPP_INT32 period_ms) noexcept;

/// Delete a timer.  Returns nonzero if it was found.  Idempotent.
CHAOS_IL2CPP_INT32 ChaosTimerDelete(CHAOS_IL2CPP_INTPTR timer_id) noexcept;

// ══════════════════════════════════════════════════════════════════════
// ThreadPool
// ══════════════════════════════════════════════════════════════════════
//
// QueueUserWorkItem takes the same (callback, state) pair the queue does.
// `unsafe_` selects the no-ExecutionContext-capture path; the distinction is
// observable to managed code (AsyncLocal inside the callback), so both are
// exported rather than collapsing to one.

CHAOS_IL2CPP_INT32 ChaosThreadPoolQueueUserWorkItem(CHAOS_IL2CPP_INTPTR callback,
                                                    CHAOS_IL2CPP_INTPTR state) noexcept;
CHAOS_IL2CPP_INT32 ChaosThreadPoolQueueUserWorkItemUnsafe(CHAOS_IL2CPP_INTPTR callback,
                                                          CHAOS_IL2CPP_INTPTR state) noexcept;

/// Current worker count (>= 0).
CHAOS_IL2CPP_INT32 ChaosThreadPoolGetWorkerCount() noexcept;

/// Ensure the pool is started.  Idempotent; safe to call before any queueing.
CHAOS_IL2CPP_INT32 ChaosThreadPoolEnsureInitialized() noexcept;

/// Shut the pool down (drain, join workers, stop the gate thread).
///
/// Exported because a started pool OWNS LIVE THREADS, and a process that
/// started one and never stopped it can exit abnormally even after all its work
/// completed — observed as exit code 3 with every test reporting PASSED, which
/// ctest then records as a failure.  Managed shutdown paths (AppDomain unload,
/// a test fixture's TearDown) need a way to close it out.
CHAOS_IL2CPP_INT32 ChaosThreadPoolShutdown() noexcept;

}  // extern "C"
