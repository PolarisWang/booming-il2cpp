using Chaos.IL2CPP.Contracts;
using Chaos.IL2CPP.HotUpdate;
using Chaos.TestFramework;

namespace HotUpdateHostPack;

internal static class PatchCallbackFlowProofEntry
{
    private static readonly ManagedMethodIdentityArtifact CallbackIdentity =
        ManagedMethodIdentityResolver.Create(
            "PatchCallbackFlowProof/HotPatch::AddOne(System.Int32)",
            "System.Int32 HotPatch::AddOne(System.Int32)");

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
        var runtimeManager = new RuntimeManager();
        runtimeManager.RegisterInt32Unary(CallbackIdentity, static value => value + 1);

        var first = runtimeManager.DispatchInt32Unary(CallbackIdentity, 41, static value => value);
        var second = runtimeManager.DispatchInt32Unary(CallbackIdentity, 10, static value => value - 1);

        Assert.Equal(42, first);
        Assert.Equal(11, second);
        return 0;
    }
}
