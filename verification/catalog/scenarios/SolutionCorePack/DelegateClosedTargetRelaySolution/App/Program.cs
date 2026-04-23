using System;

namespace DelegateClosedTargetRelay.App;

internal delegate string MessageFormatter(string prefix);

internal delegate string TailFormatter(string value);

internal sealed class DelegateBanner
{
    private readonly string _name;

    public DelegateBanner(string name)
    {
        _name = name;
    }

    public string BuildMessage(string prefix)
    {
        return prefix + _name + ".";
    }
}

internal static class DelegateStaticTail
{
    public static string AppendBang(string value)
    {
        return value + "!";
    }
}

internal static class DelegateProofEntry
{
    public static int Run()
    {
        var banner = new DelegateBanner("delegate proof");
        MessageFormatter formatter = banner.BuildMessage;
        TailFormatter tail = DelegateStaticTail.AppendBang;
        var message = tail(formatter("Delegate native proof: "));
        Console.WriteLine(message);
        return 0;
    }
}

internal static class Program
{
    private static int Main()
    {
        return DelegateProofEntry.Run();
    }
}
