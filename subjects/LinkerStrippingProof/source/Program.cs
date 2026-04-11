namespace LinkerStrippingProof;

[AttributeUsage(AttributeTargets.Class | AttributeTargets.Method | AttributeTargets.Field | AttributeTargets.Property)]
internal sealed class PreserveAttribute : Attribute
{
}

internal sealed class ReachableHarness
{
    private readonly string _name;

    public ReachableHarness(string name)
    {
        _name = name;
    }

    public string BuildMessage()
    {
        return "Linker stripping proof: " + _name + ".";
    }
}

[Preserve]
internal sealed class PreservedUnusedHarness
{
    public static string Marker()
    {
        return "Linker stripping proof: preserved path.";
    }
}

internal sealed class StrippedUnusedHarness
{
    public static string Marker()
    {
        return "Linker stripping proof: stripped path.";
    }
}

internal static class LinkerStrippingProofEntry
{
    public static int Run()
    {
        Console.WriteLine(new ReachableHarness("reachable path").BuildMessage());
        return 0;
    }
}

internal static class Program
{
    public static int Main()
    {
        return LinkerStrippingProofEntry.Run();
    }
}
