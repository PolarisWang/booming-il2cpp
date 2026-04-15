using Chaos.IL2CPP.Contracts;
using Chaos.IL2CPP.HotUpdate;
using Chaos.TestFramework;

namespace HotUpdateHostPack;

internal static class HotUpdateDispatchBenchmarkEntry
{
    private const int IterationCount = 1000;
    private static readonly ManagedMethodIdentityArtifact BenchmarkIdentity =
        ManagedMethodIdentityResolver.Create(
            "BenchHotUpdateDispatch/HotPatch::AddOne(System.Int32)",
            "System.Int32 HotPatch::AddOne(System.Int32)");

    [ChaosBenchmark(
        ChaosBenchmarkCategory.HotUpdate,
        ChaosMetric.WallClockUs,
        Alias = "hot-update-dispatch-bench",
        CapabilityFamily = ChaosCapabilityFamily.HotUpdateWorkflow,
        Capability = ChaosCapabilityItem.PatchCallbackFlow,
        Requires = ChaosRuntimeFeature.HotUpdate,
        Archetype = ChaosSolutionArchetype.SkeletonPatchSolution,
        HotUpdateCapability = ChaosHotUpdateCapability.PatchCallbackFlow,
        Modes = ChaosExecutionMode.Managed,
        WarmupCount = 1,
        IterationCount = 45,
        InvocationCount = 1)]
    public static int RunWorkload()
    {
        var runtimeManager = new RuntimeManager();
        runtimeManager.RegisterInt32Unary(BenchmarkIdentity, static value => value + 1);

        var checksum = 0;
        for (var index = 0; index < IterationCount; index++)
        {
            checksum += runtimeManager.DispatchInt32Unary(BenchmarkIdentity, index, static value => value);
        }

        return checksum;
    }
}
