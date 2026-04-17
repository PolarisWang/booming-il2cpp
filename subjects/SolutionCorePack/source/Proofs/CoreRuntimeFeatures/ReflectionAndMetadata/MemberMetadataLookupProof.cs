using Chaos.TestFramework;
using System.Reflection;

namespace CoreRuntimeFeatures;

internal sealed class MemberMetadataLookupTarget
{
    public int Count;

    public MemberMetadataLookupTarget(int count)
    {
        Count = count;
    }

    public string Format(string prefix)
    {
        return prefix + ":" + Count;
    }
}

internal static class MemberMetadataLookupProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.MetadataContract,
        Alias = "member-metadata-lookup-proof",
        CapabilityFamily = ChaosCapabilityFamily.ReflectionAndMetadata,
        Capability = ChaosCapabilityItem.MemberMetadataLookup,
        Requires = ChaosRuntimeFeature.Reflection,
        Archetype = ChaosSolutionArchetype.CoreLibReferenceSolution,
        Priority = 6)]
    public static int Run()
    {
        Type type = typeof(MemberMetadataLookupTarget);
        FieldInfo field = type.GetField(nameof(MemberMetadataLookupTarget.Count))!;
        ConstructorInfo constructor = type.GetConstructors(BindingFlags.Instance | BindingFlags.Public)[0];
        MethodInfo method = type.GetMethod(nameof(MemberMetadataLookupTarget.Format), BindingFlags.Instance | BindingFlags.Public)!;
        ParameterInfo parameter = method.GetParameters()[0];

        Assert.Equal("Count", field.Name);
        Assert.Equal(1, constructor.GetParameters().Length);
        Assert.Equal("prefix", parameter.Name);
        Assert.Equal("Format", method.Name);
        return 0;
    }
}
