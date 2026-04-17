using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal struct AbiWideningPair
{
    public int Left;
    public int Right;
}

internal static class AbiWideningHarness
{
    public static AbiWideningPair Echo(AbiWideningPair value)
    {
        return value;
    }

    public static AbiWideningPair Create(int left, int right)
    {
        return new AbiWideningPair
        {
            Left = left,
            Right = right,
        };
    }
}

internal static class AbiWideningProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "abi-widening-proof",
        CapabilityFamily = ChaosCapabilityFamily.RuntimeServices,
        Capability = ChaosCapabilityItem.RuntimeHelpers,
        Priority = 7)]
    public static int Run()
    {
        ChaosAssertState.Reset();

        AbiWideningPair seed = default;
        seed.Left = 18;
        seed.Right = 24;

        var echoed = AbiWideningHarness.Echo(seed);
        var created = AbiWideningHarness.Create(echoed.Left, echoed.Right);

        Assert.Equal(18, created.Left);
        Assert.Equal(24, created.Right);
        Assert.Equal(42, created.Left + created.Right);
        return ChaosAssertState.Complete();
    }
}
