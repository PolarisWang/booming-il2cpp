using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal static class UInt64BitwiseMath
{
    public static ulong LoadLiteralUInt64()
    {
        return 0x123456789ABCDEF0UL;
    }

    public static ulong AndLiteralUInt64(ulong value)
    {
        return value & 0x00FF00FF00FF00FFUL;
    }

    public static ulong OrLiteralUInt64(ulong value)
    {
        return value | 0x00000000000000AAUL;
    }

    public static ulong XorLiteralUInt64(ulong value)
    {
        return value ^ 0x0F0F0F0F0F0F0F0FUL;
    }

    public static int FoldUInt64ToInt32(ulong value)
    {
        return (int)value;
    }
}

internal static class UInt64BitwiseProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "uint64-bitwise-proof",
        CapabilityFamily = ChaosCapabilityFamily.PrimitivesAndOps,
        Capability = ChaosCapabilityItem.BitwiseOps,
        Priority = 2)]
    public static int Run()
    {
        ulong literal = UInt64BitwiseMath.LoadLiteralUInt64();
        ulong masked = UInt64BitwiseMath.AndLiteralUInt64(literal);
        ulong merged = UInt64BitwiseMath.OrLiteralUInt64(masked);
        ulong flipped = UInt64BitwiseMath.XorLiteralUInt64(merged);

        Assert.Equal(0x123456789ABCDEF0UL, literal);
        Assert.Equal(0x0034007800BC00F0UL, masked);
        Assert.Equal(0x0034007800BC00FAUL, merged);
        Assert.Equal(0x0F3B0F770FB30FF5UL, flipped);
        Assert.Equal(263393269, UInt64BitwiseMath.FoldUInt64ToInt32(flipped));
        return 0;
    }
}
