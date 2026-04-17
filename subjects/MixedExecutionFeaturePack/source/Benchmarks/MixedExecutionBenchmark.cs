using Chaos.TestFramework;

namespace MixedExecutionFeaturePack;

internal static class MixedExecutionBenchmarkEntry
{
    private const int IterationCount = 1000;
    private static Func<int, int>? s_interpreterFn;

    [ChaosBenchmark(
        ChaosBenchmarkCategory.RuntimeDispatch,
        ChaosMetric.WallClockUs,
        Alias = "mixed-execution-bench",
        CapabilityFamily = ChaosCapabilityFamily.MixedExecution,
        Capability = ChaosCapabilityItem.MixedBridgeDispatch,
        Requires = ChaosRuntimeFeature.Delegate,
        Archetype = ChaosSolutionArchetype.MixedBridgeSolution,
        Modes = ChaosExecutionMode.Managed | ChaosExecutionMode.Interpreter,
        WarmupCount = 1,
        IterationCount = 6,
        InvocationCount = 1000)]
    public static int RunWorkload()
    {
        s_interpreterFn = static x => x * 2 + 1;

        long checksum = 0;
        for (int i = 0; i < IterationCount * 100; i++)
        {
            int v1 = AotCompute(i);
            int v2 = InterpreterCompute(v1);
            int v3 = AotCompute(v2);
            checksum += v3;
        }

        return (int)(checksum % 10000);
    }

    private static int AotCompute(int x)
    {
        return x * x + x;
    }

    private static int InterpreterCompute(int x)
    {
        return s_interpreterFn?.Invoke(x) ?? x;
    }
}
