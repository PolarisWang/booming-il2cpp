using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal static class BoxingSink
{
    public static object? LastValue;

    public static void Consume(object value)
    {
        LastValue = value;
    }
}

internal static class ArrayBoxingProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "array-boxing-proof",
        CapabilityFamily = ChaosCapabilityFamily.PrimitivesAndOps,
        Capability = ChaosCapabilityItem.ArrayBoxing,
        Priority = 4)]
    public static int Run()
    {
        ChaosAssertState.Reset();
        BoxingSink.Consume(42);
        object[] values = new object[1];
        values[0] = 42;
        var actual = (int)values[0];
        var echoed = (int)BoxingSink.LastValue!;
        Assert.Equal(42, actual);
        Assert.Equal(42, echoed);
        return ChaosAssertState.Complete();
    }
}
