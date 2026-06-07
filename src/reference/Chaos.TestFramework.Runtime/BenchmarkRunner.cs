using System.Reflection;
using System.Text.Json;
using Chaos.TestFramework;

namespace Chaos.TestFramework.Runtime;

internal static class BenchmarkRunner
{
    public static void Run(Assembly assembly, int iterations)
    {
        var methods = new List<(Type type, MethodInfo method)>();

        foreach (var type in assembly.GetTypes())
        {
            foreach (var method in type.GetMethods(BindingFlags.Static | BindingFlags.Public | BindingFlags.NonPublic))
            {
                if (method.GetCustomAttribute<BenchmarkAttribute>() is null)
                    continue;
                if (method.GetParameters().Length != 0)
                    continue;
                methods.Add((type, method));
            }
        }

        var results = new List<object>();
        foreach (var (type, method) in methods)
        {
            var label = $"{type.Name}.{method.Name}";
            var result = BenchmarkOne(method, label, iterations);
            results.Add(result);
        }

        Console.WriteLine(JsonSerializer.Serialize(new { results }));
    }

    private static object BenchmarkOne(MethodInfo method, string label, int iterations)
    {
        // Warmup — invoke once to trigger JIT
        try { method.Invoke(null, null); }
        catch { }

        // Clean GC state before timing
        GC.Collect();
        GC.WaitForPendingFinalizers();
        GC.Collect();

        // ── Record GC stats before benchmark ──
        long allocBefore = GC.GetAllocatedBytesForCurrentThread();
        long heapBefore = GC.GetTotalMemory(false);
        int gcCount0Before = GC.CollectionCount(0);
        int gcCount1Before = GC.CollectionCount(1);
        int gcCount2Before = GC.CollectionCount(2);

        bool threw = false;
        long bestTicks = long.MaxValue;

        for (int r = 0; r < 3; r++)
        {
            long start = DateTime.UtcNow.Ticks;
            try
            {
                for (int i = 0; i < iterations; i++)
                    method.Invoke(null, null);
            }
            catch
            {
                threw = true;
                break;
            }
            long elapsed = DateTime.UtcNow.Ticks - start;
            if (elapsed < bestTicks)
                bestTicks = elapsed;
        }

        // ── Record GC stats after benchmark ──
        long allocAfter = GC.GetAllocatedBytesForCurrentThread();
        long heapAfter = GC.GetTotalMemory(false);
        int gcCount0After = GC.CollectionCount(0);
        int gcCount1After = GC.CollectionCount(1);
        int gcCount2After = GC.CollectionCount(2);

        double ms = bestTicks / 10000.0;
        // Clamp zero elapsed to avoid division-by-zero producing Infinity
        double safeMs = ms > 0 ? ms : 0.001;
        double opsPerSec = iterations / (safeMs / 1000.0);

        return new
        {
            label,
            elapsedMs = Math.Round(ms, 3),
            opsPerSecond = Math.Round(opsPerSec, 0),
            iterations,
            threw,
            gcInfo = new
            {
                totalAllocatedBytes = allocAfter - allocBefore,
                heapBefore,
                heapAfter,
                heapDelta = heapAfter - heapBefore,
                collectionCount0 = gcCount0After - gcCount0Before,
                collectionCount1 = gcCount1After - gcCount1Before,
                collectionCount2 = gcCount2After - gcCount2Before,
            }
        };
    }
}
