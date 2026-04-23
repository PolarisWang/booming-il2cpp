using System.Runtime.CompilerServices;
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

    private static Task<bool> IsExpectedAsync(int seed)
    {
        var builder = AsyncTaskMethodBuilder<bool>.Create();
        builder.SetResult(seed == 40);
        return builder.Task;
    }

    private static Task<string> DescribeAsync(int seed)
    {
        var builder = AsyncTaskMethodBuilder<string>.Create();
        builder.SetResult(seed == 40
            ? "value:42"
            : "value:unexpected");
        return builder.Task;
    }

    private static Task<long> WidenAsync()
    {
        var builder = AsyncTaskMethodBuilder<long>.Create();
        builder.SetResult(4_000_000_040L);
        return builder.Task;
    }

    private static Task<double> MeasureAsync()
    {
        var builder = AsyncTaskMethodBuilder<double>.Create();
        builder.SetResult(10.5);
        return builder.Task;
    }

    private static async ValueTask<long> ComputeValueTaskLongAsync()
    {
        await Task.Yield();
        return 42L;
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
        var isExpected = IsExpectedAsync(40).GetAwaiter().GetResult();
        var description = DescribeAsync(40).GetAwaiter().GetResult();
        var widened = WidenAsync().GetAwaiter().GetResult();
        var measured = MeasureAsync().GetAwaiter().GetResult();
        var valueTaskLong = ComputeValueTaskLongAsync().GetAwaiter().GetResult();
        return result == 42
            && isExpected
            && description == "value:42"
            && widened == 4_000_000_040L
            && measured == 10.5
            && valueTaskLong == 42L
            ? 0
            : 1;
    }
}
