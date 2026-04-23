using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal static class UnsafePointerProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.InteropContract,
        Alias = "unsafe-pointer-proof",
        CapabilityFamily = ChaosCapabilityFamily.UnsafeAndLowLevel,
        Capability = ChaosCapabilityItem.UnsafePointer,
        Requires = ChaosRuntimeFeature.UnsafePointer,
        Archetype = ChaosSolutionArchetype.CoreLibReferenceSolution,
        Priority = 6)]
    public static unsafe int Run()
    {
        int[] values = [3, 5, 7];

        fixed (int* pointer = values)
        {
            pointer[1] = 32;
            Assert.Equal(42, pointer[0] + pointer[1] + pointer[2]);
        }

        Assert.Equal(32, values[1]);
        return 0;
    }
}
