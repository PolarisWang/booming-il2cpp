using System;
using System.Diagnostics;

namespace BenchMixed;

// BenchMixed simulates AOT↔interpreter boundary.
// In C# baseline mode, both "aot" and "interpreter" methods are just regular calls.
// The benchmark measures the overhead of the cross-boundary calling pattern.
internal static class Program
{
    // Simulates an "AOT" method (fast, direct call)
    private static int AotCompute(int x) => x * x + x;

    // Simulates an "interpreter" method (slightly more expensive indirection)
    private static Func<int, int>? s_interpreterFn;

    private static int InterpreterCompute(int x)
    {
        // Invoke through a delegate to simulate interpreter dispatch overhead
        return s_interpreterFn?.Invoke(x) ?? x;
    }

    public static int Main(string[] args)
    {
        int iterations = args.Length > 0 && int.TryParse(args[0], out var n) ? n : 1000;

        // Initialize "interpreter" delegate (simulates loading a hot-update method)
        s_interpreterFn = static x => x * 2 + 1;

        var sw = Stopwatch.StartNew();
        long checksum = 0;
        for (int i = 0; i < iterations * 100; i++)
        {
            // AOT → interpreter → AOT pattern
            int v1 = AotCompute(i);
            int v2 = InterpreterCompute(v1);
            int v3 = AotCompute(v2);
            checksum += v3;
        }
        sw.Stop();

        double opsPerSec = (iterations * 100.0) / (sw.Elapsed.TotalMilliseconds / 1000.0);
        Console.WriteLine($"{{\"elapsedMilliseconds\":{sw.Elapsed.TotalMilliseconds:F3},\"iterations\":{iterations},\"opsPerSecond\":{opsPerSec:F0},\"checksum\":{checksum % 10000}}}");
        return 0;
    }
}
