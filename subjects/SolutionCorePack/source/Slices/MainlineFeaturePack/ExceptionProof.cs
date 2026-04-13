using Chaos.TestFramework;

namespace MainlineFeaturePack;

internal sealed class ExceptionThrower
{
    public ExceptionThrower()
    {
    }

    public string Capture()
    {
        try
        {
            ThrowNow();
            return "unreachable";
        }
        catch (InvalidOperationException ex)
        {
            _ = ex;
            return "Exception native proof: caught.";
        }
        finally
        {
        }
    }

    private static void ThrowNow()
    {
        throw new InvalidOperationException("phase6-proof");
    }
}

internal static class ExceptionProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "exception-proof",
        Requires = ChaosRuntimeFeature.ExceptionFlow,
        Priority = 6)]
    public static int Run()
    {
        var thrower = new ExceptionThrower();
        var message = thrower.Capture();
        Assert.Equal("Exception native proof: caught.", message);
        return 0;
    }
}
