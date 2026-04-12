using System;
namespace BranchOpsProof;
internal static class BranchOpsProofEntry {
    public static int Run() {
        int a = 5, b = 10;
        Console.WriteLine("branch-eq="  + (a == b ? "yes" : "no"));   // no
        Console.WriteLine("branch-ne="  + (a != b ? "yes" : "no"));   // yes
        Console.WriteLine("branch-lt="  + (a < b  ? "yes" : "no"));   // yes
        Console.WriteLine("branch-gt="  + (a > b  ? "yes" : "no"));   // no
        Console.WriteLine("branch-le="  + (a <= b ? "yes" : "no"));   // yes
        Console.WriteLine("branch-ge="  + (a >= b ? "yes" : "no"));   // no
        uint ua = 5, ub = 10;
        Console.WriteLine("branch-lt-un=" + (ua < ub ? "yes" : "no")); // yes
        return 0;
    }
}
internal static class Program { public static int Main() => BranchOpsProofEntry.Run(); }
