using Chaos.TestFramework;

namespace MainlineFeaturePack;

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
        Evidence = ChaosEvidenceKind.Stdout,
        Priority = 3)]
    public static int Run()
    {
        Console.WriteLine(((IMessageBuilder)new InterfaceMessageBuilder("leaf")).BuildMessage());
        return 0;
    }
}
