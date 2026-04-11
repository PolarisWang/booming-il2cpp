using System;

namespace EngineHostProof;

internal static class Program
{
    private static int Main(string[] args)
    {
        return EngineHostEntry.Run();
    }
}

internal static class EngineHostEntry
{
    public static int Run()
    {
        Console.WriteLine("engine-host-proof-managed-entry");
        return 0;
    }
}
