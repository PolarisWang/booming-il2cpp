using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal static class OverflowOpsProofEntry
{
    private static int CheckedAddOverflow()
    {
        int maxValue = int.MaxValue;
        try
        {
            checked
            {
                int _ = maxValue + 1;
            }

            return 0;
        }
        catch (OverflowException)
        {
            return 1;
        }
    }

    private static int CheckedSubtractOverflow()
    {
        int minValue = int.MinValue;
        try
        {
            checked
            {
                int _ = minValue - 1;
            }

            return 0;
        }
        catch (OverflowException)
        {
            return 1;
        }
    }

    private static int CheckedMultiplyOverflow()
    {
        int maxValue = int.MaxValue;
        try
        {
            checked
            {
                int _ = maxValue * 2;
            }

            return 0;
        }
        catch (OverflowException)
        {
            return 1;
        }
    }

    private static int CheckedSByteConversionOverflow()
    {
        int wideValue = 200;
        try
        {
            checked
            {
                sbyte _ = (sbyte)wideValue;
            }

            return 0;
        }
        catch (OverflowException)
        {
            return 1;
        }
    }

    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "overflow-ops-proof",
        CapabilityFamily = ChaosCapabilityFamily.PrimitivesAndOps,
        Capability = ChaosCapabilityItem.OverflowBehavior,
        Priority = 2)]
    public static int Run()
    {
        Assert.Equal(300, 100 + 200);
        Assert.Equal(1, CheckedAddOverflow());
        Assert.Equal(1, CheckedSubtractOverflow());
        Assert.Equal(1, CheckedMultiplyOverflow());
        Assert.Equal(1, CheckedSByteConversionOverflow());
        return 0;
    }
}
