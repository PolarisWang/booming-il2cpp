using Chaos.TestFramework;

namespace MixedExecutionFeaturePack;

internal static class MixedExecutionNativeBenchmarkEntry
{
    private const int IterationCount = 250_000;

    [ChaosBenchmark(
        ChaosBenchmarkCategory.RuntimeDispatch,
        ChaosMetric.WallClockUs,
        Alias = "mixed-execution-native-bench",
        CapabilityFamily = ChaosCapabilityFamily.MixedExecution,
        Capability = ChaosCapabilityItem.InterpreterArithmetic,
        Archetype = ChaosSolutionArchetype.MixedBridgeSolution,
        Modes = ChaosExecutionMode.Managed | ChaosExecutionMode.Native | ChaosExecutionMode.Interpreter,
        WarmupCount = 1,
        IterationCount = 6,
        InvocationCount = 1000)]
    public static int RunWorkload()
    {
        int checksum = 17;
        for (int i = 0; i < IterationCount; i++)
        {
            int aotStage = checksum * checksum + checksum + i;
            int interpreterShapeStage = aotStage * 2 + 1;
            int aotReentryStage = interpreterShapeStage * interpreterShapeStage + interpreterShapeStage;
            checksum = (checksum + aotReentryStage) % 10000;
        }

        return checksum;
    }
}
