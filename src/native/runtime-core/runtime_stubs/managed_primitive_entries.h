// ── T2.2: managed-shaped entries for instance threading primitives ──────────
//
// WHAT THESE ARE FOR
// ------------------
// The T2.0 exports in synchronization_stubs.h are the *handle* ABI: they take a
// slot id (`ChaosReaderWriterLockSlimEnterRead(rw_id, timeout_ms)`).  Generated
// C++ for the managed types cannot call them, because codegen does not pass the
// receiver at all — see the doc block on
// `RegisterReaderWriterLockSlimAndSemaphoreSlim` in
// RuntimeHelperShapeRegistry.CoreStubs.Part1.S16.cs for the generated body that
// shows this.
//
// So each entry here takes the MANAGED INSTANCE as its first argument, recovers
// the native handle from the instance's own handle field (T2.1's
// ChaosManagedHandleGetOrCreate / Load / Release), and then calls the T2.0
// handle ABI.  This file is the joint between T2.1's binding and T2.0's ABI; it
// deliberately adds no third mechanism.
//
// ── The zero-handle case is the important one ──
// `SubjectInstanceFactory.Create<T>()` is `RuntimeHelpers.GetUninitializedObject`
// (src/reference/Chaos.TestFramework.Sdk/SubjectInstanceFactory.cs:17-22): it
// allocates a ZERO-INITIALISED object and runs NO constructor.  The handle field
// is therefore 0, meaning "never bound" — not "bound to something".
//
// Every entry below treats that as a failure and returns a distinguishable
// value.  It must NOT silently succeed: on a real ReaderWriterLockSlim,
// `ExitReadLock()` without a preceding successful `EnterReadLock()` throws
// SynchronizationLockException, and a helper that answered "fine" would make
// "the constructor never ran" indistinguishable from "the lock is held" — the
// exact false-pass this phase exists to remove.
//
// ── Return convention (managed semantics, NOT the T2.0 convention) ──
// These are the *managed* surfaces, so they follow managed types:
//   * bool-returning (TryEnter*, and the void ones lowered as INTPTR)
//       -> 1 = true, 0 = false
//   * the void lowered as INTPTR -> the value is unobservable; 0 is returned.
// Callers must not read a 0 from a `TryEnter` as "error" — it means "did not
// acquire", which is a normal answer.
//
// ── Why "Infinite" entries exist ──
// The managed `EnterReadLock()` / `EnterWriteLock()` / `EnterUpgradeableReadLock()`
// take NO argument, but the native enter takes a timeout.  The entry supplies
// -1 (infinite) itself.  Modelling that as a managed parameter would invent an
// argument the subject id does not list.

#pragma once

#include <chaos/native_types.h>

extern "C" {

// ══════════════════════════════════════════════════════════════════════
// ReaderWriterLockSlim — enter (infinite wait)
// ══════════════════════════════════════════════════════════════════════
//
// `rw` is the managed instance.  Returns 1 = acquired, 0 = failed (including
// "no handle", which is not represented as a throw here).

CHAOS_IL2CPP_INT32 ChaosReaderWriterLockSlimEnterReadLockInfinite(
    CHAOS_IL2CPP_INTPTR rw) noexcept;

CHAOS_IL2CPP_INT32 ChaosReaderWriterLockSlimEnterWriteLockInfinite(
    CHAOS_IL2CPP_INTPTR rw) noexcept;

CHAOS_IL2CPP_INT32 ChaosReaderWriterLockSlimEnterUpgradeableReadLockInfinite(
    CHAOS_IL2CPP_INTPTR rw) noexcept;

// ══════════════════════════════════════════════════════════════════════
// ReaderWriterLockSlim — exit
// ══════════════════════════════════════════════════════════════════════
//
// The mirror of the enters.  On real .NET these throw when the lock is not held
// in that mode; here they return 0 for that case, because the current generated
// call sites are `call` (not wrapped in managed EH) and a C++ throw would cross
// the boundary as an unhandled exception rather than a managed one.  Returning
// 0 is the honest answer available; see the TODO below.
//
// TODO(t2.2): when the exception plumbing for these call sites exists, raise a
// managed SynchronizationLockException instead of returning 0.  Until then a
// caller checking only "did it return" cannot tell "released" from "was not
// held" — which is why the behavioral test asserts on the LOCK STATE after the
// call, not on this return value.

CHAOS_IL2CPP_INT32 ChaosReaderWriterLockSlimExitReadLock(
    CHAOS_IL2CPP_INTPTR rw) noexcept;

CHAOS_IL2CPP_INT32 ChaosReaderWriterLockSlimExitWriteLock(
    CHAOS_IL2CPP_INTPTR rw) noexcept;

CHAOS_IL2CPP_INT32 ChaosReaderWriterLockSlimExitUpgradeableReadLock(
    CHAOS_IL2CPP_INTPTR rw) noexcept;

// ══════════════════════════════════════════════════════════════════════
// ReaderWriterLockSlim — try-enter (NOW WIRED: receiver injected)
// ══════════════════════════════════════════════════════════════════════
//
// The lowering-layer receiver injection (A.2) prepends the receiver slot,
// so the shim signature is (rw_handle + timeout).  Each entry recovers the
// native lock from the instance, then calls the T2.0 handle ABI.

CHAOS_IL2CPP_INT32 ChaosReaderWriterLockSlimTryEnterReadLockInt32(
    CHAOS_IL2CPP_INTPTR rw, CHAOS_IL2CPP_INT32 timeout_ms) noexcept;

CHAOS_IL2CPP_INT32 ChaosReaderWriterLockSlimTryEnterReadLockTimeSpan(
    CHAOS_IL2CPP_INTPTR rw, CHAOS_IL2CPP_INTPTR timespan_ticks) noexcept;

CHAOS_IL2CPP_INT32 ChaosReaderWriterLockSlimTryEnterWriteLockInt32(
    CHAOS_IL2CPP_INTPTR rw, CHAOS_IL2CPP_INT32 timeout_ms) noexcept;

CHAOS_IL2CPP_INT32 ChaosReaderWriterLockSlimTryEnterWriteLockTimeSpan(
    CHAOS_IL2CPP_INTPTR rw, CHAOS_IL2CPP_INTPTR timespan_ticks) noexcept;

CHAOS_IL2CPP_INT32 ChaosReaderWriterLockSlimTryEnterUpgradeableReadLockInt32(
    CHAOS_IL2CPP_INTPTR rw, CHAOS_IL2CPP_INT32 timeout_ms) noexcept;

CHAOS_IL2CPP_INT32 ChaosReaderWriterLockSlimTryEnterUpgradeableReadLockTimeSpan(
    CHAOS_IL2CPP_INTPTR rw, CHAOS_IL2CPP_INTPTR timespan_ticks) noexcept;

// ══════════════════════════════════════════════════════════════════════
// ReaderWriterLockSlim — Dispose
// ══════════════════════════════════════════════════════════════════════
//
// Releases the binding through ChaosManagedHandleRelease, i.e. clears the field
// THEN destroys.  Idempotent: a second Dispose finds the field already 0 and
// does nothing, which is what managed `Dispose()` + finalizer both running
// requires.  Returns 1 on the call that released, 0 otherwise.

CHAOS_IL2CPP_INT32 ChaosReaderWriterLockSlimDisposeManaged(
    CHAOS_IL2CPP_INTPTR rw) noexcept;

// ══════════════════════════════════════════════════════════════════════
// ManualResetEventSlim — T2.4
// ══════════════════════════════════════════════════════════════════════
//
// Backed by the T2.0 WaitHandle family with type discriminant 0 (manual), so
// there is no new native object here — only the instance→handle recovery.
//
// `Set` / `Dispose` are straightforward.  `Wait` has FIVE managed overloads
// ((), (CancellationToken), (int), (TimeSpan), (int, CancellationToken),
// (TimeSpan, CancellationToken)) and the chunk contains all of them; they
// differ only in how the timeout is encoded and whether a cancellation token is
// attached.  The token-bearing overloads are handled by their own entries below
// — a token that is already cancelled must NOT be ignored, or a caller's
// cancellation would silently become an infinite wait.
//
// Returns follow MANAGED semantics: Set/Dispose are INT32 (nonzero = did work),
// Wait returns 1 = signalled, 0 = not signalled (timeout or cancelled).

CHAOS_IL2CPP_INT32 ChaosManualResetEventSlimSetManaged(
    CHAOS_IL2CPP_INTPTR mres) noexcept;

CHAOS_IL2CPP_INT32 ChaosManualResetEventSlimResetManaged(
    CHAOS_IL2CPP_INTPTR mres) noexcept;

CHAOS_IL2CPP_INT32 ChaosManualResetEventSlimDisposeManaged(
    CHAOS_IL2CPP_INTPTR mres) noexcept;

/// Wait() — infinite.
CHAOS_IL2CPP_INT32 ChaosManualResetEventSlimWaitManaged(
    CHAOS_IL2CPP_INTPTR mres) noexcept;

/// Wait(int timeout_ms) — -1 = infinite, 0 = poll.
CHAOS_IL2CPP_INT32 ChaosManualResetEventSlimWaitInt32(
    CHAOS_IL2CPP_INTPTR mres, CHAOS_IL2CPP_INT32 timeout_ms) noexcept;

/// Wait(TimeSpan timeout) — tick carrier pointer.
CHAOS_IL2CPP_INT32 ChaosManualResetEventSlimWaitTimeSpan(
    CHAOS_IL2CPP_INTPTR mres, CHAOS_IL2CPP_INTPTR timespan_ticks) noexcept;

// ══════════════════════════════════════════════════════════════════════
// SpinLock — T2.5 (shim-matching signatures)
// ══════════════════════════════════════════════════════════════════════
//
// SpinLock is a VALUE TYPE, so the receiver is a pointer to the struct's own
// storage.  A.2's receiver injection supplies it as chaos_fn_arg_0, so these
// entries operate on the ACTUAL instance (real per-instance mutual exclusion).
//
// Shim signatures after injection:
//   Enter(ref bool)              → (spinlock, &lockTaken)
//   TryEnter(ref bool)           → (spinlock, &lockTaken)
//   TryEnter(int, ref bool)      → (spinlock, timeout, &lockTaken)
//   TryEnter(TimeSpan, ref bool) → (spinlock, ticks, &lockTaken)
//   Exit()                       → (spinlock)

CHAOS_IL2CPP_INT32 ChaosSpinLockEnter(CHAOS_IL2CPP_INTPTR spinlock,
                                      CHAOS_IL2CPP_INTPTR lock_taken_out) noexcept;

CHAOS_IL2CPP_INT32 ChaosSpinLockTryEnter(CHAOS_IL2CPP_INTPTR spinlock,
                                         CHAOS_IL2CPP_INTPTR lock_taken_out) noexcept;

CHAOS_IL2CPP_INT32 ChaosSpinLockTryEnterInt32(CHAOS_IL2CPP_INTPTR spinlock,
                                              CHAOS_IL2CPP_INT32 timeout_ms,
                                              CHAOS_IL2CPP_INTPTR lock_taken_out) noexcept;

CHAOS_IL2CPP_INT32 ChaosSpinLockTryEnterTimeSpan(CHAOS_IL2CPP_INTPTR spinlock,
                                                 CHAOS_IL2CPP_INTPTR timespan_ticks,
                                                 CHAOS_IL2CPP_INTPTR lock_taken_out) noexcept;

/// Exit() — release.  Returns 1 = released, 0 = was not held by this thread.
CHAOS_IL2CPP_INT32 ChaosSpinLockExit(CHAOS_IL2CPP_INTPTR spinlock) noexcept;

// ══════════════════════════════════════════════════════════════════════
// SpinWait — T2.5
// ══════════════════════════════════════════════════════════════════════
//
// NOTE THE ARGUMENT COUNT — it differs from SpinLock, and getting it wrong is
// a compile error rather than a silent bug (the generated shim declares exactly
// the slots the registry lists, so a mismatch is caught by the C++ compiler).
//
// The chunk's generated shims are:
//     SpinOnce_System_Void_System_Int32_(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
//         -> ChaosSpinWaitSpinOnceInt32(chaos_fn_arg_0)
//     SpinOnce_System_Void_(chaos_fn_arg_0)
//         -> ChaosSpinWaitSpinOnce(chaos_fn_arg_0)
//
// i.e. codegen passes NO receiver for SpinWait — the subject's `this` is not an
// ABI slot.  `SpinWait.SpinOnce()` takes no managed argument either, yet the
// shim still declares one INTPTR: the callee subject id is unqualified, so the
// lowering supplies a single slot.  The function below therefore takes ONE
// pointer argument and must not require a second.
//
// Consequence: there is no struct to advance here.  SpinWait's escalating
// counter lives in managed state the entry cannot reach, so these entries
// perform the SPIN itself (pause, then yield past a threshold) rather than
// tracking a count across calls.  That is the observable part of SpinOnce's
// contract; the internal counter is not visible to managed callers either.

CHAOS_IL2CPP_INT32 ChaosSpinWaitSpinOnce(CHAOS_IL2CPP_INTPTR arg) noexcept;

CHAOS_IL2CPP_INT32 ChaosSpinWaitSpinOnceInt32(CHAOS_IL2CPP_INTPTR iterations) noexcept;

// ══════════════════════════════════════════════════════════════════════
// ThreadPool — T2.5 (callable surface only)
// ══════════════════════════════════════════════════════════════════════
//
// Only the four operations the current native pool actually implements are
// exported.  `GetAvailableThreads`/`GetMaxThreads`/`GetMinThreads` have NO
// native counterpart today (the pool has no max/min configuration surface), so
// they are deliberately NOT claimed here — registering them against a
// placeholder would reproduce exactly the fake-pass this phase removes.
//
// `QueueUserWorkItem` takes (callback, state) — the same pair the T2.0 ABI and
// the queue itself use.

CHAOS_IL2CPP_INT32 ChaosThreadPoolQueueUserWorkItemManaged(
    CHAOS_IL2CPP_INTPTR callback, CHAOS_IL2CPP_INTPTR state) noexcept;

CHAOS_IL2CPP_INT32 ChaosThreadPoolQueueUserWorkItemUnsafeManaged(
    CHAOS_IL2CPP_INTPTR callback, CHAOS_IL2CPP_INTPTR state) noexcept;

/// Current worker count (>= 0).
CHAOS_IL2CPP_INT32 ChaosThreadPoolGetWorkerCountManaged() noexcept;

}  // extern "C"
