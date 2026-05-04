using System;
using System.Diagnostics;

// Auto-generated benchmark harness for ObjectEqualityIdentityTests
// Runs 6 methods as quick smoke-benchmark.
class Program
{
    static void Main()
    {
        // Benchmark entry 0: Benchmark_System_Private_CoreLib_System_Object_Equals_System_Boolean_System_Object
        var sw0 = Stopwatch.StartNew();
        _ = new object().Equals(42);
        sw0.Stop();
        Console.WriteLine($"Entry 0: {sw0.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 1: Benchmark_System_Private_CoreLib_System_Object_Equals_System_Boolean_System_Object_System_Object
        var sw1 = Stopwatch.StartNew();
        _ = Object.Equals(42, 42);
        sw1.Stop();
        Console.WriteLine($"Entry 1: {sw1.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 2: Benchmark_System_Private_CoreLib_System_Object_ReferenceEquals_System_Boolean_System_Object_System_Object
        var sw2 = Stopwatch.StartNew();
        _ = Object.ReferenceEquals(42, 42);
        sw2.Stop();
        Console.WriteLine($"Entry 2: {sw2.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 3: Benchmark_System_Private_CoreLib_System_Object_GetHashCode_System_Int32
        var sw3 = Stopwatch.StartNew();
        _ = new object().GetHashCode();
        sw3.Stop();
        Console.WriteLine($"Entry 3: {sw3.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 4: Benchmark_System_Private_CoreLib_System_Object_ToString_System_String
        var sw4 = Stopwatch.StartNew();
        _ = new object().ToString();
        sw4.Stop();
        Console.WriteLine($"Entry 4: {sw4.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 5: Benchmark_System_Private_CoreLib_System_Object_GetType_System_Type
        var sw5 = Stopwatch.StartNew();
        _ = new object().GetType();
        sw5.Stop();
        Console.WriteLine($"Entry 5: {sw5.Elapsed.TotalMilliseconds:F3} ms");
    }
}
