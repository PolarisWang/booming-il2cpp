using Chaos.TestFramework;

namespace CoreRuntimeBenchmarks;

internal static class SpanAndMemoryBenchmarkEntry
{
    [ChaosBenchmark(
        ChaosBenchmarkCategory.RuntimeDispatch,
        ChaosMetric.WallClockUs,
        Alias = "span-memory-bench",
        CapabilityFamily = ChaosCapabilityFamily.UnsafeAndLowLevel,
        Capability = ChaosCapabilityItem.SpanAndMemory,
        Requires = ChaosRuntimeFeature.SpanMemory,
        Archetype = ChaosSolutionArchetype.CoreLibReferenceSolution,
        Modes = ChaosExecutionMode.Managed | ChaosExecutionMode.Native,
        WarmupCount = 1,
        IterationCount = 2,
        InvocationCount = 1)]
    public static int RunWorkload()
    {
        int[] values = new int[256];
        Span<int> span = values;

        for (int i = 0; i < span.Length; i++)
        {
            span[i] = i;
        }

        Memory<int> window = values.AsMemory(32, 128);
        int checksum = 0;
        foreach (int value in window.Span)
        {
            checksum += value;
        }

        return checksum % 10000;
    }
}
