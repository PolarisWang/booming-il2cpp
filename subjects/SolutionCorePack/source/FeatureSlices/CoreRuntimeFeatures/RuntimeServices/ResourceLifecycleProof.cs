using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal sealed class ResourceLifecycleProbe : IDisposable
{
    private readonly List<string> _events;

    public ResourceLifecycleProbe(List<string> events)
    {
        _events = events;
    }

    public void Dispose()
    {
        _events.Add("dispose");
    }
}

internal static class ResourceLifecycleProofEntry
{
    private static string RunScenario(bool shouldThrow)
    {
        var events = new List<string>();
        try
        {
            using var probe = new ResourceLifecycleProbe(events);
            events.Add("enter");
            if (shouldThrow)
            {
                throw new InvalidOperationException("boom");
            }

            events.Add("exit");
        }
        catch (InvalidOperationException)
        {
            events.Add("catch");
        }

        return string.Join(",", events);
    }

    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "resource-lifecycle-proof",
        CapabilityFamily = ChaosCapabilityFamily.RuntimeServices,
        Capability = ChaosCapabilityItem.ResourceLifecycle,
        Priority = 5)]
    public static int Run()
    {
        Assert.Equal("enter,exit,dispose", RunScenario(shouldThrow: false));
        Assert.Equal("enter,dispose,catch", RunScenario(shouldThrow: true));
        return 0;
    }
}
