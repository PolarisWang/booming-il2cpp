using System;
namespace ConversionOpsProof;
internal static class ConversionOpsProofEntry {
    public static int Run() {
        int i = 300;
        Console.WriteLine("conv-i1=" + (sbyte)i);       // truncate: 44
        Console.WriteLine("conv-i2=" + (short)i);       // 300
        Console.WriteLine("conv-u1=" + (byte)i);        // 44
        Console.WriteLine("conv-u2=" + (ushort)i);      // 300
        Console.WriteLine("conv-i8=" + (long)i);        // 300
        Console.WriteLine("conv-r4=" + ((float)i).ToString("F1")); // 300.0
        Console.WriteLine("conv-r8=" + ((double)i).ToString("F1")); // 300.0
        double d = 3.7;
        Console.WriteLine("conv-i4-from-r8=" + (int)d); // 3 (truncate)
        return 0;
    }
}
internal static class Program { public static int Main() => ConversionOpsProofEntry.Run(); }
