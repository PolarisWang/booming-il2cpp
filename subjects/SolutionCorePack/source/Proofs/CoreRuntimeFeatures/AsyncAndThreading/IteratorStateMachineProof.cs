using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal static class IteratorStateMachineProofEntry
{
    private static IEnumerable<int> EnumerateSequence(bool includeTail)
    {
        yield return 1;
        yield return 2;
        if (!includeTail)
        {
            yield break;
        }

        yield return 3;
    }

    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "iterator-state-machine-proof",
        CapabilityFamily = ChaosCapabilityFamily.AsyncAndIterators,
        Capability = ChaosCapabilityItem.IteratorStateMachine,
        Priority = 3)]
    public static int Run()
    {
        var full = string.Join(",", EnumerateSequence(includeTail: true));
        var partial = string.Join(",", EnumerateSequence(includeTail: false));
        Assert.Equal("1,2,3", full);
        Assert.Equal("1,2", partial);
        return 0;
    }
}
