using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal sealed class ActivatorCreateInstanceTarget
{
    public int Value { get; }

    public ActivatorCreateInstanceTarget()
    {
        Value = 42;
    }
}

internal static class ActivatorCreateInstanceProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.MetadataContract,
        Alias = "activator-create-instance-proof",
        CapabilityFamily = ChaosCapabilityFamily.ReflectionAndMetadata,
        Capability = ChaosCapabilityItem.MemberMetadataLookup,
        Requires = ChaosRuntimeFeature.Reflection,
        Archetype = ChaosSolutionArchetype.CoreLibReferenceSolution,
        Priority = 6)]
    public static int Run()
    {
        object? value = Activator.CreateInstance(typeof(ActivatorCreateInstanceTarget));

        Assert.NotNull(value);
        Assert.Equal(typeof(ActivatorCreateInstanceTarget), value.GetType());
        Assert.Equal(42, ((ActivatorCreateInstanceTarget)value).Value);
        return 0;
    }
}
