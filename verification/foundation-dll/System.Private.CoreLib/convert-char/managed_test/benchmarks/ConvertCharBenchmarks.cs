using System;
using System.Diagnostics;

// Auto-generated benchmark harness for ConvertCharTests
// Runs 10 methods as quick smoke-benchmark.
class Program
{
    static void Main()
    {
        // Benchmark entry 0: Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Boolean
        var sw0 = Stopwatch.StartNew();
        _ = Convert.ToChar(true);
        sw0.Stop();
        Console.WriteLine($"Entry 0: {sw0.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 1: Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Byte
        var sw1 = Stopwatch.StartNew();
        _ = Convert.ToChar((byte)42);
        sw1.Stop();
        Console.WriteLine($"Entry 1: {sw1.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 2: Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Char
        var sw2 = Stopwatch.StartNew();
        _ = Convert.ToChar('A');
        sw2.Stop();
        Console.WriteLine($"Entry 2: {sw2.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 3: Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_DateTime
        var sw3 = Stopwatch.StartNew();
        _ = Convert.ToChar(DateTime.UtcNow);
        sw3.Stop();
        Console.WriteLine($"Entry 3: {sw3.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 4: Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Decimal
        var sw4 = Stopwatch.StartNew();
        _ = Convert.ToChar(42m);
        sw4.Stop();
        Console.WriteLine($"Entry 4: {sw4.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 5: Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Double
        var sw5 = Stopwatch.StartNew();
        _ = Convert.ToChar(42.0);
        sw5.Stop();
        Console.WriteLine($"Entry 5: {sw5.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 6: Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Int16
        var sw6 = Stopwatch.StartNew();
        _ = Convert.ToChar((short)42);
        sw6.Stop();
        Console.WriteLine($"Entry 6: {sw6.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 7: Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Int32
        var sw7 = Stopwatch.StartNew();
        _ = Convert.ToChar(42);
        sw7.Stop();
        Console.WriteLine($"Entry 7: {sw7.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 8: Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Int64
        var sw8 = Stopwatch.StartNew();
        _ = Convert.ToChar(42L);
        sw8.Stop();
        Console.WriteLine($"Entry 8: {sw8.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 9: Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Object
        var sw9 = Stopwatch.StartNew();
        _ = Convert.ToChar(42);
        sw9.Stop();
        Console.WriteLine($"Entry 9: {sw9.Elapsed.TotalMilliseconds:F3} ms");
    }
}
