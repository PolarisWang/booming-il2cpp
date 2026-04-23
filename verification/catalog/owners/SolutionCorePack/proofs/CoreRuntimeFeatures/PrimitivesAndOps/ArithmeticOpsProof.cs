using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal static class ArithmeticOpsProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "arithmetic-ops-proof",
        CapabilityFamily = ChaosCapabilityFamily.PrimitivesAndOps,
        Capability = ChaosCapabilityItem.ArithmeticOps,
        Priority = 2)]
    public static int Run()
    {
        var left = 19;
        var right = 23;
        var sum = left + right;
        var difference = sum - 5;
        var quotient = difference / 3;
        var remainder = difference % 5;
        var mixed = (sum * 2) - (right / 2);

        Assert.Equal(42, sum);
        Assert.Equal(37, difference);
        Assert.Equal(12, quotient);
        Assert.Equal(2, remainder);
        Assert.Equal(73, mixed);
        return 0;
    }
}
