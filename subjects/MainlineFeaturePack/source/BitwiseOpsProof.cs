using Chaos.TestFramework;

namespace MainlineFeaturePack;

internal static class BitwiseOpsProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "bitwise-ops-proof",
        Evidence = ChaosEvidenceKind.Stdout,
        Priority = 2)]
    public static int Run()
    {
        int a = 0b1010;
        int b = 0b1100;
        Console.WriteLine("bit-and=" + (a & b));
        Console.WriteLine("bit-or=" + (a | b));
        Console.WriteLine("bit-xor=" + (a ^ b));
        Console.WriteLine("bit-not=" + (~a));
        Console.WriteLine("bit-shl=" + (a << 2));
        Console.WriteLine("bit-shr=" + (a >> 1));
        int neg = -8;
        Console.WriteLine("bit-shr-un=" + (int)((uint)neg >> 1));
        return 0;
    }
}
