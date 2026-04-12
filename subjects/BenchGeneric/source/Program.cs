using System;
using System.Collections.Generic;
using System.Diagnostics;

namespace BenchGeneric;

internal static class Program
{
    public static int Main(string[] args)
    {
        int iterations = args.Length > 0 && int.TryParse(args[0], out var n) ? n : 1000;

        var sw = Stopwatch.StartNew();
        long checksum = 0;
        for (int iter = 0; iter < iterations; iter++)
        {
            var list = new List<int>();
            for (int i = 0; i < 100; i++) list.Add(i);
            checksum += list.Count;

            var dict = new Dictionary<string, int>();
            for (int i = 0; i < 20; i++) dict["key" + i] = i;
            checksum += dict["key10"];
        }
        sw.Stop();

        double opsPerSec = iterations / (sw.Elapsed.TotalMilliseconds / 1000.0);
        Console.WriteLine($"{{\"elapsedMilliseconds\":{sw.Elapsed.TotalMilliseconds:F3},\"iterations\":{iterations},\"opsPerSecond\":{opsPerSec:F0},\"checksum\":{checksum % 10000}}}");
        return 0;
    }
}
