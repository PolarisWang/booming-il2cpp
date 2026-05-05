using System;
using System.Diagnostics;

// Auto-generated benchmark harness for ReflectionMemberBasicsTests
// Runs 2 methods as quick smoke-benchmark.
class Program
{
    static void Main()
    {
        // Benchmark entry 0: Benchmark_System_Private_CoreLib_System_Reflection_MemberInfo_get_Name_System_String
        var sw0 = Stopwatch.StartNew();
        _ = default(MemberInfo)!.Name;
        sw0.Stop();
        Console.WriteLine($"Entry 0: {sw0.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 1: Benchmark_System_Private_CoreLib_System_Reflection_MemberInfo_get_DeclaringType_System_Type
        var sw1 = Stopwatch.StartNew();
        _ = default(MemberInfo)!.DeclaringType;
        sw1.Stop();
        Console.WriteLine($"Entry 1: {sw1.Elapsed.TotalMilliseconds:F3} ms");
    }
}
