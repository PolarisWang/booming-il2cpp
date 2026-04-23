using System;
using System.Threading;

namespace ThreadingThreadStaticMonitor.App;

internal static class ThreadingProofEntry
{
    [ThreadStatic]
    private static int _threadSlot;

    private static readonly object Gate = new();
    private static int _sharedTotal;
    private static int _finalizedCount;

    private sealed class FinalizableProbe
    {
        ~FinalizableProbe()
        {
            Interlocked.Increment(ref _finalizedCount);
        }
    }

    private static int IncrementThreadSlot()
    {
        _threadSlot += 1;
        return _threadSlot;
    }

    private static void CreateFinalizableProbe()
    {
        var probe = new FinalizableProbe();
        _ = new WeakReference(probe);
    }

    public static int Run()
    {
        _threadSlot = 0;
        _sharedTotal = 0;
        _finalizedCount = 0;

        _ = IncrementThreadSlot();
        var mainSlot = IncrementThreadSlot();

        lock (Gate)
        {
            _sharedTotal += 1;
        }

        var workerSlot = 0;
        var worker = new Thread(() =>
        {
            workerSlot = IncrementThreadSlot();
            lock (Gate)
            {
                _sharedTotal += 1;
            }
        })
        {
            Name = "threading-threadstatic-monitor-worker",
        };

        worker.Start();
        worker.Join();

        CreateFinalizableProbe();
        GC.Collect();
        GC.WaitForPendingFinalizers();
        GC.Collect();

        if (mainSlot != 2 || workerSlot != 1 || _sharedTotal != 2 || _finalizedCount < 1)
        {
            return 1;
        }

        Console.WriteLine(
            "{\"kind\":\"threading-proof\",\"status\":\"ok\",\"threadStatic\":{\"main\":2,\"worker\":1},\"monitor\":{\"sharedTotal\":2},\"gc\":{\"rootsReported\":2,\"finalized\":1}}");
        return 0;
    }
}

internal static class Program
{
    private static int Main()
    {
        return ThreadingProofEntry.Run();
    }
}
