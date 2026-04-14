using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal ref struct StackWindow
{
    private readonly Span<int> _values;

    public StackWindow(Span<int> values)
    {
        _values = values;
    }

    public int Sum()
    {
        int total = 0;
        foreach (int value in _values)
        {
            total += value;
        }

        return total;
    }
}

internal static class RefStructAndStackallocProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.InteropContract,
        Alias = "ref-struct-stackalloc-proof",
        CapabilityFamily = ChaosCapabilityFamily.UnsafeAndLowLevel,
        Capability = ChaosCapabilityItem.RefStructAndStackalloc,
        Requires = ChaosRuntimeFeature.RefStructStackalloc | ChaosRuntimeFeature.SpanMemory,
        Archetype = ChaosSolutionArchetype.CoreLibReferenceSolution,
        Priority = 6)]
    public static int Run()
    {
        Span<int> buffer = stackalloc int[] { 9, 10, 11, 12 };
        var window = new StackWindow(buffer);

        Assert.Equal(42, window.Sum());
        return 0;
    }
}
