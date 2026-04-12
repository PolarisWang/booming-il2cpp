using System.Diagnostics;
using System.Text.Json;
using Chaos.IL2CPP.HotUpdate;

namespace BenchHotUpdateDispatch;

internal static class Program
{
    private const string SubjectId = "BenchHotUpdateDispatch/HotPatch::AddOne(System.Int32)";

    public static int Main(string[] args)
    {
        var iterations = args.Length > 0 && int.TryParse(args[0], out var parsed) ? parsed : 1000;
        var runtimeManager = new RuntimeManager();
        runtimeManager.RegisterInt32Unary(SubjectId, static value => value + 1);

        var sw = Stopwatch.StartNew();
        var checksum = 0;
        for (var index = 0; index < iterations; index++)
        {
            checksum += runtimeManager.DispatchInt32Unary(SubjectId, index, static value => value);
        }

        sw.Stop();
        var opsPerSecond = iterations / Math.Max(sw.Elapsed.TotalSeconds, 0.0001);
        Console.WriteLine(
            JsonSerializer.Serialize(new
            {
                elapsedMilliseconds = sw.Elapsed.TotalMilliseconds,
                iterations,
                opsPerSecond,
                checksum,
            }));
        return 0;
    }
}
