using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal static class ThreadingProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "threading-proof",
        CapabilityFamily = ChaosCapabilityFamily.ThreadingAndSynchronization,
        Capability = ChaosCapabilityItem.Threading,
        Requires = ChaosRuntimeFeature.Threading,
        Priority = 3)]
    public static int Run()
    {
        var result = 0;
        string? workerName = null;
        var thread = new Thread(() =>
        {
            workerName = Thread.CurrentThread.Name;
            result = 19 + 23;
        })
        {
            Name = "threading-proof-worker",
        };

        thread.Start();
        thread.Join();

        Assert.Equal(42, result);
        Assert.Equal("threading-proof-worker", workerName);
        return 0;
    }
}
