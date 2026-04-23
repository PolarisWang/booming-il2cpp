using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal static class RequiredInstantiationHarness
{
    public static T Tail<T>(IReadOnlyList<T> values)
    {
        return values[values.Count - 1];
    }
}

internal static class RequiredInstantiationClosureProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.MetadataContract,
        Alias = "required-instantiation-closure-proof",
        CapabilityFamily = ChaosCapabilityFamily.LinkerAndAotClosure,
        Capability = ChaosCapabilityItem.RequiredInstantiationClosure,
        Requires = ChaosRuntimeFeature.GenericSharing,
        Archetype = ChaosSolutionArchetype.MixedReferenceClosureSolution,
        Priority = 5)]
    public static int Run()
    {
        int lastNumber = RequiredInstantiationHarness.Tail(new[] { 4, 8, 15, 16, 23, 42 });
        string lastWord = RequiredInstantiationHarness.Tail(new[] { "chaos", "closure" });

        Assert.Equal(42, lastNumber);
        Assert.Equal("closure", lastWord);
        return 0;
    }
}
