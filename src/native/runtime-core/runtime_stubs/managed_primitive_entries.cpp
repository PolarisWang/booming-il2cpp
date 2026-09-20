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

// ── ReaderWriterLockSlim — try-enter (NOW WIRED: receiver injected by A.2) ──
//
// With the lowering-layer receiver injection, shims for these methods carry
// the instance handle as chaos_fn_arg_0 and the managed parameter(s) after it.
// Each entry recovers the native lock from the instance handle, then calls the
// T2.0 handle ABI with the parameter.

CHAOS_IL2CPP_INT32 ChaosReaderWriterLockSlimTryEnterReadLockInt32(
    CHAOS_IL2CPP_INTPTR rw, CHAOS_IL2CPP_INT32 timeout_ms) noexcept
{
    return EnterWithHandle(rw, timeout_ms, &ChaosReaderWriterLockSlimEnterRead);
}

CHAOS_IL2CPP_INT32 ChaosReaderWriterLockSlimTryEnterReadLockTimeSpan(
    CHAOS_IL2CPP_INTPTR rw, CHAOS_IL2CPP_INTPTR timespan_ticks) noexcept
{
    return EnterWithHandle(rw, TimeSpanTicksToMillis(timespan_ticks),
                           &ChaosReaderWriterLockSlimEnterRead);
}

CHAOS_IL2CPP_INT32 ChaosReaderWriterLockSlimTryEnterWriteLockInt32(
    CHAOS_IL2CPP_INTPTR rw, CHAOS_IL2CPP_INT32 timeout_ms) noexcept
{
    return EnterWithHandle(rw, timeout_ms, &ChaosReaderWriterLockSlimEnterWrite);
}

CHAOS_IL2CPP_INT32 ChaosReaderWriterLockSlimTryEnterWriteLockTimeSpan(
    CHAOS_IL2CPP_INTPTR rw, CHAOS_IL2CPP_INTPTR timespan_ticks) noexcept
{
    return EnterWithHandle(rw, TimeSpanTicksToMillis(timespan_ticks),
                           &ChaosReaderWriterLockSlimEnterWrite);
}

CHAOS_IL2CPP_INT32 ChaosReaderWriterLockSlimTryEnterUpgradeableReadLockInt32(
    CHAOS_IL2CPP_INTPTR rw, CHAOS_IL2CPP_INT32 timeout_ms) noexcept
{
    return EnterWithHandle(rw, timeout_ms,
                           &ChaosReaderWriterLockSlimEnterUpgradeableRead);
}

CHAOS_IL2CPP_INT32 ChaosReaderWriterLockSlimTryEnterUpgradeableReadLockTimeSpan(
    CHAOS_IL2CPP_INTPTR rw, CHAOS_IL2CPP_INTPTR timespan_ticks) noexcept
{
    return EnterWithHandle(rw, TimeSpanTicksToMillis(timespan_ticks),
                           &ChaosReaderWriterLockSlimEnterUpgradeableRead);
}

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
    CHAOS_IL2CPP_INTPTR mres, CHAOS_IL2CPP_INTPTR /*token*/) noexcept
{
    const CHAOS_IL2CPP_INTPTR handle = BoundHandle(mres);
    if (handle == 0) return 0;
    // 1 = signalled, 0 = timeout, -1 = error.  Anything that is not a
    // definite "signalled" must not be reported as true.
    return (ChaosWaitHandleWaitOne(handle, kInfinite) == 1) ? 1 : 0;
}

CHAOS_IL2CPP_INT32 ChaosManualResetEventSlimWaitInt32(
    CHAOS_IL2CPP_INTPTR mres, CHAOS_IL2CPP_INT32 timeout_ms, CHAOS_IL2CPP_INTPTR /*token*/) noexcept
{
    const CHAOS_IL2CPP_INTPTR handle = BoundHandle(mres);
    if (handle == 0) return 0;
    return (ChaosWaitHandleWaitOne(handle, timeout_ms) == 1) ? 1 : 0;
}

CHAOS_IL2CPP_INT32 ChaosManualResetEventSlimWaitTimeSpan(
    CHAOS_IL2CPP_INTPTR mres, CHAOS_IL2CPP_INTPTR timespan_ticks, CHAOS_IL2CPP_INTPTR /*token*/) noexcept
{
    const CHAOS_IL2CPP_INTPTR handle = BoundHandle(mres);
    if (handle == 0) return 0;
    return (ChaosWaitHandleWaitOne(handle, TimeSpanTicksToMillis(timespan_ticks)) == 1)
               ? 1
               : 0;
}

// ══════════════════════════════════════════════════════════════════════
// SpinLock — T2.5 (per-instance, receiver now injected by A.2)
// ══════════════════════════════════════════════════════════════════════
//
// SpinLock is a VALUE TYPE, so the receiver is a pointer to the struct's own
// storage — the lock word lives in the caller's stack slot.  A.2's receiver
// injection now supplies it as chaos_fn_arg_0, so these entries operate on the
// ACTUAL SpinLock instance instead of a process-global word.  That restores
// real per-instance mutual exclusion.
//
// The lock word is 0 (free) or the owning ThreadId.  Storing the id rather than
// a bool is what lets Exit verify ownership — a mispaired Exit is detectable
// instead of silently corrupting the lock.
//
// ⚠️ NOT EVERY RECEIVER ARRIVING HERE IS A STACK SLOT.  The value-type premise
// above holds only when the callee is the BCL `System.Threading.SpinLock`
// struct.  ATG also generates subjects over reference-type "SpinLock"-named
// types (e.g. `Chaos.TestFramework.Sdk.SpinLock`, which codegen emits as
// `struct ... : public System_Object`).  `SubjectInstanceFactory.Create<T>()`
// builds those through RuntimeHelpers.GetUninitializedObject, and codegen then
// passes `&chaos_locals[k]` — the address of the SLOT holding the object
// pointer.  Interpreting those bytes as an `atomic<int32_t>` reads the
// object's non-zero header, so CAS(0 -> self) can NEVER succeed and the
// `for(;;)` below spins at 100% CPU forever.  That was a real fact-run
// timeout, not a slow test.
//
// The guard below detects the impossible-for-a-free-lock case (the word is
// neither 0 nor a ThreadId we could have stored) and answers instead of
// spinning.  It is deliberately conservative: it must not turn a genuinely
// contended lock into a silent success (see the S16 note that "the constructor
// never ran" must not be indistinguishable from "the lock is held"), so the
// caller is told the lock was NOT taken.

namespace {

/// Managed thread ids are dense ints allocated from `s_next_thread_id` starting
/// at kMainThreadId + 1 (see thread_state.cpp).  No realistic run registers
/// anywhere near this many threads, so a lock word at or above it is not an id
/// this runtime ever wrote — it is the low half of an image pointer.
constexpr CHAOS_IL2CPP_UINT32 kMaxThreadIdValue = 1u << 20;

/// True when `word` cannot be a lock word this runtime ever wrote: a lock word
/// is 0 (free) or the id of a thread that took it.  An object header /
/// MethodTable pointer is a 64-bit-aligned image address whose low 32 bits sit
/// far above any allocated thread id.
///
/// Reads the full 64-bit word so a pointer-slot is not mistaken for an int32:
/// a stack-slot address like 0x0000007ff7xxxxxx has a low-32 that looks like a
/// large int32, but the complete 64-bit value is unmistakably a user-space
/// address.
bool LooksLikeObjectHeaderNotLockWord(const void* slot) noexcept {
    CHAOS_IL2CPP_UINT64 raw = 0;
    std::memcpy(&raw, slot, sizeof(raw));
    if (raw == 0) return false;                 // free lock word
    const CHAOS_IL2CPP_UINT64 low = raw & 0xFFFFFFFFULL;
    if (low != 0 && low < kMaxThreadIdValue) return false;   // a plausible ThreadId
    return true;                                // neither 0 nor a plausible id
}

}  // anonymous namespace

CHAOS_IL2CPP_INT32 ChaosSpinLockEnter(CHAOS_IL2CPP_INTPTR spinlock,
                                      CHAOS_IL2CPP_INTPTR lock_taken_out) noexcept
{
    if (lock_taken_out == 0) return 0;
    if (spinlock == 0) {
        *reinterpret_cast<CHAOS_IL2CPP_INT32*>(lock_taken_out) = 0;
        return 0;
    }
    if (LooksLikeObjectHeaderNotLockWord(reinterpret_cast<const void*>(spinlock))) {
        // Not a lock word: report "not acquired" rather than spinning forever.
        // The caller's Assert.AreEqual(true, lockTaken) then fails loudly,
        // which is the honest outcome — the receiver was never a SpinLock.
        *reinterpret_cast<CHAOS_IL2CPP_INT32*>(lock_taken_out) = 0;
        return 0;
    }
    auto* word = reinterpret_cast<std::atomic<CHAOS_IL2CPP_INT32>*>(spinlock);

    const CHAOS_IL2CPP_INT32 self =
        chaos::il2cpp::runtime_core::threading::GetCurrentThreadId();
    for (;;) {
        CHAOS_IL2CPP_INT32 expected = 0;
        if (word->compare_exchange_weak(expected, self,
                std::memory_order_acquire, std::memory_order_relaxed)) {
            *reinterpret_cast<CHAOS_IL2CPP_INT32*>(lock_taken_out) = 1;
            return 1;
        }
        CHAOS_IL2CPP_PAUSE_HINT();
    }
}

CHAOS_IL2CPP_INT32 ChaosSpinLockTryEnter(CHAOS_IL2CPP_INTPTR spinlock,
                                         CHAOS_IL2CPP_INTPTR lock_taken_out) noexcept
{
    if (lock_taken_out == 0) return 0;
    if (spinlock == 0) {
        *reinterpret_cast<CHAOS_IL2CPP_INT32*>(lock_taken_out) = 0;
        return 0;
    }
    if (LooksLikeObjectHeaderNotLockWord(reinterpret_cast<const void*>(spinlock))) {
        *reinterpret_cast<CHAOS_IL2CPP_INT32*>(lock_taken_out) = 0;
        return 0;
    }
    auto* word = reinterpret_cast<std::atomic<CHAOS_IL2CPP_INT32>*>(spinlock);
    const CHAOS_IL2CPP_INT32 self =
        chaos::il2cpp::runtime_core::threading::GetCurrentThreadId();
    CHAOS_IL2CPP_INT32 expected = 0;
    if (word->compare_exchange_weak(expected, self,
            std::memory_order_acquire, std::memory_order_relaxed)) {
        *reinterpret_cast<CHAOS_IL2CPP_INT32*>(lock_taken_out) = 1;
        return 1;
    }
    *reinterpret_cast<CHAOS_IL2CPP_INT32*>(lock_taken_out) = 0;
    return 0;
}

CHAOS_IL2CPP_INT32 ChaosSpinLockTryEnterInt32(CHAOS_IL2CPP_INTPTR spinlock,
                                              CHAOS_IL2CPP_INT32 timeout_ms,
                                              CHAOS_IL2CPP_INTPTR lock_taken_out) noexcept
{
    if (lock_taken_out == 0) return 0;
    if (spinlock == 0) {
        *reinterpret_cast<CHAOS_IL2CPP_INT32*>(lock_taken_out) = 0;
        return 0;
    }
    if (LooksLikeObjectHeaderNotLockWord(reinterpret_cast<const void*>(spinlock))) {
        *reinterpret_cast<CHAOS_IL2CPP_INT32*>(lock_taken_out) = 0;
        return 0;
    }
    if (timeout_ms == 0) return ChaosSpinLockTryEnter(spinlock, lock_taken_out);

    auto* word = reinterpret_cast<std::atomic<CHAOS_IL2CPP_INT32>*>(spinlock);
    const CHAOS_IL2CPP_INT32 self =
        chaos::il2cpp::runtime_core::threading::GetCurrentThreadId();
    for (;;) {
        CHAOS_IL2CPP_INT32 expected = 0;
        if (word->compare_exchange_weak(expected, self,
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

CHAOS_IL2CPP_INT32 ChaosSpinLockTryEnterTimeSpan(CHAOS_IL2CPP_INTPTR spinlock,
                                                 CHAOS_IL2CPP_INTPTR timespan_ticks,
                                                 CHAOS_IL2CPP_INTPTR lock_taken_out) noexcept
{
    return ChaosSpinLockTryEnterInt32(spinlock, TimeSpanTicksToMillis(timespan_ticks),
                                      lock_taken_out);
}

CHAOS_IL2CPP_INT32 ChaosSpinLockExit(CHAOS_IL2CPP_INTPTR spinlock) noexcept
{
    if (spinlock == 0) return 0;
    auto* word = reinterpret_cast<std::atomic<CHAOS_IL2CPP_INT32>*>(spinlock);
    const CHAOS_IL2CPP_INT32 self =
        chaos::il2cpp::runtime_core::threading::GetCurrentThreadId();
    CHAOS_IL2CPP_INT32 expected = self;
    if (word->compare_exchange_strong(expected, 0,
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
