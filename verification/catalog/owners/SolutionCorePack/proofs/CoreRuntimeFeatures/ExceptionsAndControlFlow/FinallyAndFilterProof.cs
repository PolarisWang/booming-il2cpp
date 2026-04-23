using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal static class FinallyAndFilterProofEntry
{
    private static int Execute()
    {
        var state = 0;
        try
        {
            try
            {
                state = (state * 10) + 1;
                throw new ArgumentOutOfRangeException();
            }
            catch (ArgumentOutOfRangeException ex) when (ex is not null)
            {
                state = (state * 10) + 2;
            }
            finally
            {
                state = (state * 10) + 3;
            }
        }
        finally
        {
            state = (state * 10) + 4;
        }

        return state;
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
        Assert.Equal(1234, Execute());
        return 0;
    }
}
