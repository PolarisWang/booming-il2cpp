using Chaos.TestFramework;

namespace CoreRuntimeBenchmarks;

internal static class UInt64BitwiseBenchmarkMath
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

internal static class UInt64BitwiseBenchmarkEntry
{
    private const int IterationCount = 250_000;

    [ChaosBenchmark(
        ChaosBenchmarkCategory.RuntimeDispatch,
        ChaosMetric.WallClockUs,
        Alias = "uint64-bitwise-bench",
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
            ulong literal = UInt64BitwiseBenchmarkMath.LoadLiteralUInt64();
            ulong masked = UInt64BitwiseBenchmarkMath.AndLiteralUInt64(literal);
            ulong merged = UInt64BitwiseBenchmarkMath.OrLiteralUInt64(masked);
            ulong flipped = UInt64BitwiseBenchmarkMath.XorLiteralUInt64(merged);
            result += UInt64BitwiseBenchmarkMath.FoldUInt64ToInt32(flipped);
        }

        return result;
    }
}
