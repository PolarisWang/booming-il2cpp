using System;
using System.Diagnostics;

// Auto-generated benchmark harness for DelegateCoreInvocationTests
// Runs 2 methods as quick smoke-benchmark.
class Program
{
    static void Main()
    {
        // Benchmark entry 0: Benchmark_System_Private_CoreLib_System_Delegate_Combine_System_Delegate_System_Delegate_System_Delegate
        var sw0 = Stopwatch.StartNew();
        _ = Delegate.Combine(null!, null!);
        sw0.Stop();
        Console.WriteLine($"Entry 0: {sw0.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 1: Benchmark_System_Private_CoreLib_System_Delegate_Remove_System_Delegate_System_Delegate_System_Delegate
        var sw1 = Stopwatch.StartNew();
        _ = Delegate.Remove(null!, null!);
        sw1.Stop();
        Console.WriteLine($"Entry 1: {sw1.Elapsed.TotalMilliseconds:F3} ms");
    }
}
