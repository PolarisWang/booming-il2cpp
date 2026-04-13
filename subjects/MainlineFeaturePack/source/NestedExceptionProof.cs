using Chaos.TestFramework;

namespace MainlineFeaturePack;

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
                Console.WriteLine("Nested EH inner finally.");
            }
        }
        finally
        {
            Console.WriteLine("Nested EH outer finally.");
        }
    }

    private static void ThrowInner()
    {
        throw new InvalidOperationException("nested-phase2-proof");
    }
}

internal static class NestedExceptionProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "nested-exception-proof",
        Requires = ChaosRuntimeFeature.ExceptionFlow,
        Evidence = ChaosEvidenceKind.Stdout,
        Priority = 2)]
    public static int Run()
    {
        var harness = new NestedExceptionHarness();
        Console.WriteLine(harness.Capture());
        return 0;
    }
}
