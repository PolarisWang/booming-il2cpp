using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal static class SpanAndMemoryProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.InteropContract,
        Alias = "span-memory-proof",
        CapabilityFamily = ChaosCapabilityFamily.UnsafeAndLowLevel,
        Capability = ChaosCapabilityItem.SpanAndMemory,
        Requires = ChaosRuntimeFeature.SpanMemory,
        Archetype = ChaosSolutionArchetype.CoreLibReferenceSolution,
        Priority = 6)]
    public static int Run()
    {
        int[] values = [4, 8, 15, 16, 23, 42];
        Span<int> window = values.AsSpan(1, 3);
        Memory<int> memory = values.AsMemory();

        window[1] = 18;

        Assert.Equal(8, memory.Span[1]);
        Assert.Equal(18, memory.Span[2]);
        Assert.Equal(49, window[0] + window[1] + window[2]);
        return 0;
    }
}
