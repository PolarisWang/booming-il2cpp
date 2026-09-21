// synchronization_stubs.cpp — extern "C" ABI exports for the threading
// primitives (T2.0).
//
// See synchronization_stubs.h for why this layer exists and why every symbol is
// C-linkage.  In short: the ShapeRegistry emits a DIRECT, UNQUALIFIED call to
// the symbol named in the registry entry, from a generated translation unit, so
// the implementations have to be reachable by an undecorated C name — a C++
// namespace function is not.
//
// Each function below is a forwarding shim.  Where a return value has to be
// translated between the native and the managed convention the conversion is
// called out inline, because those are the places where a silent bug would
// still compile and still "work" for the easy cases.

#include "synchronization_stubs.h"

#include "parallel.h"                // chaos_parallel_for_range_int
#include "synchronization.h"         // SemaphoreSlim / RWLock / Barrier / CountdownEvent
#include "thread_pool.h"             // ThreadPoolInitialize / queue / worker count
#include "timer_queue.h"             // TimerQueue*
#include "wait_handle.h"             // WaitHandle*

#include <cstdint>

// NOTE ON LINKAGE OF THE DECLARATIONS BELOW
// -----------------------------------------
// synchronization.h / wait_handle.h / thread_pool.h / timer_queue.h declare
// their functions inside `namespace chaos::il2cpp::runtime_core::threading`.
// Pulling the namespace in with a using-directive keeps the forwarding bodies
// readable without introducing any name that is itself exported — the only
// exported names in this TU are the extern "C" definitions further down, and a
// using-directive cannot change their linkage.
using namespace chaos::il2cpp::runtime_core::threading;

namespace {

// ── Handle translation ─────────────────────────────────────────────────
//
// The native layer hands out 32-bit slot ids with 0 reserved as "invalid".  The
// managed side sees a CHAOS_IL2CPP_INTPTR.  Zero maps to zero in both
// directions, which is what makes "creation failed" survivable: the managed
// object holds 0 and every subsequent call fails the handle lookup rather than
// dereferencing anything.
//
// Widening a valid id is lossless, and narrowing is guarded — an id that does
// not fit in 32 bits cannot have come from the native table, so rejecting it as
// invalid is the honest answer rather than truncating into a DIFFERENT live
// slot's id.

inline CHAOS_IL2CPP_INTPTR ToHandle(uint32_t id) noexcept {
    return static_cast<CHAOS_IL2CPP_INTPTR>(id);
}

inline uint32_t FromHandle(CHAOS_IL2CPP_INTPTR handle) noexcept {
    if (handle < 0) return kInvalidWaitHandle;          // sentinel: not an id
    if (handle > 0xFFFFFFFFLL) return kInvalidWaitHandle;  // cannot be a slot id
    return static_cast<uint32_t>(handle);
}

/// Translate a managed timeout (INT32 ms) to the native convention.
///
/// The two agree on the important values — -1 = infinite, 0 = poll — so this is
/// an identity for every value the managed API can produce.  It exists as a
/// named function so the contract is in one place: any future divergence
/// (e.g. INFINITE being INT32_MIN rather than -1) has exactly one site to fix.
inline int32_t ToTimeoutMs(CHAOS_IL2CPP_INT32 timeout_ms) noexcept {
    return static_cast<int32_t>(timeout_ms);
}

/// Native bool -> managed-visible nonzero/zero.
inline CHAOS_IL2CPP_INT32 FromBool(bool value) noexcept {
    return value ? 1 : 0;
}

}  // anonymous namespace

extern "C" {

// ══════════════════════════════════════════════════════════════════════
// SemaphoreSlim
// ══════════════════════════════════════════════════════════════════════

CHAOS_IL2CPP_INTPTR ChaosSemaphoreSlimCreate(CHAOS_IL2CPP_INT32 initial_count,
                                             CHAOS_IL2CPP_INT32 max_count) noexcept
{
    return ToHandle(SemaphoreSlimCreate(static_cast<int32_t>(initial_count),
                                        static_cast<int32_t>(max_count)));
}

CHAOS_IL2CPP_INT32 ChaosSemaphoreSlimDestroy(CHAOS_IL2CPP_INTPTR sem_id) noexcept
{
    return FromBool(SemaphoreSlimDestroy(FromHandle(sem_id)));
}

CHAOS_IL2CPP_INT32 ChaosSemaphoreSlimWait(CHAOS_IL2CPP_INTPTR sem_id,
                                          CHAOS_IL2CPP_INT32 timeout_ms) noexcept
{
    return static_cast<CHAOS_IL2CPP_INT32>(
        SemaphoreSlimWait(FromHandle(sem_id), ToTimeoutMs(timeout_ms)));
}

CHAOS_IL2CPP_INT32 ChaosSemaphoreSlimRelease(CHAOS_IL2CPP_INTPTR sem_id,
                                             CHAOS_IL2CPP_INT32 count) noexcept
{
    return static_cast<CHAOS_IL2CPP_INT32>(
        SemaphoreSlimRelease(FromHandle(sem_id), static_cast<int32_t>(count)));
}

// ══════════════════════════════════════════════════════════════════════
// ReaderWriterLockSlim
// ══════════════════════════════════════════════════════════════════════

CHAOS_IL2CPP_INTPTR ChaosReaderWriterLockSlimCreate() noexcept
{
    return ToHandle(ReaderWriterLockSlimCreate());
}

CHAOS_IL2CPP_INT32 ChaosReaderWriterLockSlimDestroy(CHAOS_IL2CPP_INTPTR rw_id) noexcept
{
    return FromBool(ReaderWriterLockSlimDestroy(FromHandle(rw_id)));
}

CHAOS_IL2CPP_INT32 ChaosReaderWriterLockSlimEnterRead(CHAOS_IL2CPP_INTPTR rw_id,
                                                      CHAOS_IL2CPP_INT32 timeout_ms) noexcept
{
    return static_cast<CHAOS_IL2CPP_INT32>(
        ReaderWriterLockSlimEnterRead(FromHandle(rw_id), ToTimeoutMs(timeout_ms)));
}

CHAOS_IL2CPP_INT32 ChaosReaderWriterLockSlimExitRead(CHAOS_IL2CPP_INTPTR rw_id) noexcept
{
    return FromBool(ReaderWriterLockSlimExitRead(FromHandle(rw_id)));
}

CHAOS_IL2CPP_INT32 ChaosReaderWriterLockSlimEnterWrite(CHAOS_IL2CPP_INTPTR rw_id,
                                                       CHAOS_IL2CPP_INT32 timeout_ms) noexcept
{
    return static_cast<CHAOS_IL2CPP_INT32>(
        ReaderWriterLockSlimEnterWrite(FromHandle(rw_id), ToTimeoutMs(timeout_ms)));
}

CHAOS_IL2CPP_INT32 ChaosReaderWriterLockSlimExitWrite(CHAOS_IL2CPP_INTPTR rw_id) noexcept
{
    return FromBool(ReaderWriterLockSlimExitWrite(FromHandle(rw_id)));
}

CHAOS_IL2CPP_INT32 ChaosReaderWriterLockSlimEnterUpgradeableRead(
    CHAOS_IL2CPP_INTPTR rw_id, CHAOS_IL2CPP_INT32 timeout_ms) noexcept
{
    return static_cast<CHAOS_IL2CPP_INT32>(
        ReaderWriterLockSlimEnterUpgradeableRead(FromHandle(rw_id), ToTimeoutMs(timeout_ms)));
}

CHAOS_IL2CPP_INT32 ChaosReaderWriterLockSlimExitUpgradeableRead(
    CHAOS_IL2CPP_INTPTR rw_id) noexcept
{
    return FromBool(ReaderWriterLockSlimExitUpgradeableRead(FromHandle(rw_id)));
}

CHAOS_IL2CPP_INT32 ChaosReaderWriterLockSlimUpgradeToWrite(
    CHAOS_IL2CPP_INTPTR rw_id, CHAOS_IL2CPP_INT32 timeout_ms) noexcept
{
    return static_cast<CHAOS_IL2CPP_INT32>(
        ReaderWriterLockSlimUpgradeToWrite(FromHandle(rw_id), ToTimeoutMs(timeout_ms)));
}

CHAOS_IL2CPP_INT32 ChaosReaderWriterLockSlimDowngradeFromWrite(
    CHAOS_IL2CPP_INTPTR rw_id) noexcept
{
    return FromBool(ReaderWriterLockSlimDowngradeFromWrite(FromHandle(rw_id)));
}

// ══════════════════════════════════════════════════════════════════════
// Barrier
// ══════════════════════════════════════════════════════════════════════

CHAOS_IL2CPP_INTPTR ChaosBarrierCreate(CHAOS_IL2CPP_INT32 participant_count) noexcept
{
    return ToHandle(BarrierCreate(static_cast<int32_t>(participant_count)));
}

CHAOS_IL2CPP_INT32 ChaosBarrierDestroy(CHAOS_IL2CPP_INTPTR barrier_id) noexcept
{
    return FromBool(BarrierDestroy(FromHandle(barrier_id)));
}

CHAOS_IL2CPP_INT32 ChaosBarrierSignalAndWait(CHAOS_IL2CPP_INTPTR barrier_id,
                                             CHAOS_IL2CPP_INT32 timeout_ms) noexcept
{
    return static_cast<CHAOS_IL2CPP_INT32>(
        BarrierSignalAndWait(FromHandle(barrier_id), ToTimeoutMs(timeout_ms)));
}

CHAOS_IL2CPP_INT32 ChaosBarrierGetRemainingParticipants(CHAOS_IL2CPP_INTPTR barrier_id) noexcept
{
    return static_cast<CHAOS_IL2CPP_INT32>(
        BarrierGetRemainingParticipants(FromHandle(barrier_id)));
}

CHAOS_IL2CPP_INT64 ChaosBarrierGetCurrentPhaseNumber(CHAOS_IL2CPP_INTPTR barrier_id) noexcept
{
    return static_cast<CHAOS_IL2CPP_INT64>(
        BarrierGetCurrentPhaseNumber(FromHandle(barrier_id)));
}

// ══════════════════════════════════════════════════════════════════════
// CountdownEvent
// ══════════════════════════════════════════════════════════════════════

CHAOS_IL2CPP_INTPTR ChaosCountdownEventCreate(CHAOS_IL2CPP_INT32 initial_count) noexcept
{
    return ToHandle(CountdownEventCreate(static_cast<int32_t>(initial_count)));
}

CHAOS_IL2CPP_INT32 ChaosCountdownEventDestroy(CHAOS_IL2CPP_INTPTR ce_id) noexcept
{
    return FromBool(CountdownEventDestroy(FromHandle(ce_id)));
}

CHAOS_IL2CPP_INT32 ChaosCountdownEventSignal(CHAOS_IL2CPP_INTPTR ce_id,
                                             CHAOS_IL2CPP_INT32 count) noexcept
{
    return static_cast<CHAOS_IL2CPP_INT32>(
        CountdownEventSignal(FromHandle(ce_id), static_cast<int32_t>(count)));
}

CHAOS_IL2CPP_INT32 ChaosCountdownEventWait(CHAOS_IL2CPP_INTPTR ce_id,
                                           CHAOS_IL2CPP_INT32 timeout_ms) noexcept
{
    return static_cast<CHAOS_IL2CPP_INT32>(
        CountdownEventWait(FromHandle(ce_id), ToTimeoutMs(timeout_ms)));
}

CHAOS_IL2CPP_INT32 ChaosCountdownEventGetCurrentCount(CHAOS_IL2CPP_INTPTR ce_id) noexcept
{
    return static_cast<CHAOS_IL2CPP_INT32>(
        CountdownEventGetCurrentCount(FromHandle(ce_id)));
}

CHAOS_IL2CPP_INT32 ChaosCountdownEventReset(CHAOS_IL2CPP_INTPTR ce_id,
                                            CHAOS_IL2CPP_INT32 count) noexcept
{
    return FromBool(CountdownEventReset(FromHandle(ce_id), static_cast<int32_t>(count)));
}

// ══════════════════════════════════════════════════════════════════════
// ManualResetEvent / AutoResetEvent
// ══════════════════════════════════════════════════════════════════════

CHAOS_IL2CPP_INTPTR ChaosWaitHandleCreate(CHAOS_IL2CPP_INT32 initial_state,
                                          CHAOS_IL2CPP_INT32 type) noexcept
{
    // An out-of-range discriminant is REJECTED rather than coerced.  The two
    // types differ only in wake policy (Manual wakes all, Auto wakes one and
    // self-resets), so coercing an unknown value to ManualResetEvent would turn
    // an argument error into a silently different concurrency behaviour — the
    // exact class of bug that is invisible until it deadlocks or misses a wakeup.
    WaitHandleType native_type;
    switch (type) {
        case 0: native_type = WaitHandleType::ManualResetEvent; break;
        case 1: native_type = WaitHandleType::AutoResetEvent;   break;
        default: return 0;
    }
    return ToHandle(WaitHandleCreate(initial_state != 0, native_type));
}

CHAOS_IL2CPP_INT32 ChaosWaitHandleClose(CHAOS_IL2CPP_INTPTR handle_id) noexcept
{
    return FromBool(WaitHandleClose(FromHandle(handle_id)));
}

CHAOS_IL2CPP_INT32 ChaosWaitHandleSet(CHAOS_IL2CPP_INTPTR handle_id) noexcept
{
    return FromBool(WaitHandleSet(FromHandle(handle_id)));
}

CHAOS_IL2CPP_INT32 ChaosWaitHandleReset(CHAOS_IL2CPP_INTPTR handle_id) noexcept
{
    return FromBool(WaitHandleReset(FromHandle(handle_id)));
}

CHAOS_IL2CPP_INT32 ChaosWaitHandleWaitOne(CHAOS_IL2CPP_INTPTR handle_id,
                                          CHAOS_IL2CPP_INT32 timeout_ms) noexcept
{
    return static_cast<CHAOS_IL2CPP_INT32>(
        WaitHandleWaitOne(FromHandle(handle_id), ToTimeoutMs(timeout_ms)));
}

CHAOS_IL2CPP_INT32 ChaosWaitHandleWaitAll(CHAOS_IL2CPP_INTPTR handle_ids,
                                          CHAOS_IL2CPP_INT32 count,
                                          CHAOS_IL2CPP_INT32 timeout_ms) noexcept
{
    // `handle_ids` arrives as a raw pointer from managed code (a pinned/blittable
    // array), NOT as a handle-table id — so it is reinterpreted directly and must
    // NOT go through FromHandle.  count <= 0 is rejected here because the native
    // entry point would otherwise scan a zero-length array and report success,
    // which reads as "all (zero) handles were signalled".
    if (handle_ids == 0 || count <= 0) return -1;
    return static_cast<CHAOS_IL2CPP_INT32>(
        WaitHandleWaitAll(reinterpret_cast<const uint32_t*>(handle_ids),
                          static_cast<uint32_t>(count), ToTimeoutMs(timeout_ms)));
}

CHAOS_IL2CPP_INT32 ChaosWaitHandleWaitAny(CHAOS_IL2CPP_INTPTR handle_ids,
                                          CHAOS_IL2CPP_INT32 count,
                                          CHAOS_IL2CPP_INT32 timeout_ms) noexcept
{
    if (handle_ids == 0 || count <= 0) return -1;
    // WaitAny returns the INDEX of the signalled handle, not a boolean — the
    // value is passed through untouched so index 0 stays distinguishable from
    // "timeout" (-1).  Collapsing it to 0/1 here would lose the index.
    return static_cast<CHAOS_IL2CPP_INT32>(
        WaitHandleWaitAny(reinterpret_cast<const uint32_t*>(handle_ids),
                          static_cast<uint32_t>(count), ToTimeoutMs(timeout_ms)));
}

// ══════════════════════════════════════════════════════════════════════
// Timer
// ══════════════════════════════════════════════════════════════════════

CHAOS_IL2CPP_INTPTR ChaosTimerCreate(CHAOS_IL2CPP_INTPTR callback,
                                     CHAOS_IL2CPP_INTPTR state,
                                     CHAOS_IL2CPP_INT32 due_time_ms,
                                     CHAOS_IL2CPP_INT32 period_ms) noexcept
{
    // A null callback is rejected by TimerQueueCreate itself, so it need not be
    // re-checked here — but the negative-time guard is ours: due_time_ms and
    // period_ms are uint32_t natively, and a negative managed value would wrap to
    // an enormous delay (effectively "never fires") instead of erroring.
    if (due_time_ms < 0 || period_ms < 0) return 0;
    return ToHandle(TimerQueueCreate(
        reinterpret_cast<void (*)(void*)>(callback),
        reinterpret_cast<void*>(state),
        static_cast<uint32_t>(due_time_ms),
        static_cast<uint32_t>(period_ms)));
}

CHAOS_IL2CPP_INT32 ChaosTimerChange(CHAOS_IL2CPP_INTPTR timer_id,
                                    CHAOS_IL2CPP_INT32 due_time_ms,
                                    CHAOS_IL2CPP_INT32 period_ms) noexcept
{
    if (due_time_ms < 0 || period_ms < 0) return 0;
    return FromBool(TimerQueueChange(FromHandle(timer_id),
                                     static_cast<uint32_t>(due_time_ms),
                                     static_cast<uint32_t>(period_ms)));
}

CHAOS_IL2CPP_INT32 ChaosTimerDelete(CHAOS_IL2CPP_INTPTR timer_id) noexcept
{
    return FromBool(TimerQueueDelete(FromHandle(timer_id)));
}

// ══════════════════════════════════════════════════════════════════════
// ThreadPool
// ══════════════════════════════════════════════════════════════════════

CHAOS_IL2CPP_INT32 ChaosThreadPoolQueueUserWorkItem(CHAOS_IL2CPP_INTPTR callback,
                                                    CHAOS_IL2CPP_INTPTR state) noexcept
{
    if (callback == 0) return 0;
    ThreadPoolQueueUserWorkItem(reinterpret_cast<void (*)(void*)>(callback),
                                reinterpret_cast<void*>(state));
    return 1;
}

CHAOS_IL2CPP_INT32 ChaosThreadPoolQueueUserWorkItemUnsafe(CHAOS_IL2CPP_INTPTR callback,
                                                          CHAOS_IL2CPP_INTPTR state) noexcept
{
    if (callback == 0) return 0;
    ThreadPoolQueueUserWorkItemUnsafe(reinterpret_cast<void (*)(void*)>(callback),
                                      reinterpret_cast<void*>(state));
    return 1;
}

CHAOS_IL2CPP_INT32 ChaosThreadPoolGetWorkerCount() noexcept
{
    return static_cast<CHAOS_IL2CPP_INT32>(ThreadPoolWorkerCount());
}

CHAOS_IL2CPP_INT32 ChaosThreadPoolEnsureInitialized() noexcept
{
    // Idempotent in the pool's own implementation, so calling it from a managed
    // static constructor and again from the first QueueUserWorkItem is safe.
    // This exists because queueing onto an UNINITIALIZED pool leaves a worker
    // alive past test teardown — a real defect hit while adding the Post test.
    ThreadPoolInitialize();
    return 1;
}

CHAOS_IL2CPP_INT32 ChaosThreadPoolShutdown() noexcept
{
    ThreadPoolShutdown();
    return 1;
}

}  // extern "C"
