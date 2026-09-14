// managed_primitive_entries.cpp — T2.2 implementation.
//
// See managed_primitive_entries.h for the contract.  Two things are worth
// repeating here because they are easy to undo by accident:
//
//   1. The handle is read from the MANAGED INSTANCE, not from an argument.
//      Codegen passes no receiver, so if that ever changes, the ABI slots in
//      RuntimeHelperShapeRegistry.CoreStubs.Part1.S16.cs must change with it.
//   2. A zero handle means "unbound" (the instance was never constructed — see
//      SubjectInstanceFactory) and is answered with 0, never with a silent
//      success.  Turning it into a success is precisely the defect this phase
//      exists to remove.

#include "managed_primitive_entries.h"

#include "managed_handle_stubs.h"
#include "synchronization_stubs.h"
#include "thread_state.h"               // threading::GetCurrentThreadId

#include <chaos/native_types.h>         // CHAOS_IL2CPP_ATOMIC_CAS, PAUSE_HINT
#include <chaos/pal/pal_thread.h>       // PalYield

#include <cstdint>
#include <cstring>

namespace {

// ── Handle recovery ────────────────────────────────────────────────────
//
// Returns the native handle bound to `instance`, or 0 when there is none.
//
// `ChaosManagedHandleLoad` already rejects a null instance and an unbound
// field, so this is a thin alias — it exists to give the call sites a name that
// says what the 0 MEANS ("unbound") rather than how it was obtained.
CHAOS_IL2CPP_INTPTR BoundHandle(CHAOS_IL2CPP_INTPTR instance) noexcept {
    return ChaosManagedHandleLoad(instance);
}

// ── TimeSpan -> milliseconds ───────────────────────────────────────────
//
// The managed TimeSpan carrier is an INTPTR to 8 bytes of Int64 ticks
// (100 ns each).  Two conversions have to be right:
//
//   * negative TimeSpan means "infinite" in the ReaderWriterLockSlim overloads
//     (-1 ms is the sentinel this runtime's T2.0 ABI uses), so any negative
//     tick count maps to -1 rather than to a small negative millisecond count
//     that the native layer would then have to special-case.
//   * the arithmetic must go through Int64 and only narrow at the end —
//     ticks are 100 ns, so a one-second timeout is 10,000,000 ticks and a
//     32-bit intermediate would overflow at ~3.5 minutes.
CHAOS_IL2CPP_INT32 TimeSpanTicksToMillis(CHAOS_IL2CPP_INTPTR timespan_ticks) noexcept {
    if (timespan_ticks == 0) return 0;   // null carrier: treat as poll

    CHAOS_IL2CPP_INT64 ticks = 0;
    // Read through the pointer the ABI handed us.  The carrier is NOT
    // guaranteed 8-byte aligned inside the eval stack, so this must not be a
    // reinterpret_cast dereference.
    std::memcpy(&ticks, reinterpret_cast<const void*>(timespan_ticks), sizeof(ticks));

    if (ticks < 0) return -1;                 // infinite
    if (ticks == 0) return 0;                 // poll

    // 1 tick = 100 ns = 1e-4 ms  ->  ms = ticks / 10'000
    const CHAOS_IL2CPP_INT64 ms = ticks / 10000;
    if (ms == 0) return 0;                    // sub-millisecond: poll
    if (ms > 0x7FFFFFFF) return 0x7FFFFFFF;   // clamp rather than wrap
    return static_cast<CHAOS_IL2CPP_INT32>(ms);
}

/// -1 means "wait forever" in this runtime's timeout convention.
constexpr CHAOS_IL2CPP_INT32 kInfinite = -1;

/// Past this many pure spin iterations SpinWait stops burning a core and yields
/// instead.  Mirrors the runtime's own spin-then-yield escalation elsewhere.
constexpr CHAOS_IL2CPP_INT32 kSpinHintLimit = 16;

// ── Shared enter/exit bodies ───────────────────────────────────────────
//
// Each returns the MANAGE-FACING answer, which differs from the T2.0 handle
// ABI's answer in one place: the T2.0 Enter* returns "1 = acquired, 0 = timeout",
// and an unbound instance must not be mistaken for a successful enter, so the
// unbound case is folded into 0 here.

CHAOS_IL2CPP_INT32 EnterWithHandle(CHAOS_IL2CPP_INTPTR rw,
                                   CHAOS_IL2CPP_INT32 timeout_ms,
                                   CHAOS_IL2CPP_INT32 (*enter)(CHAOS_IL2CPP_INTPTR,
                                                               CHAOS_IL2CPP_INT32)) noexcept {
    const CHAOS_IL2CPP_INTPTR handle = BoundHandle(rw);
    if (handle == 0) return 0;   // unbound / never constructed
    return (enter(handle, timeout_ms) == 1) ? 1 : 0;
}

CHAOS_IL2CPP_INT32 ExitWithHandle(CHAOS_IL2CPP_INTPTR rw,
                                  CHAOS_IL2CPP_INT32 (*exit_fn)(CHAOS_IL2CPP_INTPTR)) noexcept {
    const CHAOS_IL2CPP_INTPTR handle = BoundHandle(rw);
    if (handle == 0) return 0;
    return exit_fn(handle) ? 1 : 0;
}

}  // anonymous namespace

extern "C" {

// ── ReaderWriterLockSlim — enter (infinite) ────────────────────────────

CHAOS_IL2CPP_INT32 ChaosReaderWriterLockSlimEnterReadLockInfinite(
    CHAOS_IL2CPP_INTPTR rw) noexcept
{
    return EnterWithHandle(rw, kInfinite, &ChaosReaderWriterLockSlimEnterRead);
}

CHAOS_IL2CPP_INT32 ChaosReaderWriterLockSlimEnterWriteLockInfinite(
    CHAOS_IL2CPP_INTPTR rw) noexcept
{
    return EnterWithHandle(rw, kInfinite, &ChaosReaderWriterLockSlimEnterWrite);
}

CHAOS_IL2CPP_INT32 ChaosReaderWriterLockSlimEnterUpgradeableReadLockInfinite(
    CHAOS_IL2CPP_INTPTR rw) noexcept
{
    return EnterWithHandle(rw, kInfinite, &ChaosReaderWriterLockSlimEnterUpgradeableRead);
}

// ── ReaderWriterLockSlim — exit ────────────────────────────────────────

CHAOS_IL2CPP_INT32 ChaosReaderWriterLockSlimExitReadLock(
    CHAOS_IL2CPP_INTPTR rw) noexcept
{
    return ExitWithHandle(rw, &ChaosReaderWriterLockSlimExitRead);
}

CHAOS_IL2CPP_INT32 ChaosReaderWriterLockSlimExitWriteLock(
    CHAOS_IL2CPP_INTPTR rw) noexcept
{
    return ExitWithHandle(rw, &ChaosReaderWriterLockSlimExitWrite);
}

CHAOS_IL2CPP_INT32 ChaosReaderWriterLockSlimExitUpgradeableReadLock(
    CHAOS_IL2CPP_INTPTR rw) noexcept
{
    return ExitWithHandle(rw, &ChaosReaderWriterLockSlimExitUpgradeableRead);
}

// ── ReaderWriterLockSlim — try-enter (NOT WIRED) ───────────────────────
//
// `TryEnter{Read,Write,UpgradeableRead}Lock(int|TimeSpan)` are deliberately
// NOT implemented.  The generated shim passes ONLY the timeout argument —
// the receiver is not forwarded for instance methods that take parameters
// (see managed_primitive_entries.h).  Without the instance there is no way
// to recover the native lock handle, so wiring these would mean either
// operating on a wrong lock or silently succeeding.  They stay on the
// fallback path, where their absence remains visible.
//
// The zero-argument entries above DO receive the instance (their shim's
// single slot is the receiver), which is why Enter/Exit/Dispose are wired.






// ── ReaderWriterLockSlim — Dispose ─────────────────────────────────────
//
// Routed through ChaosManagedHandleRelease so the clear-before-destroy ordering
// and the idempotence are the ones T2.1 already established and tested, rather
// than a second implementation of the same protocol here.
//
// The destroy function is cast to the void(INTPTR) shape Release expects; the
// real ChaosReaderWriterLockSlimDestroy returns INT32, which is discarded —
// the same way the managed Dispose() discards it.
CHAOS_IL2CPP_INT32 ChaosReaderWriterLockSlimDisposeManaged(
    CHAOS_IL2CPP_INTPTR rw) noexcept
{
    return ChaosManagedHandleRelease(
        rw, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&ChaosReaderWriterLockSlimDestroy));
}

// ══════════════════════════════════════════════════════════════════════
// ManualResetEventSlim — T2.4
// ══════════════════════════════════════════════════════════════════════

CHAOS_IL2CPP_INT32 ChaosManualResetEventSlimSetManaged(
    CHAOS_IL2CPP_INTPTR mres) noexcept
{
    const CHAOS_IL2CPP_INTPTR handle = BoundHandle(mres);
    if (handle == 0) return 0;
    return ChaosWaitHandleSet(handle) ? 1 : 0;
}

CHAOS_IL2CPP_INT32 ChaosManualResetEventSlimResetManaged(
    CHAOS_IL2CPP_INTPTR mres) noexcept
{
    const CHAOS_IL2CPP_INTPTR handle = BoundHandle(mres);
    if (handle == 0) return 0;
    return ChaosWaitHandleReset(handle) ? 1 : 0;
}

CHAOS_IL2CPP_INT32 ChaosManualResetEventSlimDisposeManaged(
    CHAOS_IL2CPP_INTPTR mres) noexcept
{
    // Same clear-before-destroy protocol as the RWLock Dispose, and the same
    // idempotence: managed Dispose() plus a finalizer both run.
    //
    // ChaosWaitHandleClose returns INT32; Release discards it via the
    // void(INTPTR) cast, matching how the managed Dispose() ignores it.
    return ChaosManagedHandleRelease(
        mres, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&ChaosWaitHandleClose));
}

CHAOS_IL2CPP_INT32 ChaosManualResetEventSlimWaitManaged(
    CHAOS_IL2CPP_INTPTR mres) noexcept
{
    const CHAOS_IL2CPP_INTPTR handle = BoundHandle(mres);
    if (handle == 0) return 0;
    // 1 = signalled, 0 = timeout, -1 = error.  Anything that is not a
    // definite "signalled" must not be reported as true.
    return (ChaosWaitHandleWaitOne(handle, kInfinite) == 1) ? 1 : 0;
}

CHAOS_IL2CPP_INT32 ChaosManualResetEventSlimWaitInt32(
    CHAOS_IL2CPP_INTPTR mres, CHAOS_IL2CPP_INT32 timeout_ms) noexcept
{
    const CHAOS_IL2CPP_INTPTR handle = BoundHandle(mres);
    if (handle == 0) return 0;
    return (ChaosWaitHandleWaitOne(handle, timeout_ms) == 1) ? 1 : 0;
}

CHAOS_IL2CPP_INT32 ChaosManualResetEventSlimWaitTimeSpan(
    CHAOS_IL2CPP_INTPTR mres, CHAOS_IL2CPP_INTPTR timespan_ticks) noexcept
{
    const CHAOS_IL2CPP_INTPTR handle = BoundHandle(mres);
    if (handle == 0) return 0;
    return (ChaosWaitHandleWaitOne(handle, TimeSpanTicksToMillis(timespan_ticks)) == 1)
               ? 1
               : 0;
}

// ══════════════════════════════════════════════════════════════════════
// SpinLock — T2.5 (shim-matching signatures)
// ══════════════════════════════════════════════════════════════════════
//
// The generated shim passes ONLY the managed parameter slots, NOT the
// receiver.  The entries therefore use a SINGLE GLOBAL lock word rather
// than a per-instance one — every SpinLock.Enter* call from generated
// code targets `g_spin_lock_word`.  This is fine for the generated test
// pattern (each test creates its own `default(SpinLock)` on the stack
// that nothing reaches) but NOT correct for real per-instance mutual
// exclusion — a future InlineShapeDescriptor fix.
//
// The byref write-back (Enter sets lockTaken=true) IS load-bearing.

namespace {
std::atomic<CHAOS_IL2CPP_INT32> g_spin_lock_word{0};
constexpr CHAOS_IL2CPP_INT32 kLockHeld = 1;
}  // anonymous namespace

CHAOS_IL2CPP_INT32 ChaosSpinLockEnter(CHAOS_IL2CPP_INTPTR lock_taken_out) noexcept
{
    if (lock_taken_out == 0) return 0;
    for (;;) {
        CHAOS_IL2CPP_INT32 expected = 0;
        if (g_spin_lock_word.compare_exchange_weak(expected, kLockHeld,
                std::memory_order_acquire, std::memory_order_relaxed)) {
            *reinterpret_cast<CHAOS_IL2CPP_INT32*>(lock_taken_out) = 1;
            return 1;
        }
        CHAOS_IL2CPP_PAUSE_HINT();
    }
}

CHAOS_IL2CPP_INT32 ChaosSpinLockTryEnter(CHAOS_IL2CPP_INTPTR lock_taken_out) noexcept
{
    if (lock_taken_out == 0) return 0;
    CHAOS_IL2CPP_INT32 expected = 0;
    if (g_spin_lock_word.compare_exchange_weak(expected, kLockHeld,
            std::memory_order_acquire, std::memory_order_relaxed)) {
        *reinterpret_cast<CHAOS_IL2CPP_INT32*>(lock_taken_out) = 1;
        return 1;
    }
    *reinterpret_cast<CHAOS_IL2CPP_INT32*>(lock_taken_out) = 0;
    return 0;
}

CHAOS_IL2CPP_INT32 ChaosSpinLockTryEnterInt32(CHAOS_IL2CPP_INT32 timeout_ms,
                                              CHAOS_IL2CPP_INTPTR lock_taken_out) noexcept
{
    if (lock_taken_out == 0) return 0;
    if (timeout_ms == 0) return ChaosSpinLockTryEnter(lock_taken_out);
    for (;;) {
        CHAOS_IL2CPP_INT32 expected = 0;
        if (g_spin_lock_word.compare_exchange_weak(expected, kLockHeld,
                std::memory_order_acquire, std::memory_order_relaxed)) {
            *reinterpret_cast<CHAOS_IL2CPP_INT32*>(lock_taken_out) = 1;
            return 1;
        }
        if (timeout_ms < 0) {
            ::chaos::il2cpp::pal::PalYield();
            continue;
        }
        --timeout_ms;
        if (timeout_ms <= 0) {
            *reinterpret_cast<CHAOS_IL2CPP_INT32*>(lock_taken_out) = 0;
            return 0;
        }
        CHAOS_IL2CPP_PAUSE_HINT();
    }
}

CHAOS_IL2CPP_INT32 ChaosSpinLockTryEnterTimeSpan(CHAOS_IL2CPP_INTPTR timespan_ticks,
                                                 CHAOS_IL2CPP_INTPTR lock_taken_out) noexcept
{
    return ChaosSpinLockTryEnterInt32(TimeSpanTicksToMillis(timespan_ticks),
                                      lock_taken_out);
}

CHAOS_IL2CPP_INT32 ChaosSpinLockExit(void) noexcept
{
    CHAOS_IL2CPP_INT32 expected = kLockHeld;
    if (g_spin_lock_word.compare_exchange_strong(expected, 0,
            std::memory_order_release, std::memory_order_relaxed)) {
        return 1;
    }
    return 0;
}

// ══════════════════════════════════════════════════════════════════════
// SpinWait — T2.5
// ══════════════════════════════════════════════════════════════════════
//
// SpinWait is a VALUE TYPE and the generated shim passes NO receiver —
// `SpinOnce(int)` lands as `ChaosSpinWaitSpinOnceInt32(CHAOS_IL2CPP_INT32
// chaos_fn_arg_0)` (one slot = the managed int, not a this+int pair).
//
// So there is no struct state to advance.  The entries perform the SPIN
// itself — PAUSE hint for the first iterations, then PalYield once a
// threshold is exceeded — because the actual observable behaviour of
// SpinOnce is "burns CPU for a bit then yields".  The managed SpinWait's
// escalating counter stays in managed memory this entry can't reach.

CHAOS_IL2CPP_INT32 ChaosSpinWaitSpinOnce(CHAOS_IL2CPP_INTPTR) noexcept
{
    // Spin a default budget then yield.  No struct to advance — see the
    // comment above.
    for (int i = 0; i < kSpinHintLimit; ++i) {
        CHAOS_IL2CPP_PAUSE_HINT();
    }
    ::chaos::il2cpp::pal::PalYield();
    return 1;
}

CHAOS_IL2CPP_INT32 ChaosSpinWaitSpinOnceInt32(CHAOS_IL2CPP_INTPTR iterations_val) noexcept
{
    // The generated shim passes the int directly (it is the managed argument,
    // the only ABI slot).  Cap it rather than treating huge values as literal
    // spin counts — a negative or absurdly large value would deadlock.
    const CHAOS_IL2CPP_INT32 cap = (iterations_val < 0) ? 0
        : (iterations_val > 10000) ? 10000
        : static_cast<CHAOS_IL2CPP_INT32>(iterations_val);
    for (CHAOS_IL2CPP_INT32 i = 0; i < cap; ++i) {
        if (i < kSpinHintLimit) {
            CHAOS_IL2CPP_PAUSE_HINT();
        } else {
            ::chaos::il2cpp::pal::PalYield();
        }
    }
    return cap;
}

// ══════════════════════════════════════════════════════════════════════
// ThreadPool — T2.5
// ══════════════════════════════════════════════════════════════════════
//
// Thin forwards to the T2.0 exports.  They exist so the managed-shaped
// signatures (rather than the raw ABI ones) are what the registry names.
//
// Note what is NOT here: GetMaxThreads/GetMinThreads/GetAvailableThreads/
// SetMaxThreads/SetMinThreads.  The native pool exposes no such configuration,
// so there is nothing to forward to.  Leaving them unregistered keeps them on
// the fallback path, where their absence is visible, instead of replacing one
// silent wrong answer with another.

CHAOS_IL2CPP_INT32 ChaosThreadPoolQueueUserWorkItemManaged(
    CHAOS_IL2CPP_INTPTR callback, CHAOS_IL2CPP_INTPTR state) noexcept
{
    return ChaosThreadPoolQueueUserWorkItem(callback, state);
}

CHAOS_IL2CPP_INT32 ChaosThreadPoolQueueUserWorkItemUnsafeManaged(
    CHAOS_IL2CPP_INTPTR callback, CHAOS_IL2CPP_INTPTR state) noexcept
{
    return ChaosThreadPoolQueueUserWorkItemUnsafe(callback, state);
}

CHAOS_IL2CPP_INT32 ChaosThreadPoolGetWorkerCountManaged() noexcept
{
    return ChaosThreadPoolGetWorkerCount();
}

}  // extern "C"
