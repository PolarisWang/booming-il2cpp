using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal static class MonitorAndLockingProofEntry
{
    private static readonly object Gate = new();
    private static int SharedTotal;

    private static void Increment(int iterations)
    {
        for (int i = 0; i < iterations; i++)
        {
            lock (Gate)
            {
                SharedTotal += 1;
            }
        }
    }

    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "monitor-locking-proof",
        CapabilityFamily = ChaosCapabilityFamily.ThreadingAndSynchronization,
        Capability = ChaosCapabilityItem.MonitorAndLocking,
        Requires = ChaosRuntimeFeature.Synchronization,
        Priority = 4)]
    public static int Run()
    {
        SharedTotal = 0;

        var first = new Thread(() => Increment(120));
        var second = new Thread(() => Increment(80));
        first.Start();
        second.Start();
        Increment(100);
        first.Join();
        second.Join();

        bool entered = false;
        try
        {
            Monitor.TryEnter(Gate, TimeSpan.FromMilliseconds(50), ref entered);
            Assert.True(entered);
            Assert.Equal(300, SharedTotal);
        }
        finally
        {
            if (entered)
            {
                Monitor.Exit(Gate);
            }
        }

        return 0;
    }
}
