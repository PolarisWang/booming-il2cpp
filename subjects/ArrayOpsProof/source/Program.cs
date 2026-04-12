using System;
namespace ArrayOpsProof;
internal static class ArrayOpsProofEntry {
    public static int Run() {
        int[] arr = new int[5];
        for (int i = 0; i < arr.Length; i++) arr[i] = i * 10;
        Console.WriteLine("arr-len=" + arr.Length);    // 5
        Console.WriteLine("arr-0=" + arr[0]);          // 0
        Console.WriteLine("arr-4=" + arr[4]);          // 40
        string[] strs = { "hello", "world" };
        Console.WriteLine("arr-str-0=" + strs[0]);     // hello
        Console.WriteLine("arr-str-1=" + strs[1]);     // world
        double[] doubles = new double[3];
        doubles[1] = 3.14;
        Console.WriteLine("arr-dbl-1=" + doubles[1].ToString("F2")); // 3.14
        return 0;
    }
}
internal static class Program { public static int Main() => ArrayOpsProofEntry.Run(); }
