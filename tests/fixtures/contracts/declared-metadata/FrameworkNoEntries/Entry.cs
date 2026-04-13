using Chaos.TestFramework;

namespace FrameworkNoEntries;

public static class Entry
{
    public static ChaosRuntimeFeature EnabledFeatures()
    {
        return ChaosRuntimeFeature.Reflection | ChaosRuntimeFeature.Delegate;
    }
}
