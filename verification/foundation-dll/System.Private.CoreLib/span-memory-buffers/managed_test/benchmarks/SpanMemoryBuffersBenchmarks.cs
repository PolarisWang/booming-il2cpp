using System;
using System.Diagnostics;

// Auto-generated benchmark harness for SpanMemoryBuffersTests
// Runs 3 methods as quick smoke-benchmark.
class Program
{
    static void Main()
    {
        // Benchmark entry 0: Benchmark_System_Private_CoreLib_System_Span_1_CopyTo_System_Void_System_Span_1
        var sw0 = Stopwatch.StartNew();
        _ = default(Span<byte>).CopyTo(default(Span<byte>));
        sw0.Stop();
        Console.WriteLine($"Entry 0: {sw0.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 1: Benchmark_System_Private_CoreLib_System_Span_1_get_Empty_System_Span_1
        var sw1 = Stopwatch.StartNew();
        _ = Span<byte>.Empty;
        sw1.Stop();
        Console.WriteLine($"Entry 1: {sw1.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 2: Benchmark_System_Private_CoreLib_System_ReadOnlySpan_1_ToArray_T
        var sw2 = Stopwatch.StartNew();
        _ = default(ReadOnlySpan<byte>).ToArray();
        sw2.Stop();
        Console.WriteLine($"Entry 2: {sw2.Elapsed.TotalMilliseconds:F3} ms");
    }
}
