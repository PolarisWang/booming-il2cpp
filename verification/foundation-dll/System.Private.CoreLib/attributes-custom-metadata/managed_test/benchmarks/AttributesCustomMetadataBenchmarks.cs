using System;
using System.Diagnostics;

// Auto-generated benchmark harness for AttributesCustomMetadataTests
// Runs 4 methods as quick smoke-benchmark.
class Program
{
    static void Main()
    {
        // Benchmark entry 0: Benchmark_System_Private_CoreLib_System_Attribute_GetCustomAttribute_System_Attribute_System_Reflection_MemberInfo_System_Type
        var sw0 = Stopwatch.StartNew();
        _ = typeof(byte).Assembly.GetCustomAttribute(typeof(AssemblyDescriptionAttribute));
        sw0.Stop();
        Console.WriteLine($"Entry 0: {sw0.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 1: Benchmark_System_Private_CoreLib_System_Attribute_GetCustomAttribute_System_Attribute_System_Reflection_Assembly_System_Type
        var sw1 = Stopwatch.StartNew();
        _ = typeof(byte).Assembly.GetCustomAttribute(typeof(AssemblyDescriptionAttribute));
        sw1.Stop();
        Console.WriteLine($"Entry 1: {sw1.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 2: Benchmark_System_Private_CoreLib_System_Attribute_IsDefined_System_Boolean_System_Reflection_MemberInfo_System_Type
        var sw2 = Stopwatch.StartNew();
        _ = typeof(byte).Assembly.IsDefined(typeof(AssemblyDescriptionAttribute));
        sw2.Stop();
        Console.WriteLine($"Entry 2: {sw2.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 3: Benchmark_System_Private_CoreLib_System_Attribute_IsDefined_System_Boolean_System_Reflection_Assembly_System_Type
        var sw3 = Stopwatch.StartNew();
        _ = typeof(byte).Assembly.IsDefined(typeof(AssemblyDescriptionAttribute));
        sw3.Stop();
        Console.WriteLine($"Entry 3: {sw3.Elapsed.TotalMilliseconds:F3} ms");
    }
}
