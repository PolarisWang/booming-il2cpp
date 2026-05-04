using System;
using System.Diagnostics;

// Auto-generated benchmark harness for BoxingUnboxingCastsTests
// Runs 9 methods as quick smoke-benchmark.
class Program
{
    static void Main()
    {
        // Benchmark entry 0: Benchmark_System_Private_CoreLib_System_Convert_ChangeType_System_Object_System_Object_System_Type
        var sw0 = Stopwatch.StartNew();
        _ = Convert.ChangeType(42, typeof(byte));
        sw0.Stop();
        Console.WriteLine($"Entry 0: {sw0.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 1: Benchmark_System_Private_CoreLib_System_Convert_ChangeType_System_Object_System_Object_System_Type_System_IFormatProvider
        var sw1 = Stopwatch.StartNew();
        _ = Convert.ChangeType(42, typeof(byte), null!);
        sw1.Stop();
        Console.WriteLine($"Entry 1: {sw1.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 2: Benchmark_System_Private_CoreLib_System_Type_IsAssignableFrom_System_Boolean_System_Type
        var sw2 = Stopwatch.StartNew();
        _ = typeof(byte).IsAssignableFrom(typeof(byte));
        sw2.Stop();
        Console.WriteLine($"Entry 2: {sw2.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 3: Benchmark_System_Private_CoreLib_System_Type_IsInstanceOfType_System_Boolean_System_Object
        var sw3 = Stopwatch.StartNew();
        _ = typeof(byte).IsInstanceOfType(42);
        sw3.Stop();
        Console.WriteLine($"Entry 3: {sw3.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 4: Benchmark_System_Private_CoreLib_System_Type_IsSubclassOf_System_Boolean_System_Type
        var sw4 = Stopwatch.StartNew();
        _ = typeof(byte).IsSubclassOf(typeof(byte));
        sw4.Stop();
        Console.WriteLine($"Entry 4: {sw4.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 5: Benchmark_System_Private_CoreLib_System_Type_IsAssignableTo_System_Boolean_System_Type
        var sw5 = Stopwatch.StartNew();
        _ = typeof(byte).IsAssignableTo(typeof(byte));
        sw5.Stop();
        Console.WriteLine($"Entry 5: {sw5.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 6: Benchmark_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_GetObjectValue_System_Object_System_Object
        var sw6 = Stopwatch.StartNew();
        _ = RuntimeHelpers.GetObjectValue(42);
        sw6.Stop();
        Console.WriteLine($"Entry 6: {sw6.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 7: Benchmark_System_Private_CoreLib_System_Array_CreateInstance_System_Array_System_Type_System_Int32
        var sw7 = Stopwatch.StartNew();
        _ = Array.CreateInstance(typeof(byte), 42);
        sw7.Stop();
        Console.WriteLine($"Entry 7: {sw7.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 8: Benchmark_System_Private_CoreLib_System_Array_CreateInstance_System_Array_System_Type_System_Int32_System_Int32
        var sw8 = Stopwatch.StartNew();
        _ = Array.CreateInstance(typeof(byte), 42, 42);
        sw8.Stop();
        Console.WriteLine($"Entry 8: {sw8.Elapsed.TotalMilliseconds:F3} ms");
    }
}
