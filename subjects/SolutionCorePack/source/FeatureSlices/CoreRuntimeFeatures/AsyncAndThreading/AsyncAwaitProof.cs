using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal static class AsyncAwaitProofEntry
{
    internal static async Task<int> ComputeAsync()
    {
        return await Task.FromResult(42);
    }

    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "async-await-proof",
        CapabilityFamily = ChaosCapabilityFamily.AsyncAndIterators,
        Capability = ChaosCapabilityItem.AsyncAwait,
        Priority = 2)]
    public static int Run()
    {
        var result = ComputeAsync().GetAwaiter().GetResult();
        Assert.Equal(42, result);
        return 0;
    }
}
