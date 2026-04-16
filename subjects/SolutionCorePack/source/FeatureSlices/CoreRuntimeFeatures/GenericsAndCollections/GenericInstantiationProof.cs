using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal sealed class GenericInstantiationBox<T>
{
    public GenericInstantiationBox(T value)
    {
        Value = value;
    }

    public T Value { get; }
}

internal static class GenericInstantiationProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "generic-instantiation-proof",
        CapabilityFamily = ChaosCapabilityFamily.TypeSystemAndGenerics,
        Capability = ChaosCapabilityItem.GenericInstantiation,
        Requires = ChaosRuntimeFeature.GenericSharing,
        Priority = 2)]
    public static int Run()
    {
        var intBox = new GenericInstantiationBox<int>(21);
        var stringBox = new GenericInstantiationBox<string>("ok");
        var pair = CreatePair<int, string>(intBox.Value * 2, stringBox.Value);

        Assert.Equal(42, pair.Left);
        Assert.Equal("ok", pair.Right);
        return 0;
    }

    private static (TLeft Left, TRight Right) CreatePair<TLeft, TRight>(TLeft left, TRight right)
    {
        return (left, right);
    }
}
