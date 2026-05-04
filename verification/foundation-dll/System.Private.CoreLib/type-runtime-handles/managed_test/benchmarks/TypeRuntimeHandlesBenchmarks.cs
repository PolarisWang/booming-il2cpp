using System;
using System.Diagnostics;

// Auto-generated benchmark harness for TypeRuntimeHandlesTests
// Runs 7 methods as quick smoke-benchmark.
class Program
{
    static void Main()
    {
        // Benchmark entry 0: Benchmark_System_Private_CoreLib_System_Type_GetTypeFromHandle_System_Type_System_RuntimeTypeHandle
        var sw0 = Stopwatch.StartNew();
        _ = Type.GetTypeFromHandle(default(System.RuntimeTypeHandle));
        sw0.Stop();
        Console.WriteLine($"Entry 0: {sw0.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 1: Benchmark_System_Private_CoreLib_System_Type_get_IsValueType_System_Boolean
        var sw1 = Stopwatch.StartNew();
        _ = typeof(byte).IsValueType;
        sw1.Stop();
        Console.WriteLine($"Entry 1: {sw1.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 2: Benchmark_System_Private_CoreLib_System_Type_get_IsEnum_System_Boolean
        var sw2 = Stopwatch.StartNew();
        _ = typeof(byte).IsEnum;
        sw2.Stop();
        Console.WriteLine($"Entry 2: {sw2.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 3: Benchmark_System_Private_CoreLib_System_Type_get_IsArray_System_Boolean
        var sw3 = Stopwatch.StartNew();
        _ = typeof(byte).IsArray;
        sw3.Stop();
        Console.WriteLine($"Entry 3: {sw3.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 4: Benchmark_System_Private_CoreLib_System_Type_get_TypeHandle_System_RuntimeTypeHandle
        var sw4 = Stopwatch.StartNew();
        _ = typeof(byte).TypeHandle;
        sw4.Stop();
        Console.WriteLine($"Entry 4: {sw4.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 5: Benchmark_System_Private_CoreLib_System_Type_get_FullName_System_String
        var sw5 = Stopwatch.StartNew();
        _ = typeof(byte).FullName;
        sw5.Stop();
        Console.WriteLine($"Entry 5: {sw5.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 6: Benchmark_System_Private_CoreLib_System_Type_get_Name_System_String
        var sw6 = Stopwatch.StartNew();
        _ = typeof(byte).Name;
        sw6.Stop();
        Console.WriteLine($"Entry 6: {sw6.Elapsed.TotalMilliseconds:F3} ms");
    }
}
