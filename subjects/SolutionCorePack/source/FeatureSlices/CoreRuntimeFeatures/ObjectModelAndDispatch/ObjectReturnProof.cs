using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal sealed class ObjectReturnBox
{
    public int Value;
}

internal static class ObjectReturnFactory
{
    public static ObjectReturnBox Create()
    {
        return new ObjectReturnBox
        {
            Value = 42,
        };
    }
}

internal static class ObjectReturnProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "object-return-proof",
        CapabilityFamily = ChaosCapabilityFamily.RuntimeServices,
        Capability = ChaosCapabilityItem.RuntimeHelpers,
        Priority = 3)]
    public static int Run()
    {
        ChaosAssertState.Reset();
        var box = ObjectReturnFactory.Create();
        Assert.Equal(42, box.Value);
        return ChaosAssertState.Complete();
    }
}
