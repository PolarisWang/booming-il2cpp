using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal static class FunctionPointerProofEntry
{
    private static int MultiplyAndOffset(int left, int right)
    {
        return (left * right) + 2;
    }

    [ChaosUnitTest(
        ChaosUnitCategory.InteropContract,
        Alias = "function-pointer-proof",
        CapabilityFamily = ChaosCapabilityFamily.UnsafeAndLowLevel,
        Capability = ChaosCapabilityItem.FunctionPointer,
        Requires = ChaosRuntimeFeature.FunctionPointer,
        Archetype = ChaosSolutionArchetype.CoreLibReferenceSolution,
        Priority = 6)]
    public static unsafe int Run()
    {
        delegate* managed<int, int, int> pointer = &MultiplyAndOffset;
        int result = pointer(5, 8);

        Assert.Equal(42, result);
        return 0;
    }
}
