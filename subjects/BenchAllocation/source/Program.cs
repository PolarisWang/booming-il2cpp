using System;
using System.Diagnostics;

namespace BenchAllocation;

internal sealed class WorkItem
{
    public int Id;
    public string? Name;
    public double Value;
}

internal static class Program
{
    public static int Main(string[] args)
    {
        int iterations = args.Length > 0 && int.TryParse(args[0], out var n) ? n : 1000;

        var sw = Stopwatch.StartNew();
        long checksum = 0;
        for (int i = 0; i < iterations * 10; i++)
        {
            var item = new WorkItem { Id = i, Name = "item" + i, Value = i * 1.5 };
            checksum += item.Id;
        }
        sw.Stop();

        double opsPerSec = (iterations * 10.0) / (sw.Elapsed.TotalMilliseconds / 1000.0);
        Console.WriteLine($"{{\"elapsedMilliseconds\":{sw.Elapsed.TotalMilliseconds:F3},\"iterations\":{iterations},\"opsPerSecond\":{opsPerSec:F0},\"checksum\":{checksum % 10000}}}");
        return 0;
    }
}
