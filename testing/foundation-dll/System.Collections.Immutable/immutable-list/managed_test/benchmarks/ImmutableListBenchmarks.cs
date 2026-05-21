using System;
using System.Diagnostics;

// Auto-generated benchmark harness for ImmutableListTests
// Runs 6 methods as quick smoke-benchmark.
class Program
{
    static void Main()
    {
        // Benchmark entry 0: Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableList_Create_Byte_System_Collections_Immutable_Immutab
        var sw0 = Stopwatch.StartNew();
        _ = ImmutableList.Create<Byte>();
        sw0.Stop();
        Console.WriteLine($"Entry 0: {sw0.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 1: Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableList_Create_Byte_System_Collections_Immutable_Immutab
        var sw1 = Stopwatch.StartNew();
        _ = ImmutableList.Create<Byte>((byte)42);
        sw1.Stop();
        Console.WriteLine($"Entry 1: {sw1.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 2: Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableList_Create_Byte_System_Collections_Immutable_Immutab
        var sw2 = Stopwatch.StartNew();
        _ = ImmutableList.Create<Byte>((byte)42, (byte)42);
        sw2.Stop();
        Console.WriteLine($"Entry 2: {sw2.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 3: Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableList_Create_Int32_System_Collections_Immutable_Immuta
        var sw3 = Stopwatch.StartNew();
        _ = ImmutableList.Create<Int32>();
        sw3.Stop();
        Console.WriteLine($"Entry 3: {sw3.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 4: Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableList_Create_Int32_System_Collections_Immutable_Immuta
        var sw4 = Stopwatch.StartNew();
        _ = ImmutableList.Create<Int32>(42);
        sw4.Stop();
        Console.WriteLine($"Entry 4: {sw4.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 5: Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableList_Create_String_System_Collections_Immutable_Immut
        var sw5 = Stopwatch.StartNew();
        _ = ImmutableList.Create<String>("hello");
        sw5.Stop();
        Console.WriteLine($"Entry 5: {sw5.Elapsed.TotalMilliseconds:F3} ms");
    }
}
