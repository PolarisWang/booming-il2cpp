using Chaos.TestFramework;
using System.Reflection;

namespace CoreRuntimeFeatures;

internal interface ICountContract
{
    int Count { get; }
}

internal sealed class TypePropertyAndInterfaceTarget : ICountContract
{
    public int Count { get; }

    public TypePropertyAndInterfaceTarget(int count)
    {
        Count = count;
    }
}

internal static class TypePropertyAndInterfaceProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.MetadataContract,
        Alias = "type-property-and-interface-proof",
        CapabilityFamily = ChaosCapabilityFamily.ReflectionAndMetadata,
        Capability = ChaosCapabilityItem.MemberMetadataLookup,
        Requires = ChaosRuntimeFeature.Reflection,
        Archetype = ChaosSolutionArchetype.CoreLibReferenceSolution,
        Priority = 6)]
    public static int Run()
    {
        Type type = typeof(TypePropertyAndInterfaceTarget);
        PropertyInfo property = type.GetProperty(nameof(TypePropertyAndInterfaceTarget.Count))!;
        PropertyInfo[] properties = type.GetProperties();
        Type interfaceType = type.GetInterface(nameof(ICountContract), ignoreCase: true)!;
        InterfaceMapping interfaceMap = type.GetInterfaceMap(typeof(ICountContract));

        Assert.Equal(nameof(TypePropertyAndInterfaceTarget.Count), property.Name);
        Assert.True(properties.Length >= 1);
        Assert.Equal(nameof(ICountContract), interfaceType.Name);
        Assert.Equal(1, interfaceMap.InterfaceMethods.Length);
        Assert.Equal(1, interfaceMap.TargetMethods.Length);
        return 0;
    }
}
