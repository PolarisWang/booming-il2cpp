using Chaos.TestFramework;

namespace CoreRuntimeBenchmarks;

/// <summary>
/// Benchmarks the throw/catch path (exception raised and caught).
/// Lower IterationCount because throw/catch is ~1000x more expensive than happy path.
/// </summary>
internal static class ExceptionThrowCatchBenchmarkEntry
{
    private const int IterationCount = 10_000;

    [ChaosBenchmark(
        ChaosBenchmarkCategory.RuntimeDispatch,
        ChaosMetric.WallClockUs,
        Alias = "exception-throw-catch-bench",
        CapabilityFamily = ChaosCapabilityFamily.ExceptionsAndControlFlow,
        Capability = ChaosCapabilityItem.ExceptionBasic,
        Modes = ChaosExecutionMode.Managed | ChaosExecutionMode.Native,
        WarmupCount = 1,
        IterationCount = 2,
        InvocationCount = 1)]
    public static int RunWorkload()
    {
        int result = 0;
        for (int i = 0; i < IterationCount; i++)
        {
            try
            {
                ThrowIfNegative(i - (IterationCount / 2));
                result += i;
            }
            catch (InvalidOperationException)
            {
                result += -1;
            }
        }
        return result;
    }

    private static void ThrowIfNegative(int value)
    {
        if (value < 0)
        {
            throw new InvalidOperationException("negative");
        }
    }
}

/// <summary>
/// Benchmarks the happy path (entering/exiting try/catch block without throwing).
/// </summary>
internal static class TryCatchHappyPathBenchmarkEntry
{
    private const int IterationCount = 100_000;

    [ChaosBenchmark(
        ChaosBenchmarkCategory.RuntimeDispatch,
        ChaosMetric.WallClockUs,
        Alias = "try-catch-happy-path-bench",
        CapabilityFamily = ChaosCapabilityFamily.ExceptionsAndControlFlow,
        Capability = ChaosCapabilityItem.ExceptionBasic,
        Modes = ChaosExecutionMode.Managed | ChaosExecutionMode.Native,
        WarmupCount = 1,
        IterationCount = 2,
        InvocationCount = 1)]
    public static int RunWorkload()
    {
        int result = 0;
        for (int i = 0; i < IterationCount; i++)
        {
            try
            {
                result += i;
            }
            catch (InvalidOperationException)
            {
                result += -1;
            }
        }
        return result;
    }
}