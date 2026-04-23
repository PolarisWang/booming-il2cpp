using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal interface IGenericConstraintFormatter<T>
    where T : struct
{
    int Format(T value);
}

internal readonly struct GenericConstraintValue
{
    public int Value { get; init; }
}

internal sealed class GenericConstraintFormatter : IGenericConstraintFormatter<GenericConstraintValue>
{
    public int Format(GenericConstraintValue value)
    {
        return value.Value;
    }
}

internal static class GenericConstraintProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "generic-constraint-proof",
        CapabilityFamily = ChaosCapabilityFamily.TypeSystemAndGenerics,
        Capability = ChaosCapabilityItem.GenericConstraint,
        Requires = ChaosRuntimeFeature.GenericSharing,
        Priority = 2)]
    public static int Run()
    {
        var formatter = new GenericConstraintFormatter();
        var formatted = FormatValue(
            new GenericConstraintValue { Value = 42 },
            formatter);

        Assert.Equal(42, formatted);
        return 0;
    }

    private static int FormatValue<T, TFormatter>(T value, TFormatter formatter)
        where T : struct
        where TFormatter : IGenericConstraintFormatter<T>
    {
        return formatter.Format(value);
    }
}
