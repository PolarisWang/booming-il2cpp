using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal static class AsyncAwaitProofEntry
{
    private static async Task<int> ComputeAsync(int seed)
    {
        await Task.Yield();
        return await AddAsync(seed, 2);
    }

    private static async Task<int> AddAsync(int left, int right)
    {
        await Task.Yield();
        return left + right;
    }

    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "async-await-proof",
        CapabilityFamily = ChaosCapabilityFamily.AsyncAndIterators,
        Capability = ChaosCapabilityItem.AsyncAwait,
        Requires = ChaosRuntimeFeature.AsyncStateMachine,
        Priority = 3)]
    public static int Run()
    {
        var result = ComputeAsync(40).GetAwaiter().GetResult();
        Assert.Equal(42, result);
        return 0;
    }
}
