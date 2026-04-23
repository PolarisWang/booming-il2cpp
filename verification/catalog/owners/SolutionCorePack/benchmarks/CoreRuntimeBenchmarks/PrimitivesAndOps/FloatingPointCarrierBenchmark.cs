using Chaos.TestFramework;

namespace CoreRuntimeBenchmarks;

internal static class FloatingPointCarrierBenchmarkMath
{
    public static float RoundTripSingle(float value)
    {
        return value;
    }

    public static double RoundTripDouble(double value)
    {
        return value;
    }

    public static float ConvertInt32ToSingle(int value)
    {
        return (float)value;
    }

    public static double ConvertInt32ToDouble(int value)
    {
        return (double)value;
    }
}

internal static class FloatingPointCarrierBenchmarkEntry
{
    private const int IterationCount = 250_000;

    [ChaosBenchmark(
        ChaosBenchmarkCategory.RuntimeDispatch,
        ChaosMetric.WallClockUs,
        Alias = "floating-point-carrier-bench",
        CapabilityFamily = ChaosCapabilityFamily.PrimitivesAndOps,
        Capability = ChaosCapabilityItem.NumericConversion,
        Modes = ChaosExecutionMode.Managed | ChaosExecutionMode.Native,
        WarmupCount = 1,
        IterationCount = 2,
        InvocationCount = 1)]
    public static int RunWorkload()
    {
        for (int i = 0; i < IterationCount; i++)
        {
            _ = FloatingPointCarrierBenchmarkMath.RoundTripSingle(42.5f);
            _ = FloatingPointCarrierBenchmarkMath.RoundTripDouble(42.5);
            _ = FloatingPointCarrierBenchmarkMath.ConvertInt32ToSingle(i);
            _ = FloatingPointCarrierBenchmarkMath.ConvertInt32ToDouble(i);
        }

        return IterationCount;
    }
}
