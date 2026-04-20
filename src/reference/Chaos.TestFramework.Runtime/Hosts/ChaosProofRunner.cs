using System;
using Chaos.TestFramework;

namespace Chaos.TestFramework.Runtime;

public static class ChaosProofRunner
{
    public static int Run(string[]? args)
    {
        var request = ChaosManagedHostArguments.Parse(args);
        var entry = ChaosManagedHostReflection.ResolveEntry(ChaosManagedHostKind.Proof, request);
        try
        {
            ChaosAssertState.Reset();
            ChaosManagedHostReflection.InvokeStaticEntry(entry);
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
