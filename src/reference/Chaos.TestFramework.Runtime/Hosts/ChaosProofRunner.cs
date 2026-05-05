using System;
using Chaos.TestFramework;

namespace Chaos.TestFramework.Runtime;

public static class ChaosProofRunner
{
    public static int Run(string[]? args)
    {
        var request = ChaosManagedHostArguments.Parse(args);
        var entry = ChaosManagedHostReflection.ResolveEntry(ChaosManagedHostKind.Proof, request);

        var methodName = entry?.MethodName ?? "unknown";
        RuntimeTrace.Point("proof_runner.run", "proof", new Dictionary<string, object?>
        {
            ["method"] = methodName,
            ["entryIndex"] = request.EntryIndex,
        });

        try
        {
            ChaosAssertState.Reset();
            RuntimeTrace.TraceCall("proof_runner.invoke", "proof", () =>
            {
                ChaosManagedHostReflection.InvokeStaticEntry(entry);
            });
            return ChaosAssertState.Complete();
        }
        catch (ChaosAssertionException exception)
        {
            Console.Error.WriteLine(exception.Message);
            ChaosAssertState.RecordFailure();
            return ChaosAssertState.Complete();
        }
    }
}
