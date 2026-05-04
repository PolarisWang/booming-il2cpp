using System;
using System.Diagnostics;

// Auto-generated benchmark harness for GenericNullableValueTests
// Runs 8 methods as quick smoke-benchmark.
class Program
{
    static void Main()
    {
        // Benchmark entry 0: Benchmark_System_Private_CoreLib_System_Nullable_1_get_HasValue_System_Boolean
        var sw0 = Stopwatch.StartNew();
        _ = default(Nullable<int>).HasValue;
        sw0.Stop();
        Console.WriteLine($"Entry 0: {sw0.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 1: Benchmark_System_Private_CoreLib_System_Nullable_1_GetValueOrDefault_T
        var sw1 = Stopwatch.StartNew();
        _ = default(Nullable<int>).GetValueOrDefault();
        sw1.Stop();
        Console.WriteLine($"Entry 1: {sw1.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 2: Benchmark_System_Private_CoreLib_System_Nullable_1_Equals_System_Boolean_System_Object
        var sw2 = Stopwatch.StartNew();
        _ = default(Nullable<int>).Equals(42);
        sw2.Stop();
        Console.WriteLine($"Entry 2: {sw2.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 3: Benchmark_System_Private_CoreLib_System_Nullable_1_GetHashCode_System_Int32
        var sw3 = Stopwatch.StartNew();
        _ = default(Nullable<int>).GetHashCode();
        sw3.Stop();
        Console.WriteLine($"Entry 3: {sw3.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 4: Benchmark_System_Private_CoreLib_System_Nullable_1_ToString_System_String
        var sw4 = Stopwatch.StartNew();
        _ = default(Nullable<int>).ToString();
        sw4.Stop();
        Console.WriteLine($"Entry 4: {sw4.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 5: Benchmark_System_Private_CoreLib_System_ValueType_Equals_System_Boolean_System_Object
        var sw5 = Stopwatch.StartNew();
        _ = ((ValueType)42).Equals(42);
        sw5.Stop();
        Console.WriteLine($"Entry 5: {sw5.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 6: Benchmark_System_Private_CoreLib_System_ValueType_GetHashCode_System_Int32
        var sw6 = Stopwatch.StartNew();
        _ = ((ValueType)42).GetHashCode();
        sw6.Stop();
        Console.WriteLine($"Entry 6: {sw6.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 7: Benchmark_System_Private_CoreLib_System_ValueType_ToString_System_String
        var sw7 = Stopwatch.StartNew();
        _ = ((ValueType)42).ToString();
        sw7.Stop();
        Console.WriteLine($"Entry 7: {sw7.Elapsed.TotalMilliseconds:F3} ms");
    }
}
