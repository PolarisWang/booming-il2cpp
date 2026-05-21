using System;
using System.Diagnostics;

// Auto-generated benchmark harness for ImmutableQueueStackTests
// Runs 10 methods as quick smoke-benchmark.
class Program
{
    static void Main()
    {
        // Benchmark entry 0: Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_Create_Byte_System_Collections_Immutable_Immuta
        var sw0 = Stopwatch.StartNew();
        _ = ImmutableQueue.Create<Byte>();
        sw0.Stop();
        Console.WriteLine($"Entry 0: {sw0.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 1: Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_Create_Byte_System_Collections_Immutable_Immuta
        var sw1 = Stopwatch.StartNew();
        _ = ImmutableQueue.Create<Byte>((byte)42);
        sw1.Stop();
        Console.WriteLine($"Entry 1: {sw1.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 2: Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_Create_Byte_System_Collections_Immutable_Immuta
        var sw2 = Stopwatch.StartNew();
        _ = ImmutableQueue.Create<Byte>((byte)42, (byte)42);
        sw2.Stop();
        Console.WriteLine($"Entry 2: {sw2.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 3: Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_Create_Int32_System_Collections_Immutable_Immut
        var sw3 = Stopwatch.StartNew();
        _ = ImmutableQueue.Create<Int32>();
        sw3.Stop();
        Console.WriteLine($"Entry 3: {sw3.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 4: Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_Create_Int32_System_Collections_Immutable_Immut
        var sw4 = Stopwatch.StartNew();
        _ = ImmutableQueue.Create<Int32>(42);
        sw4.Stop();
        Console.WriteLine($"Entry 4: {sw4.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 5: Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_Create_Byte_System_Collections_Immutable_Immuta
        var sw5 = Stopwatch.StartNew();
        _ = ImmutableStack.Create<Byte>();
        sw5.Stop();
        Console.WriteLine($"Entry 5: {sw5.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 6: Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_Create_Byte_System_Collections_Immutable_Immuta
        var sw6 = Stopwatch.StartNew();
        _ = ImmutableStack.Create<Byte>((byte)42);
        sw6.Stop();
        Console.WriteLine($"Entry 6: {sw6.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 7: Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_Create_Byte_System_Collections_Immutable_Immuta
        var sw7 = Stopwatch.StartNew();
        _ = ImmutableStack.Create<Byte>((byte)42, (byte)42);
        sw7.Stop();
        Console.WriteLine($"Entry 7: {sw7.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 8: Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_Create_Int32_System_Collections_Immutable_Immut
        var sw8 = Stopwatch.StartNew();
        _ = ImmutableStack.Create<Int32>();
        sw8.Stop();
        Console.WriteLine($"Entry 8: {sw8.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 9: Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_Create_Int32_System_Collections_Immutable_Immut
        var sw9 = Stopwatch.StartNew();
        _ = ImmutableStack.Create<Int32>(42);
        sw9.Stop();
        Console.WriteLine($"Entry 9: {sw9.Elapsed.TotalMilliseconds:F3} ms");
    }
}
