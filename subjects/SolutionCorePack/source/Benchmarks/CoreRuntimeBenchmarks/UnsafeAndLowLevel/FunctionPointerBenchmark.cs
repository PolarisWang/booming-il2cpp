using Chaos.TestFramework;

namespace CoreRuntimeBenchmarks;

internal static class FunctionPointerBenchmarkEntry
{
    private static int MultiplyAndAccumulate(int left, int right)
    {
        return left * right;
    }

    [ChaosBenchmark(
        ChaosBenchmarkCategory.RuntimeDispatch,
        ChaosMetric.WallClockUs,
        Alias = "function-pointer-bench",
        CapabilityFamily = ChaosCapabilityFamily.UnsafeAndLowLevel,
        Capability = ChaosCapabilityItem.FunctionPointer,
        Requires = ChaosRuntimeFeature.FunctionPointer,
        Archetype = ChaosSolutionArchetype.CoreLibReferenceSolution,
        Modes = ChaosExecutionMode.Managed | ChaosExecutionMode.Native,
        WarmupCount = 1,
        IterationCount = 2,
        InvocationCount = 1)]
    public static unsafe int RunWorkload()
    {
        delegate* managed<int, int, int> pointer = &MultiplyAndAccumulate;
        int checksum = 0;

        for (int i = 1; i <= 64; i++)
        {
            checksum += pointer(i, 2);
        }

        return checksum % 10000;
    }
}
