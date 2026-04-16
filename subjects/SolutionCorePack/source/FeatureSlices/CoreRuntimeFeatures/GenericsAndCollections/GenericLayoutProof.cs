using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal sealed class GenericBox<T>
{
    public readonly T Value;

    public GenericBox(T value)
    {
        Value = value;
    }
}

internal static class GenericEcho
{
    public static T Echo<T>(T value)
    {
        return value;
    }
}

internal static class GenericLayoutProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "generic-layout-proof",
        CapabilityFamily = ChaosCapabilityFamily.TypeSystemAndGenerics,
        Capability = ChaosCapabilityItem.GenericLayout,
        Requires = ChaosRuntimeFeature.GenericSharing,
        Priority = 3)]
    public static int Run()
    {
        ChaosAssertState.Reset();
        var box = new GenericBox<int>(GenericEcho.Echo(42));
        var value = box.Value;
        Assert.Equal(42, value);
        return ChaosAssertState.Complete();
    }
}
