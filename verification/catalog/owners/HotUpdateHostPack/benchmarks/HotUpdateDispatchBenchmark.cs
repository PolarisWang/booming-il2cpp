using Chaos.IL2CPP.Contracts;
using Chaos.IL2CPP.HotUpdate;
using Chaos.TestFramework;

namespace HotUpdateHostPack;

internal static class HotUpdateDispatchBenchmarkEntry
{
    private const int IterationCount = 1000;
    private static readonly ManagedMethodIdentityArtifact BenchmarkIdentity =
        ManagedMethodIdentityResolver.Create(
            new ManagedMethodIdentitySpec
            {
                AssemblyName = "BenchHotUpdateDispatch",
                DeclaringTypeSubjectId = "BenchHotUpdateDispatch/HotPatch",
                DeclaringTypeDisplayName = "HotPatch",
                MethodName = "AddOne",
                SubjectId = "BenchHotUpdateDispatch/HotPatch::AddOne(System.Int32)",
                Signature = "System.Int32 HotPatch::AddOne(System.Int32)",
            });

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
        var workspace = Path.Combine(Path.GetTempPath(), $"bench-hot-update-dispatch-{Guid.NewGuid():N}");
        Directory.CreateDirectory(workspace);

        try
        {
            var packageRoot = HotUpdatePackageSupport.CreatePackageRoot(
                workspace,
                "dispatch",
                HotUpdatePackageSupport.CurrentAotVersion,
                "BenchHotUpdateDispatch/HotPatch::AddOne(System.Int32)",
                [0x48, 0x55, 0x44, 0x31],
                "dispatch-benchmark");
            var runtimeManager = new RuntimeManager();
            var loaded = runtimeManager.LoadPackage(
                packageRoot,
                HotUpdatePackageSupport.CurrentAotVersion,
                new HotUpdateMethodBindingSet
                {
                    Int32UnaryBindings =
                    [
                        new HotUpdateInt32UnaryBinding
                        {
                            Identity = BenchmarkIdentity,
                            Target = static value => value + 1,
                        },
                    ],
                });
            Assert.True(loaded);
            Assert.Equal(RuntimeMode.Mixed, runtimeManager.Mode);

            var checksum = 0;
            for (var index = 0; index < IterationCount; index++)
            {
                checksum += runtimeManager.DispatchInt32Unary(BenchmarkIdentity, index, static value => value);
            }

            return checksum;
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
