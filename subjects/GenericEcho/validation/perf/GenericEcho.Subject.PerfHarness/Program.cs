using System.Diagnostics;
using System.Text.Json;

namespace GenericEcho.Subject.PerfHarness;

internal static class Program
{
    private static int Main(string[] args)
    {
        var iterations = 1000;
        if (args.Length > 0 && int.TryParse(args[0], out var parsedIterations) && parsedIterations > 0)
        {
            iterations = parsedIterations;
        }

        var stopwatch = Stopwatch.StartNew();
        var lastValue = string.Empty;
        for (var index = 0; index < iterations; index += 1)
        {
            lastValue = $"{index % 7}:roadmap0";
        }

        stopwatch.Stop();

        var payload = new
        {
            harness = "csharp-perf-harness",
            subjectId = "GenericEcho",
            iterations,
            elapsedMilliseconds = stopwatch.Elapsed.TotalMilliseconds,
            lastValue,
        };

        Console.WriteLine(JsonSerializer.Serialize(payload));
        return 0;
    }
}
