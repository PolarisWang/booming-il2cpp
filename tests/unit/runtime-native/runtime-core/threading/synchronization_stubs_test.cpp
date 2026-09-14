// t2_0_abi_export_test.cpp — T2.0 exit criterion.
//
// WHAT THIS PINS
// --------------
// T2.0's stated exit is "ABI 符号可被 codegen 生成的 C++ 调用" — the ABI symbols
// are callable from codegen-generated C++.  That is a LINKAGE property, not a
// behaviour property, so this test is built to fail the way a generated TU
// would fail:
//
//   1. It includes ONLY the public ABI header, exactly like a generated TU.
//      It does not include synchronization.h / wait_handle.h / thread_pool.h /
//      timer_queue.h.  Everything it can see comes from synchronization_stubs.h.
//
//   2. Every call goes through an unqualified identifier declared inside that
//      header's extern "C" block — the same shape the ShapeRegistry emits:
//          return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(<symbol>(args...));
//
// If a declaration drifted outside the extern "C" block, or a signature disagreed
// with the definition, this file would fail to compile or link.  That is the
// failure mode threading_stubs.h documents, and it is silent at compile time —
// so a link-level test is the only thing that actually catches it.
//
// The assertions below are deliberately shallow (they check the shim forwards
// and that return conventions survive), because the BEHAVIOUR of these
// primitives is already covered by synchronization_test / wait_handle_test.  The
// property under test here is that the ABI layer is reachable and honest about
// its return values.

#include "runtime_stubs/synchronization_stubs.h"

#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <thread>

namespace {

// No `using namespace` here — and that is the point.  The ABI header declares
// every symbol at GLOBAL scope inside extern "C"; it introduces no namespace.
// A generated TU sees exactly this, so referencing a namespace here would mean
// the test is exercising a path codegen does not have.
//
// Handle discriminants, from the header contract (see ChaosWaitHandleCreate).
constexpr CHAOS_IL2CPP_INT32 kManualResetEvent = 0;
constexpr CHAOS_IL2CPP_INT32 kAutoResetEvent   = 1;

}  // namespace

// ── SemaphoreSlim: create -> wait -> release -> destroy ────────────────
TEST(SynchronizationAbi, SemaphoreSlimRoundTripsThroughTheCAbi)
{
    const CHAOS_IL2CPP_INTPTR sem = ChaosSemaphoreSlimCreate(1, 2);
    ASSERT_NE(sem, 0) << "creation must return a usable handle";

    // Initial count 1: the first wait consumes it without blocking.
    EXPECT_EQ(ChaosSemaphoreSlimWait(sem, 0), 1)
        << "a wait with budget must acquire the available count";
    // Now exhausted: a poll must time out rather than block or succeed.
    EXPECT_EQ(ChaosSemaphoreSlimWait(sem, 0), 0)
        << "polling an exhausted semaphore must report timeout (0), not success";

    EXPECT_EQ(ChaosSemaphoreSlimRelease(sem, 1), 0) << "release succeeds";
    EXPECT_EQ(ChaosSemaphoreSlimWait(sem, 0), 1) << "the released count is acquirable";

    EXPECT_NE(ChaosSemaphoreSlimDestroy(sem), 0) << "destroy finds the handle";
    // A released handle must not stay usable — this is the shim's FromHandle
    // path being exercised, not just a happy path.
    EXPECT_EQ(ChaosSemaphoreSlimWait(sem, 0), -1)
        << "a destroyed handle must report error (-1)";
}

// ── ReaderWriterLockSlim: the upgradeable path, which is why this lock
//    cannot be modelled as a plain mutex ────────────────────────────────
TEST(SynchronizationAbi, ReaderWriterLockSlimSupportsUpgradeableRead)
{
    const CHAOS_IL2CPP_INTPTR rw = ChaosReaderWriterLockSlimCreate();
    ASSERT_NE(rw, 0);

    EXPECT_EQ(ChaosReaderWriterLockSlimEnterUpgradeableRead(rw, 0), 1);
    // A plain reader may coexist with the single upgradeable holder.
    EXPECT_EQ(ChaosReaderWriterLockSlimEnterRead(rw, 0), 1);
    EXPECT_NE(ChaosReaderWriterLockSlimExitRead(rw), 0);

    EXPECT_EQ(ChaosReaderWriterLockSlimUpgradeToWrite(rw, 0), 1)
        << "upgrading from upgradeable-read must succeed once readers drain";
    EXPECT_EQ(ChaosReaderWriterLockSlimEnterRead(rw, 0), 0)
        << "readers must NOT be able to enter while the writer holds the lock";
    EXPECT_NE(ChaosReaderWriterLockSlimDowngradeFromWrite(rw), 0)
        << "downgrade leaves the upgradeable-read lock held";
    EXPECT_EQ(ChaosReaderWriterLockSlimEnterRead(rw, 0), 1)
        << "readers may enter again after the writer downgrades";

    EXPECT_NE(ChaosReaderWriterLockSlimExitRead(rw), 0);
    EXPECT_NE(ChaosReaderWriterLockSlimExitUpgradeableRead(rw), 0);
    EXPECT_NE(ChaosReaderWriterLockSlimDestroy(rw), 0);
}

// ── Barrier: the phase counter is INT64 in the ABI but int32 natively ──
//
// This is the one return conversion with a real width change, so it is worth a
// direct assertion that the wide value survives the shim.
TEST(SynchronizationAbi, BarrierPhaseNumberSurvivesTheInt64Widening)
{
    constexpr CHAOS_IL2CPP_INT32 kParticipants = 2;
    const CHAOS_IL2CPP_INTPTR barrier = ChaosBarrierCreate(kParticipants);
    ASSERT_NE(barrier, 0);

    EXPECT_EQ(ChaosBarrierGetCurrentPhaseNumber(barrier), 0)
        << "a fresh barrier is at phase 0";

    // One participant arrives and waits; it must not be released early.
    std::atomic<CHAOS_IL2CPP_INT32> first_result{-99};
    std::thread first([&] {
        first_result.store(ChaosBarrierSignalAndWait(barrier, -1), std::memory_order_release);
    });

    // Let the first thread park, then complete the phase from here.
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_EQ(ChaosBarrierSignalAndWait(barrier, -1), 1)
        << "the last arrival completes the phase";
    first.join();

    EXPECT_EQ(first_result.load(std::memory_order_acquire), 1)
        << "the parked participant must be released when the phase completes";
    EXPECT_EQ(ChaosBarrierGetCurrentPhaseNumber(barrier), 1)
        << "the phase must have advanced exactly once";
    EXPECT_EQ(ChaosBarrierGetRemainingParticipants(barrier), kParticipants)
        << "a new phase starts with all participants unaccounted for";

    EXPECT_NE(ChaosBarrierDestroy(barrier), 0);
}

// ── CountdownEvent: signal must distinguish "reached zero" from "not yet" ──
TEST(SynchronizationAbi, CountdownEventReportsReachingZeroDistinctly)
{
    const CHAOS_IL2CPP_INTPTR ce = ChaosCountdownEventCreate(2);
    ASSERT_NE(ce, 0);

    EXPECT_EQ(ChaosCountdownEventGetCurrentCount(ce), 2);
    EXPECT_EQ(ChaosCountdownEventSignal(ce, 1), 0)
        << "signalling without reaching zero must report 0, NOT 1 — collapsing "
           "the two would make a waiter unable to tell completion from progress";
    EXPECT_EQ(ChaosCountdownEventGetCurrentCount(ce), 1);
    EXPECT_EQ(ChaosCountdownEventSignal(ce, 1), 1) << "the second signal reaches zero";
    EXPECT_EQ(ChaosCountdownEventWait(ce, 0), 1) << "a zeroed event is signalled";

    EXPECT_EQ(ChaosCountdownEventReset(ce, 3), 1) << "re-arm";
    EXPECT_EQ(ChaosCountdownEventGetCurrentCount(ce), 3);
    EXPECT_EQ(ChaosCountdownEventWait(ce, 0), 0) << "re-armed means not signalled";

    EXPECT_NE(ChaosCountdownEventDestroy(ce), 0);
}

// ── WaitHandle: the type discriminant must be REJECTED when unknown ────
//
// The two handle types differ only in wake policy, so coercing a bad
// discriminant to a default would turn an argument error into silently
// different concurrency behaviour.
TEST(SynchronizationAbi, WaitHandleRejectsAnUnknownTypeDiscriminant)
{
    EXPECT_EQ(ChaosWaitHandleCreate(0, 99), 0)
        << "an out-of-range type must be rejected, not coerced to a default "
           "(that would silently change Manual-vs-Auto wake policy)";
    EXPECT_EQ(ChaosWaitHandleCreate(0, -1), 0);
}

TEST(SynchronizationAbi, WaitHandleDistinguishesManualFromAutoWakePolicy)
{
    // Manual: stays signalled, so a second wait also succeeds.
    const CHAOS_IL2CPP_INTPTR manual = ChaosWaitHandleCreate(0, kManualResetEvent);
    ASSERT_NE(manual, 0);
    EXPECT_EQ(ChaosWaitHandleWaitOne(manual, 0), 0) << "starts non-signalled";
    EXPECT_NE(ChaosWaitHandleSet(manual), 0);
    EXPECT_EQ(ChaosWaitHandleWaitOne(manual, 0), 1);
    EXPECT_EQ(ChaosWaitHandleWaitOne(manual, 0), 1)
        << "a manual-reset event must remain signalled across waits";
    EXPECT_NE(ChaosWaitHandleReset(manual), 0);
    EXPECT_EQ(ChaosWaitHandleWaitOne(manual, 0), 0) << "reset clears it";
    EXPECT_NE(ChaosWaitHandleClose(manual), 0);

    // Auto: self-resets, so the second wait must NOT succeed.
    const CHAOS_IL2CPP_INTPTR auto_reset = ChaosWaitHandleCreate(0, kAutoResetEvent);
    ASSERT_NE(auto_reset, 0);
    EXPECT_NE(ChaosWaitHandleSet(auto_reset), 0);
    EXPECT_EQ(ChaosWaitHandleWaitOne(auto_reset, 0), 1);
    EXPECT_EQ(ChaosWaitHandleWaitOne(auto_reset, 0), 0)
        << "an auto-reset event must clear itself on the first successful wait";
    EXPECT_NE(ChaosWaitHandleClose(auto_reset), 0);
}

// ── WaitAny returns an INDEX, not a boolean ────────────────────────────
//
// Index 0 and "signalled" must not be conflated with timeout (-1), and an
// out-of-range count must be rejected rather than scanning nothing and
// reporting success.
TEST(SynchronizationAbi, WaitHandleWaitAnyReturnsTheIndexNotABoolean)
{
    const CHAOS_IL2CPP_INTPTR h0 = ChaosWaitHandleCreate(0, kManualResetEvent);
    const CHAOS_IL2CPP_INTPTR h1 = ChaosWaitHandleCreate(1, kManualResetEvent);
    ASSERT_NE(h0, 0);
    ASSERT_NE(h1, 0);

    // The id array is a raw pointer (a pinned managed array), NOT a handle-table
    // id — so FromHandle must NOT be applied to it.
    uint32_t ids[2] = {static_cast<uint32_t>(h0), static_cast<uint32_t>(h1)};
    const CHAOS_IL2CPP_INTPTR ids_ptr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(ids);

    EXPECT_EQ(ChaosWaitHandleWaitAny(ids_ptr, 2, 0), 1)
        << "only index 1 is signalled, so WaitAny must return 1 — returning a "
           "boolean 1 here would be indistinguishable from a correct index";

    EXPECT_NE(ChaosWaitHandleSet(h0), 0);
    EXPECT_EQ(ChaosWaitHandleWaitAny(ids_ptr, 2, 0), 0) << "now index 0 wins";

    // WaitAll uses the INVERTED convention: 0 = all signalled, -1 = timeout.
    // This differs from WaitOne above (1 = signalled) and from WaitAny (an
    // index).  Asserting the exact value both ways is the point — a caller that
    // assumed the WaitOne convention would read a timed-out WaitAll as success.
    EXPECT_EQ(ChaosWaitHandleWaitAll(ids_ptr, 2, 0), 0)
        << "WaitAll reports SUCCESS as 0, not 1 — both handles are signalled here";
    EXPECT_NE(ChaosWaitHandleReset(h0), 0);
    EXPECT_EQ(ChaosWaitHandleWaitAll(ids_ptr, 2, 0), -1)
        << "with one handle reset, WaitAll must report timeout/error (-1)";

    // Degenerate counts are rejected, not treated as vacuous success.
    EXPECT_EQ(ChaosWaitHandleWaitAny(ids_ptr, 0, 0), -1) << "count 0 must error";
    EXPECT_EQ(ChaosWaitHandleWaitAll(ids_ptr, 0, 0), -1) << "count 0 must error";
    EXPECT_EQ(ChaosWaitHandleWaitAny(0, 2, 0), -1) << "null array must error";

    EXPECT_NE(ChaosWaitHandleClose(h0), 0);
    EXPECT_NE(ChaosWaitHandleClose(h1), 0);
}

// ── ThreadPool: worker count is an INT32 and the pool self-initializes ──
//
// The shutdown at the end is NOT incidental.  A started pool owns live threads;
// a process that leaves one running can exit abnormally (observed: exit code 3
// with every test reporting PASSED), which ctest records as a failure.  So the
// ABI exposes an explicit shutdown and this test exercises it — otherwise this
// file would pass under gtest and still be reported red by ctest.
TEST(SynchronizationAbi, ThreadPoolExportsAreReachableAndGuardNullCallbacks)
{
    EXPECT_NE(ChaosThreadPoolEnsureInitialized(), 0);
    EXPECT_GE(ChaosThreadPoolGetWorkerCount(), 0);

    // A null callback must be refused.  Queueing it would crash on a worker
    // thread instead of returning an error the caller can act on.
    EXPECT_EQ(ChaosThreadPoolQueueUserWorkItem(0, 0), 0);
    EXPECT_EQ(ChaosThreadPoolQueueUserWorkItemUnsafe(0, 0), 0);

    EXPECT_NE(ChaosThreadPoolShutdown(), 0) << "the pool must be closable";
}

// ── Timer: negative delays are rejected rather than wrapping ───────────
//
// due_time_ms/period_ms are uint32_t natively.  A negative managed value would
// wrap to an enormous delay — i.e. a timer that silently never fires.
TEST(SynchronizationAbi, TimerRejectsNegativeDelaysInsteadOfWrapping)
{
    EXPECT_EQ(ChaosTimerCreate(0, 0, -1, 0), 0)
        << "a negative due time must be rejected, not wrapped to ~49 days";
    EXPECT_EQ(ChaosTimerCreate(0, 0, 0, -5), 0)
        << "a negative period must be rejected rather than wrapping";

    // Deleting an unknown id is reported, not silently treated as success.
    EXPECT_EQ(ChaosTimerDelete(999999), 0);
}
