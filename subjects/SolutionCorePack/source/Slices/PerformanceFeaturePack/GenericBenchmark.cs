using System.Collections.Generic;
using Chaos.TestFramework;

namespace PerformanceFeaturePack;

internal static class GenericBenchmarkEntry
{
    private const int IterationCount = 1000;

    [ChaosBenchmark(
        ChaosBenchmarkCategory.RuntimeDispatch,
        ChaosMetric.WallClockUs | ChaosMetric.ManagedAllocBytes,
        Alias = "generic-bench",
        Modes = ChaosExecutionMode.Managed,
        Requires = ChaosRuntimeFeature.GenericSharing,
        WarmupCount = 1,
        IterationCount = 2,
        InvocationCount = 1)]
    public static int RunWorkload()
    {
        int checksum = 0;
        for (int iter = 0; iter < IterationCount; iter++)
        {
            var list = new List<int>();
            for (int i = 0; i < 100; i++)
            {
                list.Add(i);
            }

            checksum += list.Count;

            var dict = new Dictionary<string, int>();
            for (int i = 0; i < 20; i++)
            {
                dict["key" + i] = i;
            }

            checksum += dict["key10"];
        }

        return checksum % 10000;
    }
}
