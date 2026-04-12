using System;
namespace OverflowOpsProof;
internal static class OverflowOpsProofEntry {
    public static int Run() {
        // Normal arithmetic (no overflow)
        Console.WriteLine("no-overflow=" + (100 + 200));   // 300
        // Overflow detection with checked
        bool caught = false;
        try { checked { int _ = int.MaxValue + 1; } }
        catch (OverflowException) { caught = true; }
        Console.WriteLine("add-ovf-caught=" + (caught ? "yes" : "no"));   // yes
        caught = false;
        try { checked { int _ = int.MinValue - 1; } }
        catch (OverflowException) { caught = true; }
        Console.WriteLine("sub-ovf-caught=" + (caught ? "yes" : "no"));   // yes
        caught = false;
        try { checked { int _ = int.MaxValue * 2; } }
        catch (OverflowException) { caught = true; }
        Console.WriteLine("mul-ovf-caught=" + (caught ? "yes" : "no"));   // yes
        // conv.ovf
        caught = false;
        try { checked { sbyte _ = (sbyte)200; } }
        catch (OverflowException) { caught = true; }
        Console.WriteLine("conv-ovf-caught=" + (caught ? "yes" : "no"));  // yes
        return 0;
    }
}
internal static class Program { public static int Main() => OverflowOpsProofEntry.Run(); }
