using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal static class BoxingSink
{
    public static void Consume(object value)
    {
        _ = value;
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
        BoxingSink.Consume(42);
        var message = new FeatureBanner[] { new("array boxing proof") }[0].BuildMessage();
        Assert.Equal("Core runtime proof: array boxing proof.", message);
        return 0;
    }
}
