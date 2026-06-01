// Enum parsing benchmark harness for .NET 8
// Auto-generated for horizontal comparison with IL2CPP AOT/JIT
using System;
using System.Diagnostics;
using System.Linq;
using System.Collections.Generic;
using System.Reflection;

public static class EnumParsingBench
{
    // Number of iterations per benchmark method
    const int Iterations = 100000;

    // Pre-allocate the 'result' array for out parameters
    static object? _resultBox;

    public static void Main()
    {
        // Define benchmark cases: (methodIndex, Action to run)
        var cases = new (int Index, string Name, Action Action)[]
        {
            (1,  "Enum.GetName(RuntimeType, ulong)",       () => { var _ = Enum.GetName(typeof(DayOfWeek), (object)1); }),
            (2,  "Enum.GetName(Type, object)",             () => { var _ = Enum.GetName(typeof(DayOfWeek), (object)1); }),
            (6,  "Enum.Parse(Type, string)",               () => { var _ = Enum.Parse(typeof(DayOfWeek), "Monday"); }),
            (7,  "Enum.Parse(Type, string, bool)",         () => { var _ = Enum.Parse(typeof(DayOfWeek), "Monday", true); }),
            (10, "Enum.TryParse(Type, string, bool, out)", () => { Enum.TryParse(typeof(DayOfWeek), "Monday", true, out _resultBox); }),
            (11, "Enum.TryParse(Type, string, out)",       () => { Enum.TryParse(typeof(DayOfWeek), "Monday", out _resultBox); }),
        };

        // Warmup
        foreach (var (_, _, action) in cases)
        {
            for (int i = 0; i < 1000; i++) action();
        }

        // Benchmark
        var results = new List<object>();
        foreach (var (index, name, action) in cases)
        {
            // Dry run first (measure overhead including GC)
            GC.Collect();
            GC.WaitForPendingFinalizers();

            var sw = Stopwatch.StartNew();
            for (int i = 0; i < Iterations; i++)
            {
                action();
            }
            sw.Stop();

            var elapsedMs = sw.Elapsed.TotalMilliseconds;
            var opsPerSec = Iterations / (elapsedMs / 1000.0);
            var allocated = Process.GetCurrentProcess().PrivateMemorySize64;

            results.Add(new
            {
                methodIndex = index,
                methodName = name,
                elapsedMilliseconds = elapsedMs,
                opsPerSecond = opsPerSec,
                iterations = Iterations,
            });

            Console.Error.WriteLine($"[{index}] {name}: {elapsedMs:F3}ms, {opsPerSec:F0} ops/s");
        }

        Console.WriteLine(System.Text.Json.JsonSerializer.Serialize(new { results }));
    }
}
