using System;

namespace EngineLogWriteLite;

internal static class Program
{
    private static int Main(string[] args)
    {
        return EngineLogWriteEntry.Run();
    }
}

internal static class EngineLogWriteEntry
{
    public static int Run()
    {
        const string message = "engine-log-write-managed-entry";
        Console.WriteLine(message);
        return 0;
    }
}
