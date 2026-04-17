using Chaos.IL2CPP.Contracts;
using Chaos.IL2CPP.HotUpdate;
using Chaos.TestFramework;

namespace MixedExecutionFeaturePack;

internal static class MixedDelegateFlowProofEntry
{
    private const string DelegateWrapperId = "mixed-delegate-wrapper";
    private static readonly ManagedMethodIdentityArtifact HotUpdateIdentity =
        ManagedMethodIdentityResolver.Create(
            "MixedDelegateFlowProof/HotService::Process(System.Int32)",
            "System.Int32 HotService::Process(System.Int32)");

    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "mixed-delegate-flow-proof",
        CapabilityFamily = ChaosCapabilityFamily.MixedExecution,
        Capability = ChaosCapabilityItem.MixedDelegateFlow,
        Requires = ChaosRuntimeFeature.Delegate,
        Archetype = ChaosSolutionArchetype.MixedBridgeSolution,
        Priority = 2)]
    public static int Run()
    {
        var plan = new BridgeGenerator().Generate(new BridgeGenerationRequest
        {
            DelegateWrappers =
            [
                new DelegateWrapperSpec
                {
                    WrapperId = DelegateWrapperId,
                    HotUpdateIdentity = HotUpdateIdentity,
                    HotUpdateSubjectId = HotUpdateIdentity.SubjectId,
                },
            ],
        });

        var dispatcher = new BridgeDispatcher();
        dispatcher.ApplyPlan(plan);
        dispatcher.RegisterHotUpdateInt32UnaryTarget(HotUpdateIdentity, static value => value * 2);

        var callback = dispatcher.CreateAotDelegateWrapper(DelegateWrapperId);
        Assert.Equal(42, callback(21));
        Assert.Equal(42, InvokeCallback(callback, 21));
        return 0;
    }

    private static int InvokeCallback(Func<int, int> callback, int value)
    {
        return callback(value);
    }
}
