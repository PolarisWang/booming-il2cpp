using System;
using System.Diagnostics;

// Auto-generated benchmark harness for BufferMemoryTests
// Runs 1 methods as quick smoke-benchmark.
class Program
{
    static void Main()
    {
        // Benchmark entry 0: Benchmark_System_Private_CoreLib_System_Buffer_ByteLength_System_Int32_System_Array
        var sw0 = Stopwatch.StartNew();
        _ = Buffer.ByteLength(Array.Empty<byte>());
        sw0.Stop();
        Console.WriteLine($"Entry 0: {sw0.Elapsed.TotalMilliseconds:F3} ms");
    }
}
