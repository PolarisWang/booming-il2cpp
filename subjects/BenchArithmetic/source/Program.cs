using System;
using System.Diagnostics;

namespace BenchArithmetic;

internal static class Program
{
    public static int Main(string[] args)
    {
        int iterations = args.Length > 0 && int.TryParse(args[0], out var n) ? n : 1000;

        var sw = Stopwatch.StartNew();
        long result = 0;
        for (int i = 0; i < iterations * 100; i++)
        {
            result += i * 3 - i / 2 + i % 7;
            result += (long)Math.Sqrt(i + 1.0);
        }
        sw.Stop();

        double opsPerSec = (iterations * 100.0) / (sw.Elapsed.TotalMilliseconds / 1000.0);
        // Last line must be parseable JSON
        Console.WriteLine($"{{\"elapsedMilliseconds\":{sw.Elapsed.TotalMilliseconds:F3},\"iterations\":{iterations},\"opsPerSecond\":{opsPerSec:F0},\"checksum\":{result % 10000}}}");
        return 0;
    }
}
