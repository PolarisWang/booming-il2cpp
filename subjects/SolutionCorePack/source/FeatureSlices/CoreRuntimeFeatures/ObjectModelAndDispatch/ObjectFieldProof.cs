using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal sealed class ObjectFieldBox
{
    public int Value;
}

internal static class ObjectFieldState
{
    public static int Counter;
}

internal static class ObjectFieldProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "object-field-proof",
        CapabilityFamily = ChaosCapabilityFamily.RuntimeServices,
        Capability = ChaosCapabilityItem.RuntimeHelpers,
        Priority = 1)]
    public static int Run()
    {
        ChaosAssertState.Reset();
        var box = new ObjectFieldBox();
        box.Value = 42;
        ObjectFieldState.Counter = box.Value;
        Assert.Equal(42, box.Value);
        Assert.Equal(42, ObjectFieldState.Counter);
        return ChaosAssertState.Complete();
    }
}
