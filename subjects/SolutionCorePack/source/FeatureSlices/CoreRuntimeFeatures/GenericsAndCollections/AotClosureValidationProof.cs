using Chaos.TestFramework;
using System.Reflection;

namespace CoreRuntimeFeatures;

internal enum AotClosureSlot : byte
{
    None = 0,
    BoxInt = 1,
    BoxString = 2,
    RoundTripInt = 3,
    RoundTripString = 4,
}

internal readonly record struct AotClosureRecord(
    AotClosureSlot Slot,
    RuntimeTypeHandle TypeHandle,
    RuntimeMethodHandle MethodHandle);

internal sealed class AotClosureBox<T>
{
    public AotClosureBox(T value)
    {
        Value = value;
    }

    public T Value { get; }

    public T Echo(T input)
    {
        return input;
    }
}

internal static class AotClosureFactory
{
    public static T RoundTrip<T>(T value)
    {
        return value;
    }
}

internal static class AotClosureManifest
{
    public static AotClosureRecord[] BuildRequiredClosure()
    {
        var roundTripDefinition = typeof(AotClosureFactory).GetMethod(nameof(AotClosureFactory.RoundTrip), BindingFlags.Public | BindingFlags.Static)!;
        var roundTripInt = roundTripDefinition.MakeGenericMethod(typeof(int));
        var roundTripString = roundTripDefinition.MakeGenericMethod(typeof(string));

        return
        [
            new AotClosureRecord(
                AotClosureSlot.BoxInt,
                typeof(AotClosureBox<int>).TypeHandle,
                typeof(AotClosureBox<int>).GetMethod(nameof(AotClosureBox<int>.Echo), BindingFlags.Instance | BindingFlags.Public)!.MethodHandle),
            new AotClosureRecord(
                AotClosureSlot.BoxString,
                typeof(AotClosureBox<string>).TypeHandle,
                typeof(AotClosureBox<string>).GetMethod(nameof(AotClosureBox<string>.Echo), BindingFlags.Instance | BindingFlags.Public)!.MethodHandle),
            new AotClosureRecord(
                AotClosureSlot.RoundTripInt,
                typeof(AotClosureBox<int>).TypeHandle,
                roundTripInt.MethodHandle),
            new AotClosureRecord(
                AotClosureSlot.RoundTripString,
                typeof(AotClosureBox<string>).TypeHandle,
                roundTripString.MethodHandle),
        ];
    }
}

internal static class AotClosureValidationProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.MetadataContract,
        Alias = "aot-closure-validation-proof",
        CapabilityFamily = ChaosCapabilityFamily.LinkerAndAotClosure,
        Capability = ChaosCapabilityItem.AotClosureValidation,
        Requires = ChaosRuntimeFeature.Reflection | ChaosRuntimeFeature.MetadataSupplement | ChaosRuntimeFeature.GenericSharing,
        Archetype = ChaosSolutionArchetype.MixedReferenceClosureSolution,
        Priority = 6)]
    public static int Run()
    {
        var closure = AotClosureManifest.BuildRequiredClosure();
        var roundTripDefinition = typeof(AotClosureFactory).GetMethod(nameof(AotClosureFactory.RoundTrip), BindingFlags.Public | BindingFlags.Static)!;
        var roundTripInt = roundTripDefinition.MakeGenericMethod(typeof(int));
        var roundTripString = roundTripDefinition.MakeGenericMethod(typeof(string));

        Assert.Equal(4, closure.Length);
        Assert.Equal(typeof(AotClosureBox<int>).TypeHandle, closure[0].TypeHandle);
        Assert.Equal(typeof(AotClosureBox<string>).TypeHandle, closure[1].TypeHandle);
        Assert.Equal(roundTripInt.MethodHandle, closure[2].MethodHandle);
        Assert.Equal(roundTripString.MethodHandle, closure[3].MethodHandle);

        var intBox = new AotClosureBox<int>(42);
        var stringBox = new AotClosureBox<string>("closure");
        Assert.Equal(42, intBox.Echo(42));
        Assert.Equal("closure", stringBox.Echo("closure"));

        Assert.Equal(42, (int)roundTripInt.Invoke(null, [42])!);
        Assert.Equal("closure", (string)roundTripString.Invoke(null, ["closure"])!);
        return 0;
    }
}
