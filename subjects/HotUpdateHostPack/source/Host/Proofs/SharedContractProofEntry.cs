using Chaos.TestFramework;

namespace HotUpdateHostPack;

internal static class SharedContractProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.HotUpdateContract,
        Alias = "shared-contract-proof",
        CapabilityFamily = ChaosCapabilityFamily.HotUpdateWorkflow,
        Capability = ChaosCapabilityItem.SharedContractBinding,
        Requires = ChaosRuntimeFeature.HotUpdate,
        Archetype = ChaosSolutionArchetype.FullProjectHotUpdateSolution,
        HotUpdateCapability = ChaosHotUpdateCapability.SharedContractBinding
            | ChaosHotUpdateCapability.PatchCallbackFlow,
        Priority = 2)]
    public static int Run()
    {
        var witness = new ContractIdentityWitness();
        var ping = witness.Ping(41);
        Assert.Equal(42, ping);
        return 0;
    }
}
