using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal sealed class DirectCallVirtCarrier
{
    private readonly int _value;

    public DirectCallVirtCarrier(int value)
    {
        _value = value;
    }

    public int ReadValue()
    {
        return _value;
    }
}

internal static class DirectCallVirtProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "direct-callvirt-proof",
        CapabilityFamily = ChaosCapabilityFamily.RuntimeServices,
        Capability = ChaosCapabilityItem.RuntimeHelpers,
        Priority = 2)]
    public static int Run()
    {
        ChaosAssertState.Reset();
        var carrier = new DirectCallVirtCarrier(42);
        Assert.Equal(42, carrier.ReadValue());
        return ChaosAssertState.Complete();
    }
}
