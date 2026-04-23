using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal sealed class EventCallbackFlowHub
{
    public event Action<int>? Occurred;

    public void Raise(int value)
    {
        Occurred?.Invoke(value);
    }
}

internal static class EventCallbackFlowProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "event-callback-flow-proof",
        CapabilityFamily = ChaosCapabilityFamily.DelegatesAndClosures,
        Capability = ChaosCapabilityItem.EventCallbackFlow,
        Requires = ChaosRuntimeFeature.Delegate,
        Priority = 4)]
    public static int Run()
    {
        var sum = 0;
        var hub = new EventCallbackFlowHub();

        void OnOccurred(int value)
        {
            sum += value;
        }

        hub.Occurred += OnOccurred;
        hub.Raise(19);
        hub.Raise(23);
        hub.Occurred -= OnOccurred;
        hub.Raise(100);

        Assert.Equal(42, sum);
        return 0;
    }
}
