using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal static class BitwiseOpsProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "bitwise-ops-proof",
        CapabilityFamily = ChaosCapabilityFamily.PrimitivesAndOps,
        Capability = ChaosCapabilityItem.BitwiseOps,
        Priority = 2)]
    public static int Run()
    {
        int a = 0b1010;
        int b = 0b1100;
        Assert.Equal(8, a & b);
        Assert.Equal(14, a | b);
        Assert.Equal(6, a ^ b);
        Assert.Equal(-11, ~a);
        Assert.Equal(40, a << 2);
        Assert.Equal(5, a >> 1);
        int neg = -8;
        Assert.Equal(2147483644, (int)((uint)neg >> 1));
        return 0;
    }
}
