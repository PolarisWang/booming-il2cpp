using Chaos.TestFramework;

namespace MainlineFeaturePack;

internal static class BranchOpsProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "branch-ops-proof",
        Priority = 2)]
    public static int Run()
    {
        int a = 5;
        int b = 10;
        Assert.Equal("no", a == b ? "yes" : "no");
        Assert.Equal("yes", a != b ? "yes" : "no");
        Assert.Equal("yes", a < b ? "yes" : "no");
        Assert.Equal("no", a > b ? "yes" : "no");
        Assert.Equal("yes", a <= b ? "yes" : "no");
        Assert.Equal("no", a >= b ? "yes" : "no");
        uint ua = 5;
        uint ub = 10;
        Assert.Equal("yes", ua < ub ? "yes" : "no");
        return 0;
    }
}
