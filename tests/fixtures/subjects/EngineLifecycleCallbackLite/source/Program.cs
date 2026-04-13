using System;

namespace EngineLifecycleCallbackLite;

internal static class Program
{
    private static int Main(string[] args)
    {
        return EngineLifecycleCallbackEntry.Run();
    }
}

internal static class EngineLifecycleCallbackEntry
{
    public static int Run()
    {
        Console.WriteLine("engine-lifecycle-managed-entry");
        return 0;
    }
}
