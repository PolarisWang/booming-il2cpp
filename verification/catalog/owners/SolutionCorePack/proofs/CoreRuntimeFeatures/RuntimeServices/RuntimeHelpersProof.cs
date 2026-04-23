using Chaos.TestFramework;
using System.Runtime.CompilerServices;

namespace CoreRuntimeFeatures;

internal sealed class RuntimeHelpersBox
{
    public int Value;
}

internal static class RuntimeHelpersProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "runtime-helpers-proof",
        CapabilityFamily = ChaosCapabilityFamily.RuntimeServices,
        Capability = ChaosCapabilityItem.RuntimeHelpers,
        Priority = 4)]
    public static int Run()
    {
        var box = new RuntimeHelpersBox { Value = 42 };
        object boxed = box;
        object copy = RuntimeHelpers.GetObjectValue(boxed);

        Assert.Equal(RuntimeHelpers.GetHashCode(box), RuntimeHelpers.GetHashCode(boxed));
        Assert.True(RuntimeHelpers.IsReferenceOrContainsReferences<RuntimeHelpersBox>());
        Assert.True(copy is RuntimeHelpersBox);
        Assert.Equal(42, ((RuntimeHelpersBox)copy).Value);
        return 0;
    }
}
