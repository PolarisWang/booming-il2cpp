using Chaos.TestFramework;

namespace CoreRuntimeBenchmarks;

internal static class TaskSchedulingBenchmarkEntry
{
    [ChaosBenchmark(
        ChaosBenchmarkCategory.RuntimeDispatch,
        ChaosMetric.WallClockUs,
        Alias = "task-scheduling-bench",
        CapabilityFamily = ChaosCapabilityFamily.ThreadingAndSynchronization,
        Capability = ChaosCapabilityItem.TaskScheduling,
        Modes = ChaosExecutionMode.Managed,
        Requires = ChaosRuntimeFeature.Threading | ChaosRuntimeFeature.Synchronization,
        WarmupCount = 1,
        IterationCount = 2,
        InvocationCount = 1)]
    public static int RunWorkload()
    {
        int checksum = 0;
        for (int i = 0; i < 16; i++)
        {
            var source = new TaskCompletionSource<int>(TaskCreationOptions.RunContinuationsAsynchronously);
            Task<int> continuation = source.Task.ContinueWith(task => task.Result + 1, TaskScheduler.Default);
            Task.Run(() => source.SetResult(i)).GetAwaiter().GetResult();
            checksum += continuation.GetAwaiter().GetResult();
        }

        return checksum % 10000;
    }
}
