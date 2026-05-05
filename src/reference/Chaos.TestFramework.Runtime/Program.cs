using System;

namespace Chaos.TestFramework.Runtime;

internal static class Program
{
    private static int Main(string[] args)
    {
        // RuntimeTrace reads CHAOS_TRACE_PATH / CHAOS_TRACE_ID from env
        // (set by Python orchestration). If not set, IsEnabled is false and all calls are no-ops.
        RuntimeTrace.Point("runtime.start", "runtime");

        var hostKind = ChaosManagedHostArguments.ParseHostKind(args);
        var result = hostKind switch
        {
            ChaosManagedHostKind.Proof => ChaosProofRunner.Run(args),
            ChaosManagedHostKind.Benchmark => ChaosBenchmarkRunner.Run(args),
            _ => throw new ArgumentOutOfRangeException(nameof(args), hostKind, "Unsupported managed host kind."),
        };

        RuntimeTrace.Point("runtime.exit", "runtime", new Dictionary<string, object?>
        {
            ["exitCode"] = result,
        });
        return result;
    }
}
