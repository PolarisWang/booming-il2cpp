using System;
using System.Diagnostics;

// Auto-generated benchmark harness for CollectionsGenericCoreTests
// Runs 4 methods as quick smoke-benchmark.
class Program
{
    static void Main()
    {
        // Benchmark entry 0: Benchmark_System_Private_CoreLib_System_Collections_Generic_List_1_Clear_System_Void
        var sw0 = Stopwatch.StartNew();
        _ = new List<int>().Clear();
        sw0.Stop();
        Console.WriteLine($"Entry 0: {sw0.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 1: Benchmark_System_Private_CoreLib_System_Collections_Generic_List_1_Sort_System_Void
        var sw1 = Stopwatch.StartNew();
        _ = new List<int>().Sort();
        sw1.Stop();
        Console.WriteLine($"Entry 1: {sw1.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 2: Benchmark_System_Private_CoreLib_System_Collections_Generic_List_1_ToArray_T
        var sw2 = Stopwatch.StartNew();
        _ = new List<int>().ToArray();
        sw2.Stop();
        Console.WriteLine($"Entry 2: {sw2.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 3: Benchmark_System_Private_CoreLib_System_Collections_Generic_Dictionary_2_get_Count_System_Int32
        var sw3 = Stopwatch.StartNew();
        _ = new Dictionary<string, int>().Count;
        sw3.Stop();
        Console.WriteLine($"Entry 3: {sw3.Elapsed.TotalMilliseconds:F3} ms");
    }
}
