using Chaos.IL2CPP.Contracts;
using Chaos.IL2CPP.HotUpdate;
using Chaos.TestFramework;

namespace HotUpdateHostPack;

internal static class VersionRollbackProofEntry
{
    private const string HotPatchValueAuthorityKey = "hot-update://version-rollback/value";
    private static readonly ManagedMethodIdentityArtifact HotPatchValueIdentity =
        ManagedMethodIdentityResolver.Create(
            new ManagedMethodIdentitySpec
            {
                AssemblyName = "VersionRollbackProof",
                DeclaringTypeSubjectId = "VersionRollbackProof/HotPatch",
                DeclaringTypeDisplayName = "HotPatch",
                MethodName = "GetValue",
                SubjectId = "VersionRollbackProof/HotPatch::GetValue()",
                Signature = "System.Int32 HotPatch::GetValue()",
                ExecutionAuthorityKey = HotPatchValueAuthorityKey,
            });
    private static readonly ManagedMethodIdentityArtifact HotPatchValueBindingIdentity =
        ManagedMethodIdentityResolver.Create(
            new ManagedMethodIdentitySpec
            {
                AssemblyName = "VersionRollbackProof",
                DeclaringTypeSubjectId = "VersionRollbackProof/HotPatch",
                DeclaringTypeDisplayName = "HotPatch",
                MethodName = "GetValue",
                SubjectId = "VersionRollbackProof/HotPatch::GetValue()",
                Signature = "System.Int32 HotPatch::GetValue()",
            });

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
            const string expectedTargetAotReason =
                "hot update package target AOT version '2.0.0' is not compatible with runtime AOT version '1.0.0'. Expected matching major.minor compatibility band.";
            const string expectedKernelVersionReason =
                "hot update package kernel artifact version 'v2' is not compatible with runtime kernel artifact version 'v1'.";
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
            var incompatibleTargetAotRoot = HotUpdatePackageSupport.CreatePackageRoot(
                workspace,
                "v3-target-aot",
                "2.0.0",
                "VersionRollbackProof/HotPatch::Apply()",
                [0x56, 0x52, 0x50, 0x33],
                "rollback-proof");
            var incompatibleKernelRoot = HotUpdatePackageSupport.CreatePackageRoot(
                workspace,
                "v4-kernel",
                HotUpdatePackageSupport.CurrentAotVersion,
                "VersionRollbackProof/HotPatch::Apply()",
                [0x56, 0x52, 0x50, 0x34],
                "rollback-proof",
                kernelArtifactVersion: "v2");

            runtimeManager.LoadPackage(
                v1Root,
                HotUpdatePackageSupport.CurrentAotVersion,
                CreateBindings(11));
            var v1Value = runtimeManager.DispatchInt32(HotPatchValueIdentity, GetAotFallback);
            Assert.Equal(11, v1Value);
            var v1Handle = runtimeManager.CreateHandle(HotPatchValueIdentity);
            Assert.True(runtimeManager.TryDispatchHandle(v1Handle, Array.Empty<object?>(), out var v1HandleResult, out var v1HandleReason));
            Assert.Equal(string.Empty, v1HandleReason);
            Assert.Equal(11, Convert.ToInt32(v1HandleResult));

            runtimeManager.LoadPackage(
                v2Root,
                HotUpdatePackageSupport.CurrentAotVersion,
                CreateBindings(22));
            Assert.False(runtimeManager.TryDispatchHandle(v1Handle, Array.Empty<object?>(), out _, out var v1StaleReason));
            Assert.Equal(HotUpdateDispatchReasonCodes.StaleHandle, v1StaleReason);
            var v2Value = runtimeManager.DispatchInt32(HotPatchValueIdentity, GetAotFallback);
            Assert.Equal(22, v2Value);
            var v2Handle = runtimeManager.CreateHandle(HotPatchValueIdentity);
            Assert.True(runtimeManager.TryDispatchHandle(v2Handle, Array.Empty<object?>(), out var v2HandleResult, out var v2HandleReason));
            Assert.Equal(string.Empty, v2HandleReason);
            Assert.Equal(22, Convert.ToInt32(v2HandleResult));

            runtimeManager.Rollback();
            Assert.False(runtimeManager.TryDispatchHandle(v2Handle, Array.Empty<object?>(), out _, out var v2StaleReason));
            Assert.Equal(HotUpdateDispatchReasonCodes.StaleHandle, v2StaleReason);
            var rollbackToV1 = runtimeManager.DispatchInt32(HotPatchValueIdentity, GetAotFallback);
            Assert.Equal(11, rollbackToV1);

            runtimeManager.Rollback();
            var rollbackToAot = runtimeManager.DispatchInt32(HotPatchValueIdentity, GetAotFallback);
            Assert.Equal(5, rollbackToAot);

            var targetAotCompatible = runtimeManager.LoadPackage(
                incompatibleTargetAotRoot,
                HotUpdatePackageSupport.CurrentAotVersion,
                CreateBindings(99));
            Assert.False(targetAotCompatible);
            Assert.Equal(expectedTargetAotReason, runtimeManager.LastError);
            Assert.False(runtimeManager.CanRollback);

            var kernelCompatible = runtimeManager.LoadPackage(
                incompatibleKernelRoot,
                HotUpdatePackageSupport.CurrentAotVersion,
                CreateBindings(101));
            Assert.False(kernelCompatible);
            Assert.Equal(expectedKernelVersionReason, runtimeManager.LastError);
            Assert.False(runtimeManager.CanRollback);
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
                    Identity = HotPatchValueBindingIdentity,
                    ExecutionAuthorityKey = HotPatchValueAuthorityKey,
                    ConstantValue = value,
                },
            ],
        };
    }
}
