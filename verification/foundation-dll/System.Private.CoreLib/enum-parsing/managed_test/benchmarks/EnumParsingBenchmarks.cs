using System;
using System.Diagnostics;

// Auto-generated benchmark harness for EnumParsingTests
// Runs 1 methods as quick smoke-benchmark.
class Program
{
    static void Main()
    {
        // Benchmark entry 0: Benchmark_System_Private_CoreLib_System_Enum_ToString_System_String
        var sw0 = Stopwatch.StartNew();
        _ = DayOfWeek.Monday.ToString();
        sw0.Stop();
        Console.WriteLine($"Entry 0: {sw0.Elapsed.TotalMilliseconds:F3} ms");
    }
}
