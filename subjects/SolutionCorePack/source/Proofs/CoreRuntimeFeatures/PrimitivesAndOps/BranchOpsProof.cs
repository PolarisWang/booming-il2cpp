using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal static class BranchOpsProofEntry
{
    private static int EqualBranch(int left, int right)
    {
        if (left == right)
        {
            return 1;
        }

        return 0;
    }

    private static int NotEqualBranch(int left, int right)
    {
        if (left != right)
        {
            return 1;
        }

        return 0;
    }

    private static int LessThanBranch(int left, int right)
    {
        if (left < right)
        {
            return 1;
        }

        return 0;
    }

    private static int GreaterThanBranch(int left, int right)
    {
        if (left > right)
        {
            return 1;
        }

        return 0;
    }

    private static int LessThanOrEqualBranch(int left, int right)
    {
        if (left <= right)
        {
            return 1;
        }

        return 0;
    }

    private static int GreaterThanOrEqualBranch(int left, int right)
    {
        if (left >= right)
        {
            return 1;
        }

        return 0;
    }

    private static int UnsignedLessThanBranch(uint left, uint right)
    {
        if (left < right)
        {
            return 1;
        }

        return 0;
    }

    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "branch-ops-proof",
        CapabilityFamily = ChaosCapabilityFamily.PrimitivesAndOps,
        Capability = ChaosCapabilityItem.ComparisonAndBranch,
        Priority = 2)]
    public static int Run()
    {
        int a = 5;
        int b = 10;
        Assert.Equal(0, EqualBranch(a, b));
        Assert.Equal(1, NotEqualBranch(a, b));
        Assert.Equal(1, LessThanBranch(a, b));
        Assert.Equal(0, GreaterThanBranch(a, b));
        Assert.Equal(1, LessThanOrEqualBranch(a, b));
        Assert.Equal(0, GreaterThanOrEqualBranch(a, b));
        uint ua = 5;
        uint ub = 10;
        Assert.Equal(1, UnsignedLessThanBranch(ua, ub));
        return 0;
    }
}
