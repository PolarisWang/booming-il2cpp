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
// ReaderWriterLockSlim — try-enter (bounded wait)
// ══════════════════════════════════════════════════════════════════════
//
// Two encodings per mode, because the managed overloads differ in how the
// timeout is expressed:
//   * Int32    — `timeout_ms` directly ( -1 = infinite, 0 = poll )
//   * TimeSpan — an INTPTR to the 8-byte tick carrier; converted to ms here.
//
// Returns 1 = acquired, 0 = timeout / not acquired.

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

}  // extern "C"
