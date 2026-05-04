using System;
using System.Diagnostics;

// Auto-generated benchmark harness for ExceptionThrowDiagnosticsTests
// Runs 9 methods as quick smoke-benchmark.
class Program
{
    static void Main()
    {
        // Benchmark entry 0: Benchmark_System_Private_CoreLib_System_Exception_ctor_System_Void_System_String
        var sw0 = Stopwatch.StartNew();
        _ = new Exception("hello");
        sw0.Stop();
        Console.WriteLine($"Entry 0: {sw0.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 1: Benchmark_System_Private_CoreLib_System_Exception_ctor_System_Void_System_String_System_Exception
        var sw1 = Stopwatch.StartNew();
        _ = new Exception("hello", null!);
        sw1.Stop();
        Console.WriteLine($"Entry 1: {sw1.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 2: Benchmark_System_Private_CoreLib_System_Exception_get_Message_System_String
        var sw2 = Stopwatch.StartNew();
        _ = new Exception().Message;
        sw2.Stop();
        Console.WriteLine($"Entry 2: {sw2.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 3: Benchmark_System_Private_CoreLib_System_Exception_get_InnerException_System_Exception
        var sw3 = Stopwatch.StartNew();
        _ = new Exception().InnerException;
        sw3.Stop();
        Console.WriteLine($"Entry 3: {sw3.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 4: Benchmark_System_Private_CoreLib_System_Exception_get_HResult_System_Int32
        var sw4 = Stopwatch.StartNew();
        _ = new Exception().HResult;
        sw4.Stop();
        Console.WriteLine($"Entry 4: {sw4.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 5: Benchmark_System_Private_CoreLib_System_Exception_ToString_System_String
        var sw5 = Stopwatch.StartNew();
        _ = new Exception().ToString();
        sw5.Stop();
        Console.WriteLine($"Entry 5: {sw5.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 6: Benchmark_System_Private_CoreLib_System_Exception_GetBaseException_System_Exception
        var sw6 = Stopwatch.StartNew();
        _ = new Exception().GetBaseException();
        sw6.Stop();
        Console.WriteLine($"Entry 6: {sw6.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 7: Benchmark_System_Private_CoreLib_System_ArgumentException_ctor_System_Void_System_String_System_String
        var sw7 = Stopwatch.StartNew();
        _ = new ArgumentException("hello", "hello");
        sw7.Stop();
        Console.WriteLine($"Entry 7: {sw7.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 8: Benchmark_System_Private_CoreLib_System_ArgumentNullException_ctor_System_Void_System_String
        var sw8 = Stopwatch.StartNew();
        _ = new ArgumentNullException("hello");
        sw8.Stop();
        Console.WriteLine($"Entry 8: {sw8.Elapsed.TotalMilliseconds:F3} ms");
    }
}
