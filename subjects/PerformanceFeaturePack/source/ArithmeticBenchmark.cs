using Chaos.TestFramework;

namespace PerformanceFeaturePack;

internal static class ArithmeticBenchmarkEntry
{
    private const int IterationCount = 5_000_000;

    [ChaosBenchmark(
        ChaosBenchmarkCategory.RuntimeDispatch,
        ChaosMetric.WallClockUs,
        Alias = "arithmetic-bench",
        WarmupCount = 1,
        IterationCount = 2,
        InvocationCount = 1)]
    public static int RunWorkload()
    {
        int result = 0;
        for (int i = 0; i < IterationCount; i++)
        {
            result += i * 3 - i / 2 + i % 7;
            result -= i % 5;
        }

        return result;
    }
}
