using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal interface IMessageBuilder
{
    string BuildMessage();
}

internal sealed class InterfaceMessageBuilder : IMessageBuilder
{
    private readonly string _name;

    public InterfaceMessageBuilder(string name)
    {
        _name = name;
    }

    public string BuildMessage()
    {
        return "Interface dispatch native proof: " + _name + ".";
    }
}

internal static class InterfaceDispatchProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "interface-dispatch-proof",
        CapabilityFamily = ChaosCapabilityFamily.TypeSystemAndGenerics,
        Capability = ChaosCapabilityItem.GenericInterfaceDispatch,
        Priority = 3)]
    public static int Run()
    {
        var message = ((IMessageBuilder)new InterfaceMessageBuilder("leaf")).BuildMessage();
        Assert.Equal("Interface dispatch native proof: leaf.", message);
        return 0;
    }
}
