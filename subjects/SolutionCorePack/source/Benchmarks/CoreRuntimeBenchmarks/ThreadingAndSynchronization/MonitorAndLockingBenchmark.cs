using Chaos.TestFramework;

namespace CoreRuntimeBenchmarks;

internal static class MonitorAndLockingBenchmarkEntry
{
    private static readonly object Gate = new();

    [ChaosBenchmark(
        ChaosBenchmarkCategory.RuntimeDispatch,
        ChaosMetric.WallClockUs,
        Alias = "monitor-locking-bench",
        CapabilityFamily = ChaosCapabilityFamily.ThreadingAndSynchronization,
        Capability = ChaosCapabilityItem.MonitorAndLocking,
        Modes = ChaosExecutionMode.Managed,
        Requires = ChaosRuntimeFeature.Synchronization,
        WarmupCount = 1,
        IterationCount = 3,
        InvocationCount = 1)]
    public static int RunWorkload()
    {
        int checksum = 0;
        for (int i = 0; i < 10_000; i++)
        {
            lock (Gate)
            {
                checksum += i & 7;
            }
        }

        return checksum % 10000;
    }
}
