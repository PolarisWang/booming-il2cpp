using Chaos.TestFramework;

namespace MainlineFeaturePack;

internal static class AsyncAwaitProofEntry
{
    internal static async Task<int> ComputeAsync()
    {
        return await Task.FromResult(42);
    }

    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "async-await-proof",
        Evidence = ChaosEvidenceKind.Stdout,
        Priority = 2)]
    public static int Run()
    {
        Console.WriteLine(ComputeAsync().GetAwaiter().GetResult());
        return 0;
    }
}
