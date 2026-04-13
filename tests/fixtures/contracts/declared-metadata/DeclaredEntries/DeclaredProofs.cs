using Chaos.TestFramework;

namespace DeclaredEntries;

public static class DeclaredProofs
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "runtime-contract",
        Requires = ChaosRuntimeFeature.Reflection,
        Evidence = ChaosEvidenceKind.Stdout,
        Priority = 3)]
    public static int VerifyOutput()
    {
        return 42;
    }

    [ChaosBenchmark(
        ChaosBenchmarkCategory.RuntimeDispatch,
        ChaosMetric.WallClockUs | ChaosMetric.ManagedAllocBytes,
        Alias = "dispatch-bench",
        Requires = ChaosRuntimeFeature.GenericSharing,
        WarmupCount = 2,
        IterationCount = 12,
        InvocationCount = 100)]
    public static int BenchmarkDispatch()
    {
        return 7;
    }
}
