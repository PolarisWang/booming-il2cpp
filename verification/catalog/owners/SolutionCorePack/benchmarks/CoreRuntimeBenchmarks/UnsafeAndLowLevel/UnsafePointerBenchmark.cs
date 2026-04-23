using Chaos.TestFramework;

namespace CoreRuntimeBenchmarks;

internal static class UnsafePointerBenchmarkEntry
{
    [ChaosBenchmark(
        ChaosBenchmarkCategory.RuntimeDispatch,
        ChaosMetric.WallClockUs,
        Alias = "unsafe-pointer-bench",
        CapabilityFamily = ChaosCapabilityFamily.UnsafeAndLowLevel,
        Capability = ChaosCapabilityItem.UnsafePointer,
        Requires = ChaosRuntimeFeature.UnsafePointer,
        Archetype = ChaosSolutionArchetype.CoreLibReferenceSolution,
        Modes = ChaosExecutionMode.Managed,
        WarmupCount = 1,
        IterationCount = 2,
        InvocationCount = 1)]
    public static unsafe int RunWorkload()
    {
        int[] values = new int[128];
        int checksum = 0;

        fixed (int* pointer = values)
        {
            for (int i = 0; i < values.Length; i++)
            {
                pointer[i] = i;
                checksum += pointer[i];
            }
        }

        return checksum % 10000;
    }
}
