using Chaos.TestFramework;

namespace CoreRuntimeBenchmarks;

internal static class ShiftOpsBenchmarkMath
{
    public static int LeftShift(int value)
    {
        return value << 2;
    }

    public static int ArithmeticRightShift(int value)
    {
        return value >> 1;
    }

    public static int LogicalRightShiftSignBit(int value)
    {
        return (int)((uint)value >> 31);
    }
}

internal static class ShiftOpsBenchmarkEntry
{
    private const int IterationCount = 250_000;

    [ChaosBenchmark(
        ChaosBenchmarkCategory.RuntimeDispatch,
        ChaosMetric.WallClockUs,
        Alias = "shift-ops-bench",
        CapabilityFamily = ChaosCapabilityFamily.PrimitivesAndOps,
        Capability = ChaosCapabilityItem.BitwiseOps,
        Modes = ChaosExecutionMode.Managed | ChaosExecutionMode.Native,
        WarmupCount = 1,
        IterationCount = 2,
        InvocationCount = 1)]
    public static int RunWorkload()
    {
        int result = 0;
        for (int i = 0; i < IterationCount; i++)
        {
            result += ShiftOpsBenchmarkMath.LeftShift(i & 15);
            result += ShiftOpsBenchmarkMath.ArithmeticRightShift(-8);
            result += ShiftOpsBenchmarkMath.LogicalRightShiftSignBit(-8);
        }

        return result;
    }
}
