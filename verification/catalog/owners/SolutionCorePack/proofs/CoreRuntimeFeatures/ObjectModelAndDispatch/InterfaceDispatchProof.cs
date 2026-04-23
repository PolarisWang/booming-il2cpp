using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal interface IDispatchValue<T>
{
    T ReadValue();
}

internal sealed class InterfaceValue<T> : IDispatchValue<T>
{
    private readonly T _value;

    public InterfaceValue(T value)
    {
        _value = value;
    }

    public T ReadValue()
    {
        return _value;
    }
}

internal static class InterfaceDispatchProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "interface-dispatch-proof",
        CapabilityFamily = ChaosCapabilityFamily.TypeSystemAndGenerics,
        Capability = ChaosCapabilityItem.GenericInterfaceDispatch,
        Priority = 3)]
    public static int Run()
    {
        IDispatchValue<int> value = new InterfaceValue<int>(21);
        Assert.Equal(21, value.ReadValue());
        return 0;
    }
}
