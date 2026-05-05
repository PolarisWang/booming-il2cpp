namespace Chaos.TestFramework.Runtime;

public static class ChaosBenchmarkRunner
{
    public static int Run(string[]? args)
    {
        var request = ChaosManagedHostArguments.Parse(args);
        var entry = ChaosManagedHostReflection.ResolveEntry(ChaosManagedHostKind.Benchmark, request);

        var methodName = entry?.MethodName ?? "unknown";
        RuntimeTrace.Point("benchmark_runner.run", "benchmark", new Dictionary<string, object?>
        {
            ["method"] = methodName,
            ["entryIndex"] = request.EntryIndex,
        });

        RuntimeTrace.TraceCall("benchmark_runner.invoke", "benchmark", () =>
        {
            ChaosManagedHostReflection.InvokeStaticEntry(entry);
        });
        return 0;
    }
}
