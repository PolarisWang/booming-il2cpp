using Chaos.TestFramework;

namespace CoreRuntimeBenchmarks;

internal interface IGenericBenchmarkValue<out T>
{
    T Value { get; }
}

internal readonly struct GenericBenchmarkValue<T> : IGenericBenchmarkValue<T>
{
    public GenericBenchmarkValue(T value)
    {
        Value = value;
    }

    public T Value { get; }
}

internal static class GenericSharingBoundaryBenchmarkEntry
{
    [ChaosBenchmark(
        ChaosBenchmarkCategory.RuntimeDispatch,
        ChaosMetric.WallClockUs,
        Alias = "generic-sharing-boundary-bench",
        CapabilityFamily = ChaosCapabilityFamily.LinkerAndAotClosure,
        Capability = ChaosCapabilityItem.GenericSharingBoundary,
        Requires = ChaosRuntimeFeature.GenericSharing | ChaosRuntimeFeature.GenericVirtualDispatch,
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
            IGenericBenchmarkValue<int> integerValue = new GenericBenchmarkValue<int>(i);
            IGenericBenchmarkValue<string> stringValue = new GenericBenchmarkValue<string>("v" + i);
            checksum += integerValue.Value + stringValue.Value.Length;
        }

        return checksum % 10000;
    }
}
