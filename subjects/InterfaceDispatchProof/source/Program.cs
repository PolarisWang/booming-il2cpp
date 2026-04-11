namespace InterfaceDispatchProof;

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
    public static int Run()
    {
        Console.WriteLine(((IMessageBuilder)new InterfaceMessageBuilder("leaf")).BuildMessage());
        return 0;
    }
}

internal static class Program
{
    public static int Main()
    {
        return InterfaceDispatchProofEntry.Run();
    }
}
