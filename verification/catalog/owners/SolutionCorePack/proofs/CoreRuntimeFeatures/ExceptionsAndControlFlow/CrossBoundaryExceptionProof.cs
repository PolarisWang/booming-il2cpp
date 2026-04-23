using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

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
        CapabilityFamily = ChaosCapabilityFamily.ExceptionsAndControlFlow,
        Capability = ChaosCapabilityItem.CrossBoundaryException,
        Requires = ChaosRuntimeFeature.ExceptionFlow,
        Priority = 6)]
    public static int Run()
    {
        var caught = CrossBoundaryProofMethods.CatchFromCallee("aot");
        var filterOk = CrossBoundaryProofMethods.CatchWithCondition(5);
        var filterCaught = CrossBoundaryProofMethods.CatchWithCondition(-1);
        var finallyThrow = CrossBoundaryProofMethods.NestedFinally(true);
        var finallyOk = CrossBoundaryProofMethods.NestedFinally(false);

        Assert.Equal("caught:from-aot", caught);
        Assert.Equal("ok:5", filterOk);
        Assert.True(filterCaught.StartsWith("filtered:negative", StringComparison.Ordinal));
        Assert.True(filterCaught.Contains("value", StringComparison.Ordinal));
        Assert.Equal("inner-try;inner-finally;outer-catch:nested;outer-finally;", finallyThrow);
        Assert.Equal("inner-try;no-throw;inner-finally;outer-finally;", finallyOk);

        return 0;
    }
}
