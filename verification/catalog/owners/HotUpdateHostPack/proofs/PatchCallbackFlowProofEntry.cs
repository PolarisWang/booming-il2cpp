using Chaos.IL2CPP.Contracts;
using Chaos.IL2CPP.HotUpdate;
using Chaos.TestFramework;

namespace HotUpdateHostPack;

internal static class PatchCallbackFlowProofEntry
{
    private static readonly ManagedMethodIdentityArtifact CallbackIdentity =
        ManagedMethodIdentityResolver.Create(
            new ManagedMethodIdentitySpec
            {
                AssemblyName = "PatchCallbackFlowProof",
                DeclaringTypeSubjectId = "PatchCallbackFlowProof/HotPatch",
                DeclaringTypeDisplayName = "HotPatch",
                MethodName = "AddOne",
                SubjectId = "PatchCallbackFlowProof/HotPatch::AddOne(System.Int32)",
                Signature = "System.Int32 HotPatch::AddOne(System.Int32)",
            });

    [ChaosUnitTest(
        ChaosUnitCategory.HotUpdateContract,
        Alias = "patch-callback-flow-proof",
        CapabilityFamily = ChaosCapabilityFamily.HotUpdateWorkflow,
        Capability = ChaosCapabilityItem.PatchCallbackFlow,
        Requires = ChaosRuntimeFeature.HotUpdate,
        Archetype = ChaosSolutionArchetype.SkeletonPatchSolution,
        HotUpdateCapability = ChaosHotUpdateCapability.PatchCallbackFlow,
        Priority = 2)]
    public static int Run()
    {
        var workspace = Path.Combine(Path.GetTempPath(), $"patch-callback-flow-proof-{Guid.NewGuid():N}");
        Directory.CreateDirectory(workspace);

        try
        {
            var packageRoot = HotUpdatePackageSupport.CreatePackageRoot(
                workspace,
                "callback",
                HotUpdatePackageSupport.CurrentAotVersion,
                "PatchCallbackFlowProof/HotPatch::AddOne(System.Int32)",
                [0x50, 0x43, 0x46, 0x31],
                "callback-flow-proof");
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
                            Identity = CallbackIdentity,
                            Target = static value => value + 1,
                        },
                    ],
                });
            Assert.True(loaded);
            Assert.Equal(RuntimeMode.Mixed, runtimeManager.Mode);

            var first = runtimeManager.DispatchInt32Unary(CallbackIdentity, 41, static value => value);
            var second = runtimeManager.DispatchInt32Unary(CallbackIdentity, 10, static value => value - 1);

            Assert.Equal(42, first);
            Assert.Equal(11, second);
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
}
