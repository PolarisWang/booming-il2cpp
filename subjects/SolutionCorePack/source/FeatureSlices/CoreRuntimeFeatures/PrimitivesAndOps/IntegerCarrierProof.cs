using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal static class IntegerCarrierMath
{
    public static long RoundTripInt64(long value)
    {
        return value;
    }

    public static ushort RoundTripUInt16(ushort value)
    {
        return value;
    }

    public static short IncrementInt16(short value)
    {
        return (short)(value + 1);
    }

    public static byte TruncateToByte(int value)
    {
        return (byte)value;
    }

    public static sbyte TruncateToSByte(int value)
    {
        return (sbyte)value;
    }
}

internal static class IntegerCarrierProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "integer-carrier-proof",
        CapabilityFamily = ChaosCapabilityFamily.PrimitivesAndOps,
        Capability = ChaosCapabilityItem.NumericConversion,
        Priority = 2)]
    public static int Run()
    {
        Assert.True(IntegerCarrierMath.RoundTripInt64((long)300) == 300L);
        Assert.True(IntegerCarrierMath.RoundTripUInt16((ushort)300) == (ushort)300);
        Assert.True(IntegerCarrierMath.IncrementInt16((short)41) == (short)42);
        Assert.True(IntegerCarrierMath.TruncateToByte(300) == (byte)44);
        Assert.True(IntegerCarrierMath.TruncateToSByte(300) == (sbyte)44);
        return 0;
    }
}
