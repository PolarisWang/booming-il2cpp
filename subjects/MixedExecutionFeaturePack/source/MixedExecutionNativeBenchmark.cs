using Chaos.TestFramework;

namespace MixedExecutionFeaturePack;

internal static class MixedExecutionNativeBenchmarkEntry
{
    [ChaosBenchmark(
        ChaosBenchmarkCategory.RuntimeDispatch,
        ChaosMetric.WallClockUs,
        Alias = "mixed-execution-native-bench",
        WarmupCount = 1,
        IterationCount = 6,
        InvocationCount = 1000)]
    public static int RunWorkload()
    {
        int seed = 17;
        int aotStage = seed * seed + seed;
        int interpreterShapeStage = aotStage * 2 + 1;
        int aotReentryStage = interpreterShapeStage * interpreterShapeStage + interpreterShapeStage;
        int checksum = aotReentryStage % 10000;
        return checksum;
    }
}
