using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal static class TaskSchedulingProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "task-scheduling-proof",
        CapabilityFamily = ChaosCapabilityFamily.ThreadingAndSynchronization,
        Capability = ChaosCapabilityItem.TaskScheduling,
        Requires = ChaosRuntimeFeature.Threading | ChaosRuntimeFeature.Synchronization,
        Priority = 4)]
    public static int Run()
    {
        int continuationThreadId = 0;
        var source = new TaskCompletionSource<int>(TaskCreationOptions.RunContinuationsAsynchronously);
        Task<int> continuation = source.Task.ContinueWith(
            antecedent =>
            {
                continuationThreadId = Environment.CurrentManagedThreadId;
                return antecedent.Result + 1;
            },
            CancellationToken.None,
            TaskContinuationOptions.None,
            TaskScheduler.Default);

        Task.Run(() => source.SetResult(41)).GetAwaiter().GetResult();

        int result = continuation.GetAwaiter().GetResult();
        Assert.Equal(42, result);
        Assert.True(continuationThreadId != 0);
        return 0;
    }
}
