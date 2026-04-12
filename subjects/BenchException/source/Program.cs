using System;
using System.Diagnostics;

namespace BenchException;

internal static class Program
{
    public static int Main(string[] args)
    {
        int iterations = args.Length > 0 && int.TryParse(args[0], out var n) ? n : 1000;

        var sw = Stopwatch.StartNew();
        long checksum = 0;

        // Happy path (no throw)
        for (int i = 0; i < iterations * 100; i++)
        {
            try { checksum += i; }
            catch (Exception) { /* should not happen */ }
        }

        // Throw path (1/100 rate)
        for (int i = 0; i < iterations * 10; i++)
        {
            try { if (i % 100 == 0) throw new InvalidOperationException("bench"); checksum += i; }
            catch (InvalidOperationException) { checksum++; }
        }
        sw.Stop();

        double opsPerSec = (iterations * 110.0) / (sw.Elapsed.TotalMilliseconds / 1000.0);
        Console.WriteLine($"{{\"elapsedMilliseconds\":{sw.Elapsed.TotalMilliseconds:F3},\"iterations\":{iterations},\"opsPerSecond\":{opsPerSec:F0},\"checksum\":{checksum % 10000}}}");
        return 0;
    }
}
