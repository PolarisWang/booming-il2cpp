namespace Chaos.TestFramework.Runtime;

public static class ChaosBenchmarkRunner
{
    public static int Run(string[]? args)
    {
        var request = ChaosManagedHostArguments.Parse(args);
        var entry = ChaosManagedHostReflection.ResolveEntry(ChaosManagedHostKind.Benchmark, request);
        ChaosManagedHostReflection.InvokeStaticEntry(entry);
        return 0;
    }
}
