using System;
using System.Diagnostics;

// Auto-generated benchmark harness for ImmutableSortedDictionaryTests
// Runs 2 methods as quick smoke-benchmark.
class Program
{
    static void Main()
    {
        // Benchmark entry 0: Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_Create_string_int_System_Collections
        var sw0 = Stopwatch.StartNew();
        _ = ImmutableSortedDictionary.Create<string,int>();
        sw0.Stop();
        Console.WriteLine($"Entry 0: {sw0.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 1: Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_Create_Byte_Byte_System_Collections_
        var sw1 = Stopwatch.StartNew();
        _ = ImmutableSortedDictionary.Create<Byte,Byte>();
        sw1.Stop();
        Console.WriteLine($"Entry 1: {sw1.Elapsed.TotalMilliseconds:F3} ms");
    }
}
