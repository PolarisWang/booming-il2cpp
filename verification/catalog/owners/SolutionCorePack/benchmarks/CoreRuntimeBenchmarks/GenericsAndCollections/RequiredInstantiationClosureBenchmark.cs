using Chaos.TestFramework;

namespace CoreRuntimeBenchmarks;

internal static class RequiredInstantiationClosureBenchmarkEntry
{
    private static T Tail<T>(IReadOnlyList<T> values)
    {
        return values[values.Count - 1];
    }

    [ChaosBenchmark(
        ChaosBenchmarkCategory.RuntimeDispatch,
        ChaosMetric.WallClockUs,
        Alias = "required-instantiation-closure-bench",
        CapabilityFamily = ChaosCapabilityFamily.LinkerAndAotClosure,
        Capability = ChaosCapabilityItem.RequiredInstantiationClosure,
        Requires = ChaosRuntimeFeature.GenericSharing,
        Archetype = ChaosSolutionArchetype.MixedReferenceClosureSolution,
        Modes = ChaosExecutionMode.Managed | ChaosExecutionMode.Native,
        WarmupCount = 1,
        IterationCount = 2,
        InvocationCount = 1)]
    public static int RunWorkload()
    {
        int checksum = 0;
        for (int i = 0; i < 128; i++)
        {
            checksum += Tail(new[] { i, i + 1, i + 2 });
            checksum += Tail(new[] { "a", "bb", "ccc" }).Length;
        }

        return checksum % 10000;
    }
}
