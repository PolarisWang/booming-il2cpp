using System;
using System.Diagnostics;

namespace BenchDelegate;

internal static class Program
{
    private static int s_counter;

    private static void Tick(int x) => s_counter += x;

    public static int Main(string[] args)
    {
        int iterations = args.Length > 0 && int.TryParse(args[0], out var n) ? n : 1000;

        // Single delegate
        Action<int> single = Tick;
        // Multicast (3 targets)
        Action<int> multi = Tick;
        multi += Tick;
        multi += Tick;

        s_counter = 0;
        var sw = Stopwatch.StartNew();
        for (int i = 0; i < iterations * 100; i++)
        {
            single(1);
            if (i % 10 == 0) multi(1);
        }
        sw.Stop();

        double opsPerSec = (iterations * 100.0) / (sw.Elapsed.TotalMilliseconds / 1000.0);
        Console.WriteLine($"{{\"elapsedMilliseconds\":{sw.Elapsed.TotalMilliseconds:F3},\"iterations\":{iterations},\"opsPerSecond\":{opsPerSec:F0},\"checksum\":{s_counter % 10000}}}");
        return 0;
    }
}
