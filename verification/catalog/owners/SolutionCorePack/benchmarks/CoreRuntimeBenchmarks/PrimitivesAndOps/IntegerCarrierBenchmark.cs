using Chaos.TestFramework;

namespace CoreRuntimeBenchmarks;

internal static class IntegerCarrierBenchmarkMath
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

internal static class IntegerCarrierBenchmarkEntry
{
    private const int IterationCount = 250_000;

    [ChaosBenchmark(
        ChaosBenchmarkCategory.RuntimeDispatch,
        ChaosMetric.WallClockUs,
        Alias = "integer-carrier-bench",
        CapabilityFamily = ChaosCapabilityFamily.PrimitivesAndOps,
        Capability = ChaosCapabilityItem.NumericConversion,
        Modes = ChaosExecutionMode.Managed | ChaosExecutionMode.Native,
        WarmupCount = 1,
        IterationCount = 2,
        InvocationCount = 1)]
    public static int RunWorkload()
    {
        int result = 0;
        for (int i = 0; i < IterationCount; i++)
        {
            result += (int)IntegerCarrierBenchmarkMath.RoundTripInt64((long)(i + 10));
            result += IntegerCarrierBenchmarkMath.RoundTripUInt16((ushort)(i % 256));
            result += IntegerCarrierBenchmarkMath.IncrementInt16((short)40);
            result += IntegerCarrierBenchmarkMath.TruncateToByte(300);
            result += IntegerCarrierBenchmarkMath.TruncateToSByte(300);
        }

        return result;
    }
}
