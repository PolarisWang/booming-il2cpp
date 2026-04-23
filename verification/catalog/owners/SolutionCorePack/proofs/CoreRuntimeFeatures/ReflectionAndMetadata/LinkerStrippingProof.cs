using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

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
    [ChaosUnitTest(
        ChaosUnitCategory.MetadataContract,
        Alias = "linker-stripping-proof",
        CapabilityFamily = ChaosCapabilityFamily.ReflectionAndMetadata,
        Capability = ChaosCapabilityItem.LinkerStripping,
        Priority = 4)]
    public static int Run()
    {
        var reachable = new ReachableHarness("reachable path").BuildMessage();
        Assert.Equal("Linker stripping proof: reachable path.", reachable);
        Assert.Equal("Linker stripping proof: preserved path.", PreservedUnusedHarness.Marker());
        return 0;
    }
}
