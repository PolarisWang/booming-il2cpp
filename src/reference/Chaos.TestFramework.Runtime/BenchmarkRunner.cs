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
            foreach (var method in type.GetMethods(BindingFlags.Static | BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic))
            {
                if (method.GetParameters().Length != 0) continue;
                if (method.GetCustomAttribute<FactAttribute>() is null && method.GetCustomAttribute<HotUpdateAttribute>() is null) continue;
                methods.Add((type, method));
            }
        }
        var results = new List<object>();
        foreach (var (type, method) in methods)
        {
            var label = $"{type.Name}.{method.Name}";
            results.Add(BenchmarkOne(type, method, label, iterations));
        }
        Console.WriteLine(JsonSerializer.Serialize(new { results }));
    }
    static object BenchmarkOne(Type type, MethodInfo method, string label, int iterations)
    {
        object? inst = null;
        if (!method.IsStatic) { try { inst = Activator.CreateInstance(type); } catch { return new { label, elapsedMs = 0.0, opsPerSecond = 0.0, iterations, threw = true, gcInfo = new { } }; } }
        try { method.Invoke(inst, null); } catch { }
        GC.Collect(); GC.WaitForPendingFinalizers(); GC.Collect();
        long ab = GC.GetAllocatedBytesForCurrentThread(), hb = GC.GetTotalMemory(false);
        int g0b = GC.CollectionCount(0), g1b = GC.CollectionCount(1), g2b = GC.CollectionCount(2);
        bool threw = false; long best = long.MaxValue;
        for (int r = 0; r < 3; r++)
        {
            long start = DateTime.UtcNow.Ticks;
            try { for (int i = 0; i < iterations; i++) method.Invoke(inst, null); }
            catch { threw = true; break; }
            long elapsed = DateTime.UtcNow.Ticks - start;
            if (elapsed < best) best = elapsed;
        }
        long aa = GC.GetAllocatedBytesForCurrentThread(), ha = GC.GetTotalMemory(false);
        int g0a = GC.CollectionCount(0), g1a = GC.CollectionCount(1), g2a = GC.CollectionCount(2);
        double ms = best / 10000.0;
        return new { label, elapsedMs = Math.Round(ms, 3), opsPerSecond = Math.Round(iterations / ((ms > 0 ? ms : 0.001) / 1000.0), 0), iterations, threw, gcInfo = new { totalAllocatedBytes = aa - ab, heapBefore = hb, heapAfter = ha, heapDelta = ha - hb, collectionCount0 = g0a - g0b, collectionCount1 = g1a - g1b, collectionCount2 = g2a - g2b } };
    }
}
