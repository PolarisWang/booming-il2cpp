using System;
using System.Diagnostics;

// Auto-generated benchmark harness for MathNumericsTests
// Runs 10 methods as quick smoke-benchmark.
class Program
{
    static void Main()
    {
        // Benchmark entry 0: Benchmark_System_Private_CoreLib_System_Math_Abs_System_Int32_System_Int32
        var sw0 = Stopwatch.StartNew();
        _ = Math.Abs(42);
        sw0.Stop();
        Console.WriteLine($"Entry 0: {sw0.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 1: Benchmark_System_Private_CoreLib_System_Math_Abs_System_Double_System_Double
        var sw1 = Stopwatch.StartNew();
        _ = Math.Abs(42.0);
        sw1.Stop();
        Console.WriteLine($"Entry 1: {sw1.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 2: Benchmark_System_Private_CoreLib_System_Math_Ceiling_System_Double_System_Double
        var sw2 = Stopwatch.StartNew();
        _ = Math.Ceiling(42.0);
        sw2.Stop();
        Console.WriteLine($"Entry 2: {sw2.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 3: Benchmark_System_Private_CoreLib_System_Math_Floor_System_Double_System_Double
        var sw3 = Stopwatch.StartNew();
        _ = Math.Floor(42.0);
        sw3.Stop();
        Console.WriteLine($"Entry 3: {sw3.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 4: Benchmark_System_Private_CoreLib_System_Math_Round_System_Double_System_Double
        var sw4 = Stopwatch.StartNew();
        _ = Math.Round(42.0);
        sw4.Stop();
        Console.WriteLine($"Entry 4: {sw4.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 5: Benchmark_System_Private_CoreLib_System_Math_Round_System_Double_System_Double_System_Int32
        var sw5 = Stopwatch.StartNew();
        _ = Math.Round(42.0);
        sw5.Stop();
        Console.WriteLine($"Entry 5: {sw5.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 6: Benchmark_System_Private_CoreLib_System_Math_Max_System_Int32_System_Int32_System_Int32
        var sw6 = Stopwatch.StartNew();
        _ = Math.Max(42, 42);
        sw6.Stop();
        Console.WriteLine($"Entry 6: {sw6.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 7: Benchmark_System_Private_CoreLib_System_Math_Max_System_Double_System_Double_System_Double
        var sw7 = Stopwatch.StartNew();
        _ = Math.Max(42.0, 42.0);
        sw7.Stop();
        Console.WriteLine($"Entry 7: {sw7.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 8: Benchmark_System_Private_CoreLib_System_Math_Min_System_Int32_System_Int32_System_Int32
        var sw8 = Stopwatch.StartNew();
        _ = Math.Min(42, 42);
        sw8.Stop();
        Console.WriteLine($"Entry 8: {sw8.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 9: Benchmark_System_Private_CoreLib_System_Math_Sqrt_System_Double_System_Double
        var sw9 = Stopwatch.StartNew();
        _ = Math.Sqrt(42.0);
        sw9.Stop();
        Console.WriteLine($"Entry 9: {sw9.Elapsed.TotalMilliseconds:F3} ms");
    }
}
