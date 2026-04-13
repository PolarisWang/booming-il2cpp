using Chaos.TestFramework;

namespace HotUpdateHostPack;

internal static class SharedContractProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.HotUpdateContract,
        Alias = "shared-contract-proof",
        Requires = ChaosRuntimeFeature.HotUpdate,
        Evidence = ChaosEvidenceKind.Stdout,
        Priority = 2)]
    public static int Run()
    {
        var witness = new ContractIdentityWitness();
        Console.WriteLine("SharedContractProof entry reached.");
        Console.WriteLine("args=0");
        Console.WriteLine($"ping={witness.Ping(41)}");
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
