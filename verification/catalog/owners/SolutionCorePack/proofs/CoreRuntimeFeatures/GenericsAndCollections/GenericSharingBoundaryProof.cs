using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal interface IGenericValue<out T>
{
    T Value { get; }
}

internal readonly struct GenericValue<T> : IGenericValue<T>
{
    public GenericValue(T value)
    {
        Value = value;
    }

    public T Value { get; }
}

internal static class GenericSharingBoundaryProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.MetadataContract,
        Alias = "generic-sharing-boundary-proof",
        CapabilityFamily = ChaosCapabilityFamily.LinkerAndAotClosure,
        Capability = ChaosCapabilityItem.GenericSharingBoundary,
        Requires = ChaosRuntimeFeature.GenericSharing | ChaosRuntimeFeature.GenericVirtualDispatch,
        Archetype = ChaosSolutionArchetype.MixedReferenceClosureSolution,
        Priority = 5)]
    public static int Run()
    {
        IGenericValue<int> integerValue = new GenericValue<int>(42);
        IGenericValue<string> stringValue = new GenericValue<string>("sharing");

        var resolvedString = stringValue.Value;
        return integerValue.Value == 42 && resolvedString is not null
            ? 0
            : 1;
    }
}
