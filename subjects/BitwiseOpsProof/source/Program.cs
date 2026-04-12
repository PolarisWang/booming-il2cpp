using System;
namespace BitwiseOpsProof;
internal static class BitwiseOpsProofEntry {
    public static int Run() {
        int a = 0b1010, b = 0b1100;
        Console.WriteLine("bit-and=" + (a & b));   // 8 (0b1000)
        Console.WriteLine("bit-or="  + (a | b));   // 14 (0b1110)
        Console.WriteLine("bit-xor=" + (a ^ b));   // 6 (0b0110)
        Console.WriteLine("bit-not=" + (~a));       // -11
        Console.WriteLine("bit-shl=" + (a << 2));  // 40
        Console.WriteLine("bit-shr=" + (a >> 1));  // 5
        int neg = -8;
        Console.WriteLine("bit-shr-un=" + (int)((uint)neg >> 1)); // logical shift
        return 0;
    }
}
internal static class Program { public static int Main() => BitwiseOpsProofEntry.Run(); }
