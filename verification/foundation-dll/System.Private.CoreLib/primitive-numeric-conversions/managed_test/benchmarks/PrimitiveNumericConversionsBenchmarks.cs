using System;
using System.Diagnostics;

// Auto-generated benchmark harness for PrimitiveNumericConversionsTests
// Runs 2 methods as quick smoke-benchmark.
class Program
{
    static void Main()
    {
        // Benchmark entry 0: Benchmark_System_Private_CoreLib_System_Convert_ToString_System_String_System_Int32
        var sw0 = Stopwatch.StartNew();
        _ = Convert.ToString(42);
        sw0.Stop();
        Console.WriteLine($"Entry 0: {sw0.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 1: Benchmark_System_Private_CoreLib_System_Convert_ToString_System_String_System_Double
        var sw1 = Stopwatch.StartNew();
        _ = Convert.ToString(42.0);
        sw1.Stop();
        Console.WriteLine($"Entry 1: {sw1.Elapsed.TotalMilliseconds:F3} ms");
    }
}
