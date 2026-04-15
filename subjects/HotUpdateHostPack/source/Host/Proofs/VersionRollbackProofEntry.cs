using Chaos.IL2CPP.Contracts;
using Chaos.IL2CPP.HotUpdate;
using Chaos.TestFramework;

namespace HotUpdateHostPack;

internal static class VersionRollbackProofEntry
{
    private static readonly ManagedMethodIdentityArtifact HotPatchValueIdentity =
        ManagedMethodIdentityResolver.Create(
            "VersionRollbackProof/HotPatch::GetValue()",
            "System.Int32 HotPatch::GetValue()");

    [ChaosUnitTest(
        ChaosUnitCategory.HotUpdateContract,
        Alias = "version-rollback-proof",
        CapabilityFamily = ChaosCapabilityFamily.HotUpdateWorkflow,
        Capability = ChaosCapabilityItem.VersionRollback,
        Requires = ChaosRuntimeFeature.HotUpdate,
        Archetype = ChaosSolutionArchetype.VersionCompatibilitySolution,
        HotUpdateCapability = ChaosHotUpdateCapability.VersionRollback
            | ChaosHotUpdateCapability.PatchIntegrity,
        Priority = 2)]
    public static int Run()
    {
        var workspace = Path.Combine(Path.GetTempPath(), $"version-rollback-proof-{Guid.NewGuid():N}");
        Directory.CreateDirectory(workspace);

        try
        {
            var runtimeManager = new RuntimeManager();
            var v1Root = HotUpdatePackageSupport.CreatePackageRoot(
                workspace,
                "v1",
                HotUpdatePackageSupport.CurrentAotVersion,
                "VersionRollbackProof/HotPatch::Apply()",
                [0x56, 0x52, 0x50, 0x31],
                "rollback-proof");
            var v2Root = HotUpdatePackageSupport.CreatePackageRoot(
                workspace,
                "v2",
                HotUpdatePackageSupport.CurrentAotVersion,
                "VersionRollbackProof/HotPatch::Apply()",
                [0x56, 0x52, 0x50, 0x32],
                "rollback-proof");
            var incompatibleRoot = HotUpdatePackageSupport.CreatePackageRoot(
                workspace,
                "v3",
                "2.0.0",
                "VersionRollbackProof/HotPatch::Apply()",
                [0x56, 0x52, 0x50, 0x33],
                "rollback-proof");

            runtimeManager.LoadPackage(
                v1Root,
                HotUpdatePackageSupport.CurrentAotVersion,
                CreateBindings(11));
            var v1Value = runtimeManager.DispatchInt32(HotPatchValueIdentity, GetAotFallback);
            Assert.Equal(11, v1Value);

            runtimeManager.LoadPackage(
                v2Root,
                HotUpdatePackageSupport.CurrentAotVersion,
                CreateBindings(22));
            var v2Value = runtimeManager.DispatchInt32(HotPatchValueIdentity, GetAotFallback);
            Assert.Equal(22, v2Value);

            runtimeManager.Rollback();
            var rollbackToV1 = runtimeManager.DispatchInt32(HotPatchValueIdentity, GetAotFallback);
            Assert.Equal(11, rollbackToV1);

            runtimeManager.Rollback();
            var rollbackToAot = runtimeManager.DispatchInt32(HotPatchValueIdentity, GetAotFallback);
            Assert.Equal(5, rollbackToAot);

            var compatible = runtimeManager.LoadPackage(
                incompatibleRoot,
                HotUpdatePackageSupport.CurrentAotVersion,
                CreateBindings(99));
            Assert.False(compatible);
            return 0;
        }
        finally
        {
            if (Directory.Exists(workspace))
            {
                Directory.Delete(workspace, recursive: true);
            }
        }
    }

    private static int GetAotFallback()
    {
        return 5;
    }

    private static HotUpdateMethodBindingSet CreateBindings(int value)
    {
        return new HotUpdateMethodBindingSet
        {
            ConstantInt32Bindings =
            [
                new HotUpdateConstantInt32Binding
                {
                    Identity = HotPatchValueIdentity,
                    ConstantValue = value,
                },
            ],
        };
    }
}
