namespace ThreadingProof;

internal static class ThreadingProofEntry
{
    [ThreadStatic]
    private static int ThreadSlot;

    private static readonly object Gate = new();
    private static volatile int SharedTotal;

    public static int Run()
    {
        ThreadSlot += 1;

        lock (Gate)
        {
            SharedTotal += ThreadSlot;
        }

        Console.WriteLine(SharedTotal);
        return 0;
    }
}

internal static class Program
{
    public static int Main()
    {
        return ThreadingProofEntry.Run();
    }
}
