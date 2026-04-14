using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal sealed class NestedExceptionHarness
{
    public string Capture()
    {
        try
        {
            try
            {
                ThrowInner();
                return "unreachable-inner";
            }
            catch (InvalidOperationException ex)
            {
                _ = ex;
                return "Nested EH native proof: inner caught.";
            }
            finally
            {
            }
        }
        finally
        {
        }
    }

    private static void ThrowInner()
    {
        throw new InvalidOperationException("nested-phase2-proof");
    }
}

internal static class NestedExceptionProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "nested-exception-proof",
        CapabilityFamily = ChaosCapabilityFamily.ExceptionsAndControlFlow,
        Capability = ChaosCapabilityItem.NestedException,
        Requires = ChaosRuntimeFeature.ExceptionFlow,
        Priority = 2)]
    public static int Run()
    {
        var harness = new NestedExceptionHarness();
        var message = harness.Capture();
        Assert.Equal("Nested EH native proof: inner caught.", message);
        return 0;
    }
}
