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
            Console.WriteLine("Exception finally proof.");
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
        Evidence = ChaosEvidenceKind.Stdout,
        Priority = 6)]
    public static int Run()
    {
        var thrower = new ExceptionThrower();
        Console.WriteLine(thrower.Capture());
        return 0;
    }
}
