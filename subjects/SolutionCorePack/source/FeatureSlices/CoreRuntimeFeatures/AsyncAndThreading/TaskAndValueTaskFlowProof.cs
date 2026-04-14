using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal static class TaskAndValueTaskFlowProofEntry
{
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

    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "task-valuetask-flow-proof",
        CapabilityFamily = ChaosCapabilityFamily.AsyncAndIterators,
        Capability = ChaosCapabilityItem.TaskAndValueTaskFlow,
        Requires = ChaosRuntimeFeature.AsyncStateMachine,
        Priority = 3)]
    public static int Run()
    {
        var task = ComputeTaskAsync(40);
        var valueTask = ComputeValueTaskAsync(40);
        var taskResult = task.GetAwaiter().GetResult();
        var valueTaskResult = valueTask.GetAwaiter().GetResult();
        Assert.Equal(41, taskResult);
        Assert.Equal(42, valueTaskResult);
        Assert.Equal(83, taskResult + valueTaskResult);
        return 0;
    }
}
