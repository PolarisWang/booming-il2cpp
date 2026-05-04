using System;
using System.Diagnostics;

// Auto-generated benchmark harness for StringCharTextCoreTests
// Runs 10 methods as quick smoke-benchmark.
class Program
{
    static void Main()
    {
        // Benchmark entry 0: Benchmark_System_Private_CoreLib_System_String_IndexOf_System_Int32_System_String
        var sw0 = Stopwatch.StartNew();
        _ = "hello".IndexOf("hello");
        sw0.Stop();
        Console.WriteLine($"Entry 0: {sw0.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 1: Benchmark_System_Private_CoreLib_System_String_IndexOf_System_Int32_System_Char
        var sw1 = Stopwatch.StartNew();
        _ = "hello".IndexOf('A');
        sw1.Stop();
        Console.WriteLine($"Entry 1: {sw1.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 2: Benchmark_System_Private_CoreLib_System_String_Substring_System_String_System_Int32
        var sw2 = Stopwatch.StartNew();
        _ = "hello".Substring(1);
        sw2.Stop();
        Console.WriteLine($"Entry 2: {sw2.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 3: Benchmark_System_Private_CoreLib_System_String_Substring_System_String_System_Int32_System_Int32
        var sw3 = Stopwatch.StartNew();
        _ = "hello".Substring(1, 2);
        sw3.Stop();
        Console.WriteLine($"Entry 3: {sw3.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 4: Benchmark_System_Private_CoreLib_System_String_Compare_System_Int32_System_String_System_String
        var sw4 = Stopwatch.StartNew();
        _ = String.Compare("hello", "hello");
        sw4.Stop();
        Console.WriteLine($"Entry 4: {sw4.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 5: Benchmark_System_Private_CoreLib_System_String_Compare_System_Int32_System_String_System_Int32_System_String_System_Int32_System_I
        var sw5 = Stopwatch.StartNew();
        _ = string.Compare("hello", 0, "world", 0, 3);
        sw5.Stop();
        Console.WriteLine($"Entry 5: {sw5.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 6: Benchmark_System_Private_CoreLib_System_String_Concat_System_String_System_String_System_String
        var sw6 = Stopwatch.StartNew();
        _ = String.Concat("hello", "hello");
        sw6.Stop();
        Console.WriteLine($"Entry 6: {sw6.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 7: Benchmark_System_Private_CoreLib_System_String_Concat_System_String_System_String_System_String_System_String
        var sw7 = Stopwatch.StartNew();
        _ = String.Concat("hello", "hello", "hello");
        sw7.Stop();
        Console.WriteLine($"Entry 7: {sw7.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 8: Benchmark_System_Private_CoreLib_System_String_Format_System_String_System_String_System_Object
        var sw8 = Stopwatch.StartNew();
        _ = String.Format("hello", Array.Empty<System.Object>());
        sw8.Stop();
        Console.WriteLine($"Entry 8: {sw8.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 9: Benchmark_System_Private_CoreLib_System_String_StartsWith_System_Boolean_System_String
        var sw9 = Stopwatch.StartNew();
        _ = "hello".StartsWith("hello");
        sw9.Stop();
        Console.WriteLine($"Entry 9: {sw9.Elapsed.TotalMilliseconds:F3} ms");
    }
}
