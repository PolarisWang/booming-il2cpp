using Chaos.TestFramework;

namespace MainlineFeaturePack;

internal static class ConversionOpsProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "conversion-ops-proof",
        Evidence = ChaosEvidenceKind.Stdout,
        Priority = 2)]
    public static int Run()
    {
        int i = 300;
        Console.WriteLine("conv-i1=" + (sbyte)i);
        Console.WriteLine("conv-i2=" + (short)i);
        Console.WriteLine("conv-u1=" + (byte)i);
        Console.WriteLine("conv-u2=" + (ushort)i);
        Console.WriteLine("conv-i8=" + (long)i);
        Console.WriteLine("conv-r4=" + ((float)i).ToString("F1"));
        Console.WriteLine("conv-r8=" + ((double)i).ToString("F1"));
        double d = 3.7;
        Console.WriteLine("conv-i4-from-r8=" + (int)d);
        return 0;
    }
}
