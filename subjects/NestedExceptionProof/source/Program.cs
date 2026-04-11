namespace NestedExceptionProof;

internal sealed class NestedExceptionHarness
{
    public NestedExceptionHarness()
    {
    }

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
    public static int Run()
    {
        var harness = new NestedExceptionHarness();
        Console.WriteLine(harness.Capture());
        return 0;
    }
}

internal static class Program
{
    public static int Main()
    {
        return NestedExceptionProofEntry.Run();
    }
}
