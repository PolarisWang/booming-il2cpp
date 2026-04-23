using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

[AttributeUsage(AttributeTargets.Class | AttributeTargets.Method, AllowMultiple = false)]
internal sealed class PreserveLikeAttribute : Attribute
{
}

[PreserveLike]
internal sealed class PreservedContractTarget
{
    public string Build()
    {
        return "preserved";
    }
}

internal static class LinkerPreserveContractProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.MetadataContract,
        Alias = "linker-preserve-contract-proof",
        CapabilityFamily = ChaosCapabilityFamily.LinkerAndAotClosure,
        Capability = ChaosCapabilityItem.LinkerPreserveContract,
        Requires = ChaosRuntimeFeature.Reflection | ChaosRuntimeFeature.MetadataSupplement,
        Archetype = ChaosSolutionArchetype.CoreLibReferenceSolution,
        Priority = 6)]
    public static int Run()
    {
        Type type = typeof(PreservedContractTarget);
        bool hasPreserveMarker = type.IsDefined(typeof(PreserveLikeAttribute), inherit: false);
        string value = new PreservedContractTarget().Build();

        Assert.True(hasPreserveMarker);
        Assert.Equal("preserved", value);
        return 0;
    }
}
