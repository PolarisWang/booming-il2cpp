using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal static class FinallyAndFilterProofEntry
{
    private static string Execute()
    {
        string log = string.Empty;
        try
        {
            try
            {
                log += "body;";
                throw new ArgumentOutOfRangeException("value", "negative");
            }
            catch (ArgumentOutOfRangeException ex) when (ex.ParamName == "value")
            {
                log += "filter:" + ex.ParamName + ";";
            }
            finally
            {
                log += "inner-finally;";
            }
        }
        finally
        {
            log += "outer-finally;";
        }

        return log;
    }

    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "finally-filter-proof",
        CapabilityFamily = ChaosCapabilityFamily.ExceptionsAndControlFlow,
        Capability = ChaosCapabilityItem.FinallyAndFilter,
        Requires = ChaosRuntimeFeature.ExceptionFlow,
        Priority = 6)]
    public static int Run()
    {
        Assert.Equal("body;filter:value;inner-finally;outer-finally;", Execute());
        return 0;
    }
}
