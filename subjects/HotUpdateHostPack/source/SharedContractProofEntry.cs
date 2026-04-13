using Chaos.TestFramework;

namespace HotUpdateHostPack;

internal static class SharedContractProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.HotUpdateContract,
        Alias = "shared-contract-proof",
        Requires = ChaosRuntimeFeature.HotUpdate,
        Priority = 2)]
    public static int Run()
    {
        var witness = new ContractIdentityWitness();
        var ping = witness.Ping(41);
        Assert.Equal(42, ping);
        return 0;
    }

    private sealed class ContractIdentityWitness
    {
        public int Ping(int value)
        {
            return value + 1;
        }
    }
}
