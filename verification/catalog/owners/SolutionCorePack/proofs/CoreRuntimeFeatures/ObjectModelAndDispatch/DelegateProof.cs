using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

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
        CapabilityFamily = ChaosCapabilityFamily.DelegatesAndClosures,
        Capability = ChaosCapabilityItem.DelegateInvocation,
        Requires = ChaosRuntimeFeature.Delegate,
        Priority = 5)]
    public static int Run()
    {
        var banner = new DelegateBanner("delegate proof");
        MessageFormatter formatter = banner.BuildMessage;
        TailFormatter tail = DelegateStaticTail.AppendBang;
        var relay = new DelegateRelay(formatter);
        var message = tail(relay.Format("Delegate native proof: "));
        Assert.Equal("Delegate native proof: delegate proof.!", message);
        return 0;
    }

    // C4: Exception propagation through delegate invoke
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "delegate-exception-proof",
        CapabilityFamily = ChaosCapabilityFamily.DelegatesAndClosures,
        Capability = ChaosCapabilityItem.DelegateInvocation,
        Requires = ChaosRuntimeFeature.Delegate,
        Priority = 5)]
    public static int RunExceptionPropagation()
    {
        DelegExceptionThrower.ThrowIfZero(1);
        Assert.Throws<System.InvalidOperationException>(() => DelegExceptionThrower.ThrowIfZero(0));
        return 0;
    }

    // E4: Cross-generational card table — delegate holds nursery string
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "delegate-crossgen-proof",
        CapabilityFamily = ChaosCapabilityFamily.DelegatesAndClosures,
        Capability = ChaosCapabilityItem.DelegateInvocation,
        Requires = ChaosRuntimeFeature.Delegate,
        Priority = 5)]
    public static int RunCrossGenReference()
    {
        string? nurseryString = "nursery-" + 42 + "-alive";
        System.Func<string?> getter = () => nurseryString;
        var result1 = getter();
        Assert.Equal("nursery-42-alive", result1);

        // Drop the original reference; only the delegate (closure) holds it
        nurseryString = null;

        // Allocate heavily to trigger young GC — the delegate's captured
        // reference must survive via precise card-table scanning.
        var waste = new System.Collections.Generic.List<byte[]>();
        for (int i = 0; i < 1000; i++)
        {
            waste.Add(new byte[1024]);
        }

        var result2 = getter();
        Assert.NotNull(result2);
        Assert.Equal("nursery-42-alive", result2);
        _ = waste.Count;
        return 0;
    }
}

internal static class DelegExceptionThrower
{
    public static void ThrowIfZero(int value)
    {
        if (value == 0)
            throw new System.InvalidOperationException("Expected non-zero");
    }
}
