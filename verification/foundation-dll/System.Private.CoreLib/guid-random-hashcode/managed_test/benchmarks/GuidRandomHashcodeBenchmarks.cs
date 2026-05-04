using System;
using System.Diagnostics;

// Auto-generated benchmark harness for GuidRandomHashcodeTests
// Runs 6 methods as quick smoke-benchmark.
class Program
{
    static void Main()
    {
        // Benchmark entry 0: Benchmark_System_Private_CoreLib_System_Guid_NewGuid_System_Guid
        var sw0 = Stopwatch.StartNew();
        _ = Guid.NewGuid();
        sw0.Stop();
        Console.WriteLine($"Entry 0: {sw0.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 1: Benchmark_System_Private_CoreLib_System_Guid_ToString_System_String
        var sw1 = Stopwatch.StartNew();
        _ = Guid.NewGuid().ToString();
        sw1.Stop();
        Console.WriteLine($"Entry 1: {sw1.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 2: Benchmark_System_Private_CoreLib_System_Random_ctor_System_Void
        var sw2 = Stopwatch.StartNew();
        _ = new Random();
        sw2.Stop();
        Console.WriteLine($"Entry 2: {sw2.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 3: Benchmark_System_Private_CoreLib_System_Random_Next_System_Int32
        var sw3 = Stopwatch.StartNew();
        _ = new Random().Next();
        sw3.Stop();
        Console.WriteLine($"Entry 3: {sw3.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 4: Benchmark_System_Private_CoreLib_System_Random_Next_System_Int32_System_Int32
        var sw4 = Stopwatch.StartNew();
        _ = new Random().Next(42);
        sw4.Stop();
        Console.WriteLine($"Entry 4: {sw4.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 5: Benchmark_System_Private_CoreLib_System_Random_NextDouble_System_Double
        var sw5 = Stopwatch.StartNew();
        _ = new Random().NextDouble();
        sw5.Stop();
        Console.WriteLine($"Entry 5: {sw5.Elapsed.TotalMilliseconds:F3} ms");
    }
}
