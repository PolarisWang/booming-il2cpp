namespace AsyncAwaitProof;

internal static class AsyncAwaitProofEntry
{
    internal static async Task<int> ComputeAsync()
    {
        return await Task.FromResult(42);
    }

    public static int Run()
    {
        Console.WriteLine(ComputeAsync().GetAwaiter().GetResult());
        return 0;
    }
}

internal static class Program
{
    public static int Main()
    {
        return AsyncAwaitProofEntry.Run();
    }
}
