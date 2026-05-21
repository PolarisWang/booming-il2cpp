using System;
using System.Diagnostics;

// Auto-generated benchmark harness for ImmutableDictionaryTests
// Runs 3 methods as quick smoke-benchmark.
class Program
{
    static void Main()
    {
        // Benchmark entry 0: Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_Create_string_int_System_Collections_Immut
        var sw0 = Stopwatch.StartNew();
        _ = ImmutableDictionary.Create<string,int>();
        sw0.Stop();
        Console.WriteLine($"Entry 0: {sw0.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 1: Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_Create_Byte_Byte_System_Collections_Immuta
        var sw1 = Stopwatch.StartNew();
        _ = ImmutableDictionary.Create<Byte,Byte>();
        sw1.Stop();
        Console.WriteLine($"Entry 1: {sw1.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 2: Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_Create_string_string_System_Collections_Im
        var sw2 = Stopwatch.StartNew();
        _ = ImmutableDictionary.Create<string,string>();
        sw2.Stop();
        Console.WriteLine($"Entry 2: {sw2.Elapsed.TotalMilliseconds:F3} ms");
    }
}
