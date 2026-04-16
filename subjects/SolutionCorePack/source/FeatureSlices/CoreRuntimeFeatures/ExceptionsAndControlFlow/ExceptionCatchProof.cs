using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal sealed class ExceptionCatchHarness
{
    public int Capture()
    {
        try
        {
            ThrowNow();
            return -1;
        }
        catch (InvalidOperationException)
        {
            return 42;
        }
    }

    private static void ThrowNow()
    {
        throw new InvalidOperationException();
    }
}

internal static class ExceptionCatchProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "exception-catch-proof",
        CapabilityFamily = ChaosCapabilityFamily.ExceptionsAndControlFlow,
        Capability = ChaosCapabilityItem.ExceptionBasic,
        Requires = ChaosRuntimeFeature.ExceptionFlow,
        Priority = 6)]
    public static int Run()
    {
        ChaosAssertState.Reset();
        var value = new ExceptionCatchHarness().Capture();
        Assert.Equal(42, value);
        return ChaosAssertState.Complete();
    }
}
