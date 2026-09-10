/// threadpool_events_smoke_test.cpp — Smoke test for ThreadPool event provider.
///
/// Fires each event type and verifies no crash.  On Windows the events go
/// to ETW; on non-Windows they go to the in-memory ring buffer.
/// Also tests ring-buffer dump (non-Windows) and provider init/shutdown.

#include <gtest/gtest.h>

#include <runtime_stubs/threadpool_events.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <thread>

namespace threading = chaos::il2cpp::runtime_core::threading;

TEST(ThreadPoolEventsSmoke, ProviderInitShutdown)
{
    // Double init is safe (idempotent).
    threading::ThreadPoolEventProviderInitialize();
    threading::ThreadPoolEventProviderInitialize();

    // Double shutdown is safe.
    threading::ThreadPoolEventProviderShutdown();
    threading::ThreadPoolEventProviderShutdown();

    // Re-init after shutdown.
    threading::ThreadPoolEventProviderInitialize();
    threading::ThreadPoolEventProviderShutdown();
}

TEST(ThreadPoolEventsSmoke, FireWorkerEvents)
{
    threading::ThreadPoolEventProviderInitialize();

    threading::ThreadPoolEventEmitWorkerCreate(42);
    threading::ThreadPoolEventEmitWorkerDestroy(42);

    threading::ThreadPoolEventProviderShutdown();
}

TEST(ThreadPoolEventsSmoke, FireWorkItemEvents)
{
    threading::ThreadPoolEventProviderInitialize();

    threading::ThreadPoolEventEmitWorkItemQueue(0xDEADBEEF);
    threading::ThreadPoolEventEmitWorkItemDequeue(0xDEADBEEF);

    threading::ThreadPoolEventProviderShutdown();
}

TEST(ThreadPoolEventsSmoke, FireWorkerAdjust)
{
    threading::ThreadPoolEventProviderInitialize();

    threading::ThreadPoolEventEmitWorkerAdjust(4, 6, 2);  // Stabilizing
    threading::ThreadPoolEventEmitWorkerAdjust(6, 4, 4);  // WorkloadSurge

    threading::ThreadPoolEventProviderShutdown();
}

TEST(ThreadPoolEventsSmoke, FireIOCompletion)
{
    threading::ThreadPoolEventProviderInitialize();

    threading::ThreadPoolEventEmitIOCompletion(4096);
    threading::ThreadPoolEventEmitIOCompletion(0);

    threading::ThreadPoolEventProviderShutdown();
}

TEST(ThreadPoolEventsSmoke, FireThreadLifecycle)
{
    threading::ThreadPoolEventProviderInitialize();

    threading::ThreadPoolEventEmitThreadCreate(100);
    threading::ThreadPoolEventEmitThreadAttach(100);
    threading::ThreadPoolEventEmitThreadDetach(100);

    threading::ThreadPoolEventProviderShutdown();
}

TEST(ThreadPoolEventsSmoke, FireSafepointEvents)
{
    threading::ThreadPoolEventProviderInitialize();

    threading::ThreadPoolEventEmitSafepointBegin(1, 5000);   // epoch=1, 5s timeout
    threading::ThreadPoolEventEmitSafepointEnd(1, 2500000);  // 2.5ms pause
    threading::ThreadPoolEventEmitSafepointBegin(2, 0);      // no timeout
    threading::ThreadPoolEventEmitSafepointEnd(2, 1000000);  // 1ms pause

    threading::ThreadPoolEventProviderShutdown();
}

TEST(ThreadPoolEventsSmoke, FireMonitorContention)
{
    threading::ThreadPoolEventProviderInitialize();

    int dummy_object = 0;
    threading::ThreadPoolEventEmitMonitorContention(
        reinterpret_cast<int64_t>(&dummy_object), 0);   // first attempt
    threading::ThreadPoolEventEmitMonitorContention(
        reinterpret_cast<int64_t>(&dummy_object), 150);  // 150ms wait

    threading::ThreadPoolEventProviderShutdown();
}

TEST(ThreadPoolEventsSmoke, FireAllEventsFromMultipleThreads)
{
    threading::ThreadPoolEventProviderInitialize();

    std::thread t1([] {
        for (int i = 0; i < 10; i++) {
            threading::ThreadPoolEventEmitWorkerCreate(i);
            threading::ThreadPoolEventEmitThreadCreate(i + 100);
            threading::ThreadPoolEventEmitWorkerAdjust(i, i + 1, 0);
        }
    });

    std::thread t2([] {
        for (int i = 0; i < 10; i++) {
            threading::ThreadPoolEventEmitWorkerDestroy(i);
            threading::ThreadPoolEventEmitThreadDetach(i + 100);
            threading::ThreadPoolEventEmitIOCompletion(static_cast<uint32_t>(i * 512));
        }
    });

    std::thread t3([] {
        for (int i = 0; i < 10; i++) {
            threading::ThreadPoolEventEmitSafepointBegin(
                static_cast<uint64_t>(i), 1000);
            threading::ThreadPoolEventEmitSafepointEnd(
                static_cast<uint64_t>(i), 500000);
            threading::ThreadPoolEventEmitMonitorContention(
                reinterpret_cast<int64_t>(&i), static_cast<uint32_t>(i * 10));
        }
    });

    t1.join();
    t2.join();
    t3.join();

    threading::ThreadPoolEventProviderShutdown();
}

TEST(ThreadPoolEventsSmoke, RingBufferDumpNoCrash)
{
    // Fire some events to fill the ring buffer, then dump.
    // On Windows the dump is a no-op; on non-Windows it writes CSV.
    threading::ThreadPoolEventProviderInitialize();

    for (int i = 0; i < 50; i++) {
        threading::ThreadPoolEventEmitWorkerCreate(i);
        threading::ThreadPoolEventEmitWorkItemQueue(i * 1000);
    }

    // Dump to a temp file (no-op on Windows, writes CSV on Linux/macOS).
    threading::ThreadPoolEventDumpRingBuffer("/tmp/threadpool_events_dump.csv");

    threading::ThreadPoolEventProviderShutdown();
}
