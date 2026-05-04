using System;
using System.Diagnostics;

// Auto-generated benchmark harness for IoStreamsBasicsTests
// Runs 7 methods as quick smoke-benchmark.
class Program
{
    static void Main()
    {
        // Benchmark entry 0: Benchmark_System_Private_CoreLib_System_IO_Stream_Flush_System_Void
        var sw0 = Stopwatch.StartNew();
        _ = new MemoryStream().Flush();
        sw0.Stop();
        Console.WriteLine($"Entry 0: {sw0.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 1: Benchmark_System_Private_CoreLib_System_IO_Stream_Seek_System_Int64_System_Int64_System_IO_SeekOrigin
        var sw1 = Stopwatch.StartNew();
        _ = new MemoryStream().Seek(42L, System.IO.SeekOrigin.Begin);
        sw1.Stop();
        Console.WriteLine($"Entry 1: {sw1.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 2: Benchmark_System_Private_CoreLib_System_IO_Stream_SetLength_System_Void_System_Int64
        var sw2 = Stopwatch.StartNew();
        _ = new MemoryStream().SetLength(42L);
        sw2.Stop();
        Console.WriteLine($"Entry 2: {sw2.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 3: Benchmark_System_Private_CoreLib_System_IO_Stream_get_Length_System_Int64
        var sw3 = Stopwatch.StartNew();
        _ = new MemoryStream().Length;
        sw3.Stop();
        Console.WriteLine($"Entry 3: {sw3.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 4: Benchmark_System_Private_CoreLib_System_IO_Stream_get_Position_System_Int64
        var sw4 = Stopwatch.StartNew();
        _ = new MemoryStream().Position;
        sw4.Stop();
        Console.WriteLine($"Entry 4: {sw4.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 5: Benchmark_System_Private_CoreLib_System_IO_TextReader_ReadLine_System_String
        var sw5 = Stopwatch.StartNew();
        _ = new StringReader("hello").ReadLine();
        sw5.Stop();
        Console.WriteLine($"Entry 5: {sw5.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 6: Benchmark_System_Private_CoreLib_System_IO_TextReader_ReadToEnd_System_String
        var sw6 = Stopwatch.StartNew();
        _ = new StringReader("hello").ReadToEnd();
        sw6.Stop();
        Console.WriteLine($"Entry 6: {sw6.Elapsed.TotalMilliseconds:F3} ms");
    }
}
