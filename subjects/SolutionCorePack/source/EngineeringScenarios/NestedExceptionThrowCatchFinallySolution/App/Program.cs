using System;

namespace NestedExceptionThrowCatchFinally.App;

internal sealed class NestedExceptionHarness
{
    public string Capture()
    {
        try
        {
            try
            {
                ThrowInner();
                return "unreachable-inner";
            }
            catch (InvalidOperationException ex)
            {
                _ = ex;
                return "Nested EH native proof: inner caught.";
            }
            finally
            {
            }
        }
        finally
        {
        }
    }

    private static void ThrowInner()
    {
        throw new InvalidOperationException("nested-phase2-proof");
    }
}

internal static class NestedExceptionProofEntry
{
    public static int Run()
    {
        var harness = new NestedExceptionHarness();
        Console.WriteLine(harness.Capture());
        return 0;
    }
}

internal static class Program
{
    private static int Main()
    {
        return NestedExceptionProofEntry.Run();
    }
}
