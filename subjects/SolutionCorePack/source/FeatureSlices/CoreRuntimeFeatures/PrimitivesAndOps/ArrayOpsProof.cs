using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal static class ArrayOpsProofEntry
{
    private static void AddTo(ref int target, int delta)
    {
        target = target + delta;
    }

    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "array-ops-proof",
        CapabilityFamily = ChaosCapabilityFamily.PrimitivesAndOps,
        Capability = ChaosCapabilityItem.ArrayBasics,
        Priority = 2)]
    public static int Run()
    {
        ChaosAssertState.Reset();
        int[] values = new int[3];
        values[0] = 7;
        values[1] = 11;
        ref var tail = ref values[2];
        tail = values[0] + values[1];
        AddTo(ref tail, 24);
        Assert.Equal(3, values.Length);
        Assert.Equal(42, values[2]);
        Assert.Equal(42, tail);
        return ChaosAssertState.Complete();
    }
}
