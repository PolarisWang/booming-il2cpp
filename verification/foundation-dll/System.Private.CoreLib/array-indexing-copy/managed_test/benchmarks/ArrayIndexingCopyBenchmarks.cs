using System;
using System.Diagnostics;

// Auto-generated benchmark harness for ArrayIndexingCopyTests
// Runs 9 methods as quick smoke-benchmark.
class Program
{
    static void Main()
    {
        // Benchmark entry 0: Benchmark_System_Private_CoreLib_System_Array_Copy_System_Void_System_Array_System_Array_System_Int32
        var sw0 = Stopwatch.StartNew();
        _ = Array.Copy(new byte[4], new byte[4], 4);
        sw0.Stop();
        Console.WriteLine($"Entry 0: {sw0.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 1: Benchmark_System_Private_CoreLib_System_Array_Copy_System_Void_System_Array_System_Int32_System_Array_System_Int32_System_Int32
        var sw1 = Stopwatch.StartNew();
        _ = Array.Copy(new byte[4], 0, new byte[4], 0, 4);
        sw1.Stop();
        Console.WriteLine($"Entry 1: {sw1.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 2: Benchmark_System_Private_CoreLib_System_Array_Clear_System_Void_System_Array_System_Int32_System_Int32
        var sw2 = Stopwatch.StartNew();
        _ = Array.Clear(new byte[4], 0, 4);
        sw2.Stop();
        Console.WriteLine($"Entry 2: {sw2.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 3: Benchmark_System_Private_CoreLib_System_Array_Sort_System_Void_System_Array
        var sw3 = Stopwatch.StartNew();
        _ = Array.Sort(Array.Empty<byte>());
        sw3.Stop();
        Console.WriteLine($"Entry 3: {sw3.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 4: Benchmark_System_Private_CoreLib_System_Array_Sort_System_Void_System_Array_System_Collections_IComparer
        var sw4 = Stopwatch.StartNew();
        _ = Array.Sort<byte>(new byte[1], (IComparer<byte>)null!);
        sw4.Stop();
        Console.WriteLine($"Entry 4: {sw4.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 5: Benchmark_System_Private_CoreLib_System_Array_BinarySearch_System_Int32_System_Array_System_Object
        var sw5 = Stopwatch.StartNew();
        _ = Array.BinarySearch(new byte[4], (byte)42);
        sw5.Stop();
        Console.WriteLine($"Entry 5: {sw5.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 6: Benchmark_System_Private_CoreLib_System_Array_BinarySearch_System_Int32_System_Array_System_Int32_System_Int32_System_Object
        var sw6 = Stopwatch.StartNew();
        _ = Array.BinarySearch(new byte[4], 0, 4, (byte)42);
        sw6.Stop();
        Console.WriteLine($"Entry 6: {sw6.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 7: Benchmark_System_Private_CoreLib_System_Array_IndexOf_System_Int32_System_Array_System_Object
        var sw7 = Stopwatch.StartNew();
        _ = Array.IndexOf(new byte[4], (byte)42);
        sw7.Stop();
        Console.WriteLine($"Entry 7: {sw7.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 8: Benchmark_System_Private_CoreLib_System_Array_LastIndexOf_System_Int32_System_Array_System_Object
        var sw8 = Stopwatch.StartNew();
        _ = Array.LastIndexOf(new byte[4], (byte)42);
        sw8.Stop();
        Console.WriteLine($"Entry 8: {sw8.Elapsed.TotalMilliseconds:F3} ms");
    }
}
