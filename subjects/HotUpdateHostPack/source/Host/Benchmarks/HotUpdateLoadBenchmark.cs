using Chaos.IL2CPP.Contracts;
using Chaos.IL2CPP.HotUpdate;
using Chaos.TestFramework;

namespace HotUpdateHostPack;

internal static class HotUpdateLoadBenchmarkEntry
{
    private static readonly ManagedMethodIdentityArtifact BenchmarkIdentity =
        ManagedMethodIdentityResolver.Create(
            "BenchHotUpdateLoad/HotPatch::GetValue()",
            "System.Int32 HotPatch::GetValue()");

    [ChaosBenchmark(
        ChaosBenchmarkCategory.HotUpdate,
        ChaosMetric.WallClockUs,
        Alias = "hot-update-load-bench",
        CapabilityFamily = ChaosCapabilityFamily.HotUpdateWorkflow,
        Capability = ChaosCapabilityItem.PackageLoad,
        Requires = ChaosRuntimeFeature.HotUpdate,
        Archetype = ChaosSolutionArchetype.SkeletonPatchSolution,
        HotUpdateCapability = ChaosHotUpdateCapability.PackageLoad
            | ChaosHotUpdateCapability.PatchIntegrity,
        Modes = ChaosExecutionMode.Managed,
        WarmupCount = 1,
        IterationCount = 8,
        InvocationCount = 1)]
    public static int RunWorkload()
    {
        var workspace = Path.Combine(Path.GetTempPath(), $"bench-hot-update-load-{Guid.NewGuid():N}");
        Directory.CreateDirectory(workspace);

        try
        {
            var packageRoot = HotUpdatePackageSupport.CreatePackageRoot(
                workspace,
                "package",
                HotUpdatePackageSupport.CurrentAotVersion,
                "BenchHotUpdateLoad/HotPatch::GetValue()",
                [0x48, 0x55, 0x4C, 0x31],
                "benchmark-package");
            var runtimeManager = new RuntimeManager();
            var loaded = runtimeManager.LoadPackage(
                packageRoot,
                HotUpdatePackageSupport.CurrentAotVersion,
                new HotUpdateMethodBindingSet
                {
                    ConstantInt32Bindings =
                    [
                        new HotUpdateConstantInt32Binding
                        {
                            Identity = BenchmarkIdentity,
                            ConstantValue = 1,
                        },
                    ],
                });
            return loaded ? 1 : 0;
        }
        finally
        {
            if (Directory.Exists(workspace))
            {
                Directory.Delete(workspace, recursive: true);
            }
        }
    }
}
