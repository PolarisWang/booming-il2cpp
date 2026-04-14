using Chaos.TestFramework;
using System.Reflection;

namespace CoreRuntimeFeatures;

[AttributeUsage(AttributeTargets.Class | AttributeTargets.Method, AllowMultiple = false)]
internal sealed class ProofMarkerAttribute : Attribute
{
    public ProofMarkerAttribute(byte code)
    {
        Code = code;
    }

    public byte Code { get; }

    public string? Label { get; init; }
}

[ProofMarker(7, Label = "type")]
internal sealed class CustomAttributeLookupTarget
{
    [ProofMarker(9, Label = "method")]
    public void Marked()
    {
    }
}

internal static class CustomAttributeLookupProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.MetadataContract,
        Alias = "custom-attribute-lookup-proof",
        CapabilityFamily = ChaosCapabilityFamily.ReflectionAndMetadata,
        Capability = ChaosCapabilityItem.CustomAttributeLookup,
        Requires = ChaosRuntimeFeature.Reflection,
        Archetype = ChaosSolutionArchetype.CoreLibReferenceSolution,
        Priority = 6)]
    public static int Run()
    {
        ProofMarkerAttribute? typeAttribute = typeof(CustomAttributeLookupTarget).GetCustomAttribute<ProofMarkerAttribute>();
        ProofMarkerAttribute? methodAttribute = typeof(CustomAttributeLookupTarget)
            .GetMethod(nameof(CustomAttributeLookupTarget.Marked), BindingFlags.Instance | BindingFlags.Public)
            ?.GetCustomAttribute<ProofMarkerAttribute>();

        Assert.NotNull(typeAttribute);
        Assert.NotNull(methodAttribute);
        Assert.Equal((byte)7, typeAttribute.Code);
        Assert.Equal("method", methodAttribute.Label);
        return 0;
    }
}
