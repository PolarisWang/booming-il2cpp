using Chaos.TestFramework;

namespace MainlineFeaturePack;

internal static class CrossBoundaryProofMethods
{
    public static void ThrowKnownException(string message)
    {
        throw new InvalidOperationException(message);
    }

    public static string CatchFromCallee(string tag)
    {
        try
        {
            ThrowKnownException("from-" + tag);
            return "unreachable";
        }
        catch (InvalidOperationException ex)
        {
            return "caught:" + ex.Message;
        }
    }

    public static string CatchWithCondition(int value)
    {
        try
        {
            if (value < 0)
            {
                throw new ArgumentOutOfRangeException(nameof(value), "negative");
            }

            return "ok:" + value;
        }
        catch (ArgumentOutOfRangeException ex) when (ex.ParamName == "value")
        {
            return "filtered:" + ex.Message;
        }
    }

    public static string NestedFinally(bool shouldThrow)
    {
        string log = string.Empty;
        try
        {
            try
            {
                log += "inner-try;";
                if (shouldThrow)
                {
                    throw new Exception("nested");
                }

                log += "no-throw;";
            }
            finally
            {
                log += "inner-finally;";
            }
        }
        catch (Exception ex)
        {
            log += "outer-catch:" + ex.Message + ";";
        }
        finally
        {
            log += "outer-finally;";
        }

        return log;
    }
}

internal static class CrossBoundaryExceptionProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "cross-boundary-exception-proof",
        Requires = ChaosRuntimeFeature.ExceptionFlow,
        Evidence = ChaosEvidenceKind.Stdout,
        Priority = 6)]
    public static int Run()
    {
        Console.WriteLine("cross-ex-catch=" + CrossBoundaryProofMethods.CatchFromCallee("aot"));
        Console.WriteLine("cross-ex-filter-ok=" + CrossBoundaryProofMethods.CatchWithCondition(5));
        Console.WriteLine("cross-ex-filter-caught=" + CrossBoundaryProofMethods.CatchWithCondition(-1));
        Console.WriteLine("cross-ex-finally-throw=" + CrossBoundaryProofMethods.NestedFinally(true));
        Console.WriteLine("cross-ex-finally-ok=" + CrossBoundaryProofMethods.NestedFinally(false));
        return 0;
    }
}
