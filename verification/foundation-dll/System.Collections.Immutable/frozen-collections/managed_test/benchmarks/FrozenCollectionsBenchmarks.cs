using System;
using System.Diagnostics;

// Auto-generated benchmark harness for FrozenCollectionsTests
// Runs 8 methods as quick smoke-benchmark.
class Program
{
    static void Main()
    {
        // Benchmark entry 0: Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_Create_string_int_System_Collections_Frozen_Froz
        var sw0 = Stopwatch.StartNew();
        _ = FrozenDictionary.Create<string,int>();
        sw0.Stop();
        Console.WriteLine($"Entry 0: {sw0.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 1: Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_Create_Byte_Byte_System_Collections_Frozen_Froze
        var sw1 = Stopwatch.StartNew();
        _ = FrozenDictionary.Create<Byte,Byte>();
        sw1.Stop();
        Console.WriteLine($"Entry 1: {sw1.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 2: Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_Create_string_string_System_Collections_Frozen_F
        var sw2 = Stopwatch.StartNew();
        _ = FrozenDictionary.Create<string,string>();
        sw2.Stop();
        Console.WriteLine($"Entry 2: {sw2.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 3: Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Create_Byte_System_Collections_Frozen_FrozenSet_System_
        var sw3 = Stopwatch.StartNew();
        _ = FrozenSet.Create<Byte>();
        sw3.Stop();
        Console.WriteLine($"Entry 3: {sw3.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 4: Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Create_Byte_System_Collections_Frozen_FrozenSet_System_
        var sw4 = Stopwatch.StartNew();
        _ = FrozenSet.Create<Byte>((byte)42);
        sw4.Stop();
        Console.WriteLine($"Entry 4: {sw4.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 5: Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Create_Byte_System_Collections_Frozen_FrozenSet_System_
        var sw5 = Stopwatch.StartNew();
        _ = FrozenSet.Create<Byte>((byte)42, (byte)42);
        sw5.Stop();
        Console.WriteLine($"Entry 5: {sw5.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 6: Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Create_Int32_System_Collections_Frozen_FrozenSet_System
        var sw6 = Stopwatch.StartNew();
        _ = FrozenSet.Create<Int32>();
        sw6.Stop();
        Console.WriteLine($"Entry 6: {sw6.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 7: Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Create_Int32_System_Collections_Frozen_FrozenSet_System
        var sw7 = Stopwatch.StartNew();
        _ = FrozenSet.Create<Int32>(42);
        sw7.Stop();
        Console.WriteLine($"Entry 7: {sw7.Elapsed.TotalMilliseconds:F3} ms");
    }
}
