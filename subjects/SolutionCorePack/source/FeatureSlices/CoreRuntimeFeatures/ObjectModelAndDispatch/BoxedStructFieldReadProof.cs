using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal struct BoxedStructFieldReadState
{
    public int Value;
}

internal static class BoxedStructFieldReadProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "boxed-struct-field-read-proof",
        CapabilityFamily = ChaosCapabilityFamily.RuntimeServices,
        Capability = ChaosCapabilityItem.RuntimeHelpers,
        Priority = 2)]
    public static int Run()
    {
        ChaosAssertState.Reset();
        object boxed = new BoxedStructFieldReadState { Value = 42 };
        var actual = ((BoxedStructFieldReadState)boxed).Value;
        var copied = (BoxedStructFieldReadState)boxed;
        BoxedStructFieldReadState echoed = default;
        ref var echoedAlias = ref echoed;
        echoedAlias = copied;
        var roundtrip = echoedAlias;
        Assert.Equal(42, actual);
        Assert.Equal(42, copied.Value);
        Assert.Equal(42, echoed.Value);
        Assert.Equal(42, roundtrip.Value);
        return ChaosAssertState.Complete();
    }
}
