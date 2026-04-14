using Chaos.TestFramework;

namespace CoreRuntimeBenchmarks;

internal static class TaskAndValueTaskFlowBenchmarkEntry
{
    [ChaosBenchmark(
        ChaosBenchmarkCategory.RuntimeDispatch,
        ChaosMetric.WallClockUs,
        Alias = "task-valuetask-flow-bench",
        CapabilityFamily = ChaosCapabilityFamily.AsyncAndIterators,
        Capability = ChaosCapabilityItem.TaskAndValueTaskFlow,
        Modes = ChaosExecutionMode.Managed | ChaosExecutionMode.Native,
        Requires = ChaosRuntimeFeature.AsyncStateMachine,
        WarmupCount = 1,
        IterationCount = 2,
        InvocationCount = 1)]
    public static int RunWorkload()
    {
        int sum = 0;
        for (int i = 0; i < 32; i++)
        {
            sum += ComputeTaskAsync(i).GetAwaiter().GetResult();
            sum += ComputeValueTaskAsync(i).GetAwaiter().GetResult();
        }

        return sum % 10000;
    }

    private static async Task<int> ComputeTaskAsync(int value)
    {
        await Task.Yield();
        return value + 1;
    }

    private static async ValueTask<int> ComputeValueTaskAsync(int value)
    {
        await Task.Yield();
        return value + 2;
    }
}
