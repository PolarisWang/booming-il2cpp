using Chaos.TestFramework;

namespace MainlineFeaturePack;

internal static class OverflowOpsProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "overflow-ops-proof",
        Priority = 2)]
    public static int Run()
    {
        Assert.Equal(300, 100 + 200);
        int maxValue = int.MaxValue;
        int minValue = int.MinValue;
        int wideValue = 200;

        bool caught = false;
        try
        {
            checked
            {
                int _ = maxValue + 1;
            }
        }
        catch (OverflowException)
        {
            caught = true;
        }

        Assert.True(caught);
        caught = false;
        try
        {
            checked
            {
                int _ = minValue - 1;
            }
        }
        catch (OverflowException)
        {
            caught = true;
        }

        Assert.True(caught);
        caught = false;
        try
        {
            checked
            {
                int _ = maxValue * 2;
            }
        }
        catch (OverflowException)
        {
            caught = true;
        }

        Assert.True(caught);
        caught = false;
        try
        {
            checked
            {
                sbyte _ = (sbyte)wideValue;
            }
        }
        catch (OverflowException)
        {
            caught = true;
        }

        Assert.True(caught);
        return 0;
    }
}
