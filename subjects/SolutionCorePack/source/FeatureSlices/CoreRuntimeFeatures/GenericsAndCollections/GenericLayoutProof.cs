using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal sealed class GenericBox<T>
{
    private readonly T _value;

    public GenericBox(T value)
    {
        _value = value;
    }

    public T Value()
    {
        return _value;
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
        var box = new GenericBox<string>(GenericEcho.Echo("Generic layout native proof."));
        var message = box.Value();
        Assert.Equal("Generic layout native proof.", message);
        return 0;
    }
}
