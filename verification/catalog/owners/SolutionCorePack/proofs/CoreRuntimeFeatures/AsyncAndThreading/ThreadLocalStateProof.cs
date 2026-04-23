using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal static class ThreadLocalStateProofEntry
{
    [ThreadStatic]
    private static string? ThreadLabel;

    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "thread-local-state-proof",
        CapabilityFamily = ChaosCapabilityFamily.ThreadingAndSynchronization,
        Capability = ChaosCapabilityItem.ThreadLocalState,
        Requires = ChaosRuntimeFeature.Threading,
        Priority = 4)]
    public static int Run()
    {
        string? workerBefore = "unassigned";
        string? workerAfter = "unassigned";

        ThreadLabel = "main-thread";
        var worker = new Thread(() =>
        {
            workerBefore = ThreadLabel;
            ThreadLabel = "worker-thread";
            workerAfter = ThreadLabel;
        });

        worker.Start();
        worker.Join();

        Assert.Equal("main-thread", ThreadLabel);
        Assert.True(workerBefore is null);
        Assert.Equal("worker-thread", workerAfter);
        return 0;
    }
}
