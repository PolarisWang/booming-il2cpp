using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal abstract class DispatchBase<T>
{
    public abstract T ReadValue();
}

internal sealed class DispatchLeaf<T> : DispatchBase<T>
{
    private readonly T _value;

    public DispatchLeaf(T value)
    {
        _value = value;
    }

    public override T ReadValue()
    {
        return _value;
    }
}

internal static class DispatchProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "dispatch-proof",
        CapabilityFamily = ChaosCapabilityFamily.TypeSystemAndGenerics,
        Capability = ChaosCapabilityItem.GenericVirtualDispatch,
        Priority = 2)]
    public static int Run()
    {
        ChaosAssertState.Reset();
        DispatchBase<int> instance = new DispatchLeaf<int>(42);
        var value = instance.ReadValue();
        Assert.Equal(42, value);
        return ChaosAssertState.Complete();
    }
}
