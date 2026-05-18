using Chaos.TestFramework;
using System.Threading;

namespace CoreRuntimeFeatures;

internal static class DelegateConcurrencyProofEntry
{
    private static int s_sharedCounter;

    private static void Handler(string tag)
    {
        Interlocked.Increment(ref s_sharedCounter);
    }

    // D1: Concurrent Combine/Remove — 8 threads racing += and -=
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "delegate-concurrency-proof",
        CapabilityFamily = ChaosCapabilityFamily.DelegatesAndClosures,
        Capability = ChaosCapabilityItem.DelegateChaining,
        Requires = ChaosRuntimeFeature.Delegate,
        Priority = 2)]
    public static int RunConcurrency()
    {
        const int workerCount = 8;
        const int opsPerWorker = 1000;
        var threads = new Thread[workerCount];
        System.Action<string> shared = null!;

        for (int t = 0; t < workerCount; t++)
        {
            int local = t;
            threads[t] = new Thread(() =>
            {
                for (int i = 0; i < opsPerWorker; i++)
                {
                    // Concurrent += and -= (read-modify-write, no lock)
                    System.Action<string> handler = Handler;
                    shared += handler;
                    shared -= handler;
                }
            });
        }

        foreach (var t in threads) t.Start();
        foreach (var t in threads) t.Join();

        // After all racy +=/-=, the shared delegate must not be null
        Assert.NotNull(shared, "shared delegate should not be null after concurrent ops");

        // Invoke to verify no corruption — the exact count is non-deterministic
        // due to races, but the invoke must not crash.
        s_sharedCounter = 0;
        if (shared != null)
        {
            shared("final");
        }

        return 0;
    }
}
