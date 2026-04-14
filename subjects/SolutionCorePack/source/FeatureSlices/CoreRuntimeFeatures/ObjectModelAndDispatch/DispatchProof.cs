using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal abstract class DispatchBase
{
    public abstract string BuildMessage();
}

internal sealed class DispatchLeaf : DispatchBase
{
    private readonly string _name;

    public DispatchLeaf(string name)
    {
        _name = name;
    }

    public override string BuildMessage()
    {
        return "Dispatch native proof: " + _name + ".";
    }
}

internal static class DispatchProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "dispatch-proof",
        CapabilityFamily = ChaosCapabilityFamily.TypeSystemAndGenerics,
        Capability = ChaosCapabilityItem.GenericVirtualDispatch,
        Priority = 2)]
    public static int Run()
    {
        DispatchBase instance = new DispatchLeaf("leaf");
        var message = instance.BuildMessage();
        Assert.Equal("Dispatch native proof: leaf.", message);
        return 0;
    }
}
