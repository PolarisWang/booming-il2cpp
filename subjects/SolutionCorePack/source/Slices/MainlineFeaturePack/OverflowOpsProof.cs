using Chaos.TestFramework;

namespace MainlineFeaturePack;

internal static class OverflowOpsProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "overflow-ops-proof",
        Evidence = ChaosEvidenceKind.Stdout,
        Priority = 2)]
    public static int Run()
    {
        Console.WriteLine("no-overflow=" + (100 + 200));
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

        Console.WriteLine("add-ovf-caught=" + (caught ? "yes" : "no"));
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

        Console.WriteLine("sub-ovf-caught=" + (caught ? "yes" : "no"));
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

        Console.WriteLine("mul-ovf-caught=" + (caught ? "yes" : "no"));
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

        Console.WriteLine("conv-ovf-caught=" + (caught ? "yes" : "no"));
        return 0;
    }
}
