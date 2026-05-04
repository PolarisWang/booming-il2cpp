using System;
using System.Diagnostics;

// Auto-generated benchmark harness for TimeDateTimeTimespanTests
// Runs 8 methods as quick smoke-benchmark.
class Program
{
    static void Main()
    {
        // Benchmark entry 0: Benchmark_System_Private_CoreLib_System_DateTime_ctor_System_Void_System_Int32_System_Int32_System_Int32
        var sw0 = Stopwatch.StartNew();
        _ = new DateTime(2024, 1, 1);
        sw0.Stop();
        Console.WriteLine($"Entry 0: {sw0.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 1: Benchmark_System_Private_CoreLib_System_DateTime_ctor_System_Void_System_Int32_System_Int32_System_Int32_System_Int32_System_Int32
        var sw1 = Stopwatch.StartNew();
        _ = new DateTime(2024, 1, 1, 0, 0, 0);
        sw1.Stop();
        Console.WriteLine($"Entry 1: {sw1.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 2: Benchmark_System_Private_CoreLib_System_DateTime_ToString_System_String
        var sw2 = Stopwatch.StartNew();
        _ = DateTime.UtcNow.ToString();
        sw2.Stop();
        Console.WriteLine($"Entry 2: {sw2.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 3: Benchmark_System_Private_CoreLib_System_DateTime_ToString_System_String_System_String
        var sw3 = Stopwatch.StartNew();
        _ = DateTime.UtcNow.ToString("hello");
        sw3.Stop();
        Console.WriteLine($"Entry 3: {sw3.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 4: Benchmark_System_Private_CoreLib_System_DateTime_AddDays_System_DateTime_System_Double
        var sw4 = Stopwatch.StartNew();
        _ = DateTime.UtcNow.AddDays(42.0);
        sw4.Stop();
        Console.WriteLine($"Entry 4: {sw4.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 5: Benchmark_System_Private_CoreLib_System_DateTime_AddHours_System_DateTime_System_Double
        var sw5 = Stopwatch.StartNew();
        _ = DateTime.UtcNow.AddHours(42.0);
        sw5.Stop();
        Console.WriteLine($"Entry 5: {sw5.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 6: Benchmark_System_Private_CoreLib_System_DateTime_AddMinutes_System_DateTime_System_Double
        var sw6 = Stopwatch.StartNew();
        _ = DateTime.UtcNow.AddMinutes(42.0);
        sw6.Stop();
        Console.WriteLine($"Entry 6: {sw6.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 7: Benchmark_System_Private_CoreLib_System_DateTime_Compare_System_Int32_System_DateTime_System_DateTime
        var sw7 = Stopwatch.StartNew();
        _ = DateTime.Compare(DateTime.UtcNow, DateTime.UtcNow);
        sw7.Stop();
        Console.WriteLine($"Entry 7: {sw7.Elapsed.TotalMilliseconds:F3} ms");
    }
}
