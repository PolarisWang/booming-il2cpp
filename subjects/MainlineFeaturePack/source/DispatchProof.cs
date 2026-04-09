namespace MainlineFeaturePack;

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
    public static int Run()
    {
        DispatchBase instance = new DispatchLeaf("leaf");
        Console.WriteLine(instance.BuildMessage());
        return 0;
    }
}
