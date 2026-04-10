using System;

namespace EngineObjectHandleLite;

internal static class Program
{
    private static int Main(string[] args)
    {
        return EngineObjectHandleEntry.Run();
    }
}

internal sealed class EngineObjectHandleSample
{
    public EngineObjectHandleSample(string name)
    {
        Name = name;
    }

    public string Name { get; }
}

internal static class EngineObjectHandleEntry
{
    public static int Run()
    {
        var sample = new EngineObjectHandleSample("handle-payload");
        Console.WriteLine(sample.Name);
        return 0;
    }
}
