using System;

namespace Chaos.TestFramework.Runtime;

internal static class Program
{
    private static int Main(string[] args)
    {
        var hostKind = ChaosManagedHostArguments.ParseHostKind(args);
        return hostKind switch
        {
            ChaosManagedHostKind.Proof => ChaosProofRunner.Run(args),
            ChaosManagedHostKind.Benchmark => ChaosBenchmarkRunner.Run(args),
            _ => throw new ArgumentOutOfRangeException(nameof(args), hostKind, "Unsupported managed host kind."),
        };
    }
}
