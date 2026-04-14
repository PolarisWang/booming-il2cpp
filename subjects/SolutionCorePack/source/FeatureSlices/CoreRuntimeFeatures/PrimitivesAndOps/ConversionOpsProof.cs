using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal static class ConversionOpsProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "conversion-ops-proof",
        CapabilityFamily = ChaosCapabilityFamily.PrimitivesAndOps,
        Capability = ChaosCapabilityItem.NumericConversion,
        Priority = 2)]
    public static int Run()
    {
        int i = 300;
        Assert.Equal((sbyte)44, (sbyte)i);
        Assert.Equal((short)300, (short)i);
        Assert.Equal((byte)44, (byte)i);
        Assert.Equal((ushort)300, (ushort)i);
        Assert.Equal(300L, (long)i);
        Assert.Equal("300.0", ((float)i).ToString("F1"));
        Assert.Equal("300.0", ((double)i).ToString("F1"));
        double d = 3.7;
        Assert.Equal(3, (int)d);
        return 0;
    }
}
