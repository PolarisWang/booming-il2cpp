using Chaos.TestFramework;
using System.Reflection;

namespace CoreRuntimeFeatures;

internal enum MetadataSupplementSlot : byte
{
    None = 0,
    ClosedIntType = 1,
    ClosedStringType = 2,
    AppendMethod = 3,
}

internal readonly record struct MetadataSupplementRecord(
    MetadataSupplementSlot Slot,
    RuntimeTypeHandle SubjectHandle,
    int MetadataToken,
    byte GenericArity,
    byte ParameterCount);

internal sealed class MetadataSupplementBox<T>
{
    public MetadataSupplementBox(T value)
    {
        Value = value;
    }

    public T Value { get; }

    public void AppendTo(List<T> values)
    {
        values.Add(Value);
    }
}

internal static class MetadataSupplementRegistry
{
    public static void Add(List<MetadataSupplementRecord> records, MetadataSupplementSlot slot, Type type)
    {
        records.Add(
            new MetadataSupplementRecord(
                slot,
                type.TypeHandle,
                type.MetadataToken,
                checked((byte)type.GetGenericArguments().Length),
                0));
    }

    public static void Add(List<MetadataSupplementRecord> records, MetadataSupplementSlot slot, MethodInfo method)
    {
        records.Add(
            new MetadataSupplementRecord(
                slot,
                method.DeclaringType!.TypeHandle,
                method.MetadataToken,
                checked((byte)method.DeclaringType.GetGenericArguments().Length),
                checked((byte)method.GetParameters().Length)));
    }
}

internal static class MetadataSupplementProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.MetadataContract,
        Alias = "metadata-supplement-proof",
        CapabilityFamily = ChaosCapabilityFamily.ReflectionAndMetadata,
        Capability = ChaosCapabilityItem.MetadataSupplement,
        Requires = ChaosRuntimeFeature.Reflection | ChaosRuntimeFeature.MetadataSupplement | ChaosRuntimeFeature.GenericSharing,
        Archetype = ChaosSolutionArchetype.MixedReferenceClosureSolution,
        Priority = 6)]
    public static int Run()
    {
        var records = new List<MetadataSupplementRecord>(capacity: 3);
        var closedIntType = typeof(MetadataSupplementBox<int>);
        var closedStringType = typeof(MetadataSupplementBox<string>);
        MethodBase constructor = closedIntType.GetConstructors(BindingFlags.Instance | BindingFlags.Public)[0];
        var appendMethod = closedIntType.GetMethod(nameof(MetadataSupplementBox<int>.AppendTo), BindingFlags.Instance | BindingFlags.Public)!;

        MetadataSupplementRegistry.Add(records, MetadataSupplementSlot.ClosedIntType, closedIntType);
        MetadataSupplementRegistry.Add(records, MetadataSupplementSlot.ClosedStringType, closedStringType);
        MetadataSupplementRegistry.Add(records, MetadataSupplementSlot.AppendMethod, appendMethod);

        Assert.Equal(3, records.Count);

        var intTypeRecord = records[0];
        var stringTypeRecord = records[1];
        var appendMethodRecord = records[2];

        Assert.Equal(closedIntType.TypeHandle, intTypeRecord.SubjectHandle);
        Assert.Equal(closedStringType.TypeHandle, stringTypeRecord.SubjectHandle);
        Assert.Equal(1, intTypeRecord.GenericArity);
        Assert.Equal(1, stringTypeRecord.GenericArity);
        Assert.True(intTypeRecord.MetadataToken > 0);
        Assert.True(stringTypeRecord.MetadataToken > 0);

        Assert.Equal(closedIntType.TypeHandle, appendMethodRecord.SubjectHandle);
        Assert.Equal(1, appendMethodRecord.GenericArity);
        Assert.Equal(1, appendMethodRecord.ParameterCount);
        Assert.True(appendMethodRecord.MetadataToken > 0);

        var constructedBox = (MetadataSupplementBox<int>)constructor.Invoke(obj: null, parameters: [84])!;
        Assert.Equal(84, constructedBox.Value);

        var values = new List<int> { 21 };
        appendMethod.Invoke(new MetadataSupplementBox<int>(21), [values]);
        Assert.Equal(2, values.Count);
        Assert.Equal(21, values[1]);
        return 0;
    }
}
