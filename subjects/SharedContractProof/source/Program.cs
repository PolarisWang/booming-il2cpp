internal sealed class ContractIdentityWitness
{
    public int Ping(int value)
    {
        return value + 1;
    }
}

internal static class Program
{
    private static int Main(string[] args)
    {
        var witness = new ContractIdentityWitness();
        System.Console.WriteLine("SharedContractProof entry reached.");
        System.Console.WriteLine($"args={args.Length}");
        System.Console.WriteLine($"ping={witness.Ping(41)}");
        return 0;
    }
}
