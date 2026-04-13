using Chaos.TestFramework;

namespace MainlineFeaturePack;

internal static class BranchOpsProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "branch-ops-proof",
        Evidence = ChaosEvidenceKind.Stdout,
        Priority = 2)]
    public static int Run()
    {
        int a = 5;
        int b = 10;
        Console.WriteLine("branch-eq=" + (a == b ? "yes" : "no"));
        Console.WriteLine("branch-ne=" + (a != b ? "yes" : "no"));
        Console.WriteLine("branch-lt=" + (a < b ? "yes" : "no"));
        Console.WriteLine("branch-gt=" + (a > b ? "yes" : "no"));
        Console.WriteLine("branch-le=" + (a <= b ? "yes" : "no"));
        Console.WriteLine("branch-ge=" + (a >= b ? "yes" : "no"));
        uint ua = 5;
        uint ub = 10;
        Console.WriteLine("branch-lt-un=" + (ua < ub ? "yes" : "no"));
        return 0;
    }
}
