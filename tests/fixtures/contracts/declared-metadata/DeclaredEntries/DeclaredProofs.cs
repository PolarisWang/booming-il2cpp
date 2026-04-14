using Chaos.TestFramework;

namespace DeclaredEntries;

public static class DeclaredProofs
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "runtime-contract",
        CapabilityFamily = ChaosCapabilityFamily.ReflectionAndMetadata,
        Capability = ChaosCapabilityItem.MetadataSupplement,
        Requires = ChaosRuntimeFeature.Reflection,
        Archetype = ChaosSolutionArchetype.ReferenceAssemblySolution,
        HotUpdateCapability = ChaosHotUpdateCapability.MetadataSupplement,
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
        CapabilityFamily = ChaosCapabilityFamily.TypeSystemAndGenerics,
        Capability = ChaosCapabilityItem.GenericVirtualDispatch,
        Requires = ChaosRuntimeFeature.GenericSharing,
        Archetype = ChaosSolutionArchetype.FullProjectHotUpdateSolution,
        HotUpdateCapability = ChaosHotUpdateCapability.PackageLoad | ChaosHotUpdateCapability.SharedContractBinding,
        WarmupCount = 2,
        IterationCount = 12,
        InvocationCount = 100)]
    public static int BenchmarkDispatch()
    {
        return 7;
    }
}
