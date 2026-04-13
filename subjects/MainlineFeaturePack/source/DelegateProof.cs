using Chaos.TestFramework;

namespace MainlineFeaturePack;

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

internal sealed class DelegateRelay
{
    private readonly MessageFormatter _formatter;

    public DelegateRelay(MessageFormatter formatter)
    {
        _formatter = formatter;
    }

    public string Format(string prefix)
    {
        return _formatter(prefix);
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
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "delegate-proof",
        Requires = ChaosRuntimeFeature.Delegate,
        Evidence = ChaosEvidenceKind.Stdout,
        Priority = 5)]
    public static int Run()
    {
        var banner = new DelegateBanner("delegate proof");
        MessageFormatter formatter = banner.BuildMessage;
        TailFormatter tail = DelegateStaticTail.AppendBang;
        var relay = new DelegateRelay(formatter);
        Console.WriteLine(tail(relay.Format("Delegate native proof: ")));
        return 0;
    }
}
